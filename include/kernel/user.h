#ifndef	_USER_H
#define	_USER_H
//Funcion para modo usuario desde kernel
void user_process();

extern unsigned long user_begin;
extern unsigned long user_end;

#endif  /*_USER_H */