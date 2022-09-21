#include "esp_log.h"

#include "pwm.h"
#include "gpios.h"

#define PWM_SPEED_MODE (LEDC_LOW_SPEED_MODE)
#define PWM_DUTY_MAX_COUNT (8192)
#define PWM_TIMER (LEDC_TIMER_0)
#define PWM_FREQUENCY_HZ (5000)

#define TAG "PWM"

ledc_timer_config_t ledc_timer;

ledc_channel_config_t gpio_board_channel;

ledc_channel_config_t gpio_red_led_channel;
ledc_channel_config_t gpio_green_led_channel;
ledc_channel_config_t gpio_blue_led_channel;

bool ledc_timer_ok = false;

bool gpio_board_channel_ok = false;

bool gpio_red_led_channel_ok = false;
bool gpio_green_led_channel_ok = false;
bool gpio_blue_led_channel_ok = false;

static esp_err_t timer_init()
{
	ledc_timer = (ledc_timer_config_t) {
		.speed_mode = PWM_SPEED_MODE,
		.timer_num = PWM_TIMER,
		.duty_resolution = LEDC_TIMER_13_BIT,
		.freq_hz = PWM_FREQUENCY_HZ,
		.clk_cfg = LEDC_AUTO_CLK
	};

	return ledc_timer_config(&ledc_timer);
}

static ledc_channel_config_t default_init()
{
	return (ledc_channel_config_t) {
		.speed_mode = PWM_SPEED_MODE,
		.timer_sel = PWM_TIMER,
		.intr_type = LEDC_INTR_DISABLE,
		.duty = 0,
		.hpoint = 0
	};
}

static esp_err_t gpio_board_channel_init()
{
	gpio_board_channel = default_init();
	gpio_board_channel.channel = LEDC_CHANNEL_0;
	gpio_board_channel.gpio_num = GPIO_BOARD;

	return ledc_channel_config(&gpio_board_channel);
}

static esp_err_t gpio_red_led_channel_init()
{
	gpio_red_led_channel = default_init();
	gpio_red_led_channel.channel = LEDC_CHANNEL_1;
	gpio_red_led_channel.gpio_num = GPIO_RED_LED;

	return ledc_channel_config(&gpio_red_led_channel);
}

static esp_err_t gpio_green_led_channel_init()
{
	gpio_green_led_channel = default_init();
	gpio_green_led_channel.channel = LEDC_CHANNEL_2;
	gpio_green_led_channel.gpio_num = GPIO_GREEN_LED;

	return ledc_channel_config(&gpio_green_led_channel);
}

static esp_err_t gpio_blue_led_channel_init()
{
	gpio_blue_led_channel = default_init();
	gpio_blue_led_channel.channel = LEDC_CHANNEL_3;
	gpio_blue_led_channel.gpio_num = GPIO_BLUE_LED;

	return ledc_channel_config(&gpio_blue_led_channel);
}

pwm_error_t pwm_init()
{
	esp_err_t err = timer_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_TIMER_FAILED;
	}
	ledc_timer_ok = true;

	err = gpio_board_channel_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}
	gpio_board_channel_ok = true;

	err = gpio_red_led_channel_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}
	gpio_red_led_channel_ok = true;

	err = gpio_green_led_channel_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}
	gpio_green_led_channel_ok = true;

	err = gpio_blue_led_channel_init();
	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}
	gpio_blue_led_channel_ok = true;

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

static bool is_gpio_board_channel_null()
{
	if (!gpio_board_channel_ok) {
		ESP_LOGE(TAG, "GPIO BOARD channel is not set");
		return true;
	}

	return false;
}

static bool is_gpio_red_led_channel_null()
{
	if (!gpio_red_led_channel_ok) {
		ESP_LOGE(TAG, "GPIO RED LED channel is not set");
		return true;
	}

	return false;
}

static bool is_gpio_green_led_channel_null()
{
	if (!gpio_green_led_channel_ok) {
		ESP_LOGE(TAG, "GPIO GREEN LED channel is not set");
		return true;
	}

	return false;
}

static bool is_gpio_blue_led_channel_null()
{
	if (!gpio_blue_led_channel_ok) {
		ESP_LOGE(TAG, "GPIO BLUE LED channel is not set");
		return true;
	}

	return false;
}

static uint32_t duty_perc_to_counts(uint8_t duty_perc)
{
	uint32_t duty = (duty_perc * PWM_DUTY_MAX_COUNT) / 100;
	duty = (duty >= PWM_DUTY_MAX_COUNT) ?
		PWM_DUTY_MAX_COUNT - 1 : duty;

	return duty;
}

static uint8_t duty_counts_to_perc(uint32_t duty)
{
	return (100 * duty) / PWM_DUTY_MAX_COUNT;
}

static esp_err_t _gpio_board_set_duty(uint8_t duty_perc)
{
	uint32_t duty = duty_perc_to_counts(duty_perc);

	gpio_board_channel.duty = duty;

	return ledc_channel_config(&gpio_board_channel);
}


static esp_err_t _gpio_red_led_set_duty(uint8_t duty_perc)
{
	uint32_t duty = duty_perc_to_counts(duty_perc);

	gpio_red_led_channel.duty = duty;

	return ledc_channel_config(&gpio_red_led_channel);
}


static esp_err_t _gpio_green_led_set_duty(uint8_t duty_perc)
{
	uint32_t duty = duty_perc_to_counts(duty_perc);

	gpio_green_led_channel.duty = duty;

	return ledc_channel_config(&gpio_green_led_channel);
}

static esp_err_t _gpio_blue_led_set_duty(uint8_t duty_perc)
{
	uint32_t duty = duty_perc_to_counts(duty_perc);

	gpio_blue_led_channel.duty = duty;

	return ledc_channel_config(&gpio_blue_led_channel);
}

pwm_error_t gpio_board_set_duty(uint8_t duty_perc)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_board_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_board_set_duty(duty_perc);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

pwm_error_t gpio_red_led_set_duty(uint8_t duty_perc)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_red_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_red_led_set_duty(duty_perc);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

pwm_error_t gpio_green_led_set_duty(uint8_t duty_perc)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_green_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_green_led_set_duty(duty_perc);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

pwm_error_t gpio_blue_led_set_duty(uint8_t duty_perc)
{
	esp_err_t err;

	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_blue_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	err = _gpio_blue_led_set_duty(duty_perc);

	if (err != ESP_OK) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(err);
		return PWM_SET_CHANNEL_FAILED;
	}

	return PWM_OK;
}

// Board LED

pwm_error_t gpio_board_get_duty_perc(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_board_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = duty_counts_to_perc(gpio_board_channel.duty);

	return PWM_OK;
}

// LEDs

pwm_error_t gpio_red_led_get_duty_perc(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_red_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = duty_counts_to_perc(gpio_red_led_channel.duty);

	return PWM_OK;
}

pwm_error_t gpio_green_led_get_duty_perc(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_green_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = duty_counts_to_perc(gpio_green_led_channel.duty);

	return PWM_OK;
}

pwm_error_t gpio_blue_led_get_duty_perc(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_blue_led_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = duty_counts_to_perc(gpio_blue_led_channel.duty);

	return PWM_OK;
}
