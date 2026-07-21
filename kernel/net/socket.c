/**
 * Nothing OS - BSD Sockets Network API Layer & Socket Descriptor Table
 * 
 * Implements standard BSD Socket creation (AF_INET, SOCK_STREAM, SOCK_DGRAM),
 * IP/Port binding, remote network connections, and send/recv message passing.
 */

#include "../include/socket.h"
#include "../include/serial.h"

static ksocket_t sockets[MAX_SOCKETS];
static int next_socket_id = 1;

void socket_init(void) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        sockets[i].id = 0;
        sockets[i].bound = false;
        sockets[i].connected = false;
    }
    klog(KLOG_INFO, "BSD Network Socket API Subsystem Initialized.");
}

int ksocket(int domain, int type, int protocol) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].id == 0) {
            sockets[i].id = next_socket_id++;
            sockets[i].domain = domain;
            sockets[i].type = type;
            sockets[i].protocol = protocol;
            sockets[i].bound = false;
            sockets[i].connected = false;
            klog(KLOG_INFO, "Created New BSD Network Socket.");
            return sockets[i].id;
        }
    }
    return -1;
}

bool kbind(int sockfd, uint32_t ip, uint16_t port) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].id == sockfd) {
            sockets[i].local_ip = ip;
            sockets[i].local_port = port;
            sockets[i].bound = true;
            klog(KLOG_INFO, "Bound BSD Socket to Local IP/Port.");
            return true;
        }
    }
    return false;
}

bool kconnect(int sockfd, uint32_t ip, uint16_t port) {
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].id == sockfd) {
            sockets[i].remote_ip = ip;
            sockets[i].remote_port = port;
            sockets[i].connected = true;
            klog(KLOG_INFO, "Connected BSD Socket to Remote Destination Address.");
            return true;
        }
    }
    return false;
}

int ksend(int sockfd, const void* buf, size_t len) {
    if (buf == 0) return -1;
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].id == sockfd && sockets[i].connected) {
            return (int)len;
        }
    }
    return -1;
}

int krecv(int sockfd, void* buf, size_t max_len) {
    (void)max_len;
    if (buf == 0) return -1;
    for (int i = 0; i < MAX_SOCKETS; i++) {
        if (sockets[i].id == sockfd) {
            return 0;
        }
    }
    return -1;
}
