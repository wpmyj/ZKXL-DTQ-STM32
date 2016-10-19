/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* uart global variables */

clicker_t clickers[120];
uint32_t clicker_test_printf_flg = 0;
timer_t clicker_time;

// revice part
Uart_MessageTypeDef uart_irq_revice_massage;
static uint32_t uart_rx_timeout = 0;
static uint32_t rf_tx_time_cnt = 0;
static uint32_t rf_tx_timeout_cnt = 0;

static bool     flag_uart_rxing = false;
static uint8_t  uart_status     = UartHEADER;

// send part
Uart_MessageTypeDef uart_irq_send_massage;
uint8_t uart_tx_status      = 0;

/* uart global variables */
extern nrf_communication_t	nrf_communication;
extern uint8_t 					    dtq_to_jsq_sequence;
extern uint8_t 			        jsq_to_dtq_sequence;
extern uint8_t 					    dtq_to_jsq_packnum;
extern uint8_t 			        jsq_to_dtq_packnum;
extern uint8_t              sign_buffer[4];
extern uint8_t              retransmit_uid[4];

/* rf systick data */
volatile uint8_t rf_systick_status = 0; // 0 = IDLE
static uint8_t   rf_retransmit_status = 0;
static uint32_t  rf_retransmit_timecnt = 0;

void rf_retransmit_set_status(uint8_t new_status)
{
	rf_retransmit_status = new_status;
	DebugLog("status = %d \r\n",rf_retransmit_status);
}

uint8_t get_rf_retransmit_status(void)
{
	return rf_retransmit_status;
}

void time_inc()
{
	clicker_time.ms++;
	if(clicker_time.ms == 1000)
	{
		clicker_time.ms = 0;
		clicker_time.s++;

		if(clicker_time.s%10==0)
			clicker_test_printf_flg = 1;

		if(clicker_time.s == 60)
		{
			clicker_time.s = 0;
			clicker_time.min++;
			if(clicker_time.min == 60)
			{
				clicker_time.min = 0;
				clicker_time.hour++;
			}
		}
	}
}

/******************************************************************************
  Function:clicker_send_data_statistics
  Description:
		App RF 射频轮询处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void clicker_send_data_statistics( uint8_t send_data_status, uint8_t uidpos )
{
	switch(send_data_status)
	{
		case 1 : set_index_of_white_list_pos(3,uidpos); break;
		case 3 :
		case 4 : set_index_of_white_list_pos(4,uidpos); break;
		case 6 :
		case 7 : set_index_of_white_list_pos(5,uidpos); break;
		case 9 :
		case 10: set_index_of_white_list_pos(8,uidpos); break;
		default:break;
	}
}

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
  Function:uart_clear_message
  Description:
		清除Message中的信息
  Input :
		Message: 协议Message的指针
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_clear_message( Uart_MessageTypeDef *Message )
{
	uint8_t i;
	uint8_t *pdata = (uint8_t*)(Message);

	for(i=0;i<PACKETSIZE;i++)
	{
		*pdata = 0;
		pdata++;
	}
}

/******************************************************************************
  Function:uart_revice_data_state_mechine
  Description:
		串口数据接收函数，提取有效数据存入缓存
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_revice_data_state_mechine( uint8_t data )
{
	static uint8_t	uart_rx_cnt     = 0;
	static uint8_t  temp_sign_len   = 0;

	switch(uart_status)
		{
			case UartHEADER:
				{
					if(UART_SOF == data)		//如果命令头为0x5C则开始接收，否则不接收
					{
						uart_irq_revice_massage.HEADER = data;
						uart_status =  UartTYPE;
						flag_uart_rxing = true;
					}
				}
				break;

			case UartTYPE:
				{
					uart_irq_revice_massage.TYPE = data;
					uart_status = UartSIGN;
					temp_sign_len = 0;
				}
				break;

			case UartSIGN:
				{
						uart_irq_revice_massage.SIGN[temp_sign_len++] = data;
						if( temp_sign_len == 4 )
						{
								uart_status = UartLEN;
						}
				}
				break;

			case UartLEN:
				{
					uart_irq_revice_massage.LEN = data;

					/*  若数据长度大于 236 */
					if(uart_irq_revice_massage.LEN > UART_NBUF)
					{
						uart_status =  UartHEADER;
						/* 清除 uart_irq_revice_massage 接收信息 */
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
					else if(uart_irq_revice_massage.LEN > 0)	//  DATA不为空
					{
						uart_status = UartDATA;
						uart_rx_cnt = 0;
					}
					else//  DATA为空
					{
						uart_status = UartXOR;
					}
				}
				break;

			case UartDATA:
				{
						uart_irq_revice_massage.DATA[uart_rx_cnt++] = data;
						/* 数据接收完成 */
						if(uart_rx_cnt == uart_irq_revice_massage.LEN)
								uart_status = UartXOR;
				}
				break;

			case UartXOR:
				{
						uart_irq_revice_massage.XOR = data;
						uart_status = UartEND;
				}
				break;

			case UartEND:
				{
					if(UART_EOF == data)
					{
						uint8_t UartMessageXor = XOR_Cal((uint8_t *)(&uart_irq_revice_massage.TYPE),
												uart_irq_revice_massage.LEN + 6 );
						uart_irq_revice_massage.END = data;

						if( uart_irq_revice_massage.XOR == UartMessageXor)
						{   /* 若校验通过，则接收数据OK可用 */
								if(BUFFERFULL != buffer_get_buffer_status(REVICE_RINGBUFFER))
								{
									serial_ringbuffer_write_data(REVICE_RINGBUFFER,&uart_irq_revice_massage);
								}
							  flag_uart_rxing = false;
								uart_status = UartHEADER;
								uart_clear_message(&uart_irq_revice_massage);
						}
						else
						{
							uart_clear_message(&uart_irq_revice_massage);
						}
					}
					else
					{
						uart_status = UartHEADER;
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
				}
				break;

			default:
				break;
		}
}

