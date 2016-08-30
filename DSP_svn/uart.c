/*
 * This file handes the UART module.
 *
 * References:
 * - PIC24FJ128GA202 datasheet, document number DS30010038C
 *  - dsPIC33/PIC24 Family Reference Manual, Universal Asynchronous
 *    Receiver Transmitter, document number DS70000582E
 */

// =============================================================================
// Include statements
// =============================================================================

#include <xc.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "configuration_bits.h"
#include "pinmap.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================
volatile bool g_uart_receive_event = false;

// =============================================================================
// Private constants
// =============================================================================
#define BUFFER_SIZE     ((uint16_t)1024)
#define BACKSPACE_CHAR  (0x08)

static const uint32_t UART_BAUD = 9600;

// =============================================================================
// Private variables
// =============================================================================
static bool uart_initialized = false;

static volatile uint8_t rx_buff[BUFFER_SIZE];
static volatile uint8_t tx_buff[BUFFER_SIZE];

static volatile uint16_t rx_buff_first = 0;
static volatile uint16_t rx_buff_last = 0;
static volatile uint16_t tx_buff_first = 0;
static volatile uint16_t tx_buff_last = 0;

static volatile uint16_t rx_buff_size = 0;
static volatile uint16_t tx_buff_size = 0;

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Starts transmission of the tx buffer.
 * @param void
 * @return void
 */
static void start_tx(void);

// =============================================================================
// Public function definitions
// =============================================================================

void uart_init()
{
    volatile int16_t wait_cnt = 0;

    if (false == uart_initialized)
    {
        //
        // Variables
        //
        rx_buff_first = 0;
        rx_buff_last = 0;
        tx_buff_first = 0;
        tx_buff_last = 0;

        rx_buff_size = 0;
        tx_buff_size = 0;

        g_uart_receive_event = false;

        //
        // IO ports
        //
        UART_TX_PIN_DIRECTION = DIRECTION_OUTPUT;
        UART_RX_PIN_DIRECTION = DIRECTION_INPUT;

        __builtin_write_OSCCONL(OSCCON & 0xbf);     // Unlock PPS registers

        UART_TX_OUTPUT_MAPPING = PPS_OUT_SRC_U1TX;
        UART_RX_INPUT_MAPPING = UART_RX_RP_PIN_NBR;

         __builtin_write_OSCCONL(OSCCON | 0x40);    // Lock PPS registers

        //
        // UART module
        //
        U1MODE = 0x0000;
        U1STA = 0x0000;

        U1BRG = (PERIPHERAL_FREQ / UART_BAUD) / 16 - 1;

        U1MODEbits.PDSEL = 0; // 8 bit data, no parity
        U1MODEbits.STSEL = 0; // 1 Stop bit

        // Interrupt is generated when any character is transfered to the
        // Transmit Shift Register and the hw transmit buffer is empty.
        U1STAbits.UTXISEL0 = 0;
        U1STAbits.UTXISEL1 = 0;
        IPC3bits.U1TXIP = 2;  // Interrupt priority
        IEC0bits.U1TXIE = 1;  // TX interrupt enable

        // Interrupt is generated each time a data word is transfered from
        // the U1RSR to the receive buffer. There may be one or more characters
        // in the receive buffer.
        U1STAbits.URXISEL = 0;
        IPC2bits.U1RXIP = 2;  // Interrupt priority
        IEC0bits.U1RXIE = 1;  // RX interrupt enable

        U1MODEbits.UARTEN = 1;
        U1STAbits.UTXEN = 1;
        U1STAbits.URXEN = 1;

        for (wait_cnt = 0; wait_cnt != FOSC_FREQ / UART_BAUD; ++wait_cnt)
        {
            ;
        }

        uart_initialized = true;
    }
}

void uart_write(uint8_t data)
{
    if ((0 == tx_buff_size) && (0 == U1STAbits.UTXBF))
    {
        // hw transmit buffer not full but tx buffer is.
        U1TXREG = data;
    }
    else if (tx_buff_size < BUFFER_SIZE)
    {
        if (0 != tx_buff_size)
        {
            ++tx_buff_last;

            if (tx_buff_last >= BUFFER_SIZE)
            {
                tx_buff_last = 0;
            }
        }

        tx_buff[tx_buff_last] = data;

        ++tx_buff_size;
    }
}

