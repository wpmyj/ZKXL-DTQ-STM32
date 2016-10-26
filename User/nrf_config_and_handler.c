/** @file nrf_config_and_handler.c
 *
 * @author
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2012 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2016.06.07
 * $Revision: 	V1000
 *
 */

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "nrf.h"

/* Private define ------------------------------------------------------------*/
#define DELAY_FUNC			(Delay10us)
#define DELAY_TIME			(500)

/* Private variables ---------------------------------------------------------*/
volatile nrf_config_t		m_nrf_config;
spi_cmd_type_t 					spi_cmd_type;
volatile uint8_t				flag_nrf_int = 0;
nrf_communication_t			nrf_communication;
uint8_t 					      dtq_to_jsq_sequence;
uint8_t 					      dtq_to_jsq_packnum;
uint8_t 					      jsq_to_dtq_sequence;
uint8_t 					      jsq_to_dtq_packnum;

/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value);



static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
}

void config_param_init(void)
{
	m_nrf_config.power_mode 							= UESB_MODE_POWERUP;
	m_nrf_config.tx_or_rx_mode 							= UESB_MODE_IDLE;
	m_nrf_config.enable_ack 							= 1;
	m_nrf_config.reserved 								= 0;

    // General RF parameters
	m_nrf_config.config_param.bitrate 					= RADIO_MODE_MODE_Nrf_1Mbit;
	m_nrf_config.config_param.crc 						= RADIO_CRCCNF_LEN_Two;
	m_nrf_config.config_param.rf_channel 				= 45;
	m_nrf_config.config_param.payload_length			= 32;
	m_nrf_config.config_param.rf_addr_length			= 5;

	m_nrf_config.config_param.tx_output_power			= RADIO_TXPOWER_TXPOWER_Pos4dBm;
	memset((uint8_t *)m_nrf_config.config_param.tx_address, 0x00, 5);
	memset((uint8_t *)m_nrf_config.config_param.rx_address_p0, 0x00, 5);
	memset((uint8_t *)m_nrf_config.config_param.rx_address_p1, 0x00, 5);


	m_nrf_config.config_param.rx_address_p2				= 0x00;
	m_nrf_config.config_param.rx_address_p3				= 0x00;
	m_nrf_config.config_param.rx_address_p4				= 0x00;
	m_nrf_config.config_param.rx_address_p5				= 0x00;
	m_nrf_config.config_param.rx_address_p6				= 0x00;
	m_nrf_config.config_param.rx_address_p7				= 0x00;
	m_nrf_config.config_param.rx_pipes_enabled			= 0x01;

	m_nrf_config.config_param.dynamic_payload_length_enabled = 1;
	m_nrf_config.config_param.dynamic_ack_enabled		= 1;
	m_nrf_config.config_param.retransmit_delay			= 500;
	m_nrf_config.config_param.retransmit_count			= 15;
}

uint8_t uesb_nrf_write_config_parameter(void)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;

	spi_cmd_type.spi_cmd = UESB_WRITE_PARAM;
	spi_cmd_type.data_len = sizeof(nrf_config_t);
	memcpy(spi_cmd_type.data, (const uint8_t *)&m_nrf_config, spi_cmd_type.data_len);
	spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&spi_cmd_type, spi_cmd_type.data_len + 2);
	temp_data = (uint8_t *)&spi_cmd_type;

	CSN_LOW_2();
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPI1, *(temp_data+i));
	}
	CSN_HIGH_2();

	if(retval[0] != 0) 									//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uesb_nrf_switch_tx_rx(uesb_mode_t uesb_mode)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;

	spi_cmd_type.spi_cmd = UESB_SWITCH_TX_RX;
	spi_cmd_type.data_len = 0x02;
	spi_cmd_type.data[0] = 0x00;
	spi_cmd_type.data[1] = uesb_mode;
	if(UESB_MODE_PTX == uesb_mode)
	{
		spi_cmd_type.data[spi_cmd_type.data_len] = 0x8A;						//提前计算好，节省异或校验时间
	}
	else
	{
		spi_cmd_type.data[spi_cmd_type.data_len] = 0x8B; 						//提前计算好，节省异或校验时间
	}
