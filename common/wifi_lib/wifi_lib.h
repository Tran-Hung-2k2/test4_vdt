#ifndef WIFI_LIB_H
#define WIFI_LIB_H

/**
 * Định nghĩa kiểu con trỏ hàm callback khi sự kiện kết nối WiFi được khởi tạo.
 * Tham số và giá trị trả về của hàm callback là void.
 */
typedef void (*wifi_sta_start_event_callback_t)(void);

/**
 * Định nghĩa kiểu con trỏ hàm callback khi sự kiện mất kết nối WiFi diễn ra.
 * Tham số và giá trị trả về của hàm callback là void.
 */
typedef void (*wifi_sta_disconn_event_callback_t)(void);

/**
 * Định nghĩa kiểu con trỏ hàm callback khi sự kiện kết nối WiFi thành công diễn
 * ra. Tham số và giá trị trả về của hàm callback là void.
 */
typedef void (*wifi_connected_event_callback_t)(void);

/**
 * Hàm bắt đầu chế độ cấu hình thông minh WiFi.
 */
void smartconfig_start();

/**
 * Hàm khởi tạo kết nối WiFi dành cho Station Mode.
 * @param ssid Tên mạng WiFi.
 * @param password Mật khẩu mạng WiFi.
 */
void wifi_station_init(char *ssid, char *password);

/**
 * Hàm đăng ký callback khi sự kiện kết nối WiFi được khởi tạo.
 * @param cb Con trỏ tới hàm callback.
 */
void smartconfig_set_wifi_sta_start_event_callback(void *cb);

/**
 * Hàm đăng ký callback khi sự kiện mất kết nối WiFi diễn ra.
 * @param cb Con trỏ tới hàm callback.
 */
void smartconfig_set_wifi_sta_disconn_event_callback(void *cb);

/**
 * Hàm đăng ký callback khi sự kiện kết nối WiFi thành công diễn ra.
 * @param cb Con trỏ tới hàm callback.
 */
void smartconfig_set_wifi_connected_event_callback(void *cb);

#endif
