#ifndef	__APP_SEND_DATA_PROCESS_H_
#define	__APP_SEND_DATA_PROCESS_H_

#include "main.h"

/* 空闲 */
#define SEND_IDLE_STATUS            0
#define SEND_PRE_TIMEOUT            50 // ms

/* 前导帧 */
#define SEND_PRE_STATUS             (SEND_IDLE_STATUS+1)            // 1
#define SEND_PRE_COUNT              30 
#define SEND_PRE_DELAY100US         20  // 100us

/* 数据帧 */
#define SEND_DATA_COUNT             2
#define SEND_DATA_DELAY100US        50
#define SEND_DATA_ACK_TABLE         2

/* 广播帧 */
#define SEND_DATA1_STATUS           (SEND_PRE_STATUS+1)             // 2
#define SEND_DATA1_SUM_TABLE        0
#define SEND_DATA1_TIMEOUT          2050
#define SEND_DATA1_ACK_TABLE        3
#define SEND_DATA1_UPDATE_STATUS    (SEND_DATA1_STATUS+1)           // 3

/* 第1次重发帧 */
#define SEND_DATA2_STATUS           (SEND_DATA1_UPDATE_STATUS+1)    // 4
#define SEND_DATA2_SUM_TABLE        4
#define SEND_DATA2_TIMEOUT          2000
#define SEND_DATA2_ACK_TABLE        5
#define SEND_DATA2_SEND_OVER_STATUS (SEND_DATA2_STATUS+1)           // 5
#define SEND_DATA2_UPDATE_STATUS    (SEND_DATA2_SEND_OVER_STATUS+1) // 6

/* 第2次重发帧 */
#define SEND_DATA3_STATUS           (SEND_DATA2_UPDATE_STATUS+1)    // 7
#define SEND_DATA3_SUM_TABLE        6
#define SEND_DATA3_TIMEOUT          2000
#define SEND_DATA3_ACK_TABLE        7
#define SEND_DATA3_SEND_OVER_STATUS (SEND_DATA3_STATUS+1)           // 8
#define SEND_DATA3_UPDATE_STATUS    (SEND_DATA3_SEND_OVER_STATUS+1) // 9

/* 第3次重发帧 */
#define SEND_DATA4_STATUS           (SEND_DATA3_UPDATE_STATUS+1)    // 10
#define SEND_DATA4_SUM_TABLE        8
#define SEND_DATA4_TIMEOUT          2000
#define SEND_DATA4_ACK_TABLE        9
#define SEND_DATA4_UPDATE_STATUS    (SEND_DATA4_STATUS+1)           // 11



//#define OPEN_ACK_SHOW
//#define OPEN_ACT_TABLE_SHOW
//#define OPEN_SEND_STATUS_SHOW
#define   SEND_DATA_DETAIL_MESSAGE_SHOW

void change_clicker_send_data_status( uint8_t newstatus );
uint8_t get_clicker_send_data_status( void );
uint8_t spi_buffer_status_check( uint8_t status );
void rf_retransmit_set_status(uint8_t new_status);
uint8_t get_rf_retransmit_status(void);
void clear_uid_check_table( void );
void whitelist_checktable_or(uint8_t table1, uint8_t table2);
void clicker_send_data_statistics( uint8_t send_data_status, uint8_t uidpos );
bool checkout_online_uids(uint8_t src_table, uint8_t check_table, uint8_t mode, uint8_t *puid,uint8_t *len);
uint8_t spi_process_revice_data( void );
void send_data_result( uint8_t status );
void get_retransmit_messsage( uint8_t status );
void retansmit_data( uint8_t status );
void App_clickers_send_data_process( void );

#endif
