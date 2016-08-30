/*
 * This file forms a debug terminal.
 */

// =============================================================================
// Include statements
// =============================================================================
#include <xc.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "terminal.h"
#include "terminal_help.h"
#include "utilities.h"
#include "uart.h"
#include "pcm1774.h"
#include "audio.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================

// =============================================================================
// Private constants
// =============================================================================
static const char TERMINAL_PASSWORD[] = "open terminal";
#define PASSWORD_STR_LEN (13u)

#define CMD_BUFFER_SIZE (128)
static const char COMMAND_TERMINATION_CHARACTER = '\n';

static const char TERMINAL_OPEN[]   = "\tTerminal is now open.\r\n";
static const char COMMAND_ENTER[]   = "Command: ";
static const char SYNTAX_ERROR[]    = "\t[Syntax error]\r\n";

static const char SET[]             = "set ";
static const char GET[]             = "get ";

//
// Commands
//

/*§
 Forces a software reset.
 */
static const char CMD_SYSTEM_RESET[]    = "system reset";

/*§
 Exits the terminal and resumes normal program operation.
 */
static const char CMD_EXIT[]            = "exit";

/*§
 Activates the analog inputs of the DAC.
 */
static const char CMD_PCM1774_ANALOG[]  = "analog mode";

/*§
 Performs the intitialization rutine for the DAC.
 */
static const char CMD_PCM1774_INIT[]    = "pcm1774 init";

/*§
 Triggers a DMA transfer on the DMA0 channel.
 */
static const char CMD_TRIGGER_DMA0[]    = "trigger dma0";

/*§
 Turns one note on.
 Parmeters: <channel number> <note number> <velocity>
 */
static const char CMD_NOTE_ON[]         = "note on";

/*§
 Turns one note off.
 Parameters: <channel number> <note number>
 */
static const char CMD_NOTE_OFF[]        = "note off";

/*§
 Turns all notes off.
 */
static const char CMD_ALL_NOTES_OFF[]   = "all notes off";

static const char CMD_HELP[]            = "help";

//
// Get commands
//

/*§
 Gets register states from the SPI1 module.
 */
static const char GET_SPI1_STAT[]       = "get spi1 status";

/*§
 Gets register states from the SPI2 module.
 */
static const char GET_SPI2_STAT[]       = "get spi2 status";

/*§
 Gets register states from the DMA0 module.
 */
static const char GET_DMA0_STAT[]       = "get dma0 status";

/*§
 Gets the size of the sample buffer.
 */
static const char GET_SAMPLE_BUFF_SIZE[]= "get sample buffer size";

/*§
 Gets the current status of the audio channel square0.
 */
static const char GET_SQ0_CH_STAT[]     = "get square0 status";

/*§
 Gets the current status of the audio channel square1.
 */
static const char GET_SQ1_CH_STAT[]     = "get square1 status";

/*§
 Gets the current status of the audio channel triangle0.
 */
static const char GET_TRI0_CH_STAT[]    = "get triangle0 status";

//
// Set commands
//
/*§
 Sets the contents of a register in the DAC.
 Parameters: <register index in hex> <register value in hex>
 */
static const char SET_PCM1774_REG[]     = "set pcm1774 reg";

/*§
 Sets the main volume of the headphone amplifier.
 Paramters: <volume setting in range [0, 63]>
 */
static const char SET_PCM1774_VOL[]     = "set main volume";

/*§
 Sets the duty cycle of a square or trangle audio channel.
 Parameters: <audio channel number> <duty cycle in range [0, 255]>
 */
static const char SET_DUTY[]            = "set duty";

/*§
 Configures the vibrato of one square/triangle channel.
 Parameters: <audio channel number> <vibrato rate> <vibrato depth>
 */
static const char SET_VIBRATO_CONF[]    = "set vibrato config";

/*§
 Turns the vibrato on for one audio channel.
 Parameters: <audio channel number>
 */
static const char SET_VIBRATO_ON[]      = "set vibrato on";

