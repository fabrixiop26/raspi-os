#ifndef _MAILBOX_H
#define _MAILBOX_H

/**
 * \file mailbox.h
 * Interfaz de correo (Mailbox).
 * La forma de hacer un request en el mailbox es el siguiente:
 * 0. size of the message in bytes, (x+1)*4.
 * 1. MBOX_REQUEST magic value, indicates request message.
 * 2-x. tags con sus valores.
 * x+1. MBOX_TAG_LAST magic value, indicates no more tags,
 * Donde cada tag debe realizarse de la siguiente manera:
 * n+0 = tag identifier.
 * n+1 = value buffer (response) size in bytes.
 * n+2 = value buffer (response) size in bytes.
 * n+.. valores del buffer
 * @see <a href="https://github.com/bztsrc/raspi3-tutorial/tree/master/04_mailboxes">Adaptado de</a>
 * @see <a href="https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface">Mailbox Interface</a>
*/

#include "peripherals/base.h"

/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8  //Pedir a ARM para obtener respuestas del VideoCore IV

//Lista de tags
//https://github.com/isometimes/rpi4-osdev/blob/master/part5-framebuffer/mb.h
#define MBOX_TAG_GETSERIAL  0x10004
#define MBOX_TAG_LAST       0
#define MBOX_TAG_SETPOWER   0x28001
#define MBOX_TAG_SETCLKRATE 0x38002
#define MBOX_TAG_SETPHYWH   0x48003
#define MBOX_TAG_SETVIRTWH  0x48004
#define MBOX_TAG_SETVIRTOFF 0x48009
#define MBOX_TAG_SETDEPTH   0x48005
#define MBOX_TAG_SETPXLORDR 0x48006
#define MBOX_TAG_GETFB      0x40001
#define MBOX_TAG_GETPITCH   0x40008

//Clocks
#define MBOX_TAG_GETCLOCKRATE 0x30002
#define MBOX_TAG_CLOCK_ARM 0x000000003
#define MBOX_TAG_CLOCK_CORE 0x000000004
#define MBOX_TAG_CLOCK_UART 0x000000002

/**
 * Hace un llamado al mailbox para comunicacion.
 * @param ch El canal al cual enviar
 * @return La respuesta del mailbox. 1 para sucess 0 para error
*/
int mbox_call(unsigned char ch);

#endif