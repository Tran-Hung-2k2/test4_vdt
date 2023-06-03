#include <esp_log.h>
#include <mqtt_client.h>
#include <mqtt_lib.h>
#include <stdio.h>
#include <string.h>

static const char *MQTT_TAG = "MQTT_event";
static esp_mqtt_client_handle_t client;
static conn_event_callback_t conn_event_callback = NULL;
static disconn_event_callback_t disconn_event_callback = NULL;
static sub_event_callback_t sub_event_callback = NULL;
static unsub_event_callback_t unsub_event_callback = NULL;
static pub_event_callback_t pub_event_callback = NULL;
static data_event_callback_t data_event_callback = NULL;

void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(MQTT_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                        int32_t event_id, void *event_data) {
    ESP_LOGD(MQTT_TAG, "Event dispatched from event loop base=%s, event_id=%d",
             base, (int) event_id);
    // Ép kiểu dữ liệu của sự kiện MQTT
    esp_mqtt_event_handle_t event = event_data;
    // Lưu trữ client MQTT
    client = event->client;
    // Xử lý các sự kiện MQTT
    switch ((esp_mqtt_event_id_t) event_id) {
        // Khi kết nối thành công với broker MQTT
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED");
            if (conn_event_callback != NULL)
                conn_event_callback();  // Call callback function
            break;
        // Khi mất kết nối với broker MQTT
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DISCONNECTED");
            if (disconn_event_callback != NULL)
                disconn_event_callback();  // Call callback function
            break;
        // Khi đăng ký topic thành công
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d",
                     event->msg_id);
            if (sub_event_callback != NULL)
                sub_event_callback();  // Call callback function
            break;
        // Khi hủy đăng ký topic thành công
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d",
                     event->msg_id);
            if (unsub_event_callback != NULL)
                unsub_event_callback();  // Call callback function
            break;
        // Khi public dữ liệu lên broker MQTT thành công
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d",
                     event->msg_id);
            if (pub_event_callback != NULL)
                pub_event_callback();  // Call callback function
            break;
        // Khi nhận được dữ liệu từ topic đã đăng ký
        case MQTT_EVENT_DATA:
            ESP_LOGI(MQTT_TAG, "MQTT_EVENT_DATA");
            // In ra topic nhận được dữ liệu
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // In ra dữ liệu nhận được
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            // Call callback function
            if (data_event_callback != NULL)
                data_event_callback(event->data, event->topic);
            break;
        // Khi xảy ra lỗi trong quá trình kết nối hoặc truyền nhận dữ liệu
        case MQTT_EVENT_ERROR:
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

void mqtt_app_start(char *broker_uri, char *username, char *password) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        // Địa chỉ của broker MQTT
        .broker.address.uri = broker_uri,
        // Mật khẩu để kết nối với broker
        .credentials.authentication.password = password,
        // Tên đăng nhập để kết nối với broker
        .credentials.username = username,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);  // Khởi tạo client MQTT

    // Đăng ký hàm xử lý sự kiện MQTT
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler,
                                   NULL);
    // Bắt đầu kết nối với broker MQTT
    esp_mqtt_client_start(client);
}

void mqtt_subscribe(char *topic) {
    esp_mqtt_client_subscribe(client, topic, 0);  // sub
}

void mqtt_publish(char *topic, char *msg) {
    esp_mqtt_client_publish(client, topic, msg, 0, 0, false);  // pub
}

void mqtt_set_conn_event_callback(void *cb) {
    conn_event_callback = cb;  // set callback
}

void mqtt_set_disconn_event_callback(void *cb) {
    disconn_event_callback = cb;  // set callback
}

void mqtt_set_sub_event_callback(void *cb) {
    sub_event_callback = cb;  // set callback
}

void mqtt_set_unsub_event_callback(void *cb) {
    unsub_event_callback = cb;  // set callback
}

void mqtt_set_pub_event_callback(void *cb) {
    pub_event_callback = cb;  // set callback
}

void mqtt_set_data_event_callback(void *cb) {
    data_event_callback = cb;  // set callback
}