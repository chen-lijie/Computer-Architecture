/*
 * lock.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

/*
 * Implement a lock by named semaphore in POSIX
 */

#ifndef H_LOCK
#define H_LOCK
#include "header.h"
#include "isa.h"

typedef struct {
	char* name;
	sem_t* mutex;
	int* lock_holder;
} lock_rec, *lock_t;

void init_lock(lock_t lock, int*lock_holder);
void set_lock(lock_t lock, int*lock_holder);
void acquire_lock(lock_t lock, mem_t mem);
void release_lock(lock_t lock);
int get_lock_holder(lock_t lock);
bool_t is_my_lock(lock_t lock);

#endif
