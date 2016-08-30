/*
 * This file forms a hardware abstraction layer for the crypto module
 * in order to generate random numers.
 *
 * References:
 *  - PIC24FJ128GA202 datasheet, document number DS30010038C
 *  - dsPIC33/PIC24 Family Reference Manual, Cryptographic engine,
 *    document number  DS70005133A
 */


// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

#include "xc.h"

#include "rng.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================

// =============================================================================
// Private constants
// =============================================================================
#define RANDOM_BUFFER_SIZE  (8)
#define BITS_PER_ELEMENT    (16)

// =============================================================================
// Private variables
// =============================================================================
static bool rng_module_initialized = false;

static int16_t random_buffer[RANDOM_BUFFER_SIZE];
static uint16_t buffer_index;
static uint16_t bit_index;

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================

void rng_init(void)
{
    if (false == rng_module_initialized)
    {
        CRYCONLbits.CRYON = 1;      // Enable the cryptographic engine
        CRYCONLbits.OPMOD = 0x0A;   // Random number generation mode
        CRYCONLbits.CRYGO = 1;      // Generate one number

        buffer_index = 0;

        rng_module_initialized = true;
    }
}

void rng_deinit(void)
{
    CRYCONLbits.CRYON = 0;

    rng_module_initialized = false;
}

bool rng_get_random(void)
{
    int16_t return_val;

    if (RANDOM_BUFFER_SIZE == buffer_index)
    {
        random_buffer[0] = CRYTXTA0;
        random_buffer[1] = CRYTXTA1;
        random_buffer[2] = CRYTXTA2;
        random_buffer[3] = CRYTXTA3;
        random_buffer[4] = CRYTXTA4;
        random_buffer[5] = CRYTXTA5;
        random_buffer[6] = CRYTXTA6;
        random_buffer[7] = CRYTXTA7;

        return_val = (CRYTXTA0 >> bit_index) & 0x0001;

        CRYCONLbits.CRYGO = 1;

        buffer_index = 0;
        bit_index = 1;
    }
    else
    {
        return_val = (random_buffer[buffer_index] >> bit_index++) & 0x0001;

        if (BITS_PER_ELEMENT == bit_index)
        {
            bit_index = 0;
            ++buffer_index;
        }
    }
    
    return return_val;
}

// =============================================================================
// Private function definitions
// =============================================================================
