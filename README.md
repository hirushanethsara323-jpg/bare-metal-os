# BareMetal OS - A Unix-like Operating System from Scratch

> Building a complete operating system from the ground up

## Project Overview

This is a **bare metal OS** built from scratch using C and Assembly. It's designed with Unix-like principles.

## Architecture

```
bare-metal-os/
├── boot/           # Bootloader (GRUB)
├── kernel/         # Kernel source
│   ├── arch/x86/   # x86 architecture code
│   ├── drivers/   # Device drivers
│   ├── fs/        # File systems
│   ├── mm/        # Memory management
│   ├── proc/      # Process management
│   └── sched/     # Scheduler
├── lib/           # Standard library
├── user/          # User space programs
├── include/       # Header files
├── docs/          # Documentation
└── iso/           # ISO image output
```

## Core Components

### 1. Bootloader (boot/)
- GRUB2 multiboot compliant
- 512-byte boot sector fallback

### 2. Kernel (kernel/)
- **arch/x86/**: x86-specific code (GDT, IDT, PIC, paging)
- **mm/**: Memory management (physical, virtual memory, kmalloc)
- **sched/**: Process scheduler (preemptive multitasking)
- **fs/**: Virtual file system + ext2 driver
- **drivers/**: Hardware drivers (VGA, keyboard, ATA, serial)

### 3. Standard Library (lib/)
- libc implementation
- String functions
- I/O routines

### 4. User Space (user/)
- `/bin/init` - First user process
- `/bin/sh` - Shell
- `/bin/ls` - Directory listing
- `/bin/cat` - File display

## Building

```bash
# Install dependencies
sudo apt install build-essential binutils grub-common xorriso qemu-system-x86

# Build everything
make

# Run in QEMU
make run

# Build ISO
make iso
```

## Roadmap

### Phase 1: Boot & Kernel (Week 1-2)
- [x] Project structure
- [ ] GRUB multiboot header
- [ ] GDT setup
- [ ] IDT & PIC setup
- [ ] Paging & memory management
- [ ] Basic VGA text output
- [ ] Keyboard interrupt handling

### Phase 2: Process Management (Week 3-4)
- [ ] Context switching
- [ ] Process control block
- [ ] Round-robin scheduler
- [ ] System calls (fork, exec, exit, wait)
- [ ] Basic IPC

### Phase 3: File System (Week 5-6)
- [ ] VFS layer
- [ ] ext2 implementation
- [ ] Block cache
- [ ] Path resolution

### Phase 4: Drivers (Week 7-8)
- [ ] ATA/IDE driver
- [ ] Serial port driver
- [ ] PS/2 keyboard driver
- [ ] Basic framebuffer (optional)

### Phase 5: User Space (Week 9-10)
- [ ] LibC
- [ ] Init process
- [ ] Simple shell
- [ ] Basic utilities

### Phase 6: Polish (Week 11-12)
- [ ] System init scripts
- [ ] Configuration files
- [ ] Documentation
- [ ] Testing

## System Requirements

- x86/x86_64 processor
- 64MB+ RAM (minimum)
- QEMU or real hardware
- GRUB2 for booting

## References

- [OSDev Wiki](https://wiki.osdev.org/)
- [Linux Kernel Source](https://github.com/torvalds/linux)
- [xv6](https://github.com/mit-pdos/xv6-public) - Educational Unix-like OS
- [Little Kernel](https://github.com/littlekernel/lk) - Embedded kernel

## License

MIT License

---

**Status**: Project initialized | **Type**: Bare Metal OS | **Architecture**: x86
