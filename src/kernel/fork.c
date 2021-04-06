#include "mm.h"
#include "kernel/scheduler.h"
#include "entry.h"
#include "printf.h"
#include "kernel/fork.h"
#include "utils.h"

//no crea cambios de contexto solo prepara el nuevo proceso
int copy_process(unsigned long clone_flags, unsigned long fn, unsigned long arg, unsigned long stack)
{
	//Evitamos que el scheduler nos saque de aqui y asignamos su espacio de memoria al nuevo proceso que se usara como su stack
	preempt_disable();
	struct task_struct *p, *previous_task;
	p = (struct task_struct *) get_free_page();
	//Si no hay mas espacio pues retornamos con error
	if (!p)
		return -1;

	struct pt_regs *childregs = task_pt_regs(p);
	memzero((unsigned long)childregs, sizeof(struct pt_regs));
	memzero((unsigned long)&p->cpu_context, sizeof(struct cpu_context));

	//Si somos un hilo de kernel tratamos normal al proceso
	if (clone_flags & PF_KTHREAD) {
		p->cpu_context.x19 = fn;
		p->cpu_context.x20 = arg;
	} else {
		//Como al pasar a user mode dejamos su stack vacio y sp apuntado encima de sus childregs entonces aseguramos que cur_regs apunta a lo mismo. Aqui se guarda elr_el1 y demas valores del kernel entry incluyendo x10 x11 y x12 donde estan las funciones
		struct pt_regs * cur_regs = task_pt_regs(current);
		//Al nuevo proceso le pasamos la misma info de pt regs que la del kernel
		*childregs = *cur_regs;
		//al x0 le asignamos 0 ya que sera el valor de retorno para el caller esto en el assembly nos permitira saber si estamos en el mismo hilo o en el nuevo tambien fn sera 0 para salirnos del ret_from_fork
		childregs->regs[0] = 0;
		//Apuntamos el sp del pt regs al fondo de la pagina usada para esta tarea
		childregs->sp = stack + PAGE_SIZE;
		//Guardamos en la tarea actual un a puntador a este stack para poder limpiarlo
		p->stack = stack;
	}

	p->flags = clone_flags;
	//Asignamos la prioridad indicada
	p->priority = current->priority;
	p->state = TASK_RUNNING;
	//Asignamos el numero de quantums del nuevo proceso basado en la prioridad
	//Mayor prioridad = mayor tiempo en cpu
	p->counter = p->priority;
	p->preempt_count = 1; //disable preemtion until schedule_tail para evitar que sea cambiada sin que antes haga cierta inicializacion
	//Guardamos la direccion de memoria de la funcion a llamar en x18 y sus argumentos en x20
	//p->cpu_context.x19 = fn;
	//p->cpu_context.x20 = arg;

	//Apunta a la funcion ret_from_fork la cual activa preempt y llama a la funcion pasada
	p->cpu_context.pc = (unsigned long)ret_from_fork;
	//stack pointer es igual a la direccion antes de pt_regs
	p->cpu_context.sp = (unsigned long)childregs;
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
	nr_tasks++; //Incremento el nr task para control

	printf("\n\r----------- Task created -----------\r\n");
	printf("\n\rStruct task allocated at 0x%08x.\r\n", p);
	printf("p->cpu_context.x19 = 0x%08x. (fn)\r\n", p->cpu_context.x19);
	printf("p->cpu_context.x20 = 0x%08x. (arg)\r\n", p->cpu_context.x20);
	printf("p->cpu_context.pc  = 0x%08x. (ret_from_fork)\r\n", p->cpu_context.pc);
	printf("p->cpu_context.sp  = 0x%08x. (sp)\r\n", p->cpu_context.sp);
	printf("Memory Pages occupied: %d/%d \r\n", current_pages, PAGING_PAGES);
	printf("\n\r----------- Previous task -----------\r\n");
	printf("previous_task->next_task  = 0x%08x.\r\n", previous_task->next_task);
	printf("Stack Pointer En Fork: 0x%08x \r\n", get_stack_pointer());

	preempt_enable();
	//Devuelo el "PID"
	return nr_tasks;
}

int move_to_user_mode(unsigned long pc)
{
	struct pt_regs *regs = task_pt_regs(current);
	//Se limpia esta area de la memoria (4 safety)
	memzero((unsigned long)regs, sizeof(*regs));
	//El pc de regs apunta a la funcion a llamar en user mode ya que de aqui al ejecutarse kernel_exit pc se copia a elr_el1 y de esta forma ir a la funcion al retornar
	regs->pc = pc;
	//Al retornar de la exception estaremos en EL0
	regs->pstate = PSR_MODE_EL0t;
	unsigned long stack = get_free_page(); //allocate new user stack
	if (!stack) {
		return -1;
	}
	//El sp apunta al fondo del stack (origin) de la nueva pagina
	regs->sp = stack + PAGE_SIZE;
	//El stack del proceso actual a punta a la nueva pagina para el usuario
	current->stack = stack;
	return 0;
}

//Obtiene el tamaño del primer byte donde poder guardar pt_regs en el top del stack
struct pt_regs * task_pt_regs(struct task_struct *tsk){
	unsigned long p = (unsigned long)tsk + THREAD_SIZE - sizeof(struct pt_regs);
	return (struct pt_regs *)p;
}