/**
  ******************************************************************************
  * @file   	hal_nrf.c
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	24-5-2013
  * @brief   	
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "hal_nrf.h"

/* Private Functions -------------------------------------------------------- */
/* Private Functions -------------------------------------------------------- */

/*******************************************************************************
  * @brief  Set radio's operation mode
  * @param  op_mode: 
				RX or TX 
  *     @arg HAL_NRF_PTX
  *     @arg HAL_NRF_PRX
  * @retval None
*******************************************************************************/
void hal_nrf_set_operation_mode_3(hal_nrf_operation_mode_t op_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg_3(CONFIG);

	if (op_mode == HAL_NRF_PRX)
	{
		config.bits.prim_rx = 1;
	}
	else
	{
		config.bits.prim_rx = 0;
	}

	hal_nrf_write_reg_3(CONFIG, config.value);
}

/*******************************************************************************
  * @brief  Set radio's power mode
  * @param  pwr_mode: 
  *				power_up or power_down radio
  *     @arg HAL_NRF_PWR_DOWN
  *     @arg HAL_NRF_PWR_UP
  * @retval None
*******************************************************************************/
void hal_nrf_set_power_mode_3(hal_nrf_pwr_mode_t pwr_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg_3(CONFIG);

	if (pwr_mode == HAL_NRF_PWR_UP)
	{
		config.bits.pwr_up = 1;
	}
	else
	{
		config.bits.pwr_up = 0;
	}

	hal_nrf_write_reg_3(CONFIG, config.value);
}

/*******************************************************************************
  * @brief  Set the CRC mode used by the radio
  * @param  crc_mode: 
  *				CRC disabled, 1 or 2 bytes
  *     @arg HAL_NRF_CRC_OFF
  *     @arg HAL_NRF_CRC_8BIT
  *     @arg HAL_NRF_CRC_16BIT
  * @retval None
*******************************************************************************/
void hal_nrf_set_crc_mode_3(hal_nrf_crc_mode_t crc_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg_3(CONFIG);

	switch (crc_mode)
	{
	case HAL_NRF_CRC_OFF:
		config.bits.en_crc = 0;
		break;
	case HAL_NRF_CRC_8BIT:
		config.bits.en_crc = 1;
		config.bits.crc0 = 0;
		break;
	case HAL_NRF_CRC_16BIT:
		config.bits.en_crc = 1;
		config.bits.crc0 = 1;
		break;
	default:
		break;
	}

	hal_nrf_write_reg_3(CONFIG, config.value);
}

/*******************************************************************************
  * @brief  Enable or disable interrupt for radio
  * @param  int_source: 
  *				Specifies IRQ Source
  *     @arg HAL_NRF_MAX_RT
  *     @arg HAL_NRF_TX_DS
  *     @arg HAL_NRF_RX_DR
  * @param  irq_state:
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None
*******************************************************************************/
void hal_nrf_set_irq_mode_3(hal_nrf_irq_source_t int_source, bool irq_state)
{
	config_t config;
	config.value = hal_nrf_read_reg_3(CONFIG);

	switch (int_source)
	{
	case HAL_NRF_MAX_RT:
		config.bits.mask_max_rt = irq_state ? 0 : 1;
		break;
	case HAL_NRF_TX_DS:
		config.bits.mask_tx_ds = irq_state ? 0 : 1;
		break;
	case HAL_NRF_RX_DR:
		config.bits.mask_rx_dr = irq_state ? 0 : 1;
		break;
	}

	hal_nrf_write_reg_3(CONFIG, config.value);
}

/*******************************************************************************
  * @brief  Read then clears all interrupt flags.
  * @param  None
  * @retval Interrupt_flags
  *     @ret 0x00: None was set
  *     @ret 0x10: MAX_RT Flag was set
  *     @ret 0x20: TX_DS Flag was set
  *     @ret 0x40: RX_DR Flag was set
*******************************************************************************/
uint8_t hal_nrf_get_clear_irq_flags_3(void)
{
	uint8_t retval;

	retval = hal_nrf_write_reg_3(STATUS_2, (BIT_6|BIT_5|BIT_4));

	return (retval & (BIT_6|BIT_5|BIT_4));
}

