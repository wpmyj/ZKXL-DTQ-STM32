#include "uart.h"



void UART_init(void)//串口初始化函数
{  
#ifdef DEBUG
	
//	simple_uart_config(RTS_PIN_NUMBER_1, TX_PIN_NUMBER_1, CTS_PIN_NUMBER_1, RX_PIN_NUMBER_1, HWFC_1);
	simple_uart_config(RTS_PIN_NUMBER, TX_PIN_NUMBER, CTS_PIN_NUMBER, RX_PIN_NUMBER, HWFC);
	
#endif
}	


void my_uart_trace_dump(uint8_t * p_buffer, uint32_t len)
{
    for (uint32_t index = 0; index <  len; index++)
    {
        printf(" %02X", p_buffer[index]);
    }
    printf("\r\n");
}
		
/************************************************************** 
* 函数名  : fputc()
* 描述    : 重定义putc函数，这样可以使用printf函数从串口1打印输出
*************************************************************/
int fputc(int ch, FILE *f)
{
		/* Place your implementation of fputc here */
		/* 发送一个字节的数据 */
		simple_uart_put((uint8_t)ch);
		/* 等待发送完成 */
		return ch;
}

/************************************************************** 
* 函数名  : fgetc()
* 描述    : 重定义getc函数，这样可以使用scanf函数从串口1获取数据
*************************************************************/
int fgetc(FILE *f)
{
		/* 等待串口1输入数据 */
		/* 返回接收到的数据 */
		return (int)simple_uart_get();
}
