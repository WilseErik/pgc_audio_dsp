/* 
 * File:   pcm1774.h
 * Author: Erik
 *
 * This file forms a hardware abstraction layer for the delta-sigma
 * digital to analog converter PCM1774.
 *
 */

#ifndef PCM1774_H
#define	PCM1774_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>

#include "pcm1774_defs.h"

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
 * @brief Initializes the PCM1774 and related drivers.
 * @param void
 * @return void
 */
void pcm1774_init(void);

/**
 * @brief Configures the PCM1774 as an analog voltage amplifier.
 * @param void
 * @return void
 */
void pcm1774_analog_to_analog_mode(void);

/**
 * @brief Sets the volume of the headphone amplifier
 * @param volume - volume setting in range [0, 63]
 * @return void
 */
void pcm1774_set_volume(uint8_t volume);

/**
 * @brief Writes a byte of data to a register on the PCM1774.
 * @param reg_index - The index of the register to write to.
 * @param data - The data to write to the specified register.
 */
void pcm1774_write_reg(pcm1774_reg_addr_t reg_index, uint8_t data);

#ifdef	__cplusplus
}
#endif

#endif	/* PCM1774_H */

