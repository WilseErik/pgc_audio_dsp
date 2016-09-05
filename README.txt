This is the firmware project for the audio DSP on the PGC (Portable Game Console) project.

===========
   Goal
===========
The goal of this project is to enable the DSP to receive MIDI-messages and synthesize the required notes. The device should be able
to handle 4 channels, consisting of 2 square channels, 1 triangle  channel and one noise channel. Furthermore the device should also
support vibrato, ADSR volume envelopes, portamento and arpeggios for at least the square channels.

=============
System design
=============
Audio samples are calculated as a superposition of its channels. The samples are then pushed into a FIFO queue where they are poped by
the DMA module (Direct Memory Access). The DMA module then sends the samples to the SPI modules from which the samples are send over the
I2S interface to the DAC (Digital to Analog Converter).

For debugging and testing there is a UART terminal @9600 baud. Type "open terminal" over the UART interface to open it. Then use the
"help" command for more info.
