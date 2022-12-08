#include "bikes.h"
#include "buttons.h"
#include "computer.h"
#include "config.h"
#include "display.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define GRID_WIDTH 10
#define GRID_X (DISPLAY_WIDTH / GRID_WIDTH)
#define GRID_Y (DISPLAY_HEIGHT / GRID_WIDTH)
#define DIRECTIONS 4
#define BIKE_1_START_X 40
#define BIKE_1_START_Y 120
#define BIKE_2_START_X 280
#define BIKE_2_START_Y 120
#define LAST_LIGHT (LIGHT_LENGTH - 1)

// DISPLAY_WIDTH 320
// DISPLAY_HEIGHT 240

uint8_t BUTTON_VALUES;
uint16_t counter = 0;

enum bike_st_t { INIT_ST, MOVING_ST, LOST_ST, WIN_ST };

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
  } else if (bike->light[LAST_LIGHT].y < bike->light_current.y) {
    bike->light_direction = UP;
  }
}

void update_light(bike_t *bike) {
  for (int16_t i = LAST_LIGHT; i > 0; i--) {
    bike->light[i] = bike->light[i - 1];
  }
  bike->light[0] = bike->current;
}

void first_player_bike_init(bike_t *bike) {
  counter = 0;
  bike->current.x = BIKE_1_START_X;
  bike->current.y = BIKE_1_START_Y;

  bike->direction = RIGHT;
  bike->next.x = BIKE_1_START_X;
  bike->next.y = BIKE_1_START_Y;
  bike->next_direction = DOWN;
  bike_next(bike);

  for (uint16_t i = 0; i < LIGHT_LENGTH; i++) {
    bike->light[i].x = BIKE_1_START_X;
    bike->light[i].y = BIKE_1_START_Y;
  }

  bike->light_current.x = bike->current.x = BIKE_1_START_X - 10;
  bike->light_current.y = bike->current.y = BIKE_1_START_Y;

  bike->light_direction = RIGHT;

  //bike->color = DISPLAY_CYAN;
  bike->currentState = INIT_ST;

  bike->type = PLAYER_1;
}

void second_player_bike_init(bike_t *bike) {
  counter = 0;
  bike->current.x = BIKE_2_START_X;
  bike->current.y = BIKE_2_START_Y;

  bike->direction = LEFT;
  bike->next.x = BIKE_2_START_X;
  bike->next.y = BIKE_2_START_Y;
  bike->next_direction = DOWN;
  bike_next(bike);

  for (uint16_t i = 0; i < LIGHT_LENGTH; i++) {
    bike->light[i].x = BIKE_2_START_X;
    bike->light[i].y = BIKE_2_START_Y;
  }

  bike->light_current.x = bike->current.x = BIKE_2_START_X + 10;
  bike->light_current.y = bike->current.y = BIKE_2_START_Y;

  bike->light_direction = LEFT;

  //bike->color = DISPLAY_MAGENTA;
  bike->currentState = INIT_ST;

  bike->type = PLAYER_2;
}

void computer_bike_init(bike_t *bike) {
  counter = 0;
  bike->current.x = BIKE_2_START_X;
  bike->current.y = BIKE_2_START_Y;

  bike->direction = LEFT;
  bike->next.x = BIKE_2_START_X;
  bike->next.y = BIKE_2_START_Y;
  bike->next_direction = DOWN;
  bike_next(bike);

  for (uint16_t i = 0; i < LIGHT_LENGTH; i++) {
    bike->light[i].x = BIKE_2_START_X;
    bike->light[i].y = BIKE_2_START_Y;
  }

  bike->light_current.x = bike->current.x = BIKE_2_START_X + 10;
  bike->light_current.y = bike->current.y = BIKE_2_START_Y;

  bike->light_direction = LEFT;

  //bike->color = DISPLAY_MAGENTA;
  bike->currentState = INIT_ST;

  bike->type = COMPUTER_PLAYER;
}

