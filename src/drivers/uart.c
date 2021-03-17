#include "utils.h"
#include "peripherals/uart.h"
#include "peripherals/gpio.h"

void uart_send(char c)
{
    //Esperamos a que el bit 5 este listo
    while (get32(UART_FR) & (1 << 5)) // wait while TXFF is set
        ;
    //Envia el byte (char) 
    put32(UART_DR, c);
}

char uart_recv(void)
{
    while (get32(UART_FR) & (1 << 4)) // wait while RXFE is set
        ;
    //devuelve los ultimos 8 bits (1byte) ya que es un char
    return (get32(UART_DR) & 0xFF);
}

void uart_send_string(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        uart_send((char)str[i]);
    }
}

void uart_init ( void )
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 4<<12;                      // set alt0 for gpio14 (txd0)
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 4<<15;                      // set alt0 for gpio15 (rxd0)
	put32(GPFSEL1,selector);

    //como se usan pines al quedar al aire para evitar valores al azar se
	//hace uso de pull-up/down resitor como aqui "siempre" estaran conectados
	//hay que iniciar su estado a ninguno de los anteriores
	//See page 101 del manual BCM2837
	put32(GPPUD,0);
	delay(150);
	put32(GPPUDCLK0,(1<<14)|(1<<15));
	delay(150);
	put32(GPPUDCLK0,0);

	put32(UART_IBRD, IBRD);
	put32(UART_FBRD, FBRD);
	put32(UART_LCRH, (3u << 5));                //set WLEN to 0b11 (8 bits word)
	put32(UART_CR, (1u << 9) | (1u << 8) | 1u); // set RXE, TXE, UARTEN bits
}

void putc ( void* p, char c ){
	uart_send(c);
}