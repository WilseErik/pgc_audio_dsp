/*******************************************************************************
 * File:   pinmap.h
 * Author: Erik
 *
 * This file provides macros for mapping the extern electrical nets
 * to their GPIO pins.
 *
 ******************************************************************************/

#ifndef PINMAP_H
#define	PINMAP_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================
#include <xc.h>

// =============================================================================
// Public type definitions
// =============================================================================

// Peripheral Pin Select selectable output sources.
// Setting the RPORx register with the listed value assigns that output
// function to the associated RPn pin.
typedef enum pps_output_source_t
{
    PPS_OUT_SRC_NULL    = 0,     // Disables the RPn output function.
    PPS_OUT_SRC_C1OUT   = 1,     // Comparator 1 Output
    PPS_OUT_SRC_C2OUT   = 2,     // Comparator 2 Output
    PPS_OUT_SRC_U1TX    = 3,     // UART1 Transmit
    PPS_OUT_SRC_N_U1RTS = 4,     // UART1 Request-to-Send
    PPS_OUT_SRC_U2TX    = 5,     // UART2 Transmit
    PPS_OUT_SRC_N_U2RTS = 6,     // UART2 Request-to-Send
    PPS_OUT_SRC_SDO1    = 7,     // SPI1 Data Output
    PPS_OUT_SRC_SCK1OUT = 8,     // SPI1 Clock Output
    PPS_OUT_SRC_SS1OUT  = 9,     // SPI1 Slave Select Output
    PPS_OUT_SRC_SDO2    = 10,    // SPI2 Data Output
    PPS_OUT_SRC_SCK2OUT = 11,    // SPI2 Clock Output
    PPS_OUT_SRC_SS2OUT  = 12,    // SPI2 Slave Select Output
    PPS_OUT_SRC_OC1     = 13,    // Output Compare 1
    PPS_OUT_SRC_OC2     = 14,    // Output Compare 2
    PPS_OUT_SRC_OC3     = 15,    // Output Compare 3
    PPS_OUT_SRC_OC4     = 16,    // Output Compare 4
    PPS_OUT_SRC_OC5     = 17,    // Output Compare 5
    PPS_OUT_SRC_OC6     = 18,    // Output Compare 6
    PPS_OUT_SRC_U3TX    = 19,    // UART3 Transmit
    PPS_OUT_SRC_N_U3RTS = 20,    // UART3 Request-to-Send
    PPS_OUT_SRC_U4TX    = 21,    // UART4 Transmit
    PPS_OUT_SRC_N_U4RTS = 22,    // UART4 Request-to-Send
    PPS_OUT_SRC_SDO3    = 23,    // SPI3 Data Output
    PPS_OUT_SRC_SCK3OUT = 24,    // SPI3 Clock Output
    PPS_OUT_SRC_SS3OUT  = 25,    // SPI3 Slave Select Output
    PPS_OUT_SRC_C3OUT   = 26,    // Comparator 3 Output
    PPS_OUT_SRC_MDOUT   = 27     // DSM Modulator Output
} pps_input_source_t;

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Global constatants
// =============================================================================

#define DIRECTION_OUTPUT            (0)
#define DIRECTION_INPUT             (1)

//
// Leds
//
#define RED_LED_PIN                 LATAbits.LATA0
#define RED_LED_PIN_DIRECTION       TRISAbits.TRISA0
#define RED_LED_ON                  (RED_LED_PIN = 1)
#define RED_LED_OFF                 (RED_LED_PIN = 0)
#define RED_LED_TOGGLE              (RED_LED_PIN = !RED_LED_PIN)

#define GREEN_LED_PIN               LATAbits.LATA1
#define GREEN_LED_PIN_DIRECTION     TRISAbits.TRISA1
#define GREEN_LED_ON                (GREEN_LED_PIN = 1)
#define GREEN_LED_OFF               (GREEN_LED_PIN = 0)
#define GREEN_LED_TOGGLE            (GREEN_LED_PIN = !GREEN_LED_PIN)

//
// UART
//
#define UART_TX_PIN                 LATBbits.LATB2
#define UART_TX_PIN_DIRECTION       TRISBbits.TRISB2
#define UART_TX_OUTPUT_MAPPING      RPOR1bits.RP2R

#define UART_RX_PIN                 LATBbits.LATB3
#define UART_RX_PIN_DIRECTION       TRISBbits.TRISB3
#define UART_RX_INPUT_MAPPING       RPINR18bits.U1RXR
#define UART_RX_RP_PIN_NBR          3u

//
// I2S interface
//
#define I2S_DIN_PIN                 LATBbits.LATB7
#define I2S_DIN_PIN_DIRECTION       TRISBbits.TRISB7
#define I2S_DIN_OUTPUT_MAPPING      RPOR3bits.RP7R

#define I2S_BCK_PIN                 LATBbits.LATB8
#define I2S_BCK_PIN_DIRECTION       TRISBbits.TRISB8
#define I2S_BCK_INPUT_MAPPING       RPINR22bits.SCK2R
#define I2S_BCK_PIN_NBR             8u

#define I2S_LRCK_PIN                LATBbits.LATB9
#define I2S_LRCK_PIN_DIRECTION      TRISBbits.TRISB9
#define I2S_LRCK_INPUT_MAPPING      RPINR23bits.SS2R
#define I2S_LRCK_PIN_NBR            9u

//
// Digital to Analog Converter SPI interface
//
#define N_DAC_SS_PIN                LATBbits.LATB13
#define N_DAC_SS_PIN_DIRECTION      TRISBbits.TRISB13
#define N_DAC_SS_OUTPUT_MAPPING     RPOR6bits.RP13R
#define N_DAC_SS_ON                 (N_DAC_SS_PIN = 0)
#define N_DAC_SS_OFF                (N_DAC_SS_PIN = 1)
#define N_DAC_SS_MAPPING            RPOR6bits.RP13R

#define DAC_SPI_CLK_PIN             LATBbits.LATB14
#define DAC_SPI_CLK_PIN_DIRECITON   TRISBbits.TRISB14
#define DAC_SPI_CLK_OUTPUT_MAPPING  RPOR7bits.RP14R

#define DAC_MOSI_PIN                LATBbits.LATB15
#define DAC_MOSI_PIN_DIRECTION      TRISBbits.TRISB15
#define DAC_MOSI_OUTPUT_MAPPING     RPOR7bits.RP15R


//
// Main processor SPI link
//
#define N_LINK_SS_PIN               LATBbits.LATB5
#define N_LINK_SS_PIN_DIRECTION     TRISBbits.TRISB5

#define LINK_CLK_PIN                LATBbits.LATB6
#define LINK_CLK_PIN_DIRECTION      TRISBbits.TRISB6
#define LINK_CLK_INPUT_MAPPING      RPINR28bits.SCK3R
#define LINK_CLK_RP_PIN_NBR         6u

#define LINK_MISO_PIN               LATBbits.LATB10
#define LINK_MISO_PIN_DIRECTION     TRISBbits.TRISB10
#define LINK_MISO_OUTPUT_MAPPING    RPOR5bits.RP10R

#define LINK_MOSI_PIN               LATBbits.LATB11
#define LINK_MOSI_PIN_DIRECTION     TRISBbits.TRISB11
#define LINK_MOSI_INPUT_MAPPING     RPINR28bits.SDI3R
#define LINK_MOSI_RP_PIN_NBR        11u


//
// Tip switch (audio connector)
//
#define TIP_SWITCH_PIN              PORTAbits.RA4
// RA4 is always an input, no LATA4 bit exists.


//
// In Circut Serial Programming interface (ICSP)
//
#define PGD1_PIN                    LATBbits.LATB0
#define PGD1_PIN_DIRECTION          TRISBbits.TRISB0

#define PGC1_PIN                    LATBbits.LATB1
#define PGC1_PIN_DIRECTION          TRISBbits.TRISB1


//
// Unused pins (floating)
//
#define NC1_PIN                     LATAbits.LATA2
#define NC1_PIN_DIRECTION           TRISAbits.TRISA2
#define NC1_PIN_PULLDOWN            CNPD2bits.CN30PDE

#define NC2_PIN                     LATAbits.LATA3
#define NC2_PIN_DIRECTION           TRISAbits.TRISA3
#define NC2_PIN_PULLDOWN            CNPD2bits.CN29PDE

#define NC3_PIN                     PORTBbits.RB4
// RB4 is always an input, no LATB4 bit exists.
#define NC3_PIN_PULLDOWN            CNPD1bits.CN0PDE

#define NC4_PIN                     LATBbits.LATB12
#define NC4_PIN_DIRECTION           TRISBbits.TRISB12
#define NC4_PIN_PULLDOWN            CNPD1bits.CN14PDE

// =============================================================================
// Public function declarations
// =============================================================================

#ifdef	__cplusplus
}
#endif

#endif	/* PINMAP_H */