void uart_write_string(const char* data)
{
    const uint8_t* p = (const uint8_t*)data;
    
    // Update the tx buffer.
    while (*p && (tx_buff_size < BUFFER_SIZE))
    {
        if (0 != tx_buff_size)
        {
            ++tx_buff_last;

            if (tx_buff_last >= BUFFER_SIZE)
            {
                tx_buff_last = 0;
            }
        }

        tx_buff[tx_buff_last] = *(p++);

        ++tx_buff_size;
    }

    start_tx();
}

void uart_write_array(uint16_t nbr_of_bytes, const uint8_t* data)
{
    uint16_t i;

    // Update the tx buffer.
    for (i = 0; i != nbr_of_bytes; ++i)
    {
        if (tx_buff_size < BUFFER_SIZE)
        {
            if (0 != tx_buff_size)
            {
                ++tx_buff_last;

                if (tx_buff_last >= BUFFER_SIZE)
                {
                    tx_buff_last = 0;
                }
            }

            tx_buff[tx_buff_last] = *(data++);

            if (tx_buff_size < BUFFER_SIZE)
            {
                ++tx_buff_size;
            }
        }
    }

    start_tx();
}

uint8_t uart_get(uint16_t index)
{
    uint16_t i;
    uint8_t data;

    IEC0bits.U1RXIE = 0;

    i = rx_buff_first + index;
    
    if (i >= BUFFER_SIZE)
    {
        i -= BUFFER_SIZE;
    }

    data = rx_buff[i];

    IEC0bits.U1RXIE = 1;

    return data;
}

uint16_t uart_get_receive_buffer_size(void)
{
    return rx_buff_size;
}

bool uart_is_receive_buffer_empty(void)
{
    return (0 == rx_buff_size);
}

void uart_clear_receive_buffer(void)
{
    IEC0bits.U1RXIE = 0;

    rx_buff_size = 0;
    rx_buff_first = 0;
    rx_buff_last = 0;
    
    IEC0bits.U1RXIE = 1;
}


// =============================================================================
// Private function definitions
// =============================================================================

void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
{
    while ((0 == U1STAbits.UTXBF) && (0 != tx_buff_size))
    {
        // TX fifo not full and there are more things to send
        U1TXREG = tx_buff[tx_buff_first];

        if (1 != tx_buff_size)
        {
            ++tx_buff_first;
        }

        if (tx_buff_first >= BUFFER_SIZE)
        {
            tx_buff_first = 0;
        }

        --tx_buff_size;
    }

    IFS0bits.U1TXIF = 0;
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
{
    uint8_t received;

    IEC0bits.U1TXIE = 0;

    if (U1STAbits.OERR)
    {
        U1STAbits.OERR = 0;
    }

    while (U1STAbits.URXDA)
    {
        g_uart_receive_event = true;

        received = U1RXREG;

        if (BACKSPACE_CHAR != received)
        {
            if (0 != rx_buff_size)
            {
                ++rx_buff_last;

                if (rx_buff_last >= BUFFER_SIZE)
                {
                    rx_buff_last = 0;
                }
            }

            rx_buff[rx_buff_last] = received;

            ++rx_buff_size;
        }
        else
        {
            if (1 < rx_buff_size)
            {

                if (0 != rx_buff_last)
                {
                    --rx_buff_last;
                }
                else
                {
                    rx_buff_last = BUFFER_SIZE - 1;
                }
            }

            if (0 != rx_buff_size)
            {
                --rx_buff_size;
            }
        }
        

        uart_write(received);
    }
    
    IEC0bits.U1TXIE = 1;
    
    IFS0bits.U1RXIF = 0;
}

static void start_tx(void)
{
    IEC0bits.U1TXIE = 0;
    IEC0bits.U1RXIE = 0;

    while ((0 != tx_buff_size) && (0 == U1STAbits.UTXBF))
    {
        U1TXREG = tx_buff[tx_buff_first];

        if (1 != tx_buff_size)
        {
            ++tx_buff_first;
        }

        if (tx_buff_first >= BUFFER_SIZE)
        {
            tx_buff_first = 0;
        }

        --tx_buff_size;
    }

    IEC0bits.U1TXIE = 1;
    IEC0bits.U1RXIE = 1;
}

