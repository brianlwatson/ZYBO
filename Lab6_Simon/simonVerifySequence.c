//simonVerifySequence.c
#include "simonVerifySequence.h"
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
#include "simonDisplay.h"
#include "simonButtonHandler.h"
#include "buttons.h"
#include "supportFiles/utils.h"

#define MESSAGE_X 0
#define MESSAGE_Y (display_width()/4)
#define MESSAGE_TEXT_SIZE 2
#define MESSAGE_STARTING_OVER

#define TIMER_EXPIRED_VALUE 10
#define MESSAGE_WAIT_MS 4000  // Display messages for this long.
#define MAX_TEST_SEQUENCE_LENGTH 4
#define INIT_ZERO 0
#define INVALID_REGION -1
#define BTN0 1
#define V_DISABLED false
#define V_ENABLED true
#define ONE 1
#define ONE_MS 1
#define ZERO 0
uint8_t verifySequence_testSequence[MAX_TEST_SEQUENCE_LENGTH] = {0, 1, 2, 3};

bool verify_enable = false;
bool verify_completed = false;
bool verify_isTimeOutError =false;
bool verify_isUserInputError= false;
uint16_t timer_waiting = INIT_ZERO;
uint8_t simonRegion = INVALID_REGION;
uint16_t verifyValue = INIT_ZERO;
uint16_t verify_current_sequence_length;


//*********************************************************************************************
// verifySequence_enable()
// enables the verify sequence state machine
//*********************************************************************************************
void verifySequence_enable()
{
	verify_enable = true;
}


//*********************************************************************************************
// verifySequence_disable()
// disables the verify sequence state machine, erases the simon buttons
// disables the button handler state machine
//*********************************************************************************************
void verifySequence_disable()
{
	simonbuttonHandler_disable();
	simonDisplay_eraseAllButtons();
	verify_enable = false;
}

//*********************************************************************************************
// verifySequence_isTimeOutError
// Used to detect if there has been a time-out error.
//*********************************************************************************************
bool verifySequence_isTimeOutError()
{
	return verify_isTimeOutError;
}

//*********************************************************************************************
// verifySequence_isUserInputError()
// Used to detect if the user tapped the incorrect sequence.
//*********************************************************************************************
bool verifySequence_isUserInputError()
{
	return verify_isUserInputError;
}

//*********************************************************************************************
// verifySequence_isComplete()
// Used to detect if the verifySequence state machine has finished verifying.
//*********************************************************************************************
bool verifySequence_isComplete()
{
	return verify_completed;
}

enum VerifyStates {verify_init, verify_waiting, verify_wait_release, verify_validate, verify_complete, final_state} verify_state;


//*********************************************************************************************
// verifySequence_tick()
// verify sequence state machine
//*********************************************************************************************
void verifySequence_tick()
{
	switch(verify_state)
	{

    //verify_init
    //Resets all error cases and sets all timers to zero
    //Gets the current sequence length
    //enables the button handler
	case verify_init:
		verify_isTimeOutError = V_DISABLED;
		verify_isUserInputError = V_DISABLED;
		verify_completed = V_DISABLED;
		timer_waiting = INIT_ZERO;
		verifyValue = INIT_ZERO;
            
		verify_current_sequence_length = globals_getSequenceIterationLength();

		if(verify_enable)
		{
			simonbuttonHandler_enable();
			verify_state = verify_waiting;
		}
		break;

            
    //verify_waiting
    //provides the user with time to touch the screen
    //if the user does not touch before TIMER_EXPIRED_VALUE, verify_isTimeOutError occurs
    //disables the button handler after waiting is over
	case verify_waiting:
        timer_waiting++;
        if(timer_waiting == TIMER_EXPIRED_VALUE)
        {
            verify_isTimeOutError = V_ENABLED;
            simonbuttonHandler_disable();

            verify_state = verify_complete;
        }

        if(display_isTouched())
        {
            if(!simonbuttonHandler_releaseDetected() && timer_waiting < TIMER_EXPIRED_VALUE)
            {
                verify_state = verify_wait_release;
            }
        }
        break;

            
    //verify_wait_release
    //waits for the user to release the touch screen and disables the button handler
	case verify_wait_release:

        if(simonbuttonHandler_releaseDetected())
        {
            simonRegion = simonbuttonHandler_getRegionNumber();
            timer_waiting = INIT_ZERO;
            simonbuttonHandler_disable();
               
            verify_state = verify_validate;
        }
        break;


    //verify_validate
    //compares the verifyValue to the sequence value to see if the sequence is over
    //if they are not the same, user input error has occurred
    case verify_validate:

        if(simonRegion == globals_getSequenceValue(verifyValue))
        {
            verifyValue++;
            if(verifyValue == verify_current_sequence_length)
            {
                verifyValue = INIT_ZERO;
                verify_state = verify_complete;
            }
                
            else
            {
                timer_waiting = INIT_ZERO;
                verify_state = verify_waiting;
                simonbuttonHandler_enable();
            }
        }
            
        else
        {
            verify_isUserInputError = V_ENABLED;
            verify_state = verify_complete;
        }
        break;


    //verify_complete
    //signifies the state machine has completed
    //restarts if and only if the state machine is enabled
    case verify_complete:

        verify_completed = V_ENABLED;
        timer_waiting = INIT_ZERO;

        if(verify_enable)
        {
            verify_state = verify_init;
        }
        break;


	default:
            verify_state = verify_init;
            break;
	}
}



