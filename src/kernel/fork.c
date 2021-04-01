#include "mm.h"
#include "kernel/scheduler.h"
#include "entry.h"

//no crea cambios de contexto solo prepara el nuevo proceso
int copy_process(unsigned long fn, unsigned long arg)
{
	//Evitamos que el scheduler nos saque de aqui y asignamos su espacio de memoria al nuevo proceso que se usara como su stack
	preempt_disable();
	struct task_struct *p;
	p = (struct task_struct *) get_free_page();
	//Si no hay mas espacio pues retornamos con error
	if (!p)
		return 1;
	//Mantemeos la misma prioridad
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	//Asignamos el numero de quantums del nuevo proceso basado en la prioridad
	//Mayor prioridad = mayor tiempo en cpu
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail para evitar que sea cambiada sin que antes haga cierta inicializacion
	//Guardamos la direccion de memoria de la funcion a llamar en x18 y sus argumentos en x20
	p->cpu_context.x19 = fn;
	p->cpu_context.x20 = arg;
	//Apunta a la funcion ret_from_fork la cual activa preempt y llama a la funcion pasada
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	//stack pointer es igual a la direccion de la pagina + tamaño de la pagina
	p->cpu_context.sp = (unsigned long)p + THREAD_SIZE;
	int pid = nr_tasks++;
	task[pid] = p;	
	preempt_enable();
	return 0;
}