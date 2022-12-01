#ifndef TOUCHSCREEN
#define TOUCHSCREEN

#include "display.h"

// Status of the touchscreen
typedef enum {
  TOUCHSCREEN_IDLE,    // Touchscreen is idle (not pressed)
  TOUCHSCREEN_PRESSED, // Touchscreen is actively being pressed
  TOUCHSCREEN_RELEASED // Touchscreen has been released, but not acknowledged
} touchscreen_status_t;

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds);

// Tick the touchscreen driver state machine
void touchscreen_tick();

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status();

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch();

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location();

#endif /* TOUCHSCREEN */
