#ifndef _FORK_H
#define _FORK_H

/**
 * \file fork.h
 * Metodos para el manejo de procesos.
 */

/**
 * Crea un nuevo proceso.
 Aloja espacio para un nuevo proceso y lo vuelve disponible para el scheduler
 @param fn la funcion a copiar.
 @param arg los argumentos de la funcion.
*/
int copy_process(unsigned long fn, unsigned long arg);

#endif