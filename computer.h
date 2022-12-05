#ifndef COMPUTER
#define COMPUTER

#include <stdbool.h>
#include <stdint.h>

direction_t computeNextMove(bike_t *bike, bike_t *enemyBike);

void initBoard();

void fillSquare(uint32_t x, uint32_t y);

void clearSquare(uint32_t x, uint32_t y);

#endif /* COMPUTER */
