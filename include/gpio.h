#ifndef __LED_H_
#define __LED_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "board.h"

typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;
typedef enum {LETTER = 0, NUMBER = !LETTER} InputMode;
typedef enum {LED1 	= 0,LED2 	= 1, LBLUE	= 2, LRED	= 3,} Led_TypeDef;

/* Private functions ---------------------------------------------------------*/
void ledInit(Led_TypeDef Led);
void ledOn(Led_TypeDef Led);
void ledOff(Led_TypeDef Led);
void ledToggle(Led_TypeDef Led);
void GPIOInit_BEEP(void);
void bsp_board_init(void);

#endif
