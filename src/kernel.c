#include <stddef.h>
#include <stdint.h>

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
#include "kernel/user.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

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

//Creo el proceso en el kernel y el mismo se encarga de pasarme a user mode
//Se devuelve a la funcion que lo llamo
void kernel_process()
{
    printf("Kernel process started. EL %d\r\n", get_el());
    unsigned long begin = (unsigned long)&user_begin;
    unsigned long end = (unsigned long)&user_end;
    unsigned long process = (unsigned long)&user_process;
    int err = move_to_user_mode(begin, end - begin, process - begin);
    if (err < 0)
    {
        printf("Error while moving process to user mode\n\r");
    }

    printf("Kernel process ended\r\n");
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

/* void mainWindowsCallback(UG_MESSAGE *msg)
{
    if (msg->type == MSG_TYPE_OBJECT)
    {
        if (msg->id == OBJ_TYPE_BUTTON)
        {
            switch (msg->sub_id)
            {
            case BTN_ID_0:
                printf("Button pressed \r\n");
                break;

            default:
                break;
            }
        }
    }
} */

/* void ShowLoadingCircle(UG_U16 x, UG_U16 y, UG_U16 r, UG_COLOR c1, UG_COLOR c2, unsigned char cond)
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
} */

UG_GUI gui;

/* void ShowWindows()
{
    UG_GUI gui;
    UG_Init(&gui, UserDrawPixel, P_WIDTH, P_HEIGHT);
    UG_WINDOW win;
    UG_TEXTBOX textBox;
    UG_OBJECT objs[2];
    UG_WindowCreate(&win, objs, 2, mainWindowsCallback);
    UG_WindowSetTitleTextFont(&win, &FONT_16X26);
    UG_WindowSetTitleTextColor(&win, C_YELLOW);
    UG_WindowSetTitleText(&win, "Test");
    UG_WindowSetTitleTextAlignment(&win, ALIGN_CENTER);

    UG_RESULT res = UG_TextboxCreate(&win, &textBox, TXB_ID_0, 150, 0, 330, 200);
    UG_TextboxSetFont(&win, TXB_ID_0, &FONT_10X16);
    UG_TextboxSetText(&win, TXB_ID_0, "This is an example");
    UG_TextboxSetForeColor(&win, TXB_ID_0, C_BLACK);
    UG_TextboxSetAlignment(&win, TXB_ID_0, ALIGN_TOP_CENTER);
    UG_WindowShow(&win);
    UG_Update();
} */

/**
*  Punto principal de entrada del programa.
*  @param proc_id Identificador de la cpu
*/
void kernel_main()
{
    //only the master (processor id = 0) initialize uart
    
        uart_init();
        init_printf(NULL, putc);
        irq_vector_init();
        timer_init();
        enable_interrupt_controller();
        enable_irq();
        //espera 0.12s antes de inicializar el framebuffer
        printf("Iniciando framebuffer\r\n");
        wait_msec(120000);
        fb_init();
        //Iniciar ugui
        UG_Init(&gui, UserDrawPixel, 480, 320);
        wait_msec(120000);
        UG_FontSelect(&FONT_16X26);
        UG_SetForecolor(C_YELLOW);
        UG_PutString(136, 0, "Max Ventas OS");
        printf("To bien gui? \r\n");
        UG_ConsoleSetArea(0, 60, 480, 260);
        UG_SetForecolor(C_WHITE);
        UG_FontSelect(&FONT_10X16);
        UG_ConsoleSetBackcolor(C_BLACK);
        UG_ConsoleSetForecolor(C_WHITE);
        UG_ConsolePutString("Iniciando consola ... \n");
        UG_ConsoleSetForecolor(C_GREEN);
        UG_ConsolePutString("System Initialization Complete.\n");
        UG_ConsoleSetForecolor(C_WHITE);
        UG_ConsolePutString(">");
    
        //clock_rates();

        int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0);
        if (res < 0)
        {
            printf("Error while starting kernel process");
            return;
        }

        //No hay necesidad de retransmitir ya que tengo configurado las interrupciones de uart para esto
        while (1)
        {
            schedule();
        }
    
    //Core 1 just draws
    /* if(proc_id == 1){
        while (semaphore != 4)
        {
           
        }

        draw();
        
    } */
}