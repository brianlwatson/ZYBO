/*
 * interruptSimpleTestMain.c
 *
 *  Created on: Aug 30, 2014
 *      Author: hutch
 */
#include <stdio.h>
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
//#include "supportFiles/intervalTimer.h"
#include <stdbool.h>

#include "xparameters.h"

#define TOTAL_SECONDS 10

int main()
{

//    init_platform();
#ifdef INTERVALTIMER_H_
    intervalTimer_initAll();
    intervalTimer_resetAll();
    intervalTimer_testAll();
#endif

    printf("Simple Interrupt Test Program, will run for %d seconds.\n\r", TOTAL_SECONDS);
    u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
    printf("private timer ticks per second: %ld\n\r", privateTimerTicksPerSecond);
    // Initialize the GPIO LED driver and print out an error message if it fails (argument = true).
    leds_init(true);
    // Init all interrupts (but does not enable the interrupts at the devices).
    // Prints an error message if an internal failure occurs because the argument = true.
    interrupts_initAll(true);
    // Start the private ARM timer running.
    interrupts_enableTimerGlobalInts();
    // Start the timer.
    interrupts_startArmPrivateTimer();
	// Enable interrupts at the ARM.
	interrupts_enableArmInts();
#ifdef INTERVALTIMER_H_
	// Start a duration timer and compare its timing against the time computed by the timerIsr().
	intervalTimer_start(1);
#endif
	int countInterruptsViaInterruptsIsrFlag = 0;
	// Wait until TOTAL seconds have expired. globalTimerTickCount is incremented by timer isr.
	while (interrupts_isrInvocationCount() < (TOTAL_SECONDS * privateTimerTicksPerSecond)) {
      if (interrupts_isrFlagGlobal) {
    	  countInterruptsViaInterruptsIsrFlag++;
    	  interrupts_isrFlagGlobal = 0;
      }
	}
	printf("Detected interrupts via global flag: %d\n\r", countInterruptsViaInterruptsIsrFlag);

	#ifdef INTERVALTIMER_H_
 	intervalTimer_stop(1);
#endif
    interrupts_disableArmInts();
#ifdef INTERVALTIMER_H_
	double runningSeconds, isrRunningSeconds;
	intervalTimer_getTotalDurationInSeconds(1, &runningSeconds);
	printf("Measured run time by interval timer in seconds: %g.\n\r", runningSeconds);
	intervalTimer_getTotalDurationInSeconds(0, &isrRunningSeconds);
	printf("Measured run time in timerIsr (using interval timer): %g.\n\r", isrRunningSeconds);
#endif
   return 0;
}





