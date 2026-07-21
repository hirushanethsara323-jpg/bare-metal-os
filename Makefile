# =============================================================================
# Nothing OS - Enterprise Makefile (v2.1 Major Suite Edition)
# =============================================================================
# Build system for Nothing OS (x86 Bare Metal Operating System)
# =============================================================================

# Compiler and tools
CC      = gcc
AS      = nasm
LD      = ld
OBJCOPY = objcopy
GRUB_MKRESCUE = grub-mkrescue
XORRISO = xorriso
QEMU    = qemu-system-x86_64

# Directories
KERNEL_DIR = kernel
BOOT_DIR   = boot
ISO_DIR    = iso
BUILD_DIR  = build

# Kernel source files
KERNEL_SRC   = $(KERNEL_DIR)/main.c
KEYBOARD_SRC = $(KERNEL_DIR)/drivers/keyboard.c
SERIAL_SRC   = $(KERNEL_DIR)/drivers/serial.c
RTC_SRC      = $(KERNEL_DIR)/drivers/rtc.c
GRAPHICS_SRC = $(KERNEL_DIR)/drivers/vga_graphics.c
VGA13_SRC    = $(KERNEL_DIR)/drivers/vga_mode13.c
SOUND_SRC    = $(KERNEL_DIR)/drivers/sound.c
ANSI_SRC     = $(KERNEL_DIR)/drivers/ansi.c
PCI_SRC      = $(KERNEL_DIR)/drivers/pci.c
E1000_SRC    = $(KERNEL_DIR)/drivers/e1000.c
VBE_SRC      = $(KERNEL_DIR)/drivers/vbe.c
AHCI_SRC     = $(KERNEL_DIR)/drivers/ahci.c
ACPI_SRC     = $(KERNEL_DIR)/drivers/acpi.c
ATA_SRC      = $(KERNEL_DIR)/drivers/ata.c
MOUSE_SRC    = $(KERNEL_DIR)/drivers/mouse.c
HEAP_SRC     = $(KERNEL_DIR)/mm/heap.c
PAGING_SRC   = $(KERNEL_DIR)/mm/paging.c
VFS_SRC      = $(KERNEL_DIR)/fs/vfs.c
FAT_SRC      = $(KERNEL_DIR)/fs/fat.c
IDT_SRC      = $(KERNEL_DIR)/arch/x86/idt.c
APIC_SRC     = $(KERNEL_DIR)/arch/x86/apic.c
SYSCALL_SRC  = $(KERNEL_DIR)/arch/x86/syscall.c
TSS_SRC      = $(KERNEL_DIR)/arch/x86/tss.c
NET_SRC      = $(KERNEL_DIR)/net/net.c
SIGNAL_SRC   = $(KERNEL_DIR)/sys/signal.c
ENV_SRC      = $(KERNEL_DIR)/sys/env.c
MONITOR_SRC  = $(KERNEL_DIR)/sys/monitor.c
IPC_SRC      = $(KERNEL_DIR)/sys/ipc.c
CRYPTO_SRC   = $(KERNEL_DIR)/crypto/sha256.c
SCHED_SRC    = $(KERNEL_DIR)/proc/scheduler.c
ELF_SRC      = $(KERNEL_DIR)/proc/elf.c
KTEST_SRC    = $(KERNEL_DIR)/tests/ktest.c
BOOT_SRC     = $(BOOT_DIR)/multiboot_header.asm

# Output
KERNEL     = $(BUILD_DIR)/kernel.bin
ISO_FILE   = $(ISO_DIR)/baremetal-os.iso

# Compiler flags
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -nostdinc
CFLAGS += -I$(KERNEL_DIR)/include
ASFLAGS = -f elf32
LDFLAGS = -T $(KERNEL_DIR)/linker.ld -m elf_i386 -nostdlib

