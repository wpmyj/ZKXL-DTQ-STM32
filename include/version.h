#ifndef __VERSION_H_
#define __VERSION_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
/* Software version */
#define MAIN_VERSION          0x00
#define SUB_VERSION_1         0x01
#define SUB_VERSION_2         0x03

extern uint8_t             jsq_uid[8];
extern const uint8_t       software[3];
extern const uint8_t 		   hardware[30];
extern const uint8_t 			 company[16];
/* Private functions ---------------------------------------------------------*/
void get_mcu_uid(void);

#endif
