/********************************************************************/
/*								SERVO								*/
/********************************************************************/
#include "../inc/servo.h"
#include <stdint.h>

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
}

CServo::~CServo()
{
	bcm2835_close();
}

void CServo::set_pwm(void)
{
	bcm2835_pwm_set_data(pwm_channel, (data*RANGE));
}

void CServo::sUp(void)
{
	//1ms -> 0º
	this->data = 0.05;
	this->set_pwm();
}

void CServo::sMiddle(void)
{
	//1.5ms -> 45º
	this->data = 0.075;
	this->set_pwm();
}

void CServo::sDown(void)
{
	//2ms -> 90º
	this->data = 0.10;
	this->set_pwm();
}
