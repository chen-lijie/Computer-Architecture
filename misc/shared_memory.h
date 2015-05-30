/*
 * shared_memory.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#include "header.h"
#include "isa.h"
#include "cache.h"

#define SHARED_MEMORY_PATH "/tmp/CLJ/shared"
#define BUS_PATH "/tmp/CLJ/bus"

#define OWN_MEMORY_LENGTH 12
#define SHARED_MEMORY_LENGTH 12
#define SHARED_MEMORY_SIZE POW2(SHARED_MEMORY_LENGTH)
#define OWN_MEMORY_SIZE POW2(OWN_MEMORY_LENGTH)
#define SHARED_MEMORY_START_POS OWN_MEMORY_SIZE
#define BUS_SIZE 100

byte_t* get_shared_memory_phy(const char*name, int len);
byte_t* get_shared_memory();
byte_t* get_bus();
