#ifndef __APP_CARD_PROCESS_H_
#define __APP_CARD_PROCESS_H_

#include "main.h"
#include "app_serial_cmd_process.h"

//#define OPEN_CARD_DEBUG_SHOW 
//#define OPEN_CARD_DATA_SHOW

/* 打印信息控制 */
#ifdef 	OPEN_CARD_DEBUG_SHOW
#define DEBUG_CARD_DEBUG_LOG							     DEBUG_LOG
#else
#define DEBUG_CARD_DEBUG_LOG(...)
#endif

#ifdef 	OPEN_CARD_DATA_SHOW
#define DEBUG_CARD_DATA_LOG							     DEBUG_LOG
#else
#define DEBUG_CARD_DATA_LOG(...)
#endif

typedef struct
{
	uint8_t  N_CH_TX;
	uint8_t  N_CH_RX;
	int8_t   N_TX_POWER;
	uint8_t  N_TX_SPEED;
	uint8_t  N_TX_RETRANS;
	uint8_t  N_TX_SLEEP;
}clicker_config_typedef;

extern Process_tcb_Typedef Card_process;

void Buzze_Control(void);
void App_card_process(void);
void rf_set_card_status(uint8_t new_status);
uint8_t rf_get_card_status(void);
void card_timer_init( void );
void clicker_config_default_set( void );

#endif
