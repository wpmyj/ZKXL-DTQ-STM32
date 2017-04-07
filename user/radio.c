

/*-------------------------------- Include File -----------------------------------------*/
#include "radio.h"
/*-------------------------------- Macro definition -------------------------------------*/
//#define RADIO_DEBUG
#ifdef RADIO_DEBUG
#define radio_debug  debug_printf   
#else  
#define radio_debug(...)                    
#endif 
/*-------------------------------- Local Variable ----------------------------------------*/

radio_struct_t				radio_struct;
										   
/*-------------------------------- Gocal Variable ----------------------------------------*/
radio_struct_t				*RADIO = &radio_struct;

nrf_esb_payload_t        tx_payload;
nrf_esb_payload_t        rx_payload;

transport_layer_t		TRANSPORT;

/*-------------------------------- Function Declaration ----------------------------------*/
//static uint32_t RADIO_Config(void);
//static void RADIO_Wakeup(void);
//static void RADIO_Sleep(void);

/*----------------------------------------------------------------------------------------*/

void RADIO_init(void)
{
	ret_code_t err_code;
	
    err_code = RADIO_Config();
    APP_ERROR_CHECK(err_code);
	
	RADIO->rx_window_on_flg = false;
	RADIO->rx_window_add_flg = false;
	RADIO->rx_window_wait_data = false;
	
	RADIO->rx_window_on = RX_WINDOW_ON;
	RADIO->rx_window_off = RX_WINDOW_OFF;
	RADIO->rx_window_add = RX_WINDOW_ADD_WAIT_DATA;
	RADIO->ack_delay = RETURN_ACK_DELAY;
	
//	
//	RADIO_Wakeup       = RADIO_Wakeup;
//	RADIO_Sleep        = RADIO_Sleep;
//	RADIO_RxDataHandler		= RADIO_RxDataHandler;
//	RADIO_ReturnAck	= RADIO_ReturnAck;
//	RADIO_RxSuccessHandler	= RADIO_RxSuccessHandler;
//	RADIO_TxSuccessHandler	= RADIO_TxSuccessHandler;
//	RADIO_TxFailedHandler	= RADIO_TxFailedHandler;
}


void RADIO_Wakeup(void)
{
	RADIO->rx_window_on_flg = false;
	TIMER_RxWindowStart();		//����ʱ�䴰��ʱ����
}

void RADIO_Sleep(void)
{
	TIMER_RxWindowStop();	//����ʱ�䴰��ʱ��ֹͣ
	nrf_esb_stop_rx();			//ֹͣ����
	
}

/*****************************************************************************
  @����:nrf_esb_event_handler
  @����:2.4G�����շ����ݻص�����
  @����:
  @���:
  @����:
******************************************************************************/
void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{	
    switch (p_event->evt_id)   
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
//			printf("TX SUCCESS EVENT %02X \r\n",NRF_RADIO->STATE);
			if(TRANSPORT.sequence_re_tx_num != 0)
			{
				nrf_esb_flush_tx();
				nrf_esb_write_payload(&tx_payload);	
				TRANSPORT.sequence_re_tx_num--;
			}
			else
			{
				RADIO->rx_window_wait_data = false;
				TIMER_TxOvertimeStop();
				
				if(TRANSPORT.tx_ing_flag)	//����ڷ������ݣ�����RX���ڣ��ȴ�"ACK"
				{
					logic_high();
					TIMER_RxWindowAdd(RX_WINDOW_ADD_WAIT_ACK);
				}			
				else
					TIMER_RxWindowStart();
			}
            break;
        case NRF_ESB_EVENT_TX_FAILED:
			TIMER_TxOvertimeStop();
			TIMER_RxWindowStart();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            break;
    }
}

