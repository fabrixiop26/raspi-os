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
	//esto guarda el usuario
	msr	ttbr0_el1, x0

	bl 	__create_page_tables
	
	//Toca tener esto en cuenta para los stacks si implemento 
	//multicore
	mov	x0, #VA_START			
	add	sp, x0, #LOW_MEMORY

	//pgd se guarda en ttbr1_el1
	adrp	x0, pg_dir				
	msr	ttbr1_el1, x0
	//Cada pagina es 4kb
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

	//Aloja una nueva entrada a la tabla ya sea pgd o pud
	//tbl: a pointer to a memory region were new table has to be allocated.
	//vrit virtual address that we are currently mapping.
	//shift that we need to apply to the virtual address in order to extract current table index. (39 in case of PGD and 30 in case of PUD)
	//tmp1, tmp2 registros temporarios
	.macro	create_table_entry, tbl, virt, shift, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #PTRS_PER_TABLE - 1			// table index
	mov \tmp2, #PAGE_SIZE
	lsl \tmp2, \tmp2, \tmp1  //Multiplico PAGE_SIZE * 2^(index)
	add	\tmp2, \tbl, \tmp2				//nueva tabla contigua a la direccion
	orr	\tmp2, \tmp2, #MM_TYPE_PAGE_TABLE	//al descriptor le ponemos 11 al final
	str	\tmp2, [\tbl, \tmp1, lsl #3] // al indice se multiplica por 8 como ya que asi accedemos a la posicion correcta como en un arreglo
	mov \tmp2, #PAGE_SIZE
	lsl \tmp2, \tmp2, \tmp1  //Multiplico PAGE_SIZE * 2^(index)
	add	\tbl, \tbl, \tmp2					// next level table page
	.endm

	//tbl - a pointer to the PMD table.
	//phys - the start of the physical region to be mapped	
	//start - virtual address of the first section to be mapped.
	//end - virtual address of the last section to be mapped.
	//flags - flags that need to be copied into lower attributes of the block descriptor.
	//tmp1 - temprary register
	.macro	create_block_map, tbl, phys, start, end, flags, tmp1
	//usamos 21 para la seccion aka 2MB y start y end son indices
	//de la PMD table
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

__create_idmap:
	mov	x29, x30 //Gaurdo return address
	
	adrp	x0, idmap_dir
	mov	x1, #PG_DIR_SIZE //5 paginas
	bl	memzero

	adrp	x0, idmap_dir
	mov	x1, xzr //0
	
					//tbl virt temp1 temp2
	create_pgd_entry	x0, x1, x2, x3
	//al final x0 apunta a pmd
	mov	x1, xzr //star physical region
	mov	x2, xzr //start va
	ldr	x3, =(PHYS_MEMORY_SIZE) //ultima seccion va
	//tbl, phys, start, end, flags, tmp1
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/*Creando nueva entrada en PGD y nueva tabla PDU y PMD */
	/* adrp	x0, idmap_dir
	add    x0, x0, #VA_LOCAL_OFFSET //ya hay 3 tablas al final
	mov    x1, xzr //muevo la va inicial que seria la misma fisica 0?
	orr    x1, x1, #(1 << 39) //le pongo 000000001 como indice de pgd
	mov    x4, #3 //indice inicial
	mov    x5, #PAGE_SIZE

	//tbl, virt, shift, tmp1, tmp2, exp, pg
	create_table_entry_2 x0, x1, PGD_SHIFT, x2, x3, x4, x5
	//x0 apunta a pdu, x1 le pongo en 1 ya para que ponga contiguo al pmd
	mov x4, #1
	create_table_entry_2 x0, x1, PUD_SHIFT, x2, x3, x4, x5
	//x0 apunta al nuevo pmd
	mov x1, #LOCAL_PERIPHERAL_BASE //fisica
	mov x2, #LOCAL_PERIPHERAL_BASE //va
	//orr x2, x2, #(1 << 39)
	ldr x3, =(LAST_LOCAL_PERIPHERAL)
	mov x4, #0

	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4 */

	mov	x30, x29
	ret

__create_page_tables:
	mov	x29, x30						// save return address

	//limpiar el area inicial de las paginas
	adrp	x0, pg_dir
	mov	x1, #PG_DIR_SIZE //3 tablas de 4kb (5 en teoria para lo que quiero)
	bl 	memzero

	adrp	x0, pg_dir
	mov	x1, #VA_START 
	//tbl, virt, tmp1, tmp2
	create_pgd_entry x0, x1, x2, x3
	//tbl (x0) termina apuntando a pmd (3 level table)

	/* Mapping kernel and init stack*/
	mov 	x1, xzr							// start mapping from physical offset 0
	mov 	x2, #VA_START						// first virtual address
	ldr	x3, =(VA_START + DEVICE_BASE - SECTION_SIZE)		// last virtual address
	create_block_map x0, x1, x2, x3, MMU_FLAGS, x4

	/* Mapping device memory*/
	mov 	x1, #DEVICE_BASE					// start mapping from device base address 
	ldr 	x2, =(VA_START + DEVICE_BASE)				// first virtual address
	ldr	x3, =(VA_START + PHYS_MEMORY_SIZE - SECTION_SIZE)	// last virtual address
				//tbl, phys, start, end, flags, tmp1
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	adrp x0, pg_dir
	mov  x1, #VA_START
	orr  x1, x1, #(1 << 30) //1 en PUD index
	mov x2, #0
	mov x3, #0
	//Esto deberia crear el nuevo indice en PUD y la nueva en pmd
	create_pgd_entry x0, x1, x2, x3
	//obtengo la nueva PMD en x0
	
	mov 	x1, #LOCAL_PERIPHERAL_BASE
	ldr     x2, =(VA_START + LOCAL_PERIPHERAL_BASE) //first va
	ldr    x3, =(VA_START + LAST_LOCAL_PERIPHERAL - SECTION_SIZE)    // last virtual address
	mov x4, #0
	create_block_map x0, x1, x2, x3, MMU_DEVICE_FLAGS, x4

	mov	x30, x29						// restore return address
	ret

proc_hang:
	wfe
    b proc_hang

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

//Para entender uso de registros
//https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers