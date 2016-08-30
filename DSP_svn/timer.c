/*
 * This file handles the timer modules.
 *
 * References:
 * - PIC24FJ128GA202 datasheet, document number DS30010038C
 *  - dsPIC33/PIC24 Family Reference Manual, Timers,
 *    document number DS39704A
 */

// =============================================================================
// Include statements
// =============================================================================

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "timer.h"
#include "configuration_bits.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================
volatile bool g_timer_modulation_event = false;

// =============================================================================
// Private constants
// =============================================================================

/*
 * The frequency in Hz of which the timer overflows.
 */
const uint32_t TIMER_FREQ_HZ = 100;

// =============================================================================
// Private variables
// =============================================================================
static uint16_t pr1_reset_value = 0xFFFF;

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================

void timer_start()
{
    g_timer_modulation_event = false;

    T1CON = 0;

    T1CONbits.TON = 1;
    
    // Set prescaler to 1:8
    T1CONbits.TCKPS0 = 1;
    T1CONbits.TCKPS1 = 0;

    // Use internal clock (Fosc / 2)
    T1CONbits.TCS = 0;

    // Disable the time accumulation feature
    T1CONbits.TGATE = 0;

    pr1_reset_value = ((FOSC_FREQ / 2) / 8) / TIMER_FREQ_HZ;
    PR1 = pr1_reset_value;

    // Enable interrupts
    IFS0bits.T1IF = 0;                      // Interrupt flag
    IPC0bits.IC1IP = TIMER_INT_PRIORITY; // Interrupt priority
    IEC0bits.T1IE = 1;                      // Interrupt enable
}

void timer_stop()
{
    IEC0bits.T1IE = 0;
    IFS0bits.T1IF = 0;
}

// =============================================================================
// Private function definitions
// =============================================================================

/**
 * @breif Interrupt service rutine for the system tick timer.
 */
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{    
    PR1 = pr1_reset_value;

    g_timer_modulation_event = true;

    IFS0bits.T1IF = 0;
}

