#include <stdio.h>
#include <stdlib.h>

#include "supportFiles/leds.h"
#include "supportFiles/intervalTimer.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"

#include "simon.h"
#include "simonGame.h"


int main()
{
	simonMainControl_enable();
	while (1) {
		buttonHandler_tick();
		flashSequence_tick();
		verifySequence_tick();
		simonMainControl_tick();
		utils_msDelay(1);
		if (simonMainControl_isGameOver())
			break;
	}
//	printf("game over.\n\r");
    while (1);

//	char str[255];   // Enough for some simple printing.
//	display_init();
//	display_fillScreen(DISPLAY_BLACK);        // clear the screen.
//    display_setCursor(0, display_height()/2); //
//	display_setTextSize(2);
//	display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);
//	sprintf(str, "Touch and release to start the Simon demo.");
//	display_println(str);
//	display_println();
//    while (!display_isTouched());  // Wait here until the screen is touched.
//    while (display_isTouched());   // Now wait until the touch is released.
//    display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
//    uint8_t testSequence[] = {0, 1, 3, 2};
//    flashSequence_setSequence(testSequence, 4);
//    simon_drawButton(0); simon_drawButton(1);  // Draw all of the buttons.
//    simon_drawButton(2); simon_drawButton(3);
//
//    while (1) {
//      verifySequence_enable();
//      verifySequence_tick();
//      buttonHandler_tick();
//      if (verifySequence_isComplete()) {
//    	  if (verifySequence_isUserInputError())
//    		  printf("user input error\n\r.");
//    	  if (verifySequence_isTimeOutError())
//    		  printf("time out error\n\r.");
//    	  break;
//      }
//      utils_msDelay(1);
//    }

//    for (int i=0; i<100; i++) {
//    	switch (i % 4) {
//    	case 0: tmp = 0;
//    	break;
//    	case 1: tmp = 1;
//    	break;
//    	case 2: tmp = 3;
//    	break;
//    	case 3: tmp = 2;
//    	break;
//    	}
//    	testSequence[i] = tmp;
//    }
//    flashSequence_setDelay(50);
//    flashSequence_setSequence(testSequence, 100);
//    flashSequence_enable();
//    while (1) {
//      flashSequence_tick();
//      utils_msDelay(1);
//      if (flashSequence_completed()) {
//    	  printf("completed.\n\r");
//    	  flashSequence_disable();
//    	  break;
//      }
//    }

    while (1);



//    simon_drawButton(0); simon_drawButton(1);  // Draw all of the buttons.
//    simon_drawButton(2); simon_drawButton(3);
//
//    buttonHandler_enable();
//    while (1) {
//    	buttonHandler_tick();
//    	utils_msDelay(1);
//    	if (buttonHandler_releaseDetected()) {
//    		buttonHandler_disable();
//    		buttonHandler_tick();
//            buttonHandler_enable();
//    	}
//    }
//	simon_demo(20);  // Run the demo for 20 touches.
//	int16_t x, y;
//	uint8_t z;
//	display_init();
//	printf("Printing touch screen coordinates.\n\r");
//	while(1) {
//      if (display_isTouched()){
//  	    display_getTouchedPoint(&x, &y, &z);s
//	    printf("x: %d, y:%d, z:%d\n\r", x, y, z);
//      }
//	}

//	  display_init();
//	  display_testFillScreen();
//	  display_testLines(DISPLAY_CYAN);
//	  display_testFastLines(DISPLAY_RED, DISPLAY_BLUE);
//	  display_testRects(DISPLAY_GREEN);
//	  display_testFilledRects(DISPLAY_YELLOW, DISPLAY_MAGENTA);
//	  display_testFilledCircles(10, DISPLAY_MAGENTA);
//	  display_testCircles(10, DISPLAY_WHITE);
//	  display_testTriangles();
//	  display_testFilledTriangles();
//	  display_testRoundRects();
//	  display_testFilledRoundRects();
//	  display_testText();


}
