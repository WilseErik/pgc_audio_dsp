/*
 * This file forms a hardware abstraction layer for the DMA module
 * which is uesd in order to move data between perhipherials and memory without
 * involving the CPU.
 *
 * References:
 *  - PIC24FJ128GA202 datasheet, document number DS30010038C
 *  - dsPIC33/PIC24 Family Reference Manual, Direct Memory Access controller,
 *    document number   DS39742A
 */


// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>
#include <stdbool.h>

#include <xc.h>

#include "audio.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================
volatile int16_t g_audio_tx_buff = 0;

// =============================================================================
// Private constants
// =============================================================================

// =============================================================================
// Private variables
// =============================================================================
static volatile int16_t dma_tx_buff = 0;

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Initializes the DMA module.
 * @param void
 * @return void
 */
void dma_init(void);

// =============================================================================
// Public function definitions
// =============================================================================

void dma_i2s_ch_init(void)
{
    dma_init();

    DMACH0bits.CHEN = 0;    // Disable channel 0

    DMASRC0 = (uint16_t)&dma_tx_buff;
    DMADST0 = (uint16_t)&SPI2BUFL;

    DMACNT0 = 2;            // 2 transfers before an interrupt is generated (LR)
    DMACH0bits.SIZE = 0;    // 16 bit transfer
    DMACH0bits.TRMODE = 1;  // Repeated one-shot mode
    DMACH0bits.SAMODE = 0;  // DMASRC unchanged after a transfer completion
    DMACH0bits.DAMODE = 0;  // DMADST unchanged after a transfer completion
    DMACH0bits.RELOAD = 1;
    
    DMACH0bits.CHEN = 1;

    DMAINT0bits.CHSEL = 0x1E;   // Use SPI2 Transmitt as trigger event

    IFS0bits.DMA0IF = 0;
    IPC1bits.DMA0IP = 6;
    IEC0bits.DMA0IE = 1;

    SPI2BUFL = 0;           // Start the first transfer
}

void dma_i2s_ch_deinit(void)
{
    IEC0bits.DMA0IE = 0;
    DMACH0bits.CHEN = 0;
}

// =============================================================================
// Private function definitions
// =============================================================================

void dma_init(void)
{
    DMACONbits.DMAEN = 1;   // Enable the DMA
    DMACONbits.PRSSEL = 0;  // Fixed priority scheme

    DMAL = (uint16_t)&dma_tx_buff - 4;
    DMAH = (uint16_t)&dma_tx_buff + 4;
}

void __attribute((interrupt, no_auto_psv)) _DMA0Interrupt()
{
    if (false == audio_is_sample_buffer_empty())
    {
        dma_tx_buff = audio_pop_sample();
    }

    DMAINT0 &= 0xFF00;      // Clear the interrupt flags

    IFS2bits.SPI2TXIF = 0;  // Clear the trigger source
    IFS0bits.DMA0IF = 0;
}

