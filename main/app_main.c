#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "mqtt_client.h"
#include "mqtt_lib.h"
#include "nvs_flash.h"
#include <driver/gpio.h>
#include <freertos/timers.h>
#include <gpio_lib.h>
#include <stdlib.h>
#include <string.h>

#define MQTT_Broker "mqtt://mqtt.innoway.vn"
#define USERNAME    "TranHung"
#define PASSWORD    "ls8M4bx7zK8BKU6xj63LbHOLzl57X9Hy"
#define device_ID   "5570bd21-761d-45e2-99e8-56f43ced32ec"
#define BUTTON_PIN  GPIO_NUM_0

static EventGroupHandle_t s_wifi_event_group;
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *SMARTCONFIG_TAG = "smart_config";
static int buttonPressCount = 0;
static int isLedOn = 0;
static int current_led = 2;
static bool mqtt_connected = false;
static bool response = false;

static void smartconfig_task(void *parm);

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Chờ đợi smartconfig
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(SMARTCONFIG_TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(SMARTCONFIG_TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(SMARTCONFIG_TAG, "Got SSID and password");
        smartconfig_event_got_ssid_pswd_t *evt =
            (smartconfig_event_got_ssid_pswd_t *) event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        uint8_t rvd_data[33] = {0};

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password,
               sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid,
                   sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(SMARTCONFIG_TAG, "SSID:%s", ssid);
        ESP_LOGI(SMARTCONFIG_TAG, "PASSWORD:%s", password);
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK(
                esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            ESP_LOGI(SMARTCONFIG_TAG, "RVD_DATA:");
            for (int i = 0; i < 33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void initialise_wifi(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void smartconfig_task(void *parm) {
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));
    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group,
                                     CONNECTED_BIT | ESPTOUCH_DONE_BIT, true,
                                     false, portMAX_DELAY);
        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(SMARTCONFIG_TAG, "WiFi Connected to ap");
        }
        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(SMARTCONFIG_TAG, "smartconfig over");
            esp_smartconfig_stop();
            // Khởi tạo và kết nối với broker MQTT
            mqtt_app_start(MQTT_Broker, USERNAME, PASSWORD);
            vTaskDelete(NULL);
        }
    }
}

void buttonPress2(void *pvParameters) {
    if (buttonPressCount == 2) {
        // Thay đổi trạng thái LED
        isLedOn = 1 - isLedOn;
        gpio_set_level(current_led, isLedOn);
        buttonPressCount = 0;
        // Gửi bản tin trạng thái lên server
        char message[100];
        char ledStatus[5];
        if (isLedOn)
            strcpy(ledStatus, "on");
        else
            strcpy(ledStatus, "off");
        sprintf(message, "{\"led\": %d, \"status\": \"%s\"}", current_led,
                ledStatus);
        mqtt_publish("messages/" device_ID "/update", message);
    }
}

void reSendHeartbeat() {
    if (mqtt_connected && !response) {
        mqtt_publish("messages/" device_ID "/update", "{\"heartbeat\": 1}");
    }
}

void sendHeartbeat() {
    if (mqtt_connected) {
        response = false;
        mqtt_publish("messages/" device_ID "/update", "{\"heartbeat\": 1}");
    }
    // Timer 3 sau 20s kiểm tra xem đã nhận được bản tin code 1 từ innoway chưa
    // Nếu chưa thực hiện gửi lại bản tin bằng reSendHeartbeat()
    TimerHandle_t xTimers3 =
        xTimerCreate("sTimer3", pdMS_TO_TICKS(20000), pdFALSE, (void *) 2,
                     (void *) reSendHeartbeat);
    xTimerStart(xTimers3, 0);
}

void buttonTask(void *pvParameters) {
    // Timer 1 giúp kiếm tra sau khi ấn 2 lần người dùng có ấn lần thứ 3 trong
    // 300ms tới không
    TimerHandle_t xTimers1 =
        xTimerCreate("sTimer1", pdMS_TO_TICKS(300), pdFALSE, (void *) 0,
                     (void *) buttonPress2);
    // Thực hiện gửi heartbeat 60s 1 lần bằng hàm sendHeartbeat()
    TimerHandle_t xTimers2 =
        xTimerCreate("sTimer2", pdMS_TO_TICKS(60000), pdTRUE, (void *) 1,
                     (void *) sendHeartbeat);
    xTimerStart(xTimers2, 0);

    TickType_t lastButtonPressTime = 0;
    TickType_t currentTime;
    // Interval between button presses (500ms)
    TickType_t buttonPressInterval = 500 / portTICK_PERIOD_MS;
    while (1) {
        if (gpio_get_level(BUTTON_PIN) == 0) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (gpio_get_level(BUTTON_PIN) == 0) {
                // Đợi người dùng nhả nút
                while (gpio_get_level(BUTTON_PIN) == 0) {
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }

                currentTime = xTaskGetTickCount();
                // Thực hiện kiểm tra thời gian người dùng nhấn nút
                if ((currentTime - lastButtonPressTime) < buttonPressInterval) {
                    buttonPressCount++;
                    if (buttonPressCount == 2) {
                        // Chạy timer 2 để kiểm tra xem người dùng có nhấn nhiều
                        // hơn 2 lần không
                        xTimerStart(xTimers1, 0);
                    } else if (buttonPressCount == 4) {
                        // Chế độ smart config
                        xTaskCreate(smartconfig_task, "smartconfig_task", 4096,
                                    NULL, 3, NULL);
                        // Reset số lần nhấn nút
                        buttonPressCount = 0;
                    }
                } else {
                    // Nếu bấm giữ hoặc lần bấm đầu tiên thiết lập số lần bấm về
                    // 1
                    buttonPressCount = 1;
                }
                lastButtonPressTime = currentTime;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void mqtt_connect_handler() {
    mqtt_connected = true;
    // Đăng ký topic để nhận dữ liệu
    mqtt_subscribe("messages/" device_ID "/status");
}

void mqtt_disconnect_handler() {
    mqtt_connected = false;  //
}

void mqtt_publish_handler() {
    // Đăng ký lại topic để nhận dữ liệu
    mqtt_subscribe("messages/" device_ID "/status");
}

void mqtt_data_handler(char *data, char *topic) {
    if (strstr(data, "code")) {
        // Nếu đã nhận được phản hồi của innoway {"code": 1}
        response = true;
    } else {
        // Nếu nhận được bản tin điều khiển led
        // Kiểm tra trạng thái bật tắt
        if (strstr(data, "on"))
            isLedOn = 1;
        else
            isLedOn = 0;
        // Thực hiện lấy số hiệu chân của led muốn điều khiển
        sscanf(data, "{\"led\": %d", &current_led);
        // Thiết lập trạng thái led
        gpio_set_level(current_led, isLedOn);
    }
}

void mqtt_setup() {
    mqtt_set_conn_event_callback(mqtt_connect_handler);
    mqtt_set_disconn_event_callback(mqtt_disconnect_handler);
    mqtt_set_pub_event_callback(mqtt_publish_handler);
    mqtt_set_data_event_callback(mqtt_data_handler);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    gpio_init_io(GPIO_NUM_0, GPIO_MODE_INPUT, GPIO_INTR_ANYEDGE);
    gpio_init_io(current_led, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE);
    // Chạy 1 task mới để kiểm tra trạng thái bấm của button 0
    xTaskCreate(buttonTask, "buttonTask", 1024, NULL, 5, NULL);
    // Thực hiện đăng ký các hàm xử lý sự kiện cho MQTT event
    mqtt_setup();
    // Thực hiện set up wifi và chờ người dùng ấn smartconfig
    initialise_wifi();
}
