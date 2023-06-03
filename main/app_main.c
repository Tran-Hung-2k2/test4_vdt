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
#include "nvs_flash.h"
#include <driver/gpio.h>
#include <freertos/timers.h>
#include <gpio_lib.h>
#include <stdlib.h>
#include <string.h>

#include "protocol_examples_common.h"

// #include "lwip/dns.h"
// #include "lwip/netdb.h"
// #include "lwip/sockets.h"

#define device_ID   "5570bd21-761d-45e2-99e8-56f43ced32ec"
#define MQTT_Broker "mqtt://mqtt.innoway.vn"
#define PASSWORD    "MCSvgZLO56gyYTOK9a5EVCxbb1gsjWLe"
#define USERNAME    "TranHung"
#define BUTTON_PIN  GPIO_NUM_0
static esp_mqtt_client_handle_t client;
static EventGroupHandle_t s_wifi_event_group;

static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *MQTT_TAG = "MQTT_STATUS";
static const char *SMARTCONFIG_TAG = "smartconfig";
static int buttonPressCount = 0;
static int isLedOn = 0;
static int current_led = 2;
static bool mqtt_connected = false;
static bool response = false;

static void smartconfig_task(void *parm);
void gpio_intr_handler(void *args);
static void mqtt_app_start(void);

// Hàm log lỗi nếu mã lỗi khác 0
static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3,
        // NULL);
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
            mqtt_app_start();  // Khởi tạo và kết nối với broker MQTT
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
        esp_mqtt_client_publish(client, "messages/" device_ID "/update",
                                message, 0, 0, false);
    }
}

void reSendHeartbeat() {
    if (mqtt_connected && !response) {
        esp_mqtt_client_publish(client, "messages/" device_ID "/update",
                                "{\"heartbeat\": 1}", 0, 0, false);
    }
}

void sendHeartbeat() {
    if (mqtt_connected) {
        response = false;
        esp_mqtt_client_publish(client, "messages/" device_ID "/update",
                                "{\"heartbeat\": 1}", 0, 0, false);
    }
    TimerHandle_t xTimers3 =
        xTimerCreate("sTimer3", pdMS_TO_TICKS(20000), pdFALSE, (void *) 2,
                     (void *) reSendHeartbeat);
    xTimerStart(xTimers3, 0);
}

void buttonTask(void *pvParameters) {
    TimerHandle_t xTimers1 =
        xTimerCreate("sTimer1", pdMS_TO_TICKS(300), pdFALSE, (void *) 0,
                     (void *) buttonPress2);
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
                while (gpio_get_level(BUTTON_PIN) == 0) {
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }
                currentTime = xTaskGetTickCount();
                if ((currentTime - lastButtonPressTime) < buttonPressInterval) {
                    buttonPressCount++;
                    if (buttonPressCount == 2) {
                        xTimerStart(xTimers1, 0);
                    } else if (buttonPressCount == 4) {
                        // Chế độ smart config
                        xTaskCreate(smartconfig_task, "smartconfig_task", 4096,
                                    NULL, 3, NULL);
                        // Reset số lần nhấn nút
                        buttonPressCount = 0;
                    }
                } else {
                    buttonPressCount = 1;
                }
                lastButtonPressTime = currentTime;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Hàm xử lý sự kiện MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data) {
    char *data;
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d",
             base, (int) event_id);
    // Ép kiểu dữ liệu của sự kiện MQTT
    esp_mqtt_event_handle_t event = event_data;
    // Lưu trữ client MQTT
    client = event->client;
    // Xử lý các sự kiện MQTT
    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:  // Khi kết nối thành công với broker MQTT
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
            mqtt_connected = true;
            // Đăng ký topic để nhận dữ liệu
            esp_mqtt_client_subscribe(client, "messages/" device_ID "/status",
                                      0);
            break;
        case MQTT_EVENT_DISCONNECTED:  // Khi mất kết nối với broker MQTT
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
            mqtt_connected = false;
            break;

        case MQTT_EVENT_SUBSCRIBED:  // Khi đăng ký topic thành công
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d",
                     event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:  // Khi hủy đăng ký topic thành công
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d",
                     event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:  // Khi gửi dữ liệu lên broker MQTT thành
                                    // công
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d",
                     event->msg_id);
            // Đăng ký lại topic để nhận dữ liệu
            esp_mqtt_client_subscribe(client, "messages/" device_ID "/status",
                                      0);
            break;
        case MQTT_EVENT_DATA:  // Khi nhận được dữ liệu từ topic đã đăng ký
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
            // In ra topic nhận được dữ liệu
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // In ra dữ liệu nhận được
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            data = event->data;
            if (strstr(data, "code")) {
                response = true;
            } else {
                if (strstr(data, "on"))
                    isLedOn = 1;
                else
                    isLedOn = 0;
                sscanf(data, "{\"led\": %d", &current_led);
                gpio_set_level(current_led, isLedOn);
            }
            break;
        case MQTT_EVENT_ERROR:  // Khi xảy ra lỗi trong quá trình kết nối hoặc
                                // truyền nhận dữ liệu
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type ==
                MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                // In ra lỗi từ esp-tls
                log_error_if_nonzero("reported from esp-tls",
                                     event->error_handle->esp_tls_last_esp_err);
                // In ra lỗi từ tls stack
                log_error_if_nonzero("reported from tls stack",
                                     event->error_handle->esp_tls_stack_err);
                // In ra lỗi từ socket
                log_error_if_nonzero(
                    "captured as transport's socket errno",
                    event->error_handle->esp_transport_sock_errno);
                // In ra thông báo lỗi
                ESP_LOGI(
                    MQTT_TAG, "Last errno string (%s)",
                    strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(MQTT_TAG, "Other event id:%d", event->event_id);
            break;
    }
}

// Hàm khởi tạo và kết nối với broker MQTT
static void mqtt_app_start(void) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_Broker,  // Địa chỉ của broker MQTT
        .credentials.authentication.password = PASSWORD,  // Mật khẩu để kết nối
                                                          // với broker
        .credentials.username = USERNAME  // Tên đăng nhập để kết nối với broker
    };

    client = esp_mqtt_client_init(&mqtt_cfg);  // Khởi tạo client MQTT

    // Đăng ký hàm xử lý sự kiện MQTT
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
                                   NULL);
    // Bắt đầu kết nối với broker MQTT
    esp_mqtt_client_start(client);
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    create_io(GPIO_NUM_0, GPIO_MODE_INPUT, GPIO_INTR_ANYEDGE);
    create_io(GPIO_NUM_2, GPIO_MODE_OUTPUT, GPIO_INTR_DISABLE);
    xTaskCreate(buttonTask, "buttonTask", 1024, NULL, 5, NULL);
    initialise_wifi();
}
