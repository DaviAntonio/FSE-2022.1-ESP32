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

bool ledc_timer_ok = false;
bool gpio_board_channel_ok = false;

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

static esp_err_t gpio_board_channel_init()
{
	gpio_board_channel = (ledc_channel_config_t) {
		.speed_mode = PWM_SPEED_MODE,
		.channel = LEDC_CHANNEL_0,
		.timer_sel = PWM_TIMER,
		.intr_type = LEDC_INTR_DISABLE,
		.gpio_num = GPIO_BOARD,
		.duty = 0,
		.hpoint = 0
	};

	return ledc_channel_config(&gpio_board_channel);
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

	return PWM_OK;
}

static esp_err_t _gpio_board_set_duty(uint8_t duty_perc)
{
	uint32_t duty = (duty_perc * PWM_DUTY_MAX_COUNT) / 100;
	duty = (duty >= PWM_DUTY_MAX_COUNT) ?
		PWM_DUTY_MAX_COUNT - 1 : duty;

	gpio_board_channel.duty = duty;

	return ledc_channel_config(&gpio_board_channel);
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
		ESP_LOGE(TAG, "GPIO channel is not set");
		return true;
	}

	return false;
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

pwm_error_t gpio_board_get_duty_perc(uint8_t *duty_perc)
{
	if (is_timer_null())
		return PWM_TIMER_NOT_SET;

	if (is_gpio_board_channel_null())
		return PWM_CHANNEL_NOT_SET;

	*duty_perc = (100 * gpio_board_channel.duty) / PWM_DUTY_MAX_COUNT;

	return PWM_OK;
}
