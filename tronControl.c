#include "tronControl.h"
#include "config.h"
#include "display.h"
#include "touchscreen.h"
#include "buttons.h"
#include "interrupts.h"
#include "intervalTimer.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define TEXT_SIZE 2

// TRON states
enum tron_state_t {
    DISPLAY, GAME_MODE, COLOR_P1, COLOR_P2, GRID, TICK_GAME, WIN
};
static enum tron_state_t currentState;

static uint64_t delay_cnt = 0;
static uint64_t delay_num_ticks = 0;
static uint64_t grid_cnt = 0;
static uint64_t grid_num_ticks = 0;
static uint64_t win_cnt = 0;
static uint64_t win_num_ticks = 0;


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
    if (currentState = PLAYER_COLOR) {
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
    display_print("CHOOSE PLAYER1 COLOR:");
    display_print("CHOOSE PLAYER2 COLOR:");
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
void tronControl_init() { display_fillScreen(DISPLAY_BLACK); }

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void tronControl_tick() {
    // Mealy --> Transition
    switch (currentState) (
        case DISPLAY:
        if (delay_cnt == delay_num_ticks) {
            eraseTitleScreen();
            delay_cnt = 0;
            currentState = DISPLAY;
        }
        break;
        case GAME_MODE:
        if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {

        }
        break;
        case COLOR_P1:
        break;
        case COLOR_P2:
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
    )

    // Moore --> State
    switch (currentState) (
        case DISPLAY:
        delay_cnt++;
        printTitleScreen();
        break;
        case GAME_MODE:
        printModeScreen();
        break;
        case COLOR_P1:
        printColorScreen();
        break;
        case COLOR_P2:
        printColorScreen();
        break;
        case GRID:
        grid_cnt++;
        showGrid();
        break;
        case TICK_GAME:
        break;
        case WIN:
        win_cnt++;
        break;
    )
};
