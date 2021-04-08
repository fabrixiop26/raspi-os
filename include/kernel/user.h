#ifndef	_USER_H
#define	_USER_H


//Funcion de prueba
void user_process1(char *array);
//Funcion para modo usuario desde kernel
void user_process();

extern unsigned long user_begin;
extern unsigned long user_end;

#endif  /*_USER_H */