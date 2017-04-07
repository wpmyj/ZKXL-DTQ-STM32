#ifndef __RINGBUF_H
#define	__RINGBUF_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "nrf_delay.h"

#define BUF_MAX_SIZE        		(250)		//ringbuf分配的内存空间
#define PACK_MAX_SIZE				(32)		//每次缓存可能的最大长度

#define RING_BUF_EMPTY				(1)
#define RING_BUF_USEING				(2)
#define RING_BUF_FULL				(3)


uint8_t ringbuf_get_status(void);
void ringbuf_write_buf(uint8_t *buf, uint8_t *buf_len);
void ringbuf_read_buf(uint8_t *buf, uint8_t *buf_len);
void ringbuf_test(void);


#endif 

