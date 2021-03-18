.globl irq_vector_init
irq_vector_init:
	adr	x0, vectors		// load VBAR_EL1 with virtual
	msr	vbar_el1, x0		// vector table address
	ret

//DAIF: son los 4 bits para "enmascarar" los diferentes tipos de interrupcciones
//D: Masks debug exceptions
//A: Masks SErrors. It is called A because SErrors sometimes are called asynchronous aborts.
//I: Masks IRQs
//F: Masks FIQs
//daifclr: registro para limpiar, daifset: registro para colocar
# el immed #2 se usa ya que solo estamos manejando IRQs
.globl enable_irq
enable_irq:
	msr daifclr, #2 
	ret

.globl disable_irq
disable_irq:
	msr	daifset, #2
	ret