/*§
 Turns the vibrato off for one audio channel.
 Parameters: <audio channel number>
 */
static const char SET_VIBRATO_OFF[]     = "set vibrato off";

// =============================================================================
// Private variables
// =============================================================================

static bool command_received_event = false;
static bool terminal_open = false;
static char* reply_buff;

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Runs the terminal mode.
 * @param void
 * @return void
 */
void terminal_mode_run(void);

/**
 * @brief Updates the command buffer with any received characters.
 * @param cmd_buff - The buffer to update.
 * @param cmd_buff_len - A pointer to the number of characters in the buffer.
 * @param buffer_size - The maximum size of the buffer.
 */
void update_cmd_buffer(char* cmd_buff,
                       uint16_t* cmd_buff_len,
                       uint16_t buffer_size);

/**
 * @brief Executes the command stored in the command buffer.
 * @param cmd_buff - The command buffer in which the command is stored.
 * @param cmd_buff_len - A pointer to the number of char's in the cmd buffer.
 * @param buffer_size - The maximum size of the buffer.
 */
void execute_command(char* cmd_buff,
                     uint16_t* cmd_buff_len,
                     uint16_t buffer_size);

/* **********************************************
 *  Commands
 ***********************************************/
// Get commands
static void get_spi1_stat(void);
static void get_spi2_stat(void);
static void get_dma0_stat(void);
static void get_sample_buffer_size(void);
static void get_sq0_stat(void);
static void get_sq1_stat(void);
static void get_tri0_stat(void);

// Set commands
static void set_pcm1774_reg(char* cmd_buff);
static void set_volume(char* cmd_buff);
static void set_duty(char* cmd_buff);
static void set_vibrato_conf(char* cmd_buff);
static void set_vibrato_on(char* cmd_buff);
static void set_vibrato_off(char* cmd_buff);

// Commands
static void cmd_note_on(char* cmd_buff);
static void cmd_note_off(char* cmd_buff);
static void cmd_all_notes_off(void);

// =============================================================================
// Public function definitions
// =============================================================================

void terminal_check_access(void)
{
    uint16_t bytes_received;
    uint16_t index;
    bool password_ok;

    bytes_received = uart_get_receive_buffer_size();

    if (bytes_received >= PASSWORD_STR_LEN)
    {
        index = 0;
        password_ok = true;

        while ( (true == password_ok) &&
                (index != PASSWORD_STR_LEN) )
        {
            if (uart_get(bytes_received - 1 - index) !=
                TERMINAL_PASSWORD[PASSWORD_STR_LEN - 1 - index])
            {
                password_ok = false;
            }

            ++index;
        }

        if (password_ok)
        {
            uart_clear_receive_buffer();
            terminal_mode_run();
        }
    }
}

// =============================================================================
// Private function definitions
// =============================================================================


void terminal_mode_run(void)
{
    terminal_open = true;

    char cmd_buff[CMD_BUFFER_SIZE] = {0};
    uint16_t cmd_buff_len = 0;

    uart_write_string(NEWLINE);
    uart_write_string(TERMINAL_OPEN);
    uart_write_string(COMMAND_ENTER);

    while (terminal_open)
    {
        // Clear the WatchDog Timer
        ClrWdt();

        update_cmd_buffer(cmd_buff, &cmd_buff_len, CMD_BUFFER_SIZE);

        if (command_received_event)
        {
            command_received_event = false;

            execute_command(cmd_buff, &cmd_buff_len, CMD_BUFFER_SIZE);
        }
    }
}

