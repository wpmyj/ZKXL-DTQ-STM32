/**
  ******************************************************************************
  * @file   	pos_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief
  * @Changelog :
  *    [1].Date   : 2016_8-26
	*        Author : sam.wu
	*        brief  : 尝试分离数据，降低代码的耦合度
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/

extern Uart_TxDataTypeDef  uart232_var;
       uint8_t             flag_App_or_Ctr = APP_CTR_IDLE;		 
	     uint8_t             sign_buffer[4];
			 
extern Uart_MessageTypeDef uart_irq_send_massage;
extern uint8_t uart_tx_status;

/* Private functions ---------------------------------------------------------*/
static void send_to_pos(void);
static void receive_from_pos(void);
//static void serial_transmission_to_nrf51822(void);

void pos_handle_layer(void)
{
	send_to_pos();
	receive_from_pos();
	//serial_transmission_to_nrf51822();
}

static void send_to_pos(void)
{	
	if( uart_tx_status == 0)
	{
		/* enable interrupt Start send data*/
		USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
	
  }
}

static void receive_from_pos(void)
{
	Uart_MessageTypeDef CurrentProcessMessage;
	
	if(BUFFEREMPTY == buffer_get_buffer_status(REVICE_RINGBUFFER))
		return;
	else
		serial_ringbuffer_read_data(REVICE_RINGBUFFER, &CurrentProcessMessage);
	
	memcpy(sign_buffer,CurrentProcessMessage.SIGN,4);
	
	Buf_CtrPosToApp[0] = CurrentProcessMessage.HEADER;
	Buf_CtrPosToApp[1] = CurrentProcessMessage.TYPE;
	
  /* 串口接收到pos指令完成且系统空闲 */
	if( flag_App_or_Ctr == 0 )				    
	{	
		switch(CurrentProcessMessage.TYPE)
		{
			/* 下发给答题器 */
			case 0x10:
				{ 
					/* 获取数据长度 */
					Length_CtrPosToApp = CurrentProcessMessage.LEN;
					
					/* 获取数据内容 */ 
					memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, CurrentProcessMessage.LEN);		 
					flag_App_or_Ctr = 0x01;
				}				
				break;
			
			/* 停止下发数据 */	
			case 0x12:   
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;										
				break;
				
			case 0x20:		//添加白名单
				if(CurrentProcessMessage.LEN != 4*CurrentProcessMessage.DATA[0] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = CurrentProcessMessage.LEN;					//获取数据长度
				memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, Length_CtrPosToApp);//获取数据内容
				flag_App_or_Ctr = 0x11;											//透传给答题器的指令
				break;
				
			case 0x21:		//删除白名单
				if(CurrentProcessMessage.LEN != 4*CurrentProcessMessage.DATA[0] + 1)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = CurrentProcessMessage.LEN;					//获取数据长度
				memcpy(Buf_CtrPosToApp, CurrentProcessMessage.DATA, Length_CtrPosToApp);//获取数据内容
				flag_App_or_Ctr = 0x12;											//透传给答题器的指令	
				break;
				
			case 0x22:		//初始化白名单
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//透传给答题器的指令	
				break;	
			case 0x23:		//开启白名单
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//透传给答题器的指令	
				break;	
			case 0x24:		//关闭白名单
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//透传给答题器的指令	
				break;	
			case 0x25:		//开始考勤
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//透传给答题器的指令	
				break;	
			case 0x27:		//停止考勤
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//透传给答题器的指令	
				break;	
			case 0x28:		//开始配对
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//透传给答题器的指令	
				break;	
			case 0x2A:		//停止配对
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//透传给答题器的指令	
				break;	
			case 0x2B:		//打印当前白名单
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//透传给答题器的指令	
				break;	
			case 0x2C:		//获取设备信息
				if(CurrentProcessMessage.LEN != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1b;											//透传给答题器的指令	
				break;					
			default:		//无法识别的指令
				flag_App_or_Ctr = 0xff;	
				break;
		}
	}	
}

void app_debuglog_dump(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;
	
    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X ", p_buffer[index]);
    }
    DebugLog("\r\n");
}

void app_debuglog_dump_no_space(uint8_t * p_buffer, uint32_t len)
{
	uint32_t index = 0;
	
    for (index = 0; index <  len; index++)
    {
        DebugLog("%02X", p_buffer[index]);
    }
    DebugLog("\r\n");
}
/**
  * @}
  */
/**************************************END OF FILE****************************/

