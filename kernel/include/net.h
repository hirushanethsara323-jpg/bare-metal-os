#ifndef NET_H
#define NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_ARP  0x0806

#define IP_PROTO_ICMP  1
#define IP_PROTO_TCP   6
#define IP_PROTO_UDP   17

/* Ethernet II Frame Header */
typedef struct {
    uint8_t  dest_mac[6];
    uint8_t  src_mac[6];
    uint16_t ethertype;
} __attribute__((packed)) ethernet_frame_t;

/* ARP Protocol Packet Header */
typedef struct {
    uint16_t htype;
    uint16_t ptype;
    uint8_t  hlen;
    uint8_t  plen;
    uint16_t opcode;
    uint8_t  sender_mac[6];
    uint32_t sender_ip;
    uint8_t  target_mac[6];
    uint32_t target_ip;
} __attribute__((packed)) arp_packet_t;

/* IPv4 Header */
typedef struct {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t length;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src_ip;
    uint32_t dest_ip;
} __attribute__((packed)) ipv4_header_t;

/* ICMP Header */
typedef struct {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
} __attribute__((packed)) icmp_echo_t;

/* Function Declarations */
void net_init(void);
void net_send_ping(uint32_t target_ip);
void net_get_ip(uint32_t* ip_out);
void net_get_mac(uint8_t* mac_out);

#endif /* NET_H */
