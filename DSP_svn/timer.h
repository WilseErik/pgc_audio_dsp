/* 
 * File:   timer.h
 * Author: Erik
 *
 * This file handles the timer modules.
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================
#include <stdbool.h>
#include <stdint.h>

// =============================================================================
// Public type definitions
// =============================================================================

 /*
 * The frequency in Hz of which the timer overflows.
 */
extern const uint32_t TIMER_FREQ_HZ;

// =============================================================================
// Global variable declarations
// =============================================================================

/*
 * This event will be set every time the timer overflows.
 */
 extern volatile bool g_timer_modulation_event;

// =============================================================================
// Global constatants
// =============================================================================
#define TIMER_INT_PRIORITY (2u)


// =============================================================================
// Public function declarations
// =============================================================================

    /**
     * @brief Starts the timer.
     * @param void
     * @return void
     */
    void timer_start(void);

    /**
     * @brief Stops the timer.
     * @param void
     * @return void
     */
    void timer_stop();

#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */

