# =============================================================================
# Nothing OS - Makefile
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

# Kernel files
KERNEL_SRC   = $(KERNEL_DIR)/main.c
KEYBOARD_SRC = $(KERNEL_DIR)/drivers/keyboard.c
HEAP_SRC     = $(KERNEL_DIR)/mm/heap.c
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
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/boot.o $(BUILD_DIR)/keyboard.o $(BUILD_DIR)/heap.o

# =============================================================================
# Targets
# =============================================================================

.PHONY: all clean run iso

all: dirs $(KERNEL)

# Create build directories
dirs:
	@mkdir -p $(BUILD_DIR) $(ISO_DIR)/boot/grub

# Build kernel
$(KERNEL): $(OBJS)
	@echo "Linking Nothing OS kernel binary..."
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Kernel built successfully: $@"
	@echo "Kernel size: $$(stat -c%s $@) bytes"

# Build boot.o (C-based boot with inline assembly)
$(BUILD_DIR)/boot.o: $(KERNEL_DIR)/arch/x86/boot.c
	@echo "Building C boot code..."
	@$(CC) $(CFLAGS) -fno-pie -c -o $@ $<

# Compile PS/2 Keyboard driver
$(BUILD_DIR)/keyboard.o: $(KEYBOARD_SRC)
	@echo "Compiling PS/2 Keyboard Driver..."
	@$(CC) $(CFLAGS) -c -o $@ $<

# Compile Dynamic Heap Manager
$(BUILD_DIR)/heap.o: $(HEAP_SRC)
	@echo "Compiling Dynamic Kernel Heap Manager..."
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
