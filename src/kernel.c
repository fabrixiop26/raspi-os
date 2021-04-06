#include "drivers/uart.h"
#include "drivers/irq.h"
#include "printf.h"
#include "utils.h"
#include "drivers/timer.h"
#include "kernel/fork.h"
#include "kernel/scheduler.h"
#include "kernel/sys.h"
#include "drivers/framebuffer.h"
#include "drawutils.h"
#include "drivers/mailbox.h"
#include "ugui.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

/* void user_process1(char *array)
{
    printf("Stack Pointer Proceso 1: 0x%08x \r\n", get_stack_pointer());
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
} */

void user_process1(char *array)
{
    while (1)
    {

        printf(array);
        delay(100000);
    }
}

void draw()
{
    //printf("Draw Process \r\n");
    unsigned char col = 0x00;
    unsigned long initTime = 0;
    unsigned long endTime = 0;
    unsigned long elapsed = 0;
    unsigned int fps = 0;
    unsigned int control = 30;
    while (1)
    {
        initTime = get_system_timer();
        for (int i = 0; i < 480; i++)
        {
            for (int j = 0; j < 320; j++)
            {
                drawPixel(i, j, col);
                //col += 1;
                //col %= 16;
            }
        }
        endTime = get_system_timer();
        elapsed = endTime - initTime;
        fps = 1000000 / elapsed;
        char buff[30] = {0};
        tfp_sprintf(buff, "FPS: %d", fps);
        drawString(100, 100, buff, 0x0f, ZOOM);
        if (control <= 0)
        {
            col += 1;
            col %= 16;
            control = 30;
        }
        control--;
    }
}

//Todo el codigo aqui tiene su stack sin embargo los syscalls se hacen en el kernel process ya que deben cambiar de el0 a el1
void user_process()
{
    //El equivalente a sprintf
    //el buffer debe tener tamaño indicado
    printf("Stack Pointer User Process: 0x%08x \r\n", get_stack_pointer());
    char buf[30] = {0};
    tfp_sprintf(buf, "User process started\n\r");
    call_sys_write("User Process Started \r\n");
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
        printf("Error while allocating stack for process 2\n\r");
        return;
    }
    err = call_sys_clone((unsigned long)&user_process1, (unsigned long)"abcde", stack);
    if (err < 0)
    {
        printf("Error while clonning process 2\n\r");
        return;
    }
    printf("\r\nTerminado creacion de 2 procesos en user mode \r\n");
    printf("Stack Pointer Fin User Process: 0x%08x \r\n", get_stack_pointer());
    call_sys_exit();
}

//Creo el proceso en el kernel y el mismo se encarga de pasarme a user mode
//Se devuelve a la funcion que lo llamo
void kernel_process()
{
    printf("Kernel process started. EL %d\r\n", get_el());
    int err = move_to_user_mode((unsigned long)&user_process);
    if (err < 0)
    {
        printf("Error while moving process to user mode\n\r");
    }
    printf("Stack Pointer Kernel Process: 0x%08x \r\n", get_stack_pointer());
}

//Muevo la pantalla 10 pixeles hacia abajo?
void offset_screen()
{
    mbox[0] = 8 * 4; // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SETVIRTOFF;
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 0;  // Value(x)
    mbox[6] = 20; // Value(y)
    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP) && mbox[6] == 20)
    {
        unsigned int offset_y = mbox[6];
        printf("Offset en Y: %d", offset_y);
        drawString(0, 300, "Reemplazo", 0x0f, ZOOM);
    }
}

void clock_rates()
{
    mbox[0] = 8 * 4; // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_GETCLOCKRATE;
    mbox[3] = 4;
    mbox[4] = 8;
    mbox[5] = MBOX_TAG_CLOCK_ARM;
    mbox[6] = 0; //clock_rate
    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP) && mbox[6] != 0)
    {
        unsigned int rate = mbox[6];
        printf("ARM Clock Frecuency: %d Mhz \r\n", rate / 1000000);
    }
    else
    {
        printf("Error en arm clock rate \r\n");
    }

    mbox[0] = 8 * 4; // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_GETCLOCKRATE;
    mbox[3] = 4;
    mbox[4] = 8;
    mbox[5] = MBOX_TAG_CLOCK_CORE;
    mbox[6] = 0; //clock_rate
    mbox[7] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP) && mbox[6] != 0)
    {
        unsigned int rate = mbox[6];
        printf("Core Clock Frecuency: %d Mhz \r\n", rate / 1000000);
    }
    else
    {
        printf("Error en core clock rate \r\n");
    }
}

//Custom drawing
void UserDrawPixel(UG_S16 x, UG_S16 y, UG_COLOR c)
{
    drawPixel2(x, y, c);
}

void ShowLoadingCircle(UG_U16 x, UG_U16 y, UG_U16 r, UG_COLOR c1,  UG_COLOR c2, unsigned char cond)
{
    UG_U16 sec;
    UG_U8 j, tog;
    while (cond)
    {
        for (sec = 1; sec != 0x100; sec <<= 1)
        {
            j++;
            if (j >= 9)
            {
                j = 0;
                tog = !tog;
            }
            if (tog)
            {
                UG_DrawArc(100, 200, 20, sec, c1);
            }
            else
            {
                UG_DrawArc(100, 200, 20, sec, c2);
            }

            wait_msec(100000);
        }
    }
}

UG_GUI gui;

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
        printf("Iniciando framebuffer\r\n");
        //espera 0.12s antes de inicializar el framebuffer
        wait_msec(120000);
        fb_init();
        //Iniciar ugui
        UG_Init(&gui, UserDrawPixel, P_WIDTH, P_HEIGHT);
        UG_FontSelect(&FONT_10X16);
        UG_ConsoleSetArea(0, 60, 480, 120);
        UG_ConsoleSetBackcolor(C_BLACK);
        UG_ConsoleSetForecolor(C_WHITE);
        UG_ConsolePutString("Iniciando consola ... \n");
        UG_ConsoleSetForecolor(C_GREEN);
        UG_ConsolePutString("System Initialization Complete.\n");

        ShowLoadingCircle(120,200,30, C_BLACK, C_WHITE, 1);
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
        //Creamos nuevo kernel thread
        /* int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 0);
        if (res < 0)
        {
            printf("Error while starting kernel process");
            return;
        }
        printf("\r\nCreado kernel_process \r\n"); */
        //draw_test();
        //offset_screen();
        clock_rates();
        //No hay necesidad de retransmitir ya que tengo configurado las interrupciones de uart para esto
        while (1)
        {
            schedule();
        }
    }
    //Core 1 just draws
    /* if(proc_id == 1){
        while (semaphore != 4)
        {
           
        }

        draw();
        
    } */
}