void update_cmd_buffer(char* cmd_buff,
                       uint16_t* cmd_buff_len,
                       uint16_t buffer_size)
{
    uint16_t bytes_received;
    uint16_t index;
    uint16_t cmd_buff_start;

    if (g_uart_receive_event)
    {
        g_uart_receive_event = false;

        uart_disable_rx_interrupt();

        bytes_received = uart_get_receive_buffer_size();
        cmd_buff_start = *cmd_buff_len;
        
        for (index = 0; 
            (index != bytes_received) && ((*cmd_buff_len) < buffer_size);
            ++index)
        {
            cmd_buff[cmd_buff_start + index] = uart_get(index);
            ++(*cmd_buff_len);

            if (COMMAND_TERMINATION_CHARACTER == uart_get(index))
            {
                command_received_event = true;
            }
        }


        // Clear the cmd_buff in case of overflow.
        if ((*cmd_buff_len) == buffer_size)
        {
            for (index = 0; index != buffer_size; ++index)
            {
                cmd_buff[index] = 0;
            }

            *cmd_buff_len = 0;
        }

        uart_clear_receive_buffer();

        uart_enable_rx_interrupt();
    }
}

void execute_command(char* cmd_buff,
                     uint16_t* cmd_buff_len,
                     uint16_t buffer_size)
{
    uint16_t i = 0;
    bool syntax_error = false;
    char reply_buffer[128];
    reply_buff = reply_buffer;


    if (NULL != strstr(cmd_buff, CMD_HELP))
    {
        terminal_help(cmd_buff);
    }
    else if (NULL != strstr(cmd_buff, GET))
    {
        //
        // GET
        //
        if  (NULL != strstr(cmd_buff, GET_SPI1_STAT))
            get_spi1_stat();
        else if (NULL != strstr(cmd_buff, GET_SPI2_STAT))
            get_spi2_stat();
        else if (NULL != strstr(cmd_buff, GET_DMA0_STAT))
            get_dma0_stat();
        else if (NULL != strstr(cmd_buff, GET_SAMPLE_BUFF_SIZE))
            get_sample_buffer_size();
        else if (NULL != strstr(cmd_buff, GET_SQ0_CH_STAT))
            get_sq0_stat();
        else if (NULL != strstr(cmd_buff, GET_SQ1_CH_STAT))
            get_sq1_stat();
        else if (NULL != strstr(cmd_buff, GET_TRI0_CH_STAT))
            get_tri0_stat();
        else
        {
            syntax_error = true;
        }
        
    }
    else if (NULL != strstr(cmd_buff, SET))
    {
        //
        // SET
        //

        if (NULL != strstr(cmd_buff, SET_PCM1774_REG))
            set_pcm1774_reg(cmd_buff);
        else if (NULL != strstr(cmd_buff, SET_PCM1774_VOL))
            set_volume(cmd_buff);
        else if (NULL != strstr(cmd_buff, SET_DUTY))
            set_duty(cmd_buff);
        else if (NULL != strstr(cmd_buff, SET_VIBRATO_CONF))
            set_vibrato_conf(cmd_buff);
        else if (NULL != strstr(cmd_buff, SET_VIBRATO_ON))
            set_vibrato_on(cmd_buff);
        else if (NULL != strstr(cmd_buff, SET_VIBRATO_OFF))
            set_vibrato_off(cmd_buff);
        else
        {
            syntax_error = true;
        }
        
    }
    else
    {
        //
        // CMD
        //
        if (NULL != strstr(cmd_buff, CMD_NOTE_ON))
            cmd_note_on(cmd_buff);
        else if (NULL != strstr(cmd_buff, CMD_NOTE_OFF))
            cmd_note_off(cmd_buff);
        else if (NULL != strstr(cmd_buff, CMD_ALL_NOTES_OFF))
            cmd_all_notes_off();
        else if (NULL != strstr(cmd_buff, CMD_EXIT))
        {
            terminal_open = false;

            uart_write_string("\tTerminal closed.");
            uart_write_string(NEWLINE);
        }
        else if (NULL != strstr(cmd_buff, CMD_SYSTEM_RESET))
        {
            __asm__ volatile ("reset");
        }
        else if (NULL != strstr(cmd_buff, CMD_PCM1774_ANALOG))
        {
            uart_write_string("\tSetting the PCM1774 in analog to analog mode");
            uart_write_string(NEWLINE);

            pcm1774_analog_to_analog_mode();
        }
        else if (NULL != strstr(cmd_buff, CMD_PCM1774_INIT))
        {
            uart_write_string("\tInitializing the PCM1774");
            uart_write_string(NEWLINE);

            pcm1774_init();
        }
        else if (NULL != strstr(cmd_buff, CMD_TRIGGER_DMA0))
        {
            uart_write_string("\tDMA0 triggered");
            uart_write_string(NEWLINE);

            DMACH0bits.CHREQ = 1;
        }
        else
        {
            syntax_error = true;
        }
    }

    if (syntax_error)
    {
        uart_write_string(SYNTAX_ERROR);
    }

    if (terminal_open)
    {
        uart_write_string(COMMAND_ENTER);
    }

    // Clear the command buffer.
    for (i = 0; i != CMD_BUFFER_SIZE; ++i)
    {
        cmd_buff[i] = 0;
    }

    *cmd_buff_len = 0;

    
}

