/**
  ******************************************************************************
  * @file   	version.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	version infomation
  ******************************************************************************
  */
#include "version.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
const uint8_t software[3] = {
	MAIN_VERSION,
	SUB_VERSION_1,
	SUB_VERSION_2
};

#ifdef ZL_RP551_MAIN_E
const uint8_t hardware[30] = {
	0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x31,0x2d,0x4d,0x41,0x49,0x4e,0x2d,0x45
};
#endif

#ifdef ZL_RP551_MAIN_F
const uint8_t hardware[30] = {
	0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x31,0x2d,0x4d,0x41,0x49,0x4e,0x2d,0x46
};
#endif

#ifdef ZL_RP551_MAIN_H
const uint8_t hardware[30] = {
	0x5a,0x4c,0x2d,0x52,0x50,0x35,0x35,0x31,0x2d,0x4d,0x41,0x49,0x4e,0x2d,0x48
};
#endif

const uint8_t company[16] = {
		0xd,0x1,0xb,0x6,0xc,0x1,0xa,0xa,
		0xb,0x9,0xc,0x9,0xb,0x7,0xd,0xd
};

uint8_t  jsq_uid[8];

extern nrf_communication_t nrf_data;
extern revicer_typedef   revicer;
/*******************************************************************************
  * @brief  Get stm32 MCU.
  * @param  None
  * @retval None
*******************************************************************************/
void get_mcu_uid(void)
{
	uint32_t MCUID[4];
	
	MCUID[0] = *(volatile uint32_t *)(0x1FFFF7E8);
	MCUID[1] = *(volatile uint32_t *)(0x1FFFF7EC);
	MCUID[2] = *(volatile uint32_t *)(0x1FFFF7F0);
	
	MCUID[3] = MCUID[0] + MCUID[1] + MCUID[2];
	
	jsq_uid[7]=(uint8_t)(MCUID[3]>>0)&0x0f;
	jsq_uid[6]=(uint8_t)(MCUID[3]>>4)&0x0f; 
	jsq_uid[5]=(uint8_t)(MCUID[3]>>8)&0x0f;
	jsq_uid[4]=(uint8_t)(MCUID[3]>>12)&0x0f;
	jsq_uid[3]=(uint8_t)(MCUID[3]>>16)&0x0f;
	jsq_uid[2]=(uint8_t)(MCUID[3]>>20)&0x0f;
	jsq_uid[1]=(uint8_t)(MCUID[3]>>24)&0x0f;
	jsq_uid[0]=(uint8_t)(MCUID[3]>>28)&0x0f;

	revicer.uid[0] = (jsq_uid[1]&0x0F)|((jsq_uid[0]<<4)&0xF0);
	revicer.uid[1] = (jsq_uid[3]&0x0F)|((jsq_uid[2]<<4)&0xF0);
	revicer.uid[2] = (jsq_uid[5]&0x0F)|((jsq_uid[4]<<4)&0xF0);
	revicer.uid[3] = (jsq_uid[7]&0x0F)|((jsq_uid[6]<<4)&0xF0);
	
	memcpy(nrf_data.jsq_uid,revicer.uid,4);
}
