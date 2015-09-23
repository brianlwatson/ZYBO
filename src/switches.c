/*
 * switches.c
 *
 *  Created on: Sep 6, 2014
 *      Author: watsonb
 */

#include "switches.h"
#include "supportFiles/leds.h"
#include "xil_io.h"
#include <xparameters.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#define SWITCHES_GPIO_DEVICE_BASE_ADDRESS XPAR_GPIO_SLIDE_SWITCHES
#define SWITCHES_INIT_STATUS_OK 1
#define SWITCHES_INIT_STATUS_FAIL 0
#define XPAR_GPIO_SLIDE_SWITCHES_BASEADDR 0x41280000
#define HEX_F 0xF


//Initializes the SWICHES driver software and hardware
int switches_init()
{
	Xil_Out32(XPAR_GPIO_SLIDE_SWITCHES_BASEADDR + 0x04, 0xF);
	return SWITCHES_INIT_STATUS_OK;
}

//Returns the current value of all 4 SWITCHES
int switches_read()
{
	u32 read = Xil_In32(XPAR_GPIO_SLIDE_SWITCHES_BASEADDR);
	return read;
}


//Runs a test of the switches
//switches_read() will run until all switches are in the UP position.
//Masking is used to determine which switches are in the UP position.
//When all switches are on the on position, the LED's are turned off.
void switches_runTest()
{
	switches_init();
	u32 read = switches_read();
	while(read  != HEX_F)
	{
		read = switches_read();
		char result = read & HEX_F;
		leds_write(result);
	}
	  leds_write(0);

}



