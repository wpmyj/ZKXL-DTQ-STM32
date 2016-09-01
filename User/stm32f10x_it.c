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

Uart_MessageTypeDef uart_irq_revice_massage;
Uart_TxDataTypeDef  uart232_var;

static uint8_t  uart_status     = UartHEADER;
static uint8_t	uart_rx_cnt     = 0;			  			//中断串口接收计数器
static uint8_t  temp_sign_len   = 0;
static uint32_t uart_rx_timeout = 0;
static bool     flag_uart_rxing = false;

	

bool 		flag_tx_ok[2];		  				//中断串口接收完成标志
bool 		flag_txing[2];		   				//中断串口正在发送标志
uint8_t uart_tx_length[2];					//中断串口发送长度	
uint8_t uart_tx_cnt;			  			//中断串口发送计数器
uint8_t	uart_tx_buf[2][UART_NBUF + 5];		//中断串口发送缓存 ,两组缓冲区，轮流发送 

extern nrf_communication_t			nrf_communication;
extern uint8_t 					dtq_to_jsq_sequence;
extern uint8_t 					jsq_to_dtq_sequence;

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


void uart_clear_rx_message( Uart_MessageTypeDef *Message )
{
	uint8_t i;
	uint8_t *pdata = (uint8_t*)(Message);
	
	for(i=0;i<=PACKETSIZE;i++)
	{
		*pdata = 0;
		pdata++;
	}
	flag_uart_rxing = false;
}