# Objects
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/boot.o $(BUILD_DIR)/keyboard.o \
       $(BUILD_DIR)/heap.o $(BUILD_DIR)/idt.o $(BUILD_DIR)/serial.o \
       $(BUILD_DIR)/rtc.o $(BUILD_DIR)/vfs.o $(BUILD_DIR)/fat.o \
       $(BUILD_DIR)/vga_graphics.o $(BUILD_DIR)/vga_mode13.o \
       $(BUILD_DIR)/sound.o $(BUILD_DIR)/ansi.o $(BUILD_DIR)/pci.o \
       $(BUILD_DIR)/e1000.o $(BUILD_DIR)/vbe.o $(BUILD_DIR)/ahci.o \
       $(BUILD_DIR)/acpi.o $(BUILD_DIR)/apic.o $(BUILD_DIR)/syscall.o \
       $(BUILD_DIR)/tss.o $(BUILD_DIR)/net.o $(BUILD_DIR)/signal.o \
       $(BUILD_DIR)/env.o $(BUILD_DIR)/monitor.o $(BUILD_DIR)/ipc.o \
       $(BUILD_DIR)/sha256.o $(BUILD_DIR)/scheduler.o \
       $(BUILD_DIR)/elf.o $(BUILD_DIR)/paging.o $(BUILD_DIR)/ata.o \
       $(BUILD_DIR)/mouse.o $(BUILD_DIR)/ktest.o

# =============================================================================
# Targets
# =============================================================================

.PHONY: all clean run iso

all: dirs $(KERNEL)

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR) $(ISO_DIR)/boot/grub

# Link kernel binary
$(KERNEL): $(OBJS)
	@echo "Linking Nothing OS Enterprise v2.1 kernel binary..."
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Kernel built successfully: $@"
	@echo "Kernel size: $$(stat -c%s $@) bytes"

# Build boot.o
$(BUILD_DIR)/boot.o: $(KERNEL_DIR)/arch/x86/boot.c
	@echo "Building C boot code..."
	@$(CC) $(CFLAGS) -fno-pie -c -o $@ $<

# Compile IDT and Interrupt Manager
$(BUILD_DIR)/idt.o: $(IDT_SRC)
	@echo "Compiling IDT & PIC Interrupt Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Local APIC Multi-Core Controller
$(BUILD_DIR)/apic.o: $(APIC_SRC)
	@echo "Compiling Local APIC Multiprocessor Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile PCI Bus Driver
$(BUILD_DIR)/pci.o: $(PCI_SRC)
	@echo "Compiling PCI Bus Enumerator & Hardware Scanner Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile AHCI SATA Storage Driver
$(BUILD_DIR)/ahci.o: $(AHCI_SRC)
	@echo "Compiling AHCI Serial ATA (SATA) Host Controller Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile ACPI Power Management Engine
$(BUILD_DIR)/acpi.o: $(ACPI_SRC)
	@echo "Compiling ACPI Power Management & RSDP Scanner Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Intel e1000 Gigabit NIC Driver
$(BUILD_DIR)/e1000.o: $(E1000_SRC)
	@echo "Compiling Intel 82540EM (e1000) Gigabit NIC Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile VESA VBE 32-bit Framebuffer
$(BUILD_DIR)/vbe.o: $(VBE_SRC)
	@echo "Compiling VESA VBE 1024x768 32-bit ARGB TrueColor Framebuffer..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile System Call Dispatcher Engine
$(BUILD_DIR)/syscall.o: $(SYSCALL_SRC)
	@echo "Compiling System Call Engine (INT 0x80)..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile TSS & User Mode Engine
$(BUILD_DIR)/tss.o: $(TSS_SRC)
	@echo "Compiling Task State Segment & Ring 3 User Mode Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Network Stack Engine
$(BUILD_DIR)/net.o: $(NET_SRC)
	@echo "Compiling Ethernet / ARP / IPv4 Network Stack Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Process Signals Engine
$(BUILD_DIR)/signal.o: $(SIGNAL_SRC)
	@echo "Compiling Process Signal & Kernel Panic Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile System Environment Config Store
$(BUILD_DIR)/env.o: $(ENV_SRC)
	@echo "Compiling Global Environment Config Store..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Real-Time Performance Monitor
$(BUILD_DIR)/monitor.o: $(MONITOR_SRC)
	@echo "Compiling Real-Time Kernel Performance Telemetry Monitor..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile IPC & Semaphore Manager
