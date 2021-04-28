# Remote File System Caching in Xinu
## Description
This branch contains additions and improvements to the base remote file system provided in the Galileo version of Xinu. The largest of these changes is the inclusion of local caching, where users have the ability to allow Xinu to create and maintain a cache to store retrieved file data from the remote file server.

## Files
1. The new-rfserver directory contains an updated implementation of the remote file server that communicates with the Xinu client. Most of the changes from the original implementation are designed to return additional file metadata in networking responses to facilitate caching.
2. The xinu directory contains updated xinu source code for the Galileo backend with local caching and other small improvements implemented. Below is a description of the major files added and their purposes:
    - device/rfs/rfs_cache.c: contains code related to fetching and storing file data from the cache while maintaining various data structures
    - include/rfs_cache.h: contains definitions for various constants and data structures used to create and maintain the rfs cache
    - system/rfs_tests.c: contains various tests that were used to test the remote file system and its cache over the course of building the cache. The most useful of these would be *rfs_cache_test* function, which runs an all-encompassing test of the rfs cache and returns a time value used to evaluate performance.
    - In addition to these, some already-present files in device/rfs, such as rflread.c and rflwrite.c, were modified to add cached versions of the corresponding functions

## How to use the RFS Cache
1. There are a few preprocessor constants in include/rfs_cache.h that must be set to utilize the RFS Cache prior to compiling the xinu source code.
2. To utilize the remote file system cache, the RFS_CACHING_ENABLED flag must be set to 1. Otherwise, the caching data structures and functions will not be compiled, and the default implementation of the remote file system will be used.
3. If the RFS_CACHE_DEBUG flag is set to 1, various debugging information will be printed when utilizing the rfs cache.
4. The rfs has two portions, an array portion and a linked list portion. The array portion allows a certain number of cache blocks to be fetched in constant time, while the linked list portion provides a dynamically-size cached whose blocks can be fetched in linear time. The MAX_RFS_CBLOCKS constant determines the number of blocks that can be accessed via direct indexing (each cache block can hold up to 1024 bytes of file data). For example, if MAX_RFS_CBLOCKS is set to 5, then the first 5120 (1024 * 5) bytes of file data can be accessed in constant time. Blocks containing bytes beyond this will be added to the linked list portion of the cache. For performance, it is recommended that MAX_RFS_CBLOCKS is set to a large value, as that will ensure the majority of cache blocks are in the array portion, and thus can be fetched in constant time. 
5. The constant MAX_CBLOCKS_ALLOCABLE determines the maximum number of cache blocks that can exist in memory at any given time. If this limit is reached and another block of data gets cached, a currently-allocated block will be de-allocated, with eviction based on a least recently used (LRU) basis. Like MAX_RFS_CBLOCKS, it is recommended to set MAX_CBLOCKS_ALLOCABLE to a large value.
6. Once these constants have been set up as desired and if caching is enabled, any read() and write() operations conducted on a remote file psuedodevice will be performed with the Xinu client caching file data requested over the network.

## Performance
Some test results of the cache's performance can be found here: https://docs.google.com/document/d/1y47y16yz3daPWNkbycYcKHH_TzWcvxRNoHLUZYM6E9c/edit?usp=sharing

## Author
These improvements and the addition of a local cache to Xinu's remote file system were implemented by Anderson David, as part of a research project under Professor Douglas Comer at Purdue University in Spring 2021.