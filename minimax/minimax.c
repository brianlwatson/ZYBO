#include "minimax.h"
#include <stdio.h>

//int16_t minimax_minimax(minimax_board_t* board, bool player, int depth);
int16_t minimax_minimax(minimax_board_t* board, bool player);
uint8_t minimax_countEmptySquares(minimax_board_t* board);

#define TOP_ROW 0
#define MIDDLE_ROW 1
#define BOTTOM_ROW 2
#define LEFT_COLUMN 0
#define MIDDLE_COLUMN 1
#define RIGHT_COLUMN 2

#define PLAYER_MULTIPLIER 1
#define OPPONENT_MULTIPLIER 1

#define COUNTER_INIT_VALUE 0
#define INDEX_INIT_VALUE 0

minimax_move_t choice;

// This routine is not recursive but will invoke the recursive minimax function.
// It computes the row and column of the next move based upon:
// the current board,
// the player. true means the computer is X. false means the computer is O.
void minimax_computeNextMove(minimax_board_t* board, bool player, uint8_t* row, uint8_t* column) {
	//minimax_minimax(board, true, 0);
    //this is making it so that the computer makes the first move
	minimax_minimax(board, true);
	*row = choice.row;
	*column = choice.column;
}

//int16_t minimax_minimax(minimax_board_t* board, bool player, int depth) {
int16_t minimax_minimax(minimax_board_t* board, bool player) {
	if (minimax_isGameOver(minimax_computeBoardScore(board, player))) {   // Recursion base case, there has been a win or a draw.
	    // If game is over, you need to evaluate board based upon previous player/opponent.
	    //return minimax_computeBoardScore(board, !player);
	  // Otherwise, you need to recurse.
		return minimax_computeBoardScore(board, !player);
	}
    
	uint8_t numOfEmptySquares = minimax_countEmptySquares(board);
	
    int8_t scores[numOfEmptySquares];
    //
	minimax_move_t moves[numOfEmptySquares];
	int16_t score;
	uint8_t index = INDEX_INIT_VALUE;

	  // This while-loop will generate all possible boards.
	for (uint8_t i = TOP_ROW; i < TOP_ROW + MINIMAX_BOARD_ROWS; i++) {
		for (uint8_t j = LEFT_COLUMN; j < LEFT_COLUMN + MINIMAX_BOARD_COLUMNS; j++) {
			if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
				// Assign the square to the player or opponent.
				if (player)
					board->squares[i][j] = MINIMAX_PLAYER_SQUARE;
				else
					board->squares[i][j] = MINIMAX_OPPONENT_SQUARE;
				//score = minimax_minimax(board, !player, depth+1);
				score = minimax_minimax(board, !player);
				scores[index] = score;
				minimax_move_t m = {i, j};
				moves[index] = m;
				// Undo the change to the board (return the square to empty) prior to next iteration of for-loops.
				board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
				index++;
	      }
	    }
	  }
	  // Once you get here, you have iterated over empty squares at this level. All of the scores computed
	  // in the move-score table for boards at this level.
	  // Now you need to return the score depending upon whether you are computing min or max.
	score = scores[INDEX_INIT_VALUE];
	choice = moves[INDEX_INIT_VALUE];
	  if (player) {
		  for (int i = INDEX_INIT_VALUE; i < numOfEmptySquares; i++) {
			  if (scores[i] > score) {
				  score = scores[i];
				  choice = moves[i];
			  }
		  }
	  // choice <= get the move with the highest score in the move-score table.
	  // score <= highest score in the move-score table.
	  } else {
		  for (int i = INDEX_INIT_VALUE; i < numOfEmptySquares; i++) {
			  if (scores[i] < score) {
				  	  score = scores[i];
				  	  choice = moves[i];
			  }
		  }
	  // choice <= get the move with the lowest score in the move-score table.
	  // score <= lowest score in the move-score table.
	  }
	  return score;
}

uint8_t minimax_countEmptySquares(minimax_board_t* board) {
	uint8_t count = COUNTER_INIT_VALUE;
	for (int i = TOP_ROW; i < TOP_ROW + MINIMAX_BOARD_ROWS; i++) {
		for (int j = LEFT_COLUMN; j < LEFT_COLUMN + MINIMAX_BOARD_COLUMNS; j++) {
			if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE)
				count++;
		}
	}
	return count;
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
	return (score != MINIMAX_NOT_ENDGAME);
}

//attempt at simplifying computeboardscore
bool minimax_wins(minimax_board_t* board)
{
	uint8_t wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
	int i;
	for(i = 0; i < 8; ++i)
	{
		if(board->squares[wins[i][0]] != 0 &&
		board->squares[wins[i][0]] == board->squares[wins[i][1]] &&
		board->squares[wins[i][0]] == board->squares[wins[i][2]])
		return true;
	}
	return false;
}
// Returns the score of the board, based upon the player.
int16_t minimax_computeBoardScore(minimax_board_t* board, bool player) {
	int8_t multiplier;
	if (player)
		multiplier = PLAYER_MULTIPLIER;
	else
		multiplier = OPPONENT_MULTIPLIER;

	// Row-based wins
	if ((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[TOP_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE)
			|| (board->squares[MIDDLE_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE)
			|| (board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE))
		return MINIMAX_PLAYER_WINNING_SCORE * multiplier;
	else if((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[TOP_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE)
			|| (board->squares[MIDDLE_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE)
			|| (board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE))
		return MINIMAX_OPPONENT_WINNING_SCORE * multiplier;
	// Column-based wins
	else if ((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE)
			|| (board->squares[TOP_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE)
			|| (board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE))
		return MINIMAX_PLAYER_WINNING_SCORE * multiplier;
	else if ((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE)
			|| (board->squares[TOP_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE)
			|| (board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE))
		return MINIMAX_OPPONENT_WINNING_SCORE * multiplier;
	// Diagonal-based wins
	else if ((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE)
			|| (board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_PLAYER_SQUARE && board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_PLAYER_SQUARE))
		return MINIMAX_PLAYER_WINNING_SCORE * multiplier;
	else if ((board->squares[TOP_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[BOTTOM_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE)
			|| (board->squares[BOTTOM_ROW][LEFT_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[MIDDLE_ROW][MIDDLE_COLUMN] == MINIMAX_OPPONENT_SQUARE && board->squares[TOP_ROW][RIGHT_COLUMN] == MINIMAX_OPPONENT_SQUARE))
		return MINIMAX_OPPONENT_WINNING_SCORE * multiplier;
	else
		// If the board is full, the game is a draw. If not, the game is not over.
		for (int i = TOP_ROW; i < TOP_ROW + MINIMAX_BOARD_ROWS; i++) {
				for (int j = LEFT_COLUMN; j < LEFT_COLUMN + MINIMAX_BOARD_COLUMNS; j++) {
					if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE)
						return MINIMAX_NOT_ENDGAME;
				}
			}
		return MINIMAX_DRAW_SCORE;
}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t* board) {
	for (int i = TOP_ROW; i < TOP_ROW + MINIMAX_BOARD_ROWS; i++) {
		for (int j = LEFT_COLUMN; j < LEFT_COLUMN + MINIMAX_BOARD_COLUMNS; j++) {
			board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
		}
	}
}
