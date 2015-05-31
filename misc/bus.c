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
	init_lock(bus_lock);
	broadcast_finished = sem_open(BUS_SEM, O_CREAT, 0644, 3);
}

void set_bus_lock() {
	bus_lock = &bus_lock_rec;
	bus_lock->name = BUS_LOCK;
	set_lock(bus_lock);
	broadcast_finished = sem_open(BUS_SEM, O_CREAT, 0644, 3);
}
