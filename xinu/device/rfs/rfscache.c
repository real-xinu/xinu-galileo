/* rfscache - Collection of functions needed for the remote file system cache */
#include <xinu.h>

extern uint32 rfs_cache_testsize;

#if RFS_CACHING_ENABLED
/* byte_to_index - convertes a desired byte number to the correct index/offset to look for in the cache */
uint32 byte_to_index(
    uint32 byte,
    struct rfs_cpos *result) 
{
    if (byte < 0) {
        return SYSERR;
    }
    result->index = byte / RFS_CBLOCK_SIZE;
    result->offset = byte % RFS_CBLOCK_SIZE;
    return OK;
}

/* print_cache_list - debugging function to print current cache list for a specified remote file */
void print_cache_list(
    struct rflcblk *rfptr)  /* the remote file whose cached contents will be printed */
{
    struct rfs_cblock *cur = rfptr->cache_list;
    uint32 num = 0;
    kprintf("Printing cache_list: ");
    while (cur != NULL) {
        kprintf("{Node %d: file_start = %d, valid_bytes = %d} ->", num, cur->file_start, cur->valid_bytes);
        cur = cur->next;
        num++;
    }
    kprintf("\n");
    return;
}

/* dump_cache_blocks - prints out the contents of the specified remote file's cache */
void dump_cache_blocks(
    struct rflcblk * rfptr) /* the remote file whose cached contents will be printed */
{
    kprintf("Dumping cache\n");
    for (uint32 i = 0; i < MAX_RFS_CBLOCKS; i++) {
        kprintf("Block at index %d has %d\n", i, (uint32) rfptr->cache[i]);
    }

    print_cache_list(rfptr);
    kprintf("Done dumping cache\n");
}

/* print_lru_list - prints out the contents of the rfs's lru list */
void print_lru_list() {
    /* will be called from testing function, so need to acquire mutex */
    wait(Rf_data.rf_mutex);

    kprintf("++++++++++++++++Printing LRU list++++++++++++++++\n");
    kprintf("%d blocks used out of %d\n", Rf_data.num_cblocks, rfs_cache_testsize == 0 ? MAX_CBLOCKS_ALLOCABLE : rfs_cache_testsize);

    struct rfs_cblock *cur = Rf_data.lru_head;
    uint32 index = 0;
    char arr_index[2];
    while (cur != NULL) {
        if (cur->file_start < RF_DATALEN * 10) {
            sprintf(arr_index, "%d", cur->file_start / RF_DATALEN);
            arr_index[1] = '\0';
        }
        else {
            arr_index[0] = 'L';
            arr_index[1] = '\0';
        }

        kprintf("{Name: %s, (F%d, I%s), vbytes: %d} -> ", (&rfltab[cur->rfl_devnum-RFILE0])->rfname, cur->rfl_devnum-RFILE0, arr_index, cur->valid_bytes);
        index++;
        cur = cur->lru_next;
    }
    kprintf("\n");
    cur = Rf_data.lru_tail;
    if (cur->file_start < RF_DATALEN * 10) {
        sprintf(arr_index, "%d", cur->file_start / RF_DATALEN);
        arr_index[1] = '\0';
    }
    else {
        arr_index[0] = 'L';
        arr_index[1] = '\0';
    }
    kprintf("Tail is: {Name: %s, (F%d, I%s), vbytes: %d}\n", (&rfltab[cur->rfl_devnum-RFILE0])->rfname, cur->rfl_devnum-RFILE0, arr_index, cur->valid_bytes);
    kprintf("++++++++++++++++Finished printing LRU list++++++++++++++++\n");

    /* release mutex */
    signal(Rf_data.rf_mutex);
    return;
}

/* update_lru - places the specified cache block at the head of the remote file system's lru cache list */
uint32 update_lru(
    struct rfs_cblock *cur_block)   /* the block to place at the head of the lru list */
{
    struct rfs_cblock *prev;    /* used for removing node from lru list */
    struct rfs_cblock *next;    /* used for removing node from lru list */