/******************************************************************************
  Function:uart_send_data_state_machine
  Description:
		串口数发送函数，从缓存中提取数据发送到上位机
  Input :
		status: uart tx status
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_send_data_state_machine( void )
{
	static uint8_t uart_tx_cnt  = 0;
	static uint8_t *pdata;

	switch( uart_tx_status )
	{
		case 0:
			{
					if(BUFFEREMPTY == buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
						return;
					}
					else
					{
						serial_ringbuffer_read_data(SEND_RINGBUFFER, &uart_irq_send_massage);
						pdata = (uint8_t *)(&uart_irq_send_massage);
						uart_tx_status = 1;
						uart_tx_cnt = *(pdata+6) + 7;
						uart_tx_status = 1;
					}
			}
			break;

		case 1:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					pdata = &(uart_irq_send_massage.XOR);
					uart_tx_cnt = 2;
					uart_tx_status = 2;
				}
			}
			break;

		case 2:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					uart_clear_message(&uart_irq_send_massage);
					uart_tx_status = 0;
				}
			}
			break;

		default:
			break;
	}
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 消息缓存处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void rf_move_data_to_buffer(nrf_communication_t *Message)
{
	Uart_MessageTypeDef rf_message;
	uint8_t i = 0 ;

	rf_message.HEADER = 0x5C;
	rf_message.TYPE = 0x10;

	memcpy(rf_message.SIGN,nrf_communication.receive_buf+5,4);

	/* 获取消息的有效长度 */
	rf_message.LEN = Message->receive_buf[14];

	for (i=0;i<rf_message.LEN;i++)
	{
		rf_message.DATA[i]=Message->receive_buf[i+15];
	}

	rf_message.XOR =  XOR_Cal((uint8_t *)(&(rf_message.TYPE)), i+6);
	rf_message.END = 0xCA;

	/* 存入缓存 */
	if(BUFFERFULL != buffer_get_buffer_status(SEND_RINGBUFFER))
	{
		serial_ringbuffer_write_data(SEND_RINGBUFFER,&rf_message);
	}
}



