#ifndef __VERSION_H_
#define __VERSION_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
/* Software version */
#define MAIN_VERSION          0x01
#define SUB_VERSION_1         0x00
#define SUB_VERSION_2         0x03

/* Private functions ---------------------------------------------------------*/
void get_mcu_id(void);

#endif
