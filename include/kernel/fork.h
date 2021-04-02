#ifndef _FORK_H
#define _FORK_H

/**
 * \file fork.h
 * Metodos para la creacion de procesos.
 */

/**
 * Crea un nuevo proceso.
 Aloja espacio para un nuevo proceso y lo vuelve disponible para el scheduler
 @param fn La funcion a copiar.
 @param arg Los argumentos de la funcion.
 @param priority La prioridad del proceso.
*/
int copy_process(unsigned long fn, unsigned long arg, unsigned int priority);

#endif