add_executable(lab9.elf main.c bikes.c computer.c tronControl.c)
target_link_libraries(lab9.elf ${330_LIBS} interrupts intervalTimer buttons_switches touchscreen)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)
