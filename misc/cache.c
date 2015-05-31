/*
 * cache.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#include "header.h"
#include "cache.h"

cache_t new_cache() {
	return (cache_t) calloc(1, sizeof(cache_rec));
}
