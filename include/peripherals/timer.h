#ifndef	_P_TIMER_H
#define	_P_TIMER_H

/**
 * \file timer.h
 * Direcciones para control del System y Local Timer.
 * @see Pagina 172 del manual BCM2837 y para la implementacion del local timer
 * https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2836/QA7_rev3.4.pdf seccion 4.11
*/

#include "base.h"

#define TIMER_CS        (PBASE+0x00003000)
#define TIMER_CLO       (PBASE+0x00003004)
#define TIMER_CHI       (PBASE+0x00003008)
#define TIMER_C0        (PBASE+0x0000300C)
#define TIMER_C1        (PBASE+0x00003010)
#define TIMER_C2        (PBASE+0x00003014)
#define TIMER_C3        (PBASE+0x00003018)

//Con esto controlamos los canales de los 4 timers de 32 bits
#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

// Local timer registers
#define	TIMER_CTRL		(PERIPHERAL_BASE+0x34)
#define	TIMER_FLAG		(PERIPHERAL_BASE+0x38)


#endif  /*_P_TIMER_H */