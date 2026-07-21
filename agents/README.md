# Nothing OS - Multi-Agent Crew Framework 🤖👑

This directory contains the **CrewAI & Multi-Agent System Framework** for **Nothing OS**.

## Crew Organization & Roles

| Avatar | Agent Name | Role | Responsibilities |
|---|---|---|---|
| 👑 | **CEO Agent** | Lead OS Architect & CEO | Project vision, PR reviews, task delegation, release management |
| 🧠 | **Kernel Architect** | Low-Level Systems & Assembly | GDT, IDT, ISRs, Interrupt Controller, Multi-tasking |
| 💾 | **Memory Specialist** | Memory Management Engineer | Physical Frame Allocation, Paging, `kmalloc`/`kfree` |
| ⌨️ | **Driver Specialist** | Peripherals & Input Engineer | PS/2 Keyboard, VGA Text/Graphics, Serial UART |
| ⚙️ | **DevOps & QA Agent** | Build & Toolchain Specialist | Makefile, Linker verification, CI/CD, GitHub sync |

## Orchestrator Execution

To view team status and execute multi-agent build checks:

```bash
python3 agents/nothing_crew.py
```
