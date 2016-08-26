/**
  ******************************************************************************
  * @file   	hal_nrf.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	hal function for nrf moulde
  ******************************************************************************
  */
 
#include "main.h"

//#define USE_NRF2			(1)

/*******************************************************************************
  * @brief  双频点，一收一发
  * @param  None
  * @retval None
*******************************************************************************/
void NRF_Module_Set(void)
{
	bool gbf_nrf1_can_be_used = FALSE;
	bool gbf_nrf2_can_be_used = FALSE;

	nrf_parameter.power = HAL_NRF_0DBM;
	nrf_parameter.datarate = HAL_NRF_1MBPS;	
	nrf_parameter.crc_length = HAL_NRF_CRC_16BIT;	
	nrf_parameter.base_address_length = HAL_NRF_AW_5BYTES;	
	nrf_parameter.channel = 30;
	nrf_parameter.pipe = HAL_NRF_PIPE0;
	nrf_parameter.attempts = 3;
	nrf_parameter.prefix_address = 0xAA;
	nrf_parameter.base_address_high = 0xBB00;
	nrf_parameter.base_address_low = 0x0001;
	nrf_parameter.delay_us = 500;
		
	SPI_Init_NRF1();
	hal_nrf_write_reg(EN_RXADDR, 0x3F);
	Delay1Ms();
	if(hal_nrf_read_reg(EN_RXADDR) == 0x3F)
		gbf_nrf1_can_be_used = TRUE;
	if(gbf_nrf1_can_be_used)
		nrf24InitConf_NRF1();	

#ifdef	USE_NRF2	
	SPI_Init_NRF2();	
	hal_nrf_write_reg_2(EN_RXADDR, 0x3F);
	Delay1Ms();
	if(hal_nrf_read_reg_2(EN_RXADDR) == 0x3F)
		gbf_nrf2_can_be_used = TRUE;
	if(gbf_nrf2_can_be_used)		
		nrf24InitConf_NRF2_SendMode();
#endif //USE_NRF2
	
	DebugLog("\r\n===========================================================================\r\n");
	if((gbf_nrf1_can_be_used) && (gbf_nrf2_can_be_used == FALSE))
		DebugLog("[Radio]:nrf1 ok and nrf2 not ok\r\n");
	if((gbf_nrf1_can_be_used == FALSE) && (gbf_nrf2_can_be_used))
		DebugLog("[Radio]:nrf1 not ok and nrf2 ok\r\n");
	if((gbf_nrf1_can_be_used) && (gbf_nrf2_can_be_used))
		DebugLog("[Radio]:nrf1 ok and nrf2 ok\r\n");
	if((gbf_nrf1_can_be_used == FALSE) && (gbf_nrf2_can_be_used == FALSE))
		DebugLog("[Radio]:nrf1 not ok and nrf2 not ok\r\n");
	DebugLog("===========================================================================\r\n");
}

void nrf24AllRegResetVal_nrf1(void)
{
	CE_LOW();
	hal_nrf_set_power_mode(HAL_NRF_PWR_DOWN);
	hal_nrf_flush_rx();
	hal_nrf_flush_tx();
	hal_nrf_write_reg(CONFIG, 0x08);
	hal_nrf_write_reg(EN_AA, 0x3F);
	hal_nrf_write_reg(EN_RXADDR, 0x03);
	hal_nrf_write_reg(SETUP_AW, 0x03);
	hal_nrf_write_reg(SETUP_RETR, 0x03);
	hal_nrf_write_reg(RF_CH, 0x02);
	hal_nrf_write_reg(RF_SETUP, 0x0E);
	hal_nrf_write_reg(STATUS_2, 0x0E);
	hal_nrf_write_reg(OBSERVE_TX, 0x00);
	hal_nrf_write_reg(CD, 0x00);
	hal_nrf_write_reg(RX_PW_P0, 0x00);
	hal_nrf_write_reg(RX_PW_P1, 0x00);
	hal_nrf_write_reg(RX_PW_P2, 0x00);
	hal_nrf_write_reg(RX_PW_P3, 0x00);
	hal_nrf_write_reg(RX_PW_P4, 0x00);
	hal_nrf_write_reg(RX_PW_P5, 0x00);
	hal_nrf_write_reg(DYNPD_2, 0x00);
	hal_nrf_write_reg(FEATURE_2, 0x00);
	CE_HIGH();
}

