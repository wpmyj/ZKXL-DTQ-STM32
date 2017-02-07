#ifndef __MY_MISC_FUN_H
#define	__MY_MISC_FUN_H

//#include "main.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "my_radio.h"



uint8_t XOR_Cal(uint8_t * dat,uint16_t length);
void gpio_default_init(void);
void clocks_start(void);
uint8_t get_random_number(void);
void parameters_init(void);
#endif 

