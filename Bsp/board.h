/**
  ******************************************************************************
  * @file   	board.h
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board infomation
  ******************************************************************************
  */

#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32f10x.h"
#include "main.h"
#include "define.h"
#include "whitelist.h"
#include "nrf.h"
#include "app_timer.h"

/* board name defines --------------------------------------------------------*/
/* NVIC defines ---------------------------------------------------------------*/
/* set system nvic group */
#define SYSTEM_MVIC_GROUP_SET         NVIC_PriorityGroup_2

/* Peripheral interrupt preemption set */
#define UART1_PREEMPTION_PRIORITY     1
#define UART2_PREEMPTION_PRIORITY     1
#define NRF1_PREEMPTION_PRIORITY      0
#define NRF2_PREEMPTION_PRIORITY      1
#define TIM3_PREEMPTION_PRIORITY      0

/* Peripheral interrupt response set */
#define UART1_SUB_PRIORITY            1
#define UART2_SUB_PRIORITY            1
#define NRF_SUB_PRIORITY              0 
#define TIM3_SUB_PRIORITY             1

/* USART defines -------------------------------------------------------------*/
#define USART1pos                     USART1
#define USART1pos_GPIO                GPIOA
#define USART1pos_CLK                 RCC_APB2Periph_USART1
#define USART1pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART1pos_RxPin               GPIO_Pin_10
#define USART1pos_TxPin               GPIO_Pin_9
#define USART1pos_IRQn                USART1_IRQn
#define USART1pos_IRQHandler          USART1_IRQHandler

#define USART2pos                     USART2
#define USART2pos_GPIO                GPIOA
#define USART2pos_CLK                 RCC_APB1Periph_USART2
#define USART2pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART2pos_RxPin               GPIO_Pin_3
#define USART2pos_TxPin               GPIO_Pin_2
#define USART2pos_IRQn                USART2_IRQn
#define USART2pos_IRQHandler          USART2_IRQHandler

//#define ZL_RP551_MAIN_E
//#define ZL_RP551_MAIN_F
//#define ZL_RP551_MAIN_H

#if (!defined (ZL_RP551_MAIN_E) && !defined (ZL_RP551_MAIN_F)) && !defined (ZL_RP551_MAIN_H)
 #error "Please select board used in your application (in board.h file)"
#endif

#if (defined (ZL_RP551_MAIN_E) && defined (ZL_RP551_MAIN_F)) && defined (ZL_RP551_MAIN_H)
 #error "Please select only one board used in your application (in board.h file)"
#endif

#ifdef ZL_RP551_MAIN_E
#include "bsp_rp551_e.h"
#endif

#ifdef ZL_RP551_MAIN_F
#include "bsp_rp551_f.h"
#endif

#ifdef ZL_RP551_MAIN_H
#include "bsp_rp551_h.h"
#endif

typedef struct 
{
	uint8_t cmd;
	uint8_t	tx_buf[RF_NBUF];					
	uint8_t tx_len;
}RF_TypeDef;

typedef struct
{
	uint8_t  uid[4];
	uint16_t pos;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  rssi;
}Clicker_Typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint8_t  uart_pac_num;
	uint8_t  uart_seq_num;
	uint32_t data_statistic_count;
	uint8_t  addr_clone_flag;
}revicer_typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint16_t   len;
	uint16_t   first_uid_pos;
	uint8_t    is_printf_clear_uid;
	uint8_t    clear_uid[4];
	uint8_t    switch_status;
	uint8_t    start;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
	uint8_t    weite_std_id_status;
}wl_typedef;

extern bool 			gbf_hse_setup_fail; //外部16M晶振起振标志
extern RF_TypeDef rf_var;							// 2.4G数据包缓冲
extern uint8_t 		respon[];

typedef struct
{
	uint8_t header;
	uint8_t cmd;
	uint8_t channel;
	uint8_t data_xor;
	uint8_t end;
}cpu_spi_cmd_typedef;

extern wl_typedef       wl;
extern uint8_t dtq_self_inspection_flg;

void systick_timer_init( void );

/* platform misc functions's declaritions */
uint8_t XOR_Cal(uint8_t *data, uint16_t length);
void board_init(void);
void bsp_uart_init(void);
uint8_t spi_set_cpu_tx_signal_ch( uint8_t tx_ch );
uint8_t spi_set_cpu_rx_signal_ch( uint8_t rx_ch );

#endif //_BOARD_H_

/**************************************END OF FILE****************************/
