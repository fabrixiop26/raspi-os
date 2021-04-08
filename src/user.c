#include "kernel/user_sys.h"
#include "kernel/user.h"
#include "printf.h"

//Funcion a llamar
void loop(char* str)
{
	char buf[2] = {""};
	while (1){
		for (int i = 0; i < 5; i++){
			buf[0] = str[i];
			call_sys_write(buf);
			user_delay(1000000);
		}
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
        //Hijo
		loop("abcde");
	} else {
        //Padre
		loop("12345");
	}
}