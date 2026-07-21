# =============================================================================
# BareMetal OS - Makefile
# =============================================================================
# Build system for the BareMetal OS
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
KERNEL_SRC = $(KERNEL_DIR)/main.c
BOOT_SRC   = $(BOOT_DIR)/multiboot_header.asm

# Output
KERNEL     = $(BUILD_DIR)/kernel.bin
ISO_FILE   = $(ISO_DIR)/baremetal-os.iso

# Compiler flags
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -nostdinc
CFLAGS += -I$(KERNEL_DIR)/include
ASFLAGS = -f elf32
LDFLAGS = -T $(KERNEL_DIR)/linker.ld -m elf_i386 -nostdlib

# Objects
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/boot.o

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
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) -o $@ $(OBJS)
	@echo "Kernel built: $@"
	@echo "Kernel size: $$(stat -c%s $@) bytes"

# Assemble bootloader (only if NASM available)
$(BUILD_DIR)/multiboot_header.o: $(BOOT_SRC)
	@echo "Assembling $(BOOT_SRC)..."
	@$(AS) $(ASFLAGS) -o $@ $<

# Build boot.o (C-based boot with inline assembly)
$(BUILD_DIR)/boot.o: $(KERNEL_DIR)/arch/x86/boot.c
	@echo "Building C boot code..."
	@$(CC) $(CFLAGS) -fno-pie -c -o $@ $<

# Compile kernel
$(BUILD_DIR)/main.o: $(KERNEL_SRC)
	@echo "Compiling $(KERNEL_SRC)..."
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

# Run from ISO
run-iso: iso
	@echo "Starting QEMU from ISO..."
	@$(QEMU) -cdrom $(ISO_FILE) -m 128M -display vga

# Clean build
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR) $(ISO_DIR)
	@echo "Clean complete"

# Debug mode
debug: all
	@echo "Starting QEMU with GDB..."
	@$(QEMU) -kernel $(KERNEL) -m 128M -s -S &
	@echo "Connect with: target remote localhost:1234"

# Build documentation
docs:
	@echo "Building documentation..."
	@cat README.md

# Show kernel info
info:
	@echo "Kernel: $(KERNEL)"
	@file $(KERNEL) 2>/dev/null || echo "Kernel not built"
	@if [ -f $(KERNEL) ]; then \
		echo "Size: $$(stat -c%s $(KERNEL)) bytes"; \
		xxd -l 64 $(KERNEL) | head -5; \
	fi

# =============================================================================
# Help
# =============================================================================

help:
	@echo "BareMetal OS Build System"
	@echo ""
	@echo "Targets:"
	@echo "  make all    - Build kernel (default)"
	@echo "  make run    - Build and run in QEMU"
	@echo "  make iso    - Build ISO image"
	@echo "  make run-iso - Run from ISO"
	@echo "  make debug  - Run with GDB debugging"
	@echo "  make clean  - Clean build files"
	@echo "  make help   - Show this help"
	@echo ""
	@echo "Files:"
	@echo "  Kernel:     $(KERNEL)"
	@echo "  ISO:        $(ISO_FILE)"
