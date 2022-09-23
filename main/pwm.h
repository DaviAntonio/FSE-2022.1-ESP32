/*
 *  ESP32 MQTT Thingsboard
 *  Copyright (C) 2022 Davi Antônio da Silva Santos <antoniossdavi at gmail.com>
 *  Copyright (C) 2022 Wellington Jonatan <wellpriz at gmail.com>
 *  This file is part of ESP32 MQTT Thingsboard.

 *  ESP32 MQTT Thingsboard is free software: you can redistribute it and/or
 *  modify it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the License,
 *  or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.

 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
