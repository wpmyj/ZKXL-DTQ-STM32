/**
  ******************************************************************************
  * @file   	ringbuffer.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	ringbuffer
  ******************************************************************************
  */
	
#include "version.h"

/* Private variables ---------------------------------------------------------*/
uint8_t   software[3] = {
	MAIN_VERSION,
	SUB_VERSION_1,
	SUB_VERSION_2
};

uint8_t 	hardware[30] = {
		0x5,0xa,0x4,0xc,0x2,0xd,0x5,0x2,0x5,0x0,
		0x3,0x5,0x3,0x5,0x3,0x1,0x2,0xd,0x4,0xd,
		0x4,0x1,0x4,0x9,0x4,0xe,0x2,0xd,0x4,0x4 
};

uint8_t   company[16] = {
		0xd,0x1,0xb,0x6,0xc,0x1,0xa,0xa,
		0xb,0x9,0xc,0x9,0xb,0x7,0xd,0xd
};

uint32_t  MCUID[4];
uint8_t   jsq_uid[8];
/*******************************************************************************
  * @brief  Get stm32 MCU.
  * @param  None
  * @retval None
*******************************************************************************/
void GetMcuid(void)
{
	MCUID[0]=*(unsigned int*)(0x1FFFF7E8);
	MCUID[1]=*(unsigned int*)(0x1FFFF7EC);
	MCUID[2]=*(unsigned int*)(0x1FFFF7F0);
	MCUID[3]=MCUID[0]+MCUID[1]+MCUID[2];
	jsq_uid[7]=(uint8_t)(MCUID[3]>>0)&0x0f;
	jsq_uid[6]=(uint8_t)(MCUID[3]>>4)&0x0f; 
	jsq_uid[5]=(uint8_t)(MCUID[3]>>8)&0x0f;
	jsq_uid[4]=(uint8_t)(MCUID[3]>>12)&0x0f;
	jsq_uid[3]=(uint8_t)(MCUID[3]>>16)&0x0f;
	jsq_uid[2]=(uint8_t)(MCUID[3]>>20)&0x0f;
	jsq_uid[1]=(uint8_t)(MCUID[3]>>24)&0x0f;
	jsq_uid[0]=(uint8_t)(MCUID[3]>>28)&0x0f;
}
