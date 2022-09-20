/**
* @file 
* @author Joao Mitanda
* @date 28-12-2021
*
* @brief File to test the buzzer device driver
*
* It contains a infinit loop with:
* 1. Turn On buzzer during 5seconds
* 2. Turn Off buzzer during 5seconds
*/

#include <unistd.h>
#include <fcntl.h>

/**
 * main function
 * @return
 */
int main (void){
    char BuzzOn = '1';
    char BuzzOff = '0';
    
    system("insmod buzz.ko");
    
    /**< Open file descriptor to write on buzzer */
    int file_descriptor = open("/dev/buzzer0", O_WRONLY);

    while (1){
        /**< Turn on*/
        write(file_descriptor, &BuzzOn, 1);
        /**< Wait 1s*/
        sleep(1);
        /**< Turn off*/
        write(file_descriptor, &BuzzOff, 1);
        /**< Wait 1s*/
        sleep(1);
    }
    close(file_descriptor);

    system("rmmod buzz.ko");

    return 0;
}
