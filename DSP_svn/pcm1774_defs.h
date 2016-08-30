/* 
 * File:   pcm1774_defs.h
 * Author: Erik
 *
 * Created on den 23 december 2015, 02:00
 *
 * This header contains definitions and types for the digital to analog
 * converter and headphone amplifier PCM1774.
 *
 * For reference, see the document:
 * SLAS551 JULY 2007
 *
 * Abriviations used:
 * PG - Analog input buffer
 * HPA - HeadPhone Amplifier
 * DAC - Digital to Analog Converter
 */

#ifndef PCM1774_DEFS_H
#define	PCM1774_DEFS_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================

// =============================================================================
// Public type definitions
// =============================================================================

typedef enum pcm1774_reg_addr_t
{
    // Volume for HPA (L-ch)
    PCM1774_REG_HPA_VOL_LCH                 = (uint8_t)0x40,

    // Volume for HRA (R-ch)
    PCM1774_REG_HPA_VOL_RCH                 = (uint8_t)0x41,

    // DAC digital attenuation and soft mute (L-ch)
    PCM1774_REG_DATT_LCH                    = (uint8_t)0x44,

    // DAC digital attenuation and soft mute (R-ch)
    PCM1774_REG_DATT_RCH                    = (uint8_t)0x45,

    // DAC over sampling, de-emphasis, audio interface, DGC
    PCM1774_REG_OSAMP_DEEMP_AUDINTER_DGC    = (uint8_t)0x46,

    // Analog mixer power up/down
    PCM1774_REG_AMIX_PWR                    = (uint8_t)0x48,

    // DAC and HPA power up/down
    PCM1774_REG_DAC_HPA_PWR                 = (uint8_t)0x49,

    // Analog output configuration select
    PCM1774_REG_AOUT_CONF_SEL               = (uint8_t)0x4A,

    // HPA insertion detection, short protection
    PCM1774_REG_HPA_INSDET_SHPROT           = (uint8_t)0x4B,

    // Shut down status read back
    PCM1774_REG_SHUT_DOWN_STAT              = (uint8_t)0x4D,

    // PG1, 2, 5, 6, power up/down
    PCM1774_REG_PG_PWR                      = (uint8_t)0x52,

    // Master mode
    PCM1774_REG_MSTR_MODE                   = (uint8_t)0x54,

    // System reset, sampling rate control, data swap
    PCM1774_REG_SYS_RES_SAMP_CTRL_DATA_SWAP = (uint8_t)0x55,

    //BCK configuration, sampling rate control, zero-cross
    PCM1774_REG_BCK_CNFG_SAMP_CTRL_ZCROSS   = (uint8_t)0x56,

    // Analog input select (MUX1, 2, 3, 4)
    PCM1774_REG_AIN_SEL                     = (uint8_t)0x57,

    // Analog mixing switch (SW1, 2, 3, 4, 5, 6)
    PCM1774_REG_AMIX_SW                     = (uint8_t)0x58,

    // Analog to analog path (PG5, 6) gain
    PCM1774_REG_ATOA_PATH_GAIN              = (uint8_t)0x59,

    // Microphone boost
    PCM1774_REG_MIC_BOOST                   = (uint8_t)0x5A,

    // Bass boost gain level
    PCM1774_REG_BASS_BOOST                  = (uint8_t)0x5C,

    // Middle boost gain level
    PCM1774_REG_MIDDLE_BOOST                = (uint8_t)0x5D,

    // Treble boost gain level
    PCM1774_REG_TREBLE_BOOST                = (uint8_t)0x5E,

    // Sound effect source select, 3D sound
    PCM1774_REG_SOUND_FX_SRC_SEL            = (uint8_t)0x5F,

    // digital monaural mixing
    PCM1774_REG_DIG_MONUARL_MIXING          = (uint8_t)0x60,

    // PG1/PG2 additional Gain
    PCM1774_REG_PG_ADD_GAIN                 = (uint8_t)0x7C,

    // Power up/down time control
    PCM1774_REG_PWR_TIME_CTRL               = (uint8_t)0x7D
} pcm1774_reg_addr_t;

