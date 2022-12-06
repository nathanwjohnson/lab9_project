#include "tronControl.h"
#include "buttons.h"
#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "touchscreen.h"
#include "bikes.h"

 #include <math.h>
 #include <stdbool.h>
 #include <stdint.h>
 #include <stdio.h>
 #include <string.h>

#define TEXT_SIZE 2
#define LEFT 0
#define RIGHT 1
#define TICK_PERIOD (CONFIG_GAME_TIMER_PERIOD)

// TRON states
enum tron_state_t {
  INIT_ST,
  DISPLAY_ST,
  GAME_MODE,
  COLOR_P1,
  COLOR_P2,
  GRID,
  TICK_GAME,
  WIN
};
static enum tron_state_t currentState;

static uint64_t delay_cnt = 0;
static uint64_t delay_num_ticks = 0;
static uint64_t grid_cnt = 0;
static uint64_t grid_num_ticks = 0;
static uint64_t win_cnt = 0;
static uint64_t win_num_ticks = 0;
bool ONE_PLAYER_GAME = true;
bool PLAYER_ONE_COLOR = true;
static uint64_t location = 0;

bike_t bike[2];
bike_t* first_bike = &bike[0];
bike_t* second_bike = &bike[1];


// Prints our debug statements to the console
static void debugStatePrint() {
  static enum tron_state_t previousState;
  static bool firstPass = true;

  // Prints to the console what state we are currently in as well as the x and y
  // coordinates of the points that we touch
  if ((previousState != currentState) || firstPass) {
    firstPass = false;
    previousState = currentState;
    // Each case is for a different state and prints to the console where we are
    // at
    switch (currentState) {
    case INIT_ST:
      printf("INIT_ST\n");
      break;
    case DISPLAY_ST:
      printf("DISPLAY_ST\n");
      break;
    case GAME_MODE:
      printf("GAME_MODE\n");
      break;
    case COLOR_P1:
      printf("COLOR_P1\n");
      break;
    case COLOR_P2:
      printf("COLOR_P2\n");
      break;
    case GRID:
      printf("GRID\n");
      break;
    case TICK_GAME:
      printf("TICK_GAME\n");
      break;
    case WIN:
      printf("WIN\n");
      break;
    }
  }
}


int tron_getLocationFromPoint(display_point_t point) {
  
  int16_t x = point.x;
  int16_t y = point.y;

  uint16_t midpoint = (320 / 2);

  if (x < midpoint) {
    location = LEFT;
  } else {
    location = RIGHT;
  }
}

static void printTitleScreen() {
  display_setTextSize(5);
  display_setTextColor(DISPLAY_YELLOW);
  display_setCursor(8, 35);
  display_print("WELCOME");
  display_setTextSize(2);
  display_print(" ");
  display_setTextSize(5);
  display_print("TO:");
  display_setTextSize(12);
  display_setTextColor(DISPLAY_CYAN);
  display_setCursor(20, 85);
  display_print("TRON");
  display_setCursor(35, 185);
  display_setTextSize(2);
  display_setTextColor(DISPLAY_YELLOW);
  display_print("INSERT 1 COIN TO PLAY");
}

static void eraseTitleScreen() {
  display_setTextSize(5);
  display_setTextColor(DISPLAY_BLACK);
  display_setCursor(8, 35);
  display_print("WELCOME");
  display_setTextSize(2);
  display_print(" ");
  display_setTextSize(5);
  display_print("TO:");
  display_setTextSize(12);
  display_setTextColor(DISPLAY_BLACK);
  display_setCursor(20, 85);
  display_print("TRON");
  display_setCursor(35, 185);
  display_setTextSize(2);
  display_setTextColor(DISPLAY_BLACK);
  display_print("INSERT 1 COIN TO PLAY");
}

static void printModeScreen() {
  display_drawLine(320 / 2, 50, 320 / 2, 240, DISPLAY_WHITE);
  display_drawLine(0, 50, 320, 50, DISPLAY_WHITE);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_YELLOW);
  display_setCursor(7, 20);
  display_print("Choose Game Mode:");
  display_setCursor(10, 95);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_CYAN);
  display_print("PLAYER\n");
  display_print("  VS.\n");
  display_setCursor(11, 143);
  display_print("COMPUTER\n");
  display_setCursor(190, 95);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_CYAN);
  display_print("PLAYER\n");
  display_setCursor(198, 118);
  display_print(" VS.\n");
  display_setCursor(190, 143);
  display_print("PLAYER\n");
}

static void eraseModeScreen() {
  display_drawLine(320 / 2, 50, 320 / 2, 240, DISPLAY_BLACK);
  display_drawLine(0, 50, 320, 50, DISPLAY_BLACK);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_BLACK);
  display_setCursor(7, 20);
  display_print("Choose Game Mode:");
  display_setCursor(10, 95);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_BLACK);
  display_print("PLAYER\n");
  display_print("  VS.\n");
  display_setCursor(11, 143);
  display_print("COMPUTER\n");
  display_setCursor(190, 95);
  display_setTextSize(3);
  display_setTextColor(DISPLAY_BLACK);
  display_print("PLAYER\n");
  display_setCursor(198, 118);
  display_print(" VS.\n");
  display_setCursor(190, 143);
  display_print("PLAYER\n");
}

