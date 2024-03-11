cmake_minimum_required(VERSION 3.11)

project("arduino_uno_thermometer")

set(F_CPU 16000000ul)
set(MCU atmega328p)
set(PROG_TYPE arduino)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

add_definitions(
    -DF_CPU=${F_CPU}
)

set(CMAKE_EXE_LINKER_FLAGS -mmcu=${MCU})

add_compile_options(
    -mmcu=${MCU}
    -std=gnu11
    -Os
    -Wall
    -Wno-main
    -Wundef
    -Werror
    -Wfatal-errors
    -Wl,--relax,--gc-sections
)

add_executable(arduino_uno_thermometer main.c i2c.c)

set_target_properties(arduino_uno_thermometer PROPERTIES OUTPUT_NAME arduino_uno_thermometer.o)

add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${MCU} -U flash:w:arduino_uno_thermometer.o -P ${PROG_PORT})