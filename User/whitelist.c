#include "whitelist.h"

/* Private variables ---------------------------------------------------------*/
uint8_t						white_len;						        // 白名单长度
Switch_State			white_on_off;					        // 白名单开关 
Switch_State			attendance_on_off;		        // 考勤开关 
Switch_State			match_on_off;					        // 配对开关 
uint16_t					match_number = 1;			  	    // 配对序号
uint8_t           uid_p;
uint8_t		        uid_len = 0;					        // M1卡序列号长度
uint8_t 	        g_cSNR[10];						        // M1卡序列号
uint16_t          white_list_use_onlne_table[5][8] =
{	
	{0,0,0,0,0,0,0,0}, // UID 使用索引表
	{0,0,0,0,0,0,0,0}, // UID 在线索引表
	{0,0,0,0,0,0,0,0}, // UID 在线索暂存表
	{0,0,0,0,0,0,0,0}, // UID 新增题目重发表
	{0,0,0,0,0,0,0,0}  // UID 在线题目重发表
};
uint8_t           rf_current_uid_index = 0;


/******************************************************************************
  Function:clear_white_list_online_table
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_white_list_online_table(void)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		white_list_use_onlne_table[2][i] = white_list_use_onlne_table[1][i];
		white_list_use_onlne_table[1][i] = 0x00;
	}
}
	
/******************************************************************************
  Function:flash_white_list_use_table
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void flash_white_list_use_table(void)
{
	uint8_t i;
	for(i=0;i<8;i++)
		 EE_WriteVariable(WHITE_LIST_USE_TABLE_POS_OF_FEE+i,white_list_use_onlne_table[0][i]);
}

/******************************************************************************
  Function:get_white_list_use_table
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void get_white_list_use_table(void)
{
	uint8_t i;
	for(i=0;i<8;i++)
		 EE_ReadVariable(WHITE_LIST_USE_TABLE_POS_OF_FEE+i,&white_list_use_onlne_table[0][i]);
}

/******************************************************************************
  Function:set_index_of_white_list_pos
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void set_index_of_white_list_pos( uint8_t use_or_online, uint8_t index )
{
	uint8_t pos1 = index / 15 ;
	uint8_t pos2 = index % 15 ;
	
	white_list_use_onlne_table[use_or_online][pos1] = (white_list_use_onlne_table[use_or_online][pos1] | 
	                               (uint16_t)((uint16_t)1<<pos2)) & 0x7FFF;
	
	if(use_or_online == 0)
	{
		flash_white_list_use_table();
		if(white_len<120)
			white_len++;
	}
}

/******************************************************************************
  Function:clear_index_of_white_list_pos
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_index_of_white_list_pos( uint8_t use_or_online, uint8_t index )
{
	uint8_t pos1 = index / 15 ;
	uint8_t pos2 = index % 15 ;
	
	white_list_use_onlne_table[use_or_online][pos1] = white_list_use_onlne_table[use_or_online][pos1] &
                	               ~(uint16_t)((uint16_t)1<<pos2);

	if( use_or_online ==0 )
	{
		flash_white_list_use_table();
		if(white_len>0)
			white_len--;
	}
}

/******************************************************************************
  Function:get_index_of_white_list_pos_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool get_index_of_white_list_pos_status( uint8_t use_or_online, uint8_t index )
{
	uint8_t pos1 = index / 15 ;
	uint8_t pos2 = index % 15 ;
	
	uint16_t status = white_list_use_onlne_table[use_or_online][pos1] &
                	               (uint16_t)((uint16_t)(1<<pos2));
	
	if(status & 0x7FFF)
		return 1;
	else
		return 0;
	
}

/******************************************************************************
  Function:get_nouse_pos_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool get_nouse_pos_of_white_list( uint8_t *pos)
{
	uint8_t i;
	
	for(i=0;i<MAX_WHITE_LEN;i++)
	{
		if(get_index_of_white_list_pos_status(0,i) == false)
		{
			*pos = i;
			return OPERATION_SUCCESS;
		}
	}
	return OPERATION_ERR;
}

/******************************************************************************
  Function:uidcmp
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool uidcmp(uint8_t *uid1, uint8_t *uid2)
{
	if((uid1[0] == uid2[0])&&(uid1[1] == uid2[1])&&(uid1[2] == uid2[2])&&(uid1[3] == uid2[3]))
		return OPERATION_SUCCESS;
	else
		return OPERATION_ERR;
}

/******************************************************************************
  Function:get_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool get_index_of_uid( uint8_t index, uint8_t  uid[4] )
{
	uint16_t tmpuid[4] = {0,0,0,0};
	uint8_t i;
	uint16_t viraddr = index * 4;
	uint8_t  is_pos_use = 0;
	
	/* 获取使用表的第i位的状态 */
	is_pos_use = get_index_of_white_list_pos_status(0,index);
	
	if(is_pos_use == 1)
	{
		for(i=0;i<4;i++)
		{
			EE_ReadVariable(viraddr+i,tmpuid+i);
			uid[i] = (uint8_t)(tmpuid[i]&0xFF);
		}
		return OPERATION_SUCCESS;
	}
	else
	{
		return OPERATION_ERR;
	}
}

