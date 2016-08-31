#ifndef __LED_H_
#define __LED_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* LED defines ---------------------------------------------------------------*/
#define LEDn                  		4
#define LED1_GPIO_PORT       			GPIOC
#define LED1_GPIO_CLK        			RCC_APB2Periph_GPIOC
#define LED1_GPIO_PIN        			GPIO_Pin_1
#define LED2_GPIO_PORT       			GPIOC
#define LED2_GPIO_CLK        			RCC_APB2Periph_GPIOC
#define LED2_GPIO_PIN        			GPIO_Pin_2
#define LGREEN_GPIO_PORT       		GPIOB
#define LGREEN_GPIO_CLK        		RCC_APB2Periph_GPIOB
#define LGREEN_GPIO_PIN        		GPIO_Pin_6
#define LBLUE_GPIO_PORT       		GPIOB
#define LBLUE_GPIO_CLK        		RCC_APB2Periph_GPIOB
#define LBLUE_GPIO_PIN        		GPIO_Pin_7

/* BEEP defines ---------------------------------------------------------------*/
#define BEEP_PORT             		GPIOC
#define BEEP_CLK              		RCC_APB2Periph_GPIOC
#define BEEP_PIN              		GPIO_Pin_0
#define BEEP_EN()									{GPIO_SetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_DISEN()							{GPIO_ResetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TOGGLE()							{BEEP_PORT->ODR ^= BEEP_PIN;}

typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;
typedef enum {LETTER = 0, NUMBER = !LETTER} InputMode;
typedef enum {LED1 	= 0,LED2 	= 1,LGREEN 	= 2,LBLUE 	= 3,} Led_TypeDef;


/* Private functions ---------------------------------------------------------*/
void ledInit(Led_TypeDef Led);
void ledOn(Led_TypeDef Led);
void ledOff(Led_TypeDef Led);
void ledToggle(Led_TypeDef Led);
void GPIOInit_BEEP(void);
void GpioInit(void);

#endif
