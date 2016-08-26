/**
  ******************************************************************************
  * @file   	hal_nrf.h
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	24-5-2013
  * @brief    	
  ******************************************************************************
  */
#ifndef HAL_NRF_H__
#define HAL_NRF_H__

/* Define to prevent recursive inclusion -------------------------------------*/
#include <stdint.h>
// #include <stdbool.h>
#include "stm32f10x.h"
#include "rfsim_MCUConf.h"

/* Exported typedef --------------------------------------------------------- */
/* Exported typedef --------------------------------------------------------- */

/* Typedef for the CONFIG register ------------------------------------------ */
typedef union
{
	uint8_t value;
	struct {
		uint8_t prim_rx : 1;
		uint8_t pwr_up : 1;
		uint8_t crc0 : 1;
		uint8_t en_crc : 1;
		uint8_t mask_max_rt : 1;
		uint8_t mask_tx_ds : 1;
		uint8_t mask_rx_dr : 1;
		const uint8_t : 1;
	}bits;
} config_t;

/* Typedef for the EN_AA, EN_RXADDR and DYNPD registers --------------------- */
typedef union
{
	uint8_t value;
	struct {
		uint8_t pipe_0 : 1;
		uint8_t pipe_1 : 1;
		uint8_t pipe_2 : 1;
		uint8_t pipe_3 : 1;
		uint8_t pipe_4 : 1;
		uint8_t pipe_5 : 1;
		const uint8_t : 2;
	}bits;
}en_pipes_t;

/* Typedef for the SETUP_AW register ---------------------------------------- */
typedef union 
{
	uint8_t value;
	struct 
	{
		uint8_t aw : 2;
		const uint8_t : 6;		
	}bits;
}setup_aw_t;

/* Typedef for the SETUP_RETR register -------------------------------------- */
typedef union
{
	uint8_t value;
	struct
	{
		uint8_t arc : 4;
		uint8_t ard : 4;
	}bits;
}setup_retr_t;

/* Typedef for the RF_CH register ------------------------------------------- */
typedef union
{
	uint8_t value;
	struct
	{
		uint8_t rf_ch : 7;
		const uint8_t : 1;
	}bits;
}rf_ch_t;

/* Typedef for the RF_SETUP register ----------------------------------------- */
typedef union
{
	uint8_t value;
	struct
	{
		const uint8_t : 1;
		uint8_t rf_pwr : 2;
		uint8_t rf_dr_high : 1;
		uint8_t pll_lock : 1;
		uint8_t rf_dr_low : 1;
		const uint8_t : 1;
		uint8_t cont_wave : 1;
	}bits;
}rf_setup_t;

/* Typedef for the RX_PW_Px registers --------------------------------------- */
typedef union
{
	uint8_t value;
	struct
	{
		uint8_t rx_pw : 6;
		const uint8_t : 2;
	}bits;
}rx_pw_t;

/* Typedef for the FEATURE register ----------------------------------------- */
typedef union
{
	uint8_t value;
	struct
	{
		uint8_t en_dyn_ack : 1;
		uint8_t en_ack_pay : 1;
		uint8_t en_dpl : 1;
		const uint8_t : 5;
	}bits;
}feature_t;

/* An enum describing the radio's irq sources ------------------------------- */
typedef enum
{
	HAL_NRF_MAX_RT = 4,     /**< Max retries interrupt */
	HAL_NRF_TX_DS,          /**< TX data sent interrupt */
	HAL_NRF_RX_DR           /**< RX data received interrupt */
}hal_nrf_irq_source_t;

/* An enum describing the radio's operation mode ---------------------------- */
typedef enum
{
	HAL_NRF_PTX,            /**< Primary TX operation */
	HAL_NRF_PRX             /**< Primary RX operation */
} hal_nrf_operation_mode_t;

/* An enum describing the radio's power mode -------------------------------- */
typedef enum
{
	HAL_NRF_PWR_DOWN,       /**< Device power-down */
	HAL_NRF_PWR_UP          /**< Device power-up */
}hal_nrf_pwr_mode_t;

/* An enum describing the radio's output power mode's. ---------------------- */
typedef enum
{
	HAL_NRF_18DBM,          /**< Output power set to -18dBm */
	HAL_NRF_12DBM,          /**< Output power set to -12dBm */
	HAL_NRF_6DBM,           /**< Output power set to -6dBm  */
	HAL_NRF_0DBM            /**< Output power set to 0dBm   */
}hal_nrf_output_power_t;

/* An enum describing the radio's on-air datarate --------------------------- */
typedef enum
{
	HAL_NRF_1MBPS,          /**< Datarate set to 1 Mbps  */
	HAL_NRF_2MBPS,          /**< Datarate set to 2 Mbps  */
	HAL_NRF_250KBPS         /**< Datarate set to 250 kbps*/
}hal_nrf_datarate_t;

/* An enum describing the radio's CRC mode ---------------------------------- */
typedef enum
{
	HAL_NRF_CRC_OFF,    /**< CRC check disabled */
	HAL_NRF_CRC_8BIT,   /**< CRC check set to 8-bit */
	HAL_NRF_CRC_16BIT   /**< CRC check set to 16-bit */
} hal_nrf_crc_mode_t;

/* An enum describing the read/write payload command ------------------------ */
typedef enum
{
	HAL_NRF_TX_PLOAD = 7,   /**< TX payload definition */
	HAL_NRF_RX_PLOAD,        /**< RX payload definition */
	HAL_NRF_ACK_PLOAD
} hal_nrf_pload_command_t;

/* An enum describing the nRF24L01 pipe addresses and TX address ------------ */
typedef enum
{
	HAL_NRF_PIPE0,              /**< Select pipe0 */
	HAL_NRF_PIPE1,              /**< Select pipe1 */
	HAL_NRF_PIPE2,              /**< Select pipe2 */
	HAL_NRF_PIPE3,              /**< Select pipe3 */
	HAL_NRF_PIPE4,              /**< Select pipe4 */
	HAL_NRF_PIPE5,              /**< Select pipe5 */
	HAL_NRF_TX,                 /**< Refer to TX address*/
	HAL_NRF_ALL = 0xFF          /**< Close or open all pipes*/
} hal_nrf_address_t;

/* An enum describing the radio's address width ----------------------------- */
typedef enum
{
	HAL_NRF_AW_3BYTES = 3,      /**< Set address width to 3 bytes */
	HAL_NRF_AW_4BYTES,          /**< Set address width to 4 bytes */
	HAL_NRF_AW_5BYTES           /**< Set address width to 5 bytes */
}hal_nrf_address_width_t;


/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

/* Setting Parameter define---------------------------------------------------*/
#define TX_ADR_WIDTH 5
#define RX_ADR_WIDTH 5

// #define CSN_LOW() 	GPIO_ResetBits(SPI_CSN_PORT, SPI_CSN_PIN)
// #define CSN_HIGH() 	GPIO_SetBits(SPI_CSN_PORT, SPI_CSN_PIN)
// #define CE_LOW() 	GPIO_ResetBits(SPI_CE_PORT, SPI_CE_PIN)
// #define CE_HIGH() 	GPIO_SetBits(SPI_CE_PORT, SPI_CE_PIN)

#define CSN_LOW() 		GPIOA->BRR = GPIO_Pin_4
#define CSN_HIGH() 		GPIOA->BSRR = GPIO_Pin_4
#define CE_LOW() 		GPIOC->BRR = GPIO_Pin_4
#define CE_HIGH() 		GPIOC->BSRR = GPIO_Pin_4
                  
// #define CSN_LOW_2() 	GPIO_ResetBits(SPI_CSN_PORT_2, SPI_CSN_PIN_2)
// #define CSN_HIGH_2() 	GPIO_SetBits(SPI_CSN_PORT_2, SPI_CSN_PIN_2)
// #define CE_LOW_2() 		GPIO_ResetBits(SPI_CE_PORT_2, SPI_CE_PIN_2)
// #define CE_HIGH_2() 	GPIO_SetBits(SPI_CE_PORT_2, SPI_CE_PIN_2)

#define CSN_LOW_2() 	GPIOB->BRR = GPIO_Pin_12
#define CSN_HIGH_2() 	GPIOB->BSRR = GPIO_Pin_12
#define CE_LOW_2() 		GPIOC->BRR = GPIO_Pin_6
#define CE_HIGH_2() 	GPIOC->BSRR = GPIO_Pin_6
                     
// #define MOSI_LOW_3() 	GPIO_ResetBits(SPI_MOSI_PORT_3, SPI_MOSI_PIN_3)
// #define MOSI_HIGH_3() 	GPIO_SetBits(SPI_MOSI_PORT_3, SPI_MOSI_PIN_3)
// #define CLK_LOW_3()		GPIO_ResetBits(SPI_SCK_PORT_3, SPI_SCK_PIN_3)
// #define CLK_HIGH_3()	GPIO_SetBits(SPI_SCK_PORT_3, SPI_SCK_PIN_3)
// #define CSN_LOW_3() 	GPIO_ResetBits(SPI_CSN_PORT_3, SPI_CSN_PIN_3)
// #define CSN_HIGH_3() 	GPIO_SetBits(SPI_CSN_PORT_3, SPI_CSN_PIN_3)
// #define CE_LOW_3() 		GPIO_ResetBits(SPI_CE_PORT_3, SPI_CE_PIN_3)
// #define CE_HIGH_3() 	GPIO_SetBits(SPI_CE_PORT_3, SPI_CE_PIN_3)
// #define MISO_INPUT_3()	GPIO_ReadInputDataBit(SPI_MISO_PORT_3, SPI_MISO_PIN_3)

