#ifndef MQTT_H
#define MQTT_H

typedef void (*conn_event_callback_t)(void);
typedef void (*disconn_event_callback_t)(void);
typedef void (*sub_event_callback_t)(void);
typedef void (*unsub_event_callback_t)(void);
typedef void (*pub_event_callback_t)(void);
typedef void (*data_event_callback_t)(char *, char *);

// Hàm log lỗi nếu mã lỗi khác 0
void log_error_if_nonzero(const char *message, int error_code);
// Hàm khởi tạo và kết nối với broker MQTT
void mqtt_app_start(char *broker_uri, char *username, char *password);
// Subscribe topic
void mqtt_subscribe(char *topic);
// Public bản tin
void mqtt_publish(char *topic, char *msg);
// Set callback cho các event
void mqtt_set_conn_event_callback(void *cb);
void mqtt_set_disconn_event_callback(void *cb);
void mqtt_set_sub_event_callback(void *cb);
void mqtt_set_unsub_event_callback(void *cb);
void mqtt_set_pub_event_callback(void *cb);
void mqtt_set_data_event_callback(void *cb);

#endif
