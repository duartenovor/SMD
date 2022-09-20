#include "uart.h"

#include <poll.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

HardwareSerial::HardwareSerial()
{
    uart_filestream = -1;
}

int  HardwareSerial::read()
{
    unsigned char b=0;

    int len = ::read( uart_filestream, (void*) &b, 1 );
    if ( len != 1 ) {
        fprintf(stderr,"Unable to read from uart!\n");
        fprintf(stderr,"errno %d, %s\n", errno, strerror(errno));
    }

    return (len == 1)? (int)b: 0;
}

void HardwareSerial::write(uint8_t ch)
{
    int r;
    r = ::write(uart_filestream, &ch, 1);
    if ( r != 1 ) {
        fprintf(stderr,"Unable to write 0x%02d to uart\n",ch);
        fprintf(stderr,"errno %d, %s\n", errno, strerror(errno));
    }
}


int HardwareSerial::available()
{
    struct pollfd pfds[1];
    int r;
    int active;

    pfds[0].fd = uart_filestream;    /* file descriptor */
    pfds[0].events = POLLIN;         /**< requested events -> [There is data to read] */
    pfds[0].revents = 0;             /* returned events */

    /**< int poll(struct pollfd *fds, nfds_t nfds, int timeout); */
    /**< nfds -> number of items in the fds array*/ 
    r = poll( pfds, 1, 0 );
    active = (r == 1) && (pfds[0].revents & POLLIN);

    return active;
}

/**< Information:
 *  https://www.i-programmer.info/programming/cc/10027-serial-c-and-the-raspberry-pi.html?start=2 */
void HardwareSerial::begin()
{
    if ( uart_filestream == -1 ) {
        fprintf(stderr,"HardwareSerial::begin() not connected to uart");
        return;
    }

    //get existing configuration setup
    tcgetattr(uart_filestream, &options);

    //fcntl(deviceFD, F_SETFL, FNDELAY);
    fcntl(uart_filestream, F_SETFL, 0);

    // 9600, 19200, 28800, 38400, 57600 (default is 57600)
    cfsetispeed( &options, B57600 );        /**< for input speed */

    options.c_cflag |= (CLOCAL | CREAD);

    ////8N1 (8 data bits, No parity, 1 stop bit)
    options.c_cflag &= ~PARENB;             /**< No parity */
    options.c_cflag &= ~CSTOPB;             /**< 1 stop bit */
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;                 /**< sets the number of data bits to 8. */

    options.c_cflag &= ~CRTSCTS;  //~CNEW_RTSCTS; //disable hardware flow control

    //To set raw mode with no echo and no signals you use:
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IGNBRK);

    options.c_iflag &= ~(IGNPAR | IXON | IXOFF | IXANY);

    //raw (unprocessed) output mode
    options.c_oflag &= ~OPOST;

    /**<Stores the values back in the device [TCSANOW -> for an immediate change]*/
    tcsetattr(uart_filestream, TCSANOW, &options);
}


int HardwareSerial::open(char* device)
{
    if ((uart_filestream =
           ::open(device, O_RDWR | O_NOCTTY | O_NDELAY)) == -1) {
        fprintf(stderr,"Unable to open ttyUSB0 errno=%d %s",
            errno, strerror(errno));
    }
    return (uart_filestream == -1) ? -1: 0;
}

void HardwareSerial::close()
{
    if ( uart_filestream != -1 ) {
        ::close( uart_filestream );
        uart_filestream = -1;
    }
}
