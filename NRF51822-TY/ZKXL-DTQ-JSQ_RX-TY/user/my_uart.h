#ifndef __MY_UART_H
#define	__MY_UART_H


#include "simple_uart.h"

	//答题器串口定义
#define RX_PIN_NUMBER  (28)    // UART RX pin number.
#define TX_PIN_NUMBER  (29)    // UART TX pin number.
#define CTS_PIN_NUMBER (27)  // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER (26)    // Not used if HWFC is set to false
#define HWFC           false // UART hardware flow control


	//接收器51822RX对应UART口
#define RX_PIN_NUMBER_1  13    // UART RX pin number.
#define TX_PIN_NUMBER_1  14    // UART TX pin number.
#define CTS_PIN_NUMBER_1 11   // UART Clear To Send pin number. Not used if HWFC is set to false
#define RTS_PIN_NUMBER_1 12    // Not used if HWFC is set to false
#define HWFC_1           false // UART hardware flow control

	
void debug_uart_init(void);
int fputc(int ch, FILE *f);
void UART_send_byte(uint8_t byte);
void UART_Send(uint8_t *Buffer, uint32_t Length);
uint8_t UART_Recive(void);
void my_uart_trace_dump(uint8_t * p_buffer, uint32_t len);

#endif /* __UART_H */