// =============================================================================
// Global variable declarations
// =============================================================================

// =============================================================================
// Global constatants
// =============================================================================

// ----------------------------------- //
// PCM1774_REG_HPA_VOL_LCH
// ----------------------------------- //
//      RESERVERD                (0x80)
#define PCM1774_BITS_HMUL_MASK   (0x40)
#define PCM1774_BITS_HLV5_MASK   (0x20)
#define PCM1774_BITS_HLV4_MASK   (0x10)
#define PCM1774_BITS_HLV3_MASK   (0x08)
#define PCM1774_BITS_HLV2_MASK   (0x04)
#define PCM1774_BITS_HLV1_MASK   (0x02)
#define PCM1774_BITS_HLV0_MASK   (0x01)

//      RESERVERD                (7)
#define PCM1774_BITS_HMUL_POS    (6)
#define PCM1774_BITS_HLV5_POS    (5)
#define PCM1774_BITS_HLV4_POS    (4)
#define PCM1774_BITS_HLV3_POS    (3)
#define PCM1774_BITS_HLV2_POS    (2)
#define PCM1774_BITS_HLV1_POS    (1)
#define PCM1774_BITS_HLV0_POS    (0)

#define PCM1774_BITS_HLV_MASK    (0x3F)
#define PCM1774_BITS_HLR_POS     (0)

// ----------------------------------- //
// PCM1774_REG_HPA_VOL_RCH
// ----------------------------------- //
//      RESERVERD                (0x80)
#define PCM1774_BITS_HMUR_MASK   (0x40)
#define PCM1774_BITS_HRV5_MASK   (0x20)
#define PCM1774_BITS_HRV4_MASK   (0x10)
#define PCM1774_BITS_HRV3_MASK   (0x08)
#define PCM1774_BITS_HRV2_MASK   (0x04)
#define PCM1774_BITS_HRV1_MASK   (0x02)
#define PCM1774_BITS_HRV0_MASK   (0x01)

//      RESERVERD                (7)
#define PCM1774_BITS_HMUL_POS    (6)
#define PCM1774_BITS_HRV5_POS    (5)
#define PCM1774_BITS_HRV4_POS    (4)
#define PCM1774_BITS_HRV3_POS    (3)
#define PCM1774_BITS_HRV2_POS    (2)
#define PCM1774_BITS_HRV1_POS    (1)
#define PCM1774_BITS_HRV0_POS    (0)


#define PCM1774_BITS_HRV_MASK    (0x3F)
#define PCM1774_BITS_HRV_POS     (0)

// ----------------------------------- //
// PCM1774_REG_DATT_LCH
// ----------------------------------- //
//      RESERVERD                (0x80)
#define PCM1774_BITS_PMUL_MASK   (0x40)
#define PCM1774_BITS_ATL5_MASK   (0x20)
#define PCM1774_BITS_ATL4_MASK   (0x10)
#define PCM1774_BITS_ATL3_MASK   (0x08)
#define PCM1774_BITS_ATL2_MASK   (0x04)
#define PCM1774_BITS_ATL1_MASK   (0x02)
#define PCM1774_BITS_ATL0_MASK   (0x01)
#define PCM1774_BITS_ATL_MASK    (0x3F)

//      RESERVERD                (7)
#define PCM1774_BITS_PMUL_POS    (6)
#define PCM1774_BITS_ATL5_POS    (5)
#define PCM1774_BITS_ATL4_POS    (4)
#define PCM1774_BITS_ATL3_POS    (3)
#define PCM1774_BITS_ATL2_POS    (2)
#define PCM1774_BITS_ATL1_POS    (1)
#define PCM1774_BITS_ATL0_POS    (0)

