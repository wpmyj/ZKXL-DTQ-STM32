#ifndef	__APP_SEND_DATA_PROCESS_H_
#define	__APP_SEND_DATA_PROCESS_H_

#include "main.h"
#include "app_show_message_process.h"

//#define OPEN_BUFFER_ACK_SHOW
//#define OPEN_BUFFER_DATA_SHOW
//#define OPEN_ACK_TABLE_SHOW
//#define OPEN_SEND_STATUS_SHOW
//#define OPEN_SEND_STATUS_MESSAGE_SHOW
//#define RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
//#define SEND_DATA_DETAIL_MESSAGE_SHOW
//#define SEND_DATA_UID_MESSAGE_SHOW
//#define SUM_DATA_STATISTICS
#define ENABLE_SEND_DATA_TO_PC
//#define ENABLE_SET_SEND_DATA_PARAMETER


#ifdef 	ENABLE_SET_SEND_DATA_PARAMETER
#define DEBUG_SET_SEND_DATA_PARAMETER					 DEBUG_LOG
#else
#define DEBUG_SET_SEND_DATA_PARAMETER(...)
#endif

/* 打印信息控制 */
#ifdef 	OPEN_BUFFER_ACK_SHOW
#define DEBUG_BUFFER_ACK_LOG							     DEBUG_LOG
#else
#define DEBUG_BUFFER_ACK_LOG(...)
#endif

#ifdef 	OPEN_BUFFER_DATA_SHOW
#define DEBUG_BUFFER_DTATA_LOG							     DEBUG_LOG
#else
#define DEBUG_BUFFER_DTATA_LOG(...)
#endif

#ifdef 	RETRANSMIT_DATA_DETAIL_MESSAGE_SHOW
#define DEBUG_DATA_DETAIL_LOG							     DEBUG_LOG
#else
#define DEBUG_DATA_DETAIL_LOG(...)
#endif

#ifdef 	SEND_DATA_UID_MESSAGE_SHOW
#define DEBUG_UID_LOG							             DEBUG_LOG
#else
#define DEBUG_UID_LOG(...)
#endif

#ifdef 	SUM_DATA_STATISTICS
#define DEBUG_STATISTICS_LOG							     DEBUG_LOG
#else
#define DEBUG_STATISTICS_LOG(...)
#endif

/* 空闲 */
#define SEND_IDLE_STATUS            0
#define RETRANSMIT_SEND_DATA_COUNT  (send_data_process_tcb.retransmit_count)//5

/* 前导帧 */
#define SEND_PRE_STATUS             (SEND_IDLE_STATUS+1)            // 1
#define SEND_PRE_COUNT              (send_data_process_tcb.pre_data_count)
#define SEND_PRE_DELAY100US         (send_data_process_tcb.pre_data_delay100us)
#define SEND_PRE_TABLE              12

/* 数据帧 */
#define SEND_DATA_COUNT             2
#define SEND_DATA_DELAY100US        (send_data_process_tcb.data_delay100us)//50
#define SEND_DATA_ACK_TABLE         2

/* 广播帧 */
#define SEND_DATA1_STATUS           (SEND_PRE_STATUS+1)             // 2
#define SEND_DATA1_SUM_TABLE        0
#define SEND_DATA1_TIMEOUT          (send_data_process_tcb.rand_delayms)
#define SEND_DATA1_ACK_TABLE        3
#define SEND_DATA1_UPDATE_STATUS    (SEND_DATA1_STATUS+1)           // 3

/* 第1次重发帧 */
#define SEND_DATA2_STATUS           (SEND_DATA1_UPDATE_STATUS+1)    // 4
#define SEND_DATA2_SUM_TABLE        4
#define SEND_DATA2_TIMEOUT          (send_data_process_tcb.rand_delayms)
#define SEND_DATA2_ACK_TABLE        5
#define SEND_DATA2_SEND_OVER_STATUS (SEND_DATA2_STATUS+1)           // 5
#define SEND_DATA2_UPDATE_STATUS    (SEND_DATA2_SEND_OVER_STATUS+1) // 6

/* 第2次重发帧 */
#define SEND_DATA3_STATUS           (SEND_DATA2_UPDATE_STATUS+1)    // 7
#define SEND_DATA3_SUM_TABLE        6
#define SEND_DATA3_TIMEOUT          (send_data_process_tcb.rand_delayms)
#define SEND_DATA3_ACK_TABLE        7
#define SEND_DATA3_SEND_OVER_STATUS (SEND_DATA3_STATUS+1)           // 8
#define SEND_DATA3_UPDATE_STATUS    (SEND_DATA3_SEND_OVER_STATUS+1) // 9

/* 第3次重发帧 */
#define SEND_DATA4_STATUS           (SEND_DATA3_UPDATE_STATUS+1)    // 10
#define SEND_DATA4_SUM_TABLE        8
#define SEND_DATA4_TIMEOUT          (send_data_process_tcb.rand_delayms)
#define SEND_DATA4_ACK_TABLE        9
#define SEND_DATA4_UPDATE_STATUS    (SEND_DATA4_STATUS+2)           // 12

/* 定向发送 */
#define SINGLE_SEND_DATA_TIMEOUT    100
#define SINGLE_SEND_DATA_COUNT_MAX  10
#define SINGLE_SEND_DATA_ACK_TABLE  1

#define PACKAGE_NUM_ADD             1
#define PACKAGE_NUM_SAM             0
#define REQUEST_TABLE               13
#define REQUEST_TEMP_PRE_TABLE      14
#define REQUEST_TEMP_ACK_TABLE      15
#define RETRANSMIT_DATA_TIME_UNIT   3000

typedef struct
{
	uint8_t count;
	uint8_t sum;
	uint8_t uid[4];
	uint8_t pos;
	uint8_t status;
}retransmit_tcb_tydef;

typedef struct
{
	uint8_t  clicker_count;
	uint8_t  Is_ok_over;
	uint8_t  Is_lost_over;
	uint16_t lostuidlen;
	uint16_t okuidlen;
}message_tcb_tydef;

typedef struct
{
	uint8_t  pre_data_count;
	uint16_t pre_data_delay100us;
	uint8_t  data_count;
	uint16_t data_delay100us;
	uint16_t rand_delayms;
	uint8_t  retransmit_count;
}send_data_process_tcb_tydef;

extern task_tcb_typedef send_data_task;
extern volatile send_data_process_tcb_tydef send_data_process_tcb;

void change_clicker_send_data_status( uint8_t newstatus );
uint8_t get_clicker_send_data_status( void );
uint8_t spi_buffer_status_check( uint8_t status );
void rf_retransmit_set_status(uint8_t new_status);
uint8_t get_rf_retransmit_status(void);
void whitelist_checktable_or(uint8_t table1, uint8_t table2);
void whitelist_checktable_and(uint8_t table1, uint8_t table2, uint8_t table);
void clicker_send_data_statistics( uint8_t send_data_status, uint16_t uidpos );
uint8_t checkout_online_uids(uint8_t src_table, uint8_t check_table, uint8_t mode, uint8_t *puid,uint8_t *len);
uint8_t spi_process_revice_data( void );
void send_data_result( uint8_t status );
void get_retransmit_messsage( uint8_t status );
void retansmit_data( uint8_t status );
void App_clickers_send_data_process( void );
void send_data_env_init(void);
void send_data_process_timer_init( void );
void set_retranmist_data_status(uint8_t new_status);
#endif
