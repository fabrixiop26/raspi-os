#ifndef _SCHED_H
#define _SCHED_H

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096

#define NR_TASKS				64 

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#define TASK_RUNNING				0

/**
 * El proceso que se esta ejecutando.
*/
extern struct task_struct *current;
/**
 * Arreglo con la lista de procesos.
*/
extern struct task_struct * task[NR_TASKS];
/**
 * Numero de procesos ejecutandose en el sistema
*/
extern int nr_tasks;

/**
 * @struct cpu_context
 @brief Registros que son diferentes entre procesos.
 Los cambios de contexto solo ocurren cuando un proceso llama a la funcion cpu_switch_to, desde el punto de vista del proceso al llamar a esta funcion despues de un tiempo esta retorna. Segun ARM los reistros del x0-x18 pueden ser sobreescritos cuando se llama a una funcion.
*/
//fp = x29 y pc = x30
struct cpu_context {
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp;
	unsigned long sp;
	unsigned long pc;
};

/**
 * @struct task_struct
 @brief Estructura basica para cada proceso.
 @var task_struct::cpu_context 
 El estado de los registros.
 @var task_struct::state 
 Estado del proceso actual. Si esta operando solo en la CPU sera siempre TASK_RUNNING.
 @var task_struct::counter 
 Cantidad de tiempo que el proceso puede estar corriendo.
 @var task_struct::priority 
 Prioridad del proceso.
 @var task_struct::preempt_count 
 Controla secciones criticas (Non-Zero Value)
*/
struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter;
	long priority;
	long preempt_count;
};


extern void sched_init(void);
extern void schedule(void);
extern void timer_tick(void);
extern void preempt_disable(void);
extern void preempt_enable(void);
extern void switch_to(struct task_struct* next);
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0 \
}

#endif
#endif