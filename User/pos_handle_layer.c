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
	if(App_to_CtrPosReq)
	{
#ifdef ENABLE_DEBUG_LOG
		app_debuglog_dump(Buf_AppToCtrPos, Length_AppToCtrPos);
#else		
		CtrUartSendTask(Buf_AppToCtrPos, Length_AppToCtrPos);
#endif //ENABLE_DEBUG_LOG	
		App_to_CtrPosReq = false;
	}
}

//	uint8_t 				HEADER;						  //中断串口接收帧头
//	uint8_t 				TYPE;								//中断串口接收包类型
//	uint8_t         SIGN[4];            //中断串口接收活动标识
//	uint8_t 				LEN;								//中断串口接收数据长度
//	uint8_t 				DATA[UART_NBUF];		//中断串口接收数据
//	uint8_t 				XOR;								//中断串口接收异或
//	uint8_t 				END;								//中断串口接收帧尾
//static void serial_transmission_to_nrf51822(void)
//{
//	Buf_CtrPosToApp[0] = uart232_var.HEADER;
//	Buf_CtrPosToApp[1] = uart232_var.TYPE;
//	
//	Buf_CtrPosToApp[2] = uart232_var.SIGN[0];
//	Buf_CtrPosToApp[3] = uart232_var.SIGN[1];
//	Buf_CtrPosToApp[4] = uart232_var.SIGN[2];
//	Buf_CtrPosToApp[5] = uart232_var.SIGN[3];
//	
//	Buf_CtrPosToApp[6] = uart232_var.LEN;
//	
//	if(uart232_var.flag_uart_rx_xor_err)
//	{
//		uart232_var.flag_uart_rx_xor_err = false;
//		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor出错
//		hal_uart_clr_rx();
//		return;
//	}
//	
//	if(uart232_var.flag_uart_rx_length_err)
//	{
//		uart232_var.flag_uart_rx_length_err = false;
//		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//长度出错
//		hal_uart_clr_rx();
//		return;
//	}
//	
//	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				  //串口接收到pos指令完成且系统空闲
//	{	
//		Length_CtrPosToApp = uart232_var.LEN+7;							                //获取数据长度
//		memcpy(Buf_CtrPosToApp+7, uart232_var.DATA, uart232_var.LEN);		    //获取数据内容
//		flag_App_or_Ctr = 0x01;	
//		hal_uart_clr_rx();														                      //清除接收数据
//		return ;
//	}
//	
//}

static void receive_from_pos(void)
{
	Uart_MessageTypeDef CurrentProcessMessage;
	
	if(BUFFEREMPTY == buffer_get_buffer_status())
		return;
	else
		serial_ringbuffer_read_data(&CurrentProcessMessage);
	
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

/*********************************************************************************
* 功	能：void UartSendBuffferInit(void)
* 输  入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void UartSendBuffferInit(void)
{
	uart232_var.flag_tx_ok[0] = true;
	uart232_var.flag_tx_ok[1] = true;
}

/*********************************************************************************
* 功	能：void CtrUartSendTask(uint8_t *ptr,uint8_t len)
* 输    入: ptr		发送缓冲区
* 			len		发送长度
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void CtrUartSendTask(uint8_t *ptr,uint8_t len)
{
	uint8_t temp_state = 0;
	if(len > 0)
	{
		if(uart232_var.flag_tx_ok[0] && uart232_var.flag_tx_ok[1])
		{
			temp_state = 1;					//当前系统空闲
		}
		
		if(uart232_var.flag_tx_ok[uart_tx_i])		//如果当前缓冲区空闲，则将数据放入当前缓冲区，否则，放入另外一个
		{
			memcpy(uart232_var.uart_tx_buf[uart_tx_i], ptr, len);
			uart232_var.uart_tx_length[uart_tx_i] = len;
			uart232_var.flag_txing[uart_tx_i] = true;
			uart232_var.flag_tx_ok[uart_tx_i] = false;
		}
		else										//如果第二缓冲区有数据，直接覆盖
		{
			uart232_var.flag_txing[1 - uart_tx_i] = false;
			memcpy(uart232_var.uart_tx_buf[1 - uart_tx_i], ptr, len);
			uart232_var.uart_tx_length[1 - uart_tx_i] = len;
			uart232_var.flag_txing[1 - uart_tx_i] = true;
			uart232_var.flag_tx_ok[1 - uart_tx_i] = false;
		}
		
		if(temp_state)
		{
			uart232_var.uart_tx_cnt = 0;
			//Write one byte to the transmit data register
			USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
			uart232_var.uart_tx_length[uart_tx_i]--;
			USART_ITConfig(USART1pos, USART_IT_TXE, ENABLE);
		}
	}
	
//	for(temp_state=0;temp_state<4;temp_state++)
//  {
//		sign_buffer[temp_state]=0;
//  }	 
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

