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
/* Private define ------------------------------------------------------------*/
/* Set parameter of budffer */
#define APP_CTR_IDLE                      0x00 // 0x00: idle
#define APP_CTR_SENT_DATA_VIA_2_4G        0x01 // 0x01: 通过2.4G下发数据包处理
#define APP_CTR_GET_DATA_VIA_2_4G         0x02 // 0x02: 通过2.4G接收过来的数据包处理
#define APP_CTR_ADD_WHITE_LIST            0x11 // 0x11: 添加白名单
#define APP_CTR_DEL_WHITE_LIST            0x12 // 0x12: 删除白名单
#define APP_CTR_INIT_WHITE_LIST           0x13 // 0x13: 初始化白名单
#define APP_CTR_OPEN_WHITE_LIST           0x14 // 0x14: 开启白名单
#define APP_CTR_CLOSE_WHITE_LIST          0x15 // 0x15: 关闭白名单
#define APP_CTR_OPEN_ATTENDANCE           0x16 // 0x16: 开启考勤
#define APP_CTR_CLOSE_ATTENDANCE          0x17 // 0x17: 关闭考勤
#define APP_CTR_UPDATE_ATTENDANCE_DATA    0x18 // 0x18: 上传刷卡数据
#define APP_CTR_OPEN_PAIR                 0x19 // 0x19: 开启配对
#define APP_CTR_CLOSE_PAIR                0x0C // 0x0c: 关闭配对
#define APP_CTR_UPATE_PAIR_DATA           0x0D // 0x0d: 上传配对数据
#define APP_CTR_DATALEN_ERR               0xFE // 0xfe: 帧长度不合法
#define APP_CTR_UNKNOWN                   0xFF // 0xff: 未能识别的帧

/* Private variables ---------------------------------------------------------*/
uint8_t      flag_App_or_Ctr = APP_CTR_IDLE;
Uart_TypeDef uart232_var;					// 串口接收发送缓冲区

/* Private functions ---------------------------------------------------------*/
static void send_to_pos(void);
static void receive_from_pos(void);
static void serial_transmission_to_nrf51822(void);

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
static void serial_transmission_to_nrf51822(void)
{
	Buf_CtrPosToApp[0] = uart232_var.HEADER;
	Buf_CtrPosToApp[1] = uart232_var.TYPE;
	
	Buf_CtrPosToApp[2] = uart232_var.SIGN[0];
	Buf_CtrPosToApp[3] = uart232_var.SIGN[1];
	Buf_CtrPosToApp[4] = uart232_var.SIGN[2];
	Buf_CtrPosToApp[5] = uart232_var.SIGN[3];
	
	Buf_CtrPosToApp[6] = uart232_var.LEN;
	
	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor出错
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//长度出错
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				  //串口接收到pos指令完成且系统空闲
	{	
		Length_CtrPosToApp = uart232_var.LEN+7;							                //获取数据长度
		memcpy(Buf_CtrPosToApp+7, uart232_var.DATA, uart232_var.LEN);		    //获取数据内容
		flag_App_or_Ctr = 0x01;	
		hal_uart_clr_rx();														                      //清除接收数据
		return ;
	}
	
}

static void receive_from_pos(void)
{
	Buf_CtrPosToApp[0] = uart232_var.HEADER;
	Buf_CtrPosToApp[1] = uart232_var.TYPE;

	if(uart232_var.flag_uart_rx_xor_err)
	{
		uart232_var.flag_uart_rx_xor_err = false;
		flag_App_or_Ctr = APP_CTR_UPATE_PAIR_DATA;												//xor出错
		hal_uart_clr_rx();
		return;
	}
	
	if(uart232_var.flag_uart_rx_length_err)
	{
		uart232_var.flag_uart_rx_length_err = false;
		flag_App_or_Ctr = APP_CTR_DATALEN_ERR;												//长度出错
		hal_uart_clr_rx();
		return;
	}
	
	if((uart232_var.flag_uart_rx_ok) && ( flag_App_or_Ctr == 0))				    //串口接收到pos指令完成且系统空闲
	{	
		switch(uart232_var.TYPE)
		{
			case 0x10:
				if(uart232_var.uart_rx_buf[6] != uart232_var.uart_rx_buf[14] + 10)//下发指令长度出错，进入应用层处理
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				Length_CtrPosToApp = uart232_var.LEN;							                //获取数据长度
				memcpy(Buf_CtrPosToApp, uart232_var.DATA, uart232_var.LEN);		    //获取数据内容
				flag_App_or_Ctr = 0x01;											                      //下发给答题器
				break;
			case 0x12:                                                          //停止下发数据
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x04;											//下发给答题器
				break;
			case 0x20:		//添加白名单
				if(uart232_var.LEN != 4*uart232_var.uart_rx_buf[7] + 1)
				{
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
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x13;											//透传给答题器的指令	
				break;	
			case 0x23:		//开启白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x14;											//透传给答题器的指令	
				break;	
			case 0x24:		//关闭白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x15;											//透传给答题器的指令	
				break;	
			case 0x25:		//开始考勤
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x16;											//透传给答题器的指令	
				break;	
			case 0x27:		//停止考勤
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x17;											//透传给答题器的指令	
				break;	
			case 0x28:		//开始配对
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x18;											//透传给答题器的指令	
				break;	
			case 0x2A:		//停止配对
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x19;											//透传给答题器的指令	
				break;	
			case 0x2B:		//打印当前白名单
				if(uart232_var.uart_rx_buf[6] != 0)
				{
					flag_App_or_Ctr = 0xfe;	
					break;
				}
				flag_App_or_Ctr = 0x1a;											//透传给答题器的指令	
				break;	
			case 0x2C:		//获取设备信息
				if(uart232_var.uart_rx_buf[6] != 0)
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
		hal_uart_clr_rx();														//清除接收数据
	}	
}


/**
  * @}
  */
/**************************************END OF FILE****************************/

