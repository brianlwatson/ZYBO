//globals.c
#include <stdio.h>
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include <stdbool.h>
#include <stdint.h>
#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "xparameters.h"
#include "globals.h"

#define STARTING_SIZE 100

uint16_t SequenceIterationLength;
uint16_t SequenceLength;
int8_t sequencelist[STARTING_SIZE];

//*********************************************************************************************
//globals_setSequence(const uint8_t sequence[], uint16_t length)
//@param: const uint8_t sequence[], uint16_t length
// Sets the global array of sequences
// This is the length of the complete sequence at maximum length.
//*********************************************************************************************
void globals_setSequence(const uint8_t sequence[], uint16_t length)
{
	for(int i = 0; i < length; i++)
	{
		sequencelist[i] = sequence[i];
	}
}


//*********************************************************************************************
// globals_getSequenceValue(uint16_t index)
// @param: uint16_t index
// This returns the value of the sequence at the index.
//*********************************************************************************************
uint8_t globals_getSequenceValue(uint16_t index)
{
	return sequencelist[index];
}


//*********************************************************************************************
// globals_getSequenceLength()
// Retrieve the sequence length.
//*********************************************************************************************
uint16_t globals_getSequenceLength()
{
	return SequenceLength;
}


//*********************************************************************************************
// globals_setSequenceIterationLength(uint16_t length)
// @param: uint16_t length
// This is the length of the sequence that you are currently working on.
//*********************************************************************************************
void globals_setSequenceIterationLength(uint16_t length)
{
	SequenceIterationLength = length;
}


//*********************************************************************************************
// globals_getSequenceIterationLength
// This is the length of the sequence that you are currently working on (not the max length)
// as the player is working through the color sequence
//*********************************************************************************************
uint16_t globals_getSequenceIterationLength()
{

	return SequenceIterationLength;
}
