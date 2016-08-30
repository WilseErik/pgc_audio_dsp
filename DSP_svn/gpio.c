/*
 * This file is used for setting up the General Purpose Input Output pins
 * to the correct states.
 *
 *  * References:
 *  - PIC24FJ128GA202 datasheet, document number DS30010038C
 */


// =============================================================================
// Include statements
// =============================================================================

#include <xc.h>

#include "pinmap.h"

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

void gpio_init(void)
{
    // All IO ports as digital IO (not analog)
    ANSA = 0x0000;
    ANSB = 0x0000;

    //
    // LEDs
    //
    RED_LED_PIN_DIRECTION = DIRECTION_OUTPUT;
    GREEN_LED_PIN_DIRECTION = DIRECTION_OUTPUT;
    RED_LED_OFF;
    GREEN_LED_OFF;

    //
    // UART
    //
    UART_TX_PIN_DIRECTION = DIRECTION_OUTPUT;
    UART_RX_PIN_DIRECTION = DIRECTION_INPUT;
    UART_TX_PIN = 1;

    //
    // Digital to Analog Converter SPI interface
    //
    N_DAC_SS_PIN_DIRECTION = DIRECTION_OUTPUT;
    DAC_SPI_CLK_PIN_DIRECITON = DIRECTION_OUTPUT;
    DAC_MOSI_PIN_DIRECTION = DIRECTION_OUTPUT;

    DAC_SPI_CLK_PIN = 0;
    DAC_MOSI_PIN = 0;
    N_DAC_SS_PIN = 1;

    //
    // I2S interface
    //
    I2S_DIN_PIN_DIRECTION = DIRECTION_OUTPUT;
    I2S_BCK_PIN_DIRECTION = DIRECTION_INPUT;
    I2S_LRCK_PIN_DIRECTION = DIRECTION_INPUT;

    //
    // Main processor SPI link
    //
    N_LINK_SS_PIN_DIRECTION = DIRECTION_INPUT;
    LINK_CLK_PIN_DIRECTION = DIRECTION_INPUT;
    LINK_MISO_PIN_DIRECTION = DIRECTION_OUTPUT;
    LINK_MOSI_PIN_DIRECTION = DIRECTION_INPUT;

    LINK_MISO_PIN = 0;

    //
    // In Circut Serial Programming interface (ICSP)
    //
    PGD1_PIN_DIRECTION = DIRECTION_INPUT;
    PGC1_PIN_DIRECTION = DIRECTION_INPUT;

    //
    // Unused pins (floating)
    //
    NC1_PIN_DIRECTION = DIRECTION_OUTPUT;
    NC2_PIN_DIRECTION = DIRECTION_OUTPUT;
    NC4_PIN_DIRECTION = DIRECTION_OUTPUT;

    NC1_PIN = 0;
    NC2_PIN = 0;
    NC3_PIN_PULLDOWN = 1;
    NC4_PIN = 0;
}

// =============================================================================
// Private function definitions
// =============================================================================
