#include "i2c.h"
#include <util/delay.h>

const double I2C_PERIOD = 10.0; // In microseconds. 100 kHz

static void send_bit(char bit) {
    if (bit) {
        SDA_PORT |= SDA_PORT_BIT;
    }
    else {
        SDA_PORT &= ~SDA_PORT_BIT;
    }
    _delay_us(I2C_PERIOD / 4.0);
    SCL_PORT |= SCL_PORT_BIT;
    _delay_us(I2C_PERIOD / 4.0);
    SCL_PORT &= ~SCL_PORT_BIT;
    SDA_PORT &= ~SDA_PORT_BIT;
    _delay_us(I2C_PERIOD / 2.0);
}

static char ack() {
    _delay_us(I2C_PERIOD / 4.0);
    SCL_PORT |= SCL_PORT_BIT;
    _delay_us(I2C_PERIOD / 4.0);

    SDA_DDR &= ~SDA_PORT_BIT;
    char result = SDA_PORT & SDA_PORT_BIT;
    SDA_DDR |= SDA_PORT_BIT;

    SCL_PORT &= ~SCL_PORT_BIT;
    _delay_us(I2C_PERIOD / 2.0);

    return result;
}

void start_condition(void) {
    SDA_PORT |= SDA_PORT_BIT;
    SCL_PORT |= SCL_PORT_BIT;

    _delay_us(I2C_PERIOD);

    SDA_PORT &= ~SDA_PORT_BIT;

    _delay_us(I2C_PERIOD / 2.0);

    SCL_PORT &= ~SCL_PORT_BIT;

    _delay_us(I2C_PERIOD);
}

static void stop_condition(void) {
    SDA_PORT &= ~SDA_PORT_BIT;
    SCL_PORT &= ~SCL_PORT_BIT;

    _delay_us(I2C_PERIOD);

    SCL_PORT |= SCL_PORT_BIT;
    
    _delay_us(I2C_PERIOD / 2.0);

    SDA_PORT |= SDA_PORT_BIT;

    _delay_us(I2C_PERIOD);
}

static void send_address(char address) {
    // Send the 7-bit address bits.
    for (char i = 0; i < 7; i++) {
        send_bit(address & 0b01000000);
        address <<= 1;
    }

    // R/W bit.
    send_bit(0);
    
    // ACK bit.
    ack();
}

void i2c_start_transmission(uint8_t slave_address) {
    SDA_DDR |= SDA_PORT_BIT;
    SCL_DDR |= SCL_PORT_BIT;
    start_condition();
    send_address(slave_address);
    _delay_us(I2C_PERIOD);
}

void i2c_end_transmission(void) {
    stop_condition();
}

void i2c_send_byte(char byte) {
    for (char i = 0; i < 8; i++) {
        send_bit(byte & 0b10000000);
        byte <<= 1;
    }
    ack();
}