// ----------------------------------- //
// PCM1774_REG_DATT_RCH
// ----------------------------------- //
//      RESERVERD                (0x80)
#define PCM1774_BITS_PMUL_MASK   (0x40)
#define PCM1774_BITS_ATR5_MASK   (0x20)
#define PCM1774_BITS_ATR4_MASK   (0x10)
#define PCM1774_BITS_ATR3_MASK   (0x08)
#define PCM1774_BITS_ATR2_MASK   (0x04)
#define PCM1774_BITS_ATR1_MASK   (0x02)
#define PCM1774_BITS_ATR0_MASK   (0x01)
#define PCM1774_BITS_ATR_MASK    (0x3F)

//      RESERVERD                (7)
#define PCM1774_BITS_PMUL_POS    (6)
#define PCM1774_BITS_ATR5_POS    (5)
#define PCM1774_BITS_ATR4_POS    (4)
#define PCM1774_BITS_ATR3_POS    (3)
#define PCM1774_BITS_ATR2_POS    (2)
#define PCM1774_BITS_ATR1_POS    (1)
#define PCM1774_BITS_ATR0_POS    (0)

// ----------------------------------- //
// PCM1774_REG_OSAMP_DEEMP_AUDINTER_DGC 
// ----------------------------------- //
#define PCM1774_BITS_DEM1_MASK   (0x80)
#define PCM1774_BITS_DEM0_MASK   (0x40)
#define PCM1774_BITS_PFM1_MASK   (0x20)
#define PCM1774_BITS_PFM0_MASK   (0x10)
#define PCM1774_BITS_SPX1_MASK   (0x08)
#define PCM1774_BITS_SPX0_MASK   (0x04)
//      RESERVERD                (0x02)
#define PCM1774_BITS_OVER_MASK   (0x01)

#define PCM1774_BITS_DEM1_POS    (7)
#define PCM1774_BITS_DEM0_POS    (6)
#define PCM1774_BITS_PFM1_POS    (5)
#define PCM1774_BITS_PFM0_POS    (4)
#define PCM1774_BITS_SPX1_POS    (3)
#define PCM1774_BITS_SPX0_POS    (2)
//      RESERVERD                (1)
#define PCM1774_BITS_OVER_POS    (0)

#define PCM1774_BITS_DEM_MASK    (0xC0)
#define PCM1774_BITS_PFM_MASK    (0x30)
#define PCM1774_BITS_SPX_MASK    (0xC0)
#define PCM1774_BITS_DEM_POS     (6)
#define PCM1774_BITS_PFM_POS     (4)
#define PCM1774_BITS_SPX_POS     (2)

// ----------------------------------- //
// PCM1774_REG_AMIX_PWR
// ----------------------------------- //
//      RESERVERD                (0x80)
//      RESERVERD                (0x40)
//      RESERVERD                (0x20)
//      RESERVERD                (0x10)
//      RESERVERD                (0x08)
//      RESERVERD                (0x04)
#define PCM1774_BITS_PMXR_MASK   (0x20)
#define PCM1774_BITS_PMXL_MASK   (0x10)

//      RESERVERD                (7)
//      RESERVERD                (6)
//      RESERVERD                (5)
//      RESERVERD                (4)
//      RESERVERD                (3)
//      RESERVERD                (2)
#define PCM177_BITS_PMXR_POS     (1)
#define PCM177_BITS_PMXL_POS     (0)

// ----------------------------------- //
// PCM1774_REG_DAC_HPA_PWR
// ----------------------------------- //
#define PCM1774_BITS_PBIS_MASK   (0x80)
#define PCM1774_BITS_PDAR_MASK   (0x40)
#define PCM1774_BITS_PDAL_MASK   (0x20)
//      RESERVED                 (0x10)
#define PCM1774_BITS_PHPR_MASK   (0x08)
#define PCM1774_BITS_PHPL_MASK   (0x04)
//      RESERVED                 (0x02)
//      RESERVED                 (0x01)


