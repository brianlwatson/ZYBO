/*
 * clockDisplay.c
 *
 *  Created on: Oct 4, 2014
 *      Author: watsonb
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xparameters.h>
#include "xil_io.h"
#include <stdio.h>
#include <supportFiles/display.h>
#include "src/buttons.h"
#include "intervalTimer.h"
#include "supportFiles/utils.h"
#include "clockDisplay.h"
#include<supportFiles/utils.h>

#define DISPLAY_CLOCK_SIZE_TEXT 6
#define DISPLAY_CLOCK_HALF_SIZE_TEXT DISPLAY_CLOCK_SIZE_TEXT/2
#define VALUES 8

#define TOTAL_WIDTH 320
#define TOTAL_HEIGHT 240

#define ORIGINAL_VALUE_WIDTH 5
#define ORIGINAL_VALUE_HEIGHT 8

#define HEIGHT_OFFSET 20
#define WIDTH_OFFSET 20

#define ONE_THIRD_WIDTH display_width()/3
#define ONE_THIRD_WIDTH display_width()*(2/3)
#define ONE_HALF_HEIGHT display_height()/2
#define ONE_HALF_WIDTH display_width()/2

#define ZERO 0
#define FIFTY_NINE_SECONDS 59
#define SIXTY_SECONDS 60
#define TWELVE_HOURS 43200
#define ROLLOVER_HOURS 12
#define ONE_HOUR 3600
#define ONE_OFFSET 1
#define ONE_SECOND 1
#define ONE_MS 1
#define PIXEL_WIDTH 5
#define PIXEL_WIDTH_OFFSET 6
#define PIXEL_HEIGHT 8
#define PIXEL_HEIGHT_OFFSET 9

static char time[8];
static int32_t seconds = 0;
static int16_t Y_OFFSET = 8;
static int16_t X1_OFFSET = -24;
static int16_t X2_OFFSET = -6;
static int16_t X3_OFFSET = 12;
static int16_t CLOCKDISPLAY_DISPLAY_CENTER_WIDTH = display_width()/2;
static int16_t CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT = display_height()/2;
static uint32_t seconds_value = 0;
static uint32_t minutes_value = 0;
static uint32_t hours_value = 0;



//**********************************************************************
// clockDisplay_setHours
// @param: new_hours_value
// Uses new_hours_value to reset the global variable hours_value
//**********************************************************************
void clockDisplay_setHours(int16_t new_hours_value)
{
	hours_value = new_hours_value;
}



//**********************************************************************
// clockDisplay_setMinutes
// @param: new_minutes_value
// Uses new_minutes_value to reset the global variable minutes_value
//**********************************************************************
void clockDisplay_setMinutes(int16_t new_minutes_value)
{
	minutes_value = new_minutes_value;
}



//**********************************************************************
// clockDisplay_setSeconds
// @param: new_seconds_value
// Uses new_seconds_value to reset the global variable seconds_value
//**********************************************************************
void clockDisplay_setSeconds(int16_t new_seconds_value)
{
	seconds_value = new_seconds_value;
}


//**********************************************************************
// clockDisplay_updateTimeDisplay
// @param: forceUpdateAll
// Calculates the dimensions of the clock
// Combines hours_value, minutes_value, seconds_value into the char[8] time
// Draws the clockDisplay on the LCD screen
// Sets the default time as 12:59:00
// Configures clock rollover based on hours_value, minutes_value, seconds_value
//**********************************************************************
void clockDisplay_updateTimeDisplay(bool forceUpdateAll)
{
	//if a negative rollover occurs with hours, reset the clock to 12 hours
	if(seconds<ZERO)
	{
		int16_t max_seconds = TWELVE_HOURS;
		seconds = seconds + TWELVE_HOURS;
	}
    
	//calculate the hours, minutes and seconds using the total seconds
	seconds_value = seconds % SIXTY_SECONDS;
	minutes_value = seconds / SIXTY_SECONDS % SIXTY_SECONDS;
	hours_value = seconds / ONE_HOUR % ROLLOVER_HOURS + ONE_SECOND;
    
    //set the seconds_value, minutes_value, hours_value globally
    clockDisplay_setSeconds(seconds_value);
    clockDisplay_setMinutes(minutes_value);
    clockDisplay_setHours(hours_value);
    
    //initialize time buffer
	static char * time = "12:59:00";
    
    //calculate the size of the text and the time buffer to be printed
	int16_t CLOCKDISPLAY_DISPLAY_CENTER_WIDTH = ONE_HALF_WIDTH;
	int16_t CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT = ONE_HALF_HEIGHT;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_WIDTH = PIXEL_WIDTH*PIXEL_HEIGHT_OFFSET*DISPLAY_CLOCK_SIZE_TEXT/;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_HEIGHT = PIXEL_HEIGHT*DISPLAY_CLOCK_HALF_SIZE_TEXT;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_CENTER_WIDTH = CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - CLOCKDISPLAY_DISPLAY_TEXT_WIDTH;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_CENTER_HEIGHT = CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT - CLOCKDISPLAY_DISPLAY_TEXT_HEIGHT;
	display_setCursor(CLOCKDISPLAY_DISPLAY_TEXT_CENTER_WIDTH, CLOCKDISPLAY_DISPLAY_TEXT_CENTER_HEIGHT);
    
	sprintf(time, "%2u:%02u:%02u", hours_value, minutes_value, seconds_value);
    
	display_println(time);
}



//**********************************************************************
// clockDisplay_makeTriangle
// @parameter: x1, y1
// @parameter: positiveY
// This function uses the parameters x1 and y1 to calculate the remaining vertices of all triangles equilaterally
//**********************************************************************
void clockDisplay_makeTriangle(int16_t x1, int16_t y1, bool positiveY)
{
	uint16_t x2, x3 ,y2,y3;
    
	x2 = x1 + PIXEL_WIDTH_OFFSET*pixel_double*(DISPLAY_CLOCK_SIZE_TEXT);
	y2 = y1;
	x3 = x1 + PIXEL_WIDTH_OFFSET*(DISPLAY_CLOCK_SIZE_TEXT);
    
	//if positiveY == true, draw the upper triangles
	if(positiveY == true)
	{
		y3 = y1 - (PIXEL_WIDTH_OFFSET+1)*(DISPLAY_CLOCK_SIZE_TEXT);
	}
    
	//if positiveY == false, draw the lower triangles
	else
	{
		y3 = y1 + PIXEL_WIDTH_OFFSET+1)*(DISPLAY_CLOCK_SIZE_TEXT);
	}
    
    //draw the calculated triangle
	display_fillTriangle(x1, y1, x2, y2, x3, y3, DISPLAY_GREEN);
}



//**********************************************************************
// clockDisplay_drawTriangles()
// draws six triangles based off of the calculated OFFSETs using clockDisplay_makeTriangle
//**********************************************************************
void clockDisplay_drawTriangles()
{
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X1_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X2_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X3_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X1_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X2_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + X3_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
}



//**********************************************************************
// clockDisplay_init()
// initializes the display, colors the background, sets the text size, draws the triangles
// prevents flashing on the screen by using DISPLAY_BLACK in the display_setTextColor
// draws the initial char[8] time on the display
//**********************************************************************
void clockDisplay_init()
{
	display_init();  // Must init all of the software and underlying hardware for LCD.
	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
	display_setTextColor(DISPLAY_GREEN,DISPLAY_BLACK);
	display_setTextSize(DISPLAY_CLOCK_SIZE_TEXT);
	clockDisplay_drawTriangles();
	clockDisplay_updateTimeDisplay(true);
}



//**********************************************************************
// clockDisplay_advanceTimeOneSecond()
// increments seconds by one
// updates the clockDisplay with the new time value
//**********************************************************************
void clockDisplay_advanceTimeOneSecond()
{
	seconds = seconds + ONE_SECOND;
	clockDisplay_updateTimeDisplay(false);
}



//**********************************************************************
// clockDisplay_performIncDec()
// increments or decrements the hours, minutes, or seconds values depending on the touched region
// draws the new time value on the display
//**********************************************************************
void clockDisplay_performIncDec()
{
    int16_t x;
    int16_t y;
    uint8_t z;
    
    display_getTouchedPoint(&x,&y,&z);
    
    //Find the top half of the display to control the decrementation
    if(y > ONE_HALF_HEIGHT)
    {
        //Decrement the hours
        if(x < ONE_THIRD_WIDTH)
        {
            seconds -= ONE_HOUR;
            clockDisplay_updateTimeDisplay(false);
        }
        
        //Decrement the seconds
        else if(x > ONE_THIRD_WIDTH)
        {
            //Prevent negative rollover
            if((seconds_value % SIXTY_SECONDS) == ZERO)
            {
                seconds += SIXTY_SECONDS;
            }
            
            seconds -= ONE_SECOND;
            clockDisplay_updateTimeDisplay(false);
        }
        
        //Decrement the minutes
        else
        {
            //Prevent negative rollover
            if(minutes_value == ZERO)
            {
                seconds += ONE_HOUR;
            }
            
            seconds -= SIXTY_SECONDS;
            clockDisplay_updateTimeDisplay(false);
        }
    }
    
    //Control the incrementation
    else
    {
        //Increment the hours
        if(x < ONE_THIRD_WIDTH)
        {
            seconds += ONE_HOUR;
            clockDisplay_updateTimeDisplay(false);
        }
        
        //Increment the Seconds
        else if(x > ONE_THIRD_WIDTH)
        {
            seconds += ONE_SECOND;
            
            //Prevent positive rollover
            if((seconds % SIXTY_SECONDS) == ZERO)
            {
                seconds -= SIXTY_SECONDS;
            }
            
            clockDisplay_setMinutes(minutes_marker);
            clockDisplay_updateTimeDisplay(false);
        }
        
        //Increment the Minutes
        else
        {
            //Prevent positive rollover
            if(minutes_value == FIFTY_NINE_SECONDS)
            {
                seconds -= ONE_HOUR;
            }
            seconds += SIXTY_SECONDS;
            clockDisplay_updateTimeDisplay(false);
        }
        
    }
    
    display_clearOldTouchData();
}



//**********************************************************************
// clockDisplay_runTest()
// tests the Display and the rollover to ensure that clock functionality is correct
//**********************************************************************
void clockDisplay_runTest()
{
	//test seconds
	for(int i = 0; i < 1; i++)
	{
		clockDisplay_advanceTimeOneSecond();
		utils_msDelay(1);
	}
    
	//test minutes
	for(int y = 0; y < 1; y++)
	{
		seconds +=SIXTY_SECONDS;
		utils_msDelay(ONE_MS);
		clockDisplay_updateTimeDisplay(false);
	}
    
	//test hours
	for(int x = 0; x < 4; x++)
	{
		seconds += (SIXTY_SECONDS*SIXTY_SECONDS);
		utils_msDelay(ONE_MS);
		clockDisplay_updateTimeDisplay(false);
	}
}

































