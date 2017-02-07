#include "ringbuf.h"

static uint8_t   buf_status = RING_BUF_EMPTY;
static uint8_t   buf_size[BUF_MAX_SIZE];
static uint8_t   *p_write = buf_size;
static uint8_t	 *p_read  = buf_size;
static uint8_t   *p_bottom = buf_size;
static uint8_t   *p_top = &buf_size[BUF_MAX_SIZE];
static uint16_t   use_ringbuf_len = 0;				//已使用的缓冲长度,用于判断buf使用状态

/******************************************************************************
  @函数:ringbuf_get_status
  @描述:返回ringbuf状态
  @输入:
  @输出:
  @调用:外部
******************************************************************************/
uint8_t ringbuf_get_status(void)
{
	return buf_status;
}

/******************************************************************************
  @函数:ringbuf_write_buf
  @描述:
  @输入:	buf：要存入缓存区的数组
			buf_len：要存取缓存区的数组长度
  @输出:
  @调用:外部
******************************************************************************/
void ringbuf_write_buf(uint8_t *buf, uint8_t *buf_len)
{
	uint8_t i;
	
	*p_write = *buf_len;
	p_write = (p_write == p_top) ? p_bottom : ++p_write;	//写指针指向下一个
	
	for(i = 0;i < *buf_len;i++)
	{
		*p_write = buf[i];
		p_write = (p_write == p_top) ? p_bottom : ++p_write;
	}
	
	use_ringbuf_len = use_ringbuf_len + *buf_len + 1;
	
	if((BUF_MAX_SIZE - use_ringbuf_len) < PACK_MAX_SIZE)	//剩余的缓冲buf小于可能到来的最大buf长度，则认为已满
		buf_status = RING_BUF_FULL;
	else
		buf_status = RING_BUF_USEING;
}

/******************************************************************************
  @函数:ringbuf_read_buf
  @描述:
  @输入:	buf：读取缓冲区对应数组
			buf_len：读取缓冲区对应数组的长度
  @输出:
  @调用:外部
******************************************************************************/
void ringbuf_read_buf(uint8_t *buf, uint8_t *buf_len)
{
	uint8_t i;
	
	*buf_len = *p_read;
	p_read = (p_read == p_top) ? p_bottom : ++p_read;		//读指针指向下一个
	
	for(i = 0;i < *buf_len;i++)
	{
		buf[i] = *p_read;
		p_read = (p_read == p_top) ? p_bottom : ++p_read;
	}
	
	use_ringbuf_len = use_ringbuf_len - *buf_len - 1;
	
	if(use_ringbuf_len == 0)
		buf_status = RING_BUF_EMPTY;
	else
		buf_status = RING_BUF_USEING;
}

/******************************************************************************
  @函数:ringbuf_test
  @描述:
  @输入:
  @输出:
  @调用:外部
******************************************************************************/
void ringbuf_test(void)
{
	uint8_t i;
	uint8_t write_len = 32;
	uint8_t write_buf[32];
	uint8_t read_len;
	uint8_t read_buf[250];

	while(1)
	{
		for(i = 1;i < 18;i++)
		{
			write_len = i;
			memset(write_buf,write_len,write_len);
			if(ringbuf_get_status() != RING_BUF_FULL)
			{
				ringbuf_write_buf(write_buf,&write_len);
			}
		}

		do
		{
			memset(read_buf,0x00,250);
			ringbuf_read_buf(read_buf,&read_len);
			
			for(i = 0;i < read_len;i++)
				printf("%02X ",read_buf[i]);
			printf("\r\n");
			
		}while((ringbuf_get_status() != RING_BUF_EMPTY));
		
		nrf_delay_ms(1500);
	}
}



