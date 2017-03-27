#ifndef	__APP_SHOW_MESSAGE_PROCESS_H_
#define	__APP_SHOW_MESSAGE_PROCESS_H_

//#define USE_BUFFER_FOR_PRINT

#define READ_STRING_LEN                        50

#ifdef USE_BUFFER_FOR_PRINT
#define DEBUG_LOG							                 b_print
#else
#define DEBUG_LOG							                 printf
#endif

void message_show_process( void );
void b_print(const char *fmt, ...) ;

#endif
