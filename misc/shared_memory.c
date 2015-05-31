/*
 * shared_memory.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#include "header.h"
#include "isa.h"
#include "cache.h"
#include "shared_memory.h"
#include "bus.h"

byte_t* get_shared_memory_phy(int k, int len) {
	key_t key = k;
	int shm_id = shmget(key, len, IPC_CREAT | 0600);
//	fprintf(stderr, "HI");
	if (shm_id == -1) {
		perror("get memory failed");
	}
	byte_t*arr = (byte_t*) shmat(shm_id, NULL, 0);

//	int i = 0;
//	for (i = 0; i < 10; i++) {
//		printf("%d\n", arr[i]);
//	}

	if ((int) arr == -1) {
		perror("map memory failed");
	}
//	fprintf(stderr, "HI");
	return arr;
}

byte_t* get_shared_memory() {
	return shared_ptr;
}
byte_t* get_bus() {
	return bus_ptr;
}