void nrf24InitConf_NRF1( void )
{
	uint8_t rx_address_buf[RX_ADR_WIDTH];
	uint8_t act[1]={0x73};
	CE_LOW();
	nrf24AllRegResetVal_nrf1();
	hal_nrf_set_power_mode(HAL_NRF_PWR_DOWN);
		
	rx_address_buf[0] = nrf_parameter.prefix_address;
	rx_address_buf[1] = nrf_parameter.base_address_high >> 8;
	rx_address_buf[2] = nrf_parameter.base_address_high >> 0; 
	rx_address_buf[3] = nrf_parameter.base_address_low >> 8;
	rx_address_buf[4] = nrf_parameter.base_address_low >> 0;
		
	

	hal_nrf_flush_rx();
	hal_nrf_flush_tx();
	hal_nrf_enable_dynamic_ack(FALSE);
	hal_nrf_enable_ack_payload(TRUE);
	hal_nrf_enable_dynamic_payload(TRUE);
	hal_nrf_setup_dynamic_payload(0x01);

	hal_nrf_set_datarate(nrf_parameter.datarate);
	hal_nrf_set_output_power(nrf_parameter.power);
	hal_nrf_set_rf_channel(nrf_parameter.channel);
	hal_nrf_set_address_width(nrf_parameter.base_address_length);
	hal_nrf_set_address((hal_nrf_address_t)nrf_parameter.pipe, rx_address_buf);
	hal_nrf_set_address(HAL_NRF_TX, rx_address_buf);		
	hal_nrf_open_pipe((hal_nrf_address_t)nrf_parameter.pipe, TRUE);
	hal_nrf_set_crc_mode(nrf_parameter.crc_length);	
	hal_nrf_set_auto_retr(nrf_parameter.attempts, nrf_parameter.delay_us);	
	
	hal_nrf_write_multibyte_reg(ACTIVATE,act,1);	
	
	hal_nrf_get_clear_irq_flags();	
	hal_nrf_set_operation_mode(HAL_NRF_PRX);
	hal_nrf_set_power_mode(HAL_NRF_PWR_UP);		
	Delay1Ms();
	CE_HIGH();
	Delay1Ms();
	
	//输出参数信息
	DebugLog("[Radio]:nrf parameter:\r\n");
	DebugLog("[Radio]:output power		%d dB\r\n", (nrf_parameter.power - 3)*6);
	DebugLog("[Radio]:channel			%d\r\n", nrf_parameter.channel);
	if(HAL_NRF_250KBPS == nrf_parameter.datarate)
		DebugLog("[Radio]:datarate		250 Kbps\r\n");
	else
		DebugLog("[Radio]:datarate		%d Mbps\r\n", nrf_parameter.datarate + 1);
	DebugLog("[Radio]:address			%02X%04X%04X\r\n", nrf_parameter.prefix_address, nrf_parameter.base_address_high,  nrf_parameter.base_address_low);
	DebugLog("[Radio]:crc length		%d\r\n", nrf_parameter.crc_length);
	DebugLog("[Radio]:retransmit delay	%d us\r\n", nrf_parameter.delay_us);
	DebugLog("[Radio]:max tx_attempts		%d\r\n", nrf_parameter.attempts);
}
void nrf24AddtoAck(const uint8_t *puBuf, uint8_t uWidth)
{
	hal_nrf_flush_rx();
	hal_nrf_flush_tx();
	hal_nrf_write_multibyte_reg(W_ACK_PAYLOAD |HAL_NRF_PIPE0, puBuf, uWidth);
}
void nrf24SendPacket(const uint8_t *puBuf, uint8_t uWidth)
{
	uint8_t uSendTimeOutCnt = 0;
	hal_nrf_write_multibyte_reg(W_TX_PAYLOAD_NOACK, puBuf, uWidth);
	CE_HIGH();
	Delay1Ms();
	CE_LOW();
	while(!((hal_nrf_nop() & (BIT_6|BIT_5|BIT_4)) & (1 << TX_DS)) && (uSendTimeOutCnt++ <= 3))
	{
		Delay1Ms();
	}
	hal_nrf_write_reg(STATUS_2, 0x70);
}

#ifdef	USE_NRF2	
void nrf24AllRegResetVal_nrf2(void)
{
	CE_LOW_2();
	hal_nrf_set_power_mode_2(HAL_NRF_PWR_DOWN);
	hal_nrf_flush_rx_2();
	hal_nrf_flush_tx_2();
	hal_nrf_write_reg_2(CONFIG, 0x08);
	hal_nrf_write_reg_2(EN_AA, 0x3F);
	hal_nrf_write_reg_2(EN_RXADDR, 0x03);
	hal_nrf_write_reg_2(SETUP_AW, 0x03);
	hal_nrf_write_reg_2(SETUP_RETR, 0x03);
	hal_nrf_write_reg_2(RF_CH, 0x02);
	hal_nrf_write_reg_2(RF_SETUP, 0x0E);
	hal_nrf_write_reg_2(STATUS_2, 0x0E);
	hal_nrf_write_reg_2(OBSERVE_TX, 0x00);
	hal_nrf_write_reg_2(CD, 0x00);
	hal_nrf_write_reg_2(RX_PW_P0, 0x00);
	hal_nrf_write_reg_2(RX_PW_P1, 0x00);
	hal_nrf_write_reg_2(RX_PW_P2, 0x00);
	hal_nrf_write_reg_2(RX_PW_P3, 0x00);
	hal_nrf_write_reg_2(RX_PW_P4, 0x00);
	hal_nrf_write_reg_2(RX_PW_P5, 0x00);
	hal_nrf_write_reg_2(DYNPD_2, 0x00);
	hal_nrf_write_reg_2(FEATURE_2, 0x00);
	CE_HIGH_2();
}

