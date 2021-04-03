#include "drivers/uart.h"
#include "drivers/irq.h"
#include "printf.h"
#include "utils.h"
#include "drivers/timer.h"
#include "kernel/fork.h"
#include "kernel/scheduler.h"
#include "kernel/sys.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

void user_process1(char *array)
{
    char buf[2] = {0};
    int cont = 200;
    while (cont >= 0)
    {
        for (int i = 0; i < 5; i++)
        {
            buf[0] = array[i];
            call_sys_write(buf);
            delay(100000);
        }
        cont--;
    }
}

void user_process()
{
    //El equivalente a sprintf
    //el buffer debe tener tamaño indicado
    char buf[30] = {0};
    tfp_sprintf(buf, "User process started\n\r");
    call_sys_write(buf);
    unsigned long stack = call_sys_malloc();
    if (stack < 0)
    {
        printf("Error while allocating stack for process 1\n\r");
        return;
    }
    int err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"12345", stack);
    if (err < 0)
    {
        printf("Error while clonning process 1\n\r");
        return;
    }
    stack = call_sys_malloc();
    if (stack < 0)
    {
        printf("Error while allocating stack for process 1\n\r");
        return;
    }
    err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcd", stack);
    if (err < 0)
    {
        printf("Error while clonning process 2\n\r");
        return;
    }
    call_sys_exit();
}

//Creo el proceso en el kernel y el mismo se encarga de pasarme a user mode
void kernel_process()
{
    printf("Kernel process started. EL %d\r\n", get_el());
    int err = move_to_user_mode((unsigned long)&user_process);
    if (err < 0)
    {
        printf("Error while moving process to user mode\n\r");
    }
}

/**
*  Punto principal de entrada del programa.
*  @param proc_id Identificador de la cpu
*/
void kernel_main(char proc_id)
{
    //Wait for CPU #0 to finish initalizacion of uart
    while (proc_id != semaphore)
    {
    }
    //only the master (processor id = 0) initialize uart
    if (proc_id == 0)
    {
        uart_init();
        init_printf(0, putc);
        irq_vector_init();
        timer_init();
        enable_interrupt_controller();
        enable_irq();
        //printf("Iniciando framebuffer\r\n");
        //espera 0.12s antes de inicializar el framebuffer
        //wait_msec(120000);
        //fb_init();
    }

    printf("Processor # %c initialized \r\n", (proc_id + '0'));

    //Aumento la variable de control para que la siguiente cpu salga del bucle
    semaphore++;

    //Solo la cpu 0 tiene permitido quedarse aqui los demas terminan
    //y se van a la linea de assembly del proc_hang
    if (proc_id == 0)
    {
        // Wait for everyone else to finish
        while (semaphore != 4)
        {
        }
        int el = get_el();
        printf("Exception level: %d \r\n", el); //\r mueve el "cursor al principio de la linea"

        int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process,0,0);
        if (res < 0)
        {
            printf("Error while starting kernel process");
            return;
        }

        //No hay necesidad de retransmitir ya que tengo configurado las interrupciones de uart para esto
        while (1)
        {
            schedule();
            //uart_send(uart_recv());
        }
    }
}