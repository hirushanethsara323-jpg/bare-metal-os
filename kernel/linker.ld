/*
 * BareMetal OS - Linker Script
 * 
 * This script defines the memory layout for the kernel.
 * It creates a flat binary suitable for GRUB or direct booting.
 */

/* Entry point */
ENTRY(_start)

/* Memory layout */
SECTIONS
{
    /* The kernel is loaded at 0x100000 (1MB) */
    . = 0x100000;
    
    /* Multiboot header - must be in first 8KB */
    .multiboot : {
        *(.multiboot)
    }
    
    /* Boot code */
    .text : {
        *(.text._start)
        *(.text._kernel_entry)
        *(.text)
    }
    
    /* Text (code) section - executable code */
    .text : {
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Read-only data */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Initialized data */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* Uninitialized data */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* End marker */
    _end = .;
    
    /* Memory info (for debugging) */
    .kernel_end = .;
    .kernel_size = SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.bss);
}
