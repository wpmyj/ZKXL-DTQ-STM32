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

void App_seirial_cmd_process(void);

/* Uart Message configuration */


/* Uart Message frame header and tail */
#define UART_SOF								          (0x5C)							//帧头
#define UART_EOF 								          (0xCA)							//帧尾

/* Uart message status */
#define UartOK	 								          (0)									//串口接收帧完成
#define UartHEADER 							          (1)									//串口接收帧帧头
#define UartTYPE 								          (2)									//串口接收帧数据
#define UartLEN									          (3)									//串口接收帧异或
#define UartSIGN                          (4)
#define UartDATA 								          (5)									//串口接收帧帧尾
#define UartXOR									          (6)									//串口接收帧异或
#define UartEND 								          (7)									//串口接收帧帧尾

/* Uart Message structure definition */
typedef struct
{
	uint8_t 				HEADER;						  //中断串口接收帧头
	uint8_t 				TYPE;								//中断串口接收包类型
	uint8_t         SIGN[4];            //中断串口接收活动标识
	uint8_t 				LEN;								//中断串口接收数据长度
	uint8_t 				DATA[UART_NBUF];		//中断串口接收数据
	uint8_t 				XOR;								//中断串口接收异或
	uint8_t 				END;								//中断串口接收帧尾
} Uart_MessageTypeDef;

typedef struct
{
	uint8_t 				HEADER;						  
	uint8_t 				ID[4];								  
	uint8_t 				RFU;
	uint8_t 				TYPE;		          
	uint8_t 				LEN;								
	uint8_t 				DATA[UART_NBUF-10];		
	uint8_t 				XOR;								
	uint8_t 				END;								
}Rf_MessageTypeDef;

typedef struct
{
	uint8_t uid[4];
	uint8_t cmd_type;
	uint8_t studentid[20];
	uint8_t match_single;
}Process_tcb_Typedef;

void serial_handle_layer(void);
void App_returnErr( Uart_MessageTypeDef *SMessage, uint8_t cmd_type, uint8_t err_type );
uint8_t get_backup_massage_status( void );
void change_clicker_send_data_status( uint8_t newstatus );
uint8_t get_clicker_send_data_status( void );
#endif // __POS_HANDLE_LAYER_H_