#define PCM1774_BITS_PBIS_POS    (7)
#define PCM1774_BITS_PDAR_POS    (6)
#define PCM1774_BITS_PDAL_POS    (5)
//      RESERVED                 (4)
#define PCM1774_BITS_PHPR_POS    (3)
#define PCM1774_BITS_PHPL_POS    (2)
//      RESERVED                 (1)
//      RESERVED                 (0)

// ----------------------------------- //
// PCM1774_REG_AOUT_CONF_SEL
// ----------------------------------- //
//      RESEVED                  (0x80)
//      RESEVED                  (0x40)
//      RESEVED                  (0x20)
//      RESEVED                  (0x10)
#define PCM1774_BITS_HPS1_MASK   (0x08)
#define PCM1774_BITS_HPS0_MASK   (0x04)
//      RESEVED                  (0x02)
#define PCM1774_BITS_PCOM_MASK   (0x01)

//      RESEVED                  (7)
//      RESEVED                  (6)
//      RESEVED                  (5)
//      RESEVED                  (4)
#define PCM1774_BITS_HPS1_POS    (3)
#define PCM1774_BITS_HPS0_POS    (2)
//      RESEVED                  (1)
#define PCM1774_BITS_PCOM_POS    (0)

#define PCM1774_BITS_HPS_MASK    (0x0C)
#define PCM1774_BITS_HPS_POS     (2)

// ----------------------------------- //
// PCM1774_REG_HPA_INSDET_SHPROT
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
#define PCM1774_BITS_SDHR_MASK   (0x08)
#define PCM1774_BITS_SDHL_MASK   (0x04)
//      RESERVED                 (0x02)
//      RESERVED                 (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
#define PCM1774_BITS_SDHR_POS    (3)
#define PCM1774_BITS_SDHL_POS    (2)
//      RESERVED                 (1)
//      RESERVED                 (0)

// ----------------------------------- //
// PCM1774_REG_SHUT_DOWN_STAT
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
#define PCM1774_BITS_STHR_MASK   (0x08)
#define PCM1774_BITS_STHL_MASK   (0x04)
//      RESERVED                 (0x02)
//      RESERVED                 (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
#define PCM1774_BITS_STHR_POS    (3)
#define PCM1774_BITS_STHL_POS    (2)
//      RESERVED                 (1)
//      RESERVED                 (0)

// ----------------------------------- //
// PCM1774_REG_PG_PWR
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
#define PCM1774_BITS_PAIR_MASK   (0x20)
#define PCM1774_BITS_PAIL_MASK   (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
//      RESERVED                 (0x02)
//      RESERVED                 (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
#define PCM1774_BITS_PAIR_POS    (5)
#define PCM1774_BITS_PAIL_POS    (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
//      RESERVED                 (1)
//      RESERVED                 (0)

// ----------------------------------- //
// PCM1774_REG_MSTR_MODE
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
//      RESERVED                 (0x08)
#define PCM1774_BITS_MSTR_MASK   (0x04)
//      RESERVED                 (0x02)
#define PCM1774_BITS_BIT0_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
//      RESERVED                 (3)
#define PCM1774_BITS_MSTR_POS    (2)
//      RESERVED                 (1)
#define PCM1774_BITS_BIT0_POS    (0)

// ----------------------------------- //
// PCM1774_REG_SYS_RES_SAMP_CTRL_DATA_SWAP
// ----------------------------------- //
#define PCM1774_BITS_SRST_MASK   (0x80)
#define PCM1774_BITS_LRPC_MASK   (0x40)
#define PCM1774_BITS_NPR5_MASK   (0x20)
#define PCM1774_BITS_NPR4_MASK   (0x10)
#define PCM1774_BITS_NPR3_MASK   (0x08)
#define PCM1774_BITS_NPR2_MASK   (0x04)
#define PCM1774_BITS_NPR1_MASK   (0x02)
#define PCM1774_BITS_NPR0_MASK   (0x01)

