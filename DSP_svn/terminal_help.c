/*
This file is an auto generated file.
Do not modify its contents manually!
*/
#include <string.h>
#include <stddef.h>
#include "uart.h"
void terminal_help(char* in)
{
    if (NULL != strstr(in, "system reset"))
    {
        uart_write_string("\tForces a software reset.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "exit"))
    {
        uart_write_string("\tExits the terminal and resumes normal program operation.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "analog mode"))
    {
        uart_write_string("\tActivates the analog inputs of the DAC.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "pcm1774 init"))
    {
        uart_write_string("\tPerforms the intitialization rutine for the DAC.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "trigger dma0"))
    {
        uart_write_string("\tTriggers a DMA transfer on the DMA0 channel.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "note on"))
    {
        uart_write_string("\tTurns one note on.\n\r\tParmeters: <channel number> <note number> <velocity>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "note off"))
    {
        uart_write_string("\tTurns one note off.\n\r\tParameters: <channel number> <note number>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "all notes off"))
    {
        uart_write_string("\tTurns all notes off.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get spi1 status"))
    {
        uart_write_string("\tGets register states from the SPI1 module.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get spi2 status"))
    {
        uart_write_string("\tGets register states from the SPI2 module.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get dma0 status"))
    {
        uart_write_string("\tGets register states from the DMA0 module.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get sample buffer size"))
    {
        uart_write_string("\tGets the size of the sample buffer.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get square0 status"))
    {
        uart_write_string("\tGets the current status of the audio channel square0.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get square1 status"))
    {
        uart_write_string("\tGets the current status of the audio channel square1.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "get triangle0 status"))
    {
        uart_write_string("\tGets the current status of the audio channel triangle0.\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set pcm1774 reg"))
    {
        uart_write_string("\tSets the contents of a register in the DAC.\n\r\tParameters: <register index in hex> <register value in hex>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set main volume"))
    {
        uart_write_string("\tSets the main volume of the headphone amplifier.\n\r\tParamters: <volume setting in range [0, 63]>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set duty"))
    {
        uart_write_string("\tSets the duty cycle of a square or trangle audio channel.\n\r\tParameters: <audio channel number> <duty cycle in range [0, 255]>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set vibrato config"))
    {
        uart_write_string("\tConfigures the vibrato of one square/triangle channel.\n\r\tParameters: <audio channel number> <vibrato rate> <vibrato depth>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set vibrato on"))
    {
        uart_write_string("\tTurns the vibrato on for one audio channel.\n\r\tParameters: <audio channel number>\n\r\t\n\r");
    }
    else if (NULL != strstr(in, "set vibrato off"))
    {
        uart_write_string("\tTurns the vibrato off for one audio channel.\n\r\tParameters: <audio channel number>\n\r\t\n\r");
    }
    else
    {
        uart_write_string("\tType \"help <command>\" for more info\n\r");
        uart_write_string("\tAvailible commands:\n\r");
        uart_write_string("\t------------------------------------\n\r");
        uart_write_string("\tall notes off\n\r\tanalog mode\n\r\texit\n\r\tget dma0 status\n\r\tget sample buffer size\n\r\tget spi1 status\n\r\tget spi2 status\n\r\tget square0 status\n\r\tget square1 status\n\r\tget triangle0 status\n\r\tnote off\n\r\tnote on\n\r\tpcm1774 init\n\r\tset duty\n\r\tset main volume\n\r\tset pcm1774 reg\n\r\tset vibrato config\n\r\tset vibrato off\n\r\tset vibrato on\n\r\tsystem reset\n\r\ttrigger dma0\n\r\t");
        uart_write_string("\n\r");
    }
}
