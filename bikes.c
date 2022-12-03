#include "bikes.h"
#include "config.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>

#define GRID_WIDTH 10
#define GRID_X (DISPLAY_WIDTH / GRID_WIDTH)
#define GRID_Y (DISPLAY_HEIGHT / GRID_WIDTH)
#define DIRECTIONS 4
#define BIKE_1_START_X 100
#define BIKE_1_START_Y 120
#define BIKE_2_START_X 300
#define BIKE_2_START_Y 120
#define LAST_LIGHT (LIGHT_LENGTH - 1)

// DISPLAY_WIDTH 320
// DISPLAY_HEIGHT 240

enum bike_st_t { INIT_ST, MOVING_ST, DYING_ST, DEAD_ST };

display_point_t last_point, last_light;

void bike_next(bike_t *bike) {
  switch (bike->direction) {
  case LEFT:
    bike->next.x -= GRID_WIDTH;
    break;
  case RIGHT:
    bike->next.x += GRID_WIDTH;
    break;
  case UP:
    bike->next.y -= GRID_WIDTH;
    break;
  case DOWN:
    bike->next.y += GRID_WIDTH;
    break;
  }
}

void light_direction(bike_t *bike) {
  if (bike->light[LAST_LIGHT].x > bike->light_current.x) {
    bike->light_direction = RIGHT;
  } else if (bike->light[LAST_LIGHT].x < bike->light_current.x) {
    bike->light_direction = LEFT;
  } else if (bike->light[LAST_LIGHT].y > bike->light_current.y) {
    bike->light_direction = DOWN;
  } else {
    bike->light_direction = UP;
  }
}

void update_light(bike_t *bike) {
  for (int16_t i = LAST_LIGHT; i > 0; i--) {
    bike->light[i] = bike->light[i - 1];
  }
  bike->light[0] = bike->current;
}

void first_bike_init(bike_t *bike) {
  bike->current.x = BIKE_1_START_X;
  bike->current.y = BIKE_1_START_Y;

  bike->direction = RIGHT;
  bike->next.x = BIKE_1_START_X;
  bike->next.y = BIKE_1_START_Y;
  bike_next(bike);

  bike->color = DISPLAY_CYAN;
  bike->currentState = INIT_ST;
}

void second_bike_init(bike_t *bike) {
  bike->current.x = BIKE_2_START_X;
  bike->current.y = BIKE_2_START_Y;

  bike->direction = LEFT;
  bike->next.x = BIKE_2_START_X;
  bike->next.y = BIKE_2_START_Y;
  bike_next(bike);

  bike->color = DISPLAY_MAGENTA;
  bike->currentState = INIT_ST;
}

////////// State Machine TICK Function //////////
void bike_tick(bike_t *bike, display_point_t *enemy_light) {
  // State update and mealy operations of the bike_tick state machine
  switch (bike->currentState) {
  // moves from init to moving
  case INIT_ST:
    bike->currentState = MOVING_ST;
    break;
  case MOVING_ST:
    if (bike->current.x == bike->next.x && bike->current.y == bike->next.y) {
      for (uint16_t i = 0; i < LIGHT_LENGTH; i++) {
        if (bike->current.x == enemy_light[i].x &&
            bike->current.y == enemy_light[i].y) {
          bike->currentState = DYING_ST;
          break;
        }
      }
      // update light direction
      light_direction(bike);
      // update bike next destination
      bike_next(bike);
      // update light
      update_light(bike);
    }
    break;
  // it is dead do nothing till initialized
  case DYING_ST:
    break;
  case DEAD_ST:
    break;
  }

  // Perform moore state action next.
  switch (bike->currentState) {
  case INIT_ST:
    break;
  case MOVING_ST:
    last_point = bike->current;
    last_light = bike->light_current;
    switch (bike->direction) {
    case LEFT:
      bike->current.x -= CONFIG_BIKE_DISTANCE_PER_TICK;
      break;
    case RIGHT:
      bike->current.x += CONFIG_BIKE_DISTANCE_PER_TICK;
      break;
    case UP:
      bike->current.y -= CONFIG_BIKE_DISTANCE_PER_TICK;
      break;
    case DOWN:
      bike->current.y += CONFIG_BIKE_DISTANCE_PER_TICK;
      break;
    }
    display_drawLine(bike->current.x, bike->current.y, last_point.x,
                     last_point.y, bike->color);
    display_drawLine(bike->light_current.x, bike->light_current.y, last_light.x,
                     last_light.y, DISPLAY_BLACK);
    break;
  case DYING_ST:
    break;
  case DEAD_ST:
    break;
  }
}

// Return whether the given bike is dead.
bool bike_is_dead(bike_t *bike) {
  if (bike->currentState == DEAD_ST)
    return true;
}

void enemy_turn(bike_t *bike, direction_t direction) {
  if (direction == LEFT) {
    switch (bike->direction) {
    case LEFT:
      bike->direction = DOWN;
      break;
    case RIGHT:
      bike->direction = UP;
      break;
    case UP:
      bike->direction = LEFT;
      break;
    case DOWN:
      bike->direction = RIGHT;
      break;
    }
  }
  if (direction == RIGHT) {
    switch (bike->direction) {
    case LEFT:
      bike->direction = UP;
      break;
    case RIGHT:
      bike->direction = DOWN;
      break;
    case UP:
      bike->direction = RIGHT;
      break;
    case DOWN:
      bike->direction = RIGHT;
      break;
    }
  }
}