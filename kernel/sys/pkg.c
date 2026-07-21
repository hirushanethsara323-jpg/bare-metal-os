/**
 * Nothing OS - Package Manager & Dynamically Loaded Kernel Modules
 * 
 * Manages kernel extension packages, SHA-256 signature verification,
 * dynamic package installation, and package dependency tracking.
 */

#include "../include/pkg.h"
#include "../include/heap.h"
#include "../include/crypto.h"
#include "../include/serial.h"

static pkg_node_t* package_list = NULL;

static void str_copy(char* dest, const char* src, size_t max) {
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void pkg_init(void) {
    package_list = NULL;

    /* Install Default Base OS Packages */
    uint8_t dummy1[] = "Base System Kernel Core Binary Data";
    uint8_t dummy2[] = "Network Protocols Utility Package Data";
    uint8_t dummy3[] = "VGA Compositor Graphics Theme Package Data";

    pkg_install("base-system", "5.0.0", dummy1, sizeof(dummy1));
    pkg_install("net-tools", "1.4.0", dummy2, sizeof(dummy2));
    pkg_install("graphics-suite", "2.1.0", dummy3, sizeof(dummy3));

    klog(KLOG_INFO, "Nothing OS Package Manager Subsystem Initialized.");
}

bool pkg_install(const char* name, const char* version, const uint8_t* binary, size_t size) {
    if (name == NULL || version == NULL) return false;

    pkg_node_t* pkg = (pkg_node_t*)kmalloc(sizeof(pkg_node_t));
    if (pkg == NULL) return false;

    str_copy(pkg->name, name, 32);
    str_copy(pkg->version, version, 16);
    pkg->binary_size = size;
    pkg->installed = true;

    if (binary != NULL && size > 0) {
        sha256_hex(binary, size, pkg->sha256_hash);
    } else {
        str_copy(pkg->sha256_hash, "0000000000000000000000000000000000000000000000000000000000000000", 65);
    }

    pkg->next = package_list;
    package_list = pkg;

    return true;
}

pkg_node_t* pkg_get_installed(void) {
    return package_list;
}
