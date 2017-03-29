/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ringbuffer.h"
#include "app_show_message_process.h"
#include "stdio.h"
#include "stdarg.h"

/******************************************************************************
  Function:message_show_process
  Description:
		打印信息输出进程
  Input :
  Return:
  Others:None
******************************************************************************/
void message_show_process( void )
{
	uint8_t str[READ_STRING_LEN] = "";
	uint8_t r_index = 0;

	if(buffer_get_buffer_status(PRINT_BUF) != BUF_EMPTY) 
	{        
		uint8_t *pdata;
		print_read_data_to_buffer(str,READ_STRING_LEN); 
		/* JSON 剔除格式化输出字符 */
		pdata = str;
		while( *pdata != '\0' )
		{
			if( *pdata > 32 )
			{	
				if(*pdata != str[r_index])
					str[r_index] = *pdata;
				pdata++;
				r_index++;
			}
			else
			{
				pdata++;
			}
		}
		if( *pdata == '\0' )
			str[r_index] = '\0';
		printf("%s",str);
	}  
}

/******************************************************************************
  Function:b_print
  Description:
		打印信息输出到Buffer中
  Input :
  Return:
  Others:None
******************************************************************************/
void b_print(const char *fmt, ...)                                       
{                                                          
	char str[256], len ;
	va_list args;
	
	memset(str,0,256);
	
	va_start(args, fmt);
	len = vsprintf(str,fmt,args);
	va_end(args);
	
	if(buffer_get_buffer_status(PRINT_BUF) != BUF_FULL) 
	{                                                        
		print_write_data_to_buffer(str,len);                   
	}                                                        
	else                                                     
	{                                                        
		printf("print buffer full,data lost\r\n");             
	}	                                                       
}
