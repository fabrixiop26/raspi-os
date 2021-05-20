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
#include "kernel/pictures.h"
/**
 * \file kernel.c
 * \brief Funciones principales del kernel
 */
static unsigned int semaphore = 0; ///< variable comun entre cpus para control*/

int ready = 1;

void changeState(){
    ready = 0;
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

//No necesito esto
void window_callback(UG_MESSAGE *msg)
{
}
UG_GUI gui;
UG_WINDOW main_window;
UG_IMAGE samir_pic;
UG_BMP samir_bpm = {
    (void *)samir_bits,
    60,
    60,
    BMP_BPP_16,
    BMP_RGB565};
UG_IMAGE soto_pic;
UG_BMP soto_bpm = {
    (void *)soto_bits,
    60,
    60,
    BMP_BPP_16,
    BMP_RGB565};
UG_IMAGE melyssa_pic;
UG_BMP melyssa_bpm = {
    (void *)melyssa_bits,
    60,
    60,
    BMP_BPP_16,
    BMP_RGB565};
UG_IMAGE puche_pic;
UG_BMP puche_bpm = {
    (void *)puche_bits,
    60,
    60,
    BMP_BPP_16,
    BMP_RGB565};
UG_IMAGE un_pic;
UG_BMP un_bpm = {
    (void *)un_bits,
    60,
    60,
    BMP_BPP_16,
    BMP_RGB565};
UG_TEXTBOX textbox;
UG_TEXTBOX textbox_os;
//Maximo 4 imagenes y un texto
UG_OBJECT obj_buff_wnd[7];

void init_console()
{

    UG_RESULT hide_res = UG_WindowHide(&main_window);
    printf("Pantalla oculta %d\r\n", hide_res);
    UG_Update();

    UG_FillScreen(C_BLACK);

    UG_Update();

    delay(200000);

    UG_FontSelect(&FONT_16X26);
    UG_SetForecolor(C_YELLOW);
    UG_PutString(136, 0, "Max Ventas OS");
    UG_ConsoleSetArea(0, 60, 480, 260);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_10X16);
    UG_ConsoleSetForecolor(C_GREEN);
    UG_ConsolePutString("System Initialization Complete.\n");
    UG_ConsoleSetForecolor(C_WHITE);
    UG_ConsolePutString(">");
}

void showPictures()
{
    //Mostrar imagenes
    //Samir
    unsigned int xInit = 47;
    UG_ImageCreate(&main_window, &samir_pic, IMG_ID_0, xInit, 50, samir_bpm.width + xInit, samir_bpm.height + 50);
    UG_ImageSetBMP(&main_window, IMG_ID_0, &samir_bpm);
    UG_ImageShow(&main_window, IMG_ID_0);

    xInit += (60 + 47);

    UG_ImageCreate(&main_window, &soto_pic, IMG_ID_1, xInit, 50, soto_bpm.width + xInit, soto_bpm.height + 50);
    UG_ImageSetBMP(&main_window, IMG_ID_1, &soto_bpm);
    UG_ImageShow(&main_window, IMG_ID_1);

    xInit += (60 + 47);

    UG_ImageCreate(&main_window, &melyssa_pic, IMG_ID_2, xInit, 50, melyssa_bpm.width + xInit, melyssa_bpm.height + 50);
    UG_ImageSetBMP(&main_window, IMG_ID_2, &melyssa_bpm);
    UG_ImageShow(&main_window, IMG_ID_2);

    xInit += (60 + 47);

    UG_ImageCreate(&main_window, &puche_pic, IMG_ID_3, xInit, 50, puche_bpm.width + xInit, puche_bpm.height + 50);
    UG_ImageSetBMP(&main_window, IMG_ID_3, &puche_bpm);
    UG_ImageShow(&main_window, IMG_ID_3);

    UG_WindowShow(&main_window);
    UG_Update();
}

void showWindow()
{

    UG_FillScreen(C_BLACK);
    wait_msec(10000);
    UG_WindowShow(&main_window);
    UG_Update();

    //Preparo la consola inicial
    UG_ConsoleSetArea(0, 240, 480, 320);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_10X16);
    UG_ConsoleSetForecolor(C_WHITE);
    UG_ConsolePutString(">");
}

void showInitWindow()
{

    UG_FillScreen(C_BLACK);
    UG_Update();
    wait_msec(10000);
    //Preparar ventana
    UG_WindowCreate(&main_window, obj_buff_wnd, 7, window_callback);
    UG_WindowSetTitleTextFont(&main_window, &FONT_16X26);
    UG_WindowSetTitleText(&main_window, "Max Ventas OS");
    UG_WindowSetTitleTextColor(&main_window, C_YELLOW);
    UG_WindowSetTitleTextAlignment(&main_window, ALIGN_TOP_CENTER);

    //Titulo
    UG_TextboxCreate(&main_window, &textbox, TXB_ID_0, 0, 10, 400, 30);
    UG_TextboxSetFont(&main_window, TXB_ID_0, &FONT_10X16);
    UG_TextboxSetAlignment(&main_window, TXB_ID_0, ALIGN_CENTER);
    UG_TextboxSetText(&main_window, TXB_ID_0, "OS Para Raspberry PI 3 Creado por:");
    UG_TextboxSetForeColor(&main_window, TXB_ID_0, C_BLACK);

    showPictures();

    //Textbox con la materia
    UG_TextboxCreate(&main_window, &textbox_os, TXB_ID_2, 90, 120, 419, 200);
    UG_TextboxSetFont(&main_window, TXB_ID_2, &FONT_10X16);
    UG_TextboxSetAlignment(&main_window, TXB_ID_2, ALIGN_CENTER);
    UG_TextboxSetText(&main_window, TXB_ID_2, "Para la materia de \n Sistemas Operativos");
    //UG_TextboxSetBackColor(&main_window, TXB_ID_2, C_WHITE);
    UG_TextboxSetForeColor(&main_window, TXB_ID_2, C_BLACK);

    //Logo uninorte
    UG_ImageCreate(&main_window, &un_pic, IMG_ID_5, 70, 130, un_bpm.width + 70, un_bpm.height + 130);
    UG_ImageSetBMP(&main_window, IMG_ID_5, &un_bpm);
    UG_ImageShow(&main_window, IMG_ID_5);

    UG_RESULT result = UG_WindowResize(&main_window, 0, 0, 479, 240);
    UG_Update();
    //Preparo la consola inicial
    UG_ConsoleSetArea(0, 240, 480, 320);
    UG_SetForecolor(C_WHITE);
    UG_FontSelect(&FONT_10X16);
    UG_ConsoleSetForecolor(C_WHITE);
    UG_ConsolePutString(">");

    UG_Update();
}

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
    //init_console();
    UG_Init(&gui, UserDrawPixel, 480, 320);
    showInitWindow();
    //clock_rates();

    while(ready);

    int res = copy_process(PF_KTHREAD, (unsigned long)&kernel_process, 0, 1);
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
}