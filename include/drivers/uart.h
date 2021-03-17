#ifndef	_UART_H
#define	_UART_H

/**
 * \file uart.h
 * Metodos para la comunicacion serial con el puerto UARTPL011. 
 * @see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/fractional-baud-rate-register--uartfbrd
 */

/**
 * Inicializa el dispositivo uart.
 */
void uart_init ( void );
/**
 * Lee el byte almacenado.
 * @return el byte almacenado en el registro de datos.
 */
char uart_recv ( void );
/**
 * Envia 1 byte al registro de datos.
 * @param c el byte a enviar.
 */
void uart_send ( char c );
/**
 * Envia una lista de caracteres al registro de datos.
 * @param str El buffer de caracteres a enviar.
 */
void uart_send_string(char* str);

#endif  /*_UART_H */