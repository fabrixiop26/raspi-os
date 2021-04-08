/*
.text.boot va primero ya que en esa seccion se pondra
el codigo inicial
.text contiene instrucciones del kernel
.rodata contiene read-only data
.data normal data
.bss data que debe ser incializada en 0
After loading the image into memory, we must initialize the .bss section to 0; that's why we need to record the start and end of the section (hence the bss_begin and bss_end symbols) and align the section so that it starts at an address that is a multiple of 8. If the section is not aligned, it would be more difficult to use the str instruction to store 0 at the beginning of the bss section because the str instruction can be used only with 8-byte-aligned addresses
*/

ENTRY(_start)

SECTIONS
{
	. = 0xffff000000080000;
	.text.boot : { *(.text.boot) }
	. = ALIGN(0x00001000);
	user_begin = .;
	.text.user : { build/user* (.text) }
	.rodata.user : { build/user* (.rodata) }
	.data.user : { build/user* (.data) }
	.bss.user : { build/user* (.bss) }
	user_end = .;
	.text :  { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) } 
	bss_end = .;
	. = ALIGN(0x00001000);
	idmap_dir = .;
	.data.idmapd : {. += (3* (1 << 12));}
	. = ALIGN(0x00001000);
	pg_dir = .;
	.data.pgd : { . += (3 * (1 << 12)); }
}