/******************************************************************************
  @����:RADIO_Config
  @����:
  @����:
  @���:
  @����:
******************************************************************************/
static uint32_t RADIO_Config(void)
{
	uint32_t err_code;
	uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
	uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

	nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
	nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
	nrf_esb_config.retransmit_delay         = 600;
	nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_1MBPS;
	nrf_esb_config.event_handler            = nrf_esb_event_handler;
	nrf_esb_config.mode                     = NRF_ESB_MODE_PRX;
	nrf_esb_config.selective_auto_ack       = true;	
	nrf_esb_config.payload_length           = 250;
	
	err_code = nrf_esb_set_rf_channel(NRF_RX_CHANNEL);		//ע�⣺����������Ƶ��2����Ƶ��122���������෴
	VERIFY_SUCCESS(err_code);
	
	err_code = nrf_esb_init(&nrf_esb_config);
	VERIFY_SUCCESS(err_code);

	err_code = nrf_esb_set_base_address_0(base_addr_0);
	VERIFY_SUCCESS(err_code);

	err_code = nrf_esb_set_prefixes(addr_prefix, 8);
	VERIFY_SUCCESS(err_code);

	return err_code;
}




/******************************************************************************
  @����:RADIO_StartTx
  @����:�Զ���ͨ�Ų㿪ʼ�������ݣ�
  @����:
  @���:
  @����:
******************************************************************************/
void RADIO_StartTx(void)
{
	uint8_t i;
	
	/* ÿ�η���ǰ��ȡҪ���͵����ݣ���֤tx_payload.data�ṹ���е����������µ� */
	tx_payload.length = TRANSPORT.data_len;
	tx_payload.pipe    = NRF_PIPE;
	tx_payload.noack = true;
	memcpy(tx_payload.data, TRANSPORT.data, tx_payload.length);
	
//	tmp_debug("tx_len:%d \r\n",tx_payload.length);
//	for(i = 0;i < tx_payload.length;i++)
//		tmp_debug("%02X ",tx_payload.data[i]);
//	tmp_debug("\r\n");
	
	/* ������������ */
	TRANSPORT.sequence_re_tx_num = 1;
	TIMER_RxWindowStop();	 			
	TIMER_TxOvertimeStart(); 			 
	my_esb_mode_change(NRF_ESB_MODE_PTX);
	nrf_esb_flush_tx();
	nrf_esb_write_payload(&tx_payload);		
	
	/* �״η��ͣ������ط���ʱ�� */
	if (false == TRANSPORT.tx_ing_flag)
	{
		APP->key_send_allow_flg = false;
		TRANSPORT.tx_ing_flag = true;
		TIMER_RetransmitStart();				//�����ط���ʱ��
	}
}

