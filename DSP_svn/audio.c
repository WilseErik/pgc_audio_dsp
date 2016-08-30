/*
 * This file is responisble for the audio sample generation.
 * Samples are calculated in advance and placed in a queue.
 */



// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "audio.h"
#include "dma.h"
#include "fixed_point.h"
#include "uart.h"
#include "utilities.h"
#include "midi.h"
#include "rng.h"

// =============================================================================
// Private type definitions
// =============================================================================

/* ADSR envelope
 *
 * Attack - Decay - Substain - Release
 *
 *          Amplitude
 *              ^
 *              |
 * amplitude - -|- - ->*
 *              |     *  *
 *              |    *     *
 *         s - -| - * - - - -* * * * * * * * * *
 *              |  *                             *
 *              | *                                *
 *              |*                                   *
 *          0 - *--------------------------------------*-------> Time
 *              <------><---->                  <------>
 *                 a      d                         r
 */
typedef enum adsr_state_t
{
    ADSR_STATE_OFF,
    ADSR_STATE_ATTACK,
    ADSR_STATE_DECAY,
    ADSR_STATE_SUBSTAIN,
    ADSR_STATE_RELEASE
} adsr_state_t;

typedef struct adsr_envelope_t
{
    bool            on;
    bool            update_amplitude_event;
    adsr_state_t    state;
    uint8_t         attack;     // a
    uint8_t         decay;      // d
    uint8_t         substain;   // s
    uint8_t         release;    // r
    q16_16_t        amplitude_factor;
    q16_16_t        substain_factor;
    q16_16_t        attack_stepp;
    q16_16_t        decay_stepp;
    q16_16_t        release_stepp;
} adsr_envelope_t;

/*
 * Vibrato
 *
 * (frequency modulation)
 *
 *           Frequency
 *              ^       falling_edge           sampling frequency
 *              |           !                        <->
 *              |            ---                     ---
 *              |  stepp -> |   |                   |   |
 *              |        ---     ---             ---     ---
 *              |-------|-----------|-----------|-----------|--------> Time
 *              |    ---             ---     ---             ---
 *              |   |           stepp-> |   |                   |
 *  low_level-->|---                     ---                     ---
 *              |<---------------------->
 *                       period
 */
typedef struct vibrato_t
{
    bool on;
    uint8_t     depth;
    uint8_t     rate;
    bool        rising;
    q16_16_t    falling_edge;
    q16_16_t    period;
    q16_16_t    time;
    q16_16_t    low_level;
    q16_16_t    stepp;
} vibrato_t;


/*
 * Portamento
 *
 *      Frequency
 *          ^                 duration
 *          |                  <---->
 *          |                  .    /---------------- . . . target_frequency
 *          |                  .   /
 *          |                  .  /
 *          |                  . /
 *          |------------------./
 *          |                  .
 *          |------------------.--------------------------> time
 *                             .
 *                          note on
 */
typedef struct portamento_t
{
    bool        on;
    bool        active;
    q16_16_t    stepp;
    q16_16_t    target_frequency;    
} portamento_t;

/*
 * Arpeggio
 *
 *      |----|)-------------6--------------------------6--------------
 *      |----/-----mmmmmmmmmmmmmmmmmmmmm----mmmmmmmmmmmmmmmmmmmmm-----
 *      |-- /|-----|---|---|---|---|---|----|---|---|---|---|---|-----
 *      |--/(|,\---|---|--@----|---|--@-----|---|--@----|---|--@------
 *      |--\_|_/---|--@--------|--@---------|--@--------|--@----------
 *         (_|   -@-         -@-          -@-         -@-
 */
typedef struct arpeggio_t
{
    bool    on;
    uint8_t speed;

} arpeggio_t;


/* Square wave type
 *
 *          Amplitude
 *              ^    rising_edge
 *              |        !
 * high_level-->|. . . . .------------------          ------------------
 *              |        |                  |        |                  |
 *              |--------|------------------|--------|------------------|-> Time
 *              |        |                  |        |                  |
 * low_level--->|--------                    --------                    -----
 *              |
 *              |<------------------------->
 *                         period
 */
typedef struct square_wave_ch_t
{
    bool            note_on;
    uint8_t         note_nbr;
    uint8_t         duty;
    bool            is_high;
    int16_t         high_level;
    int16_t         low_level;
    int16_t         high_level_limit;
    q16_16_t        rising_edge;
    q16_16_t        period;
    q16_16_t        time;
    vibrato_t       vibrato;
    adsr_envelope_t envelope;
} square_wave_ch_t;

/*
 * Triangle wave type
 *
 *          Amplitude
 *              ^       falling_edge           sampling frequency
 *              |           !                        <->
 *              |            ---                     ---
 *          up_step_size -> |   |                   |   |
 *              |        ---     ---             ---     ---
 *              |-------|-----------|-----------|-----------|---------> Time
 *              |    ---             ---     ---             ---
 *              |   |  down_step_size-> |   |                   |
 *  low_level-->|---                     ---                     ---
 *              |<---------------------->
 *                       period
 */
typedef struct triangle_wave_ch_t
{
    bool            note_on;
    uint8_t         note_nbr;
    uint8_t         amplitude;
    bool            is_rising;
    uint8_t         duty;
    int16_t         current_value;
    int16_t         up_step_size;
    int16_t         down_step_size;
    int16_t         low_level;
    q16_16_t        time;
    q16_16_t        falling_edge;
    q16_16_t        period;
    vibrato_t       vibrato;
} triangle_wave_ch_t;

