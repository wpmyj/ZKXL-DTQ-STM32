#ifndef	__WHITE_LIST_H_
#define	__WHITE_LIST_H_
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define MAX_WHITE_LEN						   (120)
#define WHITE_LIST_LEN_POS_OF_FEE  (481)
#define WHITE_LIST_SW_POS_OF_FEE   (482)

typedef enum 
{
	OFF 	= 0,
	ON 	= 1
} Switch_State;

//extern white_list_t			white_list[MAX_WHITE_LEN];		// 白名单列表
extern uint8_t						white_len;						// 白名单长度
extern Switch_State				white_on_off;					// 白名单开关
extern Switch_State				attendance_on_off;		// 考勤开关 
extern Switch_State				match_on_off;					// 配对开关 
extern uint16_t						match_number;					// 配对序号
extern uint8_t            uid_p;
extern uint8_t		        uid_len;					    // M1卡序列号长度
extern uint8_t 	          g_cSNR[10];						// M1卡序列号


void get_index_of_uid( uint8_t index, uint8_t  uid[4] );
void clear_index_of_uid(uint8_t index);
void add_index_of_uid( uint8_t index, uint8_t  uid[4] );

bool initialize_white_list( void );
bool uidcmp(uint8_t *uid1, uint8_t *uid2);
bool add_uid_to_white_list(uint8_t *g_uid, uint8_t *position);
void clear_white_list(void);
bool delete_uid_from_white_list(uint8_t *g_uid);

void    store_len_to_fee(uint8_t len);
uint8_t get_len_of_white_list(void);
void    store_switch_status_to_fee(uint8_t switch_status);
uint8_t get_switch_status_of_white_list(void);
#endif // __WHITE_LIST_H_
