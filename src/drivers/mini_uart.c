#include "utils.h"
#include "peripherals/gpio.h"
#include "peripherals/mini_uart.h"

void uart_send ( char c )
{
	while(1) {
		//si el bit 0 es 1 en AUX_MU_LSR_REG entonces la data esta lista. 0x20 = 100000 (bit 5) si es 1 indica que esta vacio el transmitter asi que podemos escribir
		if(get32(AUX_MU_LSR_REG)&0x20) 
			break;
	}
	put32(AUX_MU_IO_REG,c);
}

char uart_recv ( void )
{
	while(1) {
		if(get32(AUX_MU_LSR_REG)&0x01) 
			break;
	}
	//devuelve los ultimos 8 bits (1byte) ya que es un char
	return(get32(AUX_MU_IO_REG)&0xFF);
}

void uart_send_string(char* str)
{
	for (int i = 0; str[i] != '\0'; i ++) {
		uart_send((char)str[i]);
	}
}

//GPIO pins deben ser configurados antes de usar UART
//UART1 usa RXD1, TXD1
void uart_init ( void )
{
	unsigned int selector;

	//El pin 14 y 15 en su alt5 contienen txd1 y rxd1 y esos seran usados como 
	//mini uart transmit and receive data see page 102 BCM2837 Manual
	//GPFSEL1 register se usa para controlar las alternative functions de pines //10-19 see page 92
	selector = get32(GPFSEL1);
	//como son 5 alternative functions son 3 bits
	selector &= ~(7<<12);                   // clean gpio14
	selector |= 2<<12;                      // set alt5 for gpio14
	selector &= ~(7<<15);                   // clean gpio15
	selector |= 2<<15;                      // set alt5 for gpio15
	put32(GPFSEL1,selector);

	//como se usan pones al quedar al aire para evitar valores al azar se
	//hace uso de pull-up/down resitor como aqui "siempre" estaran conectados
	//hay que iniciar su estado a ninguno de los anteriores
	//See page 101 del manual BCM2837
	put32(GPPUD,0);
	delay(150);
	put32(GPPUDCLK0,(1<<14)|(1<<15)); //especifica para hacer clock a la señal 
									//de los pines 14 y 15 que son los usados
	delay(150);												
	put32(GPPUDCLK0,0);

	put32(AUX_ENABLES,1);                   //Enable mini uart (this also enables access to its registers) see page 9 BCM2837
	put32(AUX_MU_CNTL_REG,0);               //Disable auto flow control and disable receiver and transmitter (for now) autoflow requiere otros pines
	put32(AUX_MU_IER_REG,0);                //Disable receive and transmit interrupts
	put32(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
	put32(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
	put32(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200 bits/s

	put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}