/* Noise wave type
 *
 *          Amplitude
 *              ^
 *              |
 * high_level-->|. . . . .----   -----        --      ----
 *              |        |    | |     |      |  |    |    |
 *              |--------|----|-|-----|------|--|----|----|--------> Time
 *              |        |    | |     |      |  |    |    |
 * low_level--->|--------      -       ------    ----      --------                    -----
 *              |
 *              |
 */
typedef struct noise_wave_ch_t
{
    bool            note_on;
    uint8_t         note_nbr;
    uint8_t         prescaler;
    uint8_t         counter;
    bool            is_high;
    uint16_t        amplitude;
    uint16_t        current_amplitude;
    int16_t         high_level;
    int16_t         low_level;
    int16_t         high_level_limit;
    q16_16_t        time;
    q16_16_t        period;
    adsr_envelope_t envelope;
} noise_wave_ch_t;


// =============================================================================
// Global variables
// =============================================================================
uint16_t g_audio_sample_buff_size = 0;

// =============================================================================
// Private constants
// =============================================================================
// Since the amplitude in MIDI messages are [0, 255], the MIDI-amplitudes
// are multiplied with these constats to form the square channel amplitudes.
static const int16_t LOW_AMPLITUDE_FACTOR = (-32);
static const int16_t HIGH_AMPLITUDE_FACTOR = (32);

static const float ONE_CENT_CHANGE_FACTOR = 0.000561256873183065;

// =============================================================================
// Private variables
// =============================================================================

/*
 * Channels:
 * - 2 square channels
 * - 1 triangle channel
 * - 1 noise channel
 * 
 */
static square_wave_ch_t     sq0;
static square_wave_ch_t     sq1;
static triangle_wave_ch_t   tri0;
static noise_wave_ch_t      noise0;

static int16_t sample_buff[SAMPLE_BUFF_SIZE];
static uint16_t sample_buff_first = 0;
static uint16_t sample_buff_last = 0;

// Accumulates the samples of each channel
static int16_t accumulator;

static q16_16_t midi_note_periods[MIDI_FREQUENCIES_SIZE];

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Pushes one sample into the sample buffer.
 * @param sample - the sample to append to the buffer.
 * @return void
 */
static inline void buffer_push(int16_t sample);

/**
 * @brief Calculates the next sample of square wave channel 0.
 * @details The calculated sample is added to the accumulator.
 * @return void
 */
static inline void calc_sq0_sample(void);

/**
 * @brief Calculates the next sample of square wave channel 1.
 * @details The calculated sample is added to the accumulator.
 * @return void
 */
static inline void calc_sq1_sample(void);

/**
 * @brief Calculates the next sample of triangle wave channel 0.
 * @details The calculated sample is added to the accumulator.
 * @return void
 */
static inline void calc_tri0_sample(void);

/**
 * @brief Calculates the next sample of noise channel 0.
 * @details The calculated sample is added to the accumulator.
 * @return void
 */
static inline void calc_noise0_sample(void);

/**
 * @brief Updates the square 0 vibrator modulation.
 * @param void
 * @return void
 */
static inline void modulate_sq0_vibrato(void);

/**
 * @brief Updates the square 1 vibrator modulation.
 * @param void
 * @return void
 */
static inline void modulate_sq1_vibrato(void);

/**
 * @brief Updates the square 0 amplitude adsr envelope.
 * @param void
 * @return void
 */
static inline void modulate_sq0_adsr(void);

/**
 * @brief Updates the square 1 amplitude adsr envelope.
 * @param void
 * @return void
 */
static inline void modulate_sq1_adsr(void);

/**
 * @brief Updates the noise 0 amplude adsr envelope.
 * @param void
 * @return void
 */
static inline void modulate_noise0_adsr(void);

// =============================================================================
// Public function definitions
// =============================================================================

