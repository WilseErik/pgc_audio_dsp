/* 
 * File:   audio.h
 * Audio engine for sample generation.
 *
 */

#ifndef AUDIO_H
#define	AUDIO_H

#ifdef	__cplusplus
//extern "C" {
#endif


// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

#include "midi.h"
#include "fixed_point.h"

// =============================================================================
// Public type definitions
// =============================================================================

typedef enum audio_ch_nbr_t
{
    AUDIO_CH_SQUARE0       = 0,
    AUDIO_CH_SQUARE1       = 1,
    AUDIO_CH_TRIANGLE0     = 2,
    AUDIO_CH_NOISE0        = 3,
    AUDIO_CH_NBR_OF_CHANNELS
} audio_ch_nbr_t;




// =============================================================================
// Global variable declarations
// =============================================================================

// The number of samples in the sample buffer.
extern uint16_t g_audio_sample_buff_size;

// =============================================================================
// Global constatants
// =============================================================================
#define SAMPLE_BUFF_SIZE        ((uint16_t)128u)
#define SAMPLE_BUFF_HALF_SIZE   ((uint16_t)64u)

// =============================================================================
// Public function declarations
// =============================================================================

/* *********************************************************
 *      Initialization                                     *
 ***********************************************************/

/**
 * @brief Initializes the audio engine.
 * @param void
 * @return void
 */
void audio_init(void);

/* *********************************************************
 *      Sample generation                                  *
 ***********************************************************/
/**
 * @brief Calculates one audio sample.
 * @details The calculated sample is pushed into the sample FIFO buffer.
 * @param void
 * @return void
 */
void audio_calc_sample(void);


/**
 * @brief Applies the configured delta modulation to all channels.
 * @details This function should be called every 480 samples.
 * @param void
 * @return void
 */
void audio_apply_modulation(void);

/* *********************************************************
 *      Sample FIFO buffer                                 *
 ***********************************************************/

/**
 * @brief Pops one sample from the sample buffer.
 * @param void
 * @return The first sample if the FIFO buffer.
 */
int16_t audio_pop_sample(void);

/**
 * @brief Checks if the sample buffer is empty.
 * @param void
 * @return True if the sample buffer is empty, false otherwise.
 */
static inline bool audio_is_sample_buffer_empty(void)
{
    return 0 == g_audio_sample_buff_size;
}

/**
 * @brief Checks if the sample buffer is full.
 * @param void
 * @return True if the sample buffer is full, false otherwise.
 */
static inline bool audio_is_sample_buff_full(void)
{
    return SAMPLE_BUFF_SIZE == g_audio_sample_buff_size;
}

/**
 * @brief Gets the number of calculated samples in the sample buffer.
 * @param void
 * @return The number of samples in the sample buffer.
 */
static inline uint16_t audio_get_sample_buff_size(void)
{
    return g_audio_sample_buff_size;
}

/* *********************************************************
 *      Channel configuration                              *
 ***********************************************************/

/**
 * @brief Turns a note on for one channel.
 * @param channel - The channel to play the note on.
 * @param note_nbr - The note to play.
 * @param amplitude - The amplitude of the note.
 * @return void
 */
void audio_note_on(audio_ch_nbr_t channel,
                   midi_notes_t note_nbr,
                   uint8_t amplitude);

/**
 * @brief Turns the current note off on a specified channel.
 * @param channel - The channel which note to turn off.
 * @return void
 */
void audio_note_off(audio_ch_nbr_t channel);

/**
 * @brief Sets the duty cycle of a square or triangle channel.
 * @param channel - The channel which duty cycle to change.
 * @param duty - The new duty cycle.
 * @return void
 */
void audio_set_duty(audio_ch_nbr_t channel, uint8_t duty);

/**
 * @brief Configures the vibrato settings of one channel.
 * @param channel - The channel which vibrato settings to change.
 * @param speed - The speed of the vibrato. Must be within [0, 127]
 * @param amount - How much the pich should change during the vibrato.
 * @return void
 */
void audio_configure_vibrato(audio_ch_nbr_t channel,
                             uint8_t speed,
                             uint8_t amount);

/**
 * @brief Turns the vibrato off for one channel.
 * @param channel - The channel which vibrator to turn off.
 * @return void
 */
void audio_vibrato_off(audio_ch_nbr_t channel);

/**
 * @brief Turns the vibrato on for one channel.
 * @param channel - The channel which vibrator to turn off.
 * @return void
 */
void audio_vibrato_on(audio_ch_nbr_t channel);

/**
 * @brief Configures the ADSR envelope for the amplitude.
 * @details a, d, s and r must be less than 128
 * @param channel - The channels which adsr envelope to configure.
 * @param a - The attack time [10ms]
 * @param d - The decay time [10ms]
 * @param s - The substain level
 * @param r - The release time [10ms]
 * @return void
 */
void audio_configure_amplitude_adsr(audio_ch_nbr_t channel,
                                    uint8_t a, uint8_t d,
                                    uint8_t s, uint8_t r);

/**
 * @brief Turns the amplitude ADSR modulation on for one channel.
 * @param channel - The channel which ADSR modulation to turn on.
 */
void audio_amplitude_adsr_on(audio_ch_nbr_t channel);

/**
 * @brief Turns the amplitude ADSR modulation off for one channel.
 * @param channel - The channel which ADSR modulation to turn off.
 */
void audio_amplitude_adsr_off(audio_ch_nbr_t channel);

/* *********************************************************
 *      Debug functions                                    *
 ***********************************************************/

/**
 * @brief Prints the channel status on the UART interface.
 * @param channel - The channels which status to print.
 * @return void
 */
void audio_print_channel_status(audio_ch_nbr_t channel);

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_H */

