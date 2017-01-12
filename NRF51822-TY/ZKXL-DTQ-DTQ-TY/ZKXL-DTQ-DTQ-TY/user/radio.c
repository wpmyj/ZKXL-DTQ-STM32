

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
static uint32_t radio_config(void);
static void radio_wakeup(void);
static void radio_sleep(void);

/*----------------------------------------------------------------------------------------*/

void RADIO_init(void)
{
	ret_code_t err_code;
	
    err_code = radio_config();
    APP_ERROR_CHECK(err_code);
	
	RADIO->rx_window_on_flg = false;
	RADIO->rx_window_add_flg = false;
	RADIO->rx_window_wait_data = false;
	
	RADIO->rx_window_on = RX_WINDOW_ON;
	RADIO->rx_window_off = RX_WINDOW_OFF;
	RADIO->rx_window_add = RX_WINDOW_ADD_WAIT_DATA;
	RADIO->ack_delay = RETURN_ACK_DELAY;
	
	RADIO->wakeup       = radio_wakeup;
	RADIO->sleep        = radio_sleep;
	RADIO->rx_data		= nrf_rx_data_handler;
	RADIO->return_ack	= my_nrf_return_ack;
	RADIO->rx_success	= my_nrf_rx_success_handler;
	RADIO->tx_success	= my_nrf_tx_success_handler;
	RADIO->tx_failed	= my_nrf_tx_failed_handler;
}


void radio_wakeup(void)
{
	RADIO->rx_window_on_flg = false;
	TIMER->rx_window_start();		//接收时间窗定时器打开
}

void radio_sleep(void)
{
	TIMER->rx_window_stop();	//接收时间窗定时器停止
	nrf_esb_stop_rx();			//停止接收
	
}

/*****************************************************************************
  @函数:nrf_esb_event_handler
  @描述:2.4G数据收发数据回调函数
  @输入:
  @输出:
  @调用:
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
				TIMER->tx_overtime_stop();
				
				if(TRANSPORT.tx_ing_flag)	//如果在发送数据，增加RX窗口，等待"ACK"
				{
					logic_high();
					TIMER->rx_window_add(RX_WINDOW_ADD_WAIT_ACK);
				}			
				else
					TIMER->rx_window_start();
			}
            break;
        case NRF_ESB_EVENT_TX_FAILED:
			TIMER->tx_overtime_stop();
			TIMER->rx_window_start();
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            break;
    }
}

/******************************************************************************
  @函数:radio_config
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
static uint32_t radio_config(void)
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
	
	err_code = nrf_esb_set_rf_channel(NRF_RX_CHANNEL);		//注意：答题器发送频点2接收频点122，接收器相反
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
  @函数:nrf_start_transmit
  @描述:自定义通信层开始发送数据，
  @输入:
  @输出:
  @调用:
******************************************************************************/
void nrf_start_transmit(void)
{
	uint8_t i;
	
	/* 每次发送前提取要发送的数据，保证tx_payload.data结构体中的数据是最新的 */
	tx_payload.length = TRANSPORT.data_len;
	tx_payload.pipe    = NRF_PIPE;
	tx_payload.noack = true;
	memcpy(tx_payload.data, TRANSPORT.data, tx_payload.length);
	
//	tmp_debug("tx_len:%d \r\n",tx_payload.length);
//	for(i = 0;i < tx_payload.length;i++)
//		tmp_debug("%02X ",tx_payload.data[i]);
//	tmp_debug("\r\n");
	
	/* 启动发送配置 */
	TRANSPORT.sequence_re_tx_num = 1;
	TIMER->rx_window_stop();	 			
	TIMER->tx_overtime_start(); 			 
	my_esb_mode_change(NRF_ESB_MODE_PTX);
	nrf_esb_flush_tx();
	nrf_esb_write_payload(&tx_payload);		
	
	/* 首次发送，开启重发定时器 */
	if (false == TRANSPORT.tx_ing_flag)
	{
		APP->key_send_allow_flg = false;
		TRANSPORT.tx_ing_flag = true;
		TIMER->retransmit_start();				//启动重发定时器
	}
}

