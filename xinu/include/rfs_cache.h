/* rfs_cache.h - Contains constants and data structure definitions relevant to caching for the remote file system */
#define RFS_CACHING_ENABLED 1 /* value of 1 will enable caching of requested data */

#define RFS_CACHE_DEBUG 0 /* value of 1 will print debug messages */
/* Constants */

#if RFS_CACHING_ENABLED
#ifndef MAX_RFS_CBLOCKS
#define MAX_RFS_CBLOCKS 1000 /* maximum number of cached blocks a remote file can access through direct indexing (rest are accessed via a linked list structure) */
#endif

#ifndef MAX_CBLOCKS_ALLOCABLE
#define MAX_CBLOCKS_ALLOCABLE 1000 /* maximum number of blocks that can exist in the cache at any given time */
#endif

#ifndef RFS_CBLOCK_SIZE
#define RFS_CBLOCK_SIZE 1024 /* maximum number of file data bytes a cache block can contain (equal to RF_DATALEN) */
#endif

/* struct rfs_cblock - The data structure for a block of cached remote file data */
struct rfs_cblock {
    uint32 file_start; /* the beginning file position of this chunk, it will hold bytes [file_start, file_start + RFL_CHUNK_SIZE-1] */
    bool8 is_dirty;      /* True if the chunk has been written to */
    uint32 valid_bytes; /* number of bytes in the chunk that correspond to meaningful file data */
    char block_data[RFS_CBLOCK_SIZE];  /* file data contained in the chunk */

    uint32 rfl_devnum;         /* device number for the rfl device this block belongs to */
    struct rfs_cblock * lru_next;    /* The next node in the lru list */
    struct rfs_cblock * lru_prev;    /* The prev node in the lru list */

    /* the following fields are used when the block is in the linked list portion of the remote file's cache */
    struct rfs_cblock * next; /* The next node in the linked list */
    struct rfs_cblock * prev; /* The prev node in the linked list */
};

/* struct rfs_cpos - Used to pinpoint a location in a cache block by specifying the
 * cache block index and the offset within that block
 */
struct rfs_cpos {
    uint32 index;
    uint32 offset;
};
#endif