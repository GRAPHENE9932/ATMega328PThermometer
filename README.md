# atmega328p_thermometer
Reads temperature from the DS18B20 sensor and displays it on the HD44780 LCD.
Written in C using the avr-libc library.

## Sample schematic
![Schematic](pictures/schematic.png)

## Compilation and uploading
Note: replace \<port\> with the serial port your PCB is connected to. For example, `/dev/ttyUSB0`.
```
$ git clone https://github.com/GRAPHENE9932/atmega328p_thermometer.git
$ cd atmega328p_thermometer
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -DPROG_PORT=<port> ..
$ ninja
$ ninja upload
```