void audio_init(void)
{
    uint16_t i;
    double freq;

    rng_init();
    midi_freq_table_init();

    //
    // Initialize midi_note_periods
    //
    for (i = 0; i != MIDI_FREQUENCIES_SIZE; ++i)
    {
        freq = (double)SAMPLE_FREQ_HZ / g_midi_frequencies[i];
        midi_note_periods[i] = double_to_q16_16(freq);
    }

    //
    // Initialize all channels
    //
    memset(&sq0, 0, sizeof(square_wave_ch_t));
    memset(&sq1, 0, sizeof(square_wave_ch_t));
    memset(&tri0, 0, sizeof(triangle_wave_ch_t));
    memset(&noise0, 0, sizeof(noise_wave_ch_t));

    sq0.duty = 64;
    sq1.duty = 127;
    tri0.duty = 32;

    // TODO remove me
#if 0
    sq0.note_on = true;
    sq0.duty = 128;
    sq0.low_level = -6000;
    sq0.high_level = 6000;
    sq0.period = midi_note_periods[MIDI_NOTE_C3];
    sq0.rising_edge = sq0.period / 2;
#endif
#if 0
    sq1.note_on = true;
    sq1.duty = 128;
    sq1.low_level = -6000;
    sq1.high_level = 6000;
    sq1.period = midi_note_periods[MIDI_NOTE_E3];
    sq1.rising_edge = sq1.period / 2;
#endif
#if 0
    tri0.note_on = true;
    tri0.low_level = -16000;
    tri0.period = midi_note_periods[MIDI_NOTE_G3];
    tri0.falling_edge = tri0.period / 2;
    tri0.up_step_size = 200;
    tri0.down_step_size = -200;
#endif
#if 0
    sq0.duty = 128;
    audio_note_on(AUDIO_CH_SQUARE0, MIDI_NOTE_E4, 32);
#endif
#if 0
    sq1.duty = 128;
    audio_note_on(AUDIO_CH_SQUARE1, MIDI_NOTE_G4, 32);
#endif
#if 0
    tri0.duty = 128;
    audio_note_on(AUDIO_CH_TRIANGLE0, MIDI_NOTE_C3, 128);
#endif
#if 1
    audio_configure_amplitude_adsr(AUDIO_CH_SQUARE0,
                                   0,  // a
                                   10,  // d
                                   5,  // s
                                   50); // r
    //audio_amplitude_adsr_on(AUDIO_CH_SQUARE0);

    audio_configure_amplitude_adsr(AUDIO_CH_SQUARE1,
                                   0,  // a
                                   10,  // d
                                   5,  // s
                                   50); // r
    //audio_amplitude_adsr_on(AUDIO_CH_SQUARE1);

    audio_configure_amplitude_adsr(AUDIO_CH_NOISE0,
                                   2,  // a
                                   6,  // d
                                   0,  // s
                                   0); // r
    audio_amplitude_adsr_on(AUDIO_CH_NOISE0);

    audio_configure_vibrato(AUDIO_CH_SQUARE0, 115, 30);
    audio_vibrato_on(AUDIO_CH_SQUARE0);

    audio_configure_vibrato(AUDIO_CH_SQUARE1, 115, 30);
    audio_vibrato_on(AUDIO_CH_SQUARE1);

    audio_note_on(AUDIO_CH_SQUARE0, MIDI_NOTE_E4, 32);
    audio_note_on(AUDIO_CH_SQUARE1, MIDI_NOTE_G4, 32);
    audio_note_on(AUDIO_CH_TRIANGLE0, MIDI_NOTE_C3, 92);
#endif
}

void audio_calc_sample(void)
{
    // Accumulate the samples for each channel
    accumulator = 0;

    calc_sq0_sample();
    calc_sq1_sample();
    calc_tri0_sample();
    calc_noise0_sample();

    buffer_push(accumulator);
}

void audio_apply_modulation(void)
{ 
    if (sq0.vibrato.on)
    {
        modulate_sq0_vibrato();
    }

    if (sq1.vibrato.on)
    {
        modulate_sq1_vibrato();
    }

    if (sq0.envelope.on)
    {
        modulate_sq0_adsr();
    }

    if (sq1.envelope.on)
    {
        modulate_sq1_adsr();
    }

    if (noise0.envelope.on)
    {
        modulate_noise0_adsr();
    }
}

int16_t audio_pop_sample(void)
{
    int16_t return_val = sample_buff[sample_buff_first];

    if (++sample_buff_first == SAMPLE_BUFF_SIZE)
    {
        sample_buff_first = 0;
    }

    --g_audio_sample_buff_size;

    return return_val;
}

