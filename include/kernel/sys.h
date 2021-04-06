#ifndef	_SYS_H
#define	_SYS_H

#define __NR_syscalls	    4

//Corresponde al index en el arreglo de syscalls en sys.c
#define SYS_WRITE_NUMBER    0 		// syscal numbers 
#define SYS_MALLOC_NUMBER   1 	
#define SYS_CLONE_NUMBER    2 	
#define SYS_EXIT_NUMBER     3 	

#ifndef __ASSEMBLER__

/**
 * Imprime en la consola usando UART.
 * @param buf El buffer de caracteres a imprimir
*/
void call_sys_write(char * buf);
/**
 * Crea un nuevo hilo de usuario.
 * @param fn La funcion a llamar.
 * @param arg El argumento de la funcion.
 * @param stack El stack del nuevo proceso
 * @return Codigo de exito de la funcion
*/
int call_sys_clone(unsigned long fn, unsigned long arg, unsigned long stack);
/**
 * Reserva una pagina de memoria para el nuevo proceso del usuario.
 * @return La direccion del primer byte en memoria
*/
unsigned long call_sys_malloc();
/**
 * Se encarga de la limpieza cada que un proceso termina.
*/
void call_sys_exit();

#endif
#endif  /*_SYS_H */