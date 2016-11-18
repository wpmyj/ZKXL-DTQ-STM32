#ifndef __APP_SYSTICK_PACKAGE_PROCESS_H_
#define __APP_SYSTICK_PACKAGE_PROCESS_H_

#include "main.h"

#define SISTICK_SUM_TABLE         10

void rf_change_systick_status(uint8_t rf_status);
uint8_t rf_get_systick_status(void);
void App_clickers_systick_process(void);
void systick_package_timer_init( void );

#endif
