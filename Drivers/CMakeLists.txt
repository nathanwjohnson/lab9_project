add_library(buttons_switches buttons.c switches.c)
target_link_libraries(buttons_switches ${330_LIBS})

# add_library(intervalTimer intervalTimer.c)
# target_link_libraries(intervalTimer ${330_LIBS})

# add_library(interrupts interrupts.c)
# target_link_libraries(interrupts ${330_LIBS})

# add_library(touchscreen touchscreen.c)
# target_link_libraries(touchscreen ${330_LIBS})