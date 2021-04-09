#include "kernel/user_sys.h"
#include "kernel/user.h"
#include "kernel/scheduler.h"

//Funcion a llamar
void loop(char* str)
{
	char buf[2] = {""};
	int cont = 2;
	while (cont >= 0){
		for (int i = 0; i < 5; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
		cont--;
	}
}

void user_process() 
{
	call_sys_write("User process\n\r");
	int pid = call_sys_fork();
	if (pid < 0) {
		call_sys_write("Error during fork\n\r");
		call_sys_exit();
		return;
	}
	if (pid == 0){
		call_sys_write("Hijo \r\n");
	} else {
		call_sys_write("Padre \r\n");
		loop("1234");
		//call_sys_exit();
	}
	call_sys_exit();
}