/*******************************************************************************
  * @brief  Clear one selected interrupt flag
  * @param  int_source: 
  *				Specifies IRQ Source
  *     @arg HAL_NRF_MAX_RT
  *     @arg HAL_NRF_TX_DS
  *     @arg HAL_NRF_RX_DR
  * @retval None
*******************************************************************************/
void hal_nrf_clear_irq_flag_3(hal_nrf_irq_source_t int_source)
{
	hal_nrf_write_reg_3(STATUS_2, _SET_BIT(int_source));
}

/*******************************************************************************
  * @brief  Read all interrupt flags
  * @param  None
  * @retval Interrupt_flags
  *     @ret 0x00: None was set
  *     @ret 0x10: MAX_RT Flag was set
  *     @ret 0x20: TX_DS Flag was set
  *     @ret 0x40: RX_DR Flag was set
*******************************************************************************/
uint8_t hal_nrf_get_irq_flags_3(void)
{
	return hal_nrf_nop_3() & (BIT_6|BIT_5|BIT_4);
}

/*******************************************************************************
  * @brief  Open radio pipe(s) and enable/ disable auto acknowledge
  * @param  pipe_num: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @param  auto_ack:
  *				Whether enable or disable ACK
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None
*******************************************************************************/
void hal_nrf_open_pipe_3(hal_nrf_address_t pipe_num, bool auto_ack)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	en_rxaddr.value = hal_nrf_read_reg_3(EN_RXADDR);
	en_aa.value = hal_nrf_read_reg_3(EN_AA);

	switch (pipe_num)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_PIPE2:
	case HAL_NRF_PIPE3:
	case HAL_NRF_PIPE4:
	case HAL_NRF_PIPE5:
		en_rxaddr.value = en_rxaddr.value | _SET_BIT(pipe_num);

		if (auto_ack)
		{
			en_aa.value = en_aa.value | _SET_BIT(pipe_num);
		}
		else
		{
			en_aa.value = en_aa.value & ~(_SET_BIT(pipe_num));
		}
		break;

	case HAL_NRF_ALL:
		en_rxaddr.value = (uint8_t) ~(BIT_6|BIT_7);

		if (auto_ack)
		{
			en_aa.value = (uint8_t) ~(BIT_6|BIT_7);
		}
		else
		{
			en_aa.value = 0;
		}
		break;

	default:
		break;
	}

	hal_nrf_write_reg_3(EN_RXADDR, en_rxaddr.value);
	hal_nrf_write_reg_3(EN_AA, en_aa.value);
}

/*******************************************************************************
  * @brief  Close radio pipe(s)
  * @param  pipe_num: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @retval None
*******************************************************************************/
void hal_nrf_close_pipe_3(hal_nrf_address_t pipe_num)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	en_rxaddr.value = hal_nrf_read_reg_3(EN_RXADDR);
	en_aa.value = hal_nrf_read_reg_3(EN_AA);

	switch (pipe_num)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_PIPE2:
	case HAL_NRF_PIPE3:
	case HAL_NRF_PIPE4:
	case HAL_NRF_PIPE5:
		en_rxaddr.value = en_rxaddr.value & ~(_SET_BIT(pipe_num));
		en_aa.value = en_aa.value & ~(_SET_BIT(pipe_num));
		break;

	case HAL_NRF_ALL:
		en_rxaddr.value = 0;
		en_aa.value = 0;
		break;

	default:
		break;
	}

	hal_nrf_write_reg_3(EN_RXADDR, en_rxaddr.value);
	hal_nrf_write_reg_3(EN_AA, en_aa.value);
}

