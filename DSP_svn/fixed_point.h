/* 
 * File:   fixed_point.h
 * Author: Erik
 *
 * This file provides functions for using fixed point arithmetic.
 * The type provided by this file is using 32 bits where the binary point
 * is between bit 16 and 17 and allows for faster numerical computations
 * compared to using floating point.
 */

#ifndef AUDIO_TIME_H
#define	AUDIO_TIME_H

#ifdef	__cplusplus
//extern "C" {
#endif


// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>

// =============================================================================
// Public type definitions
// =============================================================================


// Use fixed point arthmetic where the binary point is between bit 15 and
// bit 16
// For example 1.75 is represented as: 00000000 00000001 . 11000000 00000000
//
typedef uint32_t q16_16_t;

// =============================================================================
// Global constatants
// =============================================================================
#define Q16_16_T_ONE    (0x00010000)

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Public function declarations
// =============================================================================

/**
 * @brief Calculates the product of two q16_16_t numbers.
 * @param a - The first q16_16_t factor.
 * @param b - The second q16_16_t factor.
 * @return The product of a * b
 */
static inline q16_16_t q16_16_multiply(q16_16_t a, q16_16_t b)
{
    uint64_t p;

    p = (uint64_t)a * (uint64_t)b;
    p = p >> 16;
    return (q16_16_t)p;
}

/**
 * @brief Calculates the quotient of two q16_16_t numbers.
 * @param a - The nominator.
 * @param b - The denominator.
 * @return The quotient a / b
 */
static inline q16_16_t q16_16_divide(q16_16_t a, q16_16_t b)
{
    uint64_t q;

    q = (uint64_t)a << 16;
    q /= b;

    return (q16_16_t)q;
}

/**
 * @brief The number of stepps in t rounded down.
 * @param t - the time to round down.
 * @return The number of floored number of stepps in t.
 */
static inline uint16_t q16_16_floor(q16_16_t t)
{
    return t & 0xFFFF0000;
}

/**
 * @brief The number of stepps in t rounded up.
 * @param t - The time to round up.
 * @return The number of stepps in t rounded up.
 */
static inline uint16_t q16_16_ceil(q16_16_t t)
{
    return (t & 0xFFFF0000) + 0x00010000;
}

/**
 * @brief The number of stepps in t rounded to the nearest integer.
 * @param t - The time to round.
 * @return The number of stepps in t rounded to the nearest integer.
 */
static inline q16_16_t q16_16_round(q16_16_t t)
{
    if ((t & 0x0000FFFF) < (UINT16_MAX >> 1))
    {
        return t & 0xFFFF0000;
    }
    else
    {
        return (t & 0xFFFF0000) + 0x00010000;
    }
}

/**
 * @brief Converts an integer to the q16_16_t.
 * @param i - The integer to convert.
 * @return the q16_16_t representation of i.
 */
static inline q16_16_t int_to_q16_16(uint16_t i)
{
    return ((uint32_t)i) << 16;
}

static inline uint16_t q16_16_to_int(q16_16_t t)
{
    return (q16_16_round(t) >> 16);
}

/**
 * @brief Converts a double to a q16_16_t.
 * @param d - The double to convert.
 * @return The q16_16_t representation of d.
 */
static inline q16_16_t double_to_q16_16(double d)
{
    return (q16_16_t)(d * UINT16_MAX);
}

/**
 * @brief Converts a q16_16_t value to a double.
 * @param x - The q16_16_t to convert.
 * @return The double floating point representation of x.
 */
static inline double q16_16_to_double(q16_16_t x)
{
    return ((double)x) / ((double)UINT16_MAX);
}

#ifdef	__cplusplus
}
#endif

#endif	/* AUDIO_TIME_H */

