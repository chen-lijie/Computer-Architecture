/*
 * shared_memory.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */
#ifndef H_SHARED_MEMORY
#define H_SHARED_MEMORY
#include "bus.h"
#include "cache.h"

#define SHARED_MEMORY_PATH 19950801
#define BUS_PATH 20150531

#define OWN_MEMORY_LENGTH 12
#define SHARED_MEMORY_LENGTH 12
#define SHARED_MEMORY_SIZE POW2(SHARED_MEMORY_LENGTH)
#define OWN_MEMORY_SIZE POW2(OWN_MEMORY_LENGTH)
#define SHARED_MEMORY_START_POS OWN_MEMORY_SIZE
#define BUS_SIZE 100

byte_t* get_shared_memory_phy(int k, int len);
byte_t* get_shared_memory();
byte_t* get_bus();

byte_t* bus_ptr, *shared_ptr;

#endif
