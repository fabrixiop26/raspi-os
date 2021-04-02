#ifndef _SCHED_H
#define _SCHED_H

/**
 * \file scheduler.h
 Contiene definiciones y metodos para el planificador de procesos.
*/

#define THREAD_CPU_CONTEXT			0 		// offset of cpu_context in task_struct 

#ifndef __ASSEMBLER__

#define THREAD_SIZE				4096 //4Kb

//#define NR_TASKS				64 sistema estatico

//#define FIRST_TASK task[0] sistema estatico
//#define LAST_TASK task[NR_TASKS-1] sistema estatico

#define TASK_RUNNING				0

/**
 * El proceso que se esta ejecutando.
*/
extern struct task_struct *current;
/**
 * El proceso padre de todos (OS).
*/
extern struct task_struct *initial_task;
/**
 * Arreglo con la lista de procesos.
*/
//extern struct task_struct * task[NR_TASKS];
/**
 * Numero de procesos ejecutandose en el sistema.
*/
//extern int nr_tasks;
extern int current_pages;

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
 @var task_strcut::next_task
 Apunta al siguiente proceso en memoria
*/
struct task_struct {
	struct cpu_context cpu_context;
	long state;	
	long counter;
	long priority;
	long preempt_count;
	struct task_struct *next_task;
};


extern void sched_init(void);
/**
 * Ejecuta el algoritmo para planificar los procesos.
*/
extern void schedule(void);
/**
 * Llama al planificador de procesos cada que ocurre una interrupcion por timer.
*/
extern void timer_tick(void);
/**
 * Evita que el proceso actual sea cambiado por otro
*/
extern void preempt_disable(void);
/**
 * Permite que el proceso actual sea cambiado por otro.
*/
extern void preempt_enable(void);
/**
 * Inicializa el cambio de contexto entre procesos.
*/
extern void switch_to(struct task_struct* next);
/**
 * Realiza el cambio de contexto entre 2 procesos.
*/
extern void cpu_switch_to(struct task_struct* prev, struct task_struct* next);

void print_task_info(struct task_struct* t, int pid);

#define INIT_TASK \
/*cpu_context*/	{ {0,0,0,0,0,0,0,0,0,0,0,0,0}, \
/* state etc */	0,0,1, 0,0 \
}

#endif
#endif