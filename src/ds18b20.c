#include "ds18b20.h"
#include "utils.h"
#include <util/delay.h>

// DS18B20 datasheet:
// https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf

const char SKIP_ROM_COMMAND = 0xCC;
const char CONVERT_T_COMMAND = 0x44;
const char READ_SCRATCHPAD_COMMAND = 0xBE;
#define SCRATCHPAD_SIZE 9

uint8_t last_read_scratchpad[SCRATCHPAD_SIZE];

static void write_bit(bool bit) {
    DS18B20_DDR |= DS18B20_PORT_BIT;
    DS18B20_PORT &= ~DS18B20_PORT_BIT;

    // I'm using two separate _delay_us calls instead of one with ternary operator
    // inside as _delay_us argument must be known at compile time.
    if (bit) {
        _delay_us(10.0);
    }
    else {
        _delay_us(65.0);
    }

    DS18B20_DDR &= ~DS18B20_PORT_BIT;
    
    if (bit) {
        _delay_us(55.0);
    }
    else {
        _delay_us(10.0);
    }
}

static void write_byte(char byte) {
    for (char i = 0; i < 8; i++) {
        write_bit(byte & 0b00000001);
        byte >>= 1;
    }
}

static bool read_bit(void) {
    DS18B20_DDR |= DS18B20_PORT_BIT;
    DS18B20_PORT &= ~DS18B20_PORT_BIT;
    _delay_us(2.0);
    DS18B20_DDR &= ~DS18B20_PORT_BIT;
    _delay_us(10.0);
    bool result = DS18B20_PIN & DS18B20_PORT_BIT;
    _delay_us(53.0);
    return result;
}

static uint8_t read_byte(void) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        result |= read_bit() << i;
    }
    return result;
}

static void reset(void) {
    DS18B20_DDR |= DS18B20_PORT_BIT;
    DS18B20_PORT &= ~DS18B20_PORT_BIT;
    _delay_us(500.0);
    DS18B20_DDR &= ~DS18B20_PORT_BIT;
    _delay_us(500.0);
}

static ds18b20_temp_t get_temp_from_scratchpad(void) {
    uint8_t lsb = last_read_scratchpad[0];
    uint8_t msb = last_read_scratchpad[1];

    return (ds18b20_temp_t)lsb | (ds18b20_temp_t)msb << 8;
}

void ds18b20_start_measurement(void) {
    reset();
    write_byte(SKIP_ROM_COMMAND);
    write_byte(CONVERT_T_COMMAND);
}

bool ds18b20_is_measurement_over(void) {
    return read_bit();
}

ds18b20_temp_t ds18b20_get_temp(void) {
    reset();
    write_byte(SKIP_ROM_COMMAND);
    write_byte(READ_SCRATCHPAD_COMMAND);

    for (uint8_t i = 0; i < SCRATCHPAD_SIZE; i++) {
        last_read_scratchpad[i] = read_byte();
    }

    return get_temp_from_scratchpad();
}

const uint16_t FRACTIONAL_BINARY_COEFFICIENTS[4] = {
    625,
    1250,
    2500,
    5000
};

uint8_t ds18b20_temp_t_to_string(ds18b20_temp_t temp, char* buffer, uint8_t buffer_size) {
    uint8_t off = 0;
    if (temp & 0b1000000000000000) {
        buffer[off++] = '-';
        temp = ~temp;
    }

    uint8_t integer_part = temp >> 4;
    off += uint16_to_str(integer_part, buffer + off, buffer_size - off, 5, true);
    
    buffer[off++] = '.';

    uint16_t fractional_part = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (temp & 1) {
            fractional_part += FRACTIONAL_BINARY_COEFFICIENTS[i];
        }
        temp >>= 1;
    }

    off += uint16_to_str(fractional_part, buffer + off, buffer_size - off, 4, false);

    return off;
}