#include "kernel/sys.h"

.globl call_sys_write
//Gaurda el index del syscall en w8
//y genera una synchronous exception con svc
//w8 se usa para el numero del syscall por convencion y x0-x7 para los argumentos
call_sys_write:
	mov w8, #SYS_WRITE_NUMBER	
	svc #0
	ret

.globl call_sys_malloc
call_sys_malloc:
	mov w8, #SYS_MALLOC_NUMBER	
	svc #0
	ret

.globl call_sys_exit
call_sys_exit:
	mov w8, #SYS_EXIT_NUMBER	
	svc #0
	ret

.globl call_sys_clone
//retorna dos veces una en el proceso original y la segunda vez en la clonada
call_sys_clone:
	/* Save args for the child.  */
	mov	x10, x0					/*fn*/
	mov	x11, x1					/*arg*/
	mov	x12, x2					/*stack*/

	/* Do the system call.  */
	mov 	x0, x2					/* guardamos el stack para pasarlo a la funcion  */
	mov	x8, #SYS_CLONE_NUMBER //x8 por ser long
	svc	0x0
	//Si es 0 significa que estamos ejecutandonos en el hilo nuevo
	//si es non zero significa el pid del nuevo proceso y nos devolvemos a quien nos llamo (el kernel)
	//tras el ret_user al seleccionar alguna funcion salta aqui con x0 en 0
	cmp	x0, #0
	beq	thread_start
	ret

thread_start:
	//fp en 0
	mov	x29, 0
	/* Pick the function arg and execute.  */
	mov	x0, x11
	//llamo a la funcion en x10
	blr	x10

	/* We are done, pass the return value through x0.  */
	mov	x8, #SYS_EXIT_NUMBER
	svc	0x0