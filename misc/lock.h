/*
 * lock.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

/*
 * Implement a lock by named semaphore in POSIX
 */

#include "header.h"

typedef struct {
	char* name;
	sem_t* sem;
} lock_rec, *lock_t;

void init_lock(lock_t lock);
void acquire_lock(lock_t lock);
void release_lock(lock_t lock);
