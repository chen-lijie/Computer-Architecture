/*
 * main.c
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */
#include "bus.h"
#include "shared_memory.h"

int main() {
	my_id = 137;
	bus_ptr = get_shared_memory_phy(BUS_PATH, BUS_SIZE);
	shared_ptr = get_shared_memory_phy(SHARED_MEMORY_PATH, SHARED_MEMORY_SIZE);

	int*bus = (int*) get_bus();
	init_bus(bus);
	init_bus_lock();
	puts("Waiting for 2 clients");

	for (;;) {
		if (bus[BUS_CLIENT_COUNT] >= 1) {
			break;
		}
		usleep(33);
	}

	puts("Now, 1 clients are ready!");

	for (;;) {
		if (bus[BUS_CLIENT_COUNT] >= 2) {
			break;
		}
		usleep(33);
	}

	puts("Now, 2 clients are ready!");

	for (;;) {
		if (bus[BUS_EXITED] == 3) {
			break;
		}
		usleep(33);
	}

	puts("Now, 2 clients are finished!");
	return 0;
}
