/**
 * Nothing OS - Interrupt Descriptor Table (IDT), PIC Remapper & PIT Timer
 * 
 * Sets up 256 IDT gate descriptors, remaps 8259 PIC vectors to prevent
 * CPU exception conflicts, and configures PIT 8253 for system clock ticks.
 */

#include "../../include/idt.h"
#include "../../include/io.h"

/* IDT Entries Table and Pointer */
static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t   idtp;

/* System Clock Ticks */
static volatile uint32_t timer_ticks = 0;
static uint32_t timer_freq_hz = 100;

/* Default ISR Handler Stub */
static void default_isr_handler(void) {
    /* Send EOI to PIC */
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}

/**
 * Set an IDT Gate Entry.
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (base & 0xFFFF);
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

/**
 * Remap 8259 Master & Slave PICs.
 * Master PIC: IRQ 0..7   -> Interrupt Vectors 32..39
 * Slave PIC:  IRQ 8..15  -> Interrupt Vectors 40..47
 */
void pic_remap(void) {
    /* ICW1: Initialize PIC */
    outb(0x20, 0x11); io_wait();
    outb(0xA0, 0x11); io_wait();

    /* ICW2: Vector Offset */
    outb(0x21, 0x20); io_wait(); /* Master PIC vector offset 32 (0x20) */
    outb(0xA1, 0x28); io_wait(); /* Slave PIC vector offset 40 (0x28) */

    /* ICW3: Cascading configuration */
    outb(0x21, 0x04); io_wait(); /* Master PIC has Slave on IRQ2 */
    outb(0xA1, 0x02); io_wait(); /* Slave PIC cascade identity */

    /* ICW4: Environment info (8086/88 mode) */
    outb(0x21, 0x01); io_wait();
    outb(0xA1, 0x01); io_wait();

    /* Unmask all IRQs initially */
    outb(0x21, 0x00);
    outb(0xA1, 0x00);
}

/**
 * Initialize IDT and load IDTR register.
 */
void idt_init(void) {
    idtp.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idtp.base  = (uint32_t)&idt;

    /* Fill default gates */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)default_isr_handler, 0x08, IDT_FLAG_PRESENT | IDT_FLAG_GATE_32);
    }

    /* Remap 8259 PIC to avoid interrupt vector conflicts */
    pic_remap();

    /* Load IDT Register */
    __asm__ volatile ("lidt %0" : : "m"(idtp));
}

/**
 * Timer IRQ Handler (called on PIT tick).
 */
void timer_handler(void) {
    timer_ticks++;
    /* Send End of Interrupt (EOI) signal to Master PIC */
    outb(0x20, 0x20);
}

/**
 * Initialize Programmable Interval Timer (PIT 8253/8254).
 */
void pit_init(uint32_t frequency) {
    timer_freq_hz = frequency;
    uint32_t divisor = 1193182 / frequency;

    /* Channel 0, Access Mode: lobyte/hibyte, Mode 3 (Square Wave) */
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

/**
 * Get total system timer ticks since boot.
 */
uint32_t timer_get_ticks(void) {
    return timer_ticks;
}

/**
 * Get system uptime in seconds.
 */
uint32_t timer_get_uptime_sec(void) {
    return timer_ticks / timer_freq_hz;
}