/*******************************************************************************
  * @brief  Set radio's RX address and TX address
  * @param  address: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @param  addr:
  *				Pointer to Address[5 or 1]
  * @retval None
*******************************************************************************/
void hal_nrf_set_address_3(const hal_nrf_address_t address, const uint8_t *addr)
{
	switch (address)
	{
	case HAL_NRF_TX:
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
		hal_nrf_write_multibyte_reg_3(W_REGISTER + RX_ADDR_P0 + (uint8_t) address, addr, hal_nrf_get_address_width_3());
		break;
	case HAL_NRF_PIPE2:
	case HAL_NRF_PIPE3:
	case HAL_NRF_PIPE4:
	case HAL_NRF_PIPE5:
		hal_nrf_write_reg_3(RX_ADDR_P0 + (uint8_t) address, *addr);
		break;

	default:
		break;
	}
}

/*******************************************************************************
  * @brief  Get address for selected pipe
  * @param  address: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @param  addr:
  *				Pointer to Address[5 or 1]
  * @retval Address Width
  *     @ret 0x03: 3 bytes
  *     @ret 0x04: 4 bytes
  *     @ret 0x05: 5 bytes
  *     @ret 0x01: PIPE2~PIPE5 And HAL_NRF_ALL
*******************************************************************************/
uint8_t hal_nrf_get_address_3(uint8_t address, uint8_t *addr)
{
	switch (address)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_TX:
		return hal_nrf_read_multibyte_reg_3(address, addr);
	default:
		*addr = hal_nrf_read_reg_3(RX_ADDR_P0 + address);
		return 1;
	}
}

/*******************************************************************************
  * @brief  Set auto acknowledge parameters
  * @param  retr: 
  *				Specifies times of Retransmittion
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @param  delay:
  *				Specifies ARD
  * @retval None
*******************************************************************************/
void hal_nrf_set_auto_retr_3(uint8_t retr, uint16_t delay)
{
	setup_retr_t setup_retr;
	setup_retr.value = 0x00;
	setup_retr.bits.ard = (delay / 250) - 1;
	setup_retr.bits.arc = retr;

	hal_nrf_write_reg_3(SETUP_RETR, setup_retr.value);
}

/*******************************************************************************
  * @brief  Set radio's address width
  * @param  aw: 
  *				Specifies Width of Address
  *     @arg HAL_NRF_AW_3BYTES
  *     @arg HAL_NRF_AW_4BYTES
  *     @arg HAL_NRF_AW_5BYTES
  * @retval None
*******************************************************************************/
void hal_nrf_set_address_width_3(hal_nrf_address_width_t aw)
{
	setup_aw_t setup_aw;
	setup_aw.bits.aw = (uint8_t)aw - 2;

	hal_nrf_write_reg_3(SETUP_AW, setup_aw.value);
}

/*******************************************************************************
  * @brief  Gets the radio's address width
  * @param  None
  * @retval Address Width
  *     @ret 0x03: 3 bytes
  *     @ret 0x04: 4 bytes
  *     @ret 0x05: 5 bytes
*******************************************************************************/
uint8_t hal_nrf_get_address_width_3(void)
{
	return hal_nrf_read_reg_3(SETUP_AW) + 2;
}

/*******************************************************************************
  * @brief  Set payload width for selected pipe
  * @param  pipe_num: 
  *				Specifies PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @param  pload_width:
  *				Specifies width of Payload
  * @retval None
*******************************************************************************/
void hal_nrf_set_rx_payload_width_3(uint8_t pipe_num, uint8_t pload_width)
{
	hal_nrf_write_reg_3(RX_PW_P0 + pipe_num, pload_width);
}

