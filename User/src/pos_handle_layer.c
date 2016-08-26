/**
  ******************************************************************************
  * @file   	pos_handle_layer.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

static void send_to_pos(void);
static void receive_from_pos(void);

void pos_handle_layer(void)
{
	send_to_pos();
	receive_from_pos();
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

static void receive_from_pos(void)
{
	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		Buf_CtrPosToApp[0] = uart232_var.HEADER;
		Buf_CtrPosToApp[1] = uart232_var.TYPE;
		flag_App_or_Ctr = 0x0d;												//xor出错
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		Buf_CtrPosToApp[0] = uart232_var.HEADER;
		Buf_CtrPosToApp[1] = uart232_var.TYPE;
		flag_App_or_Ctr = 0xfe;												//长度出错
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				//串口接收到pos指令完成且系统空闲
	{	
		switch(uart232_var.TYPE)
		{
			case 0x10:
				if(uart232_var.uart_rx_buf[6] != uart232_var.uart_rx_buf[14] + 10)//下发指令长度出错，进入应用层处理
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.LEN;							//获取数据长度
				memcpy(Buf_CtrPosToApp, uart232_var.DATA, uart232_var.LEN);		//获取数据内容
				flag_App_or_Ctr = 0x01;											//下发给答题器
				break;
			case 0x12:                                                          //停止下发数据
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;											//下发给答题器
				break;
			case 0x20:		//添加白名单
				if(uart232_var.LEN != 4*uart232_var.uart_rx_buf[7] + 1)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.uart_rx_buf[6];					//获取数据长度
				memcpy(Buf_CtrPosToApp, &uart232_var.uart_rx_buf[7], Length_CtrPosToApp);//获取数据内容
				flag_App_or_Ctr = 0x11;											//透传给答题器的指令
				break;
			case 0x21:		//删除白名单
				if(uart232_var.uart_rx_buf[6] != 4*uart232_var.uart_rx_buf[7] + 1)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.uart_rx_buf[6];					//获取数据长度
				memcpy(Buf_CtrPosToApp, &uart232_var.uart_rx_buf[7], Length_CtrPosToApp);//获取数据内容
				flag_App_or_Ctr = 0x12;											//透传给答题器的指令	
				break;
			case 0x22:		//初始化白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//透传给答题器的指令	
				break;	
			case 0x23:		//开启白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//透传给答题器的指令	
				break;	
			case 0x24:		//关闭白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//透传给答题器的指令	
				break;	
			case 0x25:		//开始考勤
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//透传给答题器的指令	
				break;	
			case 0x27:		//停止考勤
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//透传给答题器的指令	
				break;	
			case 0x28:		//开始配对
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//透传给答题器的指令	
				break;	
			case 0x2A:		//停止配对
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//透传给答题器的指令	
				break;	
			case 0x2B:		//打印当前白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//透传给答题器的指令	
				break;	
			case 0x2C:		//获取设备信息
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					Buf_CtrPosToApp[0] = uart232_var.HEADER;
					Buf_CtrPosToApp[1] = uart232_var.TYPE;
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1b;											//透传给答题器的指令	
				break;					
			default:		//无法识别的指令
				flag_App_or_Ctr = 0xff;	
				Buf_CtrPosToApp[0] = uart232_var.HEADER;
				Buf_CtrPosToApp[1] = uart232_var.TYPE;
				break;
		}			
		hal_uart_clr_rx();														//清除接收数据
	}	
}


/**
  * @}
  */
/**************************************END OF FILE****************************/

