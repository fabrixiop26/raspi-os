#include "drivers/uart.h"
#include "drivers/irq.h"
#include "printf.h"
#include "utils.h"
#include "drivers/timer.h"
#include "drivers/framebuffer.h"
#include "kernel/fork.h"
#include "kernel/scheduler.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

void process(char *array)
{
    int cont = 200;
    while (cont >= 0)
    {
        for (int i = 0; i < 5; i++)
        {
            uart_send(array[i]);
            delay(100000);
        }

        cont--;
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
        int el = get_el();
        printf("Exception level: %d \r\n", el); //\r mueve el "cursor al principio de la linea"

        //Pasamos la funcion que quieremos copiar y los argumentos
        int res = copy_process((unsigned long)&process, (unsigned long)"12345", 1);
        if (res != 0)
        {
            printf("error while starting process 1");
            return;
        }
        res = copy_process((unsigned long)&process, (unsigned long)"abcde", 1);
        if (res != 0)
        {
            printf("error while starting process 2");
            return;
        }
    }

    //printf("Processor # %c initialized \r\n", (proc_id + '0'));

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

        //No hay necesidad de retransmitir ya que tengo configurado las interrupciones de uart para esto
        while (1)
        {
            schedule();
            //uart_send(uart_recv());
        }
    }
}