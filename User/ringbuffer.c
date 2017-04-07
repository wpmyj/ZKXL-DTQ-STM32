#include "ringbuffer.h"

uint8_t p_read = 0;
uint8_t p_write = 0;
uint8_t buff_status = BUFF_EMPTY;

dynamic_store_t		store_buff[MAX_STORE_NUM]; 	



uint8_t get_ringbuf_status(void)
{
	return buff_status;
}

void ringbuf_write_data(uint8_t *buff, uint8_t buff_len)
{
	store_buff[p_write].length = buff_len;
	memcpy(store_buff[p_write].data, buff, buff_len);
	
	if(p_write < (MAX_STORE_NUM - 1))
		p_write++;
	else
		p_write = 0;
	
	if(p_write < (MAX_STORE_NUM - 1))
	{
		if((p_write+1) == p_read)	//д����һ���Ƕ�����ʾ����
			buff_status = BUFF_FULL;
		else
			buff_status = BUFF_USEING;
	}
	else		
	{
		if(p_read == 0)				//д����һ���Ƕ�����ʾ����
			buff_status = BUFF_FULL;
		else
			buff_status = BUFF_USEING;
	}
}

void ringbuf_read_data(uint8_t *buff, uint8_t *buff_len)
{
//	uint8_t temp_buff_len;
//	uint8_t temp_buff[BUFF_LENGTH];
	*buff_len = store_buff[p_read].length;
	memcpy(buff, store_buff[p_read].data, *buff_len);
	
	if(p_read < (MAX_STORE_NUM - 1))
		p_read++;
	else
		p_read = 0;
	
	if(p_read == p_write)			//д���ڶ�����ʾ��
		buff_status = BUFF_EMPTY;
	else
		buff_status = BUFF_USEING;
}

void ringbuf_test(void)
{
	uint8_t Uart_DATA[250];
	uint8_t i = 0,j = 0;
	uint8_t temp_len;
	while(1)
	{
		for(i=1;i<10;i++)
		{
			memset(Uart_DATA,i,i);
			if(get_ringbuf_status() != BUFF_FULL)
			{
				ringbuf_write_data(Uart_DATA,i);
			}
		}
		
		for(i=1;i<10;i++)
		{
			memset(Uart_DATA,0,250);
			if(get_ringbuf_status() != BUFF_EMPTY)
			{
				ringbuf_read_data(Uart_DATA,&temp_len);
			}
			for(j=0;j<temp_len;j++)
			{
				printf("%02x",Uart_DATA[j]);
			}printf("\r\n");	
			nrf_delay_ms(100);
		}
		printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n");	
		nrf_delay_ms(3000);
	}
}


