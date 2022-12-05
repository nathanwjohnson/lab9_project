#include "display.h"
#include "touchscreen.h"
#include <stdbool.h>
#include <stdio.h>

#include "bikes.h"
#include "computer.h"

bool tronBoard[25][33];

static void printBoard() {
  for (uint32_t i = 0; i < 25; i++) {
    for (uint32_t j = 0; j < 33; j++) {
      printf("%d ", tronBoard[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

direction_t computeNextMove(bike_t *bike, bike_t *enemyBike) {
  uint32_t xPos = bike->current.x / 10;
  uint32_t yPos = bike->current.y / 10;
  uint32_t xnext = bike->current.x / 10;
  uint32_t ynext = bike->current.y / 10;
  direction_t choice;
  display_drawLine(xPos * 10 + 1, yPos * 10 + 1, xPos * 10 + 1, yPos * 10 + 1,
                   DISPLAY_CYAN);
  switch (bike->direction) {
  case LEFT:
    xnext -= 1;
    break;
  case RIGHT:
    xnext += 1;
    break;
  case UP:
    ynext -= 1;
    break;
  case DOWN:
    ynext += 1;
    break;
  }
  display_drawLine(xnext * 10 + 1, ynext * 10 + 1, xnext * 10 + 1,
                   ynext * 10 + 1, DISPLAY_YELLOW);
  printf("%d %d %d\n", xnext, ynext, tronBoard[ynext][xnext]);
  if (tronBoard[ynext][xnext] == 0) {
    choice = 2;
  } else {
    choice = 1;
  }
  printBoard();
  return choice;
}

void initBoard() {
  for (uint32_t i = 0; i < 25; i++) {
    for (uint32_t j = 0; j < 33; j++) {
      if (i == 0 || i == 24 || j == 0 || j == 32) {
        tronBoard[i][j] = 1;
      } else {
        tronBoard[i][j] = 0;
      }
    }
  }
  tronBoard[12][4] = 1;
}

void fillSquare(uint32_t x, uint32_t y) { tronBoard[y / 10][x / 10] = 1; }

void clearSquare(uint32_t x, uint32_t y) { tronBoard[y / 10][x / 10] = 0; }
