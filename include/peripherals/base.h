#ifndef	_P_BASE_H
#define	_P_BASE_H
/**
 * \file base.h
 * Direccion base de los perifericos del chip
 * @see https://usermanual.wiki/Datasheet/BCM2837ARMPeripheralsBroadcom.1054296467 Section 1.2.3
 */

#include "mm.h"

///< Direccion base para MMIO
#define DEVICE_BASE 0x3F000000
#define PBASE       (VA_START + DEVICE_BASE)

//Direccion de perifericos locales
//creo toca cambiar por el VA
#define LOCAL_PERIPHERAL_BASE 0x40000000
#define EXTRA_BYTES             0x200000
#define LAST_LOCAL_PERIPHERAL (LOCAL_PERIPHERAL_BASE + EXTRA_BYTES)
#define PERIPHERAL_BASE		(LOCAL_PERIPHERAL_BASE + VA_START)

#endif  /*_P_BASE_H */