/*******************************************************************************
  * @brief  Get Specifies PIPEx Status
  * @param  address: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL
  * @retval Pipe_Status
  *     @ret 0x00 Pipe is closed, autoack disabled
  *     @ret 0x01 Pipe is open,   autoack disabled
  *     @ret 0x03 Pipe is open,   autoack enabled
*******************************************************************************/
uint8_t hal_nrf_get_pipe_status_3(uint8_t pipe_num)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	uint8_t en_rx_r, en_aa_r;

	en_rxaddr.value = hal_nrf_read_reg_3(EN_RXADDR);
	en_aa.value = hal_nrf_read_reg_3(EN_AA);

	switch (pipe_num)
	{
	case 0:
		en_rx_r = en_rxaddr.bits.pipe_0;
		en_aa_r = en_aa.bits.pipe_0;
		break;
	case 1:
		en_rx_r = en_rxaddr.bits.pipe_1;
		en_aa_r = en_aa.bits.pipe_1;
		break;
	case 2:
		en_rx_r = en_rxaddr.bits.pipe_2;
		en_aa_r = en_aa.bits.pipe_2;
		break;
	case 3:
		en_rx_r = en_rxaddr.bits.pipe_3;
		en_aa_r = en_aa.bits.pipe_3;
		break;
	case 4:
		en_rx_r = en_rxaddr.bits.pipe_4;
		en_aa_r = en_aa.bits.pipe_4;
		break;
	case 5:
		en_rx_r = en_rxaddr.bits.pipe_5;
		en_aa_r = en_aa.bits.pipe_5;
		break;
	default:
		en_rx_r = 0;
		en_aa_r = 0;
		break;
	}

	return (uint8_t)(en_aa_r << 1) + en_rx_r;
}

/*******************************************************************************
  * @brief  Get Auto Retransmittion parameters
  * @param  None
  * @retval AutoRetrans Parameters
  *     @ret UpperNibble Retransmit Delay
  *     @ret LowerNibble Retransmit Count   
*******************************************************************************/
uint8_t hal_nrf_get_auto_retr_status_3(void)
{
	return hal_nrf_read_reg_3(OBSERVE_TX);
}

/*******************************************************************************
  * @brief  Get packet lost counter
  * @param  None
  * @retval packet lost counter
*******************************************************************************/
uint8_t hal_nrf_get_packet_lost_ctr_3(void)
{
	return ((hal_nrf_read_reg_3(OBSERVE_TX) & (BIT_7|BIT_6|BIT_5|BIT_4)) >> 4);
}

/*******************************************************************************
  * @brief  Get RX payload width for selected pipe
  * @param  pipe_num: 
  *				Specifies which PIPEx
  *     @arg HAL_NRF_PIPE0
  *     @arg HAL_NRF_PIPE1
  *     @arg HAL_NRF_PIPE2
  *     @arg HAL_NRF_PIPE3
  *     @arg HAL_NRF_PIPE4
  *     @arg HAL_NRF_PIPE5
  *     @arg HAL_NRF_TX
  *     @arg HAL_NRF_ALL   
  * @retval Payload_Width in bytes
*******************************************************************************/
uint8_t hal_nrf_get_rx_payload_width_3(uint8_t pipe_num)
{
	uint8_t pw;

	switch (pipe_num)
	{
	case 0:
		pw = hal_nrf_read_reg_3(RX_PW_P0);
		break;
	case 1:
		pw = hal_nrf_read_reg_3(RX_PW_P1);
		break;
	case 2:
		pw = hal_nrf_read_reg_3(RX_PW_P2);
		break;
	case 3:
		pw = hal_nrf_read_reg_3(RX_PW_P3);
		break;
	case 4:
		pw = hal_nrf_read_reg_3(RX_PW_P4);
		break;
	case 5:
		pw = hal_nrf_read_reg_3(RX_PW_P5);
		break;
	default:
		pw = 0;
		break;
	}

	return pw;
}

