#include "utils.h"
#include "printf.h"
#include "drivers/timer.h"
#include "entry.h"
#include "peripherals/irq.h"
#include "peripherals/timer.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",		
	"FIQ_INVALID_EL1t",		
	"ERROR_INVALID_EL1T",		

	"SYNC_INVALID_EL1h",		
	"IRQ_INVALID_EL1h",		
	"FIQ_INVALID_EL1h",		
	"ERROR_INVALID_EL1h",		

	"SYNC_INVALID_EL0_64",		
	"IRQ_INVALID_EL0_64",		
	"FIQ_INVALID_EL0_64",		
	"ERROR_INVALID_EL0_64",	

	"SYNC_INVALID_EL0_32",		
	"IRQ_INVALID_EL0_32",		
	"FIQ_INVALID_EL0_32",		
	"ERROR_INVALID_EL0_32"	
};


/* void enable_interrupt_controller()
{
	//Habilitamos el interrupt para el system timer match 1
	put32(ENABLE_IRQS_1, SYSTEM_TIMER_IRQ_1);
} */

void enable_interrupt_controller()
{
	
	// este no es el controlador de interrupciones ya que toca depender de los registros locales de interrupciones de cada CPU, pero habilito el local timer
	unsigned int local_timer_ctrl = get32(TIMER_CTRL);
	put32(TIMER_CTRL, (local_timer_ctrl | (1 << 29)));
}

void show_invalid_entry_message(int type, unsigned long esr, unsigned long address)
{
	printf("%s, ESR: %x, address: %x\r\n", entry_error_messages[type], esr, address);
}

void handle_irq(void){
//Cada cpu tiene un registro para control de interrupciones locales
//el local timer es el bit 11
	unsigned int irq = get32(CORE0_INT_SOURCE);
	switch (irq) {
		case (LOCAL_TIMER_INT):
			handle_timer_irq();
			break;
		default:
			printf("Unknown pending irq: %x\r\n", irq);
	}
}

/* void handle_irq(void)
{
	//IRQ_PENDING_1 TIene el estado de las interrupciones 0 - 31
	unsigned int irqBit = get32(IRQ_PENDING_1);
	//Verificamos si la interupccion fue producida por el system timer
	//Para manejar muchas interrupciones seria bueno colocar esto en un loop
	//hasta que todo los bits sean limpiados
	switch (irqBit) {
		case (SYSTEM_TIMER_IRQ_1):
			handle_timer_irq();
			break;
		default:
			printf("Unknown pending irq: %x\r\n", irqBit);
	}
} */