/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
#if defined(__CC_ARM) /*------------------RealView Compiler -----------------*/
__asm void GenerateSystemReset(void)
{
//	MOV R0, #1         	//;
//	MSR FAULTMASK, R0  	//; 清除FAULTMASK 禁止一切中断产生
//	LDR R0, =0xE000ED0C //;
//	LDR R1, =0x05Fa0004 //;
//	STR R1, [R0]       	//; 系统软件复位
//deadloop
//    B deadloop        //; 死循环使程序运行不到下面的代码
}
#elif (defined(__ICCARM__)) /*------------------ ICC Compiler -------------------*/
//#pragma diag_suppress=Pe940
void GenerateSystemReset(void)
{
	__ASM("MOV R0, #1");
	__ASM("MSR FAULTMASK, R0");
	SCB->AIRCR = 0x05FA0004;
	for (;;);
}
#endif

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	while(1)
	{
		GenerateSystemReset();
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();

	time_inc();

	clicker_send_data_time_set1( 1, 2,1200);
	clicker_send_data_time_set1( 4, 5,1200);
	clicker_send_data_time_set1( 7, 8,1200);

	if(get_rf_retransmit_status() == 1)
	{
		rf_retransmit_timecnt++;
		if(rf_retransmit_timecnt == 1200)
		{
			rf_retransmit_set_status(3);
			rf_retransmit_timecnt = 0;
		}
	}

	if(rf_systick_status == 3)
	{
		rf_tx_time_cnt++;

		/* 5S 产生心跳包 同时计数器清零 */
		if(rf_tx_time_cnt >= 5000)
		{
			rf_change_systick_status(4);
			rf_tx_time_cnt = 0;
		}
	}

	if(rf_systick_status == 1)
	{
		rf_tx_timeout_cnt++;
		/* 1S 结束在线状态统计，清零超时基数器 */
		if(rf_tx_timeout_cnt >= 1000)
		{
			rf_change_systick_status(2);
			change_clicker_send_data_status(2);
			rf_tx_timeout_cnt = 0;
		}
	}

	if(flag_uart_rxing)												//串口接收超时计数器
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>5)										//5msc超时后重新开始接收
		{
			uart_clear_message(&uart_irq_revice_massage);
			flag_uart_rxing = false;
			uart_status = UartHEADER;
		}
	}

	if(timer_1ms++ > 1000)
	{
#ifdef ENABLE_WATCHDOG
		IWDG_ReloadCounter();													//定时喂狗
#endif //ENABLE_WATCHDOG
		timer_1ms = 0;
		time.second++;
		if(time.second >= 60)
		{
			time.second = 0;
			time.minute++;
			if(time.minute >= 60)
			{
				time.minute = 0;
				time.hour++;
				if(time.hour >= 24)
				{
					time.hour = 0;
				}
			}
		}
	}

	if(time_for_buzzer_on > 1)									//蜂鸣器开关延时时间
	{
		time_for_buzzer_on--;
	}

	if((time_for_buzzer_on == 0)&&(time_for_buzzer_off > 0))	//蜂鸣器响延时
	{
		time_for_buzzer_off--;
	}

	if(delay_nms)
	{
		delay_nms --;
	}


}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void USART1pos_IRQHandler(void)
{
	uint8_t uart_temp = 0;

	if(USART_GetITStatus(USART1pos, USART_IT_PE) != RESET)
	{
		// Enable SC_USART RXNE Interrupt (until receiving the corrupted byte)
		USART_ITConfig(USART1pos, USART_IT_RXNE, ENABLE);
		// Flush the SC_USART DR register
		USART_ReceiveData(USART1pos);
	}

	if(USART_GetITStatus(USART1pos, USART_IT_RXNE) != RESET)
	{
	  uart_temp = USART_ReceiveData(USART1pos);

		/* store it to uart_irq_revice_massage */
		uart_revice_data_state_mechine( uart_temp );

		uart_rx_timeout = 0;
	}


	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  {
    uart_send_data_state_machine( );
	}
}

bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position);

uint8_t irq_flag;

