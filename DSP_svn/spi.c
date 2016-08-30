// =============================================================================
// Include statements
// =============================================================================
#include <xc.h>

#include <stdint.h>
#include <stdbool.h>

#include "spi.h"
#include "pinmap.h"
#include "dma.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================

// =============================================================================
// Private constants
// =============================================================================

// =============================================================================
// Private variables
// =============================================================================
static bool spi1_initialized = false;
static bool spi2_initialized = false;
static bool spi3_initialized = false;

// =============================================================================
// Private function declarations
// =============================================================================

/**
 * @brief Initializes the spi1 module.
 * @details The SPI1 module is configured for the PCM1774
 * @param void
 * @return void
 */
void spi1_init(void);

/**
 * @brief Executes a SPI write through the SPI1 module.
 * @param data - The data to send.
 */
void spi1_write(uint16_t data);

/**
 * @brief Initializes the spi2 module.
 * @details The module is configured for the PCM1774 I2S interface.
 * @param void
 * @return void
 */
void spi2_init(void);

/**
 * @brief Initializes the spi3 module.
 * @details The module is configured for PIC32 interface.
 * @param void
 * @return void
 */
void spi3_init(void);


// =============================================================================
// Public function definitions
// =============================================================================

void spi_init(spi_device_t device)
{
    switch (device)
    {
    case SPI_DEVICE_PCM1774_I2S:
        spi2_init();
        dma_i2s_ch_init();
        break;

    case SPI_DEVICE_PIC32:
        spi3_init();
        break;

    case SPI_DEVICE_PCM1774:
        spi1_init();
        break;

    }
}

uint16_t spi_tranceive(spi_device_t device, uint16_t data)
{
    uint16_t received = 0;

    switch (device)
    {
    case SPI_DEVICE_PCM1774_I2S:
        // TODO Add i2s function call
        break;

    case SPI_DEVICE_PIC32:
        // TODO Add PIC32 SPI function call
        break;

    case SPI_DEVICE_PCM1774:
        spi1_write(data);
        break;
    default:
        break;
    }

    return received;
}

void spi_write(spi_device_t device, uint16_t data)
{
    switch (device)
    {
    case SPI_DEVICE_PCM1774_I2S:
        // TODO Add i2s function call
        break;

    case SPI_DEVICE_PIC32:
        // TODO Add PIC32 SPI function call
        break;

    case SPI_DEVICE_PCM1774:
        spi1_write(data);
        break;
    
    default:
        break;
    }
}

// =============================================================================
// Private function definitions
// =============================================================================



/**
 * From dsPIC33/PIC24 Family Reference Manual
 * Serial Peripheral Interface (SPI) with Audio Codec Support
 * Page 23
 *
 * 3.4.1 MASTER MODE OPERATION
 * Perform the following steps to set up the SPI module for Master mode operation:
 * 1. Disable the SPIx interrupts in the respective IECx register.
 * 2. Stop and reset the SPI module by clearing the SPIEN bit.
 * 3. Clear the receive buffer.
 * 4. Clear the ENHBUF bit (SPIxCON1L<0>) if using Standard Buffer mode or set the bit if
 *    using Enhanced Buffer mode.
 * 5. If SPIx interrupts are not going to be used, skip this step. Otherwise, the following
 *    additional steps are performed:
 *  a) Clear the SPIx interrupt flags/events in the respective IFSx register.
 *  b) Write the SPIx interrupt priority and sub-priority bits in the respective IPCx register.
 *  c) Set the SPIx interrupt enable bits in the respective IECx register.
 * 6. Write the Baud Rate register, SPIxBRGL.
 * 7. Clear the SPIROV bit (SPIxSTATL<6>).
 * 8. Write the desired settings to the SPIxCON1L register with MSTEN (SPIxCON1L<5>) = 1.
 * 9. Enable SPI operation by setting the SPIEN bit (SPIxCON1L<15>).
 * 10. Write the data to be transmitted to the SPIxBUFL and SPIxBUFH registers. Transmission
 */
