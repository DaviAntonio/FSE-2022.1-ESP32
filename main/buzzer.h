#ifndef BUZZER_H
#define BUZZER_H

#include "timer_utils.h"

pwm_error_t buzzer_pwm_init();

pwm_error_t buzzer_get_frequency(uint32_t *freq);
pwm_error_t buzzer_get_duty(uint8_t *duty_perc);

pwm_error_t gpio_buzzer_set_duty(uint8_t duty_perc);
pwm_error_t gpio_buzzer_set_frequency(uint32_t freq);

pwm_error_t disable_buzzer();

#endif
