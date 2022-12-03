/*#ifndef COMPUTER
#define COMPUTER

#include <stdbool.h>
#include <stdint.h>

// Scoring for minimax.
#define MINIMAX_X_WINNING_SCORE 10  // This means that X will win.
#define MINIMAX_O_WINNING_SCORE -10 // This means that O will win.
#define MINIMAX_DRAW_SCORE 0        // Nobody wins.
#define MINIMAX_NOT_ENDGAME -1      // Not an end-game.

// Define a score type.
typedef int16_t minimax_score_t;

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this function
// as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn);

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn);

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board);

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score);

#endif /* COMPUTER */
