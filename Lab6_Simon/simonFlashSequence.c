//simonFlashSequence.c
#include "simonFlashSequence.h"
#include "simonDisplay.h"
#include<stdio.h>
#include<stdint.h>
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
#include "supportFiles/utils.h"
#include "globals.h"
#include "simonVerifySequence.h"

#define INITFLASHVALUE 10
#define INIT_ZERO 0
#define TWO_SECONDS 2000
#define PIXEL_COORD_ZERO 0
#define ONE 1
#define DISPLAY_HEIGHT_HALVES display_height()/2
#define TEST_SEQUENCE_LENGTH 8	// Just use a short test sequence.
uint8_t flashSequence_testSequence[TEST_SEQUENCE_LENGTH] = {SIMON_DISPLAY_REGION_0,
							    SIMON_DISPLAY_REGION_1,
							    SIMON_DISPLAY_REGION_2,
							    SIMON_DISPLAY_REGION_3,
							    SIMON_DISPLAY_REGION_3,
							    SIMON_DISPLAY_REGION_2,
							    SIMON_DISPLAY_REGION_1,
							    SIMON_DISPLAY_REGION_0};

#define INCREMENTING_SEQUENCE_MESSAGE1 "Incrementing Sequence"
#define RUN_TEST_COMPLETE_MESSAGE "Runtest() Complete"
#define MESSAGE_TEXT_SIZE 2
#define FLASH_HOLD_EXP 3
#define F_ENABLED true
#define F_DISABLED false

bool flash_enable = false;
bool flash_completed = false;
bool flash_erase = false;
uint8_t flash_string = INITFLASHVALUE;
uint8_t init_value = INIT_ZERO;
uint16_t flash_sequence_hold = INIT_ZERO;
uint8_t current_sequence_length;



//*********************************************************************************************
// flashSequence_enable()
// Turns on the state machine. Part of the interlock.
//*********************************************************************************************
void flashSequence_enable()
{
	flash_completed = false;
	flash_enable = true;
}


//*********************************************************************************************
//flashSequence_disable()
// Turns off the state machine. Part of the interlock.
//*********************************************************************************************
void flashSequence_disable()
{
	flash_enable = false;
}


//*********************************************************************************************
// flashSequence_completed()
// Other state machines can call this to determine if this state machine is finished.
//*********************************************************************************************
bool flashSequence_completed()
{
	return flash_completed;
}

enum FlashSequenceStates{flash_init, flash_getregion, flash_hold, flash_sequence, flash_complete, flash_wait} flash_state;


//*********************************************************************************************
// flashSequence_tick()
// flash sequence state machine
//*********************************************************************************************
void flashSequence_tick()
{
	switch(flash_state)
	{
            
        //flash_init
        // sets the current sequence length
        // resets global bools
		case flash_init:
			current_sequence_length = globals_getSequenceIterationLength();
			flash_completed = F_DISABLED;
			if(flash_enable)
			{
                flash_state = flash_getregion;
			}
			break;
		

        //flash_getregion
        //draws a square depending on the position of the sequence
		case flash_getregion:
			simonDisplay_drawSquare(globals_getSequenceValue(init_value), flash_erase);
			flash_erase = F_ENABLED;
            
			flash_state = flash_hold;
            break;

            
        //flash_hold
        //holds full square on the screen for FLASH_HOLD_EXP time
		case flash_hold:
			flash_sequence_hold++;
			if(flash_sequence_hold == FLASH_HOLD_EXP)
			{
                init_value++;
				simonDisplay_drawSquare(globals_getSequenceValue(init_value),flash_erase);
				flash_erase = F_DISABLED;
                flash_sequence_hold = INIT_ZERO;
                
				flash_state = flash_sequence;
			}
			break;
		

        //flash_sequence
        //determines if the sequence length has been reached
		case flash_sequence:
            if(init_value < current_sequence_length)
            {
                flash_state = flash_getregion;
            }

            if(init_value >= current_sequence_length)
            {
                flash_state = flash_complete;
            }
            break;

            
        //flash_complete
        //reinitialies values and adjusts the completed bool
		case flash_complete:
			init_value = INIT_ZERO;
			flash_completed = F_ENABLED;
			
            if(!flash_enable)
			{
				flash_state = flash_init;
			}

			break;
		

		default:
			flash_state = flash_init;
			break;

	}
}


//*********************************************************************************************
// flashSequence_printIncrementingMessage()
// Print the incrementing sequence message.
//*********************************************************************************************
void flashSequence_printIncrementingMessage()
{
    display_fillScreen(DISPLAY_BLACK);// Otherwise, tell the user that you are incrementing the sequence.
    display_setCursor(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES);	    // Roughly centered.
    display_println(INCREMENTING_SEQUENCE_MESSAGE1);  // Print the message.
    utils_msDelay(TWO_SECONDS);                              // Hold on for 2 seconds.
    display_fillScreen(DISPLAY_BLACK);		    // Clear the screen.
}

//*********************************************************************************************
// flashSequence_runTest()
// Runs the flash sequence state machine test using pre-determined sequences and lengths
//*********************************************************************************************
void flashSequence_runTest()
{
    display_init();	// We are using the display.
    display_fillScreen(DISPLAY_BLACK);	// Clear the display.
    globals_setSequence(flashSequence_testSequence, TEST_SEQUENCE_LENGTH);	// Set the sequence.
    flashSequence_enable();			        // Enable the flashSequence state machine.
    int16_t sequenceLength = ONE;	                        // Start out with a sequence of length 1.
    globals_setSequenceIterationLength(sequenceLength);	// Set the iteration length.
    display_setTextSize(MESSAGE_TEXT_SIZE);	        // Use a standard text size.
    while (1)
    {	                // Run forever unless you break.
        flashSequence_tick();	// tick the state machine.
        utils_msDelay(ONE_MS);	// Provide a 1 ms delay.
       
        if(flashSequence_completed())// When you are done flashing the sequence.
        {
            flashSequence_disable();  // Interlock by first disabling the state machine.
        }
        
        flashSequence_tick();	// tick is necessary to advance the state.
        utils_msDelay(ONE_MS);		// don't really need this here, just for completeness.
        flashSequence_enable();	// Finish the interlock by enabling the state machine.
        utils_msDelay(ONE_MS);	// Wait 1 ms for no good reason.
        sequenceLength++;	// Increment the length of the sequence.
    
        if (sequenceLength > TEST_SEQUENCE_LENGTH) // Stop if you have done the full sequence.
        {
            break;
        }
        
        flashSequence_printIncrementingMessage(); // Tell the user that you are going to the next step
        globals_setSequenceIterationLength(sequenceLength);// Set the length of the pattern.
    }
  }
    // Let the user know that you are finished.
    display_fillScreen(DISPLAY_BLACK);
    display_setCursor(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES);
    display_println(RUN_TEST_COMPLETE_MESSAGE);
}
