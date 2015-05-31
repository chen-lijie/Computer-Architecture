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
#include "lock.h"

#define cerr(...) fprintf(stderr, __VA_ARGS__)

void init_lock(lock_t lock) {
	lock->sem = sem_open(lock->name, O_CREAT, 0644, 3);
	if (lock->sem == 0) {
		cerr("Error when initialize lock %s\n", lock->name);
	}
	sem_post(lock->sem);
}

void set_lock(lock_t lock) {
	lock->sem = sem_open(lock->name, O_CREAT, 0644, 3);
	if (lock->sem == 0) {
		cerr("Error when initialize lock %s\n", lock->name);
	}
}

void acquire_lock(lock_t lock) {
	sem_wait(lock->sem);
}
void release_lock(lock_t lock) {
	sem_post(lock->sem);
}
