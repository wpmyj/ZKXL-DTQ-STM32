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
#include "stdio.h"
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
void hal_nrf_set_operation_mode(hal_nrf_operation_mode_t op_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg (CONFIG);

	if (op_mode == HAL_NRF_PRX)
	{
		config.bits.prim_rx = 1;
	}
	else
	{
		config.bits.prim_rx = 0;
	}

	hal_nrf_write_reg (CONFIG, config.value);
}

/*******************************************************************************
  * @brief  Set radio's power mode
  * @param  pwr_mode: 
  *				power_up or power_down radio
  *     @arg HAL_NRF_PWR_DOWN
  *     @arg HAL_NRF_PWR_UP
  * @retval None
*******************************************************************************/
void hal_nrf_set_power_mode(hal_nrf_pwr_mode_t pwr_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg (CONFIG);

	if (pwr_mode == HAL_NRF_PWR_UP)
	{
		config.bits.pwr_up = 1;
	}
	else
	{
		config.bits.pwr_up = 0;
	}

	hal_nrf_write_reg (CONFIG, config.value);
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
void hal_nrf_set_crc_mode(hal_nrf_crc_mode_t crc_mode)
{
	config_t config;
	config.value = hal_nrf_read_reg (CONFIG);

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

	hal_nrf_write_reg (CONFIG, config.value);
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
void hal_nrf_set_irq_mode(hal_nrf_irq_source_t int_source, bool irq_state)
{
	config_t config;
	config.value = hal_nrf_read_reg (CONFIG);

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

	hal_nrf_write_reg (CONFIG, config.value);
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
uint8_t hal_nrf_get_clear_irq_flags(void)
{
	uint8_t retval;

	retval = hal_nrf_write_reg (STATUS_2, (BIT_6|BIT_5|BIT_4));

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
void hal_nrf_clear_irq_flag(hal_nrf_irq_source_t int_source)
{
	hal_nrf_write_reg (STATUS_2, _SET_BIT(int_source));
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
uint8_t hal_nrf_get_irq_flags(void)
{
	return hal_nrf_nop() & (BIT_6|BIT_5|BIT_4);
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
void hal_nrf_open_pipe(hal_nrf_address_t pipe_num, bool auto_ack)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	en_rxaddr.value = hal_nrf_read_reg (EN_RXADDR);
	en_aa.value = hal_nrf_read_reg (EN_AA);

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

	hal_nrf_write_reg (EN_RXADDR, en_rxaddr.value);
	hal_nrf_write_reg (EN_AA, en_aa.value);
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
void hal_nrf_close_pipe(hal_nrf_address_t pipe_num)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	en_rxaddr.value = hal_nrf_read_reg (EN_RXADDR);
	en_aa.value = hal_nrf_read_reg (EN_AA);

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

	hal_nrf_write_reg (EN_RXADDR, en_rxaddr.value);
	hal_nrf_write_reg (EN_AA, en_aa.value);
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
void hal_nrf_set_address(const hal_nrf_address_t address, const uint8_t *addr)
{
	switch (address)
	{
	case HAL_NRF_TX:
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
		hal_nrf_write_multibyte_reg(W_REGISTER + RX_ADDR_P0 + (uint8_t) address, addr, hal_nrf_get_address_width());
		break;
	case HAL_NRF_PIPE2:
	case HAL_NRF_PIPE3:
	case HAL_NRF_PIPE4:
	case HAL_NRF_PIPE5:
		hal_nrf_write_reg (RX_ADDR_P0 + (uint8_t) address, *addr);
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
uint8_t hal_nrf_get_address(uint8_t address, uint8_t *addr)
{
	switch (address)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_TX:
		return hal_nrf_read_multibyte_reg (address, addr);
	default:
		*addr = hal_nrf_read_reg(RX_ADDR_P0 + address);
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
void hal_nrf_set_auto_retr(uint8_t retr, uint16_t delay)
{
	setup_retr_t setup_retr;
	setup_retr.value = 0x00;
	setup_retr.bits.ard = (delay / 250) - 1;
	setup_retr.bits.arc = retr;

	hal_nrf_write_reg (SETUP_RETR, setup_retr.value);
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
void hal_nrf_set_address_width(hal_nrf_address_width_t aw)
{
	setup_aw_t setup_aw;
	setup_aw.bits.aw = (uint8_t)aw - 2;

	hal_nrf_write_reg (SETUP_AW, setup_aw.value);
}

/*******************************************************************************
  * @brief  Gets the radio's address width
  * @param  None
  * @retval Address Width
  *     @ret 0x03: 3 bytes
  *     @ret 0x04: 4 bytes
  *     @ret 0x05: 5 bytes
*******************************************************************************/
uint8_t hal_nrf_get_address_width (void)
{
	return hal_nrf_read_reg (SETUP_AW) + 2;
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
void hal_nrf_set_rx_payload_width(uint8_t pipe_num, uint8_t pload_width)
{
	hal_nrf_write_reg (RX_PW_P0 + pipe_num, pload_width);
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
uint8_t hal_nrf_get_pipe_status(uint8_t pipe_num)
{
	en_pipes_t en_rxaddr;
	en_pipes_t en_aa;
	uint8_t en_rx_r, en_aa_r;

	en_rxaddr.value = hal_nrf_read_reg (EN_RXADDR);
	en_aa.value = hal_nrf_read_reg (EN_AA);

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
uint8_t hal_nrf_get_auto_retr_status(void)
{
	return hal_nrf_read_reg(OBSERVE_TX);
}

/*******************************************************************************
  * @brief  Get packet lost counter
  * @param  None
  * @retval packet lost counter
*******************************************************************************/
uint8_t hal_nrf_get_packet_lost_ctr(void)
{
	return ((hal_nrf_read_reg(OBSERVE_TX) & (BIT_7|BIT_6|BIT_5|BIT_4)) >> 4);
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
uint8_t hal_nrf_get_rx_payload_width(uint8_t pipe_num)
{
	uint8_t pw;

	switch (pipe_num)
	{
	case 0:
		pw = hal_nrf_read_reg (RX_PW_P0);
		break;
	case 1:
		pw = hal_nrf_read_reg (RX_PW_P1);
		break;
	case 2:
		pw = hal_nrf_read_reg (RX_PW_P2);
		break;
	case 3:
		pw = hal_nrf_read_reg (RX_PW_P3);
		break;
	case 4:
		pw = hal_nrf_read_reg (RX_PW_P4);
		break;
	case 5:
		pw = hal_nrf_read_reg (RX_PW_P5);
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
void hal_nrf_set_rf_channel(uint8_t channel)
{
	rf_ch_t rf_ch;
	rf_ch.bits.rf_ch = channel;

	hal_nrf_write_reg (RF_CH, rf_ch.value);
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
void hal_nrf_set_output_power(hal_nrf_output_power_t power)
{
	rf_setup_t rf_setup;
	rf_setup.value = hal_nrf_read_reg (RF_SETUP);

	rf_setup.bits.rf_pwr = (uint8_t)power;

	hal_nrf_write_reg (RF_SETUP, rf_setup.value);
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
void hal_nrf_set_datarate(hal_nrf_datarate_t datarate)
{
	rf_setup_t rf_setup;
	rf_setup.value = hal_nrf_read_reg (RF_SETUP);

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

	hal_nrf_write_reg (RF_SETUP, rf_setup.value);
}

/*******************************************************************************
  * @brief  Check for RX FIFO empty
  * @param  None
  * @retval RX FIFO empty bit
  *     @ret FALSE --- RX FIFO have data
  *     @ret TRUE  --- RX FIFO empty
*******************************************************************************/
bool hal_nrf_rx_fifo_empty(void)
{
	if (hal_nrf_get_rx_data_source()==7)
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
bool hal_nrf_rx_fifo_full(void)
{
	return (bool)((hal_nrf_read_reg(FIFO_STATUS)>> RX_FULL) & 1);
}

/*******************************************************************************
  * @brief  Check for TX FIFO empty
  * @param  None
  * @retval TX FIFO empty bit
  *     @ret FALSE --- TX FIFO have data
  *     @ret TRUE  --- TX FIFO empty	   
*******************************************************************************/
bool hal_nrf_tx_fifo_empty(void)
{
	return (bool)((hal_nrf_read_reg(FIFO_STATUS) >> TX_EMPTY) & 1);
}

/*******************************************************************************
  * @brief  Check for TX FIFO full
  * @param  None
  * @retval TX FIFO full bit
  *     @ret FALSE --- TX FIFO NOT full
  *     @ret TRUE  --- TX FIFO full	   
*******************************************************************************/
bool hal_nrf_tx_fifo_full(void)
{
	return (bool)((hal_nrf_read_reg(FIFO_STATUS) >> TX_FIFO_FULL) & 1);
}

/*******************************************************************************
  * @brief  Get radio's TX FIFO status
  * @param  None
  * @retval TX FIFO status
  *     @ret 0x00: TX FIFO NOT empty,NOT full
  *     @ret 0x01: TX FIFO empty
  *     @ret 0x02: TX FIFO full	   
*******************************************************************************/
uint8_t hal_nrf_get_tx_fifo_status(void)
{
	return ((hal_nrf_read_reg(FIFO_STATUS) & ((1<<TX_FIFO_FULL)|(1<<TX_EMPTY))) >> 4);
}

/*******************************************************************************
  * @brief  Get radio's RX FIFO status
  * @param  None
  * @retval RX FIFO status
  *     @ret 0x00: RX FIFO NOT empty,NOT full
  *     @ret 0x01: RX FIFO empty
  *     @ret 0x02: RX FIFO full
*******************************************************************************/
uint8_t hal_nrf_get_rx_fifo_status(void)
{
	return (hal_nrf_read_reg(FIFO_STATUS) & ((1<<RX_FULL)|(1<<RX_EMPTY)));
}

/*******************************************************************************
  * @brief  Get valude of FIFO_STATUS
  * @param  None
  * @retval valude of FIFO_STATUS
*******************************************************************************/
uint8_t hal_nrf_get_fifo_status(void)
{
	return hal_nrf_read_reg(FIFO_STATUS);
}

/*******************************************************************************
  * @brief  Get radio's transmit attempts status
  * @param  None
  * @retval Retransmit attempts counters
*******************************************************************************/
uint8_t hal_nrf_get_transmit_attempts(void)
{
	return (hal_nrf_read_reg(OBSERVE_TX) & (BIT_3|BIT_2|BIT_1|BIT_0));
}

/*******************************************************************************
  * @brief  Get the carrier detect flag
  * @param  None
  * @retval Carrier Detect
  *     @ret FALSE --- Carrier NOT Detected
  *     @ret TRUE  --- Carrier Detected  
*******************************************************************************/
uint8_t hal_nrf_get_carrier_detect(void)
{
	return (hal_nrf_read_reg(CD) & 0x01);
}

/*******************************************************************************
  * @brief  Function for enabling dynmic payload size
  * @param  setup: 
  *				Specifies which pips(s) to enable dynamic payload size
  * @retval None	  
*******************************************************************************/
void hal_nrf_setup_dynamic_payload (uint8_t setup)
{
	en_pipes_t dynpd;
	dynpd.value = setup & ~0xC0;

	hal_nrf_write_reg (DYNPD_2, dynpd.value);
}

/*******************************************************************************
  * @brief  Enables the dynamic packet length
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None  
*******************************************************************************/
void hal_nrf_enable_dynamic_payload(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg (FEATURE_2);
	feature.bits.en_dpl = (enable) ? 1 : 0;

	hal_nrf_write_reg (FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Enables the ACK payload feature
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable  
  * @retval None  		   
*******************************************************************************/
void hal_nrf_enable_ack_payload(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg (FEATURE_2);
	feature.bits.en_ack_pay = (enable) ? 1 : 0;

	hal_nrf_write_reg (FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Enables the dynamic ack feature
  * @param  enable: 
  *     @arg ENABLE:  enable
  *     @arg DISABLE: disable
  * @retval None  		   
*******************************************************************************/
void hal_nrf_enable_dynamic_ack(bool enable)
{
	feature_t feature;
	feature.value = hal_nrf_read_reg (FEATURE_2);
	feature.bits.en_dyn_ack = (enable) ? 1 : 0;

	hal_nrf_write_reg (FEATURE_2, feature.value);
}

/*******************************************************************************
  * @brief  Write TX payload to radio
  * @param  tx_pload: 
  *				Pointer of TxBuf[PayLoad_Width]
  * @param  length: 
  *				length number of bytes to write
  * @retval None  
*******************************************************************************/
void hal_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg(W_TX_PAYLOAD, tx_pload, length);
}

/*******************************************************************************
  * @brief  Write TX payload which do not require ACK
  * @param  tx_pload: 
  *				Pointer of TxBuf[PayLoad_Width]
  * @param  length: 
  *				length number of bytes to write
  * @retval None  
*******************************************************************************/
void hal_nrf_write_tx_payload_noack(const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg(W_TX_PAYLOAD_NOACK, tx_pload, length);
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
void hal_nrf_write_ack_payload(uint8_t pipe, const uint8_t *tx_pload, uint8_t length)
{
	hal_nrf_write_multibyte_reg(W_ACK_PAYLOAD | pipe, tx_pload, length);
}

/*******************************************************************************
  * @brief  Reads the payload width of the received ack payload
  * @param  None
  * @retval Payload width of the received ack payload	   
*******************************************************************************/
uint8_t hal_nrf_read_rx_payload_width(void)
{
	return hal_nrf_read_reg(R_RX_PL_WID);
}

/*******************************************************************************
  * @brief  Read RX payload
  * @param  rx_pload: 
  *				Specifies Pointer RxBuf[N];
  * @retval pipe number (MSB byte) and packet length (LSB byte)
*******************************************************************************/
uint16_t hal_nrf_read_rx_payload(uint8_t *rx_pload)
{
	return hal_nrf_read_multibyte_reg((uint8_t)(HAL_NRF_RX_PLOAD), rx_pload);
}

/*******************************************************************************
  * @brief  Get RX data source
  * @param  None
  * @retval pipe number of current packet present  
*******************************************************************************/
uint8_t hal_nrf_get_rx_data_source(void)
{
	return ((hal_nrf_nop() & (BIT_3|BIT_2|BIT_1)) >> 1);
}

/*******************************************************************************
  * @brief  Reuse TX payload
  * @param  None
  * @retval None	  
*******************************************************************************/
void hal_nrf_reuse_tx(void)
{
	CSN_LOW();
	hal_nrf_rw(REUSE_TX_PL);
	CSN_HIGH();
}

/*******************************************************************************
  * @brief  Get status of reuse TX function
  * @param  None
  * @retval Reuse TX payload mode
  *     @ret FALSE --- Not activated
  *     @ret TRUE  --- Activated
  *     @ret 0x02: RX FIFO full
*******************************************************************************/
bool hal_nrf_get_reuse_tx_status(void)
{
	return (bool)((hal_nrf_get_fifo_status() & (1<<TX_REUSE)) >> TX_REUSE);
}

/*******************************************************************************
  * @brief  Flush RX FIFO
  * @param  None
  * @retval None
*******************************************************************************/
void hal_nrf_flush_rx(void)
{
	CSN_LOW();
	hal_nrf_rw(FLUSH_RX);
	CSN_HIGH();
}

/*******************************************************************************
  * @brief  Flush TX FIFO
  * @param  None
  * @retval None
*******************************************************************************/
void hal_nrf_flush_tx(void)
{
	CSN_LOW();
	hal_nrf_rw(FLUSH_TX);
	CSN_HIGH();
}

/*******************************************************************************
  * @brief  No Operation command
  * @param  None
  * @retval Status register
*******************************************************************************/
uint8_t hal_nrf_nop(void)
{
	uint8_t retval;

	CSN_LOW();
	retval = hal_nrf_rw(NOP);
	CSN_HIGH();

	return retval;
}

/*******************************************************************************
  * @brief  Read a byte from one of nrf24 registers.
  * @param  uReg: 
				Specifies one of nrf24 registers.
  * @retval Value of regisiter Specified.
*******************************************************************************/
uint8_t hal_nrf_read_reg(uint8_t reg)
{
	uint8_t temp;

	CSN_LOW();
	hal_nrf_rw(reg);
	temp = hal_nrf_rw(0);
	CSN_HIGH();

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
uint8_t hal_nrf_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t retval;

	CSN_LOW();
	retval = hal_nrf_rw(W_REGISTER + reg);
	hal_nrf_rw(value);
	CSN_HIGH();

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
uint16_t hal_nrf_read_multibyte_reg(uint8_t reg, uint8_t *pbuf)
{
	uint8_t ctr, length;
	switch (reg)
	{
	case HAL_NRF_PIPE0:
	case HAL_NRF_PIPE1:
	case HAL_NRF_TX:
			length = ctr = hal_nrf_get_address_width();
			CSN_LOW();
			hal_nrf_rw(RX_ADDR_P0 + reg);
			break;

	case HAL_NRF_RX_PLOAD:
			if ( (reg = hal_nrf_get_rx_data_source()) < 7)
			{
				length = ctr = hal_nrf_read_rx_payload_width();

				CSN_LOW();
				hal_nrf_rw(R_RX_PAYLOAD);
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
		*pbuf++ = hal_nrf_rw(0);
	}
	CSN_HIGH();
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
void hal_nrf_write_multibyte_reg(uint8_t cmd, const uint8_t *pbuf, uint8_t length)
{
	CSN_LOW();
	hal_nrf_rw(cmd);
	while(length--)
	{
		hal_nrf_rw(*pbuf++);
	}
	CSN_HIGH();
}

/*******************************************************************************
  * @brief  Read or Write a byte to nrf24L01+ through spi.
  * @param  uWrittenByte: 
				Specifies the byte that will be written.
  * @retval The Value of STATUS Reg.
*******************************************************************************/
uint8_t hal_nrf_rw(uint8_t uWrittenByte)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, uWrittenByte);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPI1));
}





