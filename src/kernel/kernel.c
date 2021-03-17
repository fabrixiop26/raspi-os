#include "drivers/uart.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

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
        uart_send_string("Hello, world!\r\n"); //\r mueve el "cursor al principio de la linea" */
    }

    uart_send_string("Processor #");
    uart_send(proc_id + '0'); //Se contatena 0 para volverlo ascii
    uart_send_string(" initialized");
    uart_send_string("\r\n");

    //Aumento la variable de control para que la siguiente cpu salga del bucle
    semaphore++;

    //Solo la cpu 0 tiene permitido hacer echo, los demas terminan
    if (proc_id == 0)
    {
        // Wait for everyone else to finish
        while (semaphore != 4)
        {
        }

        while (1)
        {
            uart_send(uart_recv());
        }
    }
}