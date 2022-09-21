#ifndef TIMER_UTILS_H
#define TIMER_UTILS_H

enum pwm_error {
	PWM_OK = 0,
	PWM_SET_TIMER_FAILED,
	PWM_SET_CHANNEL_FAILED,
	PWM_TIMER_NOT_SET,
	PWM_CHANNEL_NOT_SET
};

typedef enum pwm_error pwm_error_t;

#endif
