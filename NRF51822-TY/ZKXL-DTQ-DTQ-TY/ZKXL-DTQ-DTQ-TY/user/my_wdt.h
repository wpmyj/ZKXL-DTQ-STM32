#ifndef __MY_WDT_H
#define	__MY_WDT_H

#include "nrf_drv_wdt.h"
#include "app_util_platform.h"
#include "app_error.h"

#include "uart.h"

extern nrf_drv_wdt_channel_id m_channel_id;

void my_wdt_init(void);

#endif 

