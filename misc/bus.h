/*
 * bus.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#ifndef H_BUS
#define H_BUS
#include "lock.h"
#define BUS_LOCK "/bus_lock"
#define BUS_SEM "/bus_sem"
#define NUM_CLIENTS 2

#define BUS_CLIENT_COUNT 0
#define BUS_ID 1
#define BUS_TYPE 2
#define BUS_ADDR 3
#define BUS_EXITED 4

lock_rec bus_lock_rec;
lock_t bus_lock;
sem_t* broadcast_finished;

void init_bus(int*bus);
void clear_bus(int* bus);

//TODO:initialize them to 0 before using the bus
#endif
