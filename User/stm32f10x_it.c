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
	if(uart232_var.flag_uart_rxing)												//串口接收超时计数器
	{
		uart232_var.uart_rx_timeout++;
		if(uart232_var.uart_rx_timeout>5)										//5msc超时后重新开始接收
		{
			hal_uart_clr_rx();
			uart232_var.uart_status = UartHEADER;
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
	// If the SC_USART detects a parity error 
	if(USART_GetITStatus(USART1pos, USART_IT_PE) != RESET)
	{
		// Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) 
		USART_ITConfig(USART1pos, USART_IT_RXNE, ENABLE);
		// Flush the SC_USART DR register 
		USART_ReceiveData(USART1pos);
	}
  
	if(USART_GetITStatus(USART1pos, USART_IT_RXNE) != RESET)
	{
	    uart232_var.uart_temp = USART_ReceiveData(USART1pos);
		switch(uart232_var.uart_status)
		{
			case UartOK:									//开始接收
				if(uart232_var.flag_uart_rx_ok || uart232_var.flag_txing[uart_tx_i])
				{	break;	}	 							//若当前命令没被取走或没有发送完成，将不再收取数据
			case UartHEADER:
				if(UART_SOF == uart232_var.uart_temp)		//如果命令头为0x5C则开始接收，否则不接收							
				{
					uart232_var.flag_uart_rxing = 1;
					uart232_var.HEADER = uart232_var.uart_temp;
					uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
					uart232_var.uart_rx_timeout = 0;			
					uart232_var.uart_status =  UartTYPE;
				}
				break;
			case UartTYPE:
				uart232_var.TYPE = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				uart232_var.uart_status = UartSIGN; 
				break;	
			case UartSIGN:
				uart232_var.SIGN[uart232_var.temp_sign_len++] = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
			    uart232_var.uart_rx_timeout = 0;
			    if(uart232_var.temp_sign_len==4)
				 { 
				   uart232_var.uart_status = UartLEN;
				 }
				 memcpy(&sign_buffer[0],&uart232_var.SIGN[0] , 4);
				break;
			case UartLEN:
				uart232_var.LEN = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				if(uart232_var.LEN > UART_NBUF)  					//  若数据长度大于32
				{
					uart232_var.flag_uart_rx_length_err = 1;		//  命令数据长度不对
					uart232_var.uart_status =  UartHEADER;
				}
				else if(uart232_var.LEN > 0)		   				//  DATA不为空
				{
					uart232_var.uart_status = UartDATA;
				}
				else												//  DATA为空
				{
					uart232_var.uart_status = UartXOR;
				}
				uart232_var.uart_rx_timeout = 0;
				break;
			case UartDATA:
				uart232_var.DATA[uart232_var.temp_data_len++] = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				if(uart232_var.temp_data_len == uart232_var.LEN)							//数据接收完成
					uart232_var.uart_status = UartXOR;
				break;
			case UartXOR:
				uart232_var.XOR = uart232_var.uart_temp;
				uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
				uart232_var.uart_rx_timeout = 0;
				uart232_var.uart_status = UartEND;
				break;
			case UartEND:
				if(UART_EOF == uart232_var.uart_temp)
				{
					uart232_var.END = uart232_var.uart_temp;
					uart232_var.uart_rx_buf[uart232_var.uart_rx_cnt++] = uart232_var.uart_temp;
					if((uart232_var.uart_rx_cnt == (uart232_var.LEN + 9))||(uart232_var.uart_rx_cnt == (uart232_var.LEN + 14)))	//若接收数据等于需接收长度（有疑问）
					{
						if( uart232_var.XOR != XOR_Cal(&uart232_var.uart_rx_buf[1], uart232_var.uart_rx_cnt - 3))//  xor校验未通过
						{
							uart232_var.flag_uart_rx_xor_err = 1;
							uart232_var.uart_status = UartOK;
						}
						else														//若校验通过，则接收数据OK可用
						{
							uart232_var.flag_uart_rx_ok = 1;                       //中断串口接收完成标志
							uart232_var.uart_status = UartOK;                       
						}
					}
					else															//若接收数据不等于需接收长度，清除之前接收buf	
					{
						uart232_var.flag_uart_rx_length_err = 1;
						uart232_var.uart_status = UartOK;
					}
				}
				else
				{
					uart232_var.uart_status = UartHEADER;
					hal_uart_clr_rx();                                               //清除所有接收相关的变量
				}
				uart232_var.flag_uart_rxing = 0;                                     //中断串口正在接收标志
				uart232_var.uart_rx_timeout = 0;	                                 //中断串口接收超时计数器
				break;
			default:
				break;
		}
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
void RFIRQ_EXTI_IRQHandler(void)
{
	uint8_t uid_p;
	EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);
	if(hal_nrf_get_irq_flags() & (1 << RX_DR))
	{
		rf_var.rx_len = hal_nrf_read_multibyte_reg(HAL_NRF_RX_PLOAD, rf_var.rx_buf) & 0xFF;
	}
	hal_nrf_get_clear_irq_flags();
	hal_nrf_flush_rx();
	hal_nrf_flush_tx();

	if(white_on_off == ON)
	{
		if(search_uid_in_white_list(&rf_var.rx_buf[1], &uid_p))	
		{
			if(rf_var.rx_buf[6] != 0x00)									//如果收到的是数据包
			{
				rf_var.flag_rx_ok = true;									//收到数据包
				memcpy(response, rf_var.rx_buf, 7);
				response[9] = XOR_Cal(&response[1], 8);
				nrf24AddtoAck(response,11);									//收到数据包后返回应答
			}
			else															//如果收到的是空包
			{
				if(rf_var.flag_txing&&(white_list[uid_p].tx_flag == false))	//有数据下发且未曾下发过
				{
					nrf24AddtoAck(rf_var.tx_buf,rf_var.tx_len);				//有需要下发的数据包，发送数据包
					white_list[uid_p].tx_flag = true;
					rf_var.flag_tx_ok = true;
				}
				else
					nrf24AddtoAck(rf_var.rx_buf,rf_var.rx_len);				//无下发数据包，返回空包数据
			}
		}
		else
		{
			memset(rf_var.rx_buf, 0x00, rf_var.rx_len);						//不在白名单，过滤掉
			rf_var.rx_len = 0x00;
		}
	}
	else   
	{
     if(rf_var.rx_buf[6] != 0x00)									//如果收到的是数据包
	 {
	  rf_var.flag_rx_ok = true;									//收到数据包
	  memcpy(response, rf_var.rx_buf, 7);
	  response[9] = XOR_Cal(&response[1], 8);
	  nrf24AddtoAck(response,11);									//收到数据包后返回应答
	 }		
	else
	{	
	 memcpy(blank_packet, rf_var.rx_buf, 6);
	 blank_packet[8] = XOR_Cal(&blank_packet[1], 7);
	 nrf24AddtoAck(blank_packet,10);		//返回空数据包
	}
   }
	ledToggle(LGREEN);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
