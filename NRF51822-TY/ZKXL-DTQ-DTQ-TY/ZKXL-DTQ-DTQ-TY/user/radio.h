#ifndef __RADIO_H
#define	__RADIO_H

#include "nrf_esb.h"
#include "nrf_error.h"
#include "uart.h"
#include "sdk_common.h"
#include "my_misc_fun.h"
#include "timer.h"
#include "nrf_delay.h"
#include "define.h"
#include "ringbuffer.h"

//底层通信相关宏定义
#define 	 NRF_MAX_NUMBER_OF_RETRANSMITS		(9)					//重发次数,不包括第一次
#define		 NRF_RETRANSMIT_DELAY				(280)				//0~255ms随机值
#define 	 NRF_PIPE						(0)						
#define      NRF_TX_CHANNEL					(2)
#define      NRF_RX_CHANNEL					(4)
#define		 NRF_ACK_PROTOCOL_LEN			(17)					//自定义的软件模拟ACK协议数据长度
#define		 NRF_TOTAL_DATA_LEN				(250)				//2.4G数据总长度	
#define		 NRF_USEFUL_DATA_LEN		(NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)	//有效数据长度
#define		 NRF_DATA_IS_USE   				(0x00)
#define      NRF_DATA_IS_ACK				(0x01)
#define      NRF_DATA_IS_PRE				(0x02)		
#define 	 NRF_DATA_NEED_ACK				(0)
#define	 	 NRF_DATA_NEEDLESS_ACK			(1)
#define 	 NRF_DATA_HEAD					(0x61)
#define      NRF_DATA_END                   (0x21)
#define      NRF_DATA_RESERVE				(0xFF)  //保留位默认值

typedef struct
{
	//发送相关
//	uint8_t 						head;
//	uint8_t 						g_id[4];		//源UID
//	uint8_t 						s_id[4];		//目标UID
//	uint8_t 						sequence;
//	uint8_t 						pack_num;
//	uint8_t 						is_ack;
//	uint8_t							reserve[2];
//	uint8_t							length;
//	uint8_t							data[233];	//250-17
//	uint8_t 						xor;
//	uint8_t 						end;
	
	uint8_t							data[NRF_TOTAL_DATA_LEN];
	uint8_t							data_len;
	uint8_t 						ack[NRF_ACK_PROTOCOL_LEN];		//这里可以减少到17byte
	uint8_t							ack_len;
	
	//接收相关
	uint8_t 						rx_buf[NRF_TOTAL_DATA_LEN];
	uint8_t							rx_len;
	
	//逻辑相关
	bool							rx_ok_flag;
	bool							tx_ok_flag;
	bool							tx_ing_flag;

	uint8_t							retransmit_num;				//重发次数
	
	uint8_t							pack_re_tx_num;				//同一包号重发次数
	uint8_t 						sequence_re_tx_num;			//同一序号重发次数
	uint8_t 						sequence_re_tx_ack_flg;			//同一序号正在重复发送ACK
	uint8_t 						sequence_re_tx_use_flg;			//同一序号正在重复发送有效数据
	bool 							tx_idle_flag;				//false:idle, true:busy 
}transport_layer_t;


typedef struct 
{	
	bool				rx_window_on_flg;
	bool				rx_window_add_flg;
	bool				rx_window_wait_data;
	
	uint16_t            rx_window_on;
	uint16_t            rx_window_off;
	uint16_t            rx_window_add;
	uint16_t            ack_delay;
	
	void				(*wakeup)(void);		
	void				(*sleep)(void);
	void				(*rx_data)(void);		//2.4G原始数据过滤
	void				(*return_ack)(void);
	void				(*rx_success)(void);	//2.4G过滤后判定为有效数据
	void				(*tx_success)(void);
	void				(*tx_failed)(void);
}radio_struct_t;


extern nrf_esb_payload_t        tx_payload;
extern nrf_esb_payload_t        rx_payload;
extern transport_layer_t		TRANSPORT;

extern radio_struct_t			*RADIO;



void RADIO_init(void);

void nrf_start_transmit(void);
void my_nrf_tx_success_handler(void);
void my_nrf_tx_failed_handler(void);
void my_nrf_rx_success_handler(void);
void my_nrf_return_ack(void);
void nrf_rx_data_handler(void);

#endif 

