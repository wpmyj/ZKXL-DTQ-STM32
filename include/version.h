#ifndef __VERSION_H_
#define __VERSION_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
extern uint8_t             jsq_uid[8];
extern const uint8_t       software[3];
extern const uint8_t 		   hardware[30];
extern const uint8_t 			 company[16];
/* Private functions ---------------------------------------------------------*/
void get_mcu_uid(void);

#endif
