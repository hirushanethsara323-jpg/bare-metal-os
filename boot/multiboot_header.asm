; =============================================================================
; BareMetal OS - GRUB Multiboot Header
; =============================================================================
; This version is loaded by GRUB in protected mode
; =============================================================================

section .multiboot
align 4

; Multiboot header - tells GRUB how to load us
multiboot_header:
    dd 0x1BADB002              ; Magic number
    dd 0x00010003              ; Flags (align + OS kernel type)
    dd -(0x1BADB002 + 0x00010003) ; Checksum
    
    dd multiboot_header        ; Header address
    dd 0x100000                 ; Load address (where kernel is loaded)
    dd 0                        ; Load end address (unused)
    dd 0                        ; BSS end address (unused)
    dd 0                        ; Entry point (we'll use _start)

section .text
global _start
extern _kernel_main

bits 32

_start:
    ; Set up stack
    mov esp, 0x90000
    
    ; Call kernel main
    call _kernel_main
    
    ; If kernel returns, halt
cli
hlt
1:  jmp 1b

; =============================================================================
; GDT - Global Descriptor Table (for flat memory model)
; =============================================================================
section .data

gdt_start:

; Null descriptor (required by CPU)
gdt_null:
    dq 0

; Code segment descriptor
gdt_code:
    dw 0xFFFF       ; Limit (4GB)
    dw 0x0000       ; Base (low)
    db 0x00         ; Base (middle)
    db 10011010b    ; Access: Present, Ring 0, Code, Readable
    db 11001111b    ; Flags: 4KB pages, 32-bit, Limit high nibble
    db 0x00         ; Base (high)

; Data segment descriptor
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b    ; Access: Present, Ring 0, Data, Writeable
    db 11001111b
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Limit
    dd gdt_start                ; Base

; =============================================================================
; IDT - Interrupt Descriptor Table (basic setup)
; =============================================================================
idt_start:
    times 256 dq 0              ; 256 entries, all zeros (null)
idt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start