/*********************************************************************************
* 功	能：void hal_uart_clr_tx(uint8_t tx_index)
* 输    入: NULL
* 返	回：NULL
* 备	注：清零发送相关变量
*********************************************************************************/
void hal_uart_clr_tx(uint8_t tx_index)
{	
	uart232_var.flag_txing[tx_index] = false;		//发送结束
	memset(uart232_var.uart_tx_buf[tx_index], 0x00,  uart232_var.uart_tx_cnt);
	uart232_var.uart_tx_cnt = 0x00;
}
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
	
	if(flag_uart_rxing)												//串口接收超时计数器
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>5)										//5msc超时后重新开始接收
		{
			uart_clear_rx_message(&uart_irq_revice_massage);
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
//		printf("[RTC]: RTC time - %02d:%02d:%02d\r\n", time.hour, time.minute, time.second);
//		SPI_Init_NRF2();
//		nrf24InitConf_NRF2();
//		ledToggle(LGREEN);
//		printf("nrf2 reset\r\n");
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

		switch(uart_status)
		{
			case UartHEADER:
				if(UART_SOF == uart_temp)		//如果命令头为0x5C则开始接收，否则不接收							
				{
					uart_irq_revice_massage.HEADER = uart_temp;		
					uart_status =  UartTYPE;
					flag_uart_rxing = true;
				}
				break;
				
			case UartTYPE:
				{
					uart_irq_revice_massage.TYPE = uart_temp;
					uart_status = UartSIGN; 
					temp_sign_len = 0;
				}
				break;	
				
			case UartSIGN:
					{
					    uart_irq_revice_massage.SIGN[temp_sign_len++] = uart_temp;
							if( temp_sign_len == 4 )
						  { 
							    uart_status = UartLEN;
						  }
						  //memcpy(&sign_buffer[0],&uart_irq_revice_massage.SIGN[0] , 4);
					}
					break;
				 
			case UartLEN:
					{
						uart_irq_revice_massage.LEN = uart_temp;
						
						/*  若数据长度大于 236 */
						if(uart_irq_revice_massage.LEN > UART_NBUF)  					
						{
							uart_status =  UartHEADER;
							/* 清除 uart_irq_revice_massage 接收信息 */
							uart_clear_rx_message(&uart_irq_revice_massage);    
						}
						else if(uart_irq_revice_massage.LEN > 0)		   				//  DATA不为空
						{
							uart_status = UartDATA;
							uart_rx_cnt = 0;
						}
						else												//  DATA为空
						{
							uart_status = UartXOR;
						}
					}
					break;
					
			case UartDATA:
					{
						  uart_irq_revice_massage.DATA[uart_rx_cnt++] = uart_temp;
						
						  if(uart_rx_cnt == uart_irq_revice_massage.LEN)							//数据接收完成
							    uart_status = UartXOR;
					}
					break;
					
			case UartXOR:
					{
						  uart_irq_revice_massage.XOR = uart_temp;
						  uart_status = UartEND;
					}
				  break;
					
			case UartEND:
					{
						if(UART_EOF == uart_temp)
						{
							uint8_t UartMessageXor = XOR_Cal((uint8_t *)(&uart_irq_revice_massage.TYPE), 
							            uart_irq_revice_massage.LEN + 6 );
							uart_irq_revice_massage.END = uart_temp;
							
							if( uart_irq_revice_massage.XOR == UartMessageXor)
							{   //若校验通过，则接收数据OK可用
								  serial_ringbuffer_write_data(&uart_irq_revice_massage);
								  uart_status = UartHEADER;
								  uart_clear_rx_message(&uart_irq_revice_massage);
							}
							else														    
							{
								uart_clear_rx_message(&uart_irq_revice_massage);                       
							}
						}
						else
						{
							uart_status = UartHEADER;
							uart_clear_rx_message(&uart_irq_revice_massage);                                                 
						}
					}
				  break;
					
			default:
				break;
		}
		uart_rx_timeout = 0;
	}
   
	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  	{   
	    // Write one byte to the transmit data register 	    
	    if(uart232_var.uart_tx_length[uart_tx_i])			//长度不为0继续发送
	    {
			USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
			uart232_var.uart_tx_length[uart_tx_i]--;
	    }
	    else
		{
			uart232_var.flag_tx_ok[uart_tx_i] = true;		//发送完成
			hal_uart_clr_tx(uart_tx_i);						//清空当前发送缓冲区
			
			if(uart232_var.flag_txing[0])					//如果第一组缓冲区需要发送
			{
				uart_tx_i = 0;
				USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
				uart232_var.uart_tx_length[uart_tx_i]--;
			}
			else if(uart232_var.flag_txing[1])				//如果第二组缓冲区需要发送
			{
				uart_tx_i = 1;
				USART_SendData(USART1pos, uart232_var.uart_tx_buf[uart_tx_i][uart232_var.uart_tx_cnt++]);
				uart232_var.uart_tx_length[uart_tx_i]--;
			}
			else											//都不需要发送
			{	
				USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
			}
		}
	}
}

bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position);

uint8_t blank_packet[]={0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCA };			//收到答题器空包包，回一个空包
uint8_t response[] =   {0x5A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xCA };		//收到答题器数据包，回确认包
uint8_t irq_flag;
uint8_t ack_buff[] = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};

