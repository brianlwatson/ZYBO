/*
 * buttons.h
 *
 *  Created on: Sep 6, 2014
 *      Author: watsonb
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

//Initializes the button driver software and hardware. Returns one of the defined states
int buttons_init();

//Returns the current value of all 4 buttons as a 4 digit binary number.
int buttons_read();

//Runs a test of the buttons. As you push the buttons, the LCD is going to light up. It will keep testing until all 4 buttons are pressed at the same time
void buttons_runTest();


#endif /* BUTTONS_H_ */