    if (Rf_data.lru_head != cur_block) {
        #if RFS_CACHE_DEBUG
        kprintf("UPDATE_LRU: Setting {File %d, block %d} as new head of lru list!\n", cur_block->rfl_devnum-RFILE0, cur_block->file_start/RF_DATALEN);
        #endif

        /* get prev and next pointers */
        prev = cur_block->lru_prev;
        next = cur_block->lru_next;

        /* remove cur_block from lru list */
        if (prev != NULL) {
            #if RFS_CACHE_DEBUG
            kprintf("UPDATE_LRU: Prev not NULL!\n");
            #endif
            prev->lru_next = next;
        }

        if (next != NULL) {
            #if RFS_CACHE_DEBUG
            kprintf("UPDATE_LRU: Next not NULL!\n");
            #endif
            next->lru_prev = prev;
        }

        /* re-insert cur_block at head of list */
        cur_block->lru_prev = NULL;
        cur_block->lru_next = Rf_data.lru_head;
        Rf_data.lru_head->lru_prev = cur_block;
        Rf_data.lru_head = cur_block;

        if (cur_block->lru_next == NULL) {
            /* is only block in list, so head is also tail */
            Rf_data.lru_tail = cur_block;
        }
    }

    return OK;
}

/* rfs_cache_fetch - Copies over as many bytes as possible from cache into buffer */
uint32 rfs_cache_fetch(
    struct rflcblk *rfptr,      /* pointer to remote file's control block */
    uint32 position,            /* position of requested data */
    char *buff,                 /* user's buffer */
    int32 count)                /* count of desired bytes read */ 
{
    struct rfs_cpos cache_loc; /* location of target bytes in cache */
    uint32 status;              /* used to verify status of various helper calls */
    uint32 bytes_found;         /* number of bytes of file data found */
    struct rfs_cblock *cur_block; /* current block we are examining */
    char *from, *to;            /* Used for copying from cache to user buffer */

    /* check that position is legal */
    if (position < 0 || position > rfptr->rfsize) {
        return SYSERR;
    }

    /* find correct location in cache */
    status = byte_to_index(position, &cache_loc);
    if (status == SYSERR) {
        return SYSERR;
    }

    #if RFS_CACHE_DEBUG
    kprintf("RFS_CACHE_FETCH: looking in index %d, offset %d\n", cache_loc.index, cache_loc.offset);
    #endif

    /* if we reach here, we know all requested bytes lie in same cache block */
    if (cache_loc.index < MAX_RFS_CBLOCKS) {
        /* block resides in array structure, directly index in */
        if (rfptr->cache[cache_loc.index] == NULL || rfptr->cache[cache_loc.index]->valid_bytes == 0) {
            /* desired block not in cache */
            /* we'll make a network request to bring this block into cache, then try fetching again, so this fetch fails */
            #if RFS_CACHE_DEBUG
            kprintf("RFS_CACHE_FETCH: desired block %d not in this remote file's cache\n", cache_loc.index);
            #endif

            return 0;
        }
        else {
            /* retrieve pointer to cache block */
            cur_block = rfptr->cache[cache_loc.index];

            /* copy until no longer in valid byte range or exceed count */
            bytes_found = 0;
            to = buff;
            from = (char *) ((uint32) cur_block->block_data + cache_loc.offset);

            #if RFS_CACHE_DEBUG
            kprintf("RFS_CACHE_FETCH: Copying ");
            #endif

            while (cache_loc.offset < cur_block->valid_bytes && bytes_found < count) {
                #if RFS_CACHE_DEBUG
                kprintf("%c", *from);
                #endif

                *to++ = *from++;
                cache_loc.offset++;
                bytes_found++;
            }
            #if RFS_CACHE_DEBUG
            kprintf("\n");
            #endif

            /* update the LRU list to indicate that this is now the most recently used cache block */
            update_lru(cur_block);

            return bytes_found;
        }
    }
    else {
        #if RFS_CACHE_DEBUG
        kprintf("RFS_CACHE_FETCH: Looking in linked list!\n");
        #endif
        /* have to look in linked list, search for desired block */
        uint32 target_file_start = cache_loc.index * RF_DATALEN;
        cur_block = rfptr->cache_list;

        /* search through sorted list until find block with correct file start */
        while (cur_block->file_start < target_file_start && cur_block != NULL) {
            cur_block = cur_block->next;
        }

        if (cur_block == NULL || cur_block->file_start != target_file_start) {
            /* block not found, we'll try making a network request then try again */
            #if RFS_CACHE_DEBUG
            kprintf("RFS_CACHE_FETCH: desired block %d not in this remote file's cache\n", cache_loc.index);
            #endif
            return 0;
        }

        if (cur_block->valid_bytes == 0) {
            /* this block is not valid, so we have a cache miss */
            #if RFS_CACHE_DEBUG
            kprintf("RFS_CACHE_FETCH: desired block %d not in this remote file's cache\n", cache_loc.index);
            #endif

            return 0;
        }

        /* found the block we were looking for */
        /* copy until no longer in valid byte range or exceed count */
        bytes_found = 0;
        to = buff;
        from = (char *) ((uint32) cur_block->block_data + cache_loc.offset);

        #if RFS_CACHE_DEBUG
        kprintf("RFS_CACHE_FETCH: Copying ");
        #endif

        while (cache_loc.offset < cur_block->valid_bytes && bytes_found < count) {
            #if RFS_CACHE_DEBUG
            kprintf("%c", *from);
            #endif

            *to++ = *from++;
            cache_loc.offset++;
            bytes_found++;
        }
        #if RFS_CACHE_DEBUG
        kprintf("\n");
        #endif

        /* update the LRU list to indicate that this is now the most recently used cache block */
        update_lru(cur_block);

        return bytes_found;
    }

    return bytes_found;
}

