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

#include "driver/ledc.h"
#include "esp_log.h"

#include "buzzer.h"
#include "gpios.h"

#define BUZZER_PWM_SPEED_MODE (LEDC_LOW_SPEED_MODE)
#define BUZZER_PWM_DUTY_MAX_COUNT (1024)
#define BUZZER_PWM_TIMER (LEDC_TIMER_1)
#define BUZZER_PWM_FREQUENCY_HZ (20000)

#define TAG "BUZZER"

static ledc_timer_config_t ledc_timer;
static bool ledc_timer_ok = false;

static ledc_channel_config_t gpio_buzzer_channel;
static bool gpio_buzzer_channel_ok = false;

static esp_err_t timer_init();
static ledc_channel_config_t default_init();
static esp_err_t gpio_buzzer_channel_init();

static bool is_timer_null();
static bool is_gpio_buzzer_channel_null();
static uint32_t duty_perc_to_counts(uint8_t duty_perc);
static uint8_t duty_counts_to_perc(uint32_t duty);
static esp_err_t _gpio_buzzer_set_duty(uint8_t duty_perc);
static esp_err_t _gpio_buzzer_set_frequency(uint32_t freq);

static esp_err_t timer_init()
{
	ledc_timer = (ledc_timer_config_t) {
		.speed_mode = BUZZER_PWM_SPEED_MODE,
		.timer_num = BUZZER_PWM_TIMER,
		.duty_resolution = LEDC_TIMER_10_BIT,
		.freq_hz = BUZZER_PWM_FREQUENCY_HZ,
		.clk_cfg = LEDC_AUTO_CLK
	};

	return ledc_timer_config(&ledc_timer);
}

static ledc_channel_config_t default_init()
{
	return (ledc_channel_config_t) {
		.speed_mode = BUZZER_PWM_SPEED_MODE,
		.timer_sel = BUZZER_PWM_TIMER,
		.intr_type = LEDC_INTR_DISABLE,
		.duty = 0,
		.hpoint = 0,
		.flags.output_invert = 1
	};
}

static esp_err_t gpio_buzzer_channel_init()
{
	gpio_buzzer_channel = default_init();
	gpio_buzzer_channel.channel = LEDC_CHANNEL_4;
	gpio_buzzer_channel.gpio_num = GPIO_BUZZER;

	return ledc_channel_config(&gpio_buzzer_channel);
}

pwm_error_t buzzer_pwm_init()
{
	esp_err_t err = timer_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_TIMER_FAILED;
	}
	ledc_timer_ok = true;

	err = gpio_buzzer_channel_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}
	gpio_buzzer_channel_ok = true;

	return PWM_OK;
}

static bool is_timer_null()
{
	if (!ledc_timer_ok) {
		ESP_LOGE(TAG, "Timer is not set");
		return true;
	}

	return false;
}

static bool is_gpio_buzzer_channel_null()
{
	if (!gpio_buzzer_channel_ok) {
		ESP_LOGE(TAG, "Channel is not set");
		return true;
	}

	return false;
}

static uint32_t duty_perc_to_counts(uint8_t duty_perc)
{
	uint32_t duty = (duty_perc * BUZZER_PWM_DUTY_MAX_COUNT) / 100;
	duty = (duty >= BUZZER_PWM_DUTY_MAX_COUNT) ?
		BUZZER_PWM_DUTY_MAX_COUNT - 1 : duty;

	return duty;
}

static uint8_t duty_counts_to_perc(uint32_t duty)
{
	return (100 * duty) / BUZZER_PWM_DUTY_MAX_COUNT;
}

static esp_err_t _gpio_buzzer_set_duty(uint8_t duty_perc)
{
	uint32_t duty = duty_perc_to_counts(duty_perc);

	gpio_buzzer_channel.duty = duty;

	return ledc_channel_config(&gpio_buzzer_channel);
}

static esp_err_t _gpio_buzzer_set_frequency(uint32_t freq)
{
	ledc_timer.freq_hz = freq;

	return ledc_timer_config(&ledc_timer);
}

// set

pwm_error_t gpio_buzzer_set_duty(uint8_t duty_perc)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_buzzer_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_buzzer_set_duty(duty_perc);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

pwm_error_t gpio_buzzer_set_frequency(uint32_t freq)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_buzzer_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_buzzer_set_frequency(freq);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

// get

pwm_error_t buzzer_get_frequency(uint32_t *freq)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	*freq = ledc_timer.freq_hz;

	return PWM_OK;
}

pwm_error_t buzzer_get_duty(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_buzzer_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = duty_counts_to_perc(gpio_buzzer_channel.duty);

	return PWM_OK;
}

pwm_error_t disable_buzzer()
{
	if (is_gpio_buzzer_channel_null())
		return PWM_CHANNEL_NOT_SET;

	// The polarity is inverted!
	esp_err_t err = ledc_stop(gpio_buzzer_channel.speed_mode,
		gpio_buzzer_channel.channel, 0);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_TIMER_FAILED;
	}

	return PWM_OK;
}
