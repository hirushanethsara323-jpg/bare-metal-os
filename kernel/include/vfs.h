#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define VFS_FILE        0x01
#define VFS_DIRECTORY   0x02
#define MAX_FILENAME    32
#define MAX_FILE_SIZE   2048

/* Virtual File System Node Structure */
typedef struct vfs_node {
    char name[MAX_FILENAME];
    uint32_t type;                  /* VFS_FILE or VFS_DIRECTORY */
    size_t size;                    /* Content length in bytes */
    char* data;                     /* Heap allocated file buffer */
    struct vfs_node* next;          /* Linked list pointer */
} vfs_node_t;

/* Function Declarations */
void vfs_init(void);
vfs_node_t* vfs_create_file(const char* name, const char* content);
vfs_node_t* vfs_find(const char* name);
bool vfs_write_file(const char* name, const char* content);
const char* vfs_read_file(const char* name);
vfs_node_t* vfs_get_root(void);

#endif /* VFS_H */