/* remove_from_lru_list - Removes the specified cache block from the lru list and updates data structures accordingly */
void remove_from_lru_list(struct rfs_cblock * to_rem) {
    struct rfs_cblock * prev;
    struct rfs_cblock * next;

    prev = to_rem->lru_prev;
    next = to_rem->lru_next;

    if (prev == NULL) {
        /* block is head of list */
        Rf_data.lru_head = next;
    }
    else {
        /* update prev */
        prev->lru_next = next;
    }

    if (next == NULL) {
        /* block is tail of list */
        Rf_data.lru_tail = prev;
    }
    else {
        /* update next */
        next->lru_prev = prev;
    }

    to_rem->lru_next = NULL;
    to_rem->lru_prev = NULL;

    freebuf((char *) to_rem);

    Rf_data.num_cblocks--;
}

/* get_new_cblock - function to retrieve a new cache block, evicting a block from the LRU list if necessary */
struct rfs_cblock * get_new_cblock()
{
    struct rfs_cblock *new_block;       /* the newly allocated block we will return */
    // struct rfs_cblock *prev;            /* used to de-allocate node from lru list */

    struct rfs_cblock *cprev;       /* used to de-allocate node from cache linked list */
    struct rfs_cblock *cnext;       /* used to de-allocate node from cache linked list */

    /* check if the max number of cache blocks have been allocated */
    if ((rfs_cache_testsize > 0 && Rf_data.num_cblocks == rfs_cache_testsize) /* included for testing */
        || Rf_data.num_cblocks == MAX_CBLOCKS_ALLOCABLE) {
        /* have to evict the tail block before allocating */
        #if RFS_CACHE_DEBUG
        kprintf("GET_NEW_CBLOCK: Have to evict tail: File %d, block %d!\n", Rf_data.lru_tail->rfl_devnum-RFILE0, Rf_data.lru_tail->file_start / RF_DATALEN);
        #endif

        /* remove block from cache */
        if (Rf_data.lru_tail->file_start < MAX_RFS_CBLOCKS * RF_DATALEN) {
            /* remove from array portion of cache */
            (&rfltab[Rf_data.lru_tail->rfl_devnum-RFILE0])->cache[Rf_data.lru_tail->file_start / RF_DATALEN] = NULL;
        }
        else {
            /* remove from linked list portion of cache */
            cprev = Rf_data.lru_tail->prev;
            cnext = Rf_data.lru_tail->next;

            if (cprev == NULL) {
                /* need to make cnext the new head of list */
                (&rfltab[Rf_data.lru_tail->rfl_devnum-RFILE0])->cache_list = cnext;

                if (cnext != NULL) {
                    cnext->prev = NULL;
                }

                Rf_data.lru_tail->prev = NULL;
                Rf_data.lru_tail->next = NULL;
            }
            else {
                /* we are not removing head of list */
                if (cprev != NULL) {
                    cprev->next = cnext;
                }

                if (cnext != NULL) {
                    cnext->prev = cprev;
                }
            }
        }

        /* remove tail from list */
        remove_from_lru_list(Rf_data.lru_tail);

        // /* de-allocate old tail */
        // freebuf((char *) Rf_data.lru_tail);

        // /* set prev as new tail of list */
        // Rf_data.lru_tail = prev;

        // /* decrement count of allocated blocks */
        // Rf_data.num_cblocks--;
    }

