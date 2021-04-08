#include "kernel/fork.h"
#include "printf.h"
#include "kernel/scheduler.h"
#include "mm.h"
#include "utils.h"

/**
 * \file sys.c
 * Contiene la logica de todos los llamados del sistema.
*/

//Llama a prinf
void sys_write(char * buf){
	printf(buf);
}

//Crea un nuevo user thread
//lo que retorna es pasado a x0 en ret_from_syscall
int sys_fork(){
	printf("Forking?");
	return copy_process(0, 0, 0);
}

void sys_exit(){
	exit_process();
}

//Lista con apuntadores a los llamados
void * const sys_call_table[] = {sys_write, sys_fork, sys_exit};