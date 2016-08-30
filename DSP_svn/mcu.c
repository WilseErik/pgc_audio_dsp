/*
 * This file contains general functions concerning the microcontroller unit.
 */

// =============================================================================
// Include statements
// =============================================================================
#include <xc.h>

#include <stdint.h>

#include "mcu.h"
#include "uart.h"
#include "pinmap.h"

// =============================================================================
// Private type definitions
// =============================================================================

// Bit masks for the Reset CONtrol register
typedef enum startup_reason_t
{
    MCU_STARTUP_POR     = (uint16_t)0x0001,
    MCU_STARTUP_BOR     = (uint16_t)0x0002,
    MCU_STARTUP_IDLE    = (uint16_t)0x0004,
    MCU_STARTUP_SLEEP   = (uint16_t)0x0008,
    MCU_STARTUP_WDTO    = (uint16_t)0x0010,
    MCU_STARTUP_SWDTEN  = (uint16_t)0x0020,
    MCU_STARTUP_SWR     = (uint16_t)0x0040,
    MCU_STARTUP_EXTR    = (uint16_t)0x0080,
    MCU_STARTUP_VREGS   = (uint16_t)0x0100,
    MCU_STARTUP_CM      = (uint16_t)0x0200,
    MCU_STARTUP_DPSLP   = (uint16_t)0x0400,
    // RESERVED         = (uint16_t)0x0800,
    // RETEN            = (uint16_t)0x1000,
    // RESEVED          = (uint16_t)0x2000,
    MCU_STARTUP_IOPUWR  = (uint16_t)0x4000,
    MCU_STARTUP_TRAPR   = (uint16_t)0x8000
} startup_reason_t;


// =============================================================================
// Global variables
// =============================================================================
static uint16_t startup_reason = 0;

// =============================================================================
// Private constants
// =============================================================================
static const char separator_line[] =
"=================================================================================\r\n";
// =============================================================================
// Private variables
// =============================================================================

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Checks the startup reason of the mcu and prints it over the UART.
 * @param void
 * @return void
 */
static void mcu_check_startup_reason(void);

// =============================================================================
// Public function definitions
// =============================================================================

void mcu_init(void)
{
    mcu_check_startup_reason();
}

// =============================================================================
// Private function definitions
// =============================================================================

static void mcu_check_startup_reason(void)
{
    startup_reason = RCON;

    uart_init();

    uart_write_string("\r\n\r\n");
    uart_write_string(separator_line);
    uart_write_string("Audio DSP started.\r\n");
    uart_write_string("Reset reason was:\r\n");
    
    if (startup_reason & MCU_STARTUP_POR)
    {
        uart_write_string("\t[POR] Power on reset\r\n");
        RCON &= ~MCU_STARTUP_POR;
    }
    else if (startup_reason & MCU_STARTUP_BOR)
    {
        uart_write_string("\t[BOR] Brown out reset\r\n");
        RCON &= ~MCU_STARTUP_BOR;
    }

    if (startup_reason & MCU_STARTUP_IDLE)
    {
        uart_write_string("\t[IDLE] Wake from idle\r\n");
        RCON &= ~MCU_STARTUP_IDLE;
    }

    if (startup_reason & MCU_STARTUP_SLEEP)
    {
        uart_write_string("\t[SLEEP] Wake from sleep\r\n");
        RCON &= ~MCU_STARTUP_SLEEP;
    }

    if (startup_reason & MCU_STARTUP_WDTO)
    {
        uart_write_string("\t[WDTO] Watch dog time out\r\n");
        RCON &= ~MCU_STARTUP_WDTO;
    }

    if (startup_reason & MCU_STARTUP_SWDTEN)
    {
        uart_write_string("\t[SWDTEN] Software enable/disable of WDT bit\r\n");
    }

    if (startup_reason & MCU_STARTUP_SWR)
    {
        uart_write_string("\t[SWR] Software reset\r\n");
        RCON &= ~MCU_STARTUP_SWR;
    }

    if (startup_reason & MCU_STARTUP_EXTR)
    {
        uart_write_string("\t[EXTR] External reset (Master CLear pin Reset)\r\n");
        RCON &= ~MCU_STARTUP_EXTR;
    }

    if (startup_reason & MCU_STARTUP_VREGS)
    {
        uart_write_string("\t[VREGS] Program Memory Power During Sleep bit\r\n");
    }

    if (startup_reason & MCU_STARTUP_CM)
    {
        uart_write_string("\t[CM] Configuration Word Mismatch Reset\r\n");
        RCON &= ~MCU_STARTUP_CM;
    }

    if (startup_reason & MCU_STARTUP_DPSLP)
    {
        uart_write_string("\t[DPSLP] The device has been in deep sleep mode\r\n");
        RCON &= ~MCU_STARTUP_DPSLP;
    }

    if (startup_reason & MCU_STARTUP_IOPUWR)
    {
        uart_write_string("\t[IOPUWR] Illegal OPcode or uninitialized W access Reset\r\n");
        RCON &= ~MCU_STARTUP_IOPUWR;
    }

    if (startup_reason & MCU_STARTUP_TRAPR)
    {
        uart_write_string("\t[TRAPR] Trap conflict reset\r\n");
        RCON &= ~MCU_STARTUP_TRAPR;
    }

    uart_write_string(separator_line);
}

void __attribute__((interrupt, auto_psv)) _DefaultInterrupt()
{
    while (1)
    {
        GREEN_LED_OFF;
        RED_LED_ON;
    }
}