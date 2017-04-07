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

//�ײ�ͨ����غ궨��
#define 	 NRF_MAX_NUMBER_OF_RETRANSMITS		(9)					//�ط�����,��������һ��
#define		 NRF_RETRANSMIT_DELAY				(280)				//0~255ms���ֵ
#define 	 NRF_PIPE						(0)						
#define      NRF_TX_CHANNEL					(2)
#define      NRF_RX_CHANNEL					(4)
#define		 NRF_ACK_PROTOCOL_LEN			(17)					//�Զ�������ģ��ACKЭ�����ݳ���
#define		 NRF_TOTAL_DATA_LEN				(250)				//2.4G�����ܳ���	
#define		 NRF_USEFUL_DATA_LEN		(NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)	//��Ч���ݳ���
#define		 NRF_DATA_IS_USE   				(0x00)
#define      NRF_DATA_IS_ACK				(0x01)
#define      NRF_DATA_IS_PRE				(0x02)		
#define 	 NRF_DATA_NEED_ACK				(0)
#define	 	 NRF_DATA_NEEDLESS_ACK			(1)
#define 	 NRF_DATA_HEAD					(0x61)
#define      NRF_DATA_END                   (0x21)
#define      NRF_DATA_RESERVE				(0xFF)  //����λĬ��ֵ

typedef struct
{
	//�������
//	uint8_t 						head;
//	uint8_t 						g_id[4];		//ԴUID
//	uint8_t 						s_id[4];		//Ŀ��UID
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
	uint8_t 						ack[NRF_ACK_PROTOCOL_LEN];		//������Լ��ٵ�17byte
	uint8_t							ack_len;
	
	//�������
	uint8_t 						rx_buf[NRF_TOTAL_DATA_LEN];
	uint8_t							rx_len;
	
	//�߼����
	bool							rx_ok_flag;
	bool							tx_ok_flag;
	bool							tx_ing_flag;

	uint8_t							retransmit_num;				//�ط�����
	
	uint8_t							pack_re_tx_num;				//ͬһ�����ط�����
	uint8_t 						sequence_re_tx_num;			//ͬһ����ط�����
	uint8_t 						sequence_re_tx_ack_flg;			//ͬһ��������ظ�����ACK
	uint8_t 						sequence_re_tx_use_flg;			//ͬһ��������ظ�������Ч����
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
	void				(*rx_data)(void);		//2.4Gԭʼ���ݹ���
	void				(*return_ack)(void);
	void				(*rx_success)(void);	//2.4G���˺��ж�Ϊ��Ч����
	void				(*tx_success)(void);
	void				(*tx_failed)(void);
}radio_struct_t;


extern nrf_esb_payload_t        tx_payload;
extern nrf_esb_payload_t        rx_payload;
extern transport_layer_t		TRANSPORT;

extern radio_struct_t			*RADIO;



void RADIO_init(void);

uint32_t RADIO_Config(void);
void RADIO_Wakeup(void);
void RADIO_Sleep(void);
void RADIO_StartTx(void);
void RADIO_TxSuccessHandler(void);
void RADIO_TxFailedHandler(void);
void RADIO_RxSuccessHandler(void);
void RADIO_ReturnAck(void);
void RADIO_RxDataHandler(void);

#endif 

