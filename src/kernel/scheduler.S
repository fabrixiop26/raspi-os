#include "kernel/scheduler.h"

.globl cpu_switch_to
cpu_switch_to:
	mov	x10, #THREAD_CPU_CONTEXT
	add	x8, x0, x10 //pasamos a x8 el cpu_context de p->current
	mov	x9, sp
	stp	x19, x20, [x8], #16		// store callee-saved registers mismo orden del task_struct
	stp	x21, x22, [x8], #16
	stp	x23, x24, [x8], #16
	stp	x25, x26, [x8], #16
	stp	x27, x28, [x8], #16
	stp	x29, x9, [x8], #16 //frame pointer (x29)
	str	x30, [x8]		//link register (x30) contiene function return address se guarda como pc (se guarda en la direccion que tenga x8)
	add	x8, x1, x10 //x1 contiene sigueinte proceso y se guarda ahora en x8
	ldp	x19, x20, [x8], #16		// restore callee-saved registers
	ldp	x21, x22, [x8], #16
	ldp	x23, x24, [x8], #16
	ldp	x25, x26, [x8], #16
	ldp	x27, x28, [x8], #16
	ldp	x29, x9, [x8], #16
	ldr	x30, [x8]
	mov	sp, x9
	ret