/*
 * This file handles the initialization of the system
 */

// =============================================================================
// Include statements
// =============================================================================
#include "gpio.h"
#include "mcu.h"
#include "timer.h"
#include "pcm1774.h"
#include "uart.h"
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

// =============================================================================
// Private variables
// =============================================================================

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================

// =============================================================================
// Private function definitions
// =============================================================================

void init_system(void)
{
    gpio_init();
    uart_init();    // Start the UART interface
    mcu_init();

    audio_init();   // Start the audio engine
    pcm1774_init(); // Set up the audio interfaces
    timer_start();  // Start the audio modulation timer
}