//*********************************************************************************************
// verifySequence_printInstructions(uint8_t length, bool startingOver)
// Prints the instructions that the user should follow when
// testing the verifySequence state machine.
// Takes an argument that specifies the length of the sequence so that
// the instructions are tailored for the length of the sequence.
// This assumes a simple incrementing pattern so that it is simple to
// instruct the user.
//*********************************************************************************************
void verifySequence_printInstructions(uint8_t length, bool startingOver)
{
    display_fillScreen(DISPLAY_BLACK);	    // Clear the screen.
    display_setTextSize(MESSAGE_TEXT_SIZE);   // Make it readable.
    display_setCursor(MESSAGE_X, MESSAGE_Y);  // Rough center.
  
    if (startingOver)// Print a message if you start over.
    {	                    
        display_fillScreen(DISPLAY_BLACK);	    // Clear the screen if starting over.
        display_setTextColor(DISPLAY_WHITE);    // Print whit text.
        display_println("Starting Over. ");
    }
    
    display_println("Tap: ");
    display_println();
  
    switch (length)
    {
        case 1:
            display_println("red");
            break;
        case 2:
            display_println("red, yellow ");
            break;
        case 3:
            display_println("red, yellow, blue ");
            break;
        case 4:
            display_println("red, yellow, blue, green ");
            break;
        
        default:
            break;
    }
  
    display_println("in that order.");
    display_println();
    display_println("hold BTN0 to quit.");
}


//*********************************************************************************************
// incrementSequenceLength(int16_t sequenceLength)
// @param: int16_t sequenceLength
// This will set the sequence to a simple sequential pattern.
// Increment the sequence length making sure to skip over 0.
// Used to change the sequence length during the test.
//*********************************************************************************************
int16_t incrementSequenceLength(int16_t sequenceLength) {
  int16_t value = (sequenceLength + ONE) % (MAX_TEST_SEQUENCE_LENGTH + ONE);
  
    if (value == ZERO)
    {
        value++;
    }
    return value;
}


//*********************************************************************************************
// verifySequence_printInfroMessage(verifySequence_infoMessage_t messageType)
// @param: verifySequence_infoMessage_t messageType
// Prints out informational messages based upon a message type (see above).
//*********************************************************************************************
void verifySequence_printInfoMessage(verifySequence_infoMessage_t messageType)
{
    display_setTextColor(DISPLAY_WHITE);
    display_setCursor(MESSAGE_X, MESSAGE_Y);
  
    switch(messageType)
    {
        case user_time_out_e:  // Tell the user that they typed too slowly.
            display_println("Error:");
            display_println();
            display_println("  User tapped sequence");
            display_println("  too slowly.");
            break;
  
        case user_wrong_sequence_e:  // Tell the user that they tapped the wrong color.
            display_println("Error: ");
            display_println();
            display_println("  User tapped the");
            display_println("  wrong sequence.");
            break;
            
        case user_correct_sequence_e:  // Tell the user that they were correct.
            display_println("User tapped");
            display_println("the correct sequence.");
            break;
            
        case user_quit_e:             // Acknowledge that you are quitting the test.
            display_println("quitting runTest().");
            break;
            
        default:
            break;
  }
}


