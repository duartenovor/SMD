#include "uart.h"

#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int  HardwareSerial::read()
{
    unsigned char b=0;

    int len = ::read( uart_filestream, (void*) &b, 1 );
    if ( len != 1 ) {
        fprintf(stderr,"Unable to read from uart!\n");
        fprintf(stderr,"errno %d, %s\n", errno, strerror(errno));
        error = 1;
    }

    #ifdef DEBUG
    printf("recv --> len=%d, 0x%02X\n", len, b);
    #endif
        return (len == 1)? (int)b: 0;
}

void HardwareSerial::print(uint8_t ch, int sz)
{
    int r;
    r = ::write(uart_filestream, &ch, 1);
    if ( r != 1 ) {
        fprintf(stderr,"Unable to write 0x%02d to uart\n",ch);
        fprintf(stderr,"errno %d, %s\n", errno, strerror(errno));
        error = 1;
    }

    #ifdef DEBUG
    printf("sending -> 0x%02X\n", ch);
    #endif

}

void HardwareSerial::write(uint8_t ch)
{
    print(ch,1);
}


int HardwareSerial::available()
{
   struct pollfd fds[1];
   int r;
   int active;

   fds[0].fd = uart_filestream;
   fds[0].events = POLLIN;
   fds[0].revents = 0;

   r = poll( fds, 1, 0 );
   active = (r == 1) && (fds[0].revents & POLLIN);

   return active;
}

void HardwareSerial::begin(unsigned long baudrate)
{
    _baud = baudrate;

    if ( uart_filestream == -1 ) {
        fprintf(stderr,"HardwareSerial::begin() not connected to uart");
        error = 1;
        return;
    }

    //get existing configuration setup
    tcgetattr(uart_filestream, &cfg);

    //fcntl(deviceFD, F_SETFL, FNDELAY);
    fcntl(uart_filestream, F_SETFL, 0);

    // 9600, 19200, 28800, 38400, 57600 (default is 57600)
    switch( baudrate )
    {
    //TODO: make configurable
    default:
        cfsetispeed( &cfg, B57600 );
        break;
    }

    cfg.c_cflag |= (CLOCAL | CREAD);

    ////8N1 (8 data bits, No parity, 1 stop bit)
    cfg.c_cflag &= ~PARENB;
    cfg.c_cflag &= ~CSTOPB;
    cfg.c_cflag &= ~CSIZE;
    cfg.c_cflag |= CS8;

    cfg.c_cflag &= ~CRTSCTS;  //~CNEW_RTSCTS; //disable hardware flow control

    //use RAW unbuffered data mode (eg, not canonical mode)
    cfg.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IGNBRK);

    cfg.c_iflag &= ~(IGNPAR | IXON | IXOFF | IXANY);

    //raw (unprocessed) output mode
    cfg.c_oflag &= ~OPOST;

    tcsetattr(uart_filestream, TCSANOW, &cfg);

}


HardwareSerial::HardwareSerial()
{
    uart_filestream = -1;
}

int HardwareSerial::open(char* device)
{
    if ((uart_filestream =
           ::open(device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
        fprintf(stderr,"Unable to open ttyUSB0 errno=%d %s",
            errno, strerror(errno));
    }
    strcpy(_device, device);
    return (uart_filestream == -1) ? -1: 0;
}

void HardwareSerial::close()
{
    if ( uart_filestream != -1 ) {
        ::close( uart_filestream );
        uart_filestream = -1;
    }
}

// int HardwareSerial::reset() {
//     close();
//     if ( open( _device ) == -1 ) {
//         return -1;
//     }
//     begin( _baud );
//     return 0;
// }
