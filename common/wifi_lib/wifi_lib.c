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
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include <stdlib.h>
#include <string.h>
#include <wifi_lib.h>

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

#define CONNECTED_BIT     BIT0
#define FAIL_BIT          BIT1
#define ESPTOUCH_DONE_BIT = BIT2;
#define ESP_MAXIMUM_RETRY 5

static EventGroupHandle_t s_wifi_event_group;
static wifi_sta_start_event_callback_t wifi_sta_start_event_callback = NULL;
static wifi_sta_disconn_event_callback_t wifi_sta_disconn_event_callback = NULL;
static wifi_connected_event_callback_t wifi_connected_event_callback = NULL;
static const char *SMCF_TAG = "smartconfig_events";
static const char *WIFI_TAG = "wifi_events";
static int s_retry_num = 0;

void smartconfig_event_task(void *parm) {
    EventBits_t uxBits;
    // Thiết lập loại trình cấu hình là SC_TYPE_ESPTOUCH
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    // Tạo struct smartconfig_start_config_t có giá trị mặc định
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    // Bắt đầu trình cấu hình thông minh và kiểm tra lỗi sau khi bắt đầu
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    // Vòng lặp vô hạn trong quá trình chờ kết nối Wi-Fi
    while (1) {
        // Chờ các bit CONNECTED_BIT hoặc ESPTOUCH_DONE_BIT được set trong Event
        // Group s_wifi_event_group
        uxBits = xEventGroupWaitBits(s_wifi_event_group,
                                     CONNECTED_BIT | ESPTOUCH_DONE_BIT, true,
                                     false, portMAX_DELAY);
        // Nếu bit CONNECTED_BIT đã được set
        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(SMCF_TAG, "WiFi Connected to ap");
        }
        // Nếu bit ESPTOUCH_DONE_BIT đã được set
        if (uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(SMCF_TAG, "smartconfig over");
            esp_smartconfig_stop();  // Dừng trình cấu hình thông minh
            if (wifi_connected_event_callback != NULL)
                wifi_connected_event_callback();
            vTaskDelete(NULL);  // Xóa task hiện tại
        }
    }
}

void smartconfig_start() {
    // Tạo một task mới với để thực hiện smartconfig
    xTaskCreate(smartconfig_event_task, "smartconfig_event_task", 4096, NULL, 3,
                NULL);
}

void wifi_connect() {
    esp_wifi_connect();  //
}

// Hàm này được gọi để thực hiện kết nối lại wifi khi quá trình kết nối bị lỗi
void wifi_reconnect() {
    // Kiểm tra số lần thử lại kết nối đã vượt quá giới hạn cho phép hay chưa
    if (s_retry_num < ESP_MAXIMUM_RETRY) {
        // Nếu số lần thử lại kết nối chưa vượt quá giới hạn, thực hiện kết nối
        // wifi lại
        esp_wifi_connect();
        // Tăng số lần thử lại kết nối lên 1
        s_retry_num++;
        // In ra thông báo cho biết đang thử lại kết nối
        ESP_LOGI(WIFI_TAG, "retry to connect to the AP");
    } else {
        // Nếu đã vượt quá giới hạn, đặt bit FAIL_BIT vào event group để thông
        // báo kết nối thất bại Đồng thời xóa bit CONNECTED_BIT khỏi event group
        // để thông báo không có kết nối được thiết lập
        xEventGroupSetBits(s_wifi_event_group, FAIL_BIT);
        xEventGroupClearBits(s_wifi_event_group, CONNECTED_BIT);
    }
    // In ra thông báo khi kết nối thất bại
    ESP_LOGI(WIFI_TAG, "connect to the AP fail");
}