static void printColorScreen() {
  display_fillRect(0, 50, 320 / 2, 95, DISPLAY_CYAN);
  display_fillRect(320 / 2, 50, 320, 95, DISPLAY_MAGENTA);
  display_fillRect(0, 145, 320 / 2, 240, DISPLAY_GREEN);
  display_fillRect(320 / 2, 145, 320, 240, DISPLAY_YELLOW);
  display_drawLine(320 / 2, -240, 320 / 2, 240, DISPLAY_BLACK);
  display_drawLine(320 / 2, -240, (320 / 2) - 1, 240, DISPLAY_BLACK);
  display_drawLine(320 / 2, -240, (320 / 2) + 1, 240, DISPLAY_BLACK);
  display_drawLine(0, 145, 320, 145, DISPLAY_BLACK);
  display_drawLine(0, 144, 320, 144, DISPLAY_BLACK);
  display_drawLine(0, 146, 320, 146, DISPLAY_BLACK);
  display_setTextSize(2);
  display_setTextColor(DISPLAY_YELLOW);
  display_setCursor(35, 20);
  if (PLAYER_ONE_COLOR == true) {
    display_print("CHOOSE PLAYER1 COLOR:");
  } else {
    display_print("CHOOSE PLAYER2 COLOR:");
  }
}

static void eraseColorScreen() {
  display_fillRect(0, 50, 320 / 2, 95, DISPLAY_BLACK);
  display_fillRect(320 / 2, 50, 320, 95, DISPLAY_BLACK);
  display_fillRect(0, 145, 320 / 2, 240, DISPLAY_BLACK);
  display_fillRect(320 / 2, 145, 320, 240, DISPLAY_BLACK);
  display_drawLine(320 / 2, -240, 320 / 2, 240, DISPLAY_BLACK);
  display_drawLine(320 / 2, -240, (320 / 2) - 1, 240, DISPLAY_BLACK);
  display_drawLine(320 / 2, -240, (320 / 2) + 1, 240, DISPLAY_BLACK);
  display_drawLine(0, 145, 320, 145, DISPLAY_BLACK);
  display_drawLine(0, 144, 320, 144, DISPLAY_BLACK);
  display_drawLine(0, 146, 320, 146, DISPLAY_BLACK);
  display_setTextSize(2);
  display_setTextColor(DISPLAY_BLACK);
  display_setCursor(35, 20);
  if (PLAYER_ONE_COLOR == true) {
    display_print("CHOOSE PLAYER1 COLOR:");
  } else {
    display_print("CHOOSE PLAYER2 COLOR:");
  }
}

static void showGrid() {
  for (uint16_t i = 0; i < 320; i = i + 10) {
    display_drawLine(i, 0, i, 240, DISPLAY_DARK_RED);
  }

  for (uint16_t i = 0; i < 240; i = i + 10) {
    display_drawLine(0, i, 320, i, DISPLAY_DARK_RED);
  }
}

static void eraseGrid() {
  for (uint16_t i = 0; i < 320; i = i + 10) {
    display_drawLine(i, 0, i, 240, DISPLAY_BLACK);
  }

  for (uint16_t i = 0; i < 240; i = i + 10) {
    display_drawLine(0, i, 320, i, DISPLAY_BLACK);
  }
}



// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void tronControl_init() {
  display_init(); 
  buttons_init();
  currentState = INIT_ST;
  delay_num_ticks = 2 / TICK_PERIOD;
  grid_num_ticks = 2 / TICK_PERIOD;
  }



// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void tronControl_tick() {
  debugStatePrint();

  //Mealy --> Transition
  switch (currentState) {
  case INIT_ST:
    printTitleScreen();
    currentState = DISPLAY_ST;
    break;
  case DISPLAY_ST:
    printf("Delay Count: %ld\n", delay_cnt);
    if (delay_cnt == delay_num_ticks) {
      eraseTitleScreen();
      delay_cnt = 0;
      currentState = GAME_MODE;
      printModeScreen();
    }
    break;
  case GAME_MODE:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      eraseModeScreen();
      if ((tron_getLocationFromPoint(touchscreen_get_location())) == LEFT) {
          ONE_PLAYER_GAME = true;
          first_player_bike_init(first_bike);
          computer_bike_init(second_bike);
      } else {
          ONE_PLAYER_GAME = false;
          first_player_bike_init(first_bike);
          second_player_bike_init(second_bike);
      }
      currentState = COLOR_P1;
      PLAYER_ONE_COLOR = true;
      touchscreen_ack_touch();
      printColorScreen();
    }
    break;
  case COLOR_P1:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      eraseColorScreen();
      if ((tron_getLocationFromPoint(touchscreen_get_location())) == LEFT) {
          first_bike->color = DISPLAY_WHITE;
      } else {
          first_bike->color = DISPLAY_WHITE;
      }
      currentState = COLOR_P2;
      PLAYER_ONE_COLOR = false;
      touchscreen_ack_touch();
      printColorScreen();
    }
    break;
  case COLOR_P2:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      eraseColorScreen();
      if ((tron_getLocationFromPoint(touchscreen_get_location())) == LEFT) {
          second_bike->color = DISPLAY_WHITE;
      } else {
          second_bike->color = DISPLAY_WHITE;
      }
      currentState = GRID;
      touchscreen_ack_touch();
      showGrid();
    }
    break;
  case GRID:
    if (grid_cnt == grid_num_ticks) {
      eraseGrid();
      grid_cnt = 0;
      currentState = TICK_GAME;
    }
    break;
  case TICK_GAME:
    break;
  case WIN:
    break;
  }

  // Moore --> State
  switch (currentState) {
  case DISPLAY_ST:
    delay_cnt++;
    break;
  case GAME_MODE:
    break;
  case COLOR_P1:
    break;
  case COLOR_P2:
    break;
  case GRID:
    grid_cnt++;
    break;
  case TICK_GAME:
    bike_tick(first_bike, second_bike);
    bike_tick(second_bike, first_bike);
    break;
  case WIN:
    win_cnt++;
    break;
  }
};