//	spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&spi_cmd_type, spi_cmd_type.data_len + 2);
	temp_data = (uint8_t *)&spi_cmd_type;

	CSN_LOW_2();
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPI1, *(temp_data+i));
	}
	CSN_HIGH_2();

	if(retval[0] != 0) 									//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uesb_nrf_get_irq_flags(SPI_TypeDef* SPIx, uint8_t *flags, uint8_t *rx_data_len, uint8_t *rx_data)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;

	*rx_data_len = 0;
	memset(spi_cmd_type.data, 0xFF, BUFFER_SIZE_MAX);
	spi_cmd_type.spi_cmd = UESB_READ_RF_INT_STATUS;
	spi_cmd_type.data_len = 0x02;
	spi_cmd_type.data[0] = 0xFF;
	spi_cmd_type.data[1] = 0xFF;
	temp_data = (uint8_t *)&spi_cmd_type;

	CSN_LOW();	//开始SPI传输
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPIx, *(temp_data+i));
		//printf("%2x ",retval[i]);
		if(i ==  2 && retval[2] != 0x00 && retval[2] != 0xFF)
		{
			*flags = retval[2];
		}

		if( i == 3 && (retval[2] & (1<<RX_DR)) && retval[3] < BUFFER_SIZE_MAX )				// If "received data ready" interrupt from hrf
	  	{
			*rx_data_len = retval[3];
			spi_cmd_type.data_len += *rx_data_len;
			spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&spi_cmd_type.data[3], spi_cmd_type.data_len - 3);
		}
	}
	//printf("\r\n");
	CSN_HIGH();	//关闭SPI传输

	memcpy(rx_data, &retval[4],*rx_data_len);
	//DELAY_FUNC(DELAY_TIME);

	if(retval[0] != 0 && retval[0] != 0xFF) 			//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uesb_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint16_t i = 0;
	uint8_t *temp_data = NULL;

	spi_cmd_type.spi_cmd = UESB_WRITE_TX_PAYLOAD;
	spi_cmd_type.data_len = length+2;
	spi_cmd_type.count = count;
	spi_cmd_type.delay100us = delay100us;

	memcpy(spi_cmd_type.data, tx_pload, length);
	spi_cmd_type.data[spi_cmd_type.data_len-2] = XOR_Cal((uint8_t *)&spi_cmd_type, spi_cmd_type.data_len+2);
	temp_data = (uint8_t *)&spi_cmd_type;

	CSN_LOW_2();	//开始SPI传输
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPI1, *(temp_data+i));
	}
	CSN_HIGH_2();	//关闭SPI传输

	if(retval[0] != 0) 									//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uesb_nrf_write_tx_payload_noack(const uint8_t *tx_pload, uint8_t length)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;

	spi_cmd_type.spi_cmd = UESB_WRITE_TX_PAYLOAD_NOACK;
	spi_cmd_type.data_len = length;
	memcpy(spi_cmd_type.data, tx_pload, length);
	spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&spi_cmd_type, spi_cmd_type.data_len + 2);
	temp_data = (uint8_t *)&spi_cmd_type;

	CSN_LOW_2();
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPI1, *(temp_data+i));
	}
	CSN_HIGH_2();

	if(retval[0] != 0) 									//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void nrf51822_parameters_init(void)
{
	uint8_t i;
	for(i = 0;i <nrf_communication.transmit_len;i++)
	{
		nrf_communication.transmit_buf[i] = 0x00;
	}

	dtq_to_jsq_sequence = 0;
	jsq_to_dtq_sequence = 0;
	dtq_to_jsq_packnum  = 0;
	jsq_to_dtq_packnum  = 0;

	nrf_communication.sequence = 0;					//初始化发送数据编号为0，每发送一次+1
	nrf_communication.number_of_retransmits = 0;
	nrf_communication.transmit_ing_flag = false;
	nrf_communication.transmit_ok_flag = false;
	nrf_communication.dtq_uid[0] = 0;
	nrf_communication.dtq_uid[1] = 0;
	nrf_communication.dtq_uid[2] = 0;
	nrf_communication.dtq_uid[3] = 0;

	nrf_communication.jsq_uid[0] = (jsq_uid[1]&0x0F)|((jsq_uid[0]<<4)&0xF0);
	nrf_communication.jsq_uid[1] = (jsq_uid[3]&0x0F)|((jsq_uid[2]<<4)&0xF0);
	nrf_communication.jsq_uid[2] = (jsq_uid[5]&0x0F)|((jsq_uid[4]<<4)&0xF0);
	nrf_communication.jsq_uid[3] = (jsq_uid[7]&0x0F)|((jsq_uid[6]<<4)&0xF0);

	//返回ACK结构体，固定参数定义，
	nrf_communication.software_ack_len = NRF_TOTAL_DATA_LEN;
	memcpy((nrf_communication.software_ack_buf + 1), nrf_communication.jsq_uid, 4);
	nrf_communication.software_ack_buf[5] = 1;	//buf[5]=1表示发送的数据是ACK
	nrf_communication.software_ack_buf[5] = NRF_USEFUL_DATA_LEN;

	//发送数据结构体，固定参数定义，
	nrf_communication.transmit_len = NRF_TOTAL_DATA_LEN;				//序号
	memcpy((nrf_communication.transmit_buf + 1), nrf_communication.jsq_uid, 4);
	nrf_communication.transmit_buf[5] = 0;										//buf[5]=0表示发送是有效数据
	nrf_communication.transmit_buf[5] = NRF_USEFUL_DATA_LEN;
}

/** @} */
/** @} */



