
// =============================================================================
// Include statements
// =============================================================================
#include <math.h>
#include <stdint.h>

#include "midi.h"

// =============================================================================
// Private type definitions
// =============================================================================

// =============================================================================
// Global variables
// =============================================================================
double g_midi_frequencies[MIDI_FREQUENCIES_SIZE];

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

void midi_freq_table_init(void)
{
    int16_t i;

    double a4_freq_hz = 440;

    for (i = 0; i != MIDI_FREQUENCIES_SIZE; ++i)
    {
        g_midi_frequencies[i] = a4_freq_hz * pow(2, (i - MIDI_NOTE_A4) / (double)12.0);
    }
}

// =============================================================================
// Private function definitions
// =============================================================================

