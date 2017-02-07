#ifndef __APP_SYSTICK_PACKAGE_PROCESS_H_
#define __APP_SYSTICK_PACKAGE_PROCESS_H_

#include "main.h"

#define SISTICK_SUM_TABLE         10
#define SISTICK_ACK_TABLE         11

void rf_change_systick_status(uint8_t rf_status);
uint8_t rf_get_systick_status(void);
void App_clickers_systick_process(void);
void systick_package_timer_init( void );
uint8_t systick_get_ack_funcction_para( void );
void systick_set_ack_funcction( uint8_t open_or_close );

#endif
