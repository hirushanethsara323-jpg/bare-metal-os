#ifndef EXT2_H
#define EXT2_H

#include <stdint.h>
#include <stdbool.h>

#define EXT2_SUPER_MAGIC 0xEF53

/* Ext2 Filesystem Superblock Structure */
typedef struct {
    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_r_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic; /* Must equal 0xEF53 */
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
} __attribute__((packed)) ext2_superblock_t;

/* Function Declarations */
bool ext2_validate_superblock(const ext2_superblock_t* sb);
void ext2_inspect_superblock(const ext2_superblock_t* sb);

#endif /* EXT2_H */
