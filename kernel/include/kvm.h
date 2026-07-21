#ifndef KVM_H
#define KVM_H

#include <stdint.h>
#include <stdbool.h>

/* Hypervisor Hardware Support Types */
typedef enum {
    HYPERVISOR_NONE,
    HYPERVISOR_VMX_INTEL,
    HYPERVISOR_SVM_AMD
} hypervisor_type_t;

/* Function Declarations */
hypervisor_type_t hypervisor_detect(void);
bool hypervisor_init(void);

#endif /* KVM_H */