/* *******************************************************
 * Command help functions
 *********************************************************/

static void get_spi1_stat(void)
{
    sprintf(reply_buff, "\tSPI1STATH: %4.4x SPI1STATL: %4.4x%s",
            SPI1STATH, SPI1STATL, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI1CON1L: %4.4x SPI1CON1H: %4.4x%s",
            SPI1CON1L, SPI1CON1H, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI1IMSKL: %4.4x SPI1IMASKH: %4.4x%s",
            SPI1IMSKL, SPI1IMSKH, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI1URDTL: %4.4x SPI1URDTH: %4.4x%s",
            SPI1URDTL, SPI1URDTH, NEWLINE);
    uart_write_string(reply_buff);
}

static void get_spi2_stat(void)
{
    sprintf(reply_buff, "\tSPI2STATH: %4.4x SPI2STATL: %4.4x%s",
            SPI2STATH, SPI2STATL, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI2CON1L: %4.4x SPI2CON1H: %4.4x%s",
            SPI2CON1L, SPI2CON1H, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI2IMSKL: %4.4x SPI2IMASKH: %4.4x%s",
            SPI2IMSKL, SPI2IMSKH, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tSPI2URDTL: %4.4x SPI2URDTH: %4.4x%s",
            SPI2URDTL, SPI2URDTH, NEWLINE);
    uart_write_string(reply_buff);
}

static void get_dma0_stat(void)
{
    sprintf(reply_buff, "\tDMACON: %4.4x%s",
            DMACON, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tDMAL: %4.4x DMAH: %4.4x%s",
            DMAL, DMAH, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tDMAINT0: %4.4x DMACH0: %4.4x%s",
            DMAINT0, DMACH0, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tDMASRC0: %4.4x DMADST0: %4.4x%s",
            DMASRC0, DMADST0, NEWLINE);
    uart_write_string(reply_buff);

    sprintf(reply_buff, "\tDMACNT0: %4.4x%s",
            DMACNT0, NEWLINE);
    uart_write_string(reply_buff);
}

static void get_sample_buffer_size(void)
{
    sprintf(reply_buff, "\tSample buffer size: %d%s",
            audio_get_sample_buff_size(), NEWLINE);
    uart_write_string(reply_buff);
}

static void set_pcm1774_reg(char* cmd_buff)
{
    char* p = cmd_buff;
    uint16_t arg1 = 0;
    uint16_t arg2 = 0;

    p = strstr(cmd_buff, SET_PCM1774_REG);
    p += strlen(SET_PCM1774_REG) + 1; // +1 for space

    arg1 = strtol(p, &p, 16);
    ++p; // for space
    arg2 = strtol(p, &p, 16);

    pcm1774_write_reg((uint8_t)arg1, (uint8_t)arg2);
}

static void set_volume(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t vol = 0;

    p = strstr(cmd_buff, SET_PCM1774_VOL);
    p += strlen(SET_PCM1774_VOL) + 1; // +1 for space

    vol = strtol(p, &p, 10);

    pcm1774_set_volume(vol);
}

static void get_sq0_stat(void)
{
    audio_print_channel_status(AUDIO_CH_SQUARE0);
}

