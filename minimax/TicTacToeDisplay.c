#include "buttons.h"
#include "switches.h"
#include "TicTacToeDisplay.h"
#include <stdint.h>
#include "supportFiles/display.h"
#include "xparameters.h"
#include <stdlib.h>
#include <stdio.h>
#include "supportFiles/utils.h"
#include "minimax.h"


#define ZERO 0
#define ONE_THIRD_HEIGHT display_height()/3
#define TWO_THIRD_HEIGHT display_height()*2/3
#define ONE_THIRD_WIDTH display_width()/3
#define TWO_THIRD_WIDTH display_width() * 2/3
#define FULL_WIDTH display_width()
#define FULL_HEIGHT display_height()
#define ONE_SIXTH_HEIGHT display_height()/6
#define ONE_SIXTH_WIDTH display_width()/6
#define HEX_ONE 0x1
#define HEX_F 0xF

#define X_OFFSET display_height()/8

uint8_t row = 0;
uint8_t column = 0;

void TTT_Display_setRow(uint8_t new_row)
{
	//row = new_row;
}

void TTT_Display_setColumn(uint8_t new_column)
{
	//column = new_column;
}

void TicTacToeDisplay_init()
{
	// Must init all of the software and underlying hardware for LCD.
	display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
	TicTacToeDisplay_drawBoardLines();
}

void TicTacToeDisplay_complete()
{
	display_fillScreen(DISPLAY_BLACK);
	display_setTextColor(DISPLAY_RED);
	display_setTextSize(4);
	display_println("TEST COMPLETE" );

}

void TicTacToeDisplay_drawX(uint8_t row, uint8_t column)
{
	uint16_t X_ORIGIN = (column*ONE_THIRD_WIDTH) + ONE_THIRD_WIDTH/2;
	uint16_t Y_ORIGIN = (row * ONE_THIRD_HEIGHT)+ ONE_THIRD_HEIGHT/2;
	display_drawLine(X_ORIGIN - X_OFFSET, Y_ORIGIN + X_OFFSET, X_ORIGIN + X_OFFSET, Y_ORIGIN-X_OFFSET, DISPLAY_RED);
	display_drawLine(X_ORIGIN - X_OFFSET, Y_ORIGIN - X_OFFSET, X_ORIGIN + X_OFFSET, Y_ORIGIN+X_OFFSET, DISPLAY_RED);
}

void TicTacToeDisplay_drawO(uint8_t row, uint8_t column)
{
	uint16_t X_ORIGIN = (column*ONE_THIRD_WIDTH) + ONE_THIRD_WIDTH/2;
	uint16_t Y_ORIGIN = (row * ONE_THIRD_HEIGHT)+ ONE_THIRD_HEIGHT/2;
	display_drawCircle(X_ORIGIN, Y_ORIGIN, X_OFFSET, DISPLAY_GREEN);

}

void TicTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column)
{
	 int16_t x;
	 int16_t y;
	 uint8_t z;
	 display_clearOldTouchData();
	 display_getTouchedPoint(&x,&y,&z);

	 if( x < ONE_THIRD_WIDTH)
	 {
		 TTT_Display_setRow(0);
		 *column = 0;
	 }

	 else if (x > TWO_THIRD_WIDTH)
	 {
	//	 TTT_Display_setRow(1);
		 *column = 2;
	 }

	 else
	 {
	//	 TTT_Display_setRow(2);
		*column = 1;
	 }

	 if(y < ONE_THIRD_HEIGHT)
	 {
		//TTT_Display_setColumn(0);
		 *row = 0;
	 }

	 else if (y > TWO_THIRD_HEIGHT)
	 {
		// TTT_Display_setColumn(1);
		 *row = 2;
	 }

	 else
	 {
		// TTT_Display_setColumn(2);
		 *row = 1;
	 }


}

void TicTacToeDisplay_drawBoardLines()
{
	display_init();
	display_drawLine(0, ONE_THIRD_HEIGHT, FULL_WIDTH, ONE_THIRD_HEIGHT, DISPLAY_WHITE);
	display_drawLine(0, TWO_THIRD_HEIGHT, FULL_WIDTH, TWO_THIRD_HEIGHT, DISPLAY_WHITE);
	display_drawLine(ONE_THIRD_WIDTH, 0, ONE_THIRD_WIDTH, FULL_HEIGHT, DISPLAY_WHITE);
	display_drawLine(TWO_THIRD_WIDTH, 0, TWO_THIRD_WIDTH, FULL_HEIGHT, DISPLAY_WHITE);
}

void TicTacToeDisplay_runTest()
{
	display_init();
		TicTacToeDisplay_init();

	char number = buttons_read();
	char read = number & HEX_F;
	char s_read = switches_read();
	display_clearOldTouchData();

	bool go = true;
	while(go)
	{
		s_read = switches_read();
		number = buttons_read();
		read = number & HEX_F;
		utils_msDelay(50);


	if(display_isTouched())
	{
		utils_msDelay(50);
		display_clearOldTouchData();

		if(display_isTouched())
			TicTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);

		if(s_read == 0x1)
		{
		TicTacToeDisplay_drawO(row, column);display_clearOldTouchData();
		}

		else
		{
		TicTacToeDisplay_drawX(row, column);display_clearOldTouchData();
		}


		display_clearOldTouchData();


	}
	if((read & 0x1) == HEX_ONE)
	{
		TicTacToeDisplay_init();
	}

	 if((read & 0x2) == 0x02)
	{
		TicTacToeDisplay_complete();
		go = false;
	}



	display_clearOldTouchData();


	}
}
