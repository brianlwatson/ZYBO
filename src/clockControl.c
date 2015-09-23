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
#include<supportFiles/utils.h>

#include "clockDisplay.h"
#include "clockControl.h"

uint32_t adTimer= 0;
uint32_t autoTimer = 0;
uint32_t rateTimer = 0;
uint32_t secondTimer = 0;
uint32_t actionCounter = 0;



enum clockControl_States{cC_init_st, cC_waiting_for_touch_st, cC_ad_timer_running_st, cC_auto_timer_running_st, cC_rate_timer_running_st, cC_rate_timer_expired_st} cC_State;

void clockControl_tick()
{
	uint16_t ad_Timer_expired_value = 2;
	uint16_t auto_Timer_expired_value = 10;
	uint16_t rate_Timer_expired_value = 1;

	switch(cC_State)
	{
		case cC_init_st:
			cC_State = cC_waiting_for_touch_st;
			break;


		case cC_waiting_for_touch_st:

			secondTimer++;
			if(actionCounter > 0 && (secondTimer % 20 == 0))
			{
			clockDisplay_advanceTimeOneSecond();
			}

			adTimer = 0;
			autoTimer = 0;
			rateTimer = 0;
			if(display_isTouched())
			{
				display_clearOldTouchData();
				cC_State = cC_ad_timer_running_st;
			}
			break;



		case cC_ad_timer_running_st:
			adTimer++;
			if(display_isTouched() && adTimer == ad_Timer_expired_value)
			{
				actionCounter++;
				cC_State = cC_auto_timer_running_st;
			}
			if(!display_isTouched() && adTimer == ad_Timer_expired_value)
			{
				clockDisplay_performIncDec();
				cC_State = cC_waiting_for_touch_st;
			}
			break;


		case cC_auto_timer_running_st:
			autoTimer++;
			if(display_isTouched() && autoTimer == auto_Timer_expired_value)
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

			if(display_isTouched() && rateTimer == rate_Timer_expired_value)
			{
				actionCounter++;
				cC_State = cC_rate_timer_expired_st;
			}
			break;

		case cC_rate_timer_expired_st:
			rateTimer = 0;

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





