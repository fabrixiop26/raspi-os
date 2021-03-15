#ifndef _P_UART_H
#define _P_UART_H

#include "peripherals/base.h"

//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/summary-of-registers?lang=en
#define UART_BASE (PBASE + 0x201000) //same as 0x00201000 aun no se de donde
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/data-register--uartdr?lang=en
#define UART_DR (UART_BASE + 0x0)
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/flag-register--uartfr?lang=en
#define UART_FR (UART_BASE + 0x18)
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/integer-baud-rate-register--uartibrd?lang=en
#define UART_IBRD (UART_BASE + 0x24)
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/fractional-baud-rate-register--uartfbrd?lang=en
#define UART_FBRD (UART_BASE + 0x28)
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/line-control-register--uartlcr-h?lang=en
#define UART_LCRH (UART_BASE + 0x2c)
//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/control-register--uartcr?lang=en
#define UART_CR (UART_BASE + 0x30)

//see https://developer.arm.com/documentation/ddi0183/g/programmers-model/register-descriptions/fractional-baud-rate-register--uartfbrd?lang=en
#define MHz *1000000
#define UART_CLK_HZ 48 MHz
#define BAUDS (unsigned int)115200
#define UART_BAUD_DIV ((double)UART_CLK_HZ) / (16 * BAUDS)
//los ultimos 16 bits
#define IBRD (unsigned int)(UART_BAUD_DIV)
//solo los ultimos 6 bits
#define FBRD (unsigned int)((UART_BAUD_DIV - UART_IBRD)*64 + 0.5)

#endif /*_P_UART_H */