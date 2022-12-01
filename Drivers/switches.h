/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef SWITCHES
#define SWITCHES

#include <stdint.h>

#define SWITCHES_SW0_MASK 0x1
#define SWITCHES_SW1_MASK 0x2
#define SWITCHES_SW2_MASK 0x4
#define SWITCHES_SW3_MASK 0x8

// Initializes the SWITCHES driver software and hardware.
void switches_init();

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read();

#endif /* SWITCHES */