#define MOSI_LOW_3() 	GPIOB->BRR = GPIO_Pin_5
#define MOSI_HIGH_3() 	GPIOB->BSRR = GPIO_Pin_5
#define CLK_LOW_3()		GPIOB->BRR = GPIO_Pin_7
#define CLK_HIGH_3()	GPIOB->BSRR = GPIO_Pin_7
#define CSN_LOW_3() 	GPIOA->BRR = GPIO_Pin_15
#define CSN_HIGH_3() 	GPIOA->BSRR = GPIO_Pin_15
#define CE_LOW_3() 		GPIOB->BRR = GPIO_Pin_6
#define CE_HIGH_3() 	GPIOB->BSRR = GPIO_Pin_6
#define MISO_INPUT_3()	GPIO_ReadInputDataBit(SPI_MISO_PORT_3, SPI_MISO_PIN_3)

// #define MOSI_LOW_4() 	GPIO_ResetBits(SPI_MOSI_PORT_4, SPI_MOSI_PIN_4)
// #define MOSI_HIGH_4() 	GPIO_SetBits(SPI_MOSI_PORT_4, SPI_MOSI_PIN_4)
// #define CLK_LOW_4()		GPIO_ResetBits(SPI_SCK_PORT_4, SPI_SCK_PIN_4)
// #define CLK_HIGH_4()	GPIO_SetBits(SPI_SCK_PORT_4, SPI_SCK_PIN_4)
// #define CSN_LOW_4() 	GPIO_ResetBits(SPI_CSN_PORT_4, SPI_CSN_PIN_4)
// #define CSN_HIGH_4() 	GPIO_SetBits(SPI_CSN_PORT_4, SPI_CSN_PIN_4)
// #define CE_LOW_4() 		GPIO_ResetBits(SPI_CE_PORT_4, SPI_CE_PIN_4)
// #define CE_HIGH_4() 	GPIO_SetBits(SPI_CE_PORT_4, SPI_CE_PIN_4)
// #define MISO_INPUT_4()	GPIO_ReadInputDataBit(SPI_MISO_PORT_4, SPI_MISO_PIN_4)

#define MOSI_LOW_4() 	GPIOB->BRR = GPIO_Pin_11
#define MOSI_HIGH_4() 	GPIOB->BSRR = GPIO_Pin_11
#define CLK_LOW_4()		GPIOB->BRR = GPIO_Pin_1
#define CLK_HIGH_4()	GPIOB->BSRR = GPIO_Pin_1
#define CSN_LOW_4() 	GPIOB->BRR = GPIO_Pin_0
#define CSN_HIGH_4() 	GPIOB->BSRR = GPIO_Pin_0
#define CE_LOW_4() 		GPIOA->BRR = GPIO_Pin_12
#define CE_HIGH_4() 	GPIOA->BSRR = GPIO_Pin_12
#define MISO_INPUT_4()	GPIO_ReadInputDataBit(SPI_MISO_PORT_4, SPI_MISO_PIN_4)
                     
/* Basic Operator define------------------------------------------------------*/
#define _SET_BIT(pos) ((uint8_t) (1 << ((uint8_t)(pos))))

/** The upper 8 bits of a 16 bit value */
#define MSB(a) ((a & 0xFF00) >> 8)
/** The lower 8 bits (of a 16 bit value) */
#define LSB(a) ((a & 0xFF))

/** Leaves the minimum of the two arguments */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/** Leaves the maximum of the two arguments */
#define MAX(a, b) ((a) < (b) ? (b) : (a))

