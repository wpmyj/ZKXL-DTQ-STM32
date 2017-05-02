#include "whitelist.h"

/* Private variables ---------------------------------------------------------*/
uint8_t 	        g_cSNR[10];						        // M1卡序列号
uint16_t          list_tcb_table[UID_LIST_TABLE_SUM][WHITE_TABLE_LEN] =
{
	{0,0,0,0,0,0,0,0}, // [0]:UID 使用索引表
	{0,0,0,0,0,0,0,0}, // [1]:UID 在线索引表
	{0,0,0,0,0,0,0,0}, // [2]:UID 

	{0,0,0,0,0,0,0,0}, // [3]:第1次统计接收表
	{0,0,0,0,0,0,0,0}, // [4]:第2次统计接收表
	{0,0,0,0,0,0,0,0}, // [5]:第2次在线索引表

	{0,0,0,0,0,0,0,0}, // [6]:第3次统计重发表
	{0,0,0,0,0,0,0,0}, // [7]:第3次在线索引表

	{0,0,0,0,0,0,0,0}, // [8]:第4次统计接收表
	{0,0,0,0,0,0,0,0}, // [9]:第4次在线索引表
	{0,0,0,0,0,0,0,0}, // [A]:心跳包在线索引表
	{0,0,0,0,0,0,0,0}, // [B]:心跳包ACK
	{0,0,0,0,0,0,0,0}, // [C]:PRE 
	{0,0,0,0,0,0,0,0}, // [D]:REQUST
  {0,0,0,0,0,0,0,0}, // [E]:REQUST TEMP PRE
	{0,0,0,0,0,0,0,0}  // [E]:REQUST TEMP ACK
};

wl_typedef        wl;
revicer_typedef   revicer;
rf_config_typedef clicker_set;

/******************************************************************************
  Function:clear_white_list_table
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void clear_white_list_table(uint8_t sel_table)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		list_tcb_table[sel_table][i] = 0x00;
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
		 EE_WriteVariable(WHITE_LIST_USE_TABLE_POS_OF_FEE+i,list_tcb_table[0][i]);
}

void clicker_config_default_set( void )
{
	uint16_t data;
	
	/* 读取信道配置参数 */
	EE_ReadVariable(CPU_RX_CH_POS_OF_FEE,&data);
	if( data < 125 )
	{
		//printf("read %d = %d\r\n",CPU_RX_CH_POS_OF_FEE,data);
		clicker_set.N_CH_TX      = data;
	}
	else
		clicker_set.N_CH_TX      = 2;

	EE_ReadVariable( CPU_TX_CH_POS_OF_FEE ,&data);
	if( data < 125 )
	{
		//printf("read %d = %d\r\n",CPU_TX_CH_POS_OF_FEE,data);
		clicker_set.N_CH_RX      = data;
	}
	else
		clicker_set.N_CH_RX      = 4;
	
	EE_ReadVariable( CPU_TX_POWER_POS_OF_FEE ,&data);
	if(( *(int8_t *)(&data) >= 1 ) && ( *(int8_t *)(&data) <= 5 ))
	{
		clicker_set.N_TX_POWER = *(int8_t *)(&data);
	}
	else
		clicker_set.N_TX_POWER = 5;
	
	EE_ReadVariable( CPU_24G_ATTENDANCE_OF_FEE ,&data);
	clicker_set.N_24G_ATTEND = *(uint8_t *)(&data);

	/* 设置设置接收器的信道 */
	spi_set_cpu_tx_signal_ch(clicker_set.N_CH_RX);
	spi_set_cpu_rx_signal_ch(clicker_set.N_CH_TX);
}

/******************************************************************************
  Function:get_white_list_from_flash
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void get_white_list_from_flash(void)
{
	uint8_t i,Is_use_pos;
	uint16_t index;
	uint16_t viraddr,tmpuid[4],switch_status,addr_clone_flag;

	/* get use table */
	for(i=0;i<8;i++)
		 EE_ReadVariable(WHITE_LIST_USE_TABLE_POS_OF_FEE+i,&list_tcb_table[0][i]);

	/* get uid */
	for(index=0;index<120;index++)
	{
		Is_use_pos = get_index_of_white_list_pos_status( 0, index );
		if(Is_use_pos == 1)
		{
			viraddr = index * 4;
			for(i=0;i<4;i++)
			{
				EE_ReadVariable(viraddr+i,tmpuid+i);
				wl.uids[index].uid[i] = (uint8_t)(tmpuid[i]&0xFF);
				wl.uids[index].pos    = index;
			}
			wl.len++;
		}
	}

	/* get switch_status */
	EE_ReadVariable(WHITE_LIST_SW_POS_OF_FEE,&switch_status);
	wl.switch_status = switch_status;
	
	/* get clone addr flag */
	EE_ReadVariable(CPU_ADDR_CLONE_FLAG,&addr_clone_flag);
	revicer.addr_clone_flag = addr_clone_flag;

  /* get clone addr */
	if(revicer.addr_clone_flag == 1)
	{
		for(i=0;i<4;i++)
		{
			EE_ReadVariable(CPU_CLONE_ADDR+i,tmpuid+i);
			revicer.uid[i] = (uint8_t)(tmpuid[i]&0xFF);
		}
	}
	
	wl.start = ON;
}

