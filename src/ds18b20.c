#include "ds18b20.h"
#include <util/delay.h>

// DS18B20 datasheet:
// https://www.analog.com/media/en/technical-documentation/data-sheets/ds18b20.pdf

const char SKIP_ROM_COMMAND = 0xCC;
const char CONVERT_T_COMMAND = 0x44;
const char READ_SCRATCHPAD_COMMAND = 0xBE;
#define SCRATCHPAD_SIZE 9

unsigned char last_read_scratchpad[SCRATCHPAD_SIZE];

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

static unsigned char read_byte(void) {
    unsigned char result = 0;
    for (unsigned char i = 0; i < 8; i++) {
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
    unsigned char lsb = last_read_scratchpad[0];
    unsigned char msb = last_read_scratchpad[1];

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

    for (unsigned char i = 0; i < SCRATCHPAD_SIZE; i++) {
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

static unsigned char short_to_string(uint16_t num, char* buffer, unsigned char buffer_size, bool skip_zeroes) {
    if (buffer_size == 0) {
        return 0;
    }

    unsigned char off = 0;

    bool non_zero_encountered = false;
    uint16_t cur_divisor = 1000; // The maximum possible value is 9999 (4 digits).
    for (unsigned char i = 0; i < 4; i++) {
        unsigned char cur_digit = num / cur_divisor % 10;
        cur_divisor /= 10;

        if (cur_digit == 0 && !non_zero_encountered && skip_zeroes) {
            continue;
        }

        if (off >= buffer_size) {
            return off;
        }
        buffer[off++] = '0' + cur_digit;
    }

    return off;
}

unsigned char ds18b20_temp_t_to_string(ds18b20_temp_t temp, char* buffer, unsigned char buffer_size) {
    unsigned char off = 0;
    if (temp & 0b1000000000000000) {
        buffer[off++] = '-';
        temp = ~temp;
    }

    unsigned char integer_part = temp >> 4;
    off += short_to_string(integer_part, buffer + off, buffer_size - off, true);
    
    buffer[off++] = '.';

    uint16_t fractional_part = 0;
    for (unsigned char i = 0; i < 4; i++) {
        if (temp & 1) {
            fractional_part += FRACTIONAL_BINARY_COEFFICIENTS[i];
        }
        temp >>= 1;
    }

    off += short_to_string(fractional_part, buffer + off, buffer_size - off, false);

    return off;
}