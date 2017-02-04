#ifndef __APP_CARD_PROCESS_H_
#define __APP_CARD_PROCESS_H_

#include "main.h"
#include "pos_handle_layer.h"
#include "rc500_handle_layer.h"

//#define OPEN_CARD_DEBUG_SHOW

/* 打印信息控制 */
#ifdef 	OPEN_CARD_DEBUG_SHOW
#define DEBUG_CARD_DEBUG_LOG							     DEBUG_LOG
#else
#define DEBUG_CARD_DEBUG_LOG(...)
#endif

extern Process_tcb_Typedef Card_process;

void Buzze_Control(void);
void App_card_process(void);
void rf_set_card_status(uint8_t new_status);
uint8_t rf_get_card_status(void);
void card_timer_init( void );

#endif