/******************************************************************************
  @����:RADIO_RxDataHandler
  @����:2.4G�յ��������������棬�˺����Ի�������ݽ��д���
  @����:
  @���:
  @����:�ⲿ��
******************************************************************************/
void RADIO_RxDataHandler(void)
{
	uint8_t i;
	static uint8_t last_rx_sequence = 0xFF;
	static uint8_t last_rx_pack_num = 0x00;
	uint8_t tmp_buf[16]; 				//����ǰ��֡�Ļ�Ӧλ
	uint16_t point_buf[8];
	uint8_t  point_bit;
	
	if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
	{
		if(0 == rx_payload.length)
			return;
//		tmp_debug("rx_len:%d \r\n",rx_payload.length);
//		for(i = 0;i < rx_payload.length;i++)
//			tmp_debug("%02X ",rx_payload.data[i]);
//		tmp_debug("\r\n");
		
			/* ��ͷ����β�����У�� */
		if(rx_payload.data[0]					  == NRF_DATA_HEAD &&
		   rx_payload.data[rx_payload.length-1] == NRF_DATA_END  &&
		   rx_payload.data[rx_payload.length-2] == XOR_Cal(rx_payload.data+1,(rx_payload.length-3)))
		{
			//�Ƿ�������ԵĽ�����������
			if(stringcmp(rx_payload.data+5,JSQ_UID,4))
			{
				/* �����ض�uid��㲥 */
				if(stringcmp(rx_payload.data+1,PUBLIC_UID, 4) || stringcmp(rx_payload.data+1,DTQ_UID,4))
				{
					switch(rx_payload.data[11])
					{
						case NRF_DATA_IS_USE:
							RADIO->rx_window_wait_data = false;
							if(rx_payload.data[10] != last_rx_pack_num)		//��һ���յ��˰���
							{
								radio_debug("radio:first rx data,return ack. \r\n");
								last_rx_sequence = rx_payload.data[9];
								last_rx_pack_num = rx_payload.data[10];
								//�ж��Ƿ���Ҫ��ack
								memcpy(tmp_buf, rx_payload.data+15+rx_payload.data[14], 16); 
								for(i = 0;i < 8;i++)
								{
									point_buf[i] = (uint16_t)((tmp_buf[i*2] << 0) | (tmp_buf[i*2+1] << 8));
								}
								point_bit = (point_buf[DTQ_NUM / 15 ] >> (DTQ_NUM % 15)) & 0x01;
								if(point_bit == 0)
									RADIO_ReturnAck();	
								RADIO_RxSuccessHandler();
							}
							else												//�ظ����յİ���
							{
								if(rx_payload.data[9] != last_rx_sequence)	//��ͬ֡�ţ��ظ�ACK
								{
									radio_debug("[radio] re_rx data,new sequence,return ack \r\n");
									last_rx_sequence = rx_payload.data[9];
									
									memcpy(tmp_buf, rx_payload.data+15+rx_payload.data[14], 16); 
									for(i = 0;i < 8;i++)
									{
										point_buf[i] = (uint16_t)((tmp_buf[i*2] << 0) | (tmp_buf[i*2+1] << 8));
									}
									point_bit = (point_buf[DTQ_NUM / 15 ] >> (DTQ_NUM % 15)) & 0x01;
									if(point_bit == 0)
										RADIO_ReturnAck();		
								}	
								else 
								{
									radio_debug("[radio] re_rx data,old sequence \r\n");
								}									
							}
							break;
						case NRF_DATA_IS_ACK:
							radio_debug("[radio] rx ack \r\n");
							TRANSPORT.tx_ok_flag = true;
							logic_low();	
//							LCD_display_student_id(rx_payload.rssi & 0x7f);
							break;
						case NRF_DATA_IS_PRE:
							if(0x00 == last_rx_pack_num)	//���ſ�����ʼ��Ϊ0������ʱ��������RX����������
							{
								if(false == RADIO->rx_window_wait_data)
								{
									RADIO->rx_window_wait_data = true;
									if(110 > (rx_payload.data[9] + 5))
									{
										TIMER_WaitDataStart(110 - rx_payload.data[9] - 5);	//��5��ǰ��֡������
									}
									else
									{
										TIMER_RxWindowAdd(RADIO->rx_window_add);
									}
								}
							}
							else	//��AckTable���м��
							{
								memcpy(tmp_buf, rx_payload.data+15+rx_payload.data[14], 16); 
								for(i = 0;i < 8;i++)
								{
									point_buf[i] = (uint16_t)((tmp_buf[i*2] << 0) | (tmp_buf[i*2+1] << 8));
								}
								point_bit = (point_buf[DTQ_NUM / 15 ] >> (DTQ_NUM % 15)) & 0x01;
								if(point_bit == 1)		//��ӦλΪ1��ʾ����Ҫ��������ͨ�ţ���������RX����
								{
									if(false == RADIO->rx_window_wait_data)
									{
										RADIO->rx_window_wait_data = true;
										if(110 > (rx_payload.data[9] + 5))
										{
											TIMER_WaitDataStart(110 - rx_payload.data[9] - 5);	//��5��ǰ��֡������
										}
										else
										{
											TIMER_RxWindowAdd(RADIO->rx_window_add);
										}
									}
								}
							}	
							break;  
						default: 
							radio_debug("rx data uid unmatch. \r\n");
							break;
							
					}
				}
			}	
		}
		else
		{
			radio_debug("rx data check err. \r\n");
		}
	}
}

/******************************************************************************
  @����:RADIO_TxSuccessHandler
  @����:
  @����:
  @���:
  @����:�ⲿ��
******************************************************************************/
void RADIO_TxSuccessHandler(void)
{
//	tmp_debug("[radio]transmit succeed \r\n");

	/* LCD��ʾ����OK����ʱ����ʱ�������ʾ */
	LCD_display_ok_failure(SEND_RESULT_OK);
	TIMER_TxResultStart();
	

}

