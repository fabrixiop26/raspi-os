#ifndef	_BOOT_H
#define	_BOOT_H

/**
* \file utils.h
* \brief Conjunto de funciones de utilidad
* 
*/
//Extern para que pueda ser accedido en Assembly
//TODO: esto se puede implementar en C asi que debo descubrir primero como
//uint32_t el problema es el delay con su ams volatile

/**
*Wait for an amount of cpu cycles.
*@param delay Cantidad de ciclos a esperar.
*/
extern void delay ( unsigned long delay);
/**
*Wait for an amount of cpu cycles.
*@param reg Direccion de memoria.
*@param data El valor a guardar
*/
extern void put32 ( unsigned long reg, unsigned int data);
/**
*Wait for an amount of cpu cycles.
*@param reg Direccion de memoria.
*@return El valor del registro
*/
extern unsigned int get32 ( unsigned long reg);

#endif  /*_BOOT_H */