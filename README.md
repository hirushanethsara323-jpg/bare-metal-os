# Nothing OS - Custom Bare Metal Operating System

> A minimal, custom x86 operating system built from scratch in C and Assembly.

## Overview

**Nothing OS** is an educational bare-metal x86 32-bit operating system kernel developed directly in C and inline assembly. It features custom boot stages, GDT/IDT management, memory allocation, context switching, and a VGA text console.

## Architecture & Project Structure

```
bare-metal-os/
├── boot/           # Bootloader (Assembly & multiboot)
├── kernel/         # Kernel source
│   ├── arch/x86/   # x86 protected mode & VGA setup
│   ├── include/    # Kernel headers (stdint, stdbool, etc.)
│   ├── mm/         # Physical memory manager (PMM)
│   ├── sched/      # Preemptive process scheduler
│   ├── main.c      # Kernel entry point & VGA display
│   └── linker.ld   # Linker script for 32-bit ELF binary
├── user/           # User space binaries (shell prototype)
├── iso/            # GRUB bootable ISO directory structure
└── Makefile        # System build file
```

## Features

- **Multiboot Compliant**: Bootable via GRUB or QEMU direct kernel boot.
- **VGA Text Mode Console**: Custom scrolling, colored output, hex formatting.
- **Physical Memory Manager**: Bitmap-based physical frame allocator.
- **Process Scheduler**: Preemptive round-robin thread prototype.

## Building & Compilation

```bash
# Compile the kernel binary
make

# Clean build artifacts
make clean

# Create bootable ISO (requires grub-mkrescue)
make iso
```

## Authors & Maintainer

- **Developer**: `hirushanethsara323-jpg`
- **License**: MIT
