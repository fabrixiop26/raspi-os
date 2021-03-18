#ifndef	_TIMER_H
#define	_TIMER_H

/**
 * \file timer.h
 * Interfaz para el manejo del System Timer.
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