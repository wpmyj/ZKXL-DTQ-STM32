#ifndef	__POS_HANDLE_LAYER_H_
#define	__POS_HANDLE_LAYER_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private define ------------------------------------------------------------*/

/* Uart to App cmd */
#define APP_CTR_SENT_DATA_VIA_2_4G        0x01 // 0x01: 通过2.4G下发数据包处理
#define APP_CTR_GET_DATA_VIA_2_4G         0x02 // 0x02: 通过2.4G接收过来的数据包处理
#define APP_CTR_ADD_WHITE_LIST            0x11 // 0x11: 添加白名单
#define APP_CTR_DEL_WHITE_LIST            0x12 // 0x12: 删除白名单
#define APP_CTR_INIT_WHITE_LIST           0x13 // 0x13: 初始化白名单
#define APP_CTR_OPEN_WHITE_LIST           0x14 // 0x14: 开启白名单
#define APP_CTR_CLOSE_WHITE_LIST          0x15 // 0x15: 关闭白名单
#define APP_CTR_OPEN_ATTENDANCE           0x16 // 0x16: 开启考勤
#define APP_CTR_CLOSE_ATTENDANCE          0x17 // 0x17: 关闭考勤
#define APP_CTR_UPDATE_ATTENDANCE_DATA    0x18 // 0x18: 上传刷卡数据
#define APP_CTR_OPEN_PAIR                 0x19 // 0x19: 开启配对
#define APP_CTR_CLOSE_PAIR                0x0C // 0x0c: 关闭配对
#define APP_CTR_UPATE_PAIR_DATA           0x0D // 0x0d: 上传配对数据
#define APP_CTR_DATALEN_ERR               0xFE // 0xfe: 帧长度不合法
#define APP_CTR_UNKNOWN                   0xFF // 0xff: 未能识别的帧

#define APP_SERIAL_CMD_STATUS_IDLE        0x00
#define APP_SERIAL_CMD_STATUS_WORK        0x01
#define APP_SERIAL_CMD_STATUS_ERR         0x02
#define APP_SERIAL_CMD_STATUS_IGNORE      0x03
#define APP_SERIAL_CMD_STATUS_WORK_IGNORE 0x04

#define START_SEND_DATA                   0
#define STOP_SEND_DATA                    1

#define VERSION_LEN                       2
#define LEN_LEN                           2
#define UID_LEN                           4
#define ACKTABLELEN                       16
#define REVICER_MESSAGE_LEN               750

void App_seirial_cmd_process(void);

/* Uart Message configuration */


/* Uart Message frame header and tail */
#define UART_SOF								                           (0x5C)							
#define UART_EOF 								                           (0xCA)							

/* Uart message status */
#define UartOK	 								                           (0)
#define UartHEAD  							                           (1+UartOK)
#define UartEVICETYPE                                      (1+UartHEAD)
#define UartVERSION                                        (1+UartEVICETYPE)
#define UartDSTID								                           (1+UartVERSION)
#define UartSRCID								                           (1+UartDSTID)		
#define UartPACNUM								                         (1+UartSRCID)
#define UartSEQNUM								                         (1+UartPACNUM)
#define UartPACKTYPE								                       (1+UartSEQNUM)	
#define UartREVICED                                        (1+UartPACKTYPE)
#define UartCMD								                             (1+UartREVICED)										                         		
#define UartLEN									                           (1+UartCMD)		
#define UartDATA 								                           (1+UartLEN)	
#define UartXOR									                           (1+UartDATA)
#define UartEND 								                           (1+UartXOR)	

#define MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA                19

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

typedef enum
{
	REVICER_CLICKER_DATA    = 0x10,
	REVICER_CLICKER_CTL     = 0x11,
	REVICER_CLICKER_UID     = 0x12,
	REVICER_CLICKER_SYSTICK = 0x13, 
  REVICER_CLICKER_ERR     = 0x14,
}UartCmd_Typedef;

typedef enum
{
	U_BIND_ON = 1,
	U_BIND_OFF,
	U_CLEAR,
	U_DEL,
	U_SHOW,
	U_CHECK_ON,
	U_CHECK_OFF,
	U_WR_STD_ID,
}UidTask_CTL_Typedef;


/* Uart Message structure definition */
typedef struct
{
	uint8_t HEAD;                        // 1 byte
	uint8_t DEVICE;                      // 1 byte   1
	uint8_t VERSION[VERSION_LEN];        // 2 byte   3
	uint8_t DSTID[UID_LEN];              // 4 byte   7
	uint8_t SRCID[UID_LEN];              // 4 byte   11
	uint8_t PACNUM;                      // 1 byte   12
	uint8_t SEQNUM;                      // 1 byte   13
	uint8_t PACKTYPE;            // 1 byte   14
	uint8_t REVICED[2];                  // 2 byte   16
	uint8_t CMDTYPE;             // 1 byte   17
	uint8_t LEN[LEN_LEN];                // 2byte    19
	uint8_t DATA[REVICER_MESSAGE_LEN];
	uint8_t XOR;                         // 1byte: form DEVICE to DATA
	uint8_t END;                         // 1byte
} Uart_MessageTypeDef;

typedef struct
{
	uint8_t HEADER;						  
	uint8_t ID[4];								  
	uint8_t RFU;
	uint8_t TYPE;		          
	uint8_t LEN;								
	uint8_t DATA[UART_NBUF-10];		
	uint8_t XOR;								
	uint8_t END;								
}Rf_MessageTypeDef;

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

void serial_handle_layer(void);
uint8_t get_backup_massage_status( void );
void change_clicker_send_data_status( uint8_t newstatus );
uint8_t get_clicker_send_data_status( void );
void App_seiral_process_init( void );
uint8_t App_returnErr( Uart_MessageTypeDef *sMessage, uint8_t cmd_type, uint8_t err_type );
#endif // __POS_HANDLE_LAYER_H_