/******************************************************************************
  Function:clear_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_index_of_uid(uint8_t index)
{
	clear_index_of_white_list_pos(0,index);
}

/******************************************************************************
  Function:add_index_of_uid
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void add_index_of_uid( uint8_t index, uint8_t  uid[4] )
{
	uint16_t viraddr = index * 4;
	uint8_t  i;
	
	for(i=0;i<4;i++)
	{
		EE_WriteVariable(viraddr+i,uid[i]);
	}
	set_index_of_white_list_pos(0,index);
}
/******************************************************************************
  Function:get_len_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_len_of_white_list(void)
{
	uint16_t len = 0;
	uint8_t i = 0;
	uint8_t is_pos_use = 0;
	
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		/* 获取使用表的第i位的状态 */
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		
		/* 如果使用的话，开始匹配 */
		if( is_pos_use == 1 )
		{
			/* 读取此位置的白名单 */
			len++;
		}
	}

	return (uint8_t)(len&0xFF);
}

/******************************************************************************
  Function:store_len_to_fee
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool store_len_to_fee(uint8_t len)
{
	uint16_t FlashStatus = 0;
	uint8_t i = 0;
	uint8_t is_pos_use = 0;
	
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		/* 获取使用表的第i位的状态 */
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		
		/* 如果使用的话，开始匹配 */
		if( is_pos_use == 1 )
		{
			/* 读取此位置的白名单 */
			len++;
		}
	}

	EE_WriteVariable(WHITE_LIST_LEN_POS_OF_FEE,len);
	if (FlashStatus != FLASH_COMPLETE)
	{
		return OPERATION_ERR;
	}
	return OPERATION_SUCCESS;
}

/******************************************************************************
  Function:store_switch_status_to_fee
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool store_switch_status_to_fee(uint8_t switch_status)
{
	uint16_t FlashStatus = 0;
	
	FlashStatus = EE_WriteVariable(WHITE_LIST_SW_POS_OF_FEE,switch_status);
	if (FlashStatus != FLASH_COMPLETE)
	{
		return OPERATION_ERR;
	}
	
	return OPERATION_SUCCESS;
}

/******************************************************************************
  Function:get_switch_status_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get_switch_status_of_white_list(void)
{
	uint16_t switch_status = 0;
	
	EE_ReadVariable(WHITE_LIST_SW_POS_OF_FEE,&switch_status);
	
	return (uint8_t)(switch_status&0xFF);
}

/******************************************************************************
  Function:initialize_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool initialize_white_list( void )
{
	uint16_t FlashStatus;
	uint8_t i;
	
	/* 格式化FLASH */
	FlashStatus = Fee_Init(FEE_INIT_CLEAR);
	if (FlashStatus != FLASH_COMPLETE)
	{
		return OPERATION_ERR;
	}
	
	/* 清除长度开关等状态 */
	white_on_off = OFF;
	store_switch_status_to_fee(white_on_off);
	white_len = 0;
	store_len_to_fee(white_len);
	
	/* 清除状态使用表 */
	for(i=0;i<8;i++)
		white_list_use_onlne_table[0][i] = 0;
	flash_white_list_use_table();
	
	return OPERATION_SUCCESS;
}