void nrf24InitConf_NRF2( void )
{
	uint8_t rx_address_buf[RX_ADR_WIDTH] = {0xAA,0xBB,0x00,0x00,0x02};
	CE_LOW_2();
	nrf24AllRegResetVal_nrf2();
	hal_nrf_set_power_mode_2(HAL_NRF_PWR_DOWN);

	hal_nrf_flush_rx_2();
	hal_nrf_flush_tx_2();
	hal_nrf_enable_dynamic_ack_2(FALSE);
	hal_nrf_enable_ack_payload_2(FALSE);
	hal_nrf_enable_dynamic_payload_2(TRUE);
	hal_nrf_setup_dynamic_payload_2(0x01);
	
	//有源卡模式设置
	hal_nrf_set_datarate_2(HAL_NRF_1MBPS);
	hal_nrf_set_rf_channel_2(RF_CHANNEL_NRF2);
	hal_nrf_set_address_2(HAL_NRF_PIPE0, rx_address_buf);
	hal_nrf_set_address_2(HAL_NRF_TX, rx_address_buf);
 
	hal_nrf_set_output_power_2(HAL_NRF_0DBM);
	hal_nrf_set_address_width_2(HAL_NRF_AW_5BYTES);
	hal_nrf_open_pipe_2(HAL_NRF_PIPE0, TRUE);
	hal_nrf_set_auto_retr_2(3,500);
	hal_nrf_get_clear_irq_flags_2();
	hal_nrf_set_crc_mode_2(HAL_NRF_CRC_16BIT);
	hal_nrf_set_operation_mode_2(HAL_NRF_PRX);
	hal_nrf_set_power_mode_2(HAL_NRF_PWR_UP);
	Delay1Ms();
	CE_HIGH_2();
	Delay1Ms();
}

void nrf24InitConf_NRF2_SendMode(void)
{
	uint8_t tx_address_buf[TX_ADR_WIDTH] = {0xAA,0xBB,0x00,0x00,0x02};
	
	CE_LOW_2();
	nrf24AllRegResetVal_nrf2();
	hal_nrf_set_power_mode_2(HAL_NRF_PWR_DOWN);

	hal_nrf_flush_rx_2();
	hal_nrf_flush_tx_2();
	hal_nrf_write_reg_2(RF_SETUP, 0x07);
	hal_nrf_enable_dynamic_ack_2(FALSE);
	hal_nrf_enable_ack_payload_2(FALSE);
	hal_nrf_enable_dynamic_payload_2(TRUE);
	hal_nrf_setup_dynamic_payload_2(0x01);
	hal_nrf_set_datarate_2(HAL_NRF_1MBPS);
	hal_nrf_set_output_power_2(HAL_NRF_0DBM);
	hal_nrf_set_rf_channel_2(8);
	hal_nrf_set_address_width_2(HAL_NRF_AW_5BYTES);
	hal_nrf_set_address_2(HAL_NRF_PIPE0, tx_address_buf);
	hal_nrf_set_address_2(HAL_NRF_TX, tx_address_buf);
	hal_nrf_open_pipe_2(HAL_NRF_PIPE0, true);
	hal_nrf_set_auto_retr_2(3,500);
	
	hal_nrf_get_clear_irq_flags_2();
	hal_nrf_set_crc_mode_2(HAL_NRF_CRC_16BIT);
	hal_nrf_set_operation_mode_2(HAL_NRF_PTX);
	hal_nrf_set_power_mode_2(HAL_NRF_PWR_UP);
	Delay1Ms();
	CE_HIGH_2();
	Delay1Ms();
}

void nrf24SendPacket_NRF2(const uint8_t *puBuf, uint8_t uWidth)
{
	uint8_t uSendTimeOutCnt = 0;
	hal_nrf_write_multibyte_reg_2(W_TX_PAYLOAD_NOACK, puBuf, uWidth);
	
	CE_HIGH_2();
	Delay1Ms();
	CE_LOW_2();
	
	while(!((hal_nrf_nop_2() & (BIT_6|BIT_5|BIT_4)) & (1 << TX_DS)) && (uSendTimeOutCnt++ <= 3))
	{
		Delay1Ms();
	}
	hal_nrf_write_reg_2(STATUS_2, 0x70);
}
#endif //USE_NRF2


/**
  * @}
  */
/**************************************END OF FILE****************************/


