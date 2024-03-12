cmake_minimum_required(VERSION 3.11)

project("atmega328p_thermometer")

set(F_CPU 16000000ul)
set(MCU atmega328p)
set(PROG_TYPE arduino)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_CXX_COMPILER avr-g++)
set(CMAKE_C_COMPILER avr-gcc)
set(CMAKE_ASM_COMPILER avr-gcc)

add_definitions(-DF_CPU=${F_CPU})

if (DEFINED REDEFINED_DS18B20_PORT)
add_definitions(-DREDEFINED_DS18B20_PORT)
add_definitions(-DDS18B20_PORT=${DS18B20_PORT})
add_definitions(-DDS18B20_PORT_BIT=${DS18B20_PORT_BIT})
add_definitions(-DDS18B20_PIN=${DS18B20_PIN})
add_definitions(-DDS18B20_DDR=${DS18B20_DDR})
endif()

if (DEFINED REDEFINED_I2C_PORTS)
add_definitions(-DREDEFINED_I2C_PORTS)
add_definitions(-DSDA_PORT=${SDA_PORT})
add_definitions(-DSDA_DDR=${SDA_DDR})
add_definitions(-DSDA_PORT_BIT=${SDA_PORT_BIT})
add_definitions(-DSCL_PORT=${SCL_PORT})
add_definitions(-DSCL_DDR=${SCL_DDR})
add_definitions(-DSCL_PORT_BIT=${SCL_PORT_BIT})
endif()

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
    --param=min-pagesize=0
    -Wl,--relax,--gc-sections
)

add_executable(atmega328p_thermometer src/main.c src/i2c.c src/hd44780.c src/ds18b20.c src/utils.c)

set_target_properties(atmega328p_thermometer PROPERTIES OUTPUT_NAME atmega328p_thermometer.o)

add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${MCU} -U flash:w:atmega328p_thermometer.o -P ${PROG_PORT})