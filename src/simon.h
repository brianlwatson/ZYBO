/*
 * simon.h
 *
 *  Created on: Jul 18, 2014
 *      Author: hutch
 */

#include <stdbool.h>
#include <stdint.h>

#ifndef SIMON_H_
#define SIMON_H_

// Width, height of the simon "buttons"
#define SIMON_BUTTON_WIDTH 60
#define SIMON_BUTTON_HEIGHT 60

int8_t simon_computeRegionNumber(int16_t x, int16_t y);
void simon_drawButton(uint8_t regionNumber);
void simon_drawSquare(uint8_t regionNo, bool erase);
void simon_demo(uint16_t touchCount);

#endif /* SIMON_H_ */
