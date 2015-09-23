#include "simonVerifySequence.h"
#define MESSAGE_X 0
#define MESSAGE_Y (display_width()/4)
#define MESSAGE_TEXT_SIZE 2
#define MESSAGE_STARTING_OVER
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
#define TIMER_EXPIRED_VALUE 500

#define MAX_TEST_SEQUENCE_LENGTH 4  // the maximum length of the pattern
uint8_t verifySequence_testSequence[MAX_TEST_SEQUENCE_LENGTH] = {0, 1, 2, 3};  // A simple pattern.
#define MESSAGE_WAIT_MS 4000  // Display messages for this long.

bool verify_enable = true;
bool verify_complete = false;
bool verify_isTimeOutError = true;
bool verify_isUserInputError = true;
uint8_t timer_waiting = 0;
uint8_t simonRegion = -1;
uint16_t verifyValue = 0;

// State machine will run when enabled.
void verifySequence_enable()
{
	verify_enable = true;
}

// This is part of the interlock. You disable the state-machine and then enable it again.
void verifySequence_disable()
{
	verify_enable = false;
}

// Used to detect if there has been a time-out error.
bool verifySequence_isTimeOutError()
{
	return verify_isTimeOutError;
}

// Used to detect if the user tapped the incorrect sequence.
bool verifySequence_isUserInputError()
{
	return verify_isUserInputError;
}

// Used to detect if the verifySequence state machine has finished verifying.
bool verifySequence_isComplete()
{
	return verify_complete;
}

enum VerifyStates {verify_init, verify_waiting, verify_wait_release, verify_validate, verify_complete} verify_state;



