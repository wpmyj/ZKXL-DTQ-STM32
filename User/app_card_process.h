#ifndef __APP_CARD_PROCESS_H_
#define __APP_CARD_PROCESS_H_

#include "main.h"
#include "pos_handle_layer.h"
#include "rc500_handle_layer.h"

extern Process_tcb_Typedef Card_process;

void Buzze_Control(void);
void App_card_process(void);

#endif
