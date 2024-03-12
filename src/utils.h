#pragma once

#include <stdint.h>
#include <stdbool.h>

uint8_t uint16_to_str(
    uint16_t num,
    char* buffer,
    uint8_t buffer_size,
    uint8_t max_digits,
    bool skip_zeroes
);