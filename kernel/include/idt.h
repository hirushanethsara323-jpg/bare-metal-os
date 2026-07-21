#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* IDT Entry Gate Attributes */
#define IDT_FLAG_PRESENT  0x80
#define IDT_FLAG_RING0    0x00
#define IDT_FLAG_RING3    0x60
#define IDT_FLAG_GATE_32  0x0E /* 32-bit Interrupt Gate */

#define IDT_ENTRIES       256

/* IDT Entry Structure */
struct idt_entry {
    uint16_t base_low;   /* Lower 16 bits of ISR address */
    uint16_t sel;        /* Kernel code segment selector (0x08) */
    uint8_t  always0;    /* Always set to 0 */
    uint8_t  flags;      /* Attributes & gate type */
    uint16_t base_high;  /* Upper 16 bits of ISR address */
} __attribute__((packed));

typedef struct idt_entry idt_entry_t;

/* IDT Pointer Register Structure */
struct idt_ptr {
    uint16_t limit;      /* Size of IDT in bytes - 1 */
    uint32_t base;       /* Base linear address of IDT */
} __attribute__((packed));

typedef struct idt_ptr idt_ptr_t;

/* Interrupt Registrations and Setup */
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

/* PIC & PIT Functions */
void pic_remap(void);
void pit_init(uint32_t frequency);
uint32_t timer_get_ticks(void);
uint32_t timer_get_uptime_sec(void);

#endif /* IDT_H */