// Standard tick function.
void verifySequence_tick()
{
	switch(verify_state)
	{

	case verify_init:
		verify_complete = false;
            if(verify_enable)
            {
                simonbuttonHandler_enable();
                verify_state = verify_waiting;
            }
		break;

	case verify_waiting:
            
            timer_waiting++;
            
            if(!simonButtonhandler_releaseDetected() && wait_timer < TIMER_EXPIRED_VALUE)
            {
                verify_state = verify_wait_release;
                timer_waiting = 0;
                break;
            }
            if(timer_waiting == TIMER_EXPIRED_VALUE)
            {
                verify_isTimeOutError = true;
                verify_complete = true;
                verify_state = verify_complete;
                timer_waiting = 0;
                break;
            }

	case verify_wait_release:
	
        if(simonButtonhandler_releaseDetected())
           {
               simonRegion = simonButtonHandler_getRegionNumber();
               buttonHandler_disable();
               verify_state = verify_validate;
               break;
           }
	
            
            
        case verify_validate:
        
            if(simonRegion == globals_getSequenceValue(verifyValue))
            {
                verifyValue++;
                if(verifyValue == verify_current_sequence_length()
                   {
                       verify_complete = true;
                       verifyValue = 0;
                       verify_state = verify_complete;
                       break;
                   }
                else
                   {
                       verify_state = verify_waiting;
                       simonbuttonHandler_enable();
                       
                   }
            }
                   else
                   {
                       verify_isUserInputError = true;
                       verify_complete = true;
                       verify_state = verify_complete;
                   }
                   break;
         


          case final_state:
                   timer_waiting = 0;
                   verifyValue = 0;
                   break;
                   

	default:
            verify_state = verify_init; break;

	}
}

// Prints the instructions that the user should follow when
// testing the verifySequence state machine.
// Takes an argument that specifies the length of the sequence so that
// the instructions are tailored for the length of the sequence.
// This assumes a simple incrementing pattern so that it is simple to
// instruct the user.
void verifySequence_printInstructions(uint8_t length, bool startingOver) {
  display_fillScreen(DISPLAY_BLACK);	    // Clear the screen.
  display_setTextSize(MESSAGE_TEXT_SIZE);   // Make it readable.
  display_setCursor(MESSAGE_X, MESSAGE_Y);  // Rough center.
  if (startingOver) {	                    // Print a message if you start over.
    display_fillScreen(DISPLAY_BLACK);	    // Clear the screen if starting over.
    display_setTextColor(DISPLAY_WHITE);    // Print whit text.
    display_println("Starting Over. ");
  }
  display_println("Tap: ");
  display_println();
  switch (length) {
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

// Just clears the screen and draws the four buttons used in Simon.
void verifySequence_drawButtons() {
	display_fillScreen(DISPLAY_BLACK);
	simonDisplay_drawAllButtons();  // Draw the four buttons.
}

// This will set the sequence to a simple sequential pattern.

// Increment the sequence length making sure to skip over 0.
// Used to change the sequence length during the test.
int16_t incrementSequenceLength(int16_t sequenceLength) {
  int16_t value = (sequenceLength + 1) % (MAX_TEST_SEQUENCE_LENGTH+1);
  if (value == 0) value++;
    return value;
}

// Used to select from a variety of informational messages.
enum verifySequence_infoMessage_t {
  user_time_out_e,            // means that the user waited too long to tap a color.
  user_wrong_sequence_e,      // means that the user tapped the wrong color.
  user_correct_sequence_e,    // means that the user tapped the correct sequence.
  user_quit_e                 // means that the user wants to quite.
};

// Prints out informational messages based upon a message type (see above).
void verifySequence_printInfoMessage(verifySequence_infoMessage_t messageType) {
  // Setup text color, position and clear the screen.
  display_setTextColor(DISPLAY_WHITE);
  display_setCursor(MESSAGE_X, MESSAGE_Y);
  display_fillScreen(DISPLAY_BLACK);
  switch(messageType) {
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

#define BTN0 1
// Tests the verifySequence state machine.
// It prints instructions to the touch-screen. The user responds by tapping the
// correct colors to match the sequence.
// Users can test the error conditions by waiting too long to tap a color or
// by tapping an incorrect color.
void verifySequence_runTest() {
  display_init();  // Always must do this.
  buttons_init();  // Need to use the push-button package so user can quit.
  int16_t sequenceLength = 1;  // Start out with a sequence length of 1.
  verifySequence_printInstructions(sequenceLength, false);  // Tell the user what to do.
  utils_msDelay(MESSAGE_WAIT_MS);  // Give them a few seconds to read the instructions.
  verifySequence_drawButtons();    // Now, draw the buttons.
  // Set the test sequence and it's length.
  globals_setSequence(verifySequence_testSequence, MAX_TEST_SEQUENCE_LENGTH);
  globals_setSequenceIterationLength(sequenceLength);
  // Enable the verifySequence state machine.
  verifySequence_enable();           // Everything is interlocked, so first enable the machine.
  while (!(buttons_read() & BTN0)) { // Need to hold button until it quits as you might be stuck in a delay.
    // verifySequence uses the buttonHandler state machine so you need to "tick" both of them.
    verifySequence_tick();  // Advance the verifySequence state machine.
    simonbuttonHandler_tick();   // Advance the buttonHandler state machine.
    utils_msDelay(1);       // Wait 1 ms.
    // If the verifySequence state machine has finished, check the result, otherwise just keep ticking both machines.
    if (verifySequence_isComplete()) {
      if (verifySequence_isTimeOutError()) {                      // Was the user too slow?
        verifySequence_printInfoMessage(user_time_out_e);         // Yes, tell the user that they were too slow.
      } else if (verifySequence_isUserInputError()) {             // Did the user tap the wrong color?
        verifySequence_printInfoMessage(user_wrong_sequence_e);   // Yes, tell them so.
      } else {
        verifySequence_printInfoMessage(user_correct_sequence_e); // User was correct if you get here.
      }
      utils_msDelay(MESSAGE_WAIT_MS);                            // Allow the user to read the message.
      sequenceLength = incrementSequenceLength(sequenceLength);  // Increment the sequence.
      globals_setSequenceIterationLength(sequenceLength);  // Set the length for the verifySequence state machine.
      verifySequence_printInstructions(sequenceLength, true);    // Print the instructions.
      utils_msDelay(MESSAGE_WAIT_MS);                            // Let the user read the instructions.
      verifySequence_drawButtons();                              // Draw the buttons.
      verifySequence_disable();                                  // Interlock: first step of handshake.
      verifySequence_tick();                                     // Advance the verifySequence machine.
      utils_msDelay(1);                                          // Wait for 1 ms.
      verifySequence_enable();                                   // Interlock: second step of handshake.
      utils_msDelay(1);                                          // Wait 1 ms.
    }
  }
  verifySequence_printInfoMessage(user_quit_e);  // Quitting, print out an informational message.
}
