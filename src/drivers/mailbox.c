#include "peripherals/base.h"
#include "utils.h"

//se necesita que este alineado a 16 bytes ya que solo usamos los 28 bits de la direccion superiores ya que los otros 4 estan reservados para el canal
//note cambiar la direccion del compilador para que detecte esto
volatile unsigned int __attribute__((aligned(16))) mbox[36];


#define VIDEOCORE_MBOX (PBASE + 0x0000B880)
#define MBOX_READ (VIDEOCORE_MBOX + 0x0)
#define MBOX_POLL (VIDEOCORE_MBOX + 0x10)
#define MBOX_SENDER (VIDEOCORE_MBOX + 0x14)
#define MBOX_STATUS (VIDEOCORE_MBOX + 0x18)
#define MBOX_CONFIG (VIDEOCORE_MBOX + 0x1C)
#define MBOX_WRITE (VIDEOCORE_MBOX + 0x20)
#define MBOX_RESPONSE 0x80000000
#define MBOX_FULL 0x80000000
#define MBOX_EMPTY 0x40000000


int mbox_call(unsigned char ch)
{
    // 28-bit address (MSB) and 4-bit value (LSB)
    unsigned int r = (((unsigned int)((unsigned long)&mbox) & ~0xF) | (ch & 0xF));
    /* wait until we can write to the mailbox */
    while (get32(MBOX_STATUS) & MBOX_FULL)
    {
    }
    //write the address of our message to the mailbox with channel identifier
    put32(MBOX_WRITE, r);
    /* now wait for the response */
    while (1)
    {
        /* is there a response? */
        while (get32(MBOX_STATUS) & MBOX_FULL)
        {
        }
        /* is it a response to our message? */
        if (r == get32(MBOX_READ))
        {
            /* is it a valid successful response? */
            //arroja 1 en caso de que si
            return mbox[1] == MBOX_RESPONSE;
        }
    }
    return 0;
}