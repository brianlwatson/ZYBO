//simonDisplay.c
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
#define SIMON_BUTTON_WIDTH 60
#define SIMON_BUTTON_WIDTH_HALVES SIMON_BUTTON_WIDTH_HALVES
#define SIMON_BUTTON_HEIGHT 60
#define SIMON_BUTTON_HEIGHT_HALVES SIMON_BUTTON_HEIGHT/2

#define ONE_FOURTH_HEIGHT display_height()*1/4
#define THREE_FOURTHS_HEIGHT display_height()*3/4
#define ONE_FOURTH_WIDTH display_width()*1/4
#define THREE_FOURTHS_WIDTH display_width()*3/4
#define DISPLAY_WIDTH_HALVES DISPLAY_WIDTH_HALVES
#define DISPLAY_HEIGHT_HALVES DISPLAY_HEIGHT_HALVES
#define PIXEL_COORD_ZERO 0
#define TOUCHES_INIT 0

#define TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS 60 // in ms
#define MAX_STR 255
#define TEXT_SIZE_2 2
#define TEXT_SIZE_5 5

#define SIMON_REGION_0 0
#define SIMON_REGION_1 1
#define SIMON_REGION_2 2
#define SIMON_REGION_3 3
#define INVALID_REGION -1

#define TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS 60



//*********************************************************************************************
// simonDisplay_computeRegionNumber(int16_t x, int16_t y)
// @param: int16_t x, int16_t y
// Computes the region of the screen that was touched based on the parameters x and y
//*********************************************************************************************
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y)
{
    if(x < PIXEL_COORD_ZERO || y < PIXEL_COORD_ZERO)
    {
        return INVALID_REGION;
    }
    
    if(x < DISPLAY_WIDTH_HALVES)
    {
        if(y < DISPLAY_HEIGHT_HALVES)
        {
            return SIMON_REGION_0;
        }
        else
        {
            return SIMON_REGION_2;
        }
    }
    
    else
    {
        if (y < DISPLAY_HEIGHT_HALVES)
        {
            return SIMON_REGION_1;
        }
        else
        {
            return SIMON_REGION_3;
        }
    }
}



//*********************************************************************************************
// simonDisplay_drawButton(uint8_t regionNumber)
// @param: uint8_t regionNumber
// Draws a small, colored button in each screen region
//*********************************************************************************************
void simonDisplay_drawButton(uint8_t regionNumber)
{
    //if the region is invalid, do not draw
	if (regionNumber < PIXEL_COORD_ZERO)
    {
        return;
    }
    
	switch(regionNumber)
    {
            
        case 2:
            display_fillRect(ONE_WIDTH_FOURTH-(SIMON_BUTTON_WIDTH_HALVES), (THREE_FOURTHS_HEIGHT)-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_BLUE);
            break;
            
        case 0:
            display_fillRect((ONE_FOURTH_WIDTH)-(SIMON_BUTTON_WIDTH_HALVES), (ONE_FOURTH_HEIGHT)-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_RED);
            break;
            
        case 1:
            display_fillRect(THREE_FOURTHS_WIDTH-(SIMON_BUTTON_WIDTH_HALVES), ONE_FOURTH_HEIGHT-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_YELLOW);
            break;
            
        case 3:
            display_fillRect(THREE_FOURTHS_WIDTH-(SIMON_BUTTON_WIDTH_HALVES), THREE_FOURTHS_HEIGHT-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_GREEN);
            break;
	}
}


//*********************************************************************************************
// simonDisplay_drawAllButtons()
// draws a button in each of the four regions
//*********************************************************************************************
void simonDisplay_drawAllButtons()
{
	simonDisplay_drawButton(SIMON_DISPLAY_REGION_0);
	simonDisplay_drawButton(SIMON_DISPLAY_REGION_1);
	simonDisplay_drawButton(SIMON_DISPLAY_REGION_2);
	simonDisplay_drawButton(SIMON_DISPLAY_REGION_3);
}


//*********************************************************************************************
// simonDisplay_eraseButton(uint8_t regionNumber)
// @param: uint8_t regionNumber
// erases all four of the buttons
//*********************************************************************************************
void simonDisplay_eraseButton(uint8_t regionNumber)
{
	// Do nothing if the region number is negative (illegal region, off LCD screen).
	if (regionNumber < PIXEL_COORD_ZERO)
    {
        return;
    }
    
	switch(regionNumber)
    {
            
        case 2:
            display_fillRect(ONE_FOURTH_WIDTH-(SIMON_BUTTON_WIDTH_HALVES), (THREE_FOURTHS_HEIGHT)-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_BLACK);
            break;
        
        case 0:
            display_fillRect((ONE_FOURTH_WIDTH)-(SIMON_BUTTON_WIDTH_HALVES), (ONE_FOURTH_HEIGHT)-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_BLACK);
            break;
        
        case 1:
            display_fillRect(3*ONE_FOURTH_WIDTH-(SIMON_BUTTON_WIDTH_HALVES), ONE_FOURTH_HEIGHT-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_BLACK);
            break;
        
        case 3:
            display_fillRect(3*ONE_FOURTH_WIDTH-(SIMON_BUTTON_WIDTH_HALVES), THREE_FOURTHS_HEIGHT-(SIMON_BUTTON_WIDTH_HALVES), SIMON_BUTTON_WIDTH, SIMON_BUTTON_HEIGHT, DISPLAY_BLACK);
            break;
	}
}


