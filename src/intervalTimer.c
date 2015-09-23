

#include "intervalTimer.h"
#include<xil_io.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdio.h>
#include<xparameters.h>

//Definition for peripheral AXI_TIMER 0
#define XPAR_AXI_TIMER_0_DEVICE_ID 0
#define XPAR_AXI_TIMER_0_BASEADDR 0x42800000

//Definition for peripheral AXI_TIMER 1
#define XPAR_AXI_TIMER_1_DEVICE_ID 1
#define XPAR_AXI_TIMER_1_BASEADDR 0x42840000

//Definition for peripheral AXI_TIMER 2
#define XPAR_AXI_TIMER_2_DEVICE_ID 2
#define XPAR_AXI_TIMER_2_BASEADDR 0x42880000

//Offsets for different registers within each counter
#define TCSR0_OFFSET 0x00
#define TCSR1_OFFSET 0x10
#define TLR0_OFFSET 0x04
#define TLR1_OFFSET 0x14
#define TCR0_OFFSET 0x08
#define TCR1_OFFSET 0x18

//Bit Location Offsets
#define UDT0_BIT 0x1
#define ENT0_BIT 0x7
#define LOAD0_BIT 0x5
#define CASC_BIT 0x11

//Shift Values
#define shift_32_left 0x32

//Return values
#define INTERVAL_TIMER_STATUS_OK 1
#define INTERVAL_TIMER_STATUS_FAIL 0

//possible uint32_t timerNumber values
#define TIMER_0 0x0
#define TIMER_1 0x1
#define TIMER_2 0x2

//define each timer's clock frequency
#define XPAR_AXE_TIMER_0_CLOCK_FREQ_HZ 100000000
#define XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ 100000000
#define XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ 100000000

//Initialization values
#define init_0 0x0
#define init_1 0x1


//----------------------------------------------------------------------------------------------
//	uint32_t findAddress(uint32_t timerNumber)
//	Returns a timer's base address
//	@param: timerNumber contains the number of the timer which will return its base address
//	@return: XPAR_AXI_TIMER_0_BASEADDR, XPAR_AXI_TIMER_1_BASEADDR, XPAR_AXI_TIMER_2_BASEADDR, 0
//----------------------------------------------------------------------------------------------
uint32_t findAddress(uint32_t timerNumber){

	if(timerNumber == TIMER_0){
		return XPAR_AXI_TIMER_0_BASEADDR;
	}

	else if(timerNumber == TIMER_1){
		return XPAR_AXI_TIMER_1_BASEADDR;
	}

	else if(timerNumber == TIMER_2){
		return XPAR_AXI_TIMER_2_BASEADDR;
	}

	//return a 0 if an invalid timerNumber is given
	return INTERVAL_TIMER_STATUS_FAIL;
}