/******************************************************************************
  @����:RADIO_TxFailedHandler
  @����:
  @����:
  @���:
  @����:�ⲿ��
******************************************************************************/
void RADIO_TxFailedHandler(void)
{
//	tmp_debug("[radio]transmit failed \r\n");

	/* LCD��ʾ����FAIL����ʱ����ʱ�������ʾ */
	LCD_display_ok_failure(SEND_RESULT_FAIL);
	TIMER_TxResultStart();
}


/******************************************************************************
  @����:my  _nrf_rx_success_handler
  @����:
  @����:
  @���:
  @����:�ⲿ��
******************************************************************************/
void RADIO_RxSuccessHandler(void)
{
	uint8_t i;
//	tmp_debug("rx_len:%d \r\n",rx_payload.length);
//	for(i = 0;i < rx_payload.length;i++)
//		tmp_debug("%02X ",rx_payload.data[i]);
//	tmp_debug("\r\n");
	
	/* ��ȡ2.4G���յ�����Ч���� */
	rf_var.rx_len = rx_payload.data[14];
	
	memcpy(rf_var.rx_buf, rx_payload.data+NRF_ACK_PROTOCOL_LEN-2, rf_var.rx_len);   
	
//	tmp_debug("rx_len:%d, ",rf_var.rx_len);
//	for(i = 0;i < rf_var.rx_len;i++)
//		tmp_debug("%02X ",rf_var.rx_buf[i]);
//	tmp_debug("\r\n");
	
	//��ͷ��β���У��ͨ��
	if( 0x5A == rf_var.rx_buf[0] && 
	    0xCA == rf_var.rx_buf[rf_var.rx_len - 1] &&
		XOR_Cal(rf_var.rx_buf + 1,rf_var.rx_len - 3) == rf_var.rx_buf[rf_var.rx_len - 2] )
	{
		rf_var.flag_rx_ok = true;
	}
}

/******************************************************************************
  @����:RADIO_ReturnAck
  @����:���������ʱ�ظ�ACK�Ķ�ʱ����
  @����:
  @���:
  @����:�ⲿ��
******************************************************************************/
void RADIO_ReturnAck(void)
{
	radio_debug("[radio]return ack \r\n");
	
	TRANSPORT.ack[1] = rx_payload.data[5];		//g_uid
	TRANSPORT.ack[2] = rx_payload.data[6];
	TRANSPORT.ack[3] = rx_payload.data[7];
	TRANSPORT.ack[4] = rx_payload.data[8];
	TRANSPORT.ack[5] = DTQ_UID[0];		//s_uid
	TRANSPORT.ack[6] = DTQ_UID[1];
	TRANSPORT.ack[7] = DTQ_UID[2];
	TRANSPORT.ack[8] = DTQ_UID[3];								
	TRANSPORT.ack[9] = rx_payload.data[9];		//���յ�sequenceԭ������ 
	TRANSPORT.ack[10] = rx_payload.data[10]; 	//���յ�pack_numԭ������
	
	TRANSPORT.ack[15] = XOR_Cal(TRANSPORT.ack+1,NRF_ACK_PROTOCOL_LEN-3);
	
	//��������ض�UID��������ACK
	if(stringcmp(rx_payload.data+1,DTQ_UID,4))
	{
		TIMER_RxWindowStop();				 //��������ʱ��RX���ڶ�ʱ���ر�
		TIMER_TxOvertimeStart(); 			 //���ͳ�ʱ��ʱ��
		my_esb_mode_change(NRF_ESB_MODE_PTX);
		TRANSPORT.sequence_re_tx_num = 1;
		
		memcpy(tx_payload.data,TRANSPORT.ack,NRF_ACK_PROTOCOL_LEN);
		tx_payload.length = NRF_ACK_PROTOCOL_LEN;
		tx_payload.pipe = NRF_PIPE;
		tx_payload.noack = true;
		nrf_esb_flush_tx();
		nrf_esb_write_payload(&tx_payload);	
	}
	else	//����㲥���ݣ�����ӳٺ�ظ�ack
	{
		
		TIMER_ReturnAckStart();
	}
}











