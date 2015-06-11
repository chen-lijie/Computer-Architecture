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
	byte_t*shared = get_shared_memory();
	int*bus = (int*) get_bus();

	//Init the shared
	memset(shared, 0, SHARED_MEMORY_SIZE);

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

	int last = shared[16];
	int counter = 0;

	for (;;) {
		if (bus[BUS_EXITED] == 3) {
			break;
		}
		usleep(33);
		int get = shared[16];
		while (get != last) {
			fprintf(stderr, "Switch #%d: Lock switch from %d %d\n", counter,
					last, get);
			++counter;
			last = get;
		}
	}

	puts("Now, 2 clients are finished!");

	// Now, print any non-zero position in shared.

	int i;
	for (i = 0; i < SHARED_MEMORY_SIZE; i += 4) {
		int pos = OWN_MEMORY_SIZE + i;
		int val = *(int*) (shared + i);
		if (val != 0) {
			printf("%0x: %d\n", pos, val);
		}
	}
	return 0;
}
