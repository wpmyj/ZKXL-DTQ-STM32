/**
  ******************************************************************************
  * @file   	variable.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	all variables called by other functions
  ******************************************************************************
  */
#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "define.h"
#include "hal_nrf.h"

/**
 * @enum nrf_esb_base_address_length_t
 * @brief Enumerator used for selecting the base address length.
 */
typedef enum 
{
    NRF_ESB_BASE_ADDRESS_LENGTH_2B,   ///< 2 byte address length
    NRF_ESB_BASE_ADDRESS_LENGTH_3B,   ///< 3 byte address length
    NRF_ESB_BASE_ADDRESS_LENGTH_4B    ///< 4 byte address length
} nrf_esb_base_address_length_t;


/**
 * @enum nrf_esb_output_power_t
 * @brief Enumerator used for selecting the TX output power.
 */
typedef enum 
{
    NRF_ESB_OUTPUT_POWER_4_DBM,          ///<  4 dBm output power.
    NRF_ESB_OUTPUT_POWER_0_DBM,          ///<  0 dBm output power.
    NRF_ESB_OUTPUT_POWER_N4_DBM,         ///< -4 dBm output power.
    NRF_ESB_OUTPUT_POWER_N8_DBM,         ///< -8 dBm output power.
    NRF_ESB_OUTPUT_POWER_N12_DBM,        ///< -12 dBm output power.
    NRF_ESB_OUTPUT_POWER_N16_DBM,        ///< -16 dBm output power.
    NRF_ESB_OUTPUT_POWER_N20_DBM         ///< -20 dBm output power.
} nrf_esb_output_power_t;


/**
 * @enum nrf_esb_datarate_t
 * @brief Enumerator used for selecting the radio data rate.
 */
typedef enum 
{
    NRF_ESB_DATARATE_250_KBPS,            ///< 250 Kbps datarate
    NRF_ESB_DATARATE_1_MBPS,              ///< 1 Mbps datarate
    NRF_ESB_DATARATE_2_MBPS,              ///< 1 Mbps datarate
} nrf_esb_datarate_t;


/**
 * @enum nrf_esb_crc_length_t
 * @brief Enumerator used for selecting the CRC length.
 */
typedef enum 
{
    NRF_ESB_CRC_OFF,            ///< CRC check disabled
    NRF_ESB_CRC_LENGTH_1_BYTE,  ///< CRC check set to 8-bit
    NRF_ESB_CRC_LENGTH_2_BYTE   ///< CRC check set to 16-bit    
} nrf_esb_crc_length_t;


typedef enum {SINGLE_CHOICE = 0x01, MULTIPLE_CHOICE = 0x02, TRUE_OR_FALSE = 0x03} QuestionType;
typedef enum {A = 0x01, B = 0x02, C = 0x04, D = 0x08, E = 0x10, F = 0x20, G = 0x40, H = 0x80} ChoiceAnswer;
typedef enum {LETTER = 0, NUMBER = !LETTER} InputMode;
typedef enum {LED1 	= 0,LED2 	= 1,LGREEN 	= 2,LBLUE 	= 3,} Led_TypeDef;
typedef enum {OFF 	= 0,ON 	= 1} Switch_State;

