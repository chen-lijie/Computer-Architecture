/*
 * bus.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#include "lock.h"
#define BUS_LOCK "/bus_lock"

typedef struct {
	int client_count; //number of clients max:2
	int id; //sender's id
	int type; //message type
	int addr;
	bool_t finished[2]; //are clients finished?
} bus, *bus_t;

void clear_bus(bus_t bus) {
	bus->id = bus->type = bus->addr = 0;
	for (int i = 0; i < 2; ++i) {
		bus->finished[i] = FALSE;
	}
}

lock_rec bus_lock_rec = { BUS_LOCK, NULL };
lock_t bus_lock = &bus_lock_rec;
sem_t broadcast_finished;

//TODO:initialize them to 0 before using the bus
