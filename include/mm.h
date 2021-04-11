#ifndef	_MM_H
#define	_MM_H

#include "peripherals/base.h"

/**
 * \file mm.h
 * Funciones y constantes para el manejo de la memoria.
 */

//Comienzo de las direcciones virtuales 64 bits de los cuales solo se usan 48
/*
Bits [39 - 47] contain an index in the PGD table. MMU uses this index to find the location of the PUD.
Bits [30 - 38] contain an index in the PUD table. MMU uses this index to find the location of the PMD.
Bits [21 - 29] contain an index in the PMD table. MMU uses this index to find the location of the PTE.
Bits [12 - 20] contain an index in the PTE table. MMU uses this index to find a page in the physical memory.
Bits [0 - 11] contain an offset in the physical page. MMU uses this offset to determine the exact position in the previously found page that corresponds to the original virtual address. (2^12 = 4Kb offset)
*/
/*
    Si los ultimos 16 bits son 1 se usa la direccion guardada en ttbr1_el1 si todo es 0 se usa ttbr0_el1. Todas las direcciones de kernel absolutas  deben comenzar con 0xffff haciendo pensar que la direccion de la imagen del OS esta cargada en VA_START
*/
#define VA_START 			0xffff000000000000
#define VA_START_LOCAL      0xffff008000000000 //pgd index sera 1 aqui 000000001
//Tamaño de la memoria fisica
#define PHYS_MEMORY_SIZE 	0x40000000 //1gb	

#define PAGE_MASK			0xfffffffffffff000

#define PAGE_SHIFT	 		12 //2^12 = 4Kb pagina/offset
#define TABLE_SHIFT 			9
#define SECTION_SHIFT			(PAGE_SHIFT + TABLE_SHIFT) //21

//2^9 = 512 indices. Cada pagina en el virtual addresss tiene 9 bits a expecion de la del offset que contiene 12 y donde cada entrada en una pagina son 8bytes (64 bits) entonces cada pagina tiene un tamaño de 512 * 8 = 4096 bytes = 4Kb
#define PAGE_SIZE   			(1 << PAGE_SHIFT)
#define VA_LOCAL_OFFSET         (PAGE_SIZE * 3) //como ya hay 3 tablas al final de la tercerca comienza la nueva
//Se usa para eliminar una tabla la ultima tabla (PTE) al mapear 2Mb de bloques. Se usa para mapear largas porciones contiguas de memoria (Secciones) 9 bits del pte + 9 bits del page offset
#define SECTION_SIZE			(1 << SECTION_SHIFT) // 2^21 = 2MB

#define LOW_MEMORY              	(4 * SECTION_SIZE) //4*2^21 = 8Mb tamaño reservado para el OS y el init task stack
#define BASE_MEMORY         ((LOW_MEMORY) + (PAGE_SIZE * 3)) //tomar en cuenta el stack de los otros 3 cores que ocuparan 4kb de stack para cada core

//Primer byte fisico donde comienza MMIO de. De esta direccion en adelante es reservado
//recordar que esta el framebuffer y debo tmbn evitarlo?
#define HIGH_MEMORY             DEVICE_BASE

#define PAGING_MEMORY           (HIGH_MEMORY - LOW_MEMORY)
#define PAGING_PAGES            (PAGING_MEMORY/PAGE_SIZE)

#define PTRS_PER_TABLE			(1 << TABLE_SHIFT) // 2^9 = 512

//Translations defines, offset para sacar el indice de cada pagina (PGD, PDU, PMD)
#define PGD_SHIFT			PAGE_SHIFT + 3*TABLE_SHIFT // 12 + 3 * 9 = 39
#define PUD_SHIFT			PAGE_SHIFT + 2*TABLE_SHIFT // 12 + 2 * 9 = 30
#define PMD_SHIFT			PAGE_SHIFT + TABLE_SHIFT // 12 + 9 = 21

//Esto debe cambiar para agregar una PDU y PMD mas
#define PG_DIR_SIZE			(5 * PAGE_SIZE)

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
 * @param src La direccion actual de memoria.
 * @param dst La nueva ubicacion en memoria.
 * @param n El tamaño del dato.
*/
void memcpy(unsigned long src, unsigned long dest, unsigned long n);
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