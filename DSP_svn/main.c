/* 
 * File:   main.c
 * Author: Erik
 *
 * Created on den 14 oktober 2015, 22:04
 */

#if !defined(DEBUG)
#if !defined(NDEBUG)
#error DEBUG or NDEBUG should be defined as project macros.
#endif
#endif

// =============================================================================
// Include statements
// =============================================================================
#include <xc.h>

#include <stdlib.h>
#include <stdint.h>

#include "pinmap.h"
#include "init.h"
#include "event_queue.h"
#include "uart.h"
#include "terminal.h"
#include "dma.h"
#include "audio.h"
#include "timer.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================

// =============================================================================
// Private constants
// =============================================================================

// =============================================================================
// Private variables
// =============================================================================

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================

int main(void)
{
    init_system();

    while (1)
    {
        // Clear the WatchDog Timer
        ClrWdt();

        if (false == audio_is_sample_buff_full())
        {
            audio_calc_sample();
        }

        if (g_timer_modulation_event)
        {
            g_timer_modulation_event = false;

            audio_apply_modulation();
        }
        else if (g_uart_receive_event)
        {
            g_uart_receive_event = false;

            terminal_check_access();
        }

#ifdef DEBUG
        if (audio_get_sample_buff_size() < SAMPLE_BUFF_HALF_SIZE)
        {
            RED_LED_ON;
        }
        else
        {
            RED_LED_OFF;
        }
#endif
    }
    
    return (EXIT_SUCCESS);
}

// =============================================================================
// Private function definitions
// =============================================================================

