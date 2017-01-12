#include "my_timer.h"

#define TIMER_DEBUG
#ifdef TIMER_DEBUG
#define timer_debug  printf   
#else  
#define timer_debug(...)                    
#endif 

//定时器宏定义
#define APP_TIMER_PRESCALER     0
#define APP_TIMER_OP_QUEUE_SIZE 5

#define TEMP_TIMEOUT_INTERVAL     			APP_TIMER_TICKS(10000, 	APP_TIMER_PRESCALER)
#define NRF_RECEIVE_TIMEOUT_INTERVAL     	APP_TIMER_TICKS(500, 	APP_TIMER_PRESCALER)
#define NRF_TRANSMIT_TIMEOUT_INTERVAL     	APP_TIMER_TICKS(50, 	APP_TIMER_PRESCALER)


APP_TIMER_DEF(temp_timer_id);
//APP_TIMER_DEF(nrf_receive_timer_id); 
APP_TIMER_DEF(nrf_transmit_timer_id);


void timers_init(void)
{
	uint32_t err_code;
	
	APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
	
	err_code = app_timer_create(&temp_timer_id,APP_TIMER_MODE_REPEATED,temp_timer_handler);
	APP_ERROR_CHECK(err_code);
	
	err_code = app_timer_create(&nrf_transmit_timer_id,APP_TIMER_MODE_REPEATED,nrf_transmit_timer_handler);
	APP_ERROR_CHECK(err_code);
	

}

void temp_timeout_start(void)
{
	uint32_t err_code;
	err_code = app_timer_start(temp_timer_id,TEMP_TIMEOUT_INTERVAL,NULL);
	APP_ERROR_CHECK(err_code);
}

void temp_timeout_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(temp_timer_id);
	APP_ERROR_CHECK(err_code);
}

void temp_timer_handler(void * p_context)
{
	//timer_debug("timer_debug:temp_timer_handler \r\n");
	
    NRF_CLOCK->EVENTS_DONE = 0;		
    NRF_CLOCK->TASKS_CAL = 1;
	
}




void nrf_transmit_timeout_start(uint8_t timer_ms)
{
	uint32_t err_code;
	err_code = app_timer_start(nrf_transmit_timer_id,APP_TIMER_TICKS(timer_ms,APP_TIMER_PRESCALER),NULL);
	APP_ERROR_CHECK(err_code);
}

void nrf_transmit_timeout_stop(void)
{
	uint32_t err_code;
	err_code = app_timer_stop(nrf_transmit_timer_id);
	APP_ERROR_CHECK(err_code);
}

void nrf_transmit_timer_handler(void * p_context)
{
	nrf_tx_flg = true;
}














