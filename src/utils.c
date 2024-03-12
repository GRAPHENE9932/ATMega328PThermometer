#include "utils.h"

unsigned char uint16_to_str(uint16_t num, char* buffer, unsigned char buffer_size, bool skip_zeroes) {
    if (buffer_size == 0) {
        return 0;
    }

    unsigned char off = 0;

    bool non_zero_encountered = false;
    uint16_t cur_divisor = 10000; // The maximum possible value is 65536 (5 digits).
    for (unsigned char i = 0; i < 5; i++) {
        unsigned char cur_digit = num / cur_divisor % 10;
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