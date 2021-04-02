#include "kernel/fork.h"
#include "printf.h"
#include "kernel/scheduler.h"
#include "mm.h"

void sys_write(char * buf){
	printf(buf);
}

int sys_clone(unsigned long stack){
	return copy_process(0, 0, 0, stack, 1);
}

unsigned long sys_malloc(){
	unsigned long addr = get_free_page();
	if (!addr) {
		return -1;
	}
	return addr;
}

void sys_exit(){
	exit_process();
}

//Lista con apuntadores a los llamados
void * const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit};