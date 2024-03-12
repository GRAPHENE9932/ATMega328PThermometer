#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define DS18B20_PORT PORTD
#define DS18B20_PIN PIND
#define DS18B20_DDR DDRD
#define DS18B20_PORT_BIT (0b00000001 << 2)

typedef uint16_t ds18b20_temp_t;

void ds18b20_start_measurement(void);
bool ds18b20_is_measurement_over(void);
ds18b20_temp_t ds18b20_get_temp(void);

uint8_t ds18b20_temp_t_to_string(ds18b20_temp_t temp, char* buffer, uint8_t buffer_size);