    /* allocate a new cache block */
    new_block = (struct rfs_cblock *) getbuf(Rf_data.buffpoolid);
    if (new_block == (struct rfs_cblock *) SYSERR) {
        return NULL;
    }

    /* increment count of allocated cache blocks */
    Rf_data.num_cblocks++;

    /* return the new cache block */
    return new_block;
}

/* insert_into_clist - helper function to insert a new cache block into the specified remote file's cache list */
uint32 insert_into_clist(
    struct rflcblk *rfptr,          /* the remote file whose list the block will be inserted into */
    struct rfs_cblock *new_block)   /* the block to insert */
{
    struct rfs_cblock *cur;         /* used to walk through cache list */

    if (rfptr->cache_list == NULL) {
        /* list is empty */
        rfptr->cache_list = new_block;

        #if RFS_CACHE_DEBUG
        print_cache_list(rfptr);
        #endif
        return OK;
    }

    /* place node into list in sorted order */
    cur = rfptr->cache_list;
    while (cur->file_start < new_block->file_start && cur->next != NULL) {
        cur = cur->next;
    }

    if (cur->file_start >= new_block->file_start) {
        /* place before */
        if (cur == rfptr->cache_list) {
            /* new_node is head of list now */
            cur->prev = new_block;
            new_block->next = cur;
            rfptr->cache_list = new_block;

            #if RFS_CACHE_DEBUG
            print_cache_list(rfptr);
            #endif
            return OK;
        }

        /* new_node is not head of list now, update both sides */
        struct rfs_cblock *prev = cur->prev;
        prev->next = new_block;
        cur->prev = new_block;
        new_block->prev = prev;
        new_block->next = cur;

        #if RFS_CACHE_DEBUG
        print_cache_list(rfptr);
        #endif
        return OK;
    }

    /* new_node is tail of list now */
    cur->next = new_block;
    new_block->prev = cur;

    #if RFS_CACHE_DEBUG
    print_cache_list(rfptr);
    #endif
    return OK;
}

/* rfs_cache_store - function to place contents of specified read request into a cache block for specified remote file */
uint32 rfs_cache_store(
    struct rflcblk *rfptr,      /* the remote file control block that will store the cached contents */
    struct rf_msg_rres * resp)  /* the read request to cache */
{
    struct rfs_cpos cache_loc;     /* position in cache to place this block */
    uint32 status;                  /* used for checking status of function calls */
    uint32 i;                       /* used for indexing into block */
    struct rfs_cblock *new_block;   /* used to store the newly allocated cache block */

    status = byte_to_index(ntohl(resp->rf_pos), &cache_loc);
    if (status == SYSERR) {
        kprintf("RFS_CACHE_STORE: Invalid position %d!\n", ntohl(resp->rf_pos));
        return SYSERR;
    }

    /* offset should always be zero since we always copy from start of block */
    if (cache_loc.offset != 0) {
        return SYSERR;
    }

    #if RFS_CACHE_DEBUG
    kprintf("RFS_CACHE_STORE: storing response in index %d, offset %d\n", cache_loc.index, cache_loc.offset);
    #endif

