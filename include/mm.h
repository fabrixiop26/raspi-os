#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

/**
 * \file mm.h
 * Funciones y constantes para el manejo de la memoria.
 */

//Comienzo de las direcciones virtuales
#define VA_START 			0xffff000000000000
//Tamaño de la memoria fisica
#define PHYS_MEMORY_SIZE 	0x40000000 //1gb	

#define PAGE_MASK			0xfffffffffffff000

#define PAGE_SHIFT	 		12 //2^12 = 4Kb pagina
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT)

#define PAGE_SIZE   			(1 << PAGE_SHIFT)	
#define SECTION_SIZE			(1 << SECTION_SHIFT)	

#define LOW_MEMORY              	(4 * SECTION_SIZE) //2*2^21 = 8Mb tamaño reservado para el OS y el init task stack
#define BASE_MEMORY         ((LOW_MEMORY * 2) + (PAGE_SIZE * 3)) //tomar en cuenta el stack de los otros 3 cores que ocuparan 4kb de stack para cada core

//Primer byte fisico donde comienza MMIO de. De esta direccion en adelante es reservado
//recordar que esta el framebuffer y debo tmbn evitarlo?
#define HIGH_MEMORY             DEVICE_BASE

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT)

//Translations defines
#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT

#define PG_DIR_SIZE			(3 * PAGE_SIZE)

#ifndef __ASSEMBLER__

#include "kernel/scheduler.h"

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
/**
 * Mapea un proceso a una direccion de memoria virtual.
 * @param task El proceso a mapear.
 * @param va Direccion virtual.
 * @param page pagina
*/
void map_page(struct task_struct *task, unsigned long va, unsigned long page);
/**
 * Copia una direccion de memoria a otra.
 * @param dst La nueva ubicacion en memoria.
 * @param src La direccion actual de memoria.
 * @param n El tamaño del dato.
*/
void memcpy(unsigned long dst, unsigned long src, unsigned long n);
/**
 * Copiar un proceso a memoria virtual.
 * @param dst El proceso a copiar.
 * @return El codigo de exito.
*/
int copy_virt_memory(struct task_struct *dst);
/**
 * Reserva una pagina en el kernel.
 * @return La nueva pagina.
*/
unsigned long allocate_kernel_page(); 
/**
 * Rserva una nueva pagina para el usuario.
 * @param task El proceso a copiar.
 * @param va La direccion virtual.
 * @return La nueva pagina.
*/
unsigned long allocate_user_page(struct task_struct *task, unsigned long va); 

extern unsigned long pg_dir;

#endif

#endif  /*_MM_H */