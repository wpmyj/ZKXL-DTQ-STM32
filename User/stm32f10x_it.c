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
#include "app_serial_cmd_process.h"
/* uart global variables */
// revice part
Uart_MessageTypeDef uart_irq_revice_massage;
static uint32_t uart_rx_timeout = 0;
static uint8_t  flag_uart_rxing = 0;

// send part
Uart_MessageTypeDef uart_irq_send_massage;

/* uart global variables */
extern nrf_communication_t	nrf_data;

/* rf systick data */
uint8_t spi_status_buffer[SPI_DATA_IRQ_BUFFER_BLOCK_COUNT][20];
uint8_t spi_status_write_index = 0, spi_status_read_index = 0, spi_status_count = 0;

/******************************************************************************
  Function:uart_clear_message
  Description:
		���Message�е���Ϣ
  Input :
		Message: Э��Message��ָ��
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
		�������ݽ��պ�������ȡ��Ч���ݴ��뻺��
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_revice_data_state_mechine( uint8_t data )
{
	static uint8_t uart_rx_cnt = 0;
	static uint8_t temp_len    = 0;

	switch(uart_rev_status.get_status(&(uart_rev_status.state)))
	{
		case UartHEAD:
			{
				/* �������ͷΪ0x5C��ʼ���գ����򲻽��� */
				if(UART_SOF == data)		
				{
					uart_irq_revice_massage.HEAD = data;
					uart_rev_status.set_status(&(uart_rev_status.state),UartEVICETYPE);
					flag_uart_rxing = 1;
				}
			}
			break;

		case UartEVICETYPE:
			{
				uart_irq_revice_massage.DEVICE= data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartVERSION);
				flag_uart_rxing = 1;
			}
			break;

		case UartVERSION:
			{
				uart_irq_revice_massage.VERSION[temp_len++] = data;
				if( temp_len == 2 )
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartDSTID);
						temp_len = 0;
					}
			}
			break;

		case UartDSTID:
			{
				uart_irq_revice_massage.DSTID[temp_len++] = data;
				if( temp_len == 4 )
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartSRCID);
						temp_len = 0;
					}
			}
			break;

		case UartSRCID:
			{
					uart_irq_revice_massage.SRCID[temp_len++] = data;
					if( temp_len == 4 )
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartPACNUM);
						temp_len = 0;
					}
			}
			break;
		case UartPACNUM:
			{
				uart_irq_revice_massage.PACNUM = data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartSEQNUM);
			}
			break;
		case UartSEQNUM:
			{
				uart_irq_revice_massage.SEQNUM = data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartPACKTYPE);
			}
			break;
		case UartPACKTYPE:
			{
				uart_irq_revice_massage.PACKTYPE = data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartREVICED);
			}
			break;
		case UartREVICED:
			{
					uart_irq_revice_massage.REVICED[temp_len++] = data;
					if( temp_len == 2 )
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartCMD);
						temp_len = 0;
					}	
			}
			break;			
		case UartCMD:
			{
				uart_irq_revice_massage.CMDTYPE = data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartLEN);
			}
			break;				

		case UartLEN:
			{
				uart_irq_revice_massage.LEN[temp_len++] = data;
				if( temp_len == 2 )
				{
					uint16_t datalen = *(uint16_t *)uart_irq_revice_massage.LEN;
					/*  �����ݳ��ȴ��� 236 */
					if(datalen > UART_NBUF)
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartHEAD);
						/* ��� uart_irq_revice_massage ������Ϣ */
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = 0;
					}
					else if(uart_irq_revice_massage.LEN > 0)	//  DATA��Ϊ��
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartDATA);
						uart_rx_cnt = 0;
					}
					else
					{
						uart_rev_status.set_status(&(uart_rev_status.state),UartXOR);
					}
					temp_len = 0;
				}
			}
			break;

		case UartDATA:
			{
				uart_irq_revice_massage.DATA[uart_rx_cnt++] = data;
				/* ���ݽ������ */
				if(uart_rx_cnt == *(uint16_t *)uart_irq_revice_massage.LEN)
					uart_rev_status.set_status(&(uart_rev_status.state),UartXOR);
			}
			break;

		case UartXOR:
			{
				uart_irq_revice_massage.XOR = data;
				uart_rev_status.set_status(&(uart_rev_status.state),UartEND);
			}
			break;

		case UartEND:
			{
				if(UART_EOF == data)
				{
					uint8_t UartMessageXor = XOR_Cal(&(uart_irq_revice_massage.DEVICE),
					    *(uint16_t *)uart_irq_revice_massage.LEN + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA);
					uart_irq_revice_massage.END = data;

					if( uart_irq_revice_massage.XOR == UartMessageXor)
					{   /* ��У��ͨ�������������OK���� */
							if(BUF_FULL != buffer_get_buffer_status(UART_RBUF))
							{
								serial_ringbuffer_write_data(UART_RBUF,&uart_irq_revice_massage);
							}
							flag_uart_rxing = 0;
							uart_rev_status.set_status(&(uart_rev_status.state),UartHEAD);
							uart_clear_message(&uart_irq_revice_massage);
					}
					else
					{
						uart_clear_message(&uart_irq_revice_massage);
					}
				}
				else
				{
					uart_rev_status.set_status(&(uart_rev_status.state),UartHEAD);
					uart_clear_message(&uart_irq_revice_massage);
					flag_uart_rxing = 0;
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
		���������ͺ������ӻ�������ȡ���ݷ��͵���λ��
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

	switch(uart_sen_status.get_status(&(uart_sen_status.state)))
	{
		case 0:
			{
					if(BUF_EMPTY == buffer_get_buffer_status(UART_SBUF))
					{
						USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
						return;
					}
					else
					{
						serial_ringbuffer_read_data(UART_SBUF, &uart_irq_send_massage);
						pdata = (uint8_t *)(&uart_irq_send_massage);
						uart_sen_status.set_status(&(uart_sen_status.state),1);
						uart_tx_cnt = *(uint16_t *)uart_irq_send_massage.LEN + 
						    MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA + 1;
						uart_sen_status.set_status(&(uart_sen_status.state),1);
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
					uart_sen_status.set_status(&(uart_sen_status.state),2);
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
					uart_sen_status.set_status(&(uart_sen_status.state),0);
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
//	MOV R0, #1         	//;
//	MSR FAULTMASK, R0  	//; ���FAULTMASK ��ֹһ���жϲ���
//	LDR R0, =0xE000ED0C //;
//	LDR R1, =0x05Fa0004 //;
//	STR R1, [R0]       	//; ϵͳ�����λ
//deadloop
//    B deadloop        //; ��ѭ��ʹ�������в�������Ĵ���
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

	if(flag_uart_rxing)												//���ڽ��ճ�ʱ������
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>10)										//5ms��ʱ�����¿�ʼ����
		{
			uart_clear_message(&uart_irq_revice_massage);
			flag_uart_rxing = 0;
			uart_rev_status.set_status(&(uart_rev_status.state),UartHEAD);
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


	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  {
    uart_send_data_state_machine( );
	}
}

void NRF1_RFIRQ_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(NRF1_EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(NRF1_EXTI_LINE_RFIRQ);

		/* ��ȡ���� */
		spi_read_tx_payload(SPI1, &nrf_data.rlen, nrf_data.rbuf);
//	{
//		uint8_t i;
//		printf("irqrevicebuf:");
//		for(i=0;i<nrf_data.rlen;i++)
//		{
//			printf(" %02x",nrf_data.rbuf[i]);
//		}
//		printf("\r\n");
//	}
		/* ���� UID У��,�ж��Ƿ��͸��Լ������� */
		if( *(nrf_data.rbuf+1) == nrf_data.jsq_uid[0] &&
			  *(nrf_data.rbuf+2) == nrf_data.jsq_uid[1] &&
				*(nrf_data.rbuf+3) == nrf_data.jsq_uid[2] &&
				*(nrf_data.rbuf+4) == nrf_data.jsq_uid[3])
		{
			if(BUF_FULL != buffer_get_buffer_status(SPI_RBUF))
			{
				uint8_t send_data_status = get_clicker_send_data_status();
				spi_write_data_to_buffer(SPI_RBUF,nrf_data.rbuf, send_data_status);
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
