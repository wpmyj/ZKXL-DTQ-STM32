#ifndef __KEY_H
#define	__KEY_H

#include <stdbool.h>
#include "app_timer.h"
#include "uart.h"
#include "define.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"

#define BUTTON_0       KEY_ROW1
#define BUTTON_1       KEY_ROW2
#define BUTTON_2       KEY_ROW3
#define BUTTON_3       KEY_ROW4
#define BUTTON_4       KEY_PWR
#define BUTTONS_NUMBER 5

#define BUTTON_DETECTION_DELAY           APP_TIMER_TICKS(10, 0)

//¼üÅÌÐÐIO¿Ú
#define KEY_ROW1		    	(0u)
#define KEY_ROW2		    	(1u)
#define KEY_ROW3		    	(2u)
#define KEY_ROW4		    	(3u)
#define	KEY_COL1				(4u)
#define	KEY_COL2				(5u)
#define	KEY_COL3				(6u)
#define	KEY_COL4				(7u)

#define	KEY_PWR					(30u)


#define KEY_PORT 				(NRF_GPIO_PORT_SELECT_PORT0)
#define KEY_VALUE_MASK			(0x0F)

typedef enum {
	KEY_SCAN, 
    KEY_ONE,
	KEY_TWO
} __KEY_STATE;	



typedef struct 
{
	bool				press_flg;			
	bool				run_flg;
	__KEY_STATE         state;
	uint8_t 			value;


}key_struct_t;

extern key_struct_t				*KEY;

void key_button_init(void);
void KEY_Scan(void);
extern void KEY_init(void);

#endif 

