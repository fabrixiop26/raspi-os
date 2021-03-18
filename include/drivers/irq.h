#ifndef	_IRQ_H
#define	_IRQ_H

/**
 * \file irq.h
 * Define la interfaz para el control de las interrupciones.
 * @see Page 109 of BCM2837 ARM Peripherals manual
*/

/**
 * Habilita el controlador de interrupciones.
 * @note No todo el tiempo los dispositivos interrumpen directamente el procesador sino que recurren a un controlador para el trabajo
*/
void enable_interrupt_controller( void );

/**
 * Inicializa el vector table para los manejadores de las interrupciones.
*/
void irq_vector_init( void );
/**
 * Habilita las interrupciones.
 * @note Esto es necesario en casos como cuando se esta guardando el estado de los registros en el stack, ya que de generarse una interrupcion el estado del programa se perderia
*/
void enable_irq( void );
/**
 * Deshabilita las interrupciones.
 * @see enable_irq
*/
void disable_irq( void );

#endif  /*_IRQ_H */