#include <util/delay.h>
#include <avr/io.h>

int main() {
    while (1) {
        DDRD |= 0b00000100;
        _delay_ms(500.0);
        PORTD |= 0b00000100;
        _delay_ms(500.0);
        PORTD &= 0b00000000;
    }
}