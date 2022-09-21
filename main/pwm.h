#ifndef PWM_H
#define PWM_H

#include "driver/ledc.h"
#include <stdbool.h>

enum pwm_error {
	PWM_OK = 0,
	PWM_SET_TIMER_FAILED,
	PWM_SET_CHANNEL_FAILED,
	PWM_TIMER_NOT_SET,
	PWM_CHANNEL_NOT_SET
};

typedef enum pwm_error pwm_error_t;

pwm_error_t pwm_init();

pwm_error_t gpio_board_set_duty(uint8_t duty_perc);

pwm_error_t gpio_red_led_set_duty(uint8_t duty_perc);
pwm_error_t gpio_green_led_set_duty(uint8_t duty_perc);
pwm_error_t gpio_blue_led_set_duty(uint8_t duty_perc);

pwm_error_t gpio_board_get_duty_perc(uint8_t *duty_perc);

pwm_error_t gpio_red_led_get_duty_perc(uint8_t *duty_perc);
pwm_error_t gpio_green_led_get_duty_perc(uint8_t *duty_perc);
pwm_error_t gpio_blue_led_get_duty_perc(uint8_t *duty_perc);

#endif
