#pragma once

void hd44780_setup(char i2c_address);
void hd44780_send_text(char i2c_address, char* text);
void hd44780_clear(char i2c_address);
void hd44780_new_line(char i2c_address);