/*
 * clockDisplay.h
 *
 *  Created on: Oct 4, 2014
 *      Author: watsonb
 */

#ifndef CLOCKDISPLAY_H_
#define CLOCKDISPLAY_H_


void clockDisplay_init();  

void clockDisplay_updateTimeDisplay(bool forceUpdateAll);

void clockDisplay_performIncDec();

void clockDisplay_advanceTimeOneSecond();

void clockDisplay_runTest();

#endif /* CLOCKDISPLAY_H_ */
