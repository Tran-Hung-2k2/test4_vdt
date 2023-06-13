#ifndef GPIO_LIB_H
#define GPIO_LIB_H

// static void IRAM_ATTR gpio_intr_handler(void *args) {
//     int gpio_num = (uint32_t) args;
//     if (gpio_num == GPIO_NUM_0) {
//         static int isOn = 1;
//         gpio_set_level(GPIO_NUM_2, isOn);
//         isOn = 1 - isOn;
//     }
// }

/*
intr_type:
GPIO_INTR_DISABLE = 0,      Disable GPIO interrupt
GPIO_INTR_POSEDGE = 1,      rising edge
GPIO_INTR_NEGEDGE = 2,      falling edge
GPIO_INTR_ANYEDGE = 3,      both rising and falling edge
GPIO_INTR_LOW_LEVEL = 4,    input low level trigger
GPIO_INTR_HIGH_LEVEL = 5,   input high level trigger

mode:
GPIO_MODE_DISABLE
GPIO_MODE_INPUT
GPIO_MODE_OUTPUT
*/
void gpio_init_io(gpio_num_t gpio_num, gpio_mode_t mode,
                  gpio_int_type_t intr_type);
void gpio_set_intr(gpio_num_t gpio_num, gpio_isr_t isr_handle, void *args);
void gpio_toggle_level(gpio_num_t gpio_num);

#endif
