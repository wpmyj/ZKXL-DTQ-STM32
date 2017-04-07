#ifndef __MY_TIMER_H
#define	__MY_TIMER_H

#include "app_timer.h"
#include "my_radio.h"




void timers_init(void);
void temp_timeout_start(void);
void temp_timeout_stop(void);
void temp_timer_handler(void * p_context);
void nrf_transmit_timeout_start(uint8_t timer_ms);
void nrf_transmit_timeout_stop(void);
void nrf_transmit_timer_handler(void * p_context);

#endif 

