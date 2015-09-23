/*
 * clockDisplay.c
 *
 *  Created on: Oct 4, 2014
 *      Author: watsonb
 */

#include "clockDisplay.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <xparameters.h>
#include "xil_io.h"
#include <stdio.h>
#include <supportFiles/display.h>

#include "supportFiles/utils.h"
#include "clockDisplay.h"
#include <supportFiles/utils.h>

#define DISPLAY_CLOCK_SIZE_TEXT 6

#define CLOCKDISPLAY_VALUE_SIXTY_SECONDS 60
#define CLOCKDISPLAY_VALUE_TWELVE_HOURS 43200
#define CLOCKDISPLAY_VALUE_HOURS_ROLLOVER 12
#define CLOCKDISPLAY_VALUE_ONE_HOUR 3600
#define CLOCKDISPLAY_VALUE_ONE_SECOND 1

int32_t seconds = 0;
#define CLOCKDISPLAY_Y_OFFSET 8
#define CLOCKDISPLAY_X1_OFFSET -24
#define CLOCKDISPLAY_X2_OFFSET -6
#define CLOCKDISPLAY_X3_OFFSET 12
int16_t CLOCKDISPLAY_DISPLAY_CENTER_WIDTH = display_width()/2;
int16_t CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT = display_height()/2;
uint32_t seconds_value = 0;
uint32_t minutes_value = 0;
uint32_t hours_value = 0;

void clockDisplay_setHours(int16_t new_hours_value)
{
	hours_value = new_hours_value;
}

void clockDisplay_setMinutes(int16_t new_minutes_value)
{
	minutes_value = new_minutes_value;
}

void clockDisplay_setSeconds(int16_t new_seconds_value)
{
	seconds_value = new_seconds_value;
}


void clockDisplay_updateTimeDisplay(bool forceUpdateAll)
{
	//if a negative rollover occurs with hours, reset the clock to 12 hours
	if(seconds<0)
	{
		int16_t max_seconds = CLOCKDISPLAY_VALUE_TWELVE_HOURS;
		seconds = seconds + CLOCKDISPLAY_VALUE_TWELVE_HOURS;
	}

	//calculate the hours, minutes and seconds using the total seconds
	seconds_value = seconds % 60;
	clockDisplay_setSeconds(seconds_value);
	minutes_value = seconds / 60 % CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
	clockDisplay_setMinutes(minutes_value);
	hours_value = seconds / CLOCKDISPLAY_VALUE_ONE_HOUR % CLOCKDISPLAY_VALUE_HOURS_ROLLOVER + 1;
	clockDisplay_setHours(hours_value);
	static char * time = "12:59:00";

	//configure the text size and placement
	int16_t CLOCKDISPLAY_DISPLAY_CENTER_WIDTH = display_width()/2;
	int16_t CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT = display_height()/2;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_WIDTH = 5*9*DISPLAY_CLOCK_SIZE_TEXT/2;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_HEIGHT = 8*DISPLAY_CLOCK_SIZE_TEXT/2;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_CENTER_WIDTH = CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - CLOCKDISPLAY_DISPLAY_TEXT_WIDTH;
	int16_t CLOCKDISPLAY_DISPLAY_TEXT_CENTER_HEIGHT = CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT - CLOCKDISPLAY_DISPLAY_TEXT_HEIGHT;
	display_setCursor(CLOCKDISPLAY_DISPLAY_TEXT_CENTER_WIDTH, CLOCKDISPLAY_DISPLAY_TEXT_CENTER_HEIGHT);

	if(forceUpdateAll == true)
	{
		seconds = 0;
		sprintf(time, "%2u:%02u:%02u", 1,00, 00);
		display_println(time);
		return;
	}
	sprintf(time, "%2u:%02u:%02u", hours_value, minutes_value, seconds_value);
	display_println(time);
}

