#include "my_ringbuffer2.h"

uint8_t p_read2 = 0;
uint8_t p_write2 = 0;
uint8_t buff_status2 = BUFF_EMPTY2;

dynamic_store_t2		store_buff2[MAX_STORE_NUM2]; 	

void read_ringbuf_use_num2(void)
{
	uint8_t ringbuf_use_num = 0;
	
	if((p_read2 == p_write2) || (p_read2 < p_write2))
	{
		ringbuf_use_num = p_write2 - p_read2;
	}
	else
	{
		ringbuf_use_num = MAX_STORE_NUM2 - p_read2 + p_write2;
	}
	printf("%d \r\n",ringbuf_use_num);
}

//零时使用的存储方案
uint8_t get_ringbuf_status2(void)
{
	return buff_status2;
}

void ringbuf_write_data2(uint8_t *buff, uint8_t buff_len)
{
	store_buff2[p_write2].length = buff_len;
	memcpy(store_buff2[p_write2].data, buff, buff_len);
	
	if(p_write2 < (MAX_STORE_NUM2 - 1))
		p_write2++;
	else
		p_write2 = 0;
	
	if(p_write2 < (MAX_STORE_NUM2 - 1))
	{
		if((p_write2+1) == p_read2)	//写的下一个是读，表示已满
			buff_status2 = BUFF_FULL2;
		else
			buff_status2 = BUFF_USEING2;
	}
	else		
	{
		if(p_read2 == 0)				//写的下一个是读，表示已满
			buff_status2 = BUFF_FULL2;
		else
			buff_status2 = BUFF_USEING2;
	}
}

void ringbuf_read_data2(uint8_t *buff, uint8_t *buff_len)
{
//	uint8_t temp_buff_len;
//	uint8_t temp_buff[BUFF_LENGTH];
	*buff_len = store_buff2[p_read2].length;
	memcpy(buff, store_buff2[p_read2].data, *buff_len);
	
	if(p_read2 < (MAX_STORE_NUM2 - 1))
		p_read2++;
	else
		p_read2 = 0;
	
	if(p_read2 == p_write2)			//写等于读，表示空
		buff_status2 = BUFF_EMPTY2;
	else
		buff_status2 = BUFF_USEING2;
	
//	printf("buff_status2 %02X \r\n",buff_status2);
}

void ringbuf_test2(void)
{
	uint8_t Uart_DATA[250];
	uint8_t i = 0,j = 0;
	uint8_t temp_len;
	while(1)
	{
		for(i=1;i<8;i++)
		{
			memset(Uart_DATA,i,i);
			if(get_ringbuf_status2() != BUFF_FULL2)
			{
				ringbuf_write_data2(Uart_DATA,i);
			}
		}
		
		for(i=1;i<8;i++)
		{
			memset(Uart_DATA,0,250);
			if(get_ringbuf_status2() != BUFF_EMPTY2)
			{
				ringbuf_read_data2(Uart_DATA,&temp_len);
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


