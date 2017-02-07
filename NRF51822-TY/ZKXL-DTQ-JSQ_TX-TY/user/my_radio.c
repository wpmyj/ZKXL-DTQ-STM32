#include "my_radio.h"

//#define RADIO_DEBUG

#ifdef RADIO_DEBUG
#define radio_debug  printf   
#else  
#define radio_debug(...)                    
#endif 

nrf_esb_payload_t        tx_payload;
nrf_esb_payload_t        rx_payload;
bool tx_ing_flag = false;				//false:idle, true:busy 

uint8_t nrf_tx_num = 0 ;					// 2.4G 发送次数
uint8_t nrf_tx_delay = 0;				//重发延时，单位100us
uint8_t preamble_num = 0;				//前导帧序号
uint8_t sequence_re_tx_num = 0;

bool nrf_tx_flg = false;

void nrf_esb_event_handler(nrf_esb_evt_t const * p_event)
{
    switch (p_event->evt_id)
    {
        case NRF_ESB_EVENT_TX_SUCCESS:
//			nrf_gpio_pin_clear(RX_PIN_NUMBER_1);		//debug
			if(sequence_re_tx_num)					//加强帧
			{
				sequence_re_tx_num--;
				nrf_esb_write_payload(&tx_payload);
			}
			
            break;
        case NRF_ESB_EVENT_TX_FAILED:
//			nrf_tx_num--;
//			if(0 == nrf_tx_num)
//			{
//				tx_ing_flag = false;
//			}
            break;
        case NRF_ESB_EVENT_RX_RECEIVED:
            if(nrf_esb_read_rx_payload(&rx_payload) == NRF_SUCCESS)
            {
				nrf_esb_flush_rx();
//				radio_debug("rx success \r");
//				for(uint8_t i = 0;i < rx_payload.length;i++)
//				{
//					radio_debug("%02X ",rx_payload.data[i]);
//				}radio_debug("...\r\n");
			}
            break;
    }
}

uint32_t my_tx_esb_init(void)
{
    uint32_t err_code;
    uint8_t base_addr_0[4] = {0xE7, 0xE7, 0xE7, 0xE7};
    uint8_t addr_prefix[8] = {0xE7, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8 };

    nrf_esb_config_t nrf_esb_config         = NRF_ESB_DEFAULT_CONFIG;
    nrf_esb_config.protocol                 = NRF_ESB_PROTOCOL_ESB_DPL;
    nrf_esb_config.retransmit_delay         = 600;
    nrf_esb_config.bitrate                  = NRF_ESB_BITRATE_1MBPS;
    nrf_esb_config.event_handler            = nrf_esb_event_handler;
    nrf_esb_config.mode                     = NRF_ESB_MODE_PTX;
	nrf_esb_config.selective_auto_ack       = true;	//lj 理解，等于true时，每次发送的时候可选择回不回复ACK
	nrf_esb_config.payload_length           = 250;
	
	err_code = nrf_esb_set_rf_channel(4);		//注意：答题器发送频点61接收频点21，接收器相反
	VERIFY_SUCCESS(err_code);
	
    err_code = nrf_esb_init(&nrf_esb_config);
    VERIFY_SUCCESS(err_code);

    err_code = nrf_esb_set_base_address_0(base_addr_0);
    VERIFY_SUCCESS(err_code);
	
    err_code = nrf_esb_set_prefixes(addr_prefix, 8);
    VERIFY_SUCCESS(err_code);

    return err_code;
}

uint32_t my_rx_esb_init(void)
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

	err_code = nrf_esb_set_rf_channel(2);		//注意：答题器发送频点61接收频点21，接收器相反
	VERIFY_SUCCESS(err_code);
	
	err_code = nrf_esb_init(&nrf_esb_config);
	VERIFY_SUCCESS(err_code);

	err_code = nrf_esb_set_base_address_0(base_addr_0);
	VERIFY_SUCCESS(err_code);

	err_code = nrf_esb_set_prefixes(addr_prefix, 8);
	VERIFY_SUCCESS(err_code);

	return err_code;
}