void spi1_init(void)
{
    if (false == spi1_initialized)
    {
        // Unlock PPS registers
        __builtin_write_OSCCONL(OSCCON & 0xbf);

        DAC_SPI_CLK_OUTPUT_MAPPING = PPS_OUT_SRC_SCK1OUT;
        DAC_MOSI_OUTPUT_MAPPING = PPS_OUT_SRC_SDO1;
        N_DAC_SS_MAPPING = PPS_OUT_SRC_SS1OUT;

        // Lock PPS registers
        __builtin_write_OSCCONL(OSCCON | 0x40);

        // SPI1 control register 1 low
        SPI1CON1L = 0x0000;

        // 16 bit mode
        SPI1CON1Lbits.MODE16 = 1;
        SPI1CON1Lbits.MODE32 = 0;
        // Input data is sampled at the middle of data output time
        SPI1CON1Lbits.SMP = 0;
        // Transmit happens on transition from active clock state to Idle clock state
        SPI1CON1Lbits.CKE = 1;
        // SSx pin is not used by the macro (SSx pin will be controlled by the port I/O)
        SPI1CON1Lbits.SSEN = 0;
        // Idle state for clock is a low level; active state is a high level
        SPI1CON1Lbits.CKP = 0;
        // Master mode
        SPI1CON1Lbits.MSTEN = 1;
        // SDI1 pin is controlled by the module
        SPI1CON1Lbits.DISSDI = 1;
        // SCK1 pin is controlled by the module
        SPI1CON1Lbits.DISSCK = 0;
        // Peripheral Clock (PBCLK) ? FCY is used by the BRG
        SPI1CON1Lbits.MCLKEN = 0;
        // Enhanced Buffer Mode is disabled
        SPI1CON1Lbits.ENHBUF = 0;

        SPI1CON2Lbits.WLENGTH = 0x0F; // 16-bit data length

        // The SCK frequency is:
        // FSCK = F_PB / (2 * (SPI1BRG + 1)) = 16MHz / (2 * (7 + 1)) = 1MHz
        SPI1BRGL = 7;

        SPI1STATLbits.SPIROV = 0;

        SPI1CON1Lbits.SPIEN = 1;

        spi1_initialized = true;
    }
}
/**
 * From dsPIC33/PIC24 Family Reference Manual
 * Serial Peripheral Interface (SPI) with Audio Codec Support
 * Page 40
 *
 * 4.5.1.1 I2S Audio Slave Mode of Operation
 * Use the following steps to set up the SPI module for the I2S Audio Slave mode of operation:
 * 1. If using interrupts, disable the SPIx interrupts in the respective IECx register.
 * 2. Stop and reset the SPI module by clearing the SPIEN bit (SPIxCON1L<15>).
 * 3. Reset the SPIx Control Register 1 High, SPIxCON1H.
 * 4. Clear the receive buffer.
 * 5. Clear the ENHBUF bit (SPIxCON1L<0>) if using Standard Buffer mode or set the bit if
 *    using Enhanced Buffer mode.
 * 6. If using interrupts, the following additional steps need to be performed:
 *  a) Clear the SPIx interrupt flags/events in the respective IFSx register.
 *  b) Write the SPIx interrupt priority and sub-priority bits in the respective IPCx register.
 *  c) Set the SPIx interrupt enable bits in the respective IECx register.
 * 7. Clear the SPIROV bit (SPIxSTATL<6>).
 * 8. Write the desired settings to the SPIxCON1H register.
 *  a) AUDMOD<1:0> bits (SPIxCON1H<9:8>) = 00
 *  b) AUDEN bit (SPIxCON1H<15>) = 1
 * 9. Write the desired settings to the SPIxCON1L register:
 *  a) MSTEN (SPIxCON1L<5>) = 0
 *  b) CKP (SPIxCON1L<6>) = 1
 *  c) MODE<32,16> (SPIxCON1L<11:10>) = 0 for 16-bit audio channel data.
 *  d) Enable SPI operation by setting the SPIEN bit (SPIxCON1L<15>).
 * 10. Transmission (and reception) will start as soon as the master provides the BCLK and
 *     LRCK.
 */
void spi2_init(void)
{
    if (false == spi2_initialized)
    {
        // Unlock PPS registers
        __builtin_write_OSCCONL(OSCCON & 0xbf);

        I2S_DIN_OUTPUT_MAPPING = PPS_OUT_SRC_SDO2;
        I2S_BCK_INPUT_MAPPING = I2S_BCK_PIN_NBR;
        I2S_LRCK_INPUT_MAPPING = I2S_LRCK_PIN_NBR;

        // Lock PPS registers
        __builtin_write_OSCCONL(OSCCON | 0x40);
        
        SPI2CON1Lbits.SPIEN = 0;
        SPI2CON1H = 0x0000;
        
        SPI2CON1Lbits.ENHBUF = 0;   // Do not use the FIFO buffer

        SPI2IMSKLbits.SPITBEN = 1;  // SPIx transmit buffer empty generates an interrupt event
        SPI2IMSKLbits.SPIRBFEN = 1;

        SPI2STATLbits.SPIROV = 0;   // SPI2 Receive Overflow Status bit

        SPI2CON1Hbits.AUDMOD = 00;  // I2S mode
        SPI2CON1Hbits.AUDEN = 1;    // Audio protocol is enabled
        SPI2CON1Hbits.FRMPOL = 0;

        SPI2CON1Hbits.IGNTUR = 1;   // Transmission underrun in not an error
        SPI2CON1Hbits.IGNROV = 1;   // Receive overflow is not an error

        SPI2URDTL = 0x5555;
        SPI2URDTH = 0x5555;

        SPI2CON1Lbits.MSTEN = 0;    // Slave
        
        SPI2CON1Lbits.CKP = 1;      // Idle state for clock is a high level
        SPI2CON1Lbits.MODE = 0;     // 16 bit data

        SPI2CON1Lbits.DISSDI = 1;   // Disable SDI
        
        SPI2CON1Lbits.SPIEN = 1;    // Enable the module

        spi2_initialized = true;
    }
}