void RFIRQ_EXTI_IRQHandler(void)
{
	bool    Is_whitelist_uid = OPERATION_ERR;
	uint8_t uidpos = 0;

	if(EXTI_GetITStatus(EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);

		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_communication.receive_len, nrf_communication.receive_buf);		//读取数据

//	printf("start = %2x ",*(nrf_communication.receive_buf));
//	printf("len = %2x ",*(nrf_communication.receive_buf+14));
//	printf("xor = %2x ",*(nrf_communication.receive_buf + *(nrf_communication.receive_buf+14)+15));
//	printf("calxor = %2x \r\n",XOR_Cal(nrf_communication.receive_buf+1, *(nrf_communication.receive_buf+14)+14));

		/* 进行 crc 校验 */
		if(*(nrf_communication.receive_buf + *(nrf_communication.receive_buf+14)+15) ==
			  XOR_Cal(nrf_communication.receive_buf+1, *(nrf_communication.receive_buf+14)+14))
		{
			/* 白名单开启，检测是否为白名单的内容 */
			Is_whitelist_uid = search_uid_in_white_list(nrf_communication.receive_buf+5,&uidpos);

			if(clickers[uidpos].use == 0)
			{
				memcpy(clickers[uidpos].uid, nrf_communication.receive_buf+5, 4);
				clickers[uidpos].use = 1;
				clickers[uidpos].first = 1;
			}
			else
			{
				clickers[uidpos].first = 0;
			}

			/* 统计答题器的接受情况 */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				uint8_t systick_current_status = 0;

				/* 获取当前的systick的状态 */
				systick_current_status = rf_get_systick_status();

				/* 获取发送状态 */
				if(systick_current_status == 1)
				{
					set_index_of_white_list_pos(1,uidpos);
				}

				/* 统计发送状态 */
				clicker_send_data_statistics( get_clicker_send_data_status(), uidpos );

				if(1 == get_rf_retransmit_status())
				{
					if(nrf_communication.dtq_uid[0] == retransmit_uid[0] &&
						 nrf_communication.dtq_uid[1] == retransmit_uid[1]
						)
					{
						rf_retransmit_set_status(2);

					}
				}
			}

			/* 白名单是否关闭 */
			if(white_on_off == OFF)
			{
				/* 白名单关闭数据透传 */
				Is_whitelist_uid = OPERATION_SUCCESS;
			}

			/* 白名单匹配 */
			if(Is_whitelist_uid == OPERATION_SUCCESS)
			{
				/* get uid */
				memcpy(sign_buffer   ,nrf_communication.receive_buf+5 ,4);
				memcpy(nrf_communication.dtq_uid,nrf_communication.receive_buf+5 ,4);

				/* 收到的是ACK */
				if(nrf_communication.receive_buf[11] == NRF_DATA_IS_ACK)
				{
					/* 返回ACK的包号和上次发送的是否相同 */
					if(nrf_communication.receive_buf[10] == jsq_to_dtq_packnum)
					{
//						printf("[ACK] uid:%02x%02x%02x%02x, ",
//							*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
//							*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
//						printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
//							(uint8_t)*(nrf_communication.receive_buf+10));
					}
				}
				else//收到的是有效数据
				{
//					printf("[DATA] uid:%02x%02x%02x%02x, ",
//						*(nrf_communication.receive_buf+5),*(nrf_communication.receive_buf+6),
//						*(nrf_communication.receive_buf+7),*(nrf_communication.receive_buf+8));
//					printf("seq:%2x, pac:%2x\r\n",(uint8_t)*(nrf_communication.receive_buf+9),
//						(uint8_t)*(nrf_communication.receive_buf+10));

					/* 重复接收的数据，返回包号和上次一样的ACK */
					if(clickers[uidpos].prepacknum != nrf_communication.receive_buf[10])
					{
						/* 统计丢包 */
						if( clickers[uidpos].use == 1 )
						{
							//float lostrate = 0.0;

							if(clickers[uidpos].first == 0)
							{
								if( nrf_communication.receive_buf[10] > clickers[uidpos].prepacknum )
									clickers[uidpos].lost_package_num += nrf_communication.receive_buf[10] - clickers[uidpos].prepacknum -1 ;

								if( nrf_communication.receive_buf[10] < clickers[uidpos].prepacknum )
									clickers[uidpos].lost_package_num += nrf_communication.receive_buf[10] + 255 - clickers[uidpos].prepacknum ;
							}
							else
							{
								clickers[uidpos].lost_package_num = 0;
							}

							/* 统计收到包数 */
							clickers[uidpos].revice_package_num++;
//						printf("clickers : %02x%02x%02x%02x, pre:%2x, cur:%2x revice = %08x, lost = %08x, \r\n",
//						clickers[uidpos].uid[0],
//						clickers[uidpos].uid[1],
//						clickers[uidpos].uid[2],
//						clickers[uidpos].uid[3],
//						clickers[uidpos].prepacknum,
//						nrf_communication.receive_buf[10],
//						clickers[uidpos].revice_package_num,
//						clickers[uidpos].lost_package_num
//						);
							clickers[uidpos].prepacknum = nrf_communication.receive_buf[10];
						}
						/* 有效数据复制到缓存 */
						//rf_move_data_to_buffer(&nrf_communication);
						/* 更新接收数据帧号与包号 */
						dtq_to_jsq_sequence = nrf_communication.receive_buf[9];
						dtq_to_jsq_packnum = nrf_communication.receive_buf[10];
						/* 回复ACK */
						my_nrf_transmit_start(&dtq_to_jsq_sequence,0,NRF_DATA_IS_ACK,0);
						/* 用户接收到数据处理函数 */
						my_nrf_receive_success_handler();
					}
				}
			}
		}
	}
	ledToggle(LBLUE);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
