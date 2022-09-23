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

#ifndef GPIOS_H
#define GPIOS_H

#include "driver/gpio.h"

#define GPIO_DHT11 (GPIO_NUM_16)
#define GPIO_BOARD (GPIO_NUM_2)

#define GPIO_RED_LED (GPIO_NUM_17)
#define GPIO_GREEN_LED (GPIO_NUM_18)
#define GPIO_BLUE_LED (GPIO_NUM_19)

#define GPIO_BUZZER (GPIO_NUM_33)

#define GPIO_BOARD_BUTTON (GPIO_NUM_0)

#endif