/*
 * TODO add doc ref to spi3 init
 *
 *
16.4 Enhanced Slave Mode
To set up the SPIx module for the Enhanced Buffer
Slave mode of operation:
1. Clear the SPIxBUFL and SPIxBUFH registers.
2. If using interrupts:
a) Clear the interrupt flag bits in the respective
IFSx register.
b) Set the interrupt enable bits in the
respective IECx register.
c) Write the SPIxIP<2:0> bits in the respective
IPCx register to set the interrupt priority.
3. Write the desired settings to the SPIxCON1L,
SPIxCON1H and SPIxCON2L registers with the
MSTEN bit (SPIxCON1L<5>) = 0.
4. Clear the SMP bit.
5. If the CKE bit is set, then the SSEN bit must be
set, thus enabling the SSx pin.
6. Clear the SPIROV bit (SPIxSTATL<6>).
7. Select Enhanced Buffer mode by setting the
ENHBUF bit (SPIxCON1L<0>).
8. Enable SPIx operation by setting the SPIEN bit
(SPIxCON1L<15>).
 */
void spi3_init(void)
{
if (false == spi3_initialized)
    {
        // Unlock PPS registers
        __builtin_write_OSCCONL(OSCCON & 0xbf);

        LINK_CLK_INPUT_MAPPING = LINK_CLK_RP_PIN_NBR;
        LINK_MOSI_INPUT_MAPPING = LINK_MOSI_RP_PIN_NBR;
        LINK_MISO_OUTPUT_MAPPING = PPS_OUT_SRC_SDO3;

        // Lock PPS registers
        __builtin_write_OSCCONL(OSCCON | 0x40);

        // Clear buffer registers
        SPI3BUFL = 0x0000;
        SPI3BUFH = 0x0000;

        //
        // Interrupts
        //

        // Clear interrupt flags
        IFS3bits.SPI3RXIF = 0;
        IFS5bits.SPI3TXIF = 0;
        IFS5bits.SPI3IF = 0;

        // Enable interrup souce
        IEC3bits.SPI3RXIE = 1;

        // TODO Implement the spi3_init function
        //IPC22bits.SPI3IP
#ifdef UNIMPLEMENTED

        // SPI1 control register 1 low
        SPI1CON1L = 0x0000;

        // 16 bit mode
        SPI1CON1Lbits.MODE16 = 1;
        SPI1CON1Lbits.MODE32 = 0;
        // Input data is sampled at the middle of data output time
        SPI1CON1Lbits.SMP = 0;
        // Transmit happens on transition from active clock state to Idle clock state
        SPI1CON1Lbits.CKE = 1;
        // SSx pin is not used by the macro (SSx pin will be controlled by the port I/O)
        SPI1CON1Lbits.SSEN = 0;
        // Idle state for clock is a low level; active state is a high level
        SPI1CON1Lbits.CKP = 0;
        // Master mode
        SPI1CON1Lbits.MSTEN = 1;
        // SDI1 pin is controlled by the module
        SPI1CON1Lbits.DISSDI = 1;
        // SCK1 pin is controlled by the module
        SPI1CON1Lbits.DISSCK = 0;
        // Peripheral Clock (PBCLK) ? FCY is used by the BRG
        SPI1CON1Lbits.MCLKEN = 0;
        // Enhanced Buffer Mode is disabled
        SPI1CON1Lbits.ENHBUF = 0;

        SPI1CON2Lbits.WLENGTH = 0x0F; // 16-bit data length

        // The SCK frequency is:
        // FSCK = F_PB / (2 * (SPI1BRG + 1)) = 16MHz / (2 * (7 + 1)) = 1MHz
        SPI1BRGL = 7;

        SPI1STATLbits.SPIROV = 0;

        SPI1CON1Lbits.SPIEN = 1;
#endif
        spi3_initialized = true;
    }
}

void spi1_write(uint16_t data)
{
    while (1 == SPI1STATLbits.SPIBUSY)
    {
        ; // Wait for all transactions to complete
    }

    SPI1BUFL = data;

    while (1 == SPI1STATLbits.SPIBUSY)
    {
        ; // Wait for the transmit buffer to be empty
    }
}