typedef struct
{

	bool 					flag_uart_rxing;		  			//中断串口正在接收标志
	bool 					flag_uart_rx_ok;		  			//中断串口接收完成标志
	
	bool 					flag_uart_rx_xor_err;				//中断串口接收XOR出错标志
	bool 					flag_uart_rx_length_err;			//中断串口接收长度出错标志
	
	uint8_t 				uart_status;						//中断接收数据步骤变量
	uint8_t 				uart_temp;							//中断接收数据临时变量
	uint8_t 				temp_uid_len;						//中断接收数据命令变量	
	uint8_t					temp_data_len;						//中断接收DATA部分临时长度变量	
	uint8_t                 temp_sign_len;                      //中断接收SIGN部分临时长度变量
	
	uint8_t 				uart_rx_timeout;					//中断串口接收超时计数器

	
	uint8_t 				HEADER;								//中断串口接收帧头
	uint8_t 				TYPE;								//中断串口接收包类型
	uint8_t                 SIGN[4];                            //中断串口接收活动标识
	uint8_t 				LEN;								//中断串口接收数据长度
	uint8_t 				DATA[UART_NBUF];					//中断串口接收数据
	uint8_t 				XOR;								//中断串口接收异或
	uint8_t 				END;								//中断串口接收帧尾
	
	uint8_t	 				uart_rx_cnt;			  			//中断串口接收计数器
	uint8_t					uart_rx_buf[UART_NBUF + 5];			//中断串口接收缓存	
	
	bool 					flag_tx_ok[2];		  				//中断串口接收完成标志
	bool 					flag_txing[2];		   				//中断串口正在发送标志
	uint8_t 				uart_tx_length[2];					//中断串口发送长度	
	uint8_t 				uart_tx_cnt;			  			//中断串口发送计数器
	uint8_t					uart_tx_buf[2][UART_NBUF + 5];		//中断串口发送缓存 ,两组缓冲区，轮流发送 

}Uart_TypeDef;

typedef struct 
{	
	bool 					flag_txing;		   					//2.4G下发数据标志
	bool 					flag_rx_ok;		  					//2.4G接收到新数据包
	bool 					flag_tx_ok;		  					//2.4G接收到新数据包
	
	uint8_t 				tx_len;			  					//2.4G发送长度
	uint8_t	 				rx_len;			  					//2.4G接收长度
	uint8_t					tx_buf[RF_NBUF];					//2.4G发送缓存  
	uint8_t					rx_buf[RF_NBUF];					//2.4G接收缓存
}RF_TypeDef;

typedef struct 
{	
	uint8_t 						packet_len;					//包长度
	uint8_t 						receive_flag;				//接收标识
	uint8_t							data[32];					//实际数据
}recv_data_packet_t;



typedef struct
{
	uint8_t 						hour;
	uint8_t 						minute;
	uint8_t 						second;
}time_t;

typedef struct
{
	bool 							state;						//uid有效标志
	bool	 						tx_flag;					//已下发标志	
	uint8_t 						count;						//收到空包计数
	uint8_t							number;						//配对序号
	uint8_t 						uid[4];						//UID
}white_list_t;

typedef union
{
	uint8_t 						true_or_false;				//判断题对应答案
	uint8_t							choice_answer;				//选择题对应答案
}answer_t;

typedef struct 
{	
	uint8_t							question_amount;			//题目数量，题目答案的有效个数，范围为1-11
	uint8_t 						question_number;			//题号，本包数据的当前题号即从哪一题开始的
	uint8_t							question_type[4];			//题目类型，每2位表示一个题目类型，1为单选，2为多选，3为判断
	answer_t						answer[16];					//题目答案，每个题目占1bytes
}answer_packet_t;

typedef struct 
{
	uint8_t							header;						//固定0x5A
	uint8_t							uid[4];						//uid
	uint8_t							pack_num;					//数据包的包号
	uint8_t							pack_type;					//数据包的类型
	uint8_t							payload_len;				//数据域的长度
	uint8_t							payload[22];				//数据域的内容
	uint8_t							xor_value;					//异或校验
	uint8_t							end;						//固定为0xCA
}datiqi_type_t;

typedef struct 
{
	hal_nrf_output_power_t			power;						//输出功率
	hal_nrf_datarate_t	 			datarate;					//速率
	hal_nrf_crc_mode_t	 			crc_length;					//CRC校验长度
	hal_nrf_address_width_t			base_address_length;		//基地址长度
	uint8_t							channel;					//频点
	uint8_t							pipe;						//接收通道	
	uint8_t							attempts;					//重发次数
	uint8_t							prefix_address;				//前缀地址
	uint16_t						base_address_low;			//基地址低16位
	uint16_t						base_address_high;			//基地址高16位
	uint16_t						delay_us;					//重发延迟时间
}nrf_parameter_t;