    if (cache_loc.index < MAX_RFS_CBLOCKS) {

        /* if there already is a block in this location (from a previous remote file), de-allocate it first before overwriting this loc */
        if (rfptr->cache[cache_loc.index] != NULL) {

            /* remove block from lru_list */
            remove_from_lru_list(rfptr->cache[cache_loc.index]);

            /* set space in cache to NULL */
            rfptr->cache[cache_loc.index] = NULL;
        }

        /* Retrieve a new cache block, evicting from the LRU list if necessary */
        new_block = get_new_cblock();
        if (new_block == NULL) {
            kprintf("Error allocating a new cache block!\n");
            return SYSERR;
        }

        /* block should be placed in the array structure */
        #if RFS_CACHE_DEBUG
        kprintf("RFS_CACHE_STORE: copying the following to block %d: ", cache_loc.index);
        #endif

        /* copy contents into block */
        for (i = 0; i < RF_DATALEN && i < ntohl(resp->rf_len); i++) {
            new_block->block_data[i] = resp->rf_data[i];
            #if RFS_CACHE_DEBUG
            kprintf("%c", new_block->block_data[i]);
            #endif
        }
        #if RFS_CACHE_DEBUG
        kprintf("\n");
        #endif

        /* set up block metadata */
        new_block->file_start = ntohl(resp->rf_pos);
        new_block->is_dirty = 0;

        /* we copy min(RF_DATALEN, resp->rf_len) bytes into block */
        new_block->valid_bytes = ntohl(resp->rf_len) <= RF_DATALEN ? ntohl(resp->rf_len) : RF_DATALEN;

        /* setup block's devnum to associate it with this remote file */
        new_block->rfl_devnum = rfptr->rfdev;

        /* block will reside in array, set next/prev pointers to NULL */
        new_block->prev = NULL;
        new_block->next = NULL;

        /* setup block's lru list pointers as NULL, will be set when the block is fetched */
        new_block->lru_prev = NULL;
        new_block->lru_next = NULL;

        /* if we are storing in response to a write, need to immediately update lru with new block */
        if (resp->rf_type == RF_MSG_WRES) {
            update_lru(new_block);
        }
        
        /* place block into array structure */
        rfptr->cache[cache_loc.index] = new_block;

        #if RFS_CACHE_DEBUG
        kprintf("RFS_CACHE_STORE: New block at address %d, cache contains address %d\n", (uint32) new_block, (uint32) rfptr->cache[cache_loc.index]);
        #endif

        return OK;
    }
    else {

        /* Retrieve a new cache block, evicting from the LRU list if necessary */
        new_block = get_new_cblock();
        if (new_block == NULL) {
            kprintf("Error allocating a new cache block!\n");
            return SYSERR;
        }

        /* have to add block to linked list of blocks */
        #if RFS_CACHE_DEBUG
        kprintf("RFS_CACHE_STORE: Adding to linked list!\n");
        #endif

        /* copy contents into block */
        for (i = 0; i < RF_DATALEN && i < ntohl(resp->rf_len); i++) {
            new_block->block_data[i] = resp->rf_data[i];
            #if RFS_CACHE_DEBUG
            kprintf("%c", new_block->block_data[i]);
            #endif
        }
        #if RFS_CACHE_DEBUG
        kprintf("\n");
        #endif

        /* set up block metadata */
        new_block->file_start = ntohl(resp->rf_pos);
        new_block->is_dirty = 0;

        /* we copy min(RF_DATALEN, resp->rf_len) bytes into block */
        new_block->valid_bytes = ntohl(resp->rf_len) <= RF_DATALEN ? ntohl(resp->rf_len) : RF_DATALEN;

        /* setup block's devnum to associate it with this remote file */
        new_block->rfl_devnum = rfptr->rfdev;

        /* setup block's lru list pointers as NULL, will be set when the block is fetched */
        new_block->lru_prev = NULL;
        new_block->lru_next = NULL;

        new_block->next = NULL;
        new_block->prev = NULL;

        /* if we are storing in response to a write, need to immediately update lru with new block */
        if (resp->rf_type == RF_MSG_WRES) {
            update_lru(new_block);
        }

        /* place node into list */
        status = insert_into_clist(rfptr, new_block);
        if (status != OK) {
            #if RFS_CACHE_DEBUG
            kprintf("Error with list insertion!\n");
            #endif
        }

        return status;
    }

    return OK;
}
#endif