#define BIT_0 0x01 /**< The value of bit 0 */
#define BIT_1 0x02 /**< The value of bit 1 */
#define BIT_2 0x04 /**< The value of bit 2 */
#define BIT_3 0x08 /**< The value of bit 3 */
#define BIT_4 0x10 /**< The value of bit 4 */
#define BIT_5 0x20 /**< The value of bit 5 */
#define BIT_6 0x40 /**< The value of bit 6 */
#define BIT_7 0x80 /**< The value of bit 7 */
#define BIT_8 0x0100 /**< The value of bit 8 */
#define BIT_9 0x0200 /**< The value of bit 9 */
#define BIT_10 0x0400 /**< The value of bit 10 */
#define BIT_11 0x0800 /**< The value of bit 11 */
#define BIT_12 0x1000 /**< The value of bit 12 */
#define BIT_13 0x2000 /**< The value of bit 13 */
#define BIT_14 0x4000 /**< The value of bit 14 */
#define BIT_15 0x8000 /**< The value of bit 15 */

/* Instruction Set define-----------------------------------------------------*/
#define W_REGISTER         0x20  /**< Register write command */
#define R_RX_PAYLOAD       0x61  /**< Read RX payload command */
#define W_TX_PAYLOAD       0xA0  /**< Write TX payload command */
#define FLUSH_TX           0xE1  /**< Flush TX register command */
#define FLUSH_RX           0xE2  /**< Flush RX register command */
#define REUSE_TX_PL        0xE3  /**< Reuse TX payload command */
#define ACTIVATE           0x50  /**< Activate features */
#define R_RX_PL_WID        0x60  /**< Read RX payload command */
#define W_ACK_PAYLOAD      0xA8  /**< Write ACK payload command */
#define W_TX_PAYLOAD_NOACK 0xB0  /**< Write ACK payload command */
#define NOP                0xFF  /**< No Operation command, used for reading status register */
#define READ_ONLY          0xFF  // Define No Operation, might be used to read status register


/* Register Memory Map define-------------------------------------------------*/
#define CONFIG        (uint8_t)0x00  /**< nRF24L01 config register */
#define EN_AA         (uint8_t)0x01  /**< nRF24L01 enable Auto-Acknowledge register */
#define EN_RXADDR     (uint8_t)0x02  /**< nRF24L01 enable RX addresses register */
#define SETUP_AW      (uint8_t)0x03  /**< nRF24L01 setup of address width register */
#define SETUP_RETR    (uint8_t)0x04  /**< nRF24L01 setup of automatic retransmission register */
#define RF_CH         (uint8_t)0x05  /**< nRF24L01 RF channel register */
#define RF_SETUP      (uint8_t)0x06  /**< nRF24L01 RF setup register */
#define STATUS_2      (uint8_t)0x07  /**< nRF24L01 status register */
#define OBSERVE_TX    (uint8_t)0x08  /**< nRF24L01 transmit observe register */
#define CD            (uint8_t)0x09  /**< nRF24L01 carrier detect register */
#define RX_ADDR_P0    (uint8_t)0x0A  /**< nRF24L01 receive address data pipe0 */
#define RX_ADDR_P1    (uint8_t)0x0B  /**< nRF24L01 receive address data pipe1 */
#define RX_ADDR_P2    (uint8_t)0x0C  /**< nRF24L01 receive address data pipe2 */
#define RX_ADDR_P3    (uint8_t)0x0D  /**< nRF24L01 receive address data pipe3 */
#define RX_ADDR_P4    (uint8_t)0x0E  /**< nRF24L01 receive address data pipe4 */
#define RX_ADDR_P5    (uint8_t)0x0F  /**< nRF24L01 receive address data pipe5 */
#define TX_ADDR       (uint8_t)0x10  /**< nRF24L01 transmit address */
#define RX_PW_P0      (uint8_t)0x11  /**< nRF24L01 \# of bytes in rx payload for pipe0 */
#define RX_PW_P1      (uint8_t)0x12  /**< nRF24L01 \# of bytes in rx payload for pipe1 */
#define RX_PW_P2      (uint8_t)0x13  /**< nRF24L01 \# of bytes in rx payload for pipe2 */
#define RX_PW_P3      (uint8_t)0x14  /**< nRF24L01 \# of bytes in rx payload for pipe3 */
#define RX_PW_P4      (uint8_t)0x15  /**< nRF24L01 \# of bytes in rx payload for pipe4 */
#define RX_PW_P5      (uint8_t)0x16  /**< nRF24L01 \# of bytes in rx payload for pipe5 */
#define FIFO_STATUS   (uint8_t)0x17  /**< nRF24L01 FIFO status register */
#define DYNPD_2       (uint8_t)0x1C  /**< nRF24L01 Dynamic payload setup */
#define FEATURE_2     (uint8_t)0x1D  /**< nRF24L01 Exclusive feature setup */
#define AGC_CONFIG    (uint8_t)0x1E  /**< nRF24L01+ AGC configuration register */

