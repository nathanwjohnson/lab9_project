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
  uint32_t xleft = bike->current.x / 10;
  uint32_t yleft = bike->current.y / 10;
  uint32_t xright = bike->current.x / 10;
  uint32_t yright = bike->current.y / 10;
  direction_t choice;
  switch (bike->direction) {
  case LEFT:
    xnext -= 1;
    yleft += 1;
    yright -= 1;
    break;
  case RIGHT:
    xnext += 1;
    yleft -= 1;
    yright += 1;
    break;
  case UP:
    ynext -= 1;
    xleft -= 1;
    xright += 1;
    break;
  case DOWN:
    ynext += 1;
    xleft += 1;
    xright -= 1;
    break;
  }

  if (tronBoard[ynext][xnext] == 0) {
    choice = 2;
  } else if (tronBoard[yleft][xleft] == 0) {
    choice = 0;
  } else if (tronBoard[yright][xright] == 0) {
    choice = 1;
  } else {
    choice = 2;
  }
  // printBoard();
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
}

void fillSquare(uint32_t x, uint32_t y) {
  if (x <= 0 || x >= 320 || y <= 0 || y >= 240) {
    return;
  }
  tronBoard[y / 10][x / 10] = 1;
}

void clearSquare(uint32_t x, uint32_t y) {
  if (x <= 0 || x >= 320 || y <= 0 || y >= 240) {
    return;
  }
  tronBoard[y / 10][x / 10] = 0;
}
