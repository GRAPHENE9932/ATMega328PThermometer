#pragma once

#include <avr/io.h>

#define SDA_PORT PORTC
#define SDA_DDR DDRC
#define SCL_PORT PORTC
#define SCL_DDR DDRC
#define SDA_PORT_BIT (0b00000001 << 4)
#define SCL_PORT_BIT (0b00000001 << 5)

void i2c_start_transmission(uint8_t slave_address);
void i2c_end_transmission(void);

void i2c_send_byte(char byte);