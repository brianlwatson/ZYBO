/*
 * clockControl.c
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
#include<supportFiles/utils.h>

#include "clockDisplay.h"
#include "clockControl.h"

#define AD_TIMER_EXPIRED_VALUE 2
#define RATE_TIMER_EXPIRED_VALUE 2
#define AUTO_TIMER_EXPIRED_VALUE 10
#define SECOND_INCREMENT_EXPIRED_VALUE 20

#define RESET 0
#define ZERO 0

static uint32_t adTimer= 0;
static uint32_t autoTimer = 0;
static uint32_t rateTimer = 0;
static uint32_t secondTimer = 0;
static uint32_t actionCounter = 0;



enum clockControl_States{cC_init_st, cC_waiting_for_touch_st, cC_ad_timer_running_st, cC_auto_timer_running_st, cC_rate_timer_running_st, cC_rate_timer_expired_st} cC_State;



//*******************************************************************************
// clockControl_tick()
// implements the state machine as given by the lab specs
// the clock will begin to run automatically after the display is touched
// after reaching the corresponding expired_value, a transition will occur
// autoTimer controls when the automatic incrementing will begin
// rateTimer controls how quickly the numbers will change
// rateTimer_expired_st allows the rateTimer to continue incrementing
// only while in the waiting_for_touch_st, the clock will increment by one second
//*******************************************************************************
void clockControl_tick()
{
	switch(cC_State)
	{
            
		case cC_init_st:
			cC_State = cC_waiting_for_touch_st;
			break;
            
            
		case cC_waiting_for_touch_st:
			secondTimer++;
            
            //If an action has been performed, start the clock
			if(actionCounter > ZERO && (secondTimer % SECOND_INCREMENT_EXPIRED_VALUE == ZERO))
			{
                clockDisplay_advanceTimeOneSecond();
			}
            
            //Reset the timer values
			adTimer = RESET;
			autoTimer = RESET;
			rateTimer = RESET;
            
			if(display_isTouched())
			{
				display_clearOldTouchData();
				cC_State = cC_ad_timer_running_st;
			}
			break;
            
            
            
		case cC_ad_timer_running_st:
			adTimer++;
            
			if(display_isTouched() && adTimer == AD_TIMER_EXPIRED_VALUE)
			{
				actionCounter++;
				cC_State = cC_auto_timer_running_st;
			}
            
			if(!display_isTouched() && adTimer == AD_TIMER_EXPIRED_VALUE)
			{
				clockDisplay_performIncDec();
				cC_State = cC_waiting_for_touch_st;
			}
			break;
            
            
            
		case cC_auto_timer_running_st:
			autoTimer++;
            
			if(display_isTouched() && autoTimer == AUTO_TIMER_EXPIRED_VALUE)
			{
				actionCounter++;
				cC_State = cC_rate_timer_running_st;
				clockDisplay_performIncDec();
			}
            
			if(!display_isTouched())
			{
				clockDisplay_performIncDec();
				cC_State = cC_waiting_for_touch_st;
			}
			break;
            
            
            
		case cC_rate_timer_running_st:
			rateTimer++;
            
			if(!display_isTouched())
			{
				cC_State = cC_waiting_for_touch_st;
			}
            
			if(display_isTouched() && rateTimer == RATE_TIMER_EXPIRED_VALUE)
			{
				actionCounter++;
				cC_State = cC_rate_timer_expired_st;
			}
			break;
            
            
            
		case cC_rate_timer_expired_st:
			rateTimer = RESET;
            
			if(display_isTouched())
			{
				actionCounter++;
				clockDisplay_performIncDec();
				cC_State = cC_rate_timer_running_st;
			}
            
			if(!display_isTouched())
			{
				cC_State = cC_waiting_for_touch_st;
			}
			break;
            
            
            
		default:
			cC_State = cC_init_st;
	}
}