// Hàm xử lý các sự kiện liên quan đến wifi và ip trên ESP32
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    // Nếu sự kiện được kích hoạt là sự kiện bắt đầu wifi station (STA)
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        // Thực hiện việc kết nối wifi
        wifi_connect();
        // Nếu có callback function đăng ký với sự kiện này thì thực hiện
        // callback function đó
        if (wifi_sta_start_event_callback != NULL)
            wifi_sta_start_event_callback();
    }
    // Nếu sự kiện được kích hoạt là sự kiện mất kết nối wifi station (STA)
    else if (event_base == WIFI_EVENT &&
             event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // Nếu có callback function đăng ký với sự kiện này thì thực hiện
        // callback function đó
        if (wifi_sta_disconn_event_callback != NULL)
            wifi_sta_disconn_event_callback();
        // Thử kết nối lại wifi
        wifi_reconnect();
    }
    // Nếu sự kiện được kích hoạt là sự kiện nhận địa chỉ ip
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        // Lấy thông tin địa chỉ ip từ event_data
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        // In ra địa chỉ ip đã nhận được
        ESP_LOGI(WIFI_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        // Thiết lập lại số lần thử kết nối và set CONNECTED_BIT trong
        // s_wifi_event_group để thông báo đã kết nối thành công
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        // In ra thông báo đã quét xong các mạng wifi có thể kết nối được
        ESP_LOGI(SMCF_TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        // In ra thông báo đã tìm thấy kênh wifi có thể kết nối được
        ESP_LOGI(SMCF_TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        // In ra thông báo đã nhận được SSID và password thông qua Smart Config
        ESP_LOGI(SMCF_TAG, "Got SSID and password");

        // Lấy thông tin SSID và password từ event_data
        smartconfig_event_got_ssid_pswd_t *evt =
            (smartconfig_event_got_ssid_pswd_t *) event_data;
        wifi_config_t wifi_config;
        uint8_t ssid[33] = {0};
        uint8_t password[65] = {0};
        uint8_t rvd_data[33] = {0};

        // Khởi tạo wifi_config và sao chép SSID và password vào đó
        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password,
               sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;
        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid,
                   sizeof(wifi_config.sta.bssid));
        }

        // Sao chép SSID và password vào các mảng tạm thời để in ra thông tin
        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        ESP_LOGI(SMCF_TAG, "SSID:%s", ssid);
        ESP_LOGI(SMCF_TAG, "PASSWORD:%s", password);

        // Nếu loại kết nối của Smart Config là EspTouch V2, lấy thông tin dữ
        // liệu từ rvd_data
        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            ESP_ERROR_CHECK(
                esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
            ESP_LOGI(SMCF_TAG, "RVD_DATA:");
            for (int i = 0; i < 33; i++) {
                printf("%02x ", rvd_data[i]);
            }
            printf("\n");
        }

        // Ngắt kết nối wifi, cài đặt wifi_config mới và kết nối lại wifi với
        // wifi_config mới
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
        esp_wifi_connect();
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
        // Set ESPTOUCH_DONE_BIT trong s_wifi_event_group để thông báo quá trình
        // kết nối đã hoàn tất
        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

// Hàm khởi tạo kết nối wifi
void wifi_station_init(char *ssid, char *password) {

    // Khởi tạo định dạng mạng esp_netif
    ESP_ERROR_CHECK(esp_netif_init());

    // Tạo một nhóm sự kiện wifi cho việc xử lý sự kiện wifi
    s_wifi_event_group = xEventGroupCreate();

    // Tạo chu trình sự kiện esp_event và đăng ký handler sự kiện wifi, ip
    // và smart config để xử lý các sự kiện liên quan đến wifi
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Tạo esp_netif để được sử dụng cho chế độ station (STA)
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    // Thiết lập cấu hình wifi mặc định và khởi tạo wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Đăng ký các handler sự kiện wifi, ip và smart config để xử lý các sự
    // kiện liên quan đến wifi
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                               &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID,
                                               &event_handler, NULL));

    // Thiết lập cấu hình wifi của station (STA) với địa chỉ ssid và
    // password được truyền vào
    wifi_config_t wifi_config = {
        .sta =
            {
                .ssid = ssid,
                .password = password,
                .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };

    // Thiết lập chế độ hoạt động của wifi là STA và thiết lập cấu hình vừa
    // được khởi tạo
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Khởi động wifi
    ESP_ERROR_CHECK(esp_wifi_start());

    // In ra thông báo đã kết thúc quá trình khởi tạo kết nối wifi
    ESP_LOGI(WIFI_TAG, "wifi_init_sta finished.");

    // Chờ cho việc kết nối đến Access Point (AP) hoặc gặp lỗi
    EventBits_t bits =
        xEventGroupWaitBits(s_wifi_event_group, CONNECTED_BIT | FAIL_BIT,
                            pdFALSE, pdFALSE, portMAX_DELAY);

    // In ra thông báo kết nối thành công hoặc thất bại
    if (bits & CONNECTED_BIT) {
        ESP_LOGI(WIFI_TAG, "connected to ap SSID:%s password:%s", ssid,
                 password);
        if (wifi_connected_event_callback != NULL)
            wifi_connected_event_callback();
    } else if (bits & FAIL_BIT) {
        ESP_LOGI(WIFI_TAG, "Failed to connect to SSID:%s, password:%s", ssid,
                 password);
    } else {
        ESP_LOGE(WIFI_TAG, "UNEXPECTED EVENT");
    }
}

/**
 * Thiết lập hàm callback cho sự kiện kết nối WiFi được khởi tạo trong chế
 * độ SmartConfig. Hàm này nhận một con trỏ void để tham chiếu đến hàm
 * callback và gán giá trị này cho biến wifi_sta_start_event_callback.
 */
void smartconfig_set_wifi_sta_start_event_callback(void *cb) {
    wifi_sta_start_event_callback = cb;
}

/**
 * Thiết lập hàm callback cho sự kiện mất kết nối WiFi trong chế độ
 * SmartConfig. Hàm này nhận một con trỏ void để tham chiếu đến hàm callback
 * và gán giá trị này cho biến wifi_sta_disconn_event_callback.
 */
void smartconfig_set_wifi_sta_disconn_event_callback(void *cb) {
    wifi_sta_disconn_event_callback = cb;
}

/**
 * Thiết lập hàm callback cho sự kiện kết nối WiFi thành công trong chế độ
 * SmartConfig. Hàm này nhận một con trỏ void để tham chiếu đến hàm callback
 * và gán giá trị này cho biến wifi_connected_event_callback.
 */
void smartconfig_set_wifi_connected_event_callback(void *cb) {
    wifi_connected_event_callback = cb;
}
