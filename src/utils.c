#include "utils.h"

static uint16_t calc_first_divisor(uint8_t digits) {
    uint16_t result = 1;
    for (uint8_t i = 0; i < digits - 1; i++) {
        result *= 10;
    }
    return result;
}

uint8_t uint16_to_str(
    uint16_t num,
    char* buffer,
    uint8_t buffer_size,
    uint8_t max_digits,
    bool skip_zeroes
) {
    if (buffer_size == 0) {
        return 0;
    }

    uint8_t off = 0;

    bool non_zero_encountered = false;
    uint16_t cur_divisor = calc_first_divisor(max_digits);
    for (uint8_t i = 0; i < max_digits; i++) {
        uint8_t cur_digit = num / cur_divisor % 10;
        cur_divisor /= 10;

        if (cur_digit == 0 && !non_zero_encountered && skip_zeroes) {
            continue;
        }

        if (cur_digit != 0) {
            non_zero_encountered = true;
        }

        if (off >= buffer_size) {
            return off;
        }

        buffer[off++] = '0' + cur_digit;
    }

    return off;
}