/******************************************************************************
  @函数:nrf_rx_data_handler
  @描述:2.4G收到数据无条件缓存，此函数对缓存的数据进行处理
  @输入:
  @输出:
  @调用:外部，
******************************************************************************/
void nrf_rx_data_handler(void)
{
	uint8_t i;
	static uint8_t last_rx_sequence = 0xFF;
	static uint8_t last_rx_pack_num = 0xFF;
	uint8_t tmp_buf[16]; 				//解析前导帧的回应位
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
		
			/* 包头、包尾、异或校验 */
		if(rx_payload.data[0]					  == NRF_DATA_HEAD &&
		   rx_payload.data[rx_payload.length-1] == NRF_DATA_END  &&
		   rx_payload.data[rx_payload.length-2] == XOR_Cal(rx_payload.data+1,(rx_payload.length-3)))
		{
			//是否是已配对的接收器发来的
			if(stringcmp(rx_payload.data+5,JSQ_UID,4))
			{
				/* 发给特定uid或广播 */
				if(stringcmp(rx_payload.data+1,PUBLIC_UID, 4) || stringcmp(rx_payload.data+1,DTQ_UID,4))
				{
					switch(rx_payload.data[11])
					{
						case NRF_DATA_IS_USE:
							RADIO->rx_window_wait_data = false;
							if(rx_payload.data[10] != last_rx_pack_num)		//第一次收到此包号
							{
								radio_debug("radio:first rx data,return ack. \r\n");
								last_rx_sequence = rx_payload.data[9];
								last_rx_pack_num = rx_payload.data[10];
								//判断是否需要回ack
								memcpy(tmp_buf, rx_payload.data+15+rx_payload.data[14], 16); 
								for(i = 0;i < 8;i++)
								{
									point_buf[i] = (uint16_t)((tmp_buf[i*2] << 0) | (tmp_buf[i*2+1] << 8));
								}
								point_bit = (point_buf[DTQ_NUM / 15 ] >> (DTQ_NUM % 15)) & 0x01;
								if(point_bit == 0)
									my_nrf_return_ack();	
								RADIO->rx_success();
							}
							else												//重复接收的包号
							{
								if(rx_payload.data[9] != last_rx_sequence)	//不同帧号，回复ACK
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
										my_nrf_return_ack();		
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
							LCD->student_id(rx_payload.rssi & 0x7f);
							break;
						case NRF_DATA_IS_PRE:
							//对AckTable进行检测
							memcpy(tmp_buf, rx_payload.data+15+rx_payload.data[14], 16); 
							for(i = 0;i < 8;i++)
							{
								point_buf[i] = (uint16_t)((tmp_buf[i*2] << 0) | (tmp_buf[i*2+1] << 8));
							}
							point_bit = (point_buf[DTQ_NUM / 15 ] >> (DTQ_NUM % 15)) & 0x01;
							if(point_bit == 1)		//对应位为1表示即将要跟答题器通信，答题器把RX窗打开
							{
								if(false == RADIO->rx_window_wait_data)
								{
									RADIO->rx_window_wait_data = true;
									if(110 > (rx_payload.data[9] + 5))
									{
										TIMER->wait_data_start(110 - rx_payload.data[9] - 5);	//留5个前导帧的余量
									}
									else
									{
										TIMER->rx_window_add(RADIO->rx_window_add);
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
  @函数:my_nrf_tx_success_handler
  @描述:
  @输入:
  @输出:
  @调用:外部，
******************************************************************************/
void my_nrf_tx_success_handler(void)
{
//	tmp_debug("[radio]transmit succeed \r\n");

	/* LCD显示发送OK，定时器超时后清除显示 */
	LCD->send_result(SEND_RESULT_OK);
	TIMER->transmit_result_start();
	

}

/******************************************************************************
  @函数:my_nrf_tx_failed_handler
  @描述:
  @输入:
  @输出:
  @调用:外部，
******************************************************************************/
void my_nrf_tx_failed_handler(void)
{
//	tmp_debug("[radio]transmit failed \r\n");

	/* LCD显示发送FAIL，定时器超时后清除显示 */
	LCD->send_result(SEND_RESULT_FAIL);
	TIMER->transmit_result_start();
}


/******************************************************************************
  @函数:my  _nrf_rx_success_handler
  @描述:
  @输入:
  @输出:
  @调用:外部，
******************************************************************************/
void my_nrf_rx_success_handler(void)
{
	uint8_t i;
	rf_var.flag_rx_ok = true;
//	tmp_debug("rx_len:%d \r\n",rx_payload.length);
//	for(i = 0;i < rx_payload.length;i++)
//		tmp_debug("%02X ",rx_payload.data[i]);
//	tmp_debug("\r\n");
	
	/* 提取2.4G接收到的有效数据 */
	rf_var.rx_len = rx_payload.data[14];
	memcpy(rf_var.rx_buf, rx_payload.data+NRF_ACK_PROTOCOL_LEN-2, rf_var.rx_len);   
	
//	tmp_debug("rx_len:%d, ",rf_var.rx_len);
//	for(i = 0;i < rf_var.rx_len;i++)
//		tmp_debug("%02X ",rf_var.rx_buf[i]);
//	tmp_debug("\r\n");
	
	
	if(stringcmp(&rf_var.rx_buf[1],DTQ_UID, 4) || stringcmp(&rf_var.rx_buf[1],PUBLIC_UID, 4))	
	{
		if(rf_var.rx_buf[6] == 0x00)			//收到空包应答
		{
			rf_var.rf_is_ok = true;
		}
		else									//收到下发数据包
		{
			rf_var.flag_rx_ok = true;
		}
	}
}

/******************************************************************************
  @函数:my_nrf_return_ack
  @描述:开启随机延时回复ACK的定时器，
  @输入:
  @输出:
  @调用:外部，
******************************************************************************/
void my_nrf_return_ack(void)
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
	TRANSPORT.ack[9] = rx_payload.data[9];		//接收的sequence原样返回 
	TRANSPORT.ack[10] = rx_payload.data[10]; 	//接收的pack_num原样返回
	
	TRANSPORT.ack[15] = XOR_Cal(TRANSPORT.ack+1,NRF_ACK_PROTOCOL_LEN-3);
	
	//如果发给特定UID，立即回ACK
	if(stringcmp(rx_payload.data+1,DTQ_UID,4))
	{
		TIMER->rx_window_stop();				 //发送数据时把RX窗口定时器关闭
		TIMER->tx_overtime_start(); 			 //发送超时定时器
		my_esb_mode_change(NRF_ESB_MODE_PTX);
		TRANSPORT.sequence_re_tx_num = 1;
		
		memcpy(tx_payload.data,TRANSPORT.ack,NRF_ACK_PROTOCOL_LEN);
		tx_payload.length = NRF_ACK_PROTOCOL_LEN;
		tx_payload.pipe = NRF_PIPE;
		tx_payload.noack = true;
		nrf_esb_flush_tx();
		nrf_esb_write_payload(&tx_payload);	
	}
	else	//如果广播数据，随机延迟后回复ack
	{
		
		TIMER->return_ack_start();
	}
}











