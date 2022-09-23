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
