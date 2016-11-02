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


/* Private variables ---------------------------------------------------------*/
spi_cmd_type_t 					spi_cmd_type;
nrf_communication_t			nrf_communication;

/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
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

/** @} */



