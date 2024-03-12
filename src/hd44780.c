#include "hd44780.h"
#include "i2c.h"
#include <util/delay.h>

// HD44780 LCD datasheet:
// https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

static void send_nibble(char nibble, char rs, char i2c_address) {
    // D7 D6 D5 D4 BT E  RW RS
    char data_to_send = (nibble << 4) | rs | 0b00001000;

    i2c_start_transmission(i2c_address);
    i2c_send_byte(data_to_send | 0b00000100); // Send with the E bit 1.
    i2c_end_transmission();

    _delay_us(1.0);

    i2c_start_transmission(i2c_address);
    i2c_send_byte(data_to_send); // Send with the E bit 0.
    i2c_end_transmission();

    _delay_us(38.0);
}

void hd44780_setup(char i2c_address) {
    // HD44780 datasheet, page 46.
    _delay_us(15001.0);

    send_nibble(0b0011, 0, i2c_address); // [Function set] to 4-bit interface.

    _delay_us(4101.0);

    send_nibble(0b0011, 0, i2c_address); // [Function set] to 4-bit interface.

    _delay_us(101.0);

    send_nibble(0b0011, 0, i2c_address); // [Function set] to 4-bit interface.

    send_nibble(0b0010, 0, i2c_address); // [Function set] to 4-bit interface.

    send_nibble(0b0010, 0, i2c_address); // [Function set] to 4-bit interface, 2 lines, 5x8 dots font.
    send_nibble(0b1000, 0, i2c_address);

    send_nibble(0b0000, 0, i2c_address); // [Display on/off control] display ON, cursor OFF, cursor blinking OFF.
    send_nibble(0b1100, 0, i2c_address);

    send_nibble(0b0000, 0, i2c_address); // [Clear display].
    send_nibble(0b0001, 0, i2c_address);

    send_nibble(0b0000, 0, i2c_address); // [Entry mode set] increment cursor, disable display shift.
    send_nibble(0b0110, 0, i2c_address);

    _delay_us(1521.0);
}

void hd44780_send_text(char i2c_address, char* text) {
    while (*(text)) {
        send_nibble((*text) >> 4, 1, i2c_address);
        send_nibble((*text) & 0x0F, 1, i2c_address);
        text++;
    }
}

void hd44780_clear(char i2c_address) {
    send_nibble(0b0000, 0, i2c_address); // [Clear display].
    send_nibble(0b0001, 0, i2c_address);

    send_nibble(0b0000, 0, i2c_address); // [Entry mode set] increment cursor, disable display shift.
    send_nibble(0b0110, 0, i2c_address);

    _delay_us(1521.0);
}

void hd44780_jump(char i2c_address, uint8_t x, uint8_t y) {
    uint8_t address = x + y * 40;
    send_nibble((address >> 4) | 0b1000, 0, i2c_address);
    send_nibble(address, 0, i2c_address);
}