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

byte_t* get_shared_memory_phy(const char*name, int len) {
	key_t key = ftok(name, 0x03);
	int shm_id = shmget(key, sizeof(int) * 100,
	IPC_CREAT | IPC_EXCL | 0600);
	if (shm_id == -1) {
		perror("get memory failed");
	}
	int*arr = (int*) shmat(shm_id, NULL, 0);
	if ((int) arr == -1) {
		perror("map memory failed");
	}
	return arr;
}

byte_t* get_shared_memory() {
	return get_shared_memory_phy(SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE);
}
bus_t get_bus() {
	return (bus_t) get_shared_memory_phy(BUS_PATH, BUS_SIZE);
}
