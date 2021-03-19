#ifndef	_TIMER_H
#define	_TIMER_H

/**
 * \file timer.h
 * Interfaz para el manejo del System Timer.
 * EL periferico con tiene 4 canales de 32 bits y uno solo de 64 bits que es el que corre, Cada canal tiene tiene un registro para comparar con los ultimos 32 bits del contador de 64 bits el cual aumenta de 1 en cada tick. Cuando los valores coinciden se envia una señal para ese canal y se envia al controlador de interrupciones
*/

/**
 * Inicializa el System Timer.
*/
void timer_init ( void );
/**
 * Controla como se debe manejar la interupccion producida por el System Timer.
*/
void handle_timer_irq ( void );

#endif  /*_TIMER_H */