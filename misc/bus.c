/*
 * bus.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */
#include "bus.h"

void clear_bus(int* bus) {
	bus[BUS_ID] = bus[BUS_TYPE] = bus[BUS_ADDR] = 0;
}

void init_bus(int*bus) {
	bus[BUS_CLIENT_COUNT] = 0;

	clear_bus(bus);
	bus[BUS_EXITED] = 0;
}

void init_bus_lock() {
	bus_lock = &bus_lock_rec;
	bus_lock->name = BUS_LOCK;
	int*bus = (int*) get_bus();
	init_lock(bus_lock, bus + 8);
	broadcast_finished = sem_open(BUS_SEM, O_CREAT, 0644, 3);
	sem_init(broadcast_finished, 1, 0);
}

void set_bus_lock() {
	bus_lock = &bus_lock_rec;
	bus_lock->name = BUS_LOCK;
	int*bus = (int*) get_bus();
	set_lock(bus_lock, bus + 8);
	broadcast_finished = sem_open(BUS_SEM, O_CREAT, 0644, 3);
}
