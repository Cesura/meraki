#include "monitor.h"
#include "descriptor_tables.h"
#include "timer.h"

int main(struct multiboot *mboot_ptr)
{
    	// Initialize ISRs
    	init_descriptor_tables();
    	
    	// Clear screen
	kclear();
    
	kprint("Kernel start\n");

    	asm volatile("int $0x3");
    	asm volatile("int $0x4");

   	asm volatile("sti");
    	init_timer(50);

    	return 0;
}
