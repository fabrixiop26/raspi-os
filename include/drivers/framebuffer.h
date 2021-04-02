#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H
/**
 * \file framebuffer.h
 * Interfaz para el manejo de la pantalla.
 * @see <a href="https://github.com/isometimes/rpi4-osdev/blob/master/part5-framebuffer/fb.h">Adaptado de</a>
*/

/**
 * Inicializa el framebuffer del VideoCore IV.
*/
void fb_init();
/**
 * Dibuja un pixel en la pantalla
 * @param x la posicion en X.
 * @param y la posicion en Y.
 * @param attr 8 bits de configuracion de color, ultimos 4 bits para el color, primeros 4 bits ignorados
*/
void drawPixel(int x, int y, unsigned char attr);
/**
 * Dibuja un caracter en pantalla.
 * @param x la posicion en X.
 * @param y la posicion en Y.
 * @param ch el caracter a dibujar
 * @param attr 8 bits de configuracion de color. Los ultimos 4 bits el color de la fuente, los primeros 4 bits el color del fondo.
 * @param zoom el tamaño con el cual dibujar la fuente.
*/
void drawChar(int x, int y, unsigned char ch, unsigned char attr, int zoom);
/**
 * Dibuja un secuencia de caracteres en pantalla.
 * @param x la posicion en X.
 * @param y la posicion en Y.
 * @param s el string a dibujar
 * @param attr 8 bits de configuracion de color. Los ultimos 4 bits el color de la fuente, los primeros 4 bits el color del fondo.
 * @param zoom el tamaño con el cual dibujar la fuente.
*/
void drawString(int x, int y, char *s, unsigned char attr, int zoom);
/**
 * Dibuja un rectangulo en pantalla.
 * @param x1 la posicion en X de la esquina superior izquierda.
 * @param y1 la posicion en Y de la esquina superior izquierda.
 * @param x2 la posicion en X de la esquina inferior derecha.
 * @param y2 la posicion en Y de la esquina inferior derecha.
 * @param attr 8 bits de configuracion de color. Los ultimos 4 bits el color del outline, los primeros 4 bits el color del relleno.
 * @param fill Si debe rellenarse o no el rectangulo. 0 para no 1 para si.
*/
void drawRect(int x1, int y1, int x2, int y2, unsigned char attr, int fill);
/**
 * Dibuja un circulo en la pantalla.
 * @param x0 la posicion en X del centro.
 * @param y0 la posicion en Y del centro.
 * @param radius el radio del circulo.
 * @param attr 8 bits de configuracion de color. Los ultimos 4 bits el color del outline, los primeros 4 bits el color del relleno.
 * @param fill Si debe rellenarse o no el rectangulo. 0 para no 1 para si.
*/
void drawCircle(int x0, int y0, int radius, unsigned char attr, int fill);
/**
 * Dibuja una linea en la pantalla.
 * @param x1 la posicion en X del comienzo de la linea.
 * @param y1 la posicion en X del comienzo de la linea.
 * @param x2 la posicion en X del fin de la linea.
 * @param y2 la posicion en X del fin de la linea.
 @param attr 8 bits de configuracion de color. Los ultimos 4 bits el color del outline, los primeros 4 bits el color del relleno.
*/
void drawLine(int x1, int y1, int x2, int y2, unsigned char attr);

#endif