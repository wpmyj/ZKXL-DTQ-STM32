#ifndef	__WHITE_LIST_H_
#define	__WHITE_LIST_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define UID_LIST_TABLE_SUM                (16)
#define MAX_WHITE_LEN						          (120)
#define WHITE_TABLE_LEN                   (8)
#define WHITE_LIST_LEN_POS_OF_FEE         (481)
#define WHITE_LIST_SW_POS_OF_FEE          (482)
#define WHITE_LIST_USE_TABLE_POS_OF_FEE   (483)
#define CPU_TX_CH_POS_OF_FEE              (WHITE_LIST_USE_TABLE_POS_OF_FEE+WHITE_TABLE_LEN)
#define CPU_RX_CH_POS_OF_FEE              (1+CPU_TX_CH_POS_OF_FEE)
#define CPU_TX_POWER_POS_OF_FEE           (CPU_RX_CH_POS_OF_FEE+1)

#define OPERATION_NOUSE                   (2)
#define OPERATION_ERR                     (1)
#define OPERATION_SUCCESS                 (0)

#define UID_USE_TABLE                     (0)
#define UID_ONLINE_TABLE                  (1)
#define UID_ONLINE_TEMP_TABLE             (2)

#ifdef 	ENABLE_DEBUG_LOG
#define WhiteListDebug								    printf
#else
#define WhiteListDebug(...)
#endif

typedef enum 
{
	OFF 	= 0,
	ON 	= 1
}switch_state_typedef;

typedef struct
{
	uint8_t  N_CH_TX;
	uint8_t  N_CH_RX;
	int8_t   N_TX_POWER;
}clicker_config_typedef;

extern uint8_t 	              g_cSNR[10];						// M1ø®–Ú¡–∫≈
extern clicker_config_typedef clicker_set;

uint8_t get_index_of_uid( uint16_t index, uint8_t  uid[4] );
void clear_index_of_uid(uint16_t index);
void add_index_of_uid( uint16_t index, uint8_t  uid[4] );

uint8_t initialize_white_list( void );
uint8_t uidcmp(uint8_t *uid1, uint8_t *uid2);
uint8_t add_uid_to_white_list(uint8_t *g_uid, uint16_t *position);
void clear_white_list(void);
uint8_t delete_uid_from_white_list(uint8_t *g_uid);
uint8_t search_uid_in_white_list(uint8_t *g_uid , uint16_t *position);
uint8_t store_len_to_fee(uint16_t len);
uint16_t get_len_of_white_list(void);
uint8_t store_switch_status_to_fee(uint8_t switch_status);
uint8_t get_switch_status_of_white_list(void);
void get_white_list_from_flash(void);
void flash_white_list_use_table(void);
void set_index_of_white_list_pos( uint8_t use_or_online, uint16_t index );
void clear_index_of_white_list_pos( uint8_t use_or_online, uint16_t index );
uint8_t get_nouse_pos_of_white_list( uint16_t *pos);
uint8_t get_next_uid_of_white_list(uint8_t sel_table, uint8_t uid[], uint16_t *uidpos );
void clear_white_list_online_table(void);
uint8_t get_index_of_white_list_pos_status( uint8_t use_or_online, uint16_t index );
void copy_online_to_store_table( void );
void clear_white_list_table(uint8_t sel_table);
void clear_current_uid_index( void );
void clicker_config_default_set( void );
#endif // __WHITE_LIST_H_