$(BUILD_DIR)/ipc.o: $(IPC_SRC)
	@echo "Compiling IPC Ring Buffer Pipe & Semaphore Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile SHA-256 Cryptography Engine
$(BUILD_DIR)/sha256.o: $(CRYPTO_SRC)
	@echo "Compiling SHA-256 Cryptographic Hash Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile ANSI Escape Sequence Parser
$(BUILD_DIR)/ansi.o: $(ANSI_SRC)
	@echo "Compiling ANSI Escape Sequence SGR Translator..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile PC Speaker Audio Driver
$(BUILD_DIR)/sound.o: $(SOUND_SRC)
	@echo "Compiling PC Speaker Audio Driver & Synthesizer..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile ELF32 Program Loader
$(BUILD_DIR)/elf.o: $(ELF_SRC)
	@echo "Compiling ELF32 Executable Binary Program Loader..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Process Scheduler Engine
$(BUILD_DIR)/scheduler.o: $(SCHED_SRC)
	@echo "Compiling Process Scheduler Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Virtual Memory Paging Engine
$(BUILD_DIR)/paging.o: $(PAGING_SRC)
	@echo "Compiling x86 Virtual Memory Paging Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Primary ATA IDE Disk Driver
$(BUILD_DIR)/ata.o: $(ATA_SRC)
	@echo "Compiling Primary ATA IDE Sector Controller Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile PS/2 Mouse Driver
$(BUILD_DIR)/mouse.o: $(MOUSE_SRC)
	@echo "Compiling PS/2 Auxiliary Mouse Controller Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Automated QA & Test Suite
$(BUILD_DIR)/ktest.o: $(KTEST_SRC)
	@echo "Compiling Automated QA & Kernel Test Suite..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile PS/2 Keyboard driver
$(BUILD_DIR)/keyboard.o: $(KEYBOARD_SRC)
	@echo "Compiling PS/2 Keyboard Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Serial UART driver
$(BUILD_DIR)/serial.o: $(SERIAL_SRC)
	@echo "Compiling Serial UART 16550 Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile CMOS RTC Driver
$(BUILD_DIR)/rtc.o: $(RTC_SRC)
	@echo "Compiling CMOS RTC Real-Time Clock Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile VGA UI/UX Graphics Engine
$(BUILD_DIR)/vga_graphics.o: $(GRAPHICS_SRC)
	@echo "Compiling VGA Graphics & Theme Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Mode 13h 256-Color Pixel Graphics Engine
$(BUILD_DIR)/vga_mode13.o: $(VGA13_SRC)
	@echo "Compiling VGA Mode 13h 320x200 256-Color Pixel GUI Engine..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Dynamic Heap Manager
$(BUILD_DIR)/heap.o: $(HEAP_SRC)
	@echo "Compiling Dynamic Kernel Heap Manager..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Virtual File System
$(BUILD_DIR)/vfs.o: $(VFS_SRC)
	@echo "Compiling Virtual File System (MemFS)..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile FAT Boot Sector Engine
$(BUILD_DIR)/fat.o: $(FAT_SRC)
	@echo "Compiling FAT Filesystem & MBR Boot Parser..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile kernel main
$(BUILD_DIR)/main.o: $(KERNEL_SRC)
	@echo "Compiling Nothing OS Kernel Main..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Build ISO image
iso: $(KERNEL)
	@echo "Building ISO..."
	@cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	@$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR) 2>/dev/null || \
	( echo "GRUB mkrescue not available, creating basic ISO..." && \
	  $(XORRISO) -as mkisofs -b boot/grub/stage1 -c boot/grub/boot.cat \
	  -no-emul-boot -boot-load-size 4 -boot-info-table \
	  -o $(ISO_FILE) $(ISO_DIR) )
	@echo "ISO built: $(ISO_FILE)"

# Run in QEMU
run: all
	@echo "Starting QEMU..."
	@$(QEMU) -kernel $(KERNEL) -m 128M -display vga -serial mon:stdio 2>&1

# Clean build
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(ISO_DIR)
	@echo "Clean complete."

help:
	@echo "Nothing OS Build System"
	@echo "Targets: make all, make clean, make iso, make run"
