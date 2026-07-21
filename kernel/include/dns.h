#ifndef DNS_H
#define DNS_H

#include <stdint.h>
#include <stdbool.h>

#define DNS_PORT 53

/* DNS Packet Header Structure */
typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) dns_header_t;

/* Function Declarations */
void dns_init(void);
uint32_t dns_lookup(const char* hostname);

#endif /* DNS_H */
