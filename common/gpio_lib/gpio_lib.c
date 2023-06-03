#include <driver/gpio.h>
#include <gpio_lib.h>
#include <stdio.h>

void create_io(gpio_num_t gpio_num, gpio_mode_t mode,
               gpio_int_type_t intr_type) {
    gpio_config_t io_conf = {
        .intr_type = intr_type,
        .mode = mode,
        .pin_bit_mask = 1ULL << gpio_num,
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);
}

void set_intr_io(gpio_num_t gpio_num, gpio_isr_t isr_handle, void *args) {
    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, isr_handle, args);
}

void toggle_io_level(gpio_num_t gpio_num) {
    static int isOn = 1;
    gpio_set_level(gpio_num, isOn);
    isOn = 1 - isOn;
}
