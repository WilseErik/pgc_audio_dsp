/*
 * This file forms a hardware abstraction layer for the delta-sigma
 * digital to analog converter PCM1774.
 *
 * References:
 * PCM1774 datasheet, document number SLAS551
 */
// =============================================================================
// Include statements
// =============================================================================
#include <stdint.h>

#include "pcm1774.h"
#include "pcm1774_defs.h"
#include "pinmap.h"
#include "spi.h"


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

// =============================================================================
// Private function declarations
// =============================================================================

// =============================================================================
// Public function definitions
// =============================================================================


/**
 * From the PCM1774 datasheet
 * Page 13
 *
 * Recomended power up sequence:
 * 
 * step - register settings - note
 * -------------------------------
 * 1 ? Turn on all power supplies(1)
 * 2 4027h Headphone amplifier L-ch volume (?6 dB)(2)
 * 3 4127h Headphone amplifier R-ch volume (?6 dB)(2)
 * 6 4427h Digital attenuator L-ch (?24 dB)(2)
 * 7 4527h Digital attenuator R-ch (?24 dB) (2)
 * 8 4620h DAC audio interface format (left-justified)(3)
 * 12 49E0h DAC (DAL, DAR) and analog bias power up
 * 13 5601h Zero-cross detection enable
 * 14 4803h Analog mixer (MXL, MXR) power up
 * 15 5811h Analog mixer input (SW2, SW5)R, HPC) power up
 * 18 4A01h VCOM power up select
 * 16 49ECh Headphone amplifier (HPL, HP
 * 19 5230h Analog front end (D2S, MCB, PG1, 2, 5, 6) power up
 * 20 5711h Analog input (MUX3, MUX4) select. Analog input (MUX1, MUX2) select
 */
void pcm1774_init(void)
{
    spi_init(SPI_DEVICE_PCM1774);
    
    // Headphone amplifier volume (+5 dB)
    pcm1774_write_reg(PCM1774_REG_HPA_VOL_LCH, 0x27);
    pcm1774_write_reg(PCM1774_REG_HPA_VOL_RCH, 0x27);

    // Digital attenuator (-24 dB)
    pcm1774_write_reg(PCM1774_REG_DATT_LCH, 0x27);
    pcm1774_write_reg(PCM1774_REG_DATT_RCH, 0x27);

    // I2S format, digital gain 0 dB
    pcm1774_write_reg(PCM1774_REG_OSAMP_DEEMP_AUDINTER_DGC, 0x00);

    // DAC (DAL, DAR) and analog bias power up
    pcm1774_write_reg(PCM1774_REG_DAC_HPA_PWR, 0xE0);

    // Zero-cross detection enable
    pcm1774_write_reg(PCM1774_REG_BCK_CNFG_SAMP_CTRL_ZCROSS, 0x01);

    // Analog mixer (MXL, MXR) power up
    pcm1774_write_reg(PCM1774_REG_AMIX_PWR, 0x03);

    // Analog mixer input (SW2, SW5) select
    pcm1774_write_reg(PCM1774_REG_AMIX_SW, 0x11);

    // Headphone amplifier (HPL, HPR, HPC) power up
    pcm1774_write_reg(PCM1774_REG_AOUT_CONF_SEL, 0x01);

    // Analog front end (D2S, MCB, PG1, 2, 5, 6) power up
    pcm1774_write_reg(PCM1774_REG_PG_PWR, 
                      PCM1774_BITS_PAIR_MASK | PCM1774_BITS_PAIL_MASK);

    // Analog input (MUX3, MUX4) select. Analog input (MUX1, MUX2) select
    pcm1774_write_reg(PCM1774_REG_AIN_SEL, 0x11);

    // 48kHz sampling rate
    pcm1774_write_reg(PCM1774_REG_BCK_CNFG_SAMP_CTRL_ZCROSS,
                      (2 << PCM1774_BITS_MSR_POS) | PCM1774_BITS_ZCRS_MASK);
    // I2S master mode
    pcm1774_write_reg(PCM1774_REG_MSTR_MODE,
                      PCM1774_BITS_MSTR_MASK | PCM1774_BITS_BIT0_MASK);

    // DAC (DAL, DAR), analog bias and headphone amp output power up
    pcm1774_write_reg(PCM1774_REG_DAC_HPA_PWR, 0xEC);

    spi_init(SPI_DEVICE_PCM1774_I2S);
}

void pcm1774_analog_to_analog_mode(void)
{
    pcm1774_write_reg(PCM1774_REG_DAC_HPA_PWR, 0xEC);

    // Analog mixer input (SW1, SW3) select
    pcm1774_write_reg(PCM1774_REG_AMIX_SW,
                      PCM1774_BITS_MXR1_MASK | PCM1774_BITS_MXL1_MASK);

    // Analog front end (D2S, MCB, PG1, 2, 5, 6) power up
    pcm1774_write_reg(PCM1774_REG_PG_PWR,
                      PCM1774_BITS_PAIR_MASK | PCM1774_BITS_PAIL_MASK);

    // Analog input (MUX3, MUX4) select. Analog input (MUX1, MUX2) select
    pcm1774_write_reg(PCM1774_REG_AIN_SEL, 0x11);

    pcm1774_write_reg(PCM1774_REG_ATOA_PATH_GAIN,
                      (0x07 << PCM1774_BITS_GMR_POS) |
                      (0x07 << PCM1774_BITS_GML_POS));
}

void pcm1774_set_volume(uint8_t volume)
{
    pcm1774_write_reg(PCM1774_REG_HPA_VOL_LCH, volume & 0x3F);
    pcm1774_write_reg(PCM1774_REG_HPA_VOL_RCH, volume & 0x3F);
}

/**
 * @details
 * Register writes are done by sending a word where the 8 most
 * significant bits are the register index and the other 8 least significant
 * bits are the data to write to that register.
 */
void pcm1774_write_reg(pcm1774_reg_addr_t reg_index, uint8_t data)
{
    uint16_t data_to_send = (reg_index << 8) | data;

    N_DAC_SS_ON;
    spi_write(SPI_DEVICE_PCM1774, data_to_send);
    N_DAC_SS_OFF;
}


// =============================================================================
// Private function definitions
// =============================================================================


