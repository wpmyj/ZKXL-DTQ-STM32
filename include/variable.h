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
#include "gpio.h"
//#include "nrf_config_and_hander.h"
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

/* An enum describing the radio's CRC mode ---------------------------------- */
typedef enum
{
	HAL_NRF_CRC_OFF,    /**< CRC check disabled */
	HAL_NRF_CRC_8BIT,   /**< CRC check set to 8-bit */
	HAL_NRF_CRC_16BIT   /**< CRC check set to 16-bit */
} hal_nrf_crc_mode_t;
/* An enum describing the radio's address width ----------------------------- */
typedef enum
{
	HAL_NRF_AW_3BYTES = 3,      /**< Set address width to 3 bytes */
	HAL_NRF_AW_4BYTES,          /**< Set address width to 4 bytes */
	HAL_NRF_AW_5BYTES           /**< Set address width to 5 bytes */
}hal_nrf_address_width_t;


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

/* An enum describing the radio's on-air datarate --------------------------- */
typedef enum
{
	HAL_NRF_1MBPS,          /**< Datarate set to 1 Mbps  */
	HAL_NRF_2MBPS,          /**< Datarate set to 2 Mbps  */
	HAL_NRF_250KBPS         /**< Datarate set to 250 kbps*/
}hal_nrf_datarate_t;


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

/* An enum describing the radio's output power mode's. ---------------------- */
typedef enum
{
	HAL_NRF_18DBM,          /**< Output power set to -18dBm */
	HAL_NRF_12DBM,          /**< Output power set to -12dBm */
	HAL_NRF_6DBM,           /**< Output power set to -6dBm  */
	HAL_NRF_0DBM            /**< Output power set to 0dBm   */
}hal_nrf_output_power_t;

typedef struct 
{	
	bool 					flag_txing;		   					//2.4G下发数据标志
	bool 					flag_tx_ok;		  					//2.4G接收到新数据包
	
	uint8_t 			tx_len;			  					//2.4G发送长度
	uint8_t				tx_buf[RF_NBUF];					//2.4G发送缓存  
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



extern uint8_t 						NDEF_DataWrite[];
extern uint8_t 						NDEF_DataRead[];
extern uint16_t						NDEF_Len;

extern bool 						  gbf_hse_setup_fail;				//外部16M晶振起振标志
extern uint8_t 						time_for_buzzer_on;				//多长时间之后开始响
extern uint16_t 					time_for_buzzer_off;			//响了多长时间之后关

extern answer_setting_parameter_t	answer_setting;					// 答题器的设置参数
extern nrf_parameter_t				nrf_parameter;

extern RF_TypeDef 				rf_var;							// 2.4G数据包缓冲
extern uint16_t						delay_nms;						// 中断延时变量
extern uint32_t 					timer_1ms;
extern time_t						time;							//保持当前时间

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

typedef struct
{
	uint8_t  uid[4];
	uint8_t  pos;
	uint8_t  use;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  firstrev;
	uint32_t lost_package_num;
	uint32_t recv_package_num;
}Clicker_Typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint32_t data_statistic_count;
}Revicer_Typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint8_t    len;
	uint8_t    switch_status;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
}WhiteList_Typedef;

typedef struct
{
	uint8_t  status;
	uint32_t cnt;
}Timer_Typedef;

typedef struct 
{
	uint8_t hour;
	uint8_t min;
	uint8_t s;
	uint16_t ms;
}timer_t;

#endif //_VARIABLE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

