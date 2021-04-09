#include "peripherals/base.h"
#include "drivers/mailbox.h"
#include "terminal.h"
#include "utils.h"
#include "printf.h"
#include "drawutils.h"

unsigned int width, height, pitch, isrgb, fb_size;
//apuntador al framebuffer
unsigned char *fb;

//Despues de cada tag debe especificarse Value buffer size (bytes) y Req. + value length (bytes)
void fb_init()
{

    //(x+1)*4 donde x seria el ultimo indice
    mbox[0] = 35 * 4; // Length of message in bytes
    mbox[1] = MBOX_REQUEST;

    mbox[2] = MBOX_TAG_SETPHYWH; // Tag identifier
    mbox[3] = 8;                 // Value size in bytes
    mbox[4] = 8;                 // Req. + value length (bytes)
    mbox[5] = P_WIDTH;               // Value(width)
    mbox[6] = P_HEIGHT;               // Value(height)

    mbox[7] = MBOX_TAG_SETVIRTWH;
    mbox[8] = 8; //Value size in bytes
    mbox[9] = 8; //Req. + value length (bytes)
    mbox[10] = V_WIDTH; //virutal width
    mbox[11] = V_HEIGHT; //virtual height

    mbox[12] = MBOX_TAG_SETVIRTOFF;
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0; // Value(x)
    mbox[16] = 0; // Value(y)

    mbox[17] = MBOX_TAG_SETDEPTH;
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32; // Bits per pixel

    mbox[21] = MBOX_TAG_SETPXLORDR;
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 0; // 0 RGB? y 1 BGR? RGB o BGR

    mbox[25] = MBOX_TAG_GETFB;
    mbox[26] = 8; //Value buffer size (bytes)
    mbox[27] = 8; //Req. + value length (bytes)
    mbox[28] = 4096; // FrameBufferInfo.pointer (bytes)
    mbox[29] = 0;    // FrameBufferInfo.size (bytes)

    mbox[30] = MBOX_TAG_GETPITCH;
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0; // Bytes per line

    mbox[34] = MBOX_TAG_LAST;

    // Check call is successful and we have a pointer with depth 32
    if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0)
    {
        mbox[28] &= 0x3FFFFFFF; // Convert GPU address to ARM address
        width = mbox[5];       // Actual physical width
        height = mbox[6];      // Actual physical height
        pitch = mbox[33];       // Number of bytes per line
        isrgb = mbox[24];       // Pixel order
        fb_size = mbox[29];    //Obtengo el tamaño
        fb = (unsigned char *)((long)mbox[28] + VA_START);
        printf("####Framebuffer Info:###### \r\n");
        printf("Framebuffer address: 0x%08x \r\n", fb);
        printf("Physical Width: %d\r\n", width);
        printf("Physical Height: %d\r\n", height);
        printf("RGB?: %d\r\n", isrgb);
        printf("Pitch: %d (bytes)\r\n", pitch);
        printf("Framebuffer Size: %d (bytes)\r\n", fb_size);
        printf("#############################\r\n\n");
    } else{
        printf("No se pudo inicializar el framebuffer \r\n");
    }
}

void drawPixel(int x, int y, unsigned char attr)
{
    int offs = (y * pitch) + (x * 4);
    //escojo un color dentro del arreglo vgapal que es la paleta
    *((unsigned int *)(fb + offs)) = vgapal[attr & 0x0f];
}

void drawPixel2(int x, int y, unsigned long int c)
{
    int offs = (y * pitch) + (x * 4);
    //escojo un color dentro del arreglo vgapal que es la paleta
    *((unsigned int *)(fb + offs)) = c;
}


void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill)
{
    int y = y1;

    while (y <= y2)
    {
        int x = x1;
        while (x <= x2)
        {
            if ((x == x1 || x == x2) || (y == y1 || y == y2))
                drawPixel(x, y, attr);
            else if (fill)
                drawPixel(x, y, (attr & 0xf0) >> 4);
            x++;
        }
        y++;
    }
}

