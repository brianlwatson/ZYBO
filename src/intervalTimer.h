

#ifndef intervalTimer_h
#define intervalTimer_h
#include<stdint.h>

uint32_t intervalTimer_start(uint32_t timerNumber);

uint32_t intervalTimer_stop(uint32_t timerNumber);

uint32_t intervalTimer_reset(uint32_t timerNumber);

uint32_t intervalTimer_init(uint32_t timerNumber);

uint32_t intervalTimer_initAll();

uint32_t intervalTimer_resetAll();

uint32_t intervalTimer_testAll();

uint32_t intervalTimer_runTest(uint32_t timerNumber);

uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double *seconds);

#endif
