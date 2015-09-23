#ifndef TicTacToeDisplay_h
#define TicTacToeDisplay_h
#include<stdint.h>
#include<stdlib.h>


// Inits the tic-tac-toe display, draws the lines that form the board.
void TicTacToeDisplay_init();

// Draws an X at the specified row and column.
void TicTacToeDisplay_drawX(uint8_t row, uint8_t column);

// Draws an O at the specified row and column.
void TicTacToeDisplay_drawO(uint8_t row, uint8_t column);

// After a touch has been detected and after the proper delay, this sets the row and column arguments
// according to where the user touched the board.
void TicTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column);

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will paint
// an X or an O, depending on whether switch 0 (SW0) is slid up (O) or down (X).
// When BTN0 is pushed, the screen is cleared. The test terminates when BTN1 is pushed.
void TicTacToeDisplay_runTest();

// This will draw the four board lines.
void TicTacToeDisplay_drawBoardLines();


#endif /* TicTacToeDisplay>h */