static void get_sq1_stat(void)
{
    audio_print_channel_status(AUDIO_CH_SQUARE1);
}

static void get_tri0_stat(void)
{
    audio_print_channel_status(AUDIO_CH_TRIANGLE0);
}

static void cmd_note_on(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 0;
    uint8_t note = 0;
    uint8_t velocity = 0;

    p = strstr(cmd_buff, CMD_NOTE_ON);
    p += strlen(CMD_NOTE_ON) + 1; // +1 for space

    channel = strtol(p, &p, 10);
    ++p; // for space
    note = strtol(p, &p, 10);
    ++p; // for space
    velocity = strtol(p, &p, 10);

    audio_note_on((audio_ch_nbr_t)channel, note, velocity);

    sprintf(reply_buff, "\tNote on channel %u, note: %u, velocity: %u%s",
            channel, note, velocity, NEWLINE);
    uart_write_string(reply_buff);
}

static void cmd_note_off(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 0;

    p = strstr(cmd_buff, CMD_NOTE_OFF);
    p += strlen(CMD_NOTE_OFF) + 1; // +1 for space

    channel = strtol(p, &p, 10);

    audio_note_off((audio_ch_nbr_t)channel);

    sprintf(reply_buff, "\tNote off channel %u%s",
            channel, NEWLINE);
    uart_write_string(reply_buff);
}

static void cmd_all_notes_off(void)
{
    uint16_t i;

    for (i = 0; i != AUDIO_CH_NBR_OF_CHANNELS; ++i)
    {
        audio_note_off((audio_ch_nbr_t)i);
    }

    sprintf(reply_buff, "\tNote off on all channels%s", NEWLINE);
    uart_write_string(reply_buff);
}

static void set_duty(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 255;
    uint8_t duty = 128;

    p = strstr(cmd_buff, SET_DUTY);
    p += strlen(SET_DUTY) + 1; // +1 for space

    channel = strtol(p, &p, 10);
    ++p;
    duty = strtol(p, &p, 10);

    audio_set_duty((audio_ch_nbr_t)channel, duty);

    sprintf(reply_buff, "\tSet duty channel %u, duty: %u%s",
            channel, duty, NEWLINE);
    uart_write_string(reply_buff);
}

static void set_vibrato_conf(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 255;
    uint8_t rate = 0;
    uint8_t depth = 0;

    p = strstr(cmd_buff, SET_VIBRATO_CONF);
    p += strlen(SET_VIBRATO_CONF) + 1; // +1 for space

    channel = strtol(p, &p, 10);
    ++p;    // for space
    rate = strtol(p, &p, 10);
    ++p;    // for space
    depth = strtol(p, &p, 10);

    audio_configure_vibrato((audio_ch_nbr_t)channel, rate, depth);

    sprintf(reply_buff,
            "\tSet vibrato config channel: %u, rate: %u, depth: %u%s",
            channel, rate, depth, NEWLINE);
    uart_write_string(reply_buff);
}

static void set_vibrato_on(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 255;

    p = strstr(cmd_buff, SET_VIBRATO_ON);
    p += strlen(SET_VIBRATO_ON) + 1;    // +1 for space

    channel = strtol(p, &p, 10);

    audio_vibrato_on((audio_ch_nbr_t)channel);

    sprintf(reply_buff, "\tSet vibrato on channel: %u%s",
            channel, NEWLINE);
    uart_write_string(reply_buff);
}

static void set_vibrato_off(char* cmd_buff)
{
    char* p = cmd_buff;
    uint8_t channel = 255;

    p = strstr(cmd_buff, SET_VIBRATO_OFF);
    p += strlen(SET_VIBRATO_OFF) + 1;    // +1 for space

    channel = strtol(p, &p, 10);

    audio_vibrato_off((audio_ch_nbr_t)channel);

    sprintf(reply_buff, "\tSet vibrato off channel: %u%s",
            channel, NEWLINE);
    uart_write_string(reply_buff);
}