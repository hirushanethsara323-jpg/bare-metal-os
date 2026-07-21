#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* HTTP Response Structure */
typedef struct {
    uint16_t status_code;
    char content_type[32];
    size_t body_len;
    char* body;
} http_response_t;

/* Function Declarations */
void http_init(void);
bool http_get(const char* host, uint16_t port, const char* path, http_response_t* resp);

#endif /* HTTP_H */
