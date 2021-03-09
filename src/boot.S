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
    b    proc_hang

proc_hang: 
    b proc_hang

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
    mov    sp, #LOW_MEMORY //iniciar el stack pointer
    bl    kernel_main

//Para entender uso de registros
//https://developer.arm.com/documentation/den0024/a/The-ABI-for-ARM-64-bit-Architecture/Register-use-in-the-AArch64-Procedure-Call-Standard/Parameters-in-general-purpose-registers