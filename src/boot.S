#include "mm.h"
#include "arm/sysregs.h"
#include "arm/mmu.h"
#include "peripherals/base.h"

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
	wfe	   //Las demas cpus esperan por sev que se ejecuta cuando cpu 0 limpia bss
    b      proc_hang     //for the rest of cpus inicializa su stack y salta a kernel_main

//clean the bss-section memezero recive bss_begin y (bss_end-bss_begin)
//por convencion la funcion en ARMv8 recibe 7 argumentos de los registros x0-x6
//LOW_MEMORY se define en mm.h y es 4MB
master:
    ldr	x0, =SCTLR_VALUE_MMU_DISABLED
	msr	sctlr_el1, x0		

	mrs x0, CurrentEL
    lsr x0, x0, #2
	cmp x0, #3
	beq el3

	ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0

	/* ldr	x0, =SCR_VALUE
	msr	scr_el2, x0 */

	/* mrs	x0, hcr_el2
	orr	x0, x0, #(1<<31) // AArch64
	msr	hcr_el2, x0 // SWIO hardwired on Pi3 */


	mov 	x0, #SPSR_VALUE
	msr	spsr_el2, x0

	adr	x0, el1_entry
	msr	elr_el2, x0

	eret

el3:

	/* ldr	x0, =HCR_VALUE
	msr	hcr_el2, x0 */

	ldr	x0, =SCR_VALUE
	msr	scr_el3, x0

	ldr	x0, =SPSR_VALUE
	msr	spsr_el3, x0		// 'spsr_el3' changed to 'spsr_el2 for .img'

	adr	x0, el1_entry		
	msr	elr_el3, x0			// 'elr_el3' changed to 'elr_el2 for .img'

	eret

//Limpia la seccion bss
//Reserva memoria en el stack
el1_entry:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero

	bl	__create_idmap
	adrp	x0, idmap_dir
	msr	ttbr0_el1, x0

	bl 	__create_page_tables
	
	mov	x0, #VA_START			
	add	sp, x0, #LOW_MEMORY

	adrp	x0, pg_dir				
	msr	ttbr1_el1, x0

	ldr	x0, =(TCR_VALUE)		
	msr	tcr_el1, x0

	ldr	x0, =(MAIR_VALUE)
	msr	mair_el1, x0


	ldr	x2, =kernel_main

	mov	x0, #SCTLR_MMU_ENABLED				
	msr	sctlr_el1, x0

	br 	x2

	.macro	create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGD_SHIFT, \tmp1, \tmp2
	create_table_entry \tbl, \virt, PUD_SHIFT, \tmp1, \tmp2
	.endm

	.macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1			// table index
	add	\tmp2, \tbl, #PAGE_SIZE
	orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE	
	str	\tmp2, [\tbl, \tmp1, lsl #3]
	add	\tbl, \tbl, #PAGE_SIZE					// next level table page
	.endm

	.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	lsr	\start, \start, #SECTION_SHIFT
	and	\start, \start, #PTRS_PER_TABLE - 1			// table index
	lsr	\end, \end, #SECTION_SHIFT
	and	\end, \end, #PTRS_PER_TABLE - 1				// table end index
	lsr	\phys, \phys, #SECTION_SHIFT
	mov	\tmp1, #\flags
	orr	\phys, \tmp1, \phys, lsl #SECTION_SHIFT			// table entry
9999:	str	\phys, [\tbl, \start, lsl #3]				// store the entry
	add	\start, \start, #1					// next entry
	add	\phys, \phys, #SECTION_SIZE				// next block
	cmp	\start, \end
	b.ls	9999b
	.endm

//el kernel se carga en address 0 asi que defino el stack pointer a 4MB
//para que el stack no sobreescriba el kernel image
/* init_stack:
	mrs	x0, mpidr_el1
	and	x0, x0,#0xFF		// get processor id again
	mov x1, x0				// store processor id in 'x1' to preserve 'x0' when call kernel_main
	mov x3, #2 //multiplicador para low memory
	mov x4, #LOW_MEMORY  //Cargo low memory 4mb base
	mul x4, x4, x3 //multiplico por 2 para que sea una base de 8mb el primer stack
	mov x2, #PAGE_SIZE		// define the stack offset in 4Kb (0x1000) 2^12
	mul x1, x1,x2			// multiply the stack offset by processor id
	add x1, x1, x4	// add to base address (#LOW_MEMORY * 8 32mb)
	mov	sp, x1
	bl	kernel_main			// call 'kernel_main' passing processor id through 'x0' */

__create_idmap:
	mov	x29, x30
	
	adrp	x0, idmap_dir
	mov	x1, #PG_DIR_SIZE
	bl	memzero

	adrp	x0, idmap_dir
	mov	x1, xzr
	create_pgd_entry	x0, x1, x2, x3

	mov	x1, xzr
	mov	x2, xzr
	ldr	x3, =(PHYS_MEMORY_SIZE)
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	mov	x30, x29
	ret

__create_page_tables:
	mov	x29, x30						// save return address

	adrp	x0, pg_dir
	mov	x1, #PG_DIR_SIZE
	bl 	memzero

	adrp	x0, pg_dir
	mov	x1, #VA_START 
	create_pgd_entry x0, x1, x2, x3

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0
	mov 	x2, #VA_START						// first virtual address
	ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + DEVICE_BASE)				// first virtual address
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov	x30, x29						// restore return address
	ret

proc_hang:
	wfe
    b proc_hang

//Para entender uso de registros
//https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers