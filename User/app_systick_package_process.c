#include "app_systick_package_process.h"
#include "app_send_data_process.h"

static uint8_t rf_systick_status = 0; 
static uint8_t open_systick_ack  = 0;
extern uint16_t list_tcb_table[13][8];
Process_tcb_Typedef systick_process;

/******************************************************************************
  Function:rf_change_systick_status
  Description:
		修改systick的状态
  Input :
		rf_status: systick的新状态
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_change_systick_status(uint8_t rf_status)
{
	rf_systick_status = rf_status;
	DebugLog("<%s> rf_systick_status = %d \r\n",__func__,rf_systick_status);
}

/******************************************************************************
  Function:rf_get_systick_status
  Description:
		获取systick的状态
  Input :
  Output:systick的新状态
  Return:
  Others:None
******************************************************************************/
uint8_t rf_get_systick_status(void)
{
	return rf_systick_status ;
}

/******************************************************************************
  Function:systick_set_ack_funcction
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
void systick_set_ack_funcction( uint8_t open_or_close )
{
	open_systick_ack = open_or_close;
	DebugLog("<%s> rf_systick_status = %d \r\n",__func__,open_systick_ack);
}

/******************************************************************************
  Function:systick_set_ack_funcction
  Description:
  Input :
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t systick_get_ack_funcction_para( void )
{
	return open_systick_ack;
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void App_clickers_systick_process(void)
{
	Uart_MessageTypeDef systick_package;
	uint8_t systick_current_status = 0;

	/* 获取当前的systick的状态 */
	systick_current_status = rf_get_systick_status();

	/* 30s 时间到 发送新的心跳包到答题器 */
	if(systick_current_status == 2)
	{
		if(open_systick_ack == 1)
		{
			Uart_MessageTypeDef  systick_massage;
			uint8_t Is_over = 1;

			while( Is_over )
			{
				DEBUG_UID_LOG("\r\nststick online uids:\r\n");
				systick_massage.LEN = 0;
				Is_over = checkout_online_uids( 0, SISTICK_ACK_TABLE, 1, systick_massage.DATA,&(systick_massage.LEN));
				if(systick_massage.LEN != 0)
				{
					#ifdef ENABLE_SEND_DATA_TO_PC
					if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						systick_massage.HEADER = 0x5C;
						systick_massage.TYPE = 0x43;
						memcpy(systick_massage.SIGN,systick_process.uid,4);
						systick_massage.XOR =  XOR_Cal((uint8_t *)(&(systick_massage.TYPE)),
			                                 systick_massage.LEN+6);
						systick_massage.END = 0xCA;
						serial_ringbuffer_write_data(SEND_RINGBUFFER,&systick_massage);
					}
					#endif
				}
			}
			memset( (uint8_t *)(list_tcb_table[SISTICK_ACK_TABLE]), 0x00, 16 );
		}
		rf_change_systick_status(3);
		return ;
	}

	if(systick_current_status == 3)
	{
		systick_package.HEADER = 0x5C;
		systick_package.TYPE   = 0x10;
		memset(systick_package.SIGN,0x00,4);
		systick_package.LEN    = 17;

		systick_package.DATA[ 0] = 0x5A;
		systick_package.DATA[ 1] = 0;
		systick_package.DATA[ 2] = 0;
		systick_package.DATA[ 3] = 0;
		systick_package.DATA[ 4] = 0;
		systick_package.DATA[ 5] = 0x00;
		systick_package.DATA[ 6] = 0x31;
		systick_package.DATA[ 7] = 0x07;

		if( system_rtc_timer.sync_flg == 1 )
		{
			*(uint16_t *)(systick_package.DATA+8) = system_rtc_timer.year;
			systick_package.DATA[10] = system_rtc_timer.mon;
			systick_package.DATA[11] = system_rtc_timer.date;
			systick_package.DATA[12] = system_rtc_timer.hour;
			systick_package.DATA[13] = system_rtc_timer.min;
			systick_package.DATA[14] = system_rtc_timer.sec;
		}
		else
		{
			memset(systick_package.DATA+8,0x00,7);
		}

		systick_package.DATA[15] = XOR_Cal(systick_package.DATA+1, 14);
		systick_package.DATA[16] = 0xCA;

		systick_package.XOR = XOR_Cal((uint8_t *)(&(systick_package.TYPE)), 17+6);
		systick_package.END = 0xCA;

		if( open_systick_ack == 1 )
		{
			memset( (uint8_t *)(list_tcb_table[SISTICK_SUM_TABLE]), 0x00, 16 );
		}
		else
		{
			memset( (uint8_t *)(list_tcb_table[SISTICK_SUM_TABLE]), 0xFF, 16 );
		}
		whitelist_checktable_and( 0, SISTICK_SUM_TABLE, SEND_PRE_TABLE );
		nrf_transmit_start(systick_package.DATA,0,NRF_DATA_IS_PRE,SEND_PRE_COUNT,
										 SEND_PRE_DELAY100US, SISTICK_SUM_TABLE,PACKAGE_NUM_ADD);
		nrf_transmit_start(systick_package.DATA, systick_package.LEN,
						NRF_DATA_IS_USEFUL, SEND_DATA_COUNT, SEND_DATA_DELAY100US, SISTICK_SUM_TABLE,PACKAGE_NUM_ADD);

		rf_change_systick_status(1);
		return ;
	}
}

/******************************************************************************
  Function:systick_timer_init
  Description:
  Input :
  Return:
  Others:None
******************************************************************************/
void systick_package_timer_init( void )
{
	sw_create_timer(&systick_package_timer , 30000, 1, 2,&(rf_systick_status), NULL);
	rf_change_systick_status(1);
}
