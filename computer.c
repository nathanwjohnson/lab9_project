#include "display.h"
#include "touchscreen.h"
#include <stdbool.h>
#include <stdio.h>

#include "bikes.h"
#include "computer.h"

bool tronBoard[26][34];

static void printBoard(bike_t *bike, bike_t *enemyBike) {
  for (uint32_t i = 0; i < 25; i++) {
    for (uint32_t j = 0; j < 34; j++) {
      printf("%d", tronBoard[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

direction_t computeNextMove(bike_t *bike, bike_t *enemyBike) {
  direction_t choice = 2;
  return choice;
}

void initBoard() {
  for (uint32_t i = 0; i < 25; i++) {
    for (uint32_t j = 0; j < 34; j++) {
      tronBoard[i][j] = 0;
    }
  }
}