#define PCM1774_BITS_SRST_POS    (7)
#define PCM1774_BITS_LRPC_POS    (6)
#define PCM1774_BITS_NPR5_POS    (5)
#define PCM1774_BITS_NPR4_POS    (4)
#define PCM1774_BITS_NPR3_POS    (3)
#define PCM1774_BITS_NPR2_POS    (2)
#define PCM1774_BITS_NPR1_POS    (1)
#define PCM1774_BITS_NPR0_POS    (0)

#define PCM1774_BITS_NPR_MASK    (0x3F)
#define PCM1774_BITS_NPR_POS     (0)

// ----------------------------------- //
// PCM1774_REG_BCK_CNFG_SAMP_CTRL_ZCROSS
// ----------------------------------- //
#define PCM1774_BITS_MBST_MASK   (0x80)
#define PCM1774_BITS_MSR2_MASK   (0x40)
#define PCM1774_BITS_MSR1_MASK   (0x20)
#define PCM1774_BITS_MSR0_MASK   (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
//      RESERVED                 (0x02)
#define PCM1774_BITS_ZCRS_MASK   (0x01)

#define PCM1774_BITS_MBST_POS    (7)
#define PCM1774_BITS_MSR2_POS    (6)
#define PCM1774_BITS_MSR1_POS    (5)
#define PCM1774_BITS_MSR0_POS    (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
//      RESERVED                 (1)
#define PCM1774_BITS_ZCRS_POS    (0)

#define PCM1774_BITS_MSR_MASK    (0x70)
#define PCM1774_BITS_MSR_POS     (4)

// ----------------------------------- //
// PCM1774_REG_AIN_SEL
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
#define PCM1774_BITS_AIR1_MASK   (0x20)
#define PCM1774_BITS_AIR0_MASK   (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
#define PCM1774_BITS_AIL1_MASK   (0x02)
#define PCM1774_BITS_AIL0_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
#define PCM1774_BITS_AIR1_POS    (5)
#define PCM1774_BITS_AIR0_POS    (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
#define PCM1774_BITS_AIL1_POS    (1)
#define PCM1774_BITS_AIL0_POS    (0)

#define PCM1774_BITS_AIR_MASK    (0x30)
#define PCM1774_BITS_AIL_MASK    (0x03)
#define PCM1774_BITS_AIR_POS     (4)
#define PCM1774_BITS_AIL_POS     (0)

// ----------------------------------- //
// PCM1774_REG_AMIX_SW
// ----------------------------------- //
//      RESERVED                 (0x80)
#define PCM1774_BITS_MXR2_MASK   (0x40)
#define PCM1774_BITS_MXR1_MASK   (0x20)
#define PCM1774_BITS_MXR0_MASK   (0x10)
//      RESERVED                 (0x08)
#define PCM1774_BITS_MXL2_MASK   (0x04)
#define PCM1774_BITS_MXL1_MASK   (0x02)
#define PCM1774_BITS_MXL0_MASK   (0x01)

//      RESERVED                 (7)
#define PCM1774_BITS_MXR2_POS    (6)
#define PCM1774_BITS_MXR1_POS    (5)
#define PCM1774_BITS_MXR0_POS    (4)
//      RESERVED                 (3)
#define PCM1774_BITS_MXL2_POS    (2)
#define PCM1774_BITS_MXL1_POS    (1)
#define PCM1774_BITS_MXL0_POS    (0)

#define PCM1774_BITS_MXR_MASK    (0x70)
#define PCM1774_BITS_MXL_MASK    (0x07)
#define PCM1774_BITS_MXR_POS     (4)
#define PCM1774_BITS_MXL_POS     (0)

// ----------------------------------- //
// PCM1774_REG_ATOA_PATH_GAIN
// ----------------------------------- //
//      RESERVED                 (0x80)
#define PCM1774_BITS_GMR2_MASK   (0x40)
#define PCM1774_BITS_GMR1_MASK   (0x20)
#define PCM1774_BITS_GMR0_MASK   (0x10)
//      RESERVED                 (0x08)
#define PCM1774_BITS_GML2_MASK   (0x04)
#define PCM1774_BITS_GML1_MASK   (0x02)
#define PCM1774_BITS_GML0_MASK   (0x01)

//      RESERVED                 (7)
#define PCM1774_BITS_GMR2_POS    (6)
#define PCM1774_BITS_GMR1_POS    (5)
#define PCM1774_BITS_GMR0_POS    (4)
//      RESERVED                 (3)
#define PCM1774_BITS_GML2_POS    (2)
#define PCM1774_BITS_GML1_POS    (1)
#define PCM1774_BITS_GML0_POS    (0)

#define PCM1774_BITS_GMR_MASK    (0x70)
#define PCM1774_BITS_GML_MASK    (0x07)
#define PCM1774_BITS_GMR_POS     (4)
#define PCM1774_BITS_GML_POS     (0)

// ----------------------------------- //
// PCM1774_REG_MIC_BOOST
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
#define PCM1774_BITS_G20R_MASK   (0x02)
#define PCM1774_BITS_G20L_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
#define PCM1774_BITS_G20R_POS    (1)
#define PCM1774_BITS_G20L_POS    (0)

// ----------------------------------- //
// PCM1774_REG_BASS_BOOST
// ----------------------------------- //
#define PCM1774_BITS_LPAE_MASK   (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
#define PCM1774_BITS_LGA4_MASK   (0x10)
#define PCM1774_BITS_LGA3_MASK   (0x08)
#define PCM1774_BITS_LGA2_MASK   (0x04)
#define PCM1774_BITS_LGA1_MASK   (0x02)
#define PCM1774_BITS_LGA0_MASK   (0x01)

#define PCM1774_BITS_LPAE_POS    (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
#define PCM1774_BITS_LGA4_POS    (4)
#define PCM1774_BITS_LGA3_POS    (3)
#define PCM1774_BITS_LGA2_POS    (2)
#define PCM1774_BITS_LGA1_POS    (1)
#define PCM1774_BITS_LGA0_POS    (0)

#define PCM1774_BITS_LGA_MASK    (0x1F)
#define PCM1774_BITS_LGA_POS     (0)

// ----------------------------------- //
// PCM1774_REG_MIDDLE_BOOST
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
#define PCM1774_BITS_MGA4_MASK   (0x10)
#define PCM1774_BITS_MGA3_MASK   (0x08)
#define PCM1774_BITS_MGA2_MASK   (0x04)
#define PCM1774_BITS_MGA1_MASK   (0x02)
#define PCM1774_BITS_MGA0_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
#define PCM1774_BITS_MGA4_POS    (4)
#define PCM1774_BITS_MGA3_POS    (3)
#define PCM1774_BITS_MGA2_POS    (2)
#define PCM1774_BITS_MGA1_POS    (1)
#define PCM1774_BITS_MGA0_POS    (0)

#define PCM1774_BITS_MGA_MASK    (0x1F)
#define PCM1774_BITS_MGA_POS     (0)

// ----------------------------------- //
// PCM1774_REG_TREBLE_BOOST
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
#define PCM1774_BITS_HGA4_MASK   (0x10)
#define PCM1774_BITS_HGA3_MASK   (0x08)
#define PCM1774_BITS_HGA2_MASK   (0x04)
#define PCM1774_BITS_HGA1_MASK   (0x02)
#define PCM1774_BITS_HGA0_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
#define PCM1774_BITS_HGA4_POS    (4)
#define PCM1774_BITS_HGA3_POS    (3)
#define PCM1774_BITS_HGA2_POS    (2)
#define PCM1774_BITS_HGA1_POS    (1)
#define PCM1774_BITS_HGA0_POS    (0)

