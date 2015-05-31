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
#include "isa.h"

#define cerr(...) fprintf(stderr, __VA_ARGS__)

void init_lock(lock_t lock, int*lock_holder) {
	lock->mutex = sem_open(lock->name, O_CREAT, 0644, 3);
	if (lock->mutex == 0) {
		cerr("Error when initialize lock %s\n", lock->name);
	}
	lock->lock_holder = lock_holder;
	*(lock->lock_holder) = -1;
	sem_post(lock->mutex);
}

void set_lock(lock_t lock, int*lock_holder) {
	lock->mutex = sem_open(lock->name, O_CREAT, 0644, 3);
	lock->lock_holder = lock_holder;
	if (lock->mutex == 0) {
		cerr("Error when initialize lock %s\n", lock->name);
	}
}

void acquire_lock(lock_t lock, mem_t mem) {
//	fprintf(stderr, "%d %d\n", my_id, *(lock->lock_holder));
	for (;;) {
		sem_wait(lock->mutex);
		if (*(lock->lock_holder) == -1) {
			break;
		}
		if (mem != NULL)
			response(mem);
		sem_post(lock->mutex);
		usleep(3);
	}
	*(lock->lock_holder) = my_id;
	sem_post(lock->mutex);
}
void release_lock(lock_t lock) {
	sem_wait(lock->mutex);
	assert(*(lock->lock_holder) == my_id);
	*(lock->lock_holder) = -1;
	sem_post(lock->mutex);
}

int get_lock_holder(lock_t lock) {
	int ret;
	sem_wait(lock->mutex);
	ret = *(lock->lock_holder);
	sem_post(lock->mutex);
	return ret;
}

bool_t is_my_lock(lock_t lock) {
	return get_lock_holder(lock) == my_id;
}
