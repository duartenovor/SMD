#include "servo.h"
#include <iostream>
 #include <unistd.h>

#define PIN1 RPI_GPIO_P1_12 //GPIO 18 (PIN 12)
//#define PIN RPI_V2_GPIO_P1_32 //GPIO 12 (PIN 32)
//#define PIN RPI_V2_GPIO_P1_33 //GPIO 33 (PIN 13)
#define PIN2 RPI_V2_GPIO_P1_35 //GPIO 35 (PIN 19)
#define PWM_CHANNEL0 0
#define PWM_CHANNEL1 1

using namespace std;

int main()
{
    CServo m1(PIN1,PWM_CHANNEL0);
    CServo m2(PIN2,PWM_CHANNEL1);

    cout << "Sincronizing..." << endl;
    cout << "---------------------------" << endl;
    cout << "Motor (1) duty cycle of 1.5ms" << endl;
    cout << "Motor (2) duty cycle of 1.5ms" << endl;
    m1.s_1_5m();
    m2.s_1_5m();
    usleep(10000000);
    cout << "---------------------------" << endl;
    cout << "Servo Motor testing..." << endl;
    cout << "---------------------------" << endl;
    cout << "Motor (1) duty cycle of 1ms" << endl;
    cout << "Motor (2) duty cycle of 2ms" << endl;
    m1.s_1m();
    m2.s_2m();
    usleep(10000000);
    cout << "---------------------------" << endl;
    cout << "Motor (1) duty cycle of 1.5ms" << endl;
    cout << "Motor (2) duty cycle of 1.5ms" << endl;
    m1.s_1_5m();
    m2.s_1_5m();
    usleep(10000000);
    cout << "---------------------------" << endl;
    cout << "Motor (1) duty cycle of 2ms" << endl;
    cout << "Motor (2) duty cycle of 1ms" << endl;
    m1.s_2m();
    m2.s_1m();
    usleep(10000000);

	return 0;
}