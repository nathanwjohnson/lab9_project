#ifndef BIKES
#define BIKES

#include <stdbool.h>
#include <stdint.h>
#include "display.h"
#include "config.h"

typedef enum {
  LEFT,
  RIGHT,
  UP,
  DOWN
} direction_t;

typedef enum {
  PLAYER_1,
  PLAYER_2,
  COMPUTER_PLAYER
} bike_type_t;

typedef struct {

  uint16_t color;

  int32_t currentState;

  bike_type_t type;

  display_point_t current, next, light_current;
  display_point_t light[LIGHT_LENGTH];

  direction_t direction, light_direction, next_direction;

} bike_t;

void first_player_bike_init(bike_t *bike);

void second_player_bike_init(bike_t *bike);

void computer_bike_init(bike_t *bike);

////////// State Machine TICK Function //////////
void bike_tick(bike_t *bike, bike_t *enemyBike);

// Return whether the given bike is dead.
bool bike_has_lost(bike_t *bike);

void enemy_turn(bike_t *bike, direction_t direction);

#endif /* BIKES */
