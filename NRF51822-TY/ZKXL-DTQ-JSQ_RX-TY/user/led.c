#include "led.h"


void LED_Init(void)
{
	nrf_gpio_range_cfg_output(LED_START, LED_STOP);
}






