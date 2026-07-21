; =============================================================================
; BareMetal OS - Bootloader
; =============================================================================
; This is a simple boot sector that loads the kernel
; =============================================================================

section .boot
bits 16

global _start
_start:
    ; BIOS boot sector signature
    jmp short main
    nop

    ; OEM label
    OEMLabel:      db "BAREOS  "
    BytesPerSector: dw 512
    SectorsPerCluster: db 1
    ReservedForBoot: dw 1
    NumberOfFats:   db 2
    MaxRootEntries: dw 224
    TotalSectors:   dw 2880
    MediaType:      db 0xF0
    SectorsPerFat:  dw 9
    SectorsPerTrack: dw 18
    HeadsPerCylinder: dw 2
    HiddenSectors:  dd 0
    TotalSectorsBig: dd 0
    DriveNumber:    db 0
    Flags:          db 0
    Signature:      db 0x29
    VolumeID:       dd 0x12345678
    VolumeLabel:    db "BAREOS     "
    FileSystem:     db "FAT12   "

main:
    ; Set up segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Load kernel from disk
    mov ah, 0x02        ; Read sectors
    mov al, 10          ; Number of sectors to read
    mov ch, 0           ; Cylinder
    mov cl, 2           ; Start sector (after boot sector)
    mov dh, 0           ; Head
    mov dl, 0x80        ; First hard disk
    mov bx, 0x1000      ; Load to 0x1000:0x0000
    int 0x13
    jc disk_error

    ; Switch to protected mode
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    jmp 0x08:protected_mode

disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $

print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp print_string
done:
    ret

disk_error_msg: db 'Disk read error!', 0

; =============================================================================
; GDT - Global Descriptor Table
; =============================================================================
gdt_start:
    ; Null descriptor
    gdt_null:
        dq 0
    
    ; Code segment
    gdt_code:
        dw 0xFFFF       ; Limit
        dw 0x0000       ; Base (low)
        db 0x00         ; Base (middle)
        db 10011010b    ; Access
        db 11001111b    ; Flags + Limit (high)
        db 0x00         ; Base (high)
    
    ; Data segment
    gdt_data:
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 10010010b
        db 11001111b
        db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

bits 32
protected_mode:
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    mov esp, 0x90000    ; Stack pointer
    
    ; Jump to kernel
    jmp 0x08:0x10000

times 510-($-$$) db 0
dw 0xAA55
