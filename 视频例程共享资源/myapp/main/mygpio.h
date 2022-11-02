#ifndef __MY_GPIO_H
#define __MY_GPIO_H

#include "myapp.h"

#include "driver/gpio.h"

#define APP_STA_LED			GPIO_NUM_2

typedef enum
{
  GPIO_PIN_RESET = 0u,
  GPIO_PIN_SET
} gpio_pin_state;

#define GPIO_OUTPUT_PIN_SEL     ( (1ULL<<GPIO_NUM_2) | \
                                  (1ULL<<GPIO_NUM_4) | \
                                  (1ULL<<GPIO_NUM_5) | \
                                  (1ULL<<GPIO_NUM_12)|	\
                                  (1ULL<<GPIO_NUM_13)| \
                                  (1ULL<<GPIO_NUM_14)| \
                                  (1ULL<<GPIO_NUM_15)| \
                                  (1ULL<<GPIO_NUM_16))


void my_gpio_init();
void my_set_sta_flag(SYS_LED_STA sta);
void my_gpio_write(gpio_num_t gpio_num, gpio_pin_state state);
void my_gpio_toggle(gpio_num_t gpio_num);
void my_gpio_handler(uint8_t cmd);
uint8_t my_get_all_gpio_sta(void);
void start_gpio_task(void);
#endif

