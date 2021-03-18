#include "utils.h"
#include "printf.h"
#include "peripherals/timer.h"

const unsigned int interval = 200000;
unsigned int curVal = 0;

void timer_init ( void )
{
	//Obtenemos el valor del contador del registro
	curVal = get32(TIMER_CLO);
	//Incrementamos dicho valor por el intervalo
	curVal += interval;
	//Asiganos el valor del registro de comperacion para la interrupcion numero 1
	put32(TIMER_C1, curVal);
}

void handle_timer_irq( void ) 
{
	//Actualizamos el valor del intervalo para que la siguiente interrupcion sea generada en el mismo intervalo
	curVal += interval;
	put32(TIMER_C1, curVal);
	//Reconocemos la interrupcion escribiendo 1 en el registro TIMER_CS
	put32(TIMER_CS, TIMER_CS_M1);
	printf("Timer interrupt received\n\r");
}