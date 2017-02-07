#ifndef __RINGBUFFER_H
#define	__RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "nrf_delay.h"

#define		MAX_STORE_NUM	(2)
#define		BUFF_LENGTH		(2)

#define BUFF_EMPTY          (1)
#define BUFF_USEING         (2)
#define BUFF_FULL           (3)

typedef struct
{
	uint8_t		length;
	uint8_t		data[BUFF_LENGTH];		

}dynamic_store_t;

uint8_t get_ringbuf_status(void);
void ringbuf_write_data(uint8_t *buff, uint8_t buff_len);
void ringbuf_read_data(uint8_t *buff, uint8_t *buff_len);
void ringbuf_test(void);


#endif 