void RFIRQ_EXTI_IRQHandler(void)
{
	bool    Is_whitelist_uid = false;
	uint8_t uidpos = 0;
	
	if(EXTI_GetITStatus(EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);
		
		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_communication.receive_len, nrf_communication.receive_buf);		//读取数据
		
		printf("UID = %2x%2x%2x%2x \r\n",
		       *(nrf_communication.receive_buf+1),*(nrf_communication.receive_buf+2),
		       *(nrf_communication.receive_buf+2),*(nrf_communication.receive_buf+3));
		
		//Is_whitelist_uid = search_uid_in_white_list(nrf_communication.receive_buf+1,&uidpos);
		Is_whitelist_uid = true;
		
		if(Is_whitelist_uid)			//白名单匹配
		{		
			if(nrf_communication.receive_buf[5] == NRF_DATA_IS_ACK)						//收到的是ACK
			{
				if(nrf_communication.receive_buf[0] == jsq_to_dtq_sequence)				//返回ACK的包号和上次发送的是否相同
				{
					nrf_communication.transmit_ok_flag = true;
					//irq_debug("irq_debug，有效ACK,包号相同: %02X  \n",nrf_communication.receive_buf[0]);	
//					for(i = 0; i < nrf_communication.receive_len; i++)
//					{
//						irq_debug("%02X ", nrf_communication.receive_buf[i]);
//					}irq_debug("\r\n");

				}
				else
				{
					//irq_debug("irq_debug，无效ACK,包号不同: %02X \n",rf_var.rx_buf[0]);
				}
			}
			else														//收到的是有效数据
			{
				if(dtq_to_jsq_sequence == nrf_communication.receive_buf[0])	//重复接收的数据，返回包号和上次一样的ACK
				{
					//irq_debug("irq_debug，重复收到数据,包号: %02X  \n",nrf_communication.receive_buf[0]);
//					dtq_to_jsq_sequence = nrf_communication.receive_buf[0];			//更新接收包号		
					my_nrf_transmit_start(ack_buff,6,NRF_DATA_IS_ACK);
					
				}
				else													//有效数据，返回ACK
				{
					//irq_debug("irq_debug，首次收到数据,包号: %02X  \n",nrf_communication.receive_buf[0]);
//					for(i = 0; i < nrf_communication.receive_len; i++)
//					{
//						irq_debug("%02X ", nrf_communication.receive_buf[i]);
//					}irq_debug("\r\n");
					rf_var.rx_len = nrf_communication.receive_buf[6];
					memcpy(rf_var.rx_buf, nrf_communication.receive_buf+10, rf_var.rx_len);	//有效数据复制到rf_var.rx_buf
					rf_var.flag_rx_ok = true;
					dtq_to_jsq_sequence = nrf_communication.receive_buf[0];				//更新接收包号
					my_nrf_transmit_start(ack_buff,6,NRF_DATA_IS_ACK);		//回复ACK
					my_nrf_receive_success_handler();						//用户接收到数据处理函数
				}	
			}
		}
		else //白名单不匹配，滤掉
		{
		  printf("Update:The Clickers not register! \r\n ");
		}
	}
	ledToggle(LBLUE);
}


//软件模拟ACK通信，处理定时器
void TIM3_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		//irq_debug("定时器3中断  ");
		
		nrf_communication.number_of_retransmits++;
		if( true == nrf_communication.transmit_ing_flag )		//正在传输
		{
			if( true == nrf_communication.transmit_ok_flag ) 	//收到有效ACK,发送成功
			{
				my_nrf_transmit_tx_success_handler();			//用户发送成功处理函数		
				nrf_communication.transmit_ing_flag = false;	
				nrf_communication.transmit_ok_flag = true;			
				nrf_communication.number_of_retransmits = 0;	
				TIM_Cmd(TIM3, DISABLE); 						//停止定时器
				//irq_debug("irq_debug:transmit succeed,sequence:	%02X \r\n",jsq_to_dtq_sequence);
			}
			else if( nrf_communication.number_of_retransmits > NRF_MAX_NUMBER_OF_RETRANSMITS )	//达到最大重发次数，发送失败
			{
				my_nrf_transmit_tx_failed_handler();			//用户发送失败处理函数
				nrf_communication.transmit_ing_flag = false;	
				nrf_communication.transmit_ok_flag = false;			
				nrf_communication.number_of_retransmits = 0;	
				TIM_Cmd(TIM3, DISABLE); 						
				//irq_debug("irq_debug:transmit  failure,sequence: %02X \r\n",jsq_to_dtq_sequence);
			}
			else
			{
				uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len);
			}
		}
		else	//定时器第一次发送数据							
		{
			nrf_communication.transmit_ing_flag = true;	
			nrf_communication.transmit_ok_flag = false;		
			uesb_nrf_write_tx_payload(nrf_communication.transmit_buf,nrf_communication.transmit_len);
		}
	}
}
/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
