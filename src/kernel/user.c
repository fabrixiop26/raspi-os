#include "kernel/user_sys.h"
#include "kernel/user.h"
#include "kernel/scheduler.h"
#include "printf.h"
#include "kernel/cmd.h"
#include "utils.h"
#include "peripherals/timer.h"

/* char w1[5] = {0};
char w2[5] = {0}; */
char *w1 = "";
char *w2 = "";

void setWord1(char *w)
{
	w1 = w;
	/* int i = 0;
	while (*w)
	{
		w1[i] = *w;
		w++;
	} */
}
void setWord2(char *w)
{
	w2 = w;
	/* int i = 0;
	while (*w)
	{
		w2[i] = *w;
		w++;
	} */
}

//Funcion a llamar
void show(char *str)
{
	int cont = 10;
	while (cont >= 0)
	{

		call_sys_write(str);
		call_sys_write("\n");
		user_delay(6000000);
		cont--;
	}
}

void user_process()
{
	//Espero que este el fork
	call_sys_write("User process \n\r");
	int pid = call_sys_fork(2);
	if (pid < 0)
	{
		call_sys_write("Error during fork\n\r");
		call_sys_exit();
		return;
	}
	if (pid == 0)
	{
		show(w1);
	}
	else
	{
		show(w2);
		//call_sys_exit();
	}
	call_sys_write("\n");
	call_sys_exit();
}