/* CONFIG register bit define-------------------------------------------------*/
#define MASK_RX_DR    6     /**< CONFIG register bit 6 */
#define MASK_TX_DS    5     /**< CONFIG register bit 5 */
#define MASK_MAX_RT   4     /**< CONFIG register bit 4 */
#define EN_CRC        3     /**< CONFIG register bit 3 */
#define CRCO          2     /**< CONFIG register bit 2 */
#define PWR_UP        1     /**< CONFIG register bit 1 */
#define PRIM_RX       0     /**< CONFIG register bit 0 */

/* RF_SETUP register bit define-----------------------------------------------*/
#define PLL_LOCK      4     /**< RF_SETUP register bit 4 */
#define RF_DR         3     /**< RF_SETUP register bit 3 */
#define RF_PWR1       2     /**< RF_SETUP register bit 2 */
#define RF_PWR0       1     /**< RF_SETUP register bit 1 */
#define LNA_HCURR     0     /**< RF_SETUP register bit 0 */

/* STATUS register bit define-------------------------------------------------*/
#define RX_DR         6     /**< STATUS register bit 6 */
#define TX_DS         5     /**< STATUS register bit 5 */
#define MAX_RT        4     /**< STATUS register bit 4 */
#define TX_FULL       0     /**< STATUS register bit 0 */

/* FIFO_STATUS register bit define--------------------------------------------*/
#define TX_REUSE      6     /**< FIFO_STATUS register bit 6 */
#define TX_FIFO_FULL  5     /**< FIFO_STATUS register bit 5 */
#define TX_EMPTY      4     /**< FIFO_STATUS register bit 4 */
#define RX_FULL       1     /**< FIFO_STATUS register bit 1 */
#define RX_EMPTY      0     /**< FIFO_STATUS register bit 0 */