/******************************************************************************
  Function:set_index_of_white_list_pos
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void set_index_of_white_list_pos( uint8_t use_or_online, uint16_t index )
{
	uint8_t pos1 = index / 16 ;
	uint8_t pos2 = index % 16 ;

	list_tcb_table[use_or_online][pos1] = (list_tcb_table[use_or_online][pos1] |
	                               (uint16_t)((uint16_t)1<<pos2)) & 0xFFFF;

	if(use_or_online == 0)
	{
		flash_white_list_use_table();
		if(wl.len<120)
		{
			wl.len++;
			store_len_to_fee(wl.len);
		}
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
void clear_index_of_white_list_pos( uint8_t use_or_online, uint16_t index )
{
	uint8_t pos1 = index / 16 ;
	uint8_t pos2 = index % 16 ;

	list_tcb_table[use_or_online][pos1] = list_tcb_table[use_or_online][pos1] &
                	               ~(uint16_t)((uint16_t)1<<pos2);

	if( use_or_online ==0 )
	{
		flash_white_list_use_table();
		if(wl.len>0)
		{
			wl.len--;
			store_len_to_fee(wl.len);
		}
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
uint8_t get_index_of_white_list_pos_status( uint8_t use_or_online, uint16_t index )
{
	uint8_t pos1 = index / 16 ;
	uint8_t pos2 = index % 16 ;

	uint16_t status = list_tcb_table[use_or_online][pos1] &
                	               (uint16_t)((uint16_t)(1<<pos2));

	if(status & 0xFFFF)
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
uint8_t get_nouse_pos_of_white_list( uint16_t *pos)
{
	uint16_t i;

	for(i=0;i<MAX_WHITE_LEN;i++)
	{
		if(get_index_of_white_list_pos_status(0,i) == 0)
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
uint8_t uidcmp(uint8_t *uid1, uint8_t *uid2)
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
uint8_t get_index_of_uid( uint16_t index, uint8_t  uid[4] )
{
	uint8_t  is_pos_use = 0;

	/* 获取使用表的第i位的状态 */
	is_pos_use = get_index_of_white_list_pos_status(0,index);

	if(is_pos_use == 1)
	{
		memcpy(uid,wl.uids[index].uid,4);
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
void clear_index_of_uid(uint16_t index)
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
void add_index_of_uid( uint16_t index, uint8_t  uid[4] )
{
	uint16_t viraddr = index * 4;
	uint8_t  i;

	for(i=0;i<4;i++)
	{
		EE_WriteVariable(viraddr+i,uid[i]);
	}
	memcpy(wl.uids[index].uid,uid,4);
	wl.uids[index].pos = index;
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
uint16_t get_len_of_white_list(void)
{
	uint16_t len = 0;
	uint16_t i = 0;
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

	return (uint16_t)(len&0xFFFF);
}

/******************************************************************************
  Function:store_len_to_fee
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t store_len_to_fee(uint16_t len)
{
	uint16_t FlashStatus = 0;

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
uint8_t store_switch_status_to_fee(uint8_t switch_status)
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
  Function:initialize_white_list
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t initialize_white_list( void )
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
	wl.switch_status = OFF;
	store_switch_status_to_fee(wl.switch_status);
	wl.len = 0;
	store_len_to_fee(wl.len);

	/* 清除状态使用表 */
	for(i=0;i<8;i++)
		list_tcb_table[0][i] = 0;
	flash_white_list_use_table();

	wl.start = ON;

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
uint8_t search_uid_in_white_list(uint8_t *g_uid , uint16_t *position)
{
	uint16_t i;
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
uint8_t delete_uid_from_white_list(uint8_t *g_uid)
{
	uint16_t pos;
	uint8_t status = 1;

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
uint8_t add_uid_to_white_list(uint8_t *g_uid, uint16_t *position)
{
	uint8_t status = 1;

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
			*position = 0;
			return OPERATION_ERR;
		}
		else
		{
			add_index_of_uid( *position, g_uid );
			return OPERATION_SUCCESS;
		}
	}
}
