#include "mm.h"
#include "kernel/scheduler.h"
#include "entry.h"
#include "printf.h"

//no crea cambios de contexto solo prepara el nuevo proceso
int copy_process(unsigned long fn, unsigned long arg, unsigned int priority)
{
	//Evitamos que el scheduler nos saque de aqui y asignamos su espacio de memoria al nuevo proceso que se usara como su stack
	preempt_disable();
	struct task_struct *p, *previous_task;
	p = (struct task_struct *) get_free_page();
	//Si no hay mas espacio pues retornamos con error
	if (!p)
		return 1;
	//Mantemeos la misma prioridad
	p->priority = priority;
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
	//Este nuevo proceso a punta a nada
	p->next_task = 0; 
	//Procesos limitados sistema anterior estatico
	/* int pid = nr_tasks++;
	task[pid] = p; */	
	previous_task = initial_task;	//Apunto a la primera tarea (kernel OS)
	while(previous_task->next_task){
		previous_task = previous_task->next_task;
	} 

	previous_task->next_task = p; //La tarea anterior a punta a esta nueva
	//nr_tasks++; //Incremento el nr task para control

	printf("\n\r----------- Task created -----------\r\n");
	printf("\n\rStruct task allocated at 0x%08x.\r\n", p);
	printf("p->cpu_context.x19 = 0x%08x. (fn)\r\n", p->cpu_context.x19);
	printf("p->cpu_context.x20 = 0x%08x. (arg)\r\n", p->cpu_context.x20);
	printf("p->cpu_context.pc  = 0x%08x. (ret_from_fork)\r\n", p->cpu_context.pc);
	printf("p->cpu_context.sp  = 0x%08x. (sp)\r\n", p->cpu_context.sp);
	printf("Memory Pages occupied: %d/%d \r\n", current_pages, PAGING_PAGES);
	printf("\n\r----------- Previous task -----------\r\n");
	printf("previous_task->next_task  = 0x%08x.\r\n", previous_task->next_task);

	preempt_enable();
	return 0;
}