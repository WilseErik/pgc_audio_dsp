/* 
 * File:   midi.h
 * Author: Erik
 *
 * Created on den 10 januari 2016, 23:12
 */

#ifndef MIDI_H
#define	MIDI_H

#ifdef	__cplusplus
//extern "C" {
#endif

// =============================================================================
// Include statements
// =============================================================================

// =============================================================================
// Public type definitions
// =============================================================================
typedef enum midi_notes_t
{
    MIDI_NOTE_CMINUS1, MIDI_NOTE_CMINUS1_SHARP,
    MIDI_NOTE_DMINUS1, MIDI_NOTE_DMINUS1_SHARP,
    MIDI_NOTE_EMINUS1,
    MIDI_NOTE_FMINUS1, MIDI_NOTE_FMINUS1_SHARP,
    MIDI_NOTE_GMINUS1, MIDI_NOTE_GMINUS1_SHARP,
    MIDI_NOTE_AMINUS1, MIDI_NOTE_AMINUS1_SHARP,
    MIDI_NOTE_HMINUS1,
    MIDI_NOTE_C0, MIDI_NOTE_C0_SHARP,
    MIDI_NOTE_D0, MIDI_NOTE_D0_SHARP,
    MIDI_NOTE_E0,
    MIDI_NOTE_F0, MIDI_NOTE_F0_SHARP,
    MIDI_NOTE_G0, MIDI_NOTE_G0_SHARP,
    MIDI_NOTE_A0, MIDI_NOTE_A0_SHARP,
    MIDI_NOTE_H0,
    MIDI_NOTE_C1, MIDI_NOTE_C1_SHARP,
    MIDI_NOTE_D1, MIDI_NOTE_D1_SHARP,
    MIDI_NOTE_E1,
    MIDI_NOTE_F1, MIDI_NOTE_F1_SHARP,
    MIDI_NOTE_G1, MIDI_NOTE_G1_SHARP,
    MIDI_NOTE_A1, MIDI_NOTE_A1_SHARP,
    MIDI_NOTE_H1,
    MIDI_NOTE_C2, MIDI_NOTE_C2_SHARP,
    MIDI_NOTE_D2, MIDI_NOTE_D2_SHARP,
    MIDI_NOTE_E2,
    MIDI_NOTE_F2,
    MIDI_NOTE_F2_SHARP,
    MIDI_NOTE_G2, MIDI_NOTE_G2_SHARP,
    MIDI_NOTE_A2, MIDI_NOTE_A2_SHARP,
    MIDI_NOTE_H2,
    MIDI_NOTE_C3, MIDI_NOTE_C3_SHARP,
    MIDI_NOTE_D3, MIDI_NOTE_D3_SHARP,
    MIDI_NOTE_E3,
    MIDI_NOTE_F3, MIDI_NOTE_F3_SHARP,
    MIDI_NOTE_G3, MIDI_NOTE_G3_SHARP,
    MIDI_NOTE_A3, MIDI_NOTE_A3_SHARP,
    MIDI_NOTE_H3,
    MIDI_NOTE_C4, MIDI_NOTE_C4_SHARP,
    MIDI_NOTE_D4, MIDI_NOTE_D4_SHARP,
    MIDI_NOTE_E4,
    MIDI_NOTE_F4, MIDI_NOTE_F4_SHARP,
    MIDI_NOTE_G4, MIDI_NOTE_G4_SHARP,
    MIDI_NOTE_A4, MIDI_NOTE_A4_SHARP,
    MIDI_NOTE_H4,
    MIDI_NOTE_C5, MIDI_NOTE_C5_SHARP,
    MIDI_NOTE_D5, MIDI_NOTE_D5_SHARP,
    MIDI_NOTE_E5,
    MIDI_NOTE_F5, MIDI_NOTE_F5_SHARP,
    MIDI_NOTE_G5, MIDI_NOTE_G5_SHARP,
    MIDI_NOTE_A5, MIDI_NOTE_A5_SHARP,
    MIDI_NOTE_H5,
    MIDI_NOTE_C6, MIDI_NOTE_C6_SHARP,
    MIDI_NOTE_D6, MIDI_NOTE_D6_SHARP,
    MIDI_NOTE_E6,
    MIDI_NOTE_F6, MIDI_NOTE_F6_SHARP,
    MIDI_NOTE_G6, MIDI_NOTE_G6_SHARP,
    MIDI_NOTE_A6, MIDI_NOTE_A6_SHARP,
    MIDI_NOTE_H6,
    MIDI_NOTE_C7, MIDI_NOTE_C7_SHARP,
    MIDI_NOTE_D7, MIDI_NOTE_D7_SHARP,
    MIDI_NOTE_E7,
    MIDI_NOTE_F7, MIDI_NOTE_F7_SHARP,
    MIDI_NOTE_G7, MIDI_NOTE_G7_SHARP,
    MIDI_NOTE_A7, MIDI_NOTE_A7_SHARP,
    MIDI_NOTE_H7,
    MIDI_NOTE_C8, MIDI_NOTE_C8_SHARP,
    MIDI_NOTE_D8, MIDI_NOTE_D8_SHARP,
    MIDI_NOTE_E8,
    MIDI_NOTE_F8, MIDI_NOTE_F8_SHARP,
    MIDI_NOTE_G8, MIDI_NOTE_G8_SHARP,
    MIDI_NOTE_A8, MIDI_NOTE_A8_SHARP,
    MIDI_NOTE_H8,
    MIDI_NOTE_C9, MIDI_NOTE_C9_SHARP,
    MIDI_NOTE_D9, MIDI_NOTE_D9_SHARP,
    MIDI_NOTE_E9,
    MIDI_NOTE_F9, MIDI_NOTE_F9_SHARP,
    MIDI_NOTE_G9, MIDI_NOTE_G9_SHARP,
    MIDI_NOTE_A9, MIDI_NOTE_A9_SHARP,
    MIDI_NOTE_H9,
    MIDI_NOTE_C10, MIDI_NOTE_C10_SHARP,
    MIDI_NOTE_D10, MIDI_NOTE_D10_SHARP,
    MIDI_NOTE_E10,
    MIDI_NOTE_F10, MIDI_NOTE_F10_SHARP,
    MIDI_NOTE_G10, MIDI_NOTE_G10_SHARP,
    MIDI_NOTE_A10, MIDI_NOTE_A10_SHARP,
    MIDI_NOTE_H10
} midi_notes_t;

// =============================================================================
// Global constatants
// =============================================================================
#define MIDI_FREQUENCIES_SIZE   (128)

// =============================================================================
// Global variable declarations
// =============================================================================
extern double g_midi_frequencies[MIDI_FREQUENCIES_SIZE];

// =============================================================================
// Public function declarations
// =============================================================================

/**
 * @brief Initializes the midi frequency table;
 */
void midi_freq_table_init(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MIDI_H */

