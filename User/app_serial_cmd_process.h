#ifndef	__POS_HANDLE_LAYER_H_
#define	__POS_HANDLE_LAYER_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private define ------------------------------------------------------------*/

/* Uart to App cmd */
#define APP_SERIAL_CMD_STATUS_IDLE        0x00
#define APP_SERIAL_CMD_STATUS_WORK        0x01
#define APP_SERIAL_CMD_STATUS_ERR         0x02
#define APP_SERIAL_CMD_STATUS_IGNORE      0x03
#define APP_SERIAL_CMD_STATUS_WORK_IGNORE 0x04

#define START_SEND_DATA                   0
#define STOP_SEND_DATA                    1

#define LEN_LEN                           2
#define UID_LEN                           4
#define ACKTABLELEN                       16
#define REVICER_MESSAGE_LEN               750

void App_seirial_cmd_process(void);

/* Uart Message configuration */


/* Uart Message frame header and tail */
#define UART_SOF								          ('{')							//֡ͷ
#define UART_EOF 								          ('}')							//֡β

/* Uart message status */
#define UartSTART 							          (1)
#define UartDATA	 								        (2)
#define UartEND 								          (3)

typedef enum
{
	CLOSE = 0,
	OPEN,
}Bool_Typedf;

typedef enum
{
	REVICER_PACKAGE_DATA = 0,
	REVICER_PACKAGE_ACK,
}UartPac_Typedef;

typedef struct
{
	uint8_t srcid[4];
	uint8_t cmd_type;
	uint8_t stdid[10];
	uint8_t retransmit;
}task_tcb_typedef;

typedef struct
{
	uint8_t state;
	uint8_t desc[30];
}State_Typedef;

typedef struct
{
	State_Typedef state;
	void    (*set_status)( State_Typedef *state, uint8_t new_status );
	uint8_t (*get_status)( State_Typedef *state );
}StateMechineTcb_Typedef;

extern StateMechineTcb_Typedef uart_rev_status,uart_sen_status;

#endif // __POS_HANDLE_LAYER_H_
