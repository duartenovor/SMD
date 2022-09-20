/********************************************************************/
/*								SERVO								*/
/********************************************************************/
#include "servo.h"
#include <bcm2835.h>
#include <stdint.h>

//#define PIN RPI_GPIO_P1_12 //GPIO 18 (PIN 12)
//#define PIN RPI_V2_GPIO_P1_32 //GPIO 12 (PIN 32)
//#define PIN RPI_V2_GPIO_P1_33 //GPIO 33 (PIN 13)
//#define PIN RPI_V2_GPIO_P1_35 //GPIO 35 (PIN 19)
// and it is controlled by PWM channel 0
/*#define PWM_CHANNEL 1*/

// This controls the max range of the PWM signal
#define RANGE 67500 //50Hz

/********************************************************************/
/*								SERVO								*/
/********************************************************************/

using namespace std;

CServo::CServo(uint8_t pin, uint8_t pwm_channel)
{
	if (bcm2835_init()){
		// Set the output pin to Alt Fun 5, to allow PWM channel 0 to be output there
		bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_ALT5);
		// Oscilator = 54MHz
		// Clock divider is set to 16.
		// 54MHz/16 = 3.375MHz
		// With a divider of 16 and a objetctive of 50Hz, in MARKSPACE mode,
		// the pulse repetition frequency will be
		// 3.375MHz/50Hz = 67500, suitable for driving the servo motor with PWM
		bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
		bcm2835_pwm_set_mode(pwm_channel, 1, 1);
		bcm2835_pwm_set_range(pwm_channel, RANGE);
	}
	this->pin = pin;
	this->pwm_channel = pwm_channel;
};

CServo::~CServo()
{
	bcm2835_close();
};

void CServo::set_pwm(void)
{
	bcm2835_pwm_set_data(pwm_channel, (data*RANGE));
};

void CServo::s_1m(void)
{
	//1ms -> 0º
	this->data = 0.05;
	this->set_pwm();
};

void CServo::s_1_5m(void)
{
	//1.5ms -> 45º
	this->data = 0.075;
	this->set_pwm();
};

void CServo::s_2m(void)
{
	//2ms -> 90º
	this->data = 0.10;
	this->set_pwm();
};