#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "cJSON.h"

#include "pwm.h"
#include "mqtt.h"
#include "buzzer.h"

#define TAG "MQTT"

#define MQTT_USERNAME CONFIG_MQTT_USERNAME
#define MQTT_SERVER_URI CONFIG_MQTT_SERVER_URI

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;

static void set_buzzer_tone_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;

	err = gpio_buzzer_set_frequency(param);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

static void set_buzzer_enable_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;
	param = (param > 0) ? 50 : param;

	if (param == 0)
		err = disable_buzzer();
	else
		err = gpio_buzzer_set_duty(50);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

// board led pwm
static void get_board_led_pwm_rpc(char *resp_topic)
{
	pwm_error_t err;
	uint8_t duty;
	char resp_msg[50];

	err = gpio_board_get_duty_perc(&duty);

	if (err == PWM_OK) {
		snprintf(resp_msg, 49, "{\"value\": %d}", duty);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	} else {
		mqtt_envia_mensagem(resp_topic, "{\"value\": null}");
	}
}

static void set_board_led_pwm_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;

	err = gpio_board_set_duty(param);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

// rgb led pwm

static void set_LEDGreen_pwm_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;

	err = gpio_green_led_set_duty(param);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

static void set_LEDRed_pwm_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;

	err = gpio_red_led_set_duty(param);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

static void set_LEDBlue_pwm_rpc(char *resp_topic, int param)
{
	pwm_error_t err;
	char resp_msg[50];

	param = (param < 0) ? 0 : param;

	err = gpio_blue_led_set_duty(param);

	if (err == PWM_OK) {
		mqtt_envia_mensagem(resp_topic, "{\"status\": 0}");
	} else {
		snprintf(resp_msg, 49, "{\"status\": %d}", err);
		mqtt_envia_mensagem(resp_topic, resp_msg);
	}
}

static void execute_rpc_request(esp_mqtt_event_handle_t event,
	char *method, int method_len, int parameter, int topic_id)
{
	char resp_topic[50];

	snprintf(resp_topic, 49, "v1/devices/me/rpc/response/%d", topic_id);

	ESP_LOGI(TAG, "Parameter: %d\n", parameter);

	if (strncmp(method, "getLEDBoard", 50) == 0) {
		get_board_led_pwm_rpc(resp_topic);
	} else if (strncmp(method, "setLEDBoard", 50) == 0) {
		set_board_led_pwm_rpc(resp_topic, parameter);
	} else if (strncmp(method, "setLEDGreen", 50) == 0) {
		set_LEDGreen_pwm_rpc(resp_topic, parameter);
	} else if (strncmp(method, "setLEDRed", 50) == 0) {
		set_LEDRed_pwm_rpc(resp_topic, parameter);
	} else if (strncmp(method, "setLEDBlue", 50) == 0) {
		set_LEDBlue_pwm_rpc(resp_topic, parameter);
	} else if (strncmp(method, "setBuzzerTone", 50) == 0) {
		set_buzzer_tone_rpc(resp_topic, parameter);
	} else if (strncmp(method, "setBuzzerEnable", 50) == 0) {
		set_buzzer_enable_rpc(resp_topic, parameter);
	} else {
		ESP_LOGE(TAG, "method: '%.*s' not implemented", method_len,
			method);
	}
}

static void handle_rpc_request_data(esp_mqtt_event_handle_t event, int topic_id)
{
	int parameter;
	char *method = NULL;

	cJSON *json = NULL;
	cJSON *json_method = NULL;
	cJSON *json_param = NULL;

	json = cJSON_ParseWithLength(event->data, event->data_len);
	if (json == NULL) {
		ESP_LOGE(TAG, "Invalid RPC request data '%.*s'",
			event->data_len, event->data);
	} else {
		json_method = cJSON_GetObjectItemCaseSensitive(json, "method");
		if (cJSON_IsString(json_method) && (json_method->valuestring != NULL)) {
			method = json_method->valuestring;
			json_param = cJSON_GetObjectItemCaseSensitive(json, "params");
			if (cJSON_IsNumber(json_param)) {
				parameter = json_param->valueint;
				execute_rpc_request(event, method,
					strnlen(method, 50), parameter,
					topic_id);
			} else {
				ESP_LOGE(TAG, "Invalid while parsing param");
			}
		} else {
			ESP_LOGE(TAG, "Error while parsing method");
		}
	}

	cJSON_Delete(json);
}

static void handle_event_topic(esp_mqtt_event_handle_t event)
{
	int rst;
	int topic_id;

	rst = sscanf(event->topic, "v1/devices/me/rpc/request/%d",
		&topic_id);
	if (rst == 1) {
		handle_rpc_request_data(event, topic_id);
	} else {
		ESP_LOGE(TAG, "Event '%.*s' is not implemented",
			event->topic_len, event->topic);
	}
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
	esp_mqtt_client_handle_t client = event->client;
	int msg_id;

	switch (event->event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

		xSemaphoreGive(conexaoMQTTSemaphore);

		msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		msg_id = esp_mqtt_client_subscribe(client, "v1/devices/me/attributes/response/+", 0);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
		printf("DATA=%.*s\r\n", event->data_len, event->data);
		handle_event_topic(event);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	default:
		ESP_LOGI(TAG, "Other event id:%d", event->event_id);
		break;
	}

	return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
	int32_t event_id, void *event_data)
{
	ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d",
		base, event_id);
	mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
	esp_mqtt_client_config_t mqtt_config = {
		.uri = MQTT_SERVER_URI,
		.username = MQTT_USERNAME
	};

	client = esp_mqtt_client_init(&mqtt_config);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID,
		mqtt_event_handler, client);
	esp_mqtt_client_start(client);
}

void mqtt_envia_mensagem(char *topico, char *mensagem)
{
	int message_id = esp_mqtt_client_publish(client, topico, mensagem,
		0, 1, 0);
	ESP_LOGI(TAG, "Sent ID: '%d' TOPIC: '%.*s' MSG: '%.*s'",
		message_id, 50, topico, 50, mensagem);
}
