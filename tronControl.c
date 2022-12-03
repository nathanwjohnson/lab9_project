#include "tronControl.h"
#include "config.h"
#include "display.h"
#include "touchscreen.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void tronControl_init() { display_fillScreen(DISPLAY_BLACK); }

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void tronControl_tick() {}
