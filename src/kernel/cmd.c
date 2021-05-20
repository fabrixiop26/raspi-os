#include "kernel/cmd.h"
#include "drivers/uart.h"
#include "utils.h"
#include "printf.h"

char dig1[30] = {0};
char dig2[30] = {0};

//Aqui obtengo el segundo y demas parametros
void parse_sched_command(char *cmd)
{
    int i1 = 0;
    int i2 = 0;
    unsigned short firstArgument = 0;

    while (*cmd)
    {
        if (*cmd == ' ')
        {
            firstArgument = 1;
            cmd++;
            continue;
        }
        if (firstArgument)
        {
            dig2[i2] = *cmd;
            i2++;
        }
        else
        {
            dig1[i1] = *cmd;
            i1++;
        }
        cmd++;
    }
    setWord1(dig1);
    setWord2(dig2);
    changeState();
}

void parse_first_command(char *cmdbuff)
{
    if (strcmp(cmdbuff, "init") == 0)
    {
        init_console();
    }

     if (strcmp(cmdbuff, "tasks") == 0)
    {
        show_tasks();
    }

     if(strcmp(cmdbuff, "about") == 0){
        //Se muestra sobre nosotros
        showWindow();
    }

    if (starts_with(cmdbuff, "start",5) == 1)
    {
        //Lo muevo con 6 para sacar el siguiente param
       parse_sched_command(cmdbuff += 6);
    }
}