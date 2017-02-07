#ifndef __LED_H
#define	__LED_H

#include "nrf_gpio.h"


#define LED_START      18
#define LED_1          18
#define LED_2          19
#define LED_3          20
#define LED_4          21
#define LED_5          22
#define LED_STOP       22

#define LED1_ON()		nrf_gpio_pin_set(LED_1);
#define LED1_OFF()		nrf_gpio_pin_clear(LED_1);
#define LED1_TOG()		nrf_gpio_pin_toggle(LED_1);

#define LED2_ON()		nrf_gpio_pin_set(LED_2);
#define LED2_OFF()		nrf_gpio_pin_clear(LED_2);
#define LED2_TOG()		nrf_gpio_pin_toggle(LED_2);

#define LED3_ON()		nrf_gpio_pin_set(LED_3);
#define LED3_OFF()		nrf_gpio_pin_clear(LED_3);
#define LED3_TOG()		nrf_gpio_pin_toggle(LED_3);

#define LED4_ON()		nrf_gpio_pin_set(LED_4);
#define LED4_OFF()		nrf_gpio_pin_clear(LED_4);
#define LED4_TOG()		nrf_gpio_pin_toggle(LED_4);

#define LED5_ON()		nrf_gpio_pin_set(LED_5);
#define LED5_OFF()		nrf_gpio_pin_clear(LED_5);
#define LED5_TOG()		nrf_gpio_pin_toggle(LED_5);

void LED_Init(void);


#endif /* __LED_H */

