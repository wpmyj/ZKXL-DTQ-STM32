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
#include "app_send_data_process.h"
#include <stdio.h>  
#include <stdlib.h>  
#include "cJSON.h"
// receive part
static uint32_t uart_rx_timeout       = 0;
static bool     flag_uart_rxing       = false;
static uint8_t  uart_status           = UartSTART;
static uint8_t  uart_json_nesting_num = 0;
static uint8_t  uart_irq_revice_massage[1024];

// send part
Uart_MessageTypeDef uart_irq_send_massage;
uint8_t uart_tx_status      = 0;

/* uart global variables */
extern nrf_communication_t	nrf_data;

/* rf systick data */
uint8_t spi_status_buffer[SPI_DATA_IRQ_BUFFER_BLOCK_COUNT][18];
uint8_t spi_status_write_index = 0, spi_status_read_index = 0, spi_status_count = 0;

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
	static uint16_t	uart_rx_cnt     = 0;

	switch(uart_status)
		{
			case UartSTART:
				{
					if(UART_SOF == data)
					{
						uart_rx_cnt           = 0;
						uart_json_nesting_num = 0;
						uart_status           = UartDATA;
						uart_json_nesting_num++;
						uart_irq_revice_massage[uart_rx_cnt++] = data ;
						flag_uart_rxing = true;
					}
				}
				break;

			case UartDATA:
				{
					uart_irq_revice_massage[uart_rx_cnt++] = data ;
					if(UART_SOF == data)
					{
						uart_json_nesting_num++;
					}
					if(UART_EOF == data)
					{
						uart_json_nesting_num--;
						if(uart_json_nesting_num == 0)
						{
							cJSON *json ;
							json = cJSON_Parse((char *)uart_irq_revice_massage);  
							if (!json)  
							{  
									printf("Error before: [%s]\n",cJSON_GetErrorPtr());  
							} 
							else
							{
								printf("\r\nParse:%s: %s \r\n",
								cJSON_GetObjectItem(json, "fun")->string,
								cJSON_GetObjectItem(json, "fun")->valuestring);

								/* start */
								if(strncmp(cJSON_GetObjectItem(json, "fun")->valuestring,"start",5) == 0)
								{
									printf("Parse:%s:%s \r\n",
									cJSON_GetObjectItem(json, "type")->string,
									cJSON_GetObjectItem(json, "type")->valuestring);
									printf("Parse:%s:%s \r\n",
									cJSON_GetObjectItem(json, "num")->string,
									cJSON_GetObjectItem(json, "num")->valuestring);
									printf("Parse:%s:%s \r\n",
									cJSON_GetObjectItem(json, "time")->string,
									cJSON_GetObjectItem(json, "time")->valuestring);
								}

								/* get_device_no */
								if(strncmp(cJSON_GetObjectItem(json, "get_device_no")->valuestring,"get_device_no",5) == 0)
								{
									
								}
								
								/* get_device_no */
								if(strncmp(cJSON_GetObjectItem(json, "getlist")->valuestring,"getlist",5) == 0)
								{
									
								}
							}
							
							if(json)
								cJSON_Delete(json);
							
							memset(uart_irq_revice_massage,0,uart_rx_cnt);
							uart_rx_cnt     = 0;
							uart_status     = UartSTART;
							flag_uart_rxing = false;
						}
					}
				}
				break;

			default:
				break;
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
	MOV R0, #1         	//;
	MSR FAULTMASK, R0  	//; 清除FAULTMASK 禁止一切中断产生
	LDR R0, =0xE000ED0C //;
	LDR R1, =0x05Fa0004 //;
	STR R1, [R0]       	//; 系统软件复位
deadloop
    B deadloop        //; 死循环使程序运行不到下面的代码
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
	Timer_list_handler();

	TimingDelay_Decrement();

	if(flag_uart_rxing)												//串口接收超时计数器
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>5)										//5ms超时后重新开始接收
		{
			flag_uart_rxing = false;
			uart_status = UartSTART;
		}
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
}

uint8_t irq_flag;
void NRF1_RFIRQ_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(NRF1_EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(NRF1_EXTI_LINE_RFIRQ);

		/* 读取数据 */
		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_data.rlen, nrf_data.rbuf);
		/* 进行 UID 校验,判断是否发送给自己的数据 */
		if( *(nrf_data.rbuf+1) == nrf_data.jsq_uid[0] &&
			  *(nrf_data.rbuf+2) == nrf_data.jsq_uid[1] &&
				*(nrf_data.rbuf+3) == nrf_data.jsq_uid[2] &&
				*(nrf_data.rbuf+4) == nrf_data.jsq_uid[3])
		{
			if(BUFFERFULL != buffer_get_buffer_status(SPI_IRQ_BUFFER))
			{
				uint8_t send_data_status = get_clicker_send_data_status();
				spi_write_data_to_buffer(SPI_IRQ_BUFFER,nrf_data.rbuf, send_data_status);
			}
			else
			{
				DEBUG_BUFFER_ACK_LOG("spi irq buffer full \r\n");
			}
		}
	}
	ledToggle(LBLUE);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
