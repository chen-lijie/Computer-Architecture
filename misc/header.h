/*
 * header.h
 *
 *  Created on: May 30, 2015
 *      Author: wjmzbmr
 */

#ifndef H_HEADER
#define H_HEADER
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

/**************** Truth Values **************/
typedef enum {
	FALSE, TRUE
} bool_t;

/***********  Implementation of Memory *****************/
typedef unsigned char byte_t;
typedef int word_t;
#endif
