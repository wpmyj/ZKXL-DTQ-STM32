#ifndef   WIEGAND_H
#define   WIEGAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "rfsim_MCUConf.h"
#define LOW  (0)
#define HIGH (1)

#define Weigand_TIM		(TIM3)

#define WIEGANDTIME		(200)
#define WEIGANDNODATA	(0x2710)

#define Data0(x) do{\
	if (x == 1)\
	{\
		GPIO_ResetBits(GPIOC, GPIO_Pin_11);\
	}\
	else\
	{\
		GPIO_SetBits(GPIOC, GPIO_Pin_11);\
	}\
  } while(0)

#define Data1(x) do{\
	if (x == 1)\
	{\
		GPIO_ResetBits(GPIOC, GPIO_Pin_10);\
	}\
	else\
	{\
		GPIO_SetBits(GPIOC, GPIO_Pin_10);\
	}\
  } while(0)

void WEIGEN_HWInit(void);
void WEIGEN_SWInit(void);
void WiegandInit(uint8_t *pSrc);
void WiegandOut(void);
void WiegandInit34(uint8_t *pSrc);
void WiegandOut34(void);

#ifdef __cplusplus
}
#endif

#endif
