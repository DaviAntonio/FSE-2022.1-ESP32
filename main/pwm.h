#ifndef PWM_H
#define PWM_H

#include "timer_utils.h"

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