#define PCM1774_BITS_HGA_MASK    (0x1F)
#define PCM1774_BITS_HGA_POS     (0)

// ----------------------------------- //
// PCM1774_REG_SOUND_FX_SRC_SEL
// ----------------------------------- //
//      RESERVED                 (0x80)
#define PCM1774_BITS_3DEN_MASK   (0x40)
//      RESERVED                 (0x20)
#define PCM1774_BITS_3FL0_MASK   (0x10)
#define PCM1774_BITS_3DP3_MASK   (0x08)
#define PCM1774_BITS_3DP2_MASK   (0x04)
#define PCM1774_BITS_3DP1_MASK   (0x02)
#define PCM1774_BITS_3DP0_MASK   (0x01)

//      RESERVED                 (7)
#define PCM1774_BITS_3DEN_POS    (6)
//      RESERVED                 (5)
#define PCM1774_BITS_3FL0_POS    (4)
#define PCM1774_BITS_3DP3_POS    (3)
#define PCM1774_BITS_3DP2_POS    (2)
#define PCM1774_BITS_3DP1_POS    (1)
#define PCM1774_BITS_3DP0_POS    (0)

#define PCM1774_BITS_3DP_MASK    (0x0F)
#define PCM1774_BITS_3DP_POS     (0)

// ----------------------------------- //
// PCM1774_REG_DIG_MONUARL_MIXING
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
//      RESERVED                 (0x02)
#define PCM1774_BITS_MXEN_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
//      RESERVED                 (1)
#define PCM1774_BITS_MXEN_POS    (0)

// ----------------------------------- //
// PCM1774_REG_PG_ADD_GAIN
// ----------------------------------- //
//      RESERVED                 (0x80)
//      RESERVED                 (0x40)
//      RESERVED                 (0x20)
//      RESERVED                 (0x10)
//      RESERVED                 (0x08)
//      RESERVED                 (0x04)
#define PCM1774_BITS_G12R_MASK   (0x02)
#define PCM1774_BITS_G12L_MASK   (0x01)

//      RESERVED                 (7)
//      RESERVED                 (6)
//      RESERVED                 (5)
//      RESERVED                 (4)
//      RESERVED                 (3)
//      RESERVED                 (2)
#define PCM1774_BITS_G12R_POS    (1)
#define PCM1774_BITS_G12L_POS    (0)

// ----------------------------------- //
// PCM1774_REG_PWR_TIME_CTRL
// ----------------------------------- //
//      RESERVED                 (0x80)
#define PCM1774_BITS_PTM1_MASK   (0x40)
#define PCM1774_BITS_PTM0_MASK   (0x20)
#define PCM1774_BITS_RES4_MASK   (0x10)
#define PCM1774_BITS_RESR3_MASK   (0x08)
#define PCM1774_BITS_RES2_MASK   (0x04)
#define PCM1774_BITS_RES1_MASK   (0x02)
#define PCM1774_BITS_RES0_MASK   (0x01)

//      RESERVED                 (7)
#define PCM1774_BITS_PTM1_POS    (6)
#define PCM1774_BITS_PTM0_POS    (5)
#define PCM1774_BITS_RES4_POS    (4)
#define PCM1774_BITS_RES3_POS    (3)
#define PCM1774_BITS_RES2_POS    (2)
#define PCM1774_BITS_RES1_POS    (1)
#define PCM1774_BITS_RES0_POS    (0)

#define PCM1774_BITS_PTM_MASK    (0x60)
#define PCM1774_BITS_RES_MASK    (0x1F)
#define PCM1774_BITS_PTM_POS     (5)
#define PCM1774_BITS_RES_POS     (0)

// =============================================================================
// Public function declarations
// =============================================================================




#ifdef	__cplusplus
}
#endif

#endif	/* PCM1774_DEFS_H */