/*******************************************************************************
  * @brief  Set radio's RF channel
  * @param  channel: 
  *				Specifies radio's RF channel
  * @retval None
*******************************************************************************/
void hal_nrf_set_rf_channel_3(uint8_t channel)
{
	rf_ch_t rf_ch;
	rf_ch.bits.rf_ch = channel;

	hal_nrf_write_reg_3(RF_CH, rf_ch.value);
}

/*******************************************************************************
  * @brief  Set radio's TX output power
  * @param  power: 
  *				Specifies radio's TX output power
  *     @arg HAL_NRF_18DBM
  *     @arg HAL_NRF_12DBM
  *     @arg HAL_NRF_6DBM
  *     @arg HAL_NRF_0DBM
  * @retval None  
*******************************************************************************/
void hal_nrf_set_output_power_3(hal_nrf_output_power_t power)
{
	rf_setup_t rf_setup;
	rf_setup.value = hal_nrf_read_reg_3(RF_SETUP);

	rf_setup.bits.rf_pwr = (uint8_t)power;

	hal_nrf_write_reg_3(RF_SETUP, rf_setup.value);
}

/*******************************************************************************
  * @brief  Set radio's on-air datarate
  * @param  datarate: 
  *				Specifies radio's on-air datarate
  *     @arg HAL_NRF_1MBPS
  *     @arg HAL_NRF_2MBPS
  *     @arg HAL_NRF_250KBPS
  * @retval None
*******************************************************************************/
void hal_nrf_set_datarate_3(hal_nrf_datarate_t datarate)
{
	rf_setup_t rf_setup;
	rf_setup.value = hal_nrf_read_reg_3(RF_SETUP);

	switch (datarate)
	{
	case HAL_NRF_250KBPS:
		rf_setup.bits.rf_dr_low = 1;
		rf_setup.bits.rf_dr_high = 0;
		break;
	case HAL_NRF_1MBPS:
		rf_setup.bits.rf_dr_low = 0;
		rf_setup.bits.rf_dr_high = 0;
		break;
	case HAL_NRF_2MBPS:
	default:
		rf_setup.bits.rf_dr_low = 0;
		rf_setup.bits.rf_dr_high = 1;
		break;
	}

	hal_nrf_write_reg_3(RF_SETUP, rf_setup.value);
}

