#include "uart.h"



void UART_init(void)//���ڳ�ʼ������
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
* ������  : fputc()
* ����    : �ض���putc��������������ʹ��printf�����Ӵ���1��ӡ���
*************************************************************/
int fputc(int ch, FILE *f)
{
		/* Place your implementation of fputc here */
		/* ����һ���ֽڵ����� */
		simple_uart_put((uint8_t)ch);
		/* �ȴ�������� */
		return ch;
}

/************************************************************** 
* ������  : fgetc()
* ����    : �ض���getc��������������ʹ��scanf�����Ӵ���1��ȡ����
*************************************************************/
int fgetc(FILE *f)
{
		/* �ȴ�����1�������� */
		/* ���ؽ��յ������� */
		return (int)simple_uart_get();
}
