#ifndef	_P_IRQ_H
#define	_P_IRQ_H

#include "base.h"

/**
 * \file irq.h
 * Define el conjunto de direcciones para el manejo de Exepciones.
 * @note Ver Pagina 112 del BCM2837 MANUAL.
 * @warning Tener en cuenta que la tabla de interrupts de perifericos de la pagina 113 del Manual presenta un error ya que de las lineas 0 - 3 deberia contener 4 interrupciones del System Timer. Las interrupciones del timer 0 y 2
 * estan reservadas y usados por la GPU y los 1 y 3 pueden ser usados para otros propositos
*/

#define IRQ_BASIC_PENDING	(PBASE+0x0000B200)
#define IRQ_PENDING_1		(PBASE+0x0000B204)
#define IRQ_PENDING_2		(PBASE+0x0000B208)
#define FIQ_CONTROL		(PBASE+0x0000B20C)
#define ENABLE_IRQS_1		(PBASE+0x0000B210)
#define ENABLE_IRQS_2		(PBASE+0x0000B214)
#define ENABLE_BASIC_IRQS	(PBASE+0x0000B218)
#define DISABLE_IRQS_1		(PBASE+0x0000B21C)
#define DISABLE_IRQS_2		(PBASE+0x0000B220)
#define DISABLE_BASIC_IRQS	(PBASE+0x0000B224)

#define SYSTEM_TIMER_IRQ_0	(1 << 0)
#define SYSTEM_TIMER_IRQ_1	(1 << 1)
#define SYSTEM_TIMER_IRQ_2	(1 << 2)
#define SYSTEM_TIMER_IRQ_3	(1 << 3)

#endif  /*_P_IRQ_H */