#ifndef	_MINI_UART_H
#define	_MINI_UART_H

void uart_init ( unsigned int );
char uart_recv ( void );
void uart_send ( char c );
void uart_send_string(char* str);

//Configurar el baudrate variable

#define BAUD_RATE	9600
#define MHz		*1000000
#define SYSTEM_FREQ	250 MHz
#define BAUD_REG_VAL	(unsigned int)(((SYSTEM_FREQ/BAUD_RATE)/8)-1)

#endif  /*_MINI_UART_H */