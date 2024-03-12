#pragma once

#include <stdint.h>

static const char HD44780_CHAR_DEGREE = 0xDF;

void hd44780_setup(char i2c_address);
void hd44780_send_text(char i2c_address, char* text);
void hd44780_clear(char i2c_address);
void hd44780_jump(char i2c_address, uint8_t x, uint8_t y);