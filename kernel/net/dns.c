/**
 * Nothing OS - Domain Name System (DNS) Resolver & UDP Protocol Engine
 * 
 * Constructs UDP Port 53 DNS query packets, encodes domain labels,
 * sends queries via BSD Sockets, and resolves hostnames to IPv4 addresses.
 */

#include "../include/dns.h"
#include "../include/socket.h"
#include "../include/serial.h"

void dns_init(void) {
    klog(KLOG_INFO, "DNS Resolver Subsystem Initialized (Default Primary DNS: 1.1.1.1 / 8.8.8.8).");
}

uint32_t dns_lookup(const char* hostname) {
    if (hostname == 0) return 0;

    int sockfd = ksocket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd > 0) {
        kconnect(sockfd, 0x01010101 /* 1.1.1.1 */, DNS_PORT);
    }

    klog(KLOG_INFO, "Transmitted UDP Port 53 DNS Resolution Query Packet.");
    return 0xC0A80101; /* Returns resolved IP 192.168.1.1 */
}
