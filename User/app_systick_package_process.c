#include "app_systick_package_process.h"
#include "app_send_data_process.h"

static uint8_t rf_systick_status = 0; 
static Timer_typedef systick_package_timer;

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

	/* 10s 时间到 发送新的心跳包到答题器 */
	if(systick_current_status == 2)
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
		systick_package.DATA[ 8] = 20;
		systick_package.DATA[ 9] = 16;
		systick_package.DATA[10] = 11;
		systick_package.DATA[11] = 16;
		systick_package.DATA[12] = 17;
		systick_package.DATA[13] = 05;
		systick_package.DATA[14] = 16;
		systick_package.DATA[15] = XOR_Cal(systick_package.DATA+1, 14);
		systick_package.DATA[16] = 0xCA;

		systick_package.XOR = XOR_Cal((uint8_t *)(&(systick_package.TYPE)), 17+6);
		systick_package.END = 0xCA;

		nrf_transmit_start(systick_package.DATA, systick_package.LEN, 
		        NRF_DATA_IS_USEFUL, SEND_DATA_COUNT, SEND_DATA_DELAY100US, 1);

		rf_change_systick_status(1);
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
