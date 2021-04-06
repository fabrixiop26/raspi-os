#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

/**
 * \file mm.h
 * Funciones y constantes para el manejo de la memoria.
 */

#define PAGE_SHIFT	 		12 //2^12 = 4Kb pagina
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              	(2 * SECTION_SIZE) //2*2^21 = 4Mb tamaño reservado para el OS y el init task stack
#define BASE_MEMORY         ((LOW_MEMORY * 8) + (PAGE_SIZE * 3)) //tomar en cuenta el stack de los otros 3 cores que ocuparan 3 paginas exactamente

//Primer byte donde comienza MMIO. De esta direccion en adelante es reservado
//recordar que esta el framebuffer y debo tmbn evitarlo
#define HIGH_MEMORY             PBASE

#define PAGING_MEMORY           (HIGH_MEMORY - BASE_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#ifndef __ASSEMBLER__

/**
 * Inicializa con 0 una seccion en memoria.
 * @param src La direccion de origen.
 * @param n El tamaño de la seccion a limpiar.
 */ 
void memzero(unsigned long src, unsigned long n);
/**
 * Obtiene la priera pagina libre en memoria.
 @return Direccion del primer byte libre en memoria.
*/
unsigned long get_free_page();
/**
 * Marca como libre la pagina de memoria indicada.
 @param p La direccion en memoria a liberar.
*/
void free_page(unsigned long p);

#endif

#endif  /*_MM_H */