void audio_note_on(audio_ch_nbr_t channel, midi_notes_t note_nbr, uint8_t velocity)
{
    q16_16_t tmp;

    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.note_nbr = note_nbr;
        sq0.is_high = false;
        sq0.high_level_limit = HIGH_AMPLITUDE_FACTOR * velocity;
        sq0.time = 0;
        sq0.period = midi_note_periods[note_nbr];
        tmp = sq0.period / UINT8_MAX;
        sq0.rising_edge = tmp * sq0.duty;

        if (sq0.vibrato.on)
        {
            audio_configure_vibrato(channel,
                                    sq0.vibrato.rate,
                                    sq0.vibrato.depth);
        }

        if (sq0.envelope.on)
        {
            sq0.low_level = 0;
            sq0.high_level = 0;
            sq0.envelope.amplitude_factor = 0;
            sq0.envelope.state = ADSR_STATE_ATTACK;
            sq0.envelope.update_amplitude_event = true;
        }
        else
        {
            sq0.high_level = sq0.high_level_limit;
            sq0.low_level = (-1) * sq0.high_level;
        }

        sq0.note_on = true;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.note_nbr = note_nbr;
        sq1.is_high = false;
        sq1.high_level_limit = HIGH_AMPLITUDE_FACTOR * velocity;
        sq1.time = 0;
        sq1.period = midi_note_periods[note_nbr];
        tmp = sq1.period / UINT8_MAX;
        sq1.rising_edge = tmp * sq1.duty;

        if (sq1.vibrato.on)
        {
            audio_configure_vibrato(channel,
                                    sq1.vibrato.rate,
                                    sq1.vibrato.depth);
        }

        if (sq1.envelope.on)
        {
            sq1.low_level = 0;
            sq1.high_level = 0;
            sq1.envelope.amplitude_factor = 0;
            sq1.envelope.state = ADSR_STATE_ATTACK;
            sq1.envelope.update_amplitude_event = true;
        }
        else
        {
            sq1.high_level = sq1.high_level_limit;
            sq1.low_level = (-1) * sq1.high_level;
        }

        sq1.note_on = true;
        break;

    case AUDIO_CH_TRIANGLE0:
        tri0.note_nbr = note_nbr;
        tri0.amplitude = velocity;
        tri0.is_rising = true;
        tri0.low_level = (LOW_AMPLITUDE_FACTOR / 2) * velocity;
        tri0.time = 0;
        tri0.period = midi_note_periods[note_nbr];
        tmp = tri0.period / UINT8_MAX;
        tri0.falling_edge = tmp * tri0.duty;
        tri0.up_step_size = (int16_t)q16_16_to_int(
            q16_16_divide(
                int_to_q16_16(velocity * HIGH_AMPLITUDE_FACTOR),
                tri0.falling_edge));
        tri0.down_step_size = (-1) * (int16_t)q16_16_to_int(
            q16_16_divide(
                int_to_q16_16(velocity * HIGH_AMPLITUDE_FACTOR),
                tri0.period - tri0.falling_edge));
        tri0.current_value = tri0.low_level;
        tri0.note_on = true;
        break;

    case AUDIO_CH_NOISE0:
        noise0.note_nbr = note_nbr;
        noise0.prescaler = 128 - note_nbr;
        noise0.counter = noise0.prescaler;
        noise0.high_level_limit = velocity * HIGH_AMPLITUDE_FACTOR;
        noise0.time = 0;
        noise0.period = midi_note_periods[note_nbr];

        if (noise0.envelope.on)
        {
            noise0.current_amplitude = 0;
            noise0.envelope.amplitude_factor = 0;
            noise0.high_level = 0;
            noise0.low_level = 0;
            noise0.envelope.state = ADSR_STATE_ATTACK;
            noise0.envelope.update_amplitude_event = true;
        }
        else
        {
            noise0.high_level = noise0.high_level_limit;
            noise0.low_level = 0 - noise0.high_level;
        }

        noise0.note_on = true;
        break;

    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Channel %d does not exist. (Note on: ch: %d, note: %d, vel: %d)%s",
                    WARNING_TAG, channel, channel, note_nbr, velocity, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_note_off(audio_ch_nbr_t channel)
{
    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.note_on = false;
        
        if (sq0.envelope.on)
        {
            sq0.envelope.state = ADSR_STATE_RELEASE;
        }
        else
        {
            sq0.low_level = 0;
            sq0.high_level = 0;
        }
        
        break;

    case AUDIO_CH_SQUARE1:
        sq1.note_on = false;

        if (sq1.envelope.on)
        {
            sq1.envelope.state = ADSR_STATE_RELEASE;
        }
        else
        {
            sq1.low_level = 0;
            sq1.high_level = 0;
        }
        break;

    case AUDIO_CH_TRIANGLE0:
        tri0.note_on = false;
        tri0.low_level = 0;
        tri0.up_step_size = 0;
        tri0.down_step_size = 0;
        break;

    case AUDIO_CH_NOISE0:
        noise0.note_on = false;

        if (noise0.envelope.on)
        {
            noise0.envelope.state = ADSR_STATE_RELEASE;
        }
        else
        {
            noise0.low_level = 0;
            noise0.high_level = 0;
        }
        break;

    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Channel %d does not exist. Note off: ch: %d%s",
                    WARNING_TAG, channel, channel, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_set_duty(audio_ch_nbr_t channel, uint8_t duty)
{
    q16_16_t tmp;

    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.duty = duty;
        tmp = sq0.period / UINT8_MAX;
        sq0.rising_edge = tmp * sq0.duty;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.duty = duty;
        tmp = sq1.period / UINT8_MAX;
        sq1.rising_edge = tmp * sq1.duty;
        break;

    case AUDIO_CH_TRIANGLE0:
        tri0.duty = duty;
        tmp = tri0.period / UINT8_MAX;
        tri0.falling_edge = tmp * tri0.duty;
        tri0.up_step_size = (int16_t)q16_16_to_int(
            q16_16_divide(
                int_to_q16_16(tri0.amplitude * HIGH_AMPLITUDE_FACTOR),
                tri0.falling_edge));
        tri0.down_step_size = (-1) * (int16_t)q16_16_to_int(
            q16_16_divide(
                int_to_q16_16(tri0.amplitude * HIGH_AMPLITUDE_FACTOR),
                tri0.period - tri0.falling_edge));
        break;

    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Channel %d does not exist. (Set duty: ch: %d) %s",
                    WARNING_TAG, channel, channel, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_configure_vibrato(audio_ch_nbr_t channel,
                             uint8_t speed,
                             uint8_t amount)
{
    uint8_t nbr_of_stepps;

    if (speed > 127)
    {
        speed = 127;
    }

    nbr_of_stepps = 128 - speed; // whitin the range [1, 128]

    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.vibrato.rate = speed;
        sq0.vibrato.depth = amount;
        sq0.vibrato.falling_edge = Q16_16_T_ONE * nbr_of_stepps;
        sq0.vibrato.period = 2 * sq0.vibrato.falling_edge;
        sq0.vibrato.stepp = q16_16_multiply(
            midi_note_periods[sq0.note_nbr],
            double_to_q16_16(amount * ONE_CENT_CHANGE_FACTOR)) /
            nbr_of_stepps;
        sq0.vibrato.low_level = midi_note_periods[sq0.note_nbr] -
            sq0.vibrato.stepp * nbr_of_stepps;
        sq0.vibrato.rising = true;
        sq0.vibrato.time = 0;
        sq0.vibrato.on = true;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.vibrato.rate = speed;
        sq1.vibrato.depth = amount;
        sq1.vibrato.falling_edge = Q16_16_T_ONE * nbr_of_stepps;
        sq1.vibrato.period = 2 * sq1.vibrato.falling_edge;
        sq1.vibrato.stepp = q16_16_multiply(
            midi_note_periods[sq1.note_nbr],
            double_to_q16_16(amount * ONE_CENT_CHANGE_FACTOR)) /
            nbr_of_stepps;
        sq1.vibrato.low_level = midi_note_periods[sq1.note_nbr] -
            sq1.vibrato.stepp * nbr_of_stepps;
        sq1.vibrato.rising = true;
        sq1.vibrato.time = 0;
        sq1.vibrato.on = true;
        break;
        
    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Vibrato not supported on channel %d. (speed: %u: amount: %u) %s",
                    WARNING_TAG, channel, speed, amount, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_vibrato_off(audio_ch_nbr_t channel)
{
    q16_16_t tmp;

    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.vibrato.on = false;
        sq0.period = midi_note_periods[sq0.note_nbr];
        tmp = sq0.period / UINT8_MAX;
        sq0.rising_edge = tmp * sq0.duty;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.vibrato.on = false;
        sq1.period = midi_note_periods[sq1.note_nbr];
        tmp = sq1.period / UINT8_MAX;
        sq1.rising_edge = tmp * sq1.duty;
        break;

    case AUDIO_CH_TRIANGLE0:
        tri0.vibrato.on = false;
        break;

    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Vibrato not supported on channel %d. (vibrato off) %s",
                    WARNING_TAG, channel, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_vibrato_on(audio_ch_nbr_t channel)
{
    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.vibrato.on = true;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.vibrato.on = true;
        break;

    case AUDIO_CH_TRIANGLE0:
        tri0.vibrato.on = true;
        break;

    default:
#ifdef DEBUG
    sprintf(g_utilities_char_buffer,
            "%s Vibrato not supported on channel %d. (vibrato on) %s",
                    WARNING_TAG, channel, NEWLINE);
    uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

void audio_configure_amplitude_adsr(audio_ch_nbr_t channel,
                                    uint8_t a, uint8_t d,
                                    uint8_t s, uint8_t r)
{
    adsr_envelope_t* env = NULL;

    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        env = &sq0.envelope;
        break;

    case AUDIO_CH_SQUARE1:
        env = &sq1.envelope;
        break;

    case AUDIO_CH_NOISE0:
        env = &noise0.envelope;
        break;

    default:
        sprintf(g_utilities_char_buffer,
        "%s Cannot configure adsr envelope on ch %d (adsr not supported)%s",
                WARNING_TAG, channel, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
        break;
    }

    if (NULL != env)
    {
        env->attack = a;
        env->decay = d;
        env->substain = s;
        env->release = r;

        if (0 != a)
        {
            env->attack_stepp = Q16_16_T_ONE / a;
        }
        else
        {
            env->attack_stepp = Q16_16_T_ONE;
        }

        if (0 != d)
        {
            env->decay_stepp = Q16_16_T_ONE / d;
        }
        else
        {
            env->decay_stepp = Q16_16_T_ONE;
        }

        env->substain_factor = Q16_16_T_ONE / 127;
        env->substain_factor *= s;

        if (0 != r)
        {
            env->release_stepp = Q16_16_T_ONE / r;
        }
        else
        {
            env->release_stepp = 2 * Q16_16_T_ONE;
        }
    }
}

void audio_amplitude_adsr_on(audio_ch_nbr_t channel)
{
    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.envelope.on = true;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.envelope.on = true;
        break;

    case AUDIO_CH_NOISE0:
        noise0.envelope.on = true;
        break;

    default:
        sprintf(g_utilities_char_buffer,
        "%s Cannot activate adsr envelope on ch %d (adsr not supported)%s",
                WARNING_TAG, channel, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
        break;

    }
}

void audio_amplitude_adsr_off(audio_ch_nbr_t channel)
{
    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sq0.envelope.on = false;
        break;

    case AUDIO_CH_SQUARE1:
        sq1.envelope.on = false;
        break;

    case AUDIO_CH_NOISE0:
        noise0.envelope.on = false;
        break;

    default:
        sprintf(g_utilities_char_buffer,
        "%s Cannot deactivate adsr envelope on ch %d (adsr not supported)%s",
                WARNING_TAG, channel, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
        break;

    }
}

void audio_print_channel_status(audio_ch_nbr_t channel)
{
    switch (channel)
    {
    case AUDIO_CH_SQUARE0:
        sprintf(g_utilities_char_buffer,
            "\tnote on: %d\t\t\tnote number: %u%s",
                    sq0.note_on, sq0.note_nbr, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\tduty: %u\t\t\tis high: %d%s",
                    sq0.duty, sq0.is_high, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\thigh level: %d\t\tlow level: %d%s",
                    sq0.high_level, sq0.low_level, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\trising edge: %f\t\tperiod: %f%s", q16_16_to_double(sq0.rising_edge),
            q16_16_to_double(sq0.period), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\ttime: %f%s",
                    q16_16_to_double(sq0.time), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        //
        // Vibrato
        //
        sprintf(g_utilities_char_buffer,
            "\t[Vibrato]%s", NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ton: %d\t\t\tdepth: %u%s",
            sq0.vibrato.on, sq0.vibrato.depth, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\trate: %u\t\t\trising: %d%s",
            sq0.vibrato.rate, sq0.vibrato.rising, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tfalling edge: %f\tperiod: %f%s",
            q16_16_to_double(sq0.vibrato.falling_edge),
            q16_16_to_double(sq0.vibrato.period),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ttime: %f\t\tlow level: %f%s",
            q16_16_to_double(sq0.vibrato.time),
            q16_16_to_double(sq0.vibrato.low_level), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tstepp: %f%s",
            q16_16_to_double(sq0.vibrato.stepp), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        //
        // ADSR modulation
        //
        sprintf(g_utilities_char_buffer,
            "\t[ADSR]%s", NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ton: %u\t\t\tstate: %u%s",
            sq0.envelope.on, sq0.envelope.state, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
        
        sprintf(g_utilities_char_buffer,
            "\t\ta: %u\t\t\td: %u%s",
            sq0.envelope.attack, sq0.envelope.decay, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ts: %u\t\t\tr: %u%s",
            sq0.envelope.substain, sq0.envelope.release, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tattack stepp: %f\t\tdecay stepp: %f%s",
            q16_16_to_double(sq0.envelope.attack_stepp),
            q16_16_to_double(sq0.envelope.decay_stepp),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tsubstain factor: %f\trelease stepp: %f%s",
            q16_16_to_double(sq0.envelope.substain_factor),
            q16_16_to_double(sq0.envelope.release_stepp),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tamplitude factor: %f%s",
            q16_16_to_double(sq0.envelope.amplitude_factor),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        break;

    case AUDIO_CH_SQUARE1:
        sprintf(g_utilities_char_buffer,
            "\tnote on: %d\t\t\tnote number: %u%s",
                    sq1.note_on, sq1.note_nbr, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\tduty: %u\t\t\tis high: %d%s",
                    sq1.duty, sq1.is_high, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\thigh level: %d\t\tlow level: %d%s",
                    sq1.high_level, sq1.low_level, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\trising edge: %f\t\tperiod: %f%s", q16_16_to_double(sq1.rising_edge),
            q16_16_to_double(sq1.period), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\ttime: %f%s",
                    q16_16_to_double(sq1.time), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        //
        // Vibrato
        //
        sprintf(g_utilities_char_buffer,
            "\t[Vibrato]%s", NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ton: %d\t\t\tdepth: %u%s",
            sq1.vibrato.on, sq1.vibrato.depth, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\trate: %u\t\t\trising: %d%s",
            sq1.vibrato.rate, sq1.vibrato.rising, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tfalling edge: %f\tperiod: %f%s",
            q16_16_to_double(sq1.vibrato.falling_edge),
            q16_16_to_double(sq1.vibrato.period),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ttime: %f\t\tlow level: %f%s",
            q16_16_to_double(sq1.vibrato.time),
            q16_16_to_double(sq1.vibrato.low_level), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tstepp: %f%s",
            q16_16_to_double(sq1.vibrato.stepp), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        //
        // ADSR modulation
        //
        sprintf(g_utilities_char_buffer,
            "\t[ADSR]%s", NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ton: %u\t\t\tstate: %u%s",
            sq1.envelope.on, sq1.envelope.state, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ta: %u\t\t\td: %u%s",
            sq1.envelope.attack, sq1.envelope.decay, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\ts: %u\t\t\tr: %u%s",
            sq1.envelope.substain, sq1.envelope.release, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tattack stepp: %f\t\tdecay stepp: %f%s",
            q16_16_to_double(sq1.envelope.attack_stepp),
            q16_16_to_double(sq1.envelope.decay_stepp),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tsubstain factor: %f\trelease stepp: %f%s",
            q16_16_to_double(sq1.envelope.substain_factor),
            q16_16_to_double(sq1.envelope.release_stepp),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\t\tamplitude factor: %f%s",
            q16_16_to_double(sq1.envelope.amplitude_factor),
            NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        break;

    case AUDIO_CH_TRIANGLE0:
        sprintf(g_utilities_char_buffer,
            "\tnote on: %d\t\t\tnote number: %u%s",
                    tri0.note_on, tri0.note_nbr, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\tduty: %u\t\t\tis rising: %d%s",
                    tri0.duty, tri0.is_rising, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\tcurret value: %d\t\tlow level: %d%s",
                    tri0.current_value, tri0.low_level, NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\tfalling edge: %f\tperiod: %f%s", q16_16_to_double(tri0.falling_edge),
            q16_16_to_double(tri0.period), NEWLINE);
        uart_write_string(g_utilities_char_buffer);

        sprintf(g_utilities_char_buffer,
            "\ttime: %f%s",
                    q16_16_to_double(tri0.time), NEWLINE);
        uart_write_string(g_utilities_char_buffer);
        
        break;

    case AUDIO_CH_NOISE0:
        break;

    default:
        break;
    }
}

// =============================================================================
// Private function definitions
// =============================================================================

static inline void buffer_push(int16_t sample)
{
    if (g_audio_sample_buff_size++)
    {
        ++sample_buff_last;

        if (sample_buff_last == SAMPLE_BUFF_SIZE)
        {
            sample_buff_last = 0;
        }
    }

    sample_buff[sample_buff_last] = sample;
}

/* *********************************************************
 *      Sample generation                                  *
 ***********************************************************/

static inline void calc_sq0_sample(void)
{
    sq0.time += Q16_16_T_ONE;

    if (false == sq0.is_high)
    {
        //
        // Low state
        //
        accumulator += sq0.low_level;

        if (sq0.time >= sq0.rising_edge)
        {
            sq0.is_high = true;
        }
    }
    else
    {
        //
        // High state
        //
        accumulator += sq0.high_level;

        if (sq0.time >= sq0.period)
        {
            sq0.time -= sq0.period;

            sq0.is_high = false;

            if (sq0.envelope.update_amplitude_event)
            {
                sq0.envelope.update_amplitude_event = false;

                sq0.high_level = q16_16_to_int(q16_16_multiply(
                        int_to_q16_16(sq0.high_level_limit),
                        sq0.envelope.amplitude_factor));
                sq0.low_level = 0 - sq0.high_level;
            }
        }
    }
}

static inline void calc_sq1_sample(void)
{
    sq1.time += Q16_16_T_ONE;

    if (false == sq1.is_high)
    {
        //
        // Low state
        //
        accumulator += sq1.low_level;

        if (sq1.time >= sq1.rising_edge)
        {
            sq1.is_high = true;
        }
    }
    else
    {
        //
        // High state
        //
        accumulator += sq1.high_level;

        if (sq1.time >= sq1.period)
        {
            sq1.time -= sq1.period;

            sq1.is_high = false;

            if (sq1.envelope.update_amplitude_event)
            {
                sq1.envelope.update_amplitude_event = false;

                sq1.high_level = q16_16_to_int(q16_16_multiply(
                        int_to_q16_16(sq1.high_level_limit),
                        sq1.envelope.amplitude_factor));
                sq1.low_level = 0 - sq1.high_level;
            }
        }

    }
}


static inline void calc_tri0_sample(void)
{
    tri0.time += Q16_16_T_ONE;

    if (tri0.is_rising)
    {
        //
        // Rising
        //
        tri0.current_value += tri0.up_step_size;

        if (tri0.time >= tri0.falling_edge)
        {
           tri0.is_rising = false;
        }
    }
    else
    {
        //
        // Falling
        //
        tri0.current_value += tri0.down_step_size;

        if (tri0.time >= tri0.period)
        {
           tri0.is_rising = true;
           tri0.current_value = tri0.low_level;
           tri0.time -= tri0.period;
        }
    }

    accumulator += tri0.current_value;
}

static inline void calc_noise0_sample(void)
{
#if 0
    noise0.time += Q16_16_T_ONE;

    if (noise0.time >= noise0.period)
    {
        noise0.time -= noise0.period;

        noise0.is_high = rng_get_random();

        if (noise0.envelope.update_amplitude_event)
        {
            noise0.envelope.update_amplitude_event = false;
            
            noise0.high_level = q16_16_to_int(q16_16_multiply(
                        int_to_q16_16(noise0.high_level_limit),
                        noise0.envelope.amplitude_factor));
            noise0.low_level = 0 - noise0.high_level;
        }
    }

    if (noise0.is_high)
    {
        accumulator += noise0.high_level;
    }
    else
    {
        accumulator += noise0.low_level;
    }
#else
    if (0 == noise0.counter--)
    {
        noise0.counter = noise0.prescaler;

        noise0.is_high = rng_get_random();

        if (noise0.envelope.update_amplitude_event)
        {
            noise0.envelope.update_amplitude_event = false;

            noise0.high_level = q16_16_to_int(q16_16_multiply(
                        int_to_q16_16(noise0.high_level_limit),
                        noise0.envelope.amplitude_factor));
            noise0.low_level = 0 - noise0.high_level;
        }
    }
    if (noise0.is_high)
    {
        accumulator += noise0.high_level;
    }
    else
    {
        accumulator += noise0.low_level;
    }

#endif
}

/* *********************************************************
 *      Vibrato modulation                                 *
 ***********************************************************/

static inline void modulate_sq0_vibrato(void)
{
    q16_16_t tmp;

    sq0.vibrato.time += Q16_16_T_ONE;

    if (sq0.vibrato.rising)
    {
        sq0.period += sq0.vibrato.stepp;
        tmp = sq0.period / UINT8_MAX;
        sq0.rising_edge = tmp * sq0.duty;

        if (sq0.vibrato.time >= sq0.vibrato.falling_edge)
        {
            sq0.vibrato.rising = false;
        }
    }
    else
    {
        sq0.period -= sq0.vibrato.stepp;
        tmp = sq0.period / UINT8_MAX;
        sq0.rising_edge = tmp * sq0.duty;

        if (sq0.vibrato.time >= sq0.vibrato.period)
        {
            sq0.vibrato.time -= sq0.vibrato.period;
            sq0.vibrato.rising = true;

            sq0.period = sq0.vibrato.low_level;
            tmp = sq0.period / UINT8_MAX;
            sq0.rising_edge = tmp * sq0.duty;
        }
    }
}

static inline void modulate_sq1_vibrato(void)
{
    q16_16_t tmp;

    sq1.vibrato.time += Q16_16_T_ONE;

    if (sq1.vibrato.rising)
    {
        sq1.period += sq1.vibrato.stepp;
        tmp = sq1.period / UINT8_MAX;
        sq1.rising_edge = tmp * sq1.duty;

        if (sq1.vibrato.time >= sq1.vibrato.falling_edge)
        {
            sq1.vibrato.rising = false;
        }
    }
    else
    {
        sq1.period -= sq1.vibrato.stepp;
        tmp = sq1.period / UINT8_MAX;
        sq1.rising_edge = tmp * sq1.duty;

        if (sq1.vibrato.time >= sq1.vibrato.period)
        {
            sq1.vibrato.time -= sq1.vibrato.period;
            sq1.vibrato.rising = true;

            sq1.period = sq1.vibrato.low_level;
            tmp = sq1.period / UINT8_MAX;
            sq1.rising_edge = tmp * sq1.duty;
        }
    }
}

/* *********************************************************
 *      ADSR volume modulation                             *
 ***********************************************************/

static inline void modulate_sq0_adsr(void)
{
    switch (sq0.envelope.state)
    {
    case ADSR_STATE_OFF:
        break;

    case ADSR_STATE_ATTACK:
        sq0.envelope.amplitude_factor += sq0.envelope.attack_stepp;

        if (sq0.envelope.amplitude_factor >= Q16_16_T_ONE)
        {
            sq0.envelope.amplitude_factor = Q16_16_T_ONE;
            sq0.envelope.state = ADSR_STATE_DECAY;
        }

        sq0.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_DECAY:
        if (sq0.envelope.amplitude_factor >= sq0.envelope.decay_stepp)
        {
            sq0.envelope.amplitude_factor -= sq0.envelope.decay_stepp;

            if (sq0.envelope.amplitude_factor <= sq0.envelope.substain_factor)
            {
                sq0.envelope.amplitude_factor = sq0.envelope.substain_factor;
                sq0.envelope.state = ADSR_STATE_SUBSTAIN;
            }
        }
        else
        {
            sq0.envelope.amplitude_factor = sq0.envelope.substain_factor;
            sq0.envelope.state = ADSR_STATE_SUBSTAIN;
        }

        sq0.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_SUBSTAIN:
        break;

    case ADSR_STATE_RELEASE:
        if (sq0.envelope.amplitude_factor <= sq0.envelope.release_stepp)
        {
            sq0.envelope.amplitude_factor = 0;
            sq0.envelope.state = ADSR_STATE_OFF;
        }
        else
        {
            sq0.envelope.amplitude_factor -= sq0.envelope.release_stepp;
        }

        sq0.envelope.update_amplitude_event = true;
        break;

    default:
#ifdef DEBUG
        sprintf(g_utilities_char_buffer, "%s Invalid adsr envelope state sq0%s",
                WARNING_TAG, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

static inline void modulate_sq1_adsr(void)
{
    switch (sq1.envelope.state)
    {
    case ADSR_STATE_OFF:
        break;

    case ADSR_STATE_ATTACK:
        sq1.envelope.amplitude_factor += sq1.envelope.attack_stepp;

        if (sq1.envelope.amplitude_factor >= Q16_16_T_ONE)
        {
            sq1.envelope.amplitude_factor = Q16_16_T_ONE;
            sq1.envelope.state = ADSR_STATE_DECAY;
        }

        sq1.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_DECAY:
        if (sq1.envelope.amplitude_factor >= sq1.envelope.decay_stepp)
        {
            sq1.envelope.amplitude_factor -= sq1.envelope.decay_stepp;

            if (sq1.envelope.amplitude_factor <= sq1.envelope.substain_factor)
            {
                sq1.envelope.amplitude_factor = sq1.envelope.substain_factor;
                sq1.envelope.state = ADSR_STATE_SUBSTAIN;
            }
        }
        else
        {
            sq1.envelope.amplitude_factor = sq1.envelope.substain_factor;
            sq1.envelope.state = ADSR_STATE_SUBSTAIN;
        }

        sq1.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_SUBSTAIN:
        break;

    case ADSR_STATE_RELEASE:
        if (sq1.envelope.amplitude_factor <= sq1.envelope.release_stepp)
        {
            sq1.envelope.amplitude_factor = 0;
            sq1.envelope.state = ADSR_STATE_OFF;
        }
        else
        {
            sq1.envelope.amplitude_factor -= sq1.envelope.release_stepp;
        }

        sq1.envelope.update_amplitude_event = true;
        break;

    default:
#ifdef DEBUG
        sprintf(g_utilities_char_buffer, "%s Invalid adsr envelope state sq1%s",
                WARNING_TAG, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}

static inline void modulate_noise0_adsr(void)
{
    switch (noise0.envelope.state)
    {
    case ADSR_STATE_OFF:
        break;

    case ADSR_STATE_ATTACK:
        noise0.envelope.amplitude_factor += noise0.envelope.attack_stepp;

        if (noise0.envelope.amplitude_factor >= Q16_16_T_ONE)
        {
            noise0.envelope.amplitude_factor = Q16_16_T_ONE;
            noise0.envelope.state = ADSR_STATE_DECAY;
        }

        noise0.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_DECAY:
        if (noise0.envelope.amplitude_factor >= noise0.envelope.decay_stepp)
        {
            noise0.envelope.amplitude_factor -= noise0.envelope.decay_stepp;

            if (noise0.envelope.amplitude_factor <= noise0.envelope.substain_factor)
            {
                noise0.envelope.amplitude_factor = noise0.envelope.substain_factor;
                noise0.envelope.state = ADSR_STATE_SUBSTAIN;
            }
        }
        else
        {
            noise0.envelope.amplitude_factor = noise0.envelope.substain_factor;
            noise0.envelope.state = ADSR_STATE_SUBSTAIN;
        }

        noise0.envelope.update_amplitude_event = true;
        break;

    case ADSR_STATE_SUBSTAIN:
        break;

    case ADSR_STATE_RELEASE:
        if (noise0.envelope.amplitude_factor <= noise0.envelope.release_stepp)
        {
            noise0.envelope.amplitude_factor = 0;
            noise0.envelope.state = ADSR_STATE_OFF;
        }
        else
        {
            noise0.envelope.amplitude_factor -= noise0.envelope.release_stepp;
        }

        noise0.envelope.update_amplitude_event = true;
        break;

    default:
#ifdef DEBUG
        sprintf(g_utilities_char_buffer, "%s Invalid adsr envelope state n%s",
                WARNING_TAG, NEWLINE);
        uart_write_string(g_utilities_char_buffer);
#endif
        break;
    }
}