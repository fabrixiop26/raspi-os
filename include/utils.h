#ifndef	_BOOT_H
#define	_BOOT_H

//Extern para que pueda ser accedido en Assembly
//TODO: esto se puede implementar en C asi que debo descubrir primero como
//uint32_t el problema es el delay con su ams volatile
extern void delay ( unsigned long);
extern void put32 ( unsigned long, unsigned int );
extern unsigned int get32 ( unsigned long );

//unsiged long es 32bits y unsigned int son 16

#endif  /*_BOOT_H */