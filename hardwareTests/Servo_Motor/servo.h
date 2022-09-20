#ifndef SERVO_H
#define SERVO_H

#include <bcm2835.h>

/********************************************************************/
/*								SERVO								*/
/********************************************************************/

class CServo{

    public:
        CServo(uint8_t pin, uint8_t pwm_channel);
        ~CServo();
        void s_1m(void);
        void s_1_5m(void);
        void s_2m(void);

    private:
        float data;
        void set_pwm(void);
        uint8_t pwm_channel;
        uint8_t pin;
};

#endif /*SERVO_H*/