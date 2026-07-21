# BareMetal OS - Project Index

```
bare-metal-os/
│
├── 📁 boot/                          # Bootloader
│   ├── boot.asm                      # Simple 16-bit boot sector
│   └── multiboot_header.asm          # GRUB multiboot header
│
├── 📁 kernel/                        # Kernel source
│   ├── main.c                        # Kernel main entry
│   ├── linker.ld                     # Linker script
│   ├── mm/
│   │   └── pmm.c                     # Physical Memory Manager
│   └── sched/
│       └── scheduler.c               # Process Scheduler
│
├── 📁 user/                          # User space programs
│   └── bin/
│       └── shell.c                   # Simple command shell
│
├── 📁 iso/                           # ISO image contents
│   └── boot/grub/
│       └── grub.cfg                  # GRUB configuration
│
├── 📁 include/                       # Header files
│   └── stdint.h                      # Standard integer types
│
├── Makefile                          # Build system
├── README.md                         # Project documentation
└── PROJECT_INDEX.md                 # This file
```

## Quick Start

```bash
# Build the kernel
make

# Run in QEMU
make run

# Create bootable ISO
make iso

# Clean build
make clean
```

## What's Implemented

| Component | Status | Description |
|-----------|--------|-------------|
| Bootloader | ✅ | GRUB multiboot + simple boot sector |
| Terminal | ✅ | VGA text mode driver |
| Memory (PMM) | ✅ | Bitmap-based physical memory manager |
| Scheduler | ✅ | Round-robin preemptive multitasking |
| Shell | ✅ | Basic command-line interface |

## What's Next

| Component | Priority | Description |
|-----------|----------|-------------|
| VFS | High | Virtual File System layer |
| ext2 FS | High | EXT2 file system driver |
| System Calls | High | Kernel-user interface |
| Keyboard | Medium | PS/2 keyboard driver |
| Paging | Medium | Virtual memory management |
| ATA Driver | Medium | Hard disk driver |
| User LibC | Medium | C standard library |

## Build Requirements

```bash
# Debian/Ubuntu
sudo apt install build-essential nasm grub-common xorriso qemu-system-x86

# Fedora
sudo dnf install gcc nasm grub2 xorriso qemu-system-x86

# Arch
sudo pacman -S base-devel nasm grub xorriso qemu
```

## Resources

- **OSDev Wiki**: https://wiki.osdev.org/
- **Little Kernel**: https://github.com/littlekernel/lk
- **xv6**: https://github.com/mit-pdos/xv6-public

---

**Happy Hacking!** 🚀
