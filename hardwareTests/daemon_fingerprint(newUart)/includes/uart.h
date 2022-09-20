#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>            //Used for UART
#include <fcntl.h>             //Used for UART
#include <termios.h>           //Used for UART

class HardwareSerial
{
public:
    HardwareSerial();

    void begin();

    void write(uint8_t ch);
    int read();
    int available();

    int open(char* device);
    void close();
private:
    int uart_filestream;
    struct termios options;
};

#endif //UART_H