void drawLine(int x1, int y1, int x2, int y2, unsigned char attr)
{
    int dx, dy, p, x, y;

    dx = x2 - x1;
    dy = y2 - y1;
    x = x1;
    y = y1;
    p = 2 * dy - dx;

    while (x < x2)
    {
        if (p >= 0)
        {
            drawPixel(x, y, attr);
            y++;
            p = p + 2 * dy - 2 * dx;
        }
        else
        {
            drawPixel(x, y, attr);
            p = p + 2 * dy;
        }
        x++;
    }
}

void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        if (fill)
        {
            drawLine(x0 - y, y0 + x, x0 + y, y0 + x, (attr & 0xf0) >> 4);
            drawLine(x0 - x, y0 + y, x0 + x, y0 + y, (attr & 0xf0) >> 4);
            drawLine(x0 - x, y0 - y, x0 + x, y0 - y, (attr & 0xf0) >> 4);
            drawLine(x0 - y, y0 - x, x0 + y, y0 - x, (attr & 0xf0) >> 4);
        }
        drawPixel(x0 - y, y0 + x, attr);
        drawPixel(x0 + y, y0 + x, attr);
        drawPixel(x0 - x, y0 + y, attr);
        drawPixel(x0 + x, y0 + y, attr);
        drawPixel(x0 - x, y0 - y, attr);
        drawPixel(x0 + x, y0 - y, attr);
        drawPixel(x0 - y, y0 - x, attr);
        drawPixel(x0 + y, y0 - x, attr);

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }

        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

/* void drawChar(int x, int y, unsigned char ch, unsigned char attr, int zoom)
{
    //Si el char esta en el arreglo (fila) y multiplico por el tamaño en bytes de cada "glyph e.g matrix de 8x8 que pinta" de linea (FONT_BPG) para obtener el comienzo del elemnto que es
    unsigned char *glyph = (unsigned char *)&font7x8_basic + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i = 1; i <= (FONT_HEIGHT * zoom); i++)
    {
        for (int j = 0; j < (FONT_WIDTH * zoom); j++)
        {
            //pongo un 1 en cada columna para comprar con bitwise zoom veces
            unsigned char mask = 1 << (j/zoom);
            //si el gliph en esa posicion es 1 se pinta como foreground
            //si es 0 como background
            unsigned char col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

            drawPixel(x + j, y + i, col);
        }
        //al puntero le sumo los bytes por linea que tiene la fuente para acceder a la siguiente "fila" del glyph que corresponde a la siguiente posicion del arreglo font solo si ya ha pasado zoom veces
        glyph += (i % zoom) ? 0 : FONT_BPL;
    }
} */

void drawChar(int x, int y, unsigned char ch, unsigned char attr, int zoom)
{
    //Si el char esta en el arreglo (fila) y multiplico por el tamaño en bytes de cada "glyph e.g matrix de 8x8 que pinta" de linea (FONT_BPG) para obtener el comienzo del elemento que es
    unsigned short int *glyph = (unsigned short int *)&font9x16_basic + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

    for (int i = 1; i <= (FONT_WIDTH * zoom); i++)
    {
        for (int j = 0; j < (FONT_HEIGHT * zoom); j++)
        {
            //pongo un 1 en cada columna para comprar con bitwise zoom veces
            unsigned short int mask = 1 << ((FONT_HEIGHT - 1) - (j/zoom));
            //si el gliph en esa posicion es 1 se pinta como foreground
            //si es 0 como background
            unsigned short int col = (*glyph & mask) ? attr & 0x0f : (attr & 0xf0) >> 4;

            unsigned char newCol = col & 0xFF;

            drawPixel(x + i - 1, y + (FONT_HEIGHT * zoom) - j , newCol);
        }
        //al puntero le sumo los bytes por linea que tiene la fuente para acceder a la siguiente "fila" del glyph que corresponde a la siguiente posicion del arreglo font solo si ya ha pasado zoom veces
        glyph += (i % zoom) ? 0 : FONT_BPL;
    }
}


void drawString(int x, int y, char *s, unsigned char attr, int zoom)
{
    while (*s)
    {
        if (*s == '\r')
        {
            x = 0;
        }
        else if (*s == '\n')
        {
            x = 0;
            y += (17) * zoom;
        }
        else
        {
            drawChar(x, y, *s, attr, zoom);
            x += (FONT_WIDTH + 1) * zoom;
        }
        s++;
    }
}
