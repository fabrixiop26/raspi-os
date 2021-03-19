#include "utils.h"
#include "printf.h"
#include "peripherals/timer.h"

const unsigned int interval = 200000;
unsigned int threshold = 0;

void timer_init ( void )
{
	//Obtenemos el valor del contador del registro ultimos 32bits
	threshold = get32(TIMER_CLO);
	//Incrementamos dicho valor por el intervalo
	threshold += interval;
	//Asiganos el valor del registro de comperacion para el timer numero 1
	//Cuando los ultimos 32 bits del timer sean iguales al valor de este registro se disparara la señal
	put32(TIMER_C1, threshold);
}

void handle_timer_irq( void ) 
{
	//Actualizamos el valor del intervalo para que la siguiente interrupcion sea generada en el mismo intervalo
	threshold += interval;
	//printf("Current Value: %d", curVal);
	put32(TIMER_C1, threshold);
	//Reconocemos la interrupcion escribiendo 1 en el bit correspondiente del registro TIMER_CS segun indicado en el manual
	put32(TIMER_CS, TIMER_CS_M1);
	printf("Timer interrupt received\n\r");
}