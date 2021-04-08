#ifndef _ENTRY_H
#define _ENTRY_H

/**
 * \file entry.h
 * Definicion y Configuracion para el manejador de Exepciones.
 * 16 exception handlers (4 exception levels multiplied by 4 execution states).
 * Se define un vector table que mantiene las direcciones de todos los manejadores de exepciones. Para ver la definicion ir a <a href="https://developer.arm.com/docs/ddi0487/ca/arm-architecture-reference-manual-armv8-for-armv8-a-architecture-profile">Pagina 1876 del manual Aarch64-Reference-Manual</a>
*/

#define S_FRAME_SIZE			272 // size of all saved registers 
#define S_X0				    0  // offset of x0 register in saved stack frame

#define SYNC_INVALID_EL1t		0 
#define IRQ_INVALID_EL1t		1 
#define FIQ_INVALID_EL1t		2 
#define ERROR_INVALID_EL1t		3 

#define SYNC_INVALID_EL1h		4 
#define FIQ_INVALID_EL1h		5 
#define ERROR_INVALID_EL1h		6 

#define FIQ_INVALID_EL0_64		7 
#define ERROR_INVALID_EL0_64	8 

#define SYNC_INVALID_EL0_32		9 
#define IRQ_INVALID_EL0_32		10 
#define FIQ_INVALID_EL0_32		11 
#define ERROR_INVALID_EL0_32	12 

#define SYNC_ERROR			    13 
#define SYSCALL_ERROR			14 
#define DATA_ABORT_ERROR		15 

#ifndef __ASSEMBLER__

/**
 * Devolver la ejecucion tras un fork.
*/
void ret_from_fork(void);

#endif

#endif