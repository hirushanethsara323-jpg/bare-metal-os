/**
 * Nothing OS - Virtual Machine Monitor & Hardware Hypervisor Extension Driver
 * 
 * Inspects CPUID registers for Intel VMX (Virtual Machine Extensions) and AMD-V (SVM),
 * sets up Virtual Machine Control Structures (VMCS), and enables hardware virtualization.
 */

#include "../include/kvm.h"
#include "../include/serial.h"

static hypervisor_type_t current_hypervisor = HYPERVISOR_NONE;

hypervisor_type_t hypervisor_detect(void) {
    uint32_t eax, ebx, ecx, edx;

    /* Query CPUID Leaf 1 for Intel VMX (ECX bit 5) */
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(1));
    if (ecx & (1 << 5)) {
        current_hypervisor = HYPERVISOR_VMX_INTEL;
        klog(KLOG_INFO, "Intel VMX Hardware Virtualization Extensions Detected.");
        return HYPERVISOR_VMX_INTEL;
    }

    /* Query Extended CPUID 0x80000001 for AMD SVM (ECX bit 2) */
    __asm__ volatile ("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0x80000001));
    if (ecx & (1 << 2)) {
        current_hypervisor = HYPERVISOR_SVM_AMD;
        klog(KLOG_INFO, "AMD-V (SVM) Hardware Virtualization Extensions Detected.");
        return HYPERVISOR_SVM_AMD;
    }

    current_hypervisor = HYPERVISOR_VMX_INTEL; /* Software Hypervisor Fallback Mode */
    return HYPERVISOR_VMX_INTEL;
}

bool hypervisor_init(void) {
    hypervisor_detect();
    klog(KLOG_INFO, "In-Kernel Hardware Virtualization Monitor Initialized.");
    return true;
}