//*********************************************************************************************
// verifySequence_eraseInfroMessage(verifySequence_infoMessage_t messageType)
// @param: verifySequence_infoMessage_t messageType
// Erases informational messages based upon a message type (see above).
//*********************************************************************************************
void verifySequence_eraseInfoMessage(verifySequence_infoMessage_t messageType)
{
    display_setTextColor(DISPLAY_BLACK);
    display_setCursor(MESSAGE_X, MESSAGE_Y);
  
    switch(messageType)
    {
        case user_time_out_e:  // Tell the user that they typed too slowly.
            display_println("Error:");
            display_println();
            display_println("  User tapped sequence");
            display_println("  too slowly.");
            break;
            
        case user_wrong_sequence_e:  // Tell the user that they tapped the wrong color.
            display_println("Error: ");
            display_println();
            display_println("  User tapped the");
            display_println("  wrong sequence.");
            break;
            
        case user_correct_sequence_e:  // Tell the user that they were correct.
            display_println("User tapped");
            display_println("the correct sequence.");
            break;
            
        case user_quit_e:             // Acknowledge that you are quitting the test.
            display_println("quitting runTest().");
            break;
            
        default:
            break;
  }
}


//*********************************************************************************************
// verifySequence_runTest()
// Tests the verifySequence state machine.
// It prints instructions to the touch-screen. The user responds by tapping the
// correct colors to match the sequence.
// Users can test the error conditions by waiting too long to tap a color or
// by tapping an incorrect color.
//*********************************************************************************************
void verifySequence_runTest()
{
    display_init();  // Always must do this.
    buttons_init();  // Need to use the push-button package so user can quit.
    int16_t sequenceLength = ONE;  // Start out with a sequence length of 1.
    verifySequence_printInstructions(sequenceLength, false);  // Tell the user what to do.
    utils_msDelay(MESSAGE_WAIT_MS);  // Give them a few seconds to read the instructions.
    simonDisplay_drawAllButtons()    // Now, draw the buttons.
    // Set the test sequence and it's length.
    globals_setSequence(verifySequence_testSequence, MAX_TEST_SEQUENCE_LENGTH);
    globals_setSequenceIterationLength(sequenceLength);
    // Enable the verifySequence state machine.
    verifySequence_enable();           // Everything is interlocked, so first enable the machine.
    while (!(buttons_read() & BTN0))// Need to hold button until it quits as you might be stuck in a delay.
    {
        // verifySequence uses the buttonHandler state machine so you need to "tick" both of them.
        verifySequence_tick();  // Advance the verifySequence state machine.
        simonbuttonHandler_tick();   // Advance the buttonHandler state machine.
        utils_msDelay(ONE_MS);       // Wait 1 ms.
        // If the verifySequence state machine has finished, check the result, otherwise just keep ticking both machines.
    if (verifySequence_isComplete())
    {
      if (verifySequence_isTimeOutError())
      {                      // Was the user too slow?
        verifySequence_printInfoMessage(user_time_out_e);         // Yes, tell the user that they were too slow.
      }
      else if (verifySequence_isUserInputError())
      {             // Did the user tap the wrong color?
        verifySequence_printInfoMessage(user_wrong_sequence_e);   // Yes, tell them so.
      }
      else
      {
        verifySequence_printInfoMessage(user_correct_sequence_e); // User was correct if you get here.
      }
      utils_msDelay(MESSAGE_WAIT_MS);                            // Allow the user to read the message.
      sequenceLength = incrementSequenceLength(sequenceLength);  // Increment the sequence.
      globals_setSequenceIterationLength(sequenceLength);  // Set the length for the verifySequence state machine.
      verifySequence_printInstructions(sequenceLength, V_ENABLED);    // Print the instructions.
      utils_msDelay(MESSAGE_WAIT_MS);                            // Let the user read the instructions.
      verifySequence_drawButtons();                              // Draw the buttons.
      verifySequence_disable();                                  // Interlock: first step of handshake.
      verifySequence_tick();                                     // Advance the verifySequence machine.
      utils_msDelay(ONE_MS);                                          // Wait for 1 ms.
      verifySequence_enable();                                   // Interlock: second step of handshake.
      utils_msDelay(ONE_MS);                                          // Wait 1 ms.
    }
  }
  verifySequence_printInfoMessage(user_quit_e);  // Quitting, print out an informational message.
}
