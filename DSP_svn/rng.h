/* 
 * File:   rng.h
 * Author: Erik
 *
 * Created on den 23 januari 2016, 22:48
 *
 * This file provides an API for generating random numbers using dedicated
 * hardware. (RNG = Random Number Generation)
 */

#ifndef RNG_H
#define	RNG_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// Public type definitions
// =============================================================================

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Global constatants
// =============================================================================

// =============================================================================
// Public function declarations
// =============================================================================

/**
 * @brief Initializes the Random Number Generator module.
 * @param void
 * @return void
 */
void rng_init(void);

/**
 * @brief Closes the Random Number Generator module
 * @param void
 * @return void
 */
void rng_deinit(void);

/**
 * @brief Gets a random number from the rng module.
 * @param void
 * @return A random number.
 */
bool rng_get_random(void);

#ifdef	__cplusplus
}
#endif

#endif	/* RNG_H */

