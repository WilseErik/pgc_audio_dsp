/* 
 * File:   gpio.h
 * Author: Erik
 *
 * This file is used for setting up the General Purpose Input Output pins
 * to the correct states.
 */

#ifndef GPIO_H
#define	GPIO_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================

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
     * @brief Initialies all GPIO ports on the MCU.
     * @param None.
     * @return None.
     **/
    void gpio_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* GPIO_H */