void clockDisplay_makeTriangle(int16_t x1, int16_t y1, bool positiveY)
{
	uint16_t x2, x3,y2,y3;

	x2 = x1 + 12*(DISPLAY_CLOCK_SIZE_TEXT);
	y2 = y1;
	x3 = x1 + 6*(DISPLAY_CLOCK_SIZE_TEXT);

	//if positiveY == true, draw the upper triangles
	if(positiveY == true)
	{
		y3 = y1 - 7*(DISPLAY_CLOCK_SIZE_TEXT);
	}
	//if positiveY == false, draw the lower triangles
	else
	{
		y3 = y1 + 7*(DISPLAY_CLOCK_SIZE_TEXT);
	}
	display_fillTriangle(x1, y1, x2, y2, x3, y3, DISPLAY_GREEN);
}

void clockDisplay_drawTriangles()
{
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X1_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X2_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X3_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH - CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, true);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X1_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X2_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
	clockDisplay_makeTriangle(CLOCKDISPLAY_DISPLAY_CENTER_HEIGHT + CLOCKDISPLAY_X3_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, CLOCKDISPLAY_DISPLAY_CENTER_WIDTH + CLOCKDISPLAY_Y_OFFSET*DISPLAY_CLOCK_SIZE_TEXT, false);
}

void clockDisplay_init()
{
	display_init();  // Must init all of the software and underlying hardware for LCD.
	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
	display_setTextColor(DISPLAY_GREEN,DISPLAY_BLACK);
	display_setTextSize(DISPLAY_CLOCK_SIZE_TEXT);
	clockDisplay_drawTriangles();
	clockDisplay_updateTimeDisplay(true);
}

void clockDisplay_advanceTimeOneSecond()  // Advances the time forward by 1 second.
{
	seconds = seconds + 1;
	clockDisplay_updateTimeDisplay(false);
}

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
		seconds +=CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
		utils_msDelay(1);
		clockDisplay_updateTimeDisplay(false);
	}
	//test hours
	for(int x = 0; x < 4; x++)
	{
		seconds += (CLOCKDISPLAY_VALUE_SIXTY_SECONDS*CLOCKDISPLAY_VALUE_SIXTY_SECONDS);
		utils_msDelay(1);
		clockDisplay_updateTimeDisplay(false);
	}
}

void clockDisplay_performIncDec()
{
	 int16_t x= 0;
	 int16_t y=0;
	 uint8_t z=0;

	 uint32_t hours_marker = hours_value;
	 uint32_t minutes_marker = minutes_value;

	 display_getTouchedPoint(&x,&y,&z);

	   if(y > display_height()/2)
	   {
		   if(x < display_width()/3)
		   {
			   seconds -= CLOCKDISPLAY_VALUE_ONE_HOUR;
			   clockDisplay_updateTimeDisplay(false);
		   }
		   else if(x > display_width() * 2/3)
		   {
			   if((seconds_value % 60) == 0)
			   {
				   seconds += CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
			   }
			   seconds -= CLOCKDISPLAY_VALUE_ONE_SECOND;
			   clockDisplay_updateTimeDisplay(false);
		   }
		   else
		   {
			   if(minutes_value == 0)
			   {
				   seconds += CLOCKDISPLAY_VALUE_ONE_HOUR;
			   }
			  	 seconds -= CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
			   clockDisplay_updateTimeDisplay(false);
		   }
	   }
	   else
	   {
		   if(x < display_width()/3)
		   {
			   seconds += CLOCKDISPLAY_VALUE_ONE_HOUR;
			   clockDisplay_updateTimeDisplay(false);
		   }
		   else if(x > display_width() * 2/3)
		   {
			   seconds += CLOCKDISPLAY_VALUE_ONE_SECOND;
			   if((seconds % 60) == 0)
			   {
				   seconds -= CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
			   }
			   clockDisplay_setMinutes(minutes_marker);
			   clockDisplay_updateTimeDisplay(false);
		   }
		   else
		   {
			   if(minutes_value == 59)
			   {
				   seconds -= CLOCKDISPLAY_VALUE_ONE_HOUR;
			   }
			   seconds += CLOCKDISPLAY_VALUE_SIXTY_SECONDS;
			   clockDisplay_updateTimeDisplay(false);
		   }
	   }
	   display_clearOldTouchData();
}
