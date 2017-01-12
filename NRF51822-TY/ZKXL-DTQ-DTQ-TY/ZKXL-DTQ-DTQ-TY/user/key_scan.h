/** @file key_scan.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2012 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2015.11.28
 * $Revision: 	V1000
 *
 */
 
#ifndef _KEY_SCAN_H_
#define _KEY_SCAN_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "nrf_gpio.h"
//#include "pca10001.h"
#include "nrf_delay.h"
#include "define.h"
#include "timer.h"

//¼üÅÌÐÐIO¿Ú
#define KEY_ROW1		    	(0u)
#define KEY_ROW2		    	(1u)
#define KEY_ROW3		    	(2u)
#define KEY_ROW4		    	(3u)
#define	KEY_COL1				(4u)
#define	KEY_COL2				(5u)
#define	KEY_COL3				(6u)
#define	KEY_COL4				(7u)

#define	KEY_PWR					(30u)



#define KEY_PORT 				(NRF_GPIO_PORT_SELECT_PORT0)
#define KEY_VALUE_MASK			(0x0F)

#define ENABLE_KEY_SCAN_DEBUGLOG
#ifdef ENABLE_KEY_SCAN_DEBUGLOG
	#define KeyScan_DebugLog		printf
#else
	#define KeyScan_DebugLog(...)
#endif //ENABLE_KEY_SCAN_DEBUGLOG

void Key_Init(void);
uint8_t Key_scan(bool key_down_wait_flag);



 
#endif	//_KEY_SCAN_H_
