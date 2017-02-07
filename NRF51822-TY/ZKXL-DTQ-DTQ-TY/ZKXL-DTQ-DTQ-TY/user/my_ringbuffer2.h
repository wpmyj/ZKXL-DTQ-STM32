#ifndef __MY_RINGBUFFER2_H
#define	__MY_RINGBUFFER2_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "nrf_delay.h"

#define		MAX_STORE_NUM2	(3)
#define		BUFF_LENGTH2		(250)

#define BUFF_EMPTY2          (1)
#define BUFF_USEING2         (2)
#define BUFF_FULL2           (3)

typedef struct
{
	uint8_t		length;
	uint8_t		data[BUFF_LENGTH2];		

}dynamic_store_t2;

uint8_t get_ringbuf_status2(void);
void ringbuf_write_data2(uint8_t *buff, uint8_t buff_len);
void ringbuf_read_data2(uint8_t *buff, uint8_t *buff_len);
void ringbuf_test2(void);
void read_ringbuf_use_num2(void);


#endif 

