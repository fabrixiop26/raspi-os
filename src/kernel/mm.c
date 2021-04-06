#include "mm.h"

static unsigned short mem_map [ PAGING_PAGES ] = {0,};

int current_pages = 0;

unsigned long get_free_page()
{
	for (int i = 0; i < PAGING_PAGES; i++){
		if (mem_map[i] == 0){
			mem_map[i] = 1;
			current_pages += 1;
			return BASE_MEMORY + i*PAGE_SIZE;
		}
	}
	return 0;
}

void free_page(unsigned long p){
	mem_map[(p - BASE_MEMORY) / PAGE_SIZE] = 0;
	current_pages -= 1;
}