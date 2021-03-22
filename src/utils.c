#include "peripherals/timer.h"
#include "utils.h"

unsigned long get_system_timer()
{
    unsigned int h=-1, l;
    // we must read MMIO area as two separate 32 bit reads
    h=get32(TIMER_CHI);
    l=get32(TIMER_CLO);
    // we have to repeat it if high word changed during read
    if(h!=get32(TIMER_CHI)) {
        h=get32(TIMER_CHI);
        l=get32(TIMER_CLO);
    }
    // compose long int value
    return ((unsigned long) h << 32) | l;
}

void wait_msec(unsigned int n)
{
    unsigned long t=get_system_timer();
    // we must check if it's non-zero, because qemu does not emulate
    // system timer, and returning constant zero would mean infinite loop
    if(t) while(get_system_timer() < t+n);
}