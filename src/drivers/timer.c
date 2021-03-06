#include "peripherals/timer.h"
#include "kernel/scheduler.h"
#include "drivers/timer.h"
#include "utils.h"

//20M (c.a 1s)
const unsigned int interval = 200000;
unsigned int threshold = 0;

/* void timer_init(void)
{
	//Obtenemos el valor del contador del registro ultimos 32bits

	threshold = get32(TIMER_CLO);
	//Incrementamos dicho valor por el intervalo
	threshold += interval;
	//Asiganos el valor del registro de comperacion para el timer numero 1
	//Cuando los ultimos 32 bits del timer sean iguales al valor de este registro se disparara la señal
	put32(TIMER_C1, threshold);
	printf("Quiza?\n\r");
	//parte local
	//put32(TIMER_CTRL, ((1<<28) | interval));
} */

void timer_init(void){
	//Dirijo las interrupciones del local timer al core 0 irq,
	//las demas cpu no tienen implementado aun IRQ (4 safety)
	put32(TIMER_INTERRUPT_ROUTE, 0);
	// Set value, enable Timer and Interrupt
	//los bits 0:27 son los que controlan el intervalo de tiempo
	//el local timer disminuye y se recarga solo al llegar a 0
	put32(TIMER_CTRL, ((1<<28) | interval));
}

/* void handle_timer_irq(void)
{
	//Actualizamos el valor del intervalo para que la siguiente interrupcion sea generada en el mismo intervalo
	threshold += interval;
	//printf("Current Value: %d", curVal);
	put32(TIMER_C1, threshold);
	//Reconocemos la interrupcion escribiendo 1 en el bit correspondiente del registro TIMER_CS segun indicado en el manual
	put32(TIMER_CS, TIMER_CS_M1);
	timer_tick();
} */


void handle_timer_irq( void ) 
{
	//printf("Timer interrupt received, Local Timer\n\r");
	//3 (11) se recarga cuando se pone en 1 el bit 30
	put32(TIMER_FLAG, (3<<30));				// clear interrupt flag and reload timer
	//Controlo cada cuantas interrupciones un proceso puede correr
	//aka aviso al scheduler que un "quantum ha pasado"
	timer_tick();
}