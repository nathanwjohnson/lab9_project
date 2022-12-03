/*#include "display.h"
#include "touchscreen.h"
#include <stdbool.h>
#include <stdio.h>

#include "computer.h"

minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {
  // if it is Xs turn, look for if X won, otherwise check if O won
  if (is_Xs_turn) {
    // iterates through rows
    for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
      // checks horizontal rows
      if (board->squares[TOP + i][LFT] == MINIMAX_X_SQUARE &&
          board->squares[TOP + i][MID] == MINIMAX_X_SQUARE &&
          board->squares[TOP + i][RGT] == MINIMAX_X_SQUARE) {
        return MINIMAX_X_WINNING_SCORE;
      }
    }
    // iterates through columns
    for (uint8_t i = 0; i < TICTACTOE_BOARD_COLUMNS; i++) {
      // checks vertical rows
      if (board->squares[TOP][LFT + i] == MINIMAX_X_SQUARE &&
          board->squares[MID][LFT + i] == MINIMAX_X_SQUARE &&
          board->squares[BOT][LFT + i] == MINIMAX_X_SQUARE) {
        return MINIMAX_X_WINNING_SCORE;
      }
    }
    // checks diagonal
    if (board->squares[TOP][LFT] == MINIMAX_X_SQUARE &&
        board->squares[MID][MID] == MINIMAX_X_SQUARE &&
        board->squares[BOT][RGT] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
    // checks diagonal
    if (board->squares[TOP][RGT] == MINIMAX_X_SQUARE &&
        board->squares[MID][MID] == MINIMAX_X_SQUARE &&
        board->squares[BOT][LFT] == MINIMAX_X_SQUARE) {
      return MINIMAX_X_WINNING_SCORE;
    }
  } else if (!is_Xs_turn) {
    // iterates through rows
    for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
      // checks horizontal rows
      if (board->squares[TOP + i][LFT] == MINIMAX_O_SQUARE &&
          board->squares[TOP + i][MID] == MINIMAX_O_SQUARE &&
          board->squares[TOP + i][RGT] == MINIMAX_O_SQUARE) {
        return MINIMAX_O_WINNING_SCORE;
      }
    }
    // iterates through columns
    for (uint8_t i = 0; i < TICTACTOE_BOARD_COLUMNS; i++) {
      // checks vertical rows
      if (board->squares[TOP][LFT + i] == MINIMAX_O_SQUARE &&
          board->squares[MID][LFT + i] == MINIMAX_O_SQUARE &&
          board->squares[BOT][LFT + i] == MINIMAX_O_SQUARE) {
        return MINIMAX_O_WINNING_SCORE;
      }
    }
    // checks diagonal
    if (board->squares[TOP][LFT] == MINIMAX_O_SQUARE &&
        board->squares[MID][MID] == MINIMAX_O_SQUARE &&
        board->squares[BOT][RGT] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
    // checks diagonal
    if (board->squares[TOP][RGT] == MINIMAX_O_SQUARE &&
        board->squares[MID][MID] == MINIMAX_O_SQUARE &&
        board->squares[BOT][LFT] == MINIMAX_O_SQUARE) {
      return MINIMAX_O_WINNING_SCORE;
    }
  }
  // iterates through rows
  for (uint8_t currentRow = 0; currentRow < TICTACTOE_BOARD_ROWS;
       currentRow++) {
    // iterates through columns
    for (uint8_t currentColumn = 0; currentColumn < TICTACTOE_BOARD_COLUMNS;
         currentColumn++) {
      if (board->squares[currentRow][currentColumn] == MINIMAX_EMPTY_SQUARE) {
        return MINIMAX_NOT_ENDGAME;
      }
    }
  }
  return MINIMAX_DRAW_SCORE;
}

// recursively determines the best spot to play at depending on the current
// board and whose turn it is
minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn) {
  minimax_score_t score = minimax_computeBoardScore(board, !is_Xs_turn);
  int8_t tempScoreArrayValue;
  // set the default score array value to either 11 or -11 based on whose turn
  // it is
  if (is_Xs_turn) {
    tempScoreArrayValue = SCORE_ARRAY_DEFAULT_LOW;
  } else {
    tempScoreArrayValue = SCORE_ARRAY_DEFAULT_HIGH;
  }
  minimax_score_t score_array[TICTACTOE_BOARD_ROWS][TICTACTOE_BOARD_COLUMNS];
  // if the game is over
  if (minimax_isGameOver(score)) {
    // Recursion base case, there has been a win or a draw.
    // Evaluate board based upon prev player's turn.
    return minimax_computeBoardScore(board, !is_Xs_turn);
  }

  tictactoe_location_t moves_array[TICTACTOE_BOARD_ROWS]
                                  [TICTACTOE_BOARD_COLUMNS];
  minimax_score_t min = MINIMAX_X_WINNING_SCORE;
  minimax_score_t max = MINIMAX_O_WINNING_SCORE;

  // Otherwise, you need to recurse.
  // This loop will generate all possible boards and call
  // minimax recursively for every empty square
  for (uint8_t currentRow = 0; currentRow < TICTACTOE_BOARD_ROWS;
       currentRow++) {
    for (uint8_t currentColumn = 0; currentColumn < TICTACTOE_BOARD_COLUMNS;
         currentColumn++) {
      // if this spot is an empty space
      if (board->squares[currentRow][currentColumn] == MINIMAX_EMPTY_SQUARE) {
        // Simulate playing at this location
        board->squares[currentRow][currentColumn] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        score = minimax(board, !is_Xs_turn);

        score_array[currentRow][currentColumn] = score;
        moves_array[currentRow][currentColumn].row = currentRow;
        moves_array[currentRow][currentColumn].column = currentColumn;

        // Undo the change to the board
        board->squares[currentRow][currentColumn] = MINIMAX_EMPTY_SQUARE;
      } else {
        score_array[currentRow][currentColumn] = tempScoreArrayValue;
      }
    }
  }

  // Once you get here, you have iterated over empty squares at this level. All
  // of the scores have been computed in the move-score table for boards at this
  // level. Now you need to return the score depending upon whether you are
  // computing min or max.
  if (is_Xs_turn) {
    // iterates through rows
    for (uint8_t currentRow = 0; currentRow < TICTACTOE_BOARD_ROWS;
         currentRow++) {
      // iterates through columns
      for (uint8_t currentColumn = 0; currentColumn < TICTACTOE_BOARD_COLUMNS;
           currentColumn++) {
        // if greater than max, then this is the best choice yet
        if (score_array[currentRow][currentColumn] > max) {
          max = score_array[currentRow][currentColumn];
          choice = moves_array[currentRow][currentColumn];
        }
      }
      score = max;
    }
  } else {
    // iterates through rows
    for (uint8_t currentRow = 0; currentRow < TICTACTOE_BOARD_ROWS;
         currentRow++) {
      // iterates through columns
      for (uint8_t currentColumn = 0; currentColumn < TICTACTOE_BOARD_COLUMNS;
           currentColumn++) {
        // if less than min, then this is the best choice yet
        if (score_array[currentRow][currentColumn] < min) {
          min = score_array[currentRow][currentColumn];
          choice = moves_array[currentRow][currentColumn];
        }
      }
    }
    score = min;
  }
  return score;
}

// This routine is not recursive but will invoke the recursive minimax
// function. You will call this function from the controlling state machine
// that you will implement in a later milestone. It computes the row and
// column of the next move based upon: the current board and player.
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
                                             bool is_Xs_turn) {
  // printBoard(board);
  // printf("Is Xs turn: %s\n", is_Xs_turn ? "True" : "False");
  minimax_score_t score = minimax(board, is_Xs_turn);
  return choice;
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {
  // iterates through rows
  for (uint8_t currentRow = 0; currentRow < TICTACTOE_BOARD_ROWS;
       currentRow++) {
    // iterates through columns
    for (uint8_t currentColumn = 0; currentColumn < TICTACTOE_BOARD_COLUMNS;
         currentColumn++) {
      board->squares[TOP + currentRow][LFT + currentColumn] =
          MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  if (score == MINIMAX_NOT_ENDGAME) {
    return false;
  }
  return true;
}
*/