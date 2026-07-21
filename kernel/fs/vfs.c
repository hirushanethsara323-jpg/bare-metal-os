/**
 * Nothing OS - In-Memory Virtual File System (MemFS / RAMDisk)
 * 
 * Provides file creation, reading, writing, and directory listing
 * using dynamic kernel heap buffers.
 */

#include "../include/vfs.h"
#include "../include/heap.h"

static vfs_node_t* vfs_root = NULL;

static size_t str_len(const char* s) {
    size_t l = 0;
    while (s[l]) l++;
    return l;
}

static void str_copy(char* dest, const char* src, size_t max_len) {
    size_t i = 0;
    while (src[i] && i < max_len - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

static int str_cmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/**
 * Initialize Virtual File System with default system configuration files.
 */
void vfs_init(void) {
    vfs_root = NULL;
    
    /* Create default system files in RAMDisk */
    vfs_create_file("welcome.txt", "Welcome to Nothing OS v0.3.0 MemFS RAMDisk!");
    vfs_create_file("kernel.cfg", "OS_NAME=NothingOS\nVERSION=0.3.0\nARCH=i386\nHEAP=ENABLED\nVFS=ENABLED");
    vfs_create_file("about.txt", "Nothing OS is an enterprise x86 operating system developed by AI Crew Board.");
}

/**
 * Get root node of VFS directory list.
 */
vfs_node_t* vfs_get_root(void) {
    return vfs_root;
}

/**
 * Find a file node by name in VFS.
 */
vfs_node_t* vfs_find(const char* name) {
    vfs_node_t* curr = vfs_root;
    while (curr != NULL) {
        if (str_cmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

/**
 * Create a new file in VFS with initial text content.
 */
vfs_node_t* vfs_create_file(const char* name, const char* content) {
    /* Check if file already exists */
    vfs_node_t* existing = vfs_find(name);
    if (existing != NULL) {
        vfs_write_file(name, content);
        return existing;
    }
    
    vfs_node_t* new_file = (vfs_node_t*)kmalloc(sizeof(vfs_node_t));
    if (new_file == NULL) {
        return NULL;
    }
    
    str_copy(new_file->name, name, MAX_FILENAME);
    new_file->type = VFS_FILE;
    
    size_t content_len = content ? str_len(content) : 0;
    if (content_len > 0) {
        new_file->data = (char*)kmalloc(content_len + 1);
        if (new_file->data) {
            str_copy(new_file->data, content, content_len + 1);
            new_file->size = content_len;
        } else {
            new_file->data = NULL;
            new_file->size = 0;
        }
    } else {
        new_file->data = NULL;
        new_file->size = 0;
    }
    
    /* Prepend to linked list */
    new_file->next = vfs_root;
    vfs_root = new_file;
    
    return new_file;
}

/**
 * Write/Overwrite content of an existing file or create if missing.
 */
bool vfs_write_file(const char* name, const char* content) {
    vfs_node_t* file = vfs_find(name);
    if (file == NULL) {
        return (vfs_create_file(name, content) != NULL);
    }
    
    if (file->data != NULL) {
        kfree(file->data);
        file->data = NULL;
    }
    
    size_t content_len = content ? str_len(content) : 0;
    if (content_len > 0) {
        file->data = (char*)kmalloc(content_len + 1);
        if (file->data == NULL) return false;
        str_copy(file->data, content, content_len + 1);
        file->size = content_len;
    } else {
        file->size = 0;
    }
    
    return true;
}

/**
 * Read contents of a file in VFS.
 */
const char* vfs_read_file(const char* name) {
    vfs_node_t* file = vfs_find(name);
    if (file == NULL || file->data == NULL) {
        return NULL;
    }
    return file->data;
}