////////// State Machine TICK Function //////////
void bike_tick(bike_t *bike, bike_t *enemyBike) {
  // printf("Entered bike_tick\n");

  // State update and mealy operations of the bike_tick state machine
  switch (bike->currentState) {
  // moves from init to moving
  case INIT_ST:
    // makes bike2 wait 5 ticks until it starts
    if (bike->type == PLAYER_1 || counter >= 5) {
      bike->currentState = MOVING_ST;
      counter = 0;
    } else {
      counter++;
    }
    // printf("Entered INIT_ST\n");
    break;
  case MOVING_ST:
    // printf("Entered MOVING_ST\n");

    if (bike->current.x <= 0 || bike->current.x >= 320 ||
        bike->current.y <= 0 || bike->current.y >= 240) {
      bike->currentState = LOST_ST;
      enemyBike->currentState = WIN_ST;
    }

    BUTTON_VALUES = buttons_read();
    // printf("Buttons Pushed: %d\n", buttons_read());

    // printf("NEXT.X: %d\n", bike->next.x);
    if (abs(bike->current.x - bike->next.x) > 7 ||
        abs(bike->current.y - bike->next.y) > 7) {
      if ((BUTTON_VALUES == 4) && (bike->type == PLAYER_1)) {
        bike->next_direction = RIGHT;
      } else if ((BUTTON_VALUES == 8) && (bike->type == PLAYER_1)) {
        bike->next_direction = LEFT;
      }

      if ((BUTTON_VALUES == 1) && (bike->type == PLAYER_2)) {
        bike->next_direction = RIGHT;
      } else if ((BUTTON_VALUES == 2) && (bike->type == PLAYER_2)) {
        bike->next_direction = LEFT;
      }
    }

    if (bike->current.x == bike->next.x && bike->current.y == bike->next.y) {
      for (uint16_t i = 0; i < LIGHT_LENGTH; i++) {
        // checks for collisions with enemy light trail
        if (bike->current.x == enemyBike->light[i].x &&
            bike->current.y == enemyBike->light[i].y) {
          bike->currentState = LOST_ST;
          enemyBike->currentState = WIN_ST;
          // checks for collisions with your own light trail
        } else if (bike->current.x == bike->light[i].x &&
                   bike->current.y == bike->light[i].y) {
          bike->currentState = LOST_ST;
          enemyBike->currentState = WIN_ST;
        }
      }

      // uint64_t color = rand() % 4;
      // if (color == 1) {
      //   color = DISPLAY_GREEN;
      // } else if (color == 2) {
      //   color = DISPLAY_RED;
      // }else if (color == 3) {
      //   color = DISPLAY_YELLOW;
      // }else if (color == 4) {
      //   color = DISPLAY_WHITE;
      // }

      /*for (int16_t i = 1; i < LIGHT_LENGTH; i++) {
        display_drawLine(bike->light[i].x, bike->light[i].y,
                         bike->light[i - 1].x, bike->light[i - 1].y, color);
      } */

      if (bike->type == COMPUTER_PLAYER) {
        bike->next_direction = computeNextMove(bike, enemyBike);
      }

      fillSquare(bike->current.x, bike->current.y);

      enemy_turn(bike, bike->next_direction);
      // update light
      update_light(bike);
      clearSquare(bike->light[LAST_LIGHT].x, bike->light[LAST_LIGHT].y);
      // update light direction
      light_direction(bike);
      // update bike next destination
      bike_next(bike);

      fillSquare(bike->next.x, bike->next.y);

      bike->next_direction = DOWN;
    }
    break;
  // it is dead do nothing till initialized
  case LOST_ST:
    /*for (int16_t i = 1; i < LIGHT_LENGTH; i++) {
          display_drawLine(bike->light[i].x, bike->light[i].y,
                           bike->light[i - 1].x, bike->light[i - 1].y, color);
        } */
    break;
  case WIN_ST:
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
    if ((bike->light[LAST_LIGHT].x != bike->light[LAST_LIGHT - 1].x) ||
        (bike->light[LAST_LIGHT].y != bike->light[LAST_LIGHT - 1].y)) {
      switch (bike->light_direction) {
      case LEFT:
        bike->light_current.x -= CONFIG_BIKE_DISTANCE_PER_TICK;
        break;
      case RIGHT:
        bike->light_current.x += CONFIG_BIKE_DISTANCE_PER_TICK;
        break;
      case UP:
        bike->light_current.y -= CONFIG_BIKE_DISTANCE_PER_TICK;
        break;
      case DOWN:
        bike->light_current.y += CONFIG_BIKE_DISTANCE_PER_TICK;
        break;
      }
    }
    // uint64_t color = rand() % 4;
    // if (color == 1) {
    //   color = DISPLAY_GREEN;
    // } else if (color == 2) {
    //   color = DISPLAY_RED;
    // }else if (color == 3) {
    //   color = DISPLAY_YELLOW;
    // }else if (color == 4) {
    //   color = DISPLAY_WHITE;
    // }
    display_drawLine(bike->current.x, bike->current.y, last_point.x,
                     last_point.y, bike->color);
    // printf("bike->light[LAST_LIGHT].x %d\n", bike->light[LAST_LIGHT].x);
    // if (counter > LIGHT_LENGTH * 10) {
    // printf("\n");
    // for (int16_t i = 1; i < LIGHT_LENGTH; i++) {
    //   display_drawLine(bike->light[i].x + 1, bike->light[i].y + 1,
    //                    bike->light[i].x + 1, bike->light[i].y + 1,
    //                    DISPLAY_RED);
    // }
    // display_drawLine(bike->light[LAST_LIGHT].x + 1,
    //                  bike->light[LAST_LIGHT].y + 1,
    //                  bike->light[LAST_LIGHT].x + 1,
    //                  bike->light[LAST_LIGHT].y + 1, DISPLAY_BLACK);
    // printf(" %d %d\n", bike->light[LAST_LIGHT].x,
    // bike->light[LAST_LIGHT-1].x);
    if ((bike->light[LAST_LIGHT].x != bike->light[LAST_LIGHT - 1].x) ||
        (bike->light[LAST_LIGHT].y != bike->light[LAST_LIGHT - 1].y)) {
      // printf("here\n");
      display_drawLine(bike->light_current.x, bike->light_current.y,
                       last_light.x, last_light.y, DISPLAY_BLACK);
    } else if (counter < 20) {
      display_drawLine(BIKE_1_START_X, BIKE_1_START_Y, BIKE_1_START_X - 10,
                       BIKE_1_START_Y, DISPLAY_BLACK);
      display_drawLine(BIKE_2_START_X, BIKE_2_START_Y, BIKE_2_START_X + 10,
                       BIKE_2_START_Y, DISPLAY_BLACK);
      counter++;
    }
    // } else if (counter < 2) {
    //   if (bike->type == PLAYER_1) {
    //     display_drawLine(last_light.x - 10, last_light.y, last_light.x,
    //                      last_light.y, bike->color);
    //   } else {
    //     display_drawLine(last_light.x + 10, last_light.y, last_light.x,
    //                      last_light.y, bike->color);
    //   }
    //   counter++;
    // } else {
    //   counter++;
    // }
    break;
  case LOST_ST:
    // display_drawLine(bike->light_current.x, bike->light_current.y,
    // last_light.x,
    //                  last_light.y, DISPLAY_BLACK);
    // printf("bike->light[LAST_LIGHT].x %d\n", counter);
    if (counter < 5) {
      if ((bike->light[LAST_LIGHT].x != bike->light[LAST_LIGHT - 1].x) ||
          (bike->light[LAST_LIGHT].y != bike->light[LAST_LIGHT - 1].y)) {
        display_drawLine(bike->light_current.x, bike->light_current.y,
                         bike->light[LAST_LIGHT].x, bike->light[LAST_LIGHT].y,
                         DISPLAY_BLACK);
      }
      for (int16_t i = 1; i < LIGHT_LENGTH; i++) {
        display_drawLine(bike->light[i].x, bike->light[i].y,
                         bike->light[i - 1].x, bike->light[i - 1].y,
                         DISPLAY_BLACK);
      }
      counter++;
    } else if (counter < 10) {
      if ((bike->light[LAST_LIGHT].x != bike->light[LAST_LIGHT - 1].x) ||
          (bike->light[LAST_LIGHT].y != bike->light[LAST_LIGHT - 1].y)) {
        display_drawLine(bike->light_current.x, bike->light_current.y,
                         bike->light[LAST_LIGHT].x, bike->light[LAST_LIGHT].y,
                         DISPLAY_RED);
      }
      for (int16_t i = 1; i < LIGHT_LENGTH; i++) {
        display_drawLine(bike->light[i].x, bike->light[i].y,
                         bike->light[i - 1].x, bike->light[i - 1].y,
                         DISPLAY_RED);
      }
      counter++;
    } else {
      counter = 0;
    }
    break;
  case WIN_ST:
    break;
  }
}

// Return whether the given bike is dead.
bool bike_has_lost(bike_t *bike) {
  // printf("bike->light[LAST_LIGHT].x %d\n", bike->currentState);
  if (bike->currentState == LOST_ST)
    return true;
  else {
    return false;
  }
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
      bike->direction = LEFT;
      break;
    }
  }
}