//*********************************************************************************************
// simonDisplay_eraseAllButtons()
// erases all of the buttons
//*********************************************************************************************
void simonDisplay_eraseAllButtons()
{
	simonDisplay_eraseButton(SIMON_DISPLAY_REGION_0);
	simonDisplay_eraseButton(SIMON_DISPLAY_REGION_1);
	simonDisplay_eraseButton(SIMON_DISPLAY_REGION_2);
	simonDisplay_eraseButton(SIMON_DISPLAY_REGION_3);
}


//*********************************************************************************************
// simonDisplay_drawSquare(uint8_t regionNo, bool erase)
// @param: uint8_t regionNo, bool erase
// draws a full square in the corresponding region
//*********************************************************************************************
void simonDisplay_drawSquare(uint8_t regionNo, bool erase)
{
    // Do nothing if the region number is illegal (off LCD screen).
    if (regionNo < PIXEL_COORD_ZERO)
    {
        return;
    }
    
    switch(regionNo)
    {
        case 0:
            
            if (!erase)
            {
                display_fillRect(PIXEL_COORD_ZERO, PIXEL_COORD_ZERO, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_RED);
            }
            else
            {
                display_fillRect(PIXEL_COORD_ZERO, PIXEL_COORD_ZERO, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_BLACK);
            }
            break;
            
        case 1:
            if (!erase)
            {
                display_fillRect(DISPLAY_WIDTH_HALVES, PIXEL_COORD_ZERO, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_YELLOW);
            }
            else
            {
                display_fillRect(DISPLAY_WIDTH_HALVES, PIXEL_COORD_ZERO, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_BLACK);
            }
            break;
  
        case 2:
            if (!erase)
            {
                display_fillRect(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_BLUE);
            }
            else
            {
                display_fillRect(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_BLACK);
            }
            break;
            
        case 3:
            if (!erase)
            {
                display_fillRect(DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_GREEN);
            }
            else
            {
                display_fillRect(DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_WIDTH_HALVES, DISPLAY_HEIGHT_HALVES, DISPLAY_BLACK);
            }
            break;
   }
}


//*********************************************************************************************
// simonDisplay_runTest(uint16_t touchCount)
// @param: uint16_t touchCount
// Runs a brief demonstration of how buttons can be pressed
//The routine will continue to run until the touchCount has been reached, e.g.,
// the user has touched the pad touchCount times.
//*********************************************************************************************
void simonDisplay_runTest(uint16_t touchCount) {
  display_init();  // Always initialize the display.
  char str[MAX_STR];   // Enough for some simple printing.
  uint8_t regionNumber;
  uint16_t touches = TOUCHES_INIT;
  // Write an informational message and wait for the user to touch the LCD.
  display_fillScreen(DISPLAY_BLACK);        // clear the screen.
  display_setCursor(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES); //
  display_setTextSize(TEXT_SIZE_2);
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);
  sprintf(str, "Touch and release to start the Simon demo.");
  display_println(str);
  display_println();
  sprintf(str, "Demo will terminate after %d touches.", touchCount);
  display_println(str);
  while (!display_isTouched());       // Wait here until the screen is touched.
  while (display_isTouched());        // Now wait until the touch is released.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
  simonDisplay_drawAllButtons();      // Draw all of the buttons.
  bool touched = false;  	      // Keep track of when the pad is touched.
  int16_t x, y;  		      // Use these to keep track of coordinates.
  uint8_t z;      		      // This is the relative touch pressure.
  while (touches < touchCount) {  // Run the loop according to the number of touches passed in.
    if (!display_isTouched() && touched) {         // user has stopped touching the pad.
      simonDisplay_drawSquare(regionNumber, true); // Erase the square.
      simonDisplay_drawButton(regionNumber);	   // DISPLAY_REDraw the button.
      touched = false;															// Released the touch, set touched to false.
    } else if (display_isTouched() && !touched) {   // User started touching the pad.
      touched = true;                               // Just touched the pad, set touched = true.
      touches++;  																	// Keep track of the number of touches.
      display_clearOldTouchData();  // Get rid of data from previous touches.
      // Must wait this many milliseconds for the chip to do analog processing.
      utils_msDelay(TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS);
      display_getTouchedPoint(&x, &y, &z);                  // After the wait, get the touched point.
      regionNumber = simonDisplay_computeRegionNumber(x, y);// Compute the region number.
      simonDisplay_drawSquare(regionNumber, false);	    // Draw the square (erase = false).
    }
  }
  // Done with the demo, write an informational message to the user.
  display_fillScreen(DISPLAY_BLACK);        // clear the screen.
  display_setCursor(PIXEL_COORD_ZERO, DISPLAY_HEIGHT_HALVES); // Place the cursor in the middle of the screen.
  display_setTextSize(TEXT_SIZE_2);                   // Make it readable.
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);  // red is foreground color, black is background color.
  sprintf(str, "Simon demo terminated");    // Format a string using sprintf.
  display_println(str);                     // Print it to the LCD.
  sprintf(str, "after %d touches.", touchCount);  // Format the rest of the string.
  display_println(str);  // Print it to the LCD.
}
