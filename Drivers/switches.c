#include "switches.h"
#include "leds.h"
#include "xparameters.h"
#include "xil_io.h"

#define TRI_STATE_OFFSET 0x04
#define SET_TRI_STATE_AS_INPUT_BIT_MASK 0xf

static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the SWITCHES driver software and hardware.
void switches_init() {
    writeRegister(TRI_STATE_OFFSET, SET_TRI_STATE_AS_INPUT_BIT_MASK);
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read() {
    return readRegister(0x00);
}