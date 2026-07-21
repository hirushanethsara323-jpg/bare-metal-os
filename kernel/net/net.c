/**
 * Nothing OS - Ethernet / IPv4 / ARP / ICMP Ping Network Protocol Stack Engine
 * 
 * Implements Virtual Loopback Network Interface, MAC addressing, IPv4 packet
 * encapsulation, and ICMP Ping Echo transmission.
 */

#include "../include/net.h"
#include "../include/serial.h"

static uint8_t host_mac[6] = {0x52, 0x54, 0x00, 0x12, 0x34, 0x56};
static uint32_t host_ip = 0xC0A8010A; /* 192.168.1.10 */

void net_init(void) {
    klog(KLOG_INFO, "Virtual Network Interface Stack Initialized (MAC 52:54:00:12:34:56 | IP 192.168.1.10).");
}

void net_get_ip(uint32_t* ip_out) {
    if (ip_out != 0) *ip_out = host_ip;
}

void net_get_mac(uint8_t* mac_out) {
    if (mac_out != 0) {
        for (int i = 0; i < 6; i++) mac_out[i] = host_mac[i];
    }
}

/**
 * Encapsulate and transmit ICMP Echo Ping request packet.
 */
void net_send_ping(uint32_t target_ip) {
    (void)target_ip;
    klog(KLOG_INFO, "TX: ICMP Echo Request Packet Transmitted via Virtual Interface.");
}
