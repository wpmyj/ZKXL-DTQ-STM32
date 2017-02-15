/**
  ******************************************************************************
  * @file   	app_clickers_systick_process.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  * @Changelog :
  *    [1].Date   : 2017-02-08
	*        Author : sam.wu
	*        brief  : 心跳处理函数文件
  ******************************************************************************
  */
	
#include "app_systick_package_process.h"
#include "app_send_data_process.h"

extern uint16_t list_tcb_table[UID_TABLE_NUM][8];

/******************************************************************************
  Function:App_clickers_systick_process
  Description:
		App RF 心跳处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_systick_process(void)
{
	Uart_MessageTypeDef systick_package;
	uint8_t status = 0;

	uint8_t send_data_status   = get_clicker_send_data_status() ;
	uint8_t single_data_status = get_single_send_data_status();

	status = send_data_status | single_data_status;

	if(( status == 0 ) && ( system_rtc_timer.sync_flg == 1 ))
	{
		systick_package.HEADER   = 0x5C;
		systick_package.TYPE     = 0x10;
		memset(systick_package.SIGN,0x00,4);
		systick_package.LEN      = 17;
		systick_package.DATA[ 0] = 0x5A;
		systick_package.DATA[ 1] = 0;
		systick_package.DATA[ 2] = 0;
		systick_package.DATA[ 3] = 0;
		systick_package.DATA[ 4] = 0;
		systick_package.DATA[ 5] = 0x00;
		systick_package.DATA[ 6] = 0x31;
		systick_package.DATA[ 7] = 0x07;
		*(uint16_t *)(systick_package.DATA+8) = system_rtc_timer.year;
		systick_package.DATA[10] = system_rtc_timer.mon;
		systick_package.DATA[11] = system_rtc_timer.date;
		systick_package.DATA[12] = system_rtc_timer.hour;
		systick_package.DATA[13] = system_rtc_timer.min;
		systick_package.DATA[14] = system_rtc_timer.sec;

		systick_package.DATA[15] = XOR_Cal(systick_package.DATA+1, 14);
		systick_package.DATA[16] = 0xCA;

		systick_package.XOR = XOR_Cal((uint8_t *)(&(systick_package.TYPE)), 17+6);
		systick_package.END = 0xCA;

		memset( (uint8_t *)(list_tcb_table[SISTICK_SUM_TABLE]), 0xFF, 16 );
		
		whitelist_checktable_and( 0, SISTICK_SUM_TABLE, SEND_PRE_TABLE );
		nrf_transmit_start(systick_package.DATA,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
										 SEND_PRE_DELAY100US, SISTICK_SUM_TABLE,PACKAGE_NUM_ADD);
		nrf_transmit_start(systick_package.DATA, systick_package.LEN,
						NRF_DATA_IS_USEFUL, SEND_DATA_COUNT, SEND_DATA_DELAY100US, SISTICK_SUM_TABLE,PACKAGE_NUM_SAM);

		system_rtc_timer.sync_flg = 0;
	}
}