/******************************************************************************
  Function:search_uid_in_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool search_uid_in_white_list(uint8_t *g_uid , uint8_t *position)
{
	uint8_t i;
	uint8_t temuid[4];
	uint8_t is_pos_use = 0;
	uint8_t is_same = 0;
	
	for(i=0; i < MAX_WHITE_LEN; i++)
	{
		/* 获取使用表的第i位的状态 */
		is_pos_use = get_index_of_white_list_pos_status(0,i);
		
		/* 如果使用的话，开始匹配 */
		if( is_pos_use == 1 )
		{
			/* 读取此位置的白名单 */
			get_index_of_uid( i, temuid);
			is_same = uidcmp(temuid,g_uid);
			
			/* 如果白名单中已经存在该UID */
			if(is_same == OPERATION_SUCCESS)	
			{  
				*position = i;
				return OPERATION_SUCCESS;
			}
		}
	}
	return OPERATION_ERR;
}

/******************************************************************************
  Function:delete_uid_from_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool delete_uid_from_white_list(uint8_t *g_uid)
{
	uint8_t pos;
	uint8_t status = true;

	/* search the uid form white list*/
	status = search_uid_in_white_list( g_uid, &pos );
	
	if(status == OPERATION_ERR)
	{
		WhiteListDebug("<%s> The UID is not in white list \r\n",__func__);
		return OPERATION_ERR;
	}
	else
	{
		WhiteListDebug("<%s> The UID is in white list pos = %d\r\n",__func__,pos);
		clear_index_of_uid(pos);
	}
	
	return OPERATION_SUCCESS;
}

/******************************************************************************
  Function:add_uid_to_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t add_uid_to_white_list(uint8_t *g_uid, uint8_t *position)
{
	uint8_t status = true;
	
	/* search the uid form white list*/
	status = search_uid_in_white_list( g_uid, position );
	if(status == OPERATION_SUCCESS)
	{
		WhiteListDebug("<%s>The UID is in white list pos = %d\r\n",__func__,*position);
		return OPERATION_NOUSE;
	}
	else
	{
		status = get_nouse_pos_of_white_list(position);
	
		if(status == OPERATION_ERR)
		{
			WhiteListDebug("<%s> The white list is full \r\n",__func__);
			return OPERATION_ERR;
		}
		else
		{
			add_index_of_uid( *position, g_uid );
			return OPERATION_SUCCESS;
		}
	}
}

/******************************************************************************
  Function:get_next_uid_of_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
bool get_next_uid_of_white_list(uint8_t sel_table, uint8_t uid[])
{
	uint8_t i;
	
	/* 向后查找下一个UID */
	for(i=rf_current_uid_index;i<MAX_WHITE_LEN;i++)
	{
		if(get_index_of_white_list_pos_status(sel_table,i) == 1)
		{
			get_index_of_uid(i,uid);
			rf_current_uid_index = i+1;
			return OPERATION_SUCCESS;
		}
	}
	
	/* 向前查找下一个UID */
	for(i=0;i<rf_current_uid_index;i++)
	{
		if(get_index_of_white_list_pos_status(sel_table,i) == 1)
		{
			get_index_of_uid(i,uid);
			rf_current_uid_index = i+1;
			return OPERATION_SUCCESS;
		}
	}
	
	return OPERATION_ERR;
}
