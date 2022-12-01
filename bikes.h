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
  DOWN,
} direction_t;

typedef struct {

  uint16_t color;

  int32_t currentState;

  display_point_t current, next, light_current;
  display_point_t light[LIGHT_LENGTH];

  direction_t direction, light_direction;

} bike_t;

void player_bike_init(bike_t *bike);

void enemy_bike_init(bike_t *bike);

////////// State Machine TICK Function //////////
void bike_tick(bike_t *bike);

// Return whether the given missile is dead.
bool bike_is_dead(bike_t *bike);

void enemy_turn(bike_t *bike);

#endif /* BIKES */
