#ifndef	__WHITE_LIST_H_
#define	__WHITE_LIST_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define MAX_WHITE_LEN						          (120)
#define WHITE_LIST_LEN_POS_OF_FEE         (481)
#define WHITE_LIST_SW_POS_OF_FEE          (482)
#define WHITE_LIST_USE_TABLE_POS_OF_FEE   (483)


#define OPERATION_NOUSE                   (2)
#define OPERATION_ERR                     (1)
#define OPERATION_SUCCESS                 (0)

#define UID_USE_TABLE                     (0)
#define UID_ONLINE_TABLE                  (1)
#define UID_ONLINE_TEMP_TABLE             (2)
#define UID_TABLE_NUM                     (17)

#ifdef 	ENABLE_DEBUG_LOG
#define WhiteListDebug								    printf
#else
#define WhiteListDebug(...)
#endif

typedef enum 
{
	OFF 	= 0,
	ON 	= 1
} Switch_State;

extern uint8_t            uid_p;
extern uint8_t		        uid_len;					    // M1卡序列号长度
extern uint8_t 	          g_cSNR[10];						// M1卡序列号


bool get_index_of_uid( uint8_t index, uint8_t  uid[4] );
void clear_index_of_uid(uint8_t index);
void add_index_of_uid( uint8_t index, uint8_t  uid[4] );

bool initialize_white_list( void );
bool uidcmp(uint8_t *uid1, uint8_t *uid2);
uint8_t add_uid_to_white_list(uint8_t *g_uid, uint8_t *position);
void clear_white_list(void);
bool delete_uid_from_white_list(uint8_t *g_uid);
bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position);

bool store_len_to_fee(uint8_t len);
uint8_t get_len_of_white_list(void);
bool store_switch_status_to_fee(uint8_t switch_status);
uint8_t get_switch_status_of_white_list(void);
void get_white_list_from_flash(void);
void flash_white_list_use_table(void);
void set_index_of_white_list_pos( uint8_t use_or_online, uint8_t index );
void clear_index_of_white_list_pos( uint8_t use_or_online, uint8_t index );
bool get_nouse_pos_of_white_list( uint8_t *pos);
bool get_next_uid_of_white_list(uint8_t sel_table, uint8_t uid[], uint8_t *uidpos );
void clear_white_list_online_table(void);
bool get_index_of_white_list_pos_status( uint8_t use_or_online, uint8_t index );
void copy_online_to_store_table( void );
void clear_white_list_table(uint8_t sel_table);
void clear_current_uid_index( void );
#endif // __WHITE_LIST_H_
