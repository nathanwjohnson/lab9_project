#ifndef COMPUTER
#define COMPUTER

#include <stdbool.h>
#include <stdint.h>

direction_t computeNextMove(bike_t *bike, bike_t *enemyBike);

void initBoard();

#endif /* COMPUTER */
