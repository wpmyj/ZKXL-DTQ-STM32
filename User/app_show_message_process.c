/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ringbuffer.h"
#include "app_show_message_process.h"
#include "stdio.h"
#include "stdarg.h"

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
	uint8_t r_index = 0;		
	char *pdata;
	static uint8_t skip_flag = 0x00;                                                         
	char str[256];
	va_list args;

	memset(str,0,256);

	va_start(args, fmt);
	vsprintf(str,fmt,args);
	va_end(args);

	/* JSON 剔除格式化输出字符 */
	pdata = str;
#ifdef FILTER_NOUSE_CHAR
	while( *pdata != '\0' )
	{
//		if( *pdata ==  '\"')
//		{
//			skip_flag = skip_flag ^ 0x01;
//			*pdata    = '\'';
//		}

		if((skip_flag == 1) || (*pdata > 32))
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
#endif
	printf("%s",str);		                                                       
}
