#include "kernel/scheduler.h"
#include "drivers/irq.h"
#include "printf.h"

static struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);
//Esto es posible porque hay mucho mas espacio en realidad que el de 64
struct task_struct * task[NR_TASKS] = {&(init_task), };

int nr_tasks = 1;
int current_task_pid = 0;
int next_task_pid = 0;

void preempt_disable(void)
{
	current->preempt_count++;
}

void preempt_enable(void)
{
	current->preempt_count--;
}

//Verifico si hay un nuevo proceso que debe adelantarse al que se este ejecutando en el momento
void _schedule(void)
{
	//Esta funcion es una seccion critica y no debe ser interrumpida
	preempt_disable();
	int next,c;
	struct task_struct * p;
	//Como estamos en una sola cpu la unica forma de cambiar el estado de un proceso es mediante un interrupt. ya que con que exista una sola el segundo for loop incrementa su contador asi que tras la primera interacion del while el p->counter sera minimo de 0
	while (1) {
		c = -1;
		next = 0;
		for (int i = 0; i < NR_TASKS; i++){
			p = task[i];
			//Si el proceso esta corriendo y su counter es 0 o mas c se vuelve ese valor y la siguiente proceso a ejectuar sera el. en uns OS real puede que no esten en running debido a espera por I/O o esperando una interrupcion o en su defecto todas tienen counter 0
			if (p && p->state == TASK_RUNNING && p->counter > c) {
				c = p->counter;
				next = i;
			}
		}
		//En caso de que sea 0 no se ejecuta ya que termino su tiempo y de ser mayor a 1 entonces este sera el proceso a ejecutar. Si no hay ningun task next = 0 y me ejecuto
		if (c) {
			break;
		}

		//Si no hay ningun candidato entonces cada taria se le incrementa su counter: Entre mas pasen el counter incrementa pero nunca mas que 2 * priority
		for (int i = 0; i < NR_TASKS; i++) {
			p = task[i];
			if (p) {
				p->counter = (p->counter >> 1) + p->priority;
			}
		}
		//repetimos hasta encontrar algo que haga el break
	}
	//La primera vez retornaara de ret_form_fork en los demas casos sera la direccion guardada en cpu_context por esta misma funcion
	current_task_pid = next_task_pid;
	next_task_pid = next;
	switch_to(task[next]);
	preempt_enable();
}

void schedule(void)
{
	current->counter = 0;
	_schedule();
}

void switch_to(struct task_struct * next) 
{
	/* for(int t=0; t < nr_tasks; t++) {
		p = task[t];
		printf("\n\rtask[%d] counter = %d\n\r", t, p->counter);
		printf("task[%d] priority = %d\n\r", t, p->priority);
		printf("task[%d] preempt_count = %d\n\r", t, p->preempt_count);
		printf("task[%d] sp = 0x%08x\n\r", t, p->cpu_context.sp);
		printf("\n\r###################################\r\n");
	} */
	
	if (current == next) 
		return;
	//Si no es la misma pued guardamos la actual como la anterior y la actual sera la siguiente
	struct task_struct * prev = current;
	current = next;
	printf("\n\r\n\r######## - Task switch - #########\r\n");
	printf("Current Task -> ");
	print_task_info(prev, current_task_pid);
	printf("New Task -> ");
	print_task_info(next, next_task_pid);
	printf("\n\r###################################\r\n");	
	printf("\n\rCurrent task output: ");
	//Context switch
	cpu_switch_to(prev, next);
}

void schedule_tail(void) {
	preempt_enable();
}

void print_task_info(struct task_struct * t, int pid){
	printf("PID: %d, Counter: %d, Priority: %d, Preempt_count: %d, Sp: 0x%08x \r\n", pid, t->counter, t->priority, t->preempt_count, t->cpu_context.sp);
}


void timer_tick()
{
	--current->counter; //Disminuyo el counter
	//En caso de que aun no sea 0 de que este en seccion critica no hago nada
	if (current->counter>0 || current->preempt_count >0) {
		return;
	}
	//me aseguro que el counter sea 0  y vuelvo a programar el scheduler
	current->counter=0;
	//Como estamos en un interrupt al llegar aqui se habilitan para poder luego planificar
	enable_irq();
	_schedule();
	disable_irq();
}