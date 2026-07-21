# Nothing OS Development Corporation - Executive AI Board 🏢👑

Welcome to **Nothing OS Development Corporation**. This repository contains our custom 32-bit x86 Operating System kernel (**Nothing OS v0.4.0**) alongside our specialized executive AI engineering team.

## Executive Corporate Board & Agent Roster

| Avatar | Agent Name | Corporate Role | Engineering Domain & System Responsibilities |
|---|---|---|---|
| 👑 | **CEO Agent** | Chief Executive Officer & OS Lead | Overall OS Architecture, Sprint Roadmap, Code Review & GitHub Release Management |
| 🔍 | **OS Research Agent** | Architecture & Standards Lead | Research OSDev specifications, POSIX standards, and x86 hardware primitives |
| 🧪 | **Automated Testing Lead** | QA & Self-Testing Specialist | Self-Testing Suite (`ktest`), Memory Verification, VFS Integrity & Syscall Validation |
| 🧠 | **Core Kernel Architect** | x86 Assembly & Core Lead | GDT, 256-Gate IDT, 8259 PIC Vector Remapping, CPU Interrupt Handling |
| ⚙️ | **POSIX Syscall Lead** | System Call Engine Specialist | Software Interrupt Vector `INT 0x80` Dispatcher (`SYS_YIELD`, `SYS_WRITE`, `SYS_ALLOC`) |
| 💾 | **Memory Specialist** | Systems & Memory Lead | Physical Memory Allocator, Boundary-Tag Kernel Heap (`kmalloc`/`kfree`) |
| ⌨️ | **Hardware Driver Lead** | Peripherals & Interface Engineer | PS/2 Keyboard Input Driver, Serial Port UART 16550 COM1 Telemetry Engine |
| ⏰ | **Clock & Peripherals Lead** | Timers & CMOS RTC Specialist | PIT 8253 System Timer (100 Hz), CMOS Real-Time Clock (RTC Date/Time) |
| 🎨 | **UI/UX Graphics Engine Lead** | Theme & Display Specialist | VGA Window Box Components, Progress Bars, Status Bars, Color Theme Palettes |
| 📁 | **VFS & Storage Specialist** | Filesystem & RAMDisk Lead | In-Memory Virtual File System (MemFS RAMDisk), File Creation, Reading & Writing |
| 🐚 | **Shell & UX Lead** | Console & Shell Specialist | Interactive System Shell (`ls`, `cat`, `touch`, `write`, `theme`, `test`, `syscall`) |
| ⚙️ | **DevOps & QA Lead** | Build & Toolchain Specialist | Makefile, Linker Script, Compiler Checks & GitHub Synchronization |

## Orchestrator Execution Command

To display the active corporate board and trigger automated build verification:

```bash
python3 agents/nothing_crew.py
```
