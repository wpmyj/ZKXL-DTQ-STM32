/**
  ******************************************************************************
  * @file   	board.c
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board init
  ******************************************************************************
  */
#include "main.h"
#include "board.h"

/*******************************************************************************
* Function Name   : void uart_send_char( uint8_t ch )
* Description     : Retargets the C library printf function
* Input           : None
* Output          : None
*******************************************************************************/
void uart_send_char( uint8_t ch )
{
	/* Write a character to the USART */
	USART_SendData(USART1pos, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_TXE) == SET))
	{
	}
}

/*******************************************************************************
* Function Name   : int fputc(int ch, FILE *f)
* Description     : Retargets the C library printf function to the printf
* Input           : None
* Output          : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	/* Write a character to the USART */
	USART_SendData(USART1pos, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_TXE) == SET))
	{
	}

	return ch;
}

/*******************************************************************************
* Function Name   : int fgetc(FILE *f)
* Description     : Retargets the C library printf function to the fgetc
* Input           : None
* Output          : None
*******************************************************************************/
int fgetc(FILE *f)
{
	/* Loop until received a char */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_RXNE) == SET))
	{
	}

	/* Read a character from the USART and RETURN */
	return (USART_ReceiveData(USART1pos));
}

/*******************************************************************************
* Function Name   : uint8_t XOR_Cal(uint8_t *data, uint16_t length)
* Description     : 异或计算函数
* Input           : data	: 数据指针
*        		    length	：数据长度
* Output          : None
*******************************************************************************/
uint8_t XOR_Cal(uint8_t *data, uint16_t length)
{
	uint8_t temp_xor;
	uint16_t i;

	temp_xor = *data;
	for(i = 1;i < length; i++)
	{
		temp_xor = temp_xor ^ *(data+i);
	}
	return temp_xor;
}

/**************************************END OF FILE****************************/
