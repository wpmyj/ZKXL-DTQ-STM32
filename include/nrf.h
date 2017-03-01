#ifndef	__NRF_H_
#define	__NRF_H_

#include <stdint.h>
#include "stm32f10x_spi.h"
#include "board.h"

/* nrf configuration define---------------------------------------------------*/
#define NRF_MAX_NUMBER_OF_RETRANSMITS		(3)				//����ط�����
#define	NRF_RETRANSMIT_DELAY			      (280)				//�ط���ʱ ms
#define	NRF_ACK_PROTOCOL_LEN			      (10)				//�Զ�������ģ��ACKЭ�����ݳ���
#define	NRF_TOTAL_DATA_LEN				      (250)				//2.4G�����ܳ���
#define	NRF_USEFUL_DATA_LEN	            (NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)		//��Ч���ݳ���
#define	NRF_DATA_IS_USEFUL				      (0)
#define NRF_DATA_IS_ACK				          (1)
#define NRF_DATA_IS_PRE				          (2)

#define BUFFER_SIZE_MAX							    (255)

/* STATUS register bit define-------------------------------------------------*/
#define RX_DR                           6     /**< STATUS register bit 6 */
#define TX_DS                           5     /**< STATUS register bit 5 */
#define MAX_RT                          4     /**< STATUS register bit 4 */
#define TX_FULL                         0     /**< STATUS register bit 0 */


/* STATUS register bit define-------------------------------------------------*/
typedef enum {
    UESB_MODE_POWERUP,          													// power up
    UESB_MODE_POWERDOWN         													// power down
} uesb_power_mode_t;

typedef enum {
  UESB_MODE_PTX,          														// Primary transmitter
  UESB_MODE_PRX,          														// Primary receiver
	UESB_MODE_IDLE																	// idle mode
} uesb_mode_t;

typedef enum {
    RADIO_MODE_MODE_Nrf_1Mbit,
    RADIO_MODE_MODE_Nrf_2Mbit,
    RADIO_MODE_MODE_Nrf_250Kbit
} uesb_bitrate_t;

typedef enum {
    RADIO_CRCCNF_LEN_Disabled,
	RADIO_CRCCNF_LEN_One,
    RADIO_CRCCNF_LEN_Two,
} uesb_crc_t;

typedef enum {
    RADIO_TXPOWER_TXPOWER_Pos4dBm 		= 0x04UL,
    RADIO_TXPOWER_TXPOWER_0dBm			  = 0x00UL,
    RADIO_TXPOWER_TXPOWER_Neg4dBm		  = 0xFCUL,
    RADIO_TXPOWER_TXPOWER_Neg8dBm		  = 0xF8UL,
    RADIO_TXPOWER_TXPOWER_Neg12dBm		= 0xF4UL,
    RADIO_TXPOWER_TXPOWER_Neg16dBm		= 0xF0UL,
    RADIO_TXPOWER_TXPOWER_Neg20dBm		= 0xECUL,
    RADIO_TXPOWER_TXPOWER_Neg30dBm		= 0xD8UL
} uesb_tx_power_t;

typedef enum {
  UESB_WRITE_PARAM 				    = 0x80,
  UESB_FLUSH_TX 					    = 0x81,
  UESB_FLUSH_RX 					    = 0x82,
	UESB_WRITE_TX_PAYLOAD 			= 0x83,
	UESB_WRITE_TX_PAYLOAD_NOACK = 0x84,
	UESB_READ_RX_PAYLOAD 			  = 0x85,
	UESB_READ_RF_INT_STATUS 		= 0x86,
	UESB_CLEAR_RF_INT_STATUS 		= 0x87,
	UESB_SWITCH_TX_RX	 			    = 0x88,
} spi_cmd_t;



typedef struct
{
	uint8_t	 						      dtq_uid[4];				//�˴�����UID
	uint8_t							      jsq_uid[4];				//��֮��ԵĽ�����UID
	uint8_t							      rlen;
	uint8_t							      tlen;
	uint8_t							      rbuf[NRF_TOTAL_DATA_LEN];		//�������ݷ�����
	uint8_t							      tbuf[NRF_TOTAL_DATA_LEN];		//�������ݷ�����
}nrf_communication_t;

typedef struct
{
	uint8_t 				spi_cmd;
	uint8_t         count;
	uint8_t         delay100us;
	uint8_t					data_len;
	uint8_t 				data[BUFFER_SIZE_MAX];
	uint8_t 				xor;														//Ϊ�������������
} spi_cmd_type_t;

typedef struct
{
	uint8_t 				spi_cmd;
	uint8_t         signal;
	uint8_t					data_len;
	uint8_t 				data[BUFFER_SIZE_MAX];
	uint8_t 				xor;														//Ϊ�������������
} nrf_to_stm32_cmd_type_t;

typedef struct
{
	uint8_t dist[4];
	uint8_t transmit_count;
	uint8_t delay100us;
	uint8_t is_pac_add;
	uint8_t package_type;
	uint8_t *data_buf;
	uint8_t data_len;
	uint8_t is_add_table;
	uint8_t sel_table;
} nrf_transmit_parameter_t;

/* Private functions ---------------------------------------------------------*/
uint8_t uesb_nrf_get_irq_flags(SPI_TypeDef* SPIx, uint8_t *flags, uint8_t *rx_data_len, uint8_t *rx_data);
uint8_t uesb_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us);
uint8_t uesb_nrf_write_tx_payload_noack(const uint8_t *tx_pload, uint8_t length);

void nrf_transmit_start( nrf_transmit_parameter_t *t_conf);
void my_nrf_transmit_tx_success_handler(void);
void my_nrf_transmit_tx_failed_handler(void);
void my_nrf_receive_success_handler(void);

void nrf51822_spi_init(void);
void nrf51822_parameters_init(void);
void TIM3_Int_Init(u16 arr,u16 psc);

#endif //__NRF_H_
