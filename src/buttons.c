/*
 * buttons.c
 *
 *  Created on: Sep 6, 2014
 *      Author: watsonb
 */
#include <stdio.h>
#include "buttons.h"
#include "xil_io.h"
#include <xparameters.h>
#include <stdlib.h>
#include "supportFiles/leds.h"
#include "supportFiles/display.h"
#define BUTTONS_INIT_STATUS_OK 1
#define BUTTONS_INIT_STATUS_FAIL 0
#define XPAR_GPIO_PUSH_BUTTONS_BASEADDR 0x41240000
#define BUTTONS_GPIO_DEVICE_BASE_ADDRESS XPAR_GPIO_PUSH_BUTTONS_BASEADDR
#define HEX_F 0xF
#define HEX_ONE 0x1
#define HEX_TWO 0x2
#define HEX_FOUR 0x4
#define HEX_EIGHT 0x8


//Initiate the buttons using the GPIO base address
int buttons_init()
{
	*((uint32_t *) BUTTONS_GPIO_DEVICE_BASE_ADDRESS + 4) = HEX_F;
	return BUTTONS_INIT_STATUS_OK;
}

//Return a pointer that will be masked with 0xF to display which buttons are being pressed
int buttons_read()
{
	uint32_t *ptr = (uint32_t *) BUTTONS_GPIO_DEVICE_BASE_ADDRESS;
	return *ptr & HEX_F;
}

//Test the buttons by running the buttons_read() until all buttons are pressed
//Masking is used to determine which bit is activated
//A corresponding colored rectangle will be filled in for each button
//If a button is not pressed, the rectangle will remain black.
//After all buttons are pressed, display a black screen
void buttons_runTest()
{
	buttons_init();
	char number = buttons_read();

	while(number != HEX_F)
	{
		char read = number & HEX_F;
		leds_write(0);

		 if((read & 0x1) == HEX_ONE)
		 {
			 display_fillRect(100, 240, 140, 80, DISPLAY_YELLOW);
		 }
		 else
			 display_fillRect(100, 240, 140, 80, DISPLAY_BLACK);


		 if((read & 0x2) == HEX_TWO)
		 {
			 display_fillRect(100, 160,140, 80, DISPLAY_GREEN);
		 }
		 else
			 display_fillRect(100, 160,140, 80, DISPLAY_BLACK);


		 if((read & 0x4) == HEX_FOUR)
		 {
			 display_fillRect(100, 80, 140, 80, DISPLAY_RED);
		 }
		 else
			 display_fillRect(100, 80, 140, 80, DISPLAY_BLACK);


		 if((read & 0x8) == HEX_EIGHT)
		 {
			 display_fillRect(100, 0, 140, 80, DISPLAY_BLUE);
		 }
		 else
			 display_fillRect(100, 0, 140, 80, DISPLAY_BLACK);

		 number = buttons_read();

	}
		display_fillScreen(DISPLAY_BLACK);
}


