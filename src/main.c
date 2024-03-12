#include "hd44780.h"
#include "ds18b20.h"
#include "utils.h"
#include <string.h>
#include <util/delay.h>
#include <avr/io.h>

char first_line[17] = "Temp:           ";
char second_line[17] = "Elap:           ";

void construct_first_line(ds18b20_temp_t temp) {
    memcpy(first_line, "Temp:           ", 17);

    unsigned char off = ds18b20_temp_t_to_string(temp, first_line + 6, 9) + 6;
    first_line[off++] = HD44780_CHAR_DEGREE;
    first_line[off] = 'C';
}

void construct_second_line(uint16_t millis) {
    memcpy(second_line, "Elap:           ", 17);

    unsigned char off = uint16_to_str(millis, second_line + 6, 17 - 6 - 2, true) + 6;
    second_line[off++] = 'm';
    second_line[off] = 's';
}

void initialize_clock(void) {
    TCCR1B &= 0b11111000; // Set x1024 prescaler.
    TCCR1B |= 0b00000101;
}

void reset_clock(void) {
    TCNT1 = 0;
}

uint16_t get_clock_millis(void) {
    #if F_CPU % 1000000 == 0 && F_CPU <= 128000000
    return TCNT1 / 125 * (uint16_t)(F_CPU / 2000000);
    #else
    #error "Not standard/unsupported CPU frequency."
    #endif
}

int main() {
    initialize_clock();
    hd44780_setup(0x27);

    while (1) {
        if (!ds18b20_is_measurement_over()) {
            continue;
        }
        ds18b20_start_measurement();
        ds18b20_temp_t temp = ds18b20_get_temp();
        construct_first_line(temp);

        uint16_t millis = get_clock_millis();
        reset_clock();
        construct_second_line(millis);

        hd44780_jump(0x27, 0, 0);
        hd44780_send_text(0x27, first_line);
        hd44780_jump(0x27, 0, 1);
        hd44780_send_text(0x27, second_line);
    }
}