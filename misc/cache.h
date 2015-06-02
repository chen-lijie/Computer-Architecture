/*
 * cache.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#ifndef H_CACHE
#define H_CACHE

#include "header.h"

/*
 * Some numeric values about the catch
 */

#define POW2(LEN) (1<<(LEN))
#define MASK2(LEN) (POW2(LEN) - 1)
#define MEMORY_LENGTH 13
#define MEMORY_SIZE POW2(MEMORY_LENGTH)
#define BLOCK_LENGTH 4
#define BLOCK_SZIE POW2(BLOCK_LENGTH)
#define SET_LENGTH 3
#define NUM_WAYS 8
#define TAG_LENGTH (MEMORY_LENGTH - BLOCK_LENGTH - SET_LENGTH)
#define BLOCK_START(TAG,SET) ((((TAG)<<(SET_LENGTH))+SET)<<BLOCK_LENGTH)

/*
 * addr splits as TAG/SET/BLOCK
 * Macros for splitting
 */
#define GET(val,start,len) ((val)>>(start)&MASK2((len)))
#define GET_ADDR_BLOCK(addr) GET(addr,0,BLOCK_LENGTH)
#define GET_ADDR_SET(addr) GET(addr,BLOCK_LENGTH,SET_LENGTH)
#define GET_ADDR_TAG(addr) GET(addr,BLOCK_LENGTH+SET_LENGTH,TAG_LENGTH)

typedef struct {
	bool_t valid, dirty;
	int tag;
	byte_t contents[BLOCK_SZIE];
} cache_blk_rec, *cache_blk_t;

//NUM_WAYS-way catch
typedef struct {
	cache_blk_rec blks[POW2(SET_LENGTH)][NUM_WAYS];
} cache_rec, *cache_t;

cache_t new_cache();
#endif
