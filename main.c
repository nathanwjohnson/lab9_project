#include <stdio.h>

#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "buttons.h"
#include "switches.h"
#include "bikes.h"
#include "computer.h"
#include "tronControl.h"
#include "intervalTimer.h"

bike_t bike[2];

void isr() {
    intervalTimer_ackInterrupt(INTERVAL_TIMER_0);

    for (uint16_t i = 0; i < 2; i++) {
    bike_tick(&bike[i]);
    }

}

void main() {
    printf("It's working\n");

    display_init();
    buttons_init();
    display_fillScreen(DISPLAY_BLACK);

    // Initialize timer interrupts
    interrupts_init();
    interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
    interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
    interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);

    intervalTimer_initCountDown(INTERVAL_TIMER_0, CONFIG_GAME_TIMER_PERIOD);
    intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_0);

    first_bike_init(&bike[0]);
    //second_bike_init(&bike[1]);

    for (uint16_t i = 0; i < 2; i++)
        bike_tick(&bike[i]);

    // Main game loop
    while (1)
        ;
}