/*******************************************************************************
  * @brief  Check for RX FIFO empty
  * @param  None
  * @retval RX FIFO empty bit
  *     @ret FALSE --- RX FIFO have data
  *     @ret TRUE  --- RX FIFO empty
*******************************************************************************/
bool hal_nrf_rx_fifo_empty_3(void)
{
	if (hal_nrf_get_rx_data_source_3()==7)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
  * @brief  Check for RX FIFO full
  * @param  None
  * @retval RX FIFO full bit
  *     @ret FALSE --- RX FIFO NOT full
  *     @ret TRUE  --- RX FIFO full
*******************************************************************************/
bool hal_nrf_rx_fifo_full_3(void)
{
	return (bool)((hal_nrf_read_reg_3(FIFO_STATUS)>> RX_FULL) & 1);
}

/*******************************************************************************
  * @brief  Check for TX FIFO empty
  * @param  None
  * @retval TX FIFO empty bit
  *     @ret FALSE --- TX FIFO have data
  *     @ret TRUE  --- TX FIFO empty	   
*******************************************************************************/
bool hal_nrf_tx_fifo_empty_3(void)
{
	return (bool)((hal_nrf_read_reg_3(FIFO_STATUS) >> TX_EMPTY) & 1);
}

/*******************************************************************************
  * @brief  Check for TX FIFO full
  * @param  None
  * @retval TX FIFO full bit
  *     @ret FALSE --- TX FIFO NOT full
  *     @ret TRUE  --- TX FIFO full	   
*******************************************************************************/
bool hal_nrf_tx_fifo_full_3(void)
{
	return (bool)((hal_nrf_read_reg_3(FIFO_STATUS) >> TX_FIFO_FULL) & 1);
}

/*******************************************************************************
  * @brief  Get radio's TX FIFO status
  * @param  None
  * @retval TX FIFO status
  *     @ret 0x00: TX FIFO NOT empty,NOT full
  *     @ret 0x01: TX FIFO empty
  *     @ret 0x02: TX FIFO full	   
*******************************************************************************/
uint8_t hal_nrf_get_tx_fifo_status_3(void)
{
	return ((hal_nrf_read_reg_3(FIFO_STATUS) & ((1<<TX_FIFO_FULL)|(1<<TX_EMPTY))) >> 4);
}

/*******************************************************************************
  * @brief  Get radio's RX FIFO status
  * @param  None
  * @retval RX FIFO status
  *     @ret 0x00: RX FIFO NOT empty,NOT full
  *     @ret 0x01: RX FIFO empty
  *     @ret 0x02: RX FIFO full
*******************************************************************************/
uint8_t hal_nrf_get_rx_fifo_status_3(void)
{
	return (hal_nrf_read_reg_3(FIFO_STATUS) & ((1<<RX_FULL)|(1<<RX_EMPTY)));
}

/*******************************************************************************
  * @brief  Get valude of FIFO_STATUS
  * @param  None
  * @retval valude of FIFO_STATUS
*******************************************************************************/
uint8_t hal_nrf_get_fifo_status_3(void)
{
	return hal_nrf_read_reg_3(FIFO_STATUS);
}

/*******************************************************************************
  * @brief  Get radio's transmit attempts status
  * @param  None
  * @retval Retransmit attempts counters
*******************************************************************************/
uint8_t hal_nrf_get_transmit_attempts_3(void)
{
	return (hal_nrf_read_reg_3(OBSERVE_TX) & (BIT_3|BIT_2|BIT_1|BIT_0));
}

/*******************************************************************************
  * @brief  Get the carrier detect flag
  * @param  None
  * @retval Carrier Detect
  *     @ret FALSE --- Carrier NOT Detected
  *     @ret TRUE  --- Carrier Detected  
*******************************************************************************/
uint8_t hal_nrf_get_carrier_detect_3(void)
{
	return (hal_nrf_read_reg_3(CD) & 0x01);
}

/*******************************************************************************
  * @brief  Function for enabling dynmic payload size
  * @param  setup: 
  *				Specifies which pips(s) to enable dynamic payload size
  * @retval None	  
*******************************************************************************/
void hal_nrf_setup_dynamic_payload_3(uint8_t setup)
{
	en_pipes_t dynpd;
	dynpd.value = setup & ~0xC0;

	hal_nrf_write_reg_3(DYNPD_2, dynpd.value);
}

/*******************************************************************************
  * @brief  Enables the dynamic packet length
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None  
*******************************************************************************/
void hal_nrf_enable_dynamic_payload_3(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg_3(FEATURE_2);
	feature.bits.en_dpl = (enable) ? 1 : 0;

	hal_nrf_write_reg_3(FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Enables the ACK payload feature
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable  
  * @retval None  		   
*******************************************************************************/
void hal_nrf_enable_ack_payload_3(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg_3(FEATURE_2);
	feature.bits.en_ack_pay = (enable) ? 1 : 0;

	hal_nrf_write_reg_3(FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Enables the dynamic ack feature
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None  		   
*******************************************************************************/
void hal_nrf_enable_dynamic_ack_3(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg_3(FEATURE_2);
	feature.bits.en_dyn_ack = (enable) ? 1 : 0;

	hal_nrf_write_reg_3(FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Write TX payload to radio
  * @param  tx_pload: 
  *				Pointer of TxBuf[PayLoad_Width]
  * @param  length: 
  *				length number of bytes to write
  * @retval None  
*******************************************************************************/
void hal_nrf_write_tx_payload_3(const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg_3(W_TX_PAYLOAD, tx_pload, length);
}

/*******************************************************************************
  * @brief  Write TX payload which do not require ACK
  * @param  tx_pload: 
  *				Pointer of TxBuf[PayLoad_Width]
  * @param  length: 
  *				length number of bytes to write
  * @retval None  
*******************************************************************************/
void hal_nrf_write_tx_payload_noack_3(const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg_3(W_TX_PAYLOAD_NOACK, tx_pload, length);
}

/*******************************************************************************
  * @brief  Write ack payload
  * @param  pipe: 
  *				PIPE0 ~ PIPE5
  * @param  tx_pload: 
  *				Pointer of TxBuf[PayLoad_Width]
  * @param  length: 
  *				length number of bytes to write
  * @retval None	  
*******************************************************************************/
void hal_nrf_write_ack_payload_3(uint8_t pipe, const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg_3(W_ACK_PAYLOAD | pipe, tx_pload, length);
}

/*******************************************************************************
  * @brief  Reads the payload width of the received ack payload
  * @param  None
  * @retval Payload width of the received ack payload	   
*******************************************************************************/
uint8_t hal_nrf_read_rx_payload_width_3(void)
{
	return hal_nrf_read_reg_3(R_RX_PL_WID);
}

/*******************************************************************************
  * @brief  Read RX payload
  * @param  rx_pload: 
  *				Specifies Pointer RxBuf[N];
  * @retval pipe number (MSB byte) and packet length (LSB byte)
*******************************************************************************/
uint16_t hal_nrf_read_rx_payload_3(uint8_t *rx_pload)
{
	return hal_nrf_read_multibyte_reg_3((uint8_t)(HAL_NRF_RX_PLOAD), rx_pload);
}

/*******************************************************************************
  * @brief  Get RX data source
  * @param  None
  * @retval pipe number of current packet present  
*******************************************************************************/
uint8_t hal_nrf_get_rx_data_source_3(void)
{
	return ((hal_nrf_nop_3() & (BIT_3|BIT_2|BIT_1)) >> 1);
}

/*******************************************************************************
  * @brief  Reuse TX payload
  * @param  None
  * @retval None	  
*******************************************************************************/
void hal_nrf_reuse_tx_3(void)
{
	CSN_LOW_3();
	hal_nrf_rw_3(REUSE_TX_PL);
	CSN_HIGH_3();
}

/*******************************************************************************
  * @brief  Get status of reuse TX function
  * @param  None
  * @retval Reuse TX payload mode
  *     @ret FALSE --- Not activated
  *     @ret TRUE  --- Activated
  *     @ret 0x02: RX FIFO full
*******************************************************************************/
bool hal_nrf_get_reuse_tx_status_3(void)
{
	return (bool)((hal_nrf_get_fifo_status_3() & (1<<TX_REUSE)) >> TX_REUSE);
}

/*******************************************************************************
  * @brief  Flush RX FIFO
  * @param  None
  * @retval None
*******************************************************************************/
void hal_nrf_flush_rx_3(void)
{
	CSN_LOW_3();
	hal_nrf_rw_3(FLUSH_RX);
	CSN_HIGH_3();
}

/*******************************************************************************
  * @brief  Flush TX FIFO
  * @param  None
  * @retval None
*******************************************************************************/
void hal_nrf_flush_tx_3(void)
{
	CSN_LOW_3();
	hal_nrf_rw_3(FLUSH_TX);
	CSN_HIGH_3();
}

/*******************************************************************************
  * @brief  No Operation command
  * @param  None
  * @retval Status register
*******************************************************************************/
uint8_t hal_nrf_nop_3(void)
{
	uint8_t retval;

	CSN_LOW_3();
	retval = hal_nrf_rw_3(NOP);
	CSN_HIGH_3();

	return retval;
}

/*******************************************************************************
  * @brief  Read a byte from one of nrf24 registers.
  * @param  uReg: 
				Specifies one of nrf24 registers.
  * @retval Value of regisiter Specified.
*******************************************************************************/
uint8_t hal_nrf_read_reg_3(uint8_t reg)
{
	uint8_t temp;

	CSN_LOW_3();
	hal_nrf_rw_3(reg);
	temp = hal_nrf_rw_3(0);
	CSN_HIGH_3();

	return temp;
}

/*******************************************************************************
  * @brief  Write a byte to one of nrf24 registers.
  * @param  uReg: 
				Specifies one of nrf24 registers.
			uWrittenByte:
				will be written to the uReg.
  * @retval None
*******************************************************************************/
uint8_t hal_nrf_write_reg_3(uint8_t reg, uint8_t value)
{
	uint8_t retval;

	CSN_LOW_3();
	retval = hal_nrf_rw_3(W_REGISTER + reg);
	hal_nrf_rw_3(value);
	CSN_HIGH_3();

	return retval;
}

/*******************************************************************************
  * @brief  Read bytes from nrf24 registers.
  * @param  reg: 
				Specifies one of nrf24 registers.
			puBuf:
				Specifies Pointer of RevBuf[]
  * @retval combination PIPEx Number and Width of PayLoad
  *     @ret UpperByte: PIPEx Number
  *     @ret LowerByte: Width of PayLoad 
*******************************************************************************/
uint16_t hal_nrf_read_multibyte_reg_3(uint8_t reg, uint8_t *pbuf)
{
	uint8_t ctr, length;
	switch (reg)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_TX:
			length = ctr = hal_nrf_get_address_width_3();
			CSN_LOW_3();
			hal_nrf_rw_3(RX_ADDR_P0 + reg);
			break;

	case HAL_NRF_RX_PLOAD:
			if ( (reg = hal_nrf_get_rx_data_source_3()) < 7)
			{
				length = ctr = hal_nrf_read_rx_payload_width_3();

				CSN_LOW_3();
				hal_nrf_rw_3(R_RX_PAYLOAD);
			}
			else
			{
				ctr = length = 0;
			}
			break;

	default:
		ctr = length = 0;
		break;
	}

	while (ctr--)
	{
		*pbuf++ = hal_nrf_rw_3(0);
	}
	CSN_HIGH_3();
	return (((uint16_t) reg << 8) | length);
}

/*******************************************************************************
  * @brief  Write bytes to one of nrf24 registers.
  * @param  cmd: 
				Specifies cmd
			pbuf:
				Specifies Pointer of Buf[length]
			length:
				Specifies width of puBuf[length]
  * @retval None
*******************************************************************************/
void hal_nrf_write_multibyte_reg_3(uint8_t cmd, const uint8_t *pbuf, uint8_t length)
{
	CSN_LOW_3();
	hal_nrf_rw_3(cmd);
	while(length--)
	{
		hal_nrf_rw_3(*pbuf++);
	}
	CSN_HIGH_3();
}

/*******************************************************************************
  * @brief  Read or Write a byte to nrf24L01+ through spi.
  * @param  uWrittenByte: 
				Specifies the byte that will be written.
  * @retval The Value of STATUS Reg.
*******************************************************************************/
uint8_t hal_nrf_rw_3(uint8_t uWrittenByte)
{
	uint8_t i = 0;
	uint8_t uTmp = uWrittenByte;
	for(i = 0; i < 8; i++)
	{
		if (uTmp & 0x80)
		{
			MOSI_HIGH_3();
		}
		else
		{
			MOSI_LOW_3();
		}
		uTmp <<= 1;
		CLK_HIGH_3();
		// if(MISO_INPUT_3())
		if((GPIOB->IDR & GPIO_Pin_4) != (uint32_t)Bit_RESET)
		{
			uTmp |= 0x01;
		}
		else
		{
			uTmp &= 0xfe;
		}
		CLK_LOW_3();
	}
	return(uTmp);
}


