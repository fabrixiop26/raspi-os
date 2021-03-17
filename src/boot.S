#include "mm.h"

//Todo este codigo ira a la seccion .text.boot como definido en el linker 
// paraque se coloque al inicio del codigo en el .elf
.section ".text.boot"

.globl _start
//carga el valor de mpidr_el1 a x0 extra los ultimos 8 bits que es el id del 
//proceso y de ser 0 (cbz) se va a master sino entonces queda en ciclo
//see https://developer.arm.com/documentation/ddi0500/j/System-Control/AArch64-register-descriptions/Multiprocessor-Affinity-Register
_start:
    mrs    x0, mpidr_el1        
    and    x0, x0,#0xFF        // Check processor id
    cbz    x0, master        // Hang for all non-primary CPU
    b    init_stack     //for the rest of cpus

//clean the bss-section memezero recive bss_begin y (bss_end-bss_begin)
//por convencion la funcion en ARMv8 recibe 7 argumentos de los registros x0-x6
//LOW_MEMORY se define en mm.h y es 4MB
master:
    adr    x0, bss_begin
    adr    x1, bss_end
    sub    x1, x1, x0
    bl     memzero

//el kernel se carga en address 0 asi que defino el stack pointer a 4MB
//para que el stack no sobreescriba el kernel image
init_stack:
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF		// get processor id again
	mov x1, x0				// store processor id in 'x1' to preserve 'x0' when call kernel_main
	mov x2, #0x1000			// define the stack offset in 4Kb (0x1000)
	mul x1, x1,x2			// multiply the stack offset by processor id
	add x1, x1,#LOW_MEMORY	// add to base address (#LOW_MEMORY)
	mov	sp, x1
	bl	kernel_main			// call 'kernel_main' passing processor id through 'x0'

proc_hang:
	wfe
    b proc_hang

//Para entender uso de registros
//https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers