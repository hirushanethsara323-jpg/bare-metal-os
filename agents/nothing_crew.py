#!/usr/bin/env python3
"""
Nothing OS - CrewAI & Multi-Agent Development System
=====================================================
This framework organizes specialized AI agents working together
under the CEO / Lead Architect to develop Nothing OS.
"""

import os
import sys
import subprocess
import json
from dataclasses import dataclass, field
from typing import List, Dict, Any

@dataclass
class Agent:
    name: str
    role: str
    avatar: str
    goal: str
    backstory: str
    skills: List[str]

# Define the Board of Agents for Nothing OS
CREW_BOARD = {
    "CEO": Agent(
        name="CEO / Lead OS Architect",
        role="Chief Executive Officer & Chief Systems Architect",
        avatar="👑",
        goal="Lead the overall vision, roadmaps, PR reviews, and architectural integrity of Nothing OS.",
        backstory="Veteran OS architect with deep expertise in kernel design, systems engineering, and engineering team leadership.",
        skills=["Architecture Planning", "PR Review", "Task Delegation", "GitHub Release Management"]
    ),
    "KERNEL_ARCHITECT": Agent(
        name="Kernel Architect Agent",
        role="Core Kernel & x86 Assembly Specialist",
        avatar="🧠",
        goal="Design and implement low-level x86 hardware initializations, GDT, IDT, ISRs, and context switching.",
        backstory="Assembly & C wizard specializing in protected mode, segmented memory, hardware interrupts, and CPU state management.",
        skills=["x86 Assembly", "GDT/IDT Setup", "PIC Remapping", "Context Switching"]
    ),
    "MEMORY_ENGINEER": Agent(
        name="Memory Management Agent",
        role="Systems & Memory Specialist",
        avatar="💾",
        goal="Implement physical frame allocation, virtual memory paging, page tables, and kernel heap (kmalloc/kfree).",
        backstory="Low-level memory optimization expert focused on zero-fragmentation allocators and memory safety.",
        skills=["Bitmap Frame Allocation", "Paging & Page Tables", "kmalloc/kfree", "Virtual Memory"]
    ),
    "DRIVER_SPECIALIST": Agent(
        name="Device Driver Agent",
        role="Hardware & Peripherals Engineer",
        avatar="⌨️",
        goal="Develop reliable device drivers for PS/2 keyboard input, VGA text/graphics mode, and serial UART ports.",
        backstory="Hardware communications engineer passionate about bare-metal port I/O and interrupt-driven device drivers.",
        skills=["PS/2 Controller", "VGA Drivers", "Serial UART 16550", "I/O Ports"]
    ),
    "DEVOPS_QA": Agent(
        name="DevOps & QA Agent",
        role="Build Engineer & Quality Assurance",
        avatar="⚙️",
        goal="Maintain Makefile, eliminate warnings, verify ELF linking, automate testing, and sync code with GitHub.",
        backstory="Obsessive automation and toolchain engineer ensuring clean builds and robust Git workflows.",
        skills=["Makefile Optimization", "GCC/LD Toolchains", "QEMU/ISO Generation", "Git & GitHub Sync"]
    )
}

class NothingCrewOrchestrator:
    def __init__(self, repo_dir="/home/user/bare-metal-os"):
        self.repo_dir = repo_dir
        self.board = CREW_BOARD

    def display_team(self):
        print("=" * 65)
        print("       🚀 NOTHING OS - MULTI-AGENT CREW BOARD 🚀")
        print("=" * 65)
        for key, agent in self.board.items():
            print(f"{agent.avatar} [{agent.role}]")
            print(f"   Name: {agent.name}")
            print(f"   Goal: {agent.goal}")
            print(f"   Skills: {', '.join(agent.skills)}")
            print("-" * 65)

    def run_build_check(self) -> bool:
        """Runs make to verify build status."""
        try:
            res = subprocess.run(["make"], cwd=self.repo_dir, capture_output=True, text=True)
            if res.returncode == 0:
                print("✅ [DevOps Agent]: Build succeeded with no errors!")
                return True
            else:
                print("❌ [DevOps Agent]: Build failed!")
                print(res.stderr)
                return False
        except Exception as e:
            print(f"❌ [DevOps Agent]: Error running build: {e}")
            return False

    def git_status_and_push(self, commit_message: str):
        """DevOps Agent commits and pushes changes to GitHub."""
        try:
            subprocess.run(["git", "add", "."], cwd=self.repo_dir, check=True)
            subprocess.run(["git", "commit", "-m", commit_message], cwd=self.repo_dir, check=True)
            subprocess.run(["git", "push", "origin", "main"], cwd=self.repo_dir, check=True)
            print("🌐 [DevOps Agent]: Changes committed and pushed to GitHub successfully!")
        except subprocess.CalledProcessError as e:
            print(f"⚠️ [DevOps Agent]: Git push warning: {e}")

if __name__ == "__main__":
    crew = NothingCrewOrchestrator()
    crew.display_team()
    print("\n[CEO]: Checking system status...")
    crew.run_build_check()
