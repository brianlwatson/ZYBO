//simonButtonHandler.c
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
#include "simonButtonHandler.h"

#define RUN_TEST_TERMINATION_MESSAGE1 "buttonHandler_runTest()"
#define RUN_TEST_TERMINATION_MESSAGE2 "terminated."
#define RUN_TEST_TEXT_SIZE 2

#define SIMON_BUTTON_WIDTH 60
#define SIMON_DISPLAY_GETREGION -1
#define SIMON_DISPLAY_REGION_0 0
#define SIMON_DISPLAY_REGION_1 1
#define SIMON_DISPLAY_REGION_2 2
#define SIMON_DISPLAY_REGION_3 3
#define TOUCHES_INIT
#define PIXEL_COORD_ZERO 0
#define ONE_MS

#define DISPLAY_WIDTH_HALVES display_width()/2
#define DISPLAY_HEIGHT_HALVES display_height()/2

#define AD_TIMER_EXPIRED_VALUE 1
#define AD_TIMER_INIT 0
#define RUN_TEST_INIT 0
#define BOOL_INIT false
#define ENABLED true
#define DISABLED false

uint16_t touches = TOUCHES_INIT;
uint8_t region = SIMON_DISPLAY_GETREGION;
uint16_t adTimer = AD_TIMER_INIT;

bool enable = false;
bool release = true;
bool erase = false;



//*********************************************************************************************
// simonbuttonHandler(getRegionNumber()
// determines which region of the screen is touched
// converts and returns the touched point into a simon Region Number
//*********************************************************************************************
uint8_t simonbuttonHandler_getRegionNumber()
{
	int16_t x;
	int16_t y;
	uint8_t z;
	display_getTouchedPoint(&x,&y,&z);

	if (x < PIXEL_COORD_ZERO || y < PIXEL_COORD_ZERO)
    {
	    return SIMON_DISPLAY_GETREGION;
    }

	  if (x < DISPLAY_WIDTH_HALVES)
	  {
		  if (y < DISPLAY_HEIGHT_HALVES)
		  {
			  return SIMON_DISPLAY_REGION_0;
		  }

		  else
          {
			  return SIMON_DISPLAY_REGION_2;
          }
	  }

	  else
      {
	      if (y < DISPLAY_HEIGHT_HALVES)
          {
	    	  return SIMON_DISPLAY_REGION_1;
          }
	      else
          {
	    	  return SIMON_DISPLAY_REGION_3;
          }
      }
}


//*********************************************************************************************
// simonbuttonHandler_enable()
// enables the button handler state machine
//*********************************************************************************************
void simonbuttonHandler_enable()
{
	enable = true;
}


//*********************************************************************************************
// simonbuttonHandler_disable()
// disables the button handler state machine
//*********************************************************************************************
void simonbuttonHandler_disable()
{
	enable = false;
}


//*********************************************************************************************
// simonbuttonHandler_releaseDetected()
// detects a release of the touch screen
//*********************************************************************************************
bool simonbuttonHandler_releaseDetected()
{
	return release;
}

//*********************************************************************************************
// simonbuttonHandler_runTest(int16_t touchCountArt)
// @param: int16_t touchCountArt
// Runs the button handler until touchCountArg touches are achieved
//*********************************************************************************************
void simonbuttonHandler_runTest(int16_t touchCountArg)
{
  int16_t touchCount = RUN_TEST_INIT;             // Keep track of the number of touches.
  display_init();                     // Always have to init the display.
  display_fillScreen(DISPLAY_BLACK);  // Clear the display.
  simonDisplay_drawAllButtons();      // Draw the four buttons.
  simonbuttonHandler_enable();
  while (touchCount < touchCountArg) {  // Loop here while touchCount is less than the touchCountArg
    simonbuttonHandler_tick();               // Advance the state machine.
    utils_msDelay(ONE_MS);			// Wait here for 1 ms.
    if (simonbuttonHandler_releaseDetected()) {  // If a release is detected, then the screen was touched.
      touchCount++;                         // Keep track of the number of touches.
      printf("button released: %d\n\r", simonbuttonHandler_getRegionNumber());  // Get the region number that was touched.
      simonbuttonHandler_disable();             // Interlocked behavior: handshake with the button handler (now disabled).
      utils_msDelay(ONE_MS);                     // wait 1 ms.
      simonbuttonHandler_tick();                 // Advance the state machine.
      simonbuttonHandler_enable();               // Interlocked behavior: enable the buttonHandler.
      utils_msDelay(ONE_MS);                     // wait 1 ms.
      simonbuttonHandler_tick();                 // Advance the state machine.
    }
  }
  display_fillScreen(DISPLAY_BLACK);			// clear the screen.
  display_setTextSize(RUN_TEST_TEXT_SIZE);		// Set the text size.
  display_setCursor(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES);		// Move the cursor to a rough center point.
  display_println(RUN_TEST_TERMINATION_MESSAGE1);	// Print the termination message on two lines.
  display_println(RUN_TEST_TERMINATION_MESSAGE2);
}


enum simonButtonHandlerStates {sbh_init, sbh_draw, sbh_ad_timer, sbh_waiting_for_release, sbh_complete} sbh_state;


//*********************************************************************************************
// simonbuttonHandler_tick()
// button handler state machine
//*********************************************************************************************
void simonbuttonHandler_tick()
{

		switch(sbh_state)
		{
            //sbh_init
            //Initializes values and draws the buttons
			case sbh_init:
				release = BOOL_INIT;
				adTimer = AD_TIMER_INIT;
				region = SIMON_DISPLAY_GETREGION;

				if(enable == ENABLED)
				{
					simonDisplay_drawAllButtons();
					sbh_state = sbh_ad_timer;
				}
				break;

            
            //sbh_ad_timer
            //Debounces the touches
            //If the button handler is not enabled, transitions to completed
			case sbh_ad_timer:
                if(!enable)
				{
					sbh_state = sbh_complete;
				}

				if(display_isTouched())
				{
					adTimer++;
				}

				if(adTimer == AD_TIMER_EXPIRED_VALUE)
				{
					sbh_state = sbh_draw;
					display_clearOldTouchData();
					adTimer = AD_TIMER_INIT;
				}
				break;

                
            //sbh_draw
            //draws the square in the region of the user's touch
			case sbh_draw:
				display_clearOldTouchData();
				region = simonbuttonHandler_getRegionNumber();
				simonDisplay_drawSquare(region, erase);
				erase = ENABLED;
				release = DISABLED;
                
				sbh_state = sbh_waiting_for_release;
				break;

                
            //sbh_waiting_for_release
            //waits for the user to release his/her touch
			case sbh_waiting_for_release:
				if(!display_isTouched())
				{
					simonDisplay_drawSquare(region, ENABLED);
					simonDisplay_drawButton(region);
					erase = DISABLED;
					release = ENABLED;
					sbh_state = sbh_complete;
				}
				break;

                
            //sbh_complete
            // sets the button handler's completed bool to true
            // returns to init if the button handler is enabled
			case sbh_complete:
				erase = DISABLED;

				if(enable)
				{
					sbh_state = sbh_init;
				}

				break;


			default:
				sbh_state= sbh_init;
		}
}


