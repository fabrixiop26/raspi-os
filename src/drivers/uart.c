#include "utils.h"
#include "peripherals/uart.h"
#include "peripherals/gpio.h"
#include "printf.h"
#include "drivers/framebuffer.h"

/* typedef struct point {
	int x;
	int y;
} point_t;

point_t p; */
int x = 0;
int y = 0;
const char charWidth = 9;
const char charHeigth = 16;
const char lineHeight = charHeigth + 1;
const char separation = charWidth + 1;
int zoom = 1;

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

void uart_init(void)
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7 << 12); // clean gpio14
	selector |= 4 << 12;	// set alt0 for gpio14 (txd0)
	selector &= ~(7 << 15); // clean gpio15
	selector |= 4 << 15;	// set alt0 for gpio15 (rxd0)
	put32(GPFSEL1, selector);

	//como se usan pines al quedar al aire para evitar valores al azar se
	//hace uso de pull-up/down resitor como aqui "siempre" estaran conectados
	//hay que iniciar su estado a ninguno de los anteriores
	//See page 101 del manual BCM2837
	put32(GPPUD, 0);
	delay(150);
	put32(GPPUDCLK0, (1 << 14) | (1 << 15));
	delay(150);
	put32(GPPUDCLK0, 0);

	put32(UART_IBRD, IBRD);
	put32(UART_FBRD, FBRD);
	put32(UART_IMSC, UARTRXINTR);				//activo el flag para los interrupts del receive
	put32(UART_LCRH, (3u << 5) | (0 << 4));		//set WLEN to 0b11 (8 bits word) y activa las fifos
	put32(UART_IFLS, (0 << 3));					//evito el uso de fifos
	put32(UART_CR, (1u << 9) | (1u << 8) | 1u); // set RXE, TXE, UARTEN bits
}

void handle_irq_uart()
{
	char c = uart_recv();
	//Si es enter añado otra linea
	//TODO: hay un bug aqui que pone el cursor en otra linea despues de dar enter. quiza sea el local timer interrupt que interfiere
	if (c == '\r')
	{
		uart_send('\n');
		//Esto se volvera line height
		y += lineHeight * zoom;
		x = 0;
		drawChar(x, y, c, 0x0f, zoom);
	}
	else
	{
		drawChar(x, y, c, 0x0f, zoom);
		//Esto puede ser un valor no necesario de la fuente
		//como espaciado
		x += separation * zoom;
		uart_send(c);
	}
	//Limpio el pending iterrupt the receive uart
	put32(UART_ICR, UARTRXINTR);
}

void putc(void *p, char c)
{
	uart_send(c);
}