#ifndef SOCKET_H
#define SOCKET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define AF_INET     2
#define SOCK_STREAM 1
#define SOCK_DGRAM  2

#define MAX_SOCKETS 16

/* BSD Socket Structure */
typedef struct {
    int id;
    int domain;
    int type;
    int protocol;
    uint32_t local_ip;
    uint16_t local_port;
    uint32_t remote_ip;
    uint16_t remote_port;
    bool bound;
    bool connected;
} ksocket_t;

/* Function Declarations */
void socket_init(void);
int ksocket(int domain, int type, int protocol);
bool kbind(int sockfd, uint32_t ip, uint16_t port);
bool kconnect(int sockfd, uint32_t ip, uint16_t port);
int ksend(int sockfd, const void* buf, size_t len);
int krecv(int sockfd, void* buf, size_t max_len);

#endif /* SOCKET_H */