/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
/* NRF3 ------------------------------------------------------- */
/* NRF3 ------------------------------------------------------- */
void hal_nrf_set_irq_mode(hal_nrf_irq_source_t int_source, bool irq_state);
void hal_nrf_enable_dynamic_payload(bool enable);
void hal_nrf_enable_ack_payload(bool enable);
void hal_nrf_enable_dynamic_ack(bool enable);
void hal_nrf_setup_dynamic_payload(uint8_t setup);
uint8_t hal_nrf_read_rx_payload_width(void);
void hal_nrf_write_ack_payload(uint8_t pipe, const uint8_t *tx_pload, uint8_t length);
uint8_t hal_nrf_get_clear_irq_flags(void);
void hal_nrf_clear_irq_flag(hal_nrf_irq_source_t int_source);
void hal_nrf_set_crc_mode(hal_nrf_crc_mode_t crc_mode);
void hal_nrf_open_pipe(hal_nrf_address_t pipe_num, bool auto_ack);
void hal_nrf_close_pipe(hal_nrf_address_t pipe_num);
void hal_nrf_set_address(const hal_nrf_address_t address, const uint8_t *addr);
uint8_t hal_nrf_get_address (uint8_t address, uint8_t *addr);
void hal_nrf_set_auto_retr(uint8_t retr, uint16_t delay);
void hal_nrf_set_address_width(hal_nrf_address_width_t address_width);
uint8_t hal_nrf_get_address_width (void);
void hal_nrf_set_rx_payload_width(uint8_t pipe_num, uint8_t pload_width);
bool hal_nrf_get_irq_mode(uint8_t int_source);
uint8_t hal_nrf_get_irq_flags(void);
uint8_t hal_nrf_get_pipe_status(uint8_t pipe_num);
uint8_t hal_nrf_get_auto_retr_status(void);
uint8_t hal_nrf_get_packet_lost_ctr(void);
uint8_t hal_nrf_get_rx_payload_width(uint8_t pipe_num);
void hal_nrf_set_operation_mode(hal_nrf_operation_mode_t op_mode);
void hal_nrf_set_power_mode(hal_nrf_pwr_mode_t pwr_mode);
void hal_nrf_set_rf_channel(uint8_t channel);
void hal_nrf_set_output_power(hal_nrf_output_power_t power);
void hal_nrf_set_datarate(hal_nrf_datarate_t datarate);
uint8_t hal_nrf_get_tx_fifo_status(void);
bool hal_nrf_tx_fifo_empty(void);
bool hal_nrf_tx_fifo_full(void);
uint8_t hal_nrf_get_rx_fifo_status(void);
bool hal_nrf_rx_fifo_empty(void);
bool hal_nrf_rx_fifo_full(void);
uint8_t hal_nrf_get_fifo_status(void);
uint8_t hal_nrf_get_transmit_attempts(void);
uint8_t hal_nrf_get_carrier_detect(void);
uint8_t hal_nrf_get_rx_data_source(void);
uint16_t hal_nrf_read_rx_payload(uint8_t *rx_pload);
void hal_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_write_tx_payload_noack(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_reuse_tx(void);
bool hal_nrf_get_reuse_tx_status(void);
void hal_nrf_flush_rx(void);
void hal_nrf_flush_tx(void);
uint8_t hal_nrf_nop(void);
void hal_nrf_set_pll_mode(bool pll_lock);
void hal_nrf_set_lna_gain(bool lna_gain);
void hal_nrf_enable_continious_wave (bool enable);

uint8_t hal_nrf_rw(uint8_t value);
uint8_t hal_nrf_read_reg(uint8_t reg);
uint8_t hal_nrf_write_reg(uint8_t reg, uint8_t value);
uint16_t hal_nrf_read_multibyte_reg(uint8_t reg, uint8_t *pbuf);
void hal_nrf_write_multibyte_reg(uint8_t reg, const uint8_t *pbuf, uint8_t length);

/* NRF2 ------------------------------------------------------- */
/* NRF2 ------------------------------------------------------- */
void hal_nrf_set_irq_mode_2(hal_nrf_irq_source_t int_source, bool irq_state);
void hal_nrf_enable_dynamic_payload_2(bool enable);
void hal_nrf_enable_ack_payload_2(bool enable);
void hal_nrf_enable_dynamic_ack_2(bool enable);
void hal_nrf_setup_dynamic_payload_2(uint8_t setup);
uint8_t hal_nrf_read_rx_payload_width_2(void);
void hal_nrf_write_ack_payload_2(uint8_t pipe, const uint8_t *tx_pload, uint8_t length);
uint8_t hal_nrf_get_clear_irq_flags_2(void);
void hal_nrf_clear_irq_flag_2(hal_nrf_irq_source_t int_source);
void hal_nrf_set_crc_mode_2(hal_nrf_crc_mode_t crc_mode);
void hal_nrf_open_pipe_2(hal_nrf_address_t pipe_num, bool auto_ack);
void hal_nrf_close_pipe_2(hal_nrf_address_t pipe_num);
void hal_nrf_set_address_2(const hal_nrf_address_t address, const uint8_t *addr);
uint8_t hal_nrf_get_address_2(uint8_t address, uint8_t *addr);
void hal_nrf_set_auto_retr_2(uint8_t retr, uint16_t delay);
void hal_nrf_set_address_width_2(hal_nrf_address_width_t address_width);
uint8_t hal_nrf_get_address_width_2(void);
void hal_nrf_set_rx_payload_width_2(uint8_t pipe_num, uint8_t pload_width);
bool hal_nrf_get_irq_mode_2(uint8_t int_source);
uint8_t hal_nrf_get_irq_flags_2(void);
uint8_t hal_nrf_get_pipe_status_2(uint8_t pipe_num);
uint8_t hal_nrf_get_auto_retr_status_2(void);
uint8_t hal_nrf_get_packet_lost_ctr_2(void);
uint8_t hal_nrf_get_rx_payload_width_2(uint8_t pipe_num);
void hal_nrf_set_operation_mode_2(hal_nrf_operation_mode_t op_mode);
void hal_nrf_set_power_mode_2(hal_nrf_pwr_mode_t pwr_mode);
void hal_nrf_set_rf_channel_2(uint8_t channel);
void hal_nrf_set_output_power_2(hal_nrf_output_power_t power);
void hal_nrf_set_datarate_2(hal_nrf_datarate_t datarate);
uint8_t hal_nrf_get_tx_fifo_status_2(void);
bool hal_nrf_tx_fifo_empty_2(void);
bool hal_nrf_tx_fifo_full_2(void);
uint8_t hal_nrf_get_rx_fifo_status_2(void);
bool hal_nrf_rx_fifo_empty_2(void);
bool hal_nrf_rx_fifo_full_2(void);
uint8_t hal_nrf_get_fifo_status_2(void);
uint8_t hal_nrf_get_transmit_attempts_2(void);
uint8_t hal_nrf_get_carrier_detect_2(void);
uint8_t hal_nrf_get_rx_data_source_2(void);
uint16_t hal_nrf_read_rx_payload_2(uint8_t *rx_pload);
void hal_nrf_write_tx_payload_2(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_write_tx_payload_noack_2(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_reuse_tx_2(void);
bool hal_nrf_get_reuse_tx_status_2(void);
void hal_nrf_flush_rx_2(void);
void hal_nrf_flush_tx_2(void);
uint8_t hal_nrf_nop_2(void);
void hal_nrf_set_pll_mode_2(bool pll_lock);
void hal_nrf_set_lna_gain_2(bool lna_gain);
void hal_nrf_enable_continious_wave_2(bool enable);

uint8_t hal_nrf_rw_2(uint8_t value);
uint8_t hal_nrf_read_reg_2(uint8_t reg);
uint8_t hal_nrf_write_reg_2(uint8_t reg, uint8_t value);
uint16_t hal_nrf_read_multibyte_reg_2(uint8_t reg, uint8_t *pbuf);
void hal_nrf_write_multibyte_reg_2(uint8_t reg, const uint8_t *pbuf, uint8_t length);

/* NRF3 ------------------------------------------------------- */
/* NRF3 ------------------------------------------------------- */
void hal_nrf_set_irq_mode_3(hal_nrf_irq_source_t int_source, bool irq_state);
void hal_nrf_enable_dynamic_payload_3(bool enable);
void hal_nrf_enable_ack_payload_3(bool enable);
void hal_nrf_enable_dynamic_ack_3(bool enable);
void hal_nrf_setup_dynamic_payload_3(uint8_t setup);
uint8_t hal_nrf_read_rx_payload_width_3(void);
void hal_nrf_write_ack_payload_3(uint8_t pipe, const uint8_t *tx_pload, uint8_t length);
uint8_t hal_nrf_get_clear_irq_flags_3(void);
void hal_nrf_clear_irq_flag_3(hal_nrf_irq_source_t int_source);
void hal_nrf_set_crc_mode_3(hal_nrf_crc_mode_t crc_mode);
void hal_nrf_open_pipe_3(hal_nrf_address_t pipe_num, bool auto_ack);
void hal_nrf_close_pipe_3(hal_nrf_address_t pipe_num);
void hal_nrf_set_address_3(const hal_nrf_address_t address, const uint8_t *addr);
uint8_t hal_nrf_get_address_3(uint8_t address, uint8_t *addr);
void hal_nrf_set_auto_retr_3(uint8_t retr, uint16_t delay);
void hal_nrf_set_address_width_3(hal_nrf_address_width_t address_width);
uint8_t hal_nrf_get_address_width_3(void);
void hal_nrf_set_rx_payload_width_3(uint8_t pipe_num, uint8_t pload_width);
bool hal_nrf_get_irq_mode_3(uint8_t int_source);
uint8_t hal_nrf_get_irq_flags_3(void);
uint8_t hal_nrf_get_pipe_status_3(uint8_t pipe_num);
uint8_t hal_nrf_get_auto_retr_status_3(void);
uint8_t hal_nrf_get_packet_lost_ctr_3(void);
uint8_t hal_nrf_get_rx_payload_width_3(uint8_t pipe_num);
void hal_nrf_set_operation_mode_3(hal_nrf_operation_mode_t op_mode);
void hal_nrf_set_power_mode_3(hal_nrf_pwr_mode_t pwr_mode);
void hal_nrf_set_rf_channel_3(uint8_t channel);
void hal_nrf_set_output_power_3(hal_nrf_output_power_t power);
void hal_nrf_set_datarate_3(hal_nrf_datarate_t datarate);
uint8_t hal_nrf_get_tx_fifo_status_3(void);
bool hal_nrf_tx_fifo_empty_3(void);
bool hal_nrf_tx_fifo_full_3(void);
uint8_t hal_nrf_get_rx_fifo_status_3(void);
bool hal_nrf_rx_fifo_empty_3(void);
bool hal_nrf_rx_fifo_full_3(void);
uint8_t hal_nrf_get_fifo_status_3(void);
uint8_t hal_nrf_get_transmit_attempts_3(void);
uint8_t hal_nrf_get_carrier_detect_3(void);
uint8_t hal_nrf_get_rx_data_source_3(void);
uint16_t hal_nrf_read_rx_payload_3(uint8_t *rx_pload);
void hal_nrf_write_tx_payload_3(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_write_tx_payload_noack_3(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_reuse_tx_3(void);
bool hal_nrf_get_reuse_tx_status_3(void);
void hal_nrf_flush_rx_3(void);
void hal_nrf_flush_tx_3(void);
uint8_t hal_nrf_nop_3(void);
void hal_nrf_set_pll_mode_3(bool pll_lock);
void hal_nrf_set_lna_gain_3(bool lna_gain);
void hal_nrf_enable_continious_wave_3(bool enable);

uint8_t hal_nrf_rw_3(uint8_t value);
uint8_t hal_nrf_read_reg_3(uint8_t reg);
uint8_t hal_nrf_write_reg_3(uint8_t reg, uint8_t value);
uint16_t hal_nrf_read_multibyte_reg_3(uint8_t reg, uint8_t *pbuf);
void hal_nrf_write_multibyte_reg_3(uint8_t reg, const uint8_t *pbuf, uint8_t length);


/* NRF4 ------------------------------------------------------- */
/* NRF4 ------------------------------------------------------- */
void hal_nrf_set_irq_mode_4(hal_nrf_irq_source_t int_source, bool irq_state);
void hal_nrf_enable_dynamic_payload_4(bool enable);
void hal_nrf_enable_ack_payload_4(bool enable);
void hal_nrf_enable_dynamic_ack_4(bool enable);
void hal_nrf_setup_dynamic_payload_4(uint8_t setup);
uint8_t hal_nrf_read_rx_payload_width_4(void);
void hal_nrf_write_ack_payload_4(uint8_t pipe, const uint8_t *tx_pload, uint8_t length);
uint8_t hal_nrf_get_clear_irq_flags_4(void);
void hal_nrf_clear_irq_flag_4(hal_nrf_irq_source_t int_source);
void hal_nrf_set_crc_mode_4(hal_nrf_crc_mode_t crc_mode);
void hal_nrf_open_pipe_4(hal_nrf_address_t pipe_num, bool auto_ack);
void hal_nrf_close_pipe_4(hal_nrf_address_t pipe_num);
void hal_nrf_set_address_4(const hal_nrf_address_t address, const uint8_t *addr);
uint8_t hal_nrf_get_address_4(uint8_t address, uint8_t *addr);
void hal_nrf_set_auto_retr_4(uint8_t retr, uint16_t delay);
void hal_nrf_set_address_width_4(hal_nrf_address_width_t address_width);
uint8_t hal_nrf_get_address_width_4(void);
void hal_nrf_set_rx_payload_width_4(uint8_t pipe_num, uint8_t pload_width);
bool hal_nrf_get_irq_mode_4(uint8_t int_source);
uint8_t hal_nrf_get_irq_flags_4(void);
uint8_t hal_nrf_get_pipe_status_4(uint8_t pipe_num);
uint8_t hal_nrf_get_auto_retr_status_4(void);
uint8_t hal_nrf_get_packet_lost_ctr_4(void);
uint8_t hal_nrf_get_rx_payload_width_4(uint8_t pipe_num);
void hal_nrf_set_operation_mode_4(hal_nrf_operation_mode_t op_mode);
void hal_nrf_set_power_mode_4(hal_nrf_pwr_mode_t pwr_mode);
void hal_nrf_set_rf_channel_4(uint8_t channel);
void hal_nrf_set_output_power_4(hal_nrf_output_power_t power);
void hal_nrf_set_datarate_4(hal_nrf_datarate_t datarate);
uint8_t hal_nrf_get_tx_fifo_status_4(void);
bool hal_nrf_tx_fifo_empty_4(void);
bool hal_nrf_tx_fifo_full_4(void);
uint8_t hal_nrf_get_rx_fifo_status_4(void);
bool hal_nrf_rx_fifo_empty_4(void);
bool hal_nrf_rx_fifo_full_4(void);
uint8_t hal_nrf_get_fifo_status_4(void);
uint8_t hal_nrf_get_transmit_attempts_4(void);
uint8_t hal_nrf_get_carrier_detect_4(void);
uint8_t hal_nrf_get_rx_data_source_4(void);
uint16_t hal_nrf_read_rx_payload_4(uint8_t *rx_pload);
void hal_nrf_write_tx_payload_4(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_write_tx_payload_noack_4(const uint8_t *tx_pload, uint8_t length);
void hal_nrf_reuse_tx_4(void);
bool hal_nrf_get_reuse_tx_status_4(void);
void hal_nrf_flush_rx_4(void);
void hal_nrf_flush_tx_4(void);
uint8_t hal_nrf_nop_4(void);
void hal_nrf_set_pll_mode_4(bool pll_lock);
void hal_nrf_set_lna_gain_4(bool lna_gain);
void hal_nrf_enable_continious_wave_4(bool enable);

uint8_t hal_nrf_rw_4(uint8_t value);
uint8_t hal_nrf_read_reg_4(uint8_t reg);
uint8_t hal_nrf_write_reg_4(uint8_t reg, uint8_t value);
uint16_t hal_nrf_read_multibyte_reg_4(uint8_t reg, uint8_t *pbuf);
void hal_nrf_write_multibyte_reg_4(uint8_t reg, const uint8_t *pbuf, uint8_t length);


#endif


/*******************END OF FILE***********************************************/