typedef struct 
{
	nrf_esb_output_power_t			power;							//输出功率
	nrf_esb_datarate_t	 			datarate;						//速率
	nrf_esb_crc_length_t 			crc_length;						//CRC校验长度
	nrf_esb_base_address_length_t	base_address_length;			//基地址长度
	uint8_t							channel;						//频点
	uint8_t							pipe;							//接收通道
	uint8_t							attempts;						//重发次数
	uint8_t							prefix_address;					//前缀地址
	uint16_t						base_address_low;				//基地址低16位
	uint16_t						base_address_high;				//基地址高16位
	uint16_t						delay_us;						//重发延迟时间
}answer_nrf_parameter_t;

typedef struct 
{
	uint8_t							Length_high_bytes;				//数据长度高字节	
	uint8_t							Length_low_bytes;				//数据长度低字节
	answer_nrf_parameter_t			ptx_parameter;					//发送射频参数
	answer_nrf_parameter_t			prx_parameter;					//接收射频参数
//	uint16_t						nrfSleepTime;					//腕带休眠时间
//	uint8_t							nrfSleepFlag;					//休眠标志
//	uint8_t							nrfWorkMode;					//工作模式标志
//	uint8_t							nrfMatchSIM;					//匹配标志
//	uint8_t							HWVersion[14];					//腕带硬件版本，ZL-S741-MAIN-X
//	uint16_t						SWVersion;						//腕带软件版本
//	uint8_t							SIM_UID[8];						//SIM卡序列号
	uint8_t							InitFlag;						//初始化标志
	uint8_t							Xor;							//数据段校验值
}answer_setting_parameter_t;

extern white_list_t					white_list[MAX_WHITE_LEN];		// 白名单列表
extern uint8_t						white_len;						// 白名单长度
extern Switch_State					white_on_off;					// 白名单开关
extern Switch_State					attendance_on_off;				// 考勤开关 
extern Switch_State					match_on_off;					// 配对开关 
extern uint16_t						match_number;					// 配对序号

extern uint8_t 						NDEF_DataWrite[];
extern uint8_t 						NDEF_DataRead[];

extern bool 						gbf_hse_setup_fail;				//外部16M晶振起振标志
extern uint8_t 						time_for_buzzer_on;				//多长时间之后开始响
extern uint16_t 					time_for_buzzer_off;			//响了多长时间之后关

extern answer_setting_parameter_t	answer_setting;					// 答题器的设置参数
extern nrf_parameter_t				nrf_parameter;


extern uint8_t                      sign_buffer[];
extern uint8_t						uart_tx_i;						//串口发送缓冲索引
extern Uart_TypeDef 				uart232_var;
extern RF_TypeDef 					rf_var;							// 2.4G数据包缓冲
extern uint16_t						delay_nms;						// 中断延时变量
extern uint32_t 					timer_1ms;
extern time_t						time;							//保持当前时间

extern uint8_t						uid_len;
extern uint8_t 						g_cSNR[];                    	//M1卡序列号

extern bool							flag_upload_uid_once;			//是否单次上传卡号标志
extern uint8_t						ReadNDEF_Step;					// 读取NDEF文件的步骤
extern uint8_t						FindCard_Step;					// 寻卡的步骤

extern uint8_t 						flag_App_or_Ctr;
extern uint16_t 					Length_CtrRC500ToApp;
extern uint16_t 					Length_AppToCtrRC500;
extern uint16_t 					Length_CtrPosToApp;
extern uint16_t 					Length_AppToCtrPos;
extern uint8_t 						Buf_CtrRC500ToApp[];
extern uint8_t 						Buf_AppToCtrRC500[];
extern uint8_t 						Buf_CtrPosToApp[];
extern uint8_t 						Buf_AppToCtrPos[];
extern uint8_t 						Buf_CtrRC500return[];			 //RC500返回数据缓存区
extern bool 						App_to_CtrPosReq;
extern bool 						App_to_CtrRC500Req;

extern uint8_t 						g_cardType[];					//返回卡类型
extern uint8_t 						respon[];
extern uint8_t 						g_cCid;

extern uint32_t                     MCUID[4];
extern uint8_t                     jsq_uid[8];
extern uint8_t                     software[3];
extern uint8_t 					   hardware[30];
extern uint8_t 					   company[16];
#endif //_VARIABLE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

