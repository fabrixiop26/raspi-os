#include "peripherals/timer.h"
#include "utils.h"

unsigned long get_system_timer()
{
    unsigned int h = -1, l;
    // we must read MMIO area as two separate 32 bit reads
    h = get32(TIMER_CHI);
    l = get32(TIMER_CLO);
    // we have to repeat it if high word changed during read
    if (h != get32(TIMER_CHI))
    {
        h = get32(TIMER_CHI);
        l = get32(TIMER_CLO);
    }
    // compose long int value
    return ((unsigned long)h << 32) | l;
}

void wait_msec(unsigned int n)
{
    unsigned long t = get_system_timer();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if (t)
        while (get_system_timer() < t + n)
            ;
}

int strcmp(char *w1, char *w2)
{
    int i = 0;
    while (w1[i] == w2[i] && w1[i] != '\0')
        i++;
    if (w1[i] > w2[i])
        return 1;
    else if (w1[i] < w2[i])
        return -1;
    else
        return 0;
}

int starts_with(char *w1, char *w2, int len)
{
    int i = 0;
    while (w1[i] == w2[i] && w1[i] != '\0')
    {

        i++;
        if (i == len)
        {
            return 1;
        }
    }
    return 0;
}