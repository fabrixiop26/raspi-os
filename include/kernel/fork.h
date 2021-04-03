#ifndef _FORK_H
#define _FORK_H

/**
 * \file fork.h
 * Metodos para la creacion de procesos.
 */

#include "kernel/scheduler.h"

//PSR Bits
#define PSR_MODE_EL0t	0x00000000
#define PSR_MODE_EL1t	0x00000004
#define PSR_MODE_EL1h	0x00000005
#define PSR_MODE_EL2t	0x00000008
#define PSR_MODE_EL2h	0x00000009
#define PSR_MODE_EL3t	0x0000000c
#define PSR_MODE_EL3h	0x0000000d

/**
 * Crea un nuevo proceso.
 Aloja espacio para un nuevo proceso y lo vuelve disponible para el scheduler
 @param clone_flags Condiciones para la copia del proceso
 @param fn La funcion a copiar.
 @param arg Los argumentos de la funcion.
 @param priority La prioridad del proceso.
*/
int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack);
/**
 * Movemos la ejecucion a modo usuario.
 * @param pc La funcion en la cual empezar desde user mode
*/
int move_to_user_mode(unsigned long pc);

struct pt_regs * task_pt_regs(struct task_struct *tsk);

struct pt_regs {
	unsigned long regs[31];
	unsigned long sp;
	unsigned long pc;
	unsigned long pstate;
};


#endif