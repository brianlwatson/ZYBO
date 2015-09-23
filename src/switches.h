/*
 * switches.h
 *
 *  Created on: Sep 6, 2014
 *      Author: watsonb
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

// Initializes the SWITCHES driver software and hardware. Returns 1 or 0 if good or bad
int switches_init();

//Returns the current value of all 4 SWITCHES as a 4 digit binary
int switches_read();

//Runs a test of all of the switches until they are all in the 1 position simultaneously
void switches_runTest();


#endif /* SWITCHES_H_ */
