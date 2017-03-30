/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ringbuffer.h"
#include "app_show_message_process.h"
#include "stdio.h"
#include "stdarg.h"

/******************************************************************************
  Function:b_print
  Description:
		��ӡ��Ϣ�����Buffer��
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

	/* JSON �޳���ʽ������ַ� */
	pdata = str;
	while( *pdata != '\0' )
	{
		if( *pdata ==  '\'')
			skip_flag = skip_flag ^ 0x01;

		if( skip_flag == 0x00 )
		{
			if( *pdata > 32)  
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
		else
		{
			if(*pdata != str[r_index])
					str[r_index] = *pdata;
			pdata++;
			r_index++;
		}
	}
	if( *pdata == '\0' )
		str[r_index] = '\0';
	printf("%s",str);		                                                       
}
