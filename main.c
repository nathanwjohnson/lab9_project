#include <stdio.h>

#include "bikes.h"
#include "buttons.h"
#include "computer.h"
#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "switches.h"
#include "tronControl.h"

bike_t bike[2];

void isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  if (bike_has_lost(&bike[0])) {
    // printf("player 1 wins\n");
  } else if (bike_has_lost(&bike[1])) {
    // printf("player 0 wins\n");
  }
  bike_tick(&bike[0], &bike[1]);
  bike_tick(&bike[1], &bike[0]);
}

void main() {
  display_init();
  buttons_init();
  display_fillScreen(DISPLAY_BLACK);

  for (uint16_t i = 0; i < 320; i = i + 10) {
    //display_drawLine(i, 0, i, 240, DISPLAY_DARK_RED);
  }

  for (uint16_t i = 0; i < 240; i = i + 10) {
    //display_drawLine(0, i, 320, i, DISPLAY_DARK_RED);
  }

  // Initialize timer interrupts
  interrupts_init();
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);

  intervalTimer_initCountDown(INTERVAL_TIMER_0, CONFIG_GAME_TIMER_PERIOD);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);

  initBoard();
  first_player_bike_init(&bike[0]);
  // second_player_bike_init(&bike[1]);
  computer_bike_init(&bike[1]);

  // Main game loop
  while (1)
    ;
}