//-----------------------------------------------------------------------------------------------
// uint32_t getFrequency(uint32_t timerNumber)
// @param: timerNumber contains the number of the timer which will have its frequency returned
// @return: TIMER_0_FREQUENCY, TIMER_1_FREQUENCY, TIMER_2_FREQUENCY, 0
//-----------------------------------------------------------------------------------------------
uint32_t getFrequency(uint32_t timerNumber){

	if(timerNumber == TIMER_0){
		return XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
	}

	else if(timerNumber == TIMER_1){
		return XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
	}

	else if(timerNumber == TIMER_2){
		return XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
	}

	//return a 0 if an invalid timerNumber is given
	return INTERVAL_TIMER_STATUS_FAIL;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_start(uint32_t timerNumber)
// @param: timerNumber contains the number of the timer which will be started
// @return: 1
// this function accesses the ENT0 bit of the TCSR0 register and sets a 1 to it
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_start(uint32_t timerNumber){

	uint32_t BASE_ADDR = findAddress(timerNumber);

	//Finds the current value of the TCSR0 and initializes a 1 to the ENT0_BIT
	uint32_t set_ENT0 = (Xil_In32(BASE_ADDR + TCSR0_OFFSET) | (init_1 << ENT0_BIT));

	//Writes the adjusted value to the TCSR0 to the corresponding timer
	Xil_Out32(BASE_ADDR + TCSR0_OFFSET, set_ENT0);

	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_stop(uint32_t timerNumber)
// @param: timerNumber contains the number of the timer which will be stopped
// @return: 1
// this function accesses the ENT0 bit of the TCSR0 register and writes a 0 to it
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_stop(uint32_t timerNumber){

	uint32_t BASE_ADDR = findAddress(timerNumber);

	//Finds the current value of the TCSR0 and writes a 0 to the ENT0_BIT
	uint32_t clear_ENT0 = Xil_In32(BASE_ADDR + TCSR0_OFFSET) & ~(init_1 << ENT0_BIT);

	//Writes the adjusted value to the TCSR0 to the corresponding timer
	Xil_Out32(BASE_ADDR + TCSR0_OFFSET, clear_ENT0);

	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_reset(uint32_t timerNumber)
// @param: timerNumber contains the number of the timer which will be reset
// @return: 1
// accesses the TLR0, TLR1, LOAD0, LOAD1 bits and writes a 0 to each of them, resetting them
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_reset(uint32_t timerNumber){

	uint32_t BASE_ADDR = findAddress(timerNumber);

	//Store a 0 into counter 0:
		//Write a 0 into the TLR0 Register
		Xil_Out32((BASE_ADDR + TLR0_OFFSET), init_0);

		//Write a 1 into the LOAD0 bit in the TCSR0
		uint32_t set_LOAD0 = (Xil_In32(BASE_ADDR + TCSR0_OFFSET) | (init_1 << LOAD0_BIT));
		Xil_Out32(BASE_ADDR + TCSR0_OFFSET, set_LOAD0);

	//Store a 0 into counter 1
		//Write a 0 into the TLR1 register
		Xil_Out32((BASE_ADDR + TLR1_OFFSET), init_0);

		//Write a 1 into the Load 1 bit of the TCSR1 register
		uint32_t set_LOAD1 = (Xil_In32(BASE_ADDR + TCSR0_OFFSET) | (init_1 << LOAD0_BIT));
		Xil_Out32((BASE_ADDR + TCSR1_OFFSET), set_LOAD1);

		//Initialize the corresponding timer after resetting it
		intervalTimer_init(timerNumber);

		return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_init(uint32_t timerNumber)
// @param: timerNumber contains the number of the timer which will be reset
// @return: 1
// Initializes the corresponding timer by writing a 0 to the TCSR0 and TCSR1 registers
// Sets the cascade bit of the TCSR0 register
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_init(uint32_t timerNumber){

	uint32_t BASE_ADDR = findAddress(timerNumber);

	//Write a 0 to the TCSR0 and TCSR1 registers
	Xil_Out32((BASE_ADDR + TCSR0_OFFSET), init_0);
	Xil_Out32((BASE_ADDR + TCSR1_OFFSET), init_0);

	//Set the Cascade bit
	uint32_t set_cascade = Xil_In32(BASE_ADDR + TCSR0_OFFSET) | (init_1 << CASC_BIT);
	Xil_Out32((BASE_ADDR + TCSR0_OFFSET), set_cascade);

	//Set the UDT0 Bit to 0 for count up enabling - this could be considered "clearing it"
	uint32_t set_counterup = (Xil_In32(BASE_ADDR + TCSR0_OFFSET) & (~(init_1 << UDT0_BIT)));
	Xil_Out32((BASE_ADDR + TCSR0_OFFSET), set_counterup);

	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_initAll()
// @param: timerNumber contains the number of the timer which will be reset
// @return: 1
// Initializes all of the timers by calling intervalTimer_init(timerNumber) on each timer
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_initAll(){

	intervalTimer_init(TIMER_0);
	intervalTimer_init(TIMER_1);
	intervalTimer_init(TIMER_2);

	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_resetAll()
// @param: timerNumber contains the number of the timer which will be reset
// @return: 1
// Resets all of the timers by calling intervalTimer_reset(timerNumber) on each timer
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_resetAll(){

	intervalTimer_reset(TIMER_0);
	intervalTimer_reset(TIMER_1);
	intervalTimer_reset(TIMER_2);

	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_testAll()
// @return: 1
// tests the functionality of all of the timers.
// this functionality is defined in the lab3.c and was given to us in the lab specs
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_testAll(){
	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_runTest()
// @param: timerNumber contains the number of the timer which will be reset
// @return: 1
// runs the test
// this functionality is defined in the lab3.c and was given to us in the lab specs
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_runTest(uint32_t timerNumber){
	return INTERVAL_TIMER_STATUS_OK;
}



//-----------------------------------------------------------------------------------------------
// uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double*seconds)
// @param: timerNumber contains the number of the timer which will be rest
// @param: seconds is a pointer to the value which will be returned to the console in seconds
// return: seconds
// this function converts the values stored in the TCR0 and TCR1 registers into seconds
//-----------------------------------------------------------------------------------------------
uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double *seconds){

	uint32_t BASE_ADDR = findAddress(timerNumber);
	uint32_t timer_frequency = getFrequency(timerNumber);

	//Get the value of the top 32 bits.
	uint32_t top_cascade = Xil_In32(BASE_ADDR + TCR1_OFFSET);

	//Get the value of the lower 32 bits.
	uint32_t bottom_cascade = Xil_In32(BASE_ADDR + TCR0_OFFSET);

	//Concatenate the top 32 bits from TCR1 with the bottom 32 bits from TCR0
	unsigned long long cascade = ((top_cascade << shift_32_left)| bottom_cascade);

	//Computer seconds through double division
	*seconds = ((double)cascade / ((double)timer_frequency/1000));

	return *seconds ;
}



