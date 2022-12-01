#include "buttons.h"
#include "leds.h"
#include "xparameters.h"
#include "xil_io.h"

#define TRI_STATE_OFFSET 0x04
#define SET_TRI_STATE_AS_INPUT_BIT_MASK 0xf

static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the button driver software and hardware.
void buttons_init() {
    writeRegister(TRI_STATE_OFFSET, SET_TRI_STATE_AS_INPUT_BIT_MASK);
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read() {
    return readRegister(0x00);
}