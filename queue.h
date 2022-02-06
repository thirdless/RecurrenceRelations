#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "semaphore.h"

#define QUEUE_LEN	32

typedef struct queue {
	char *buff;
	int wrpos;
	int rdpos;
	int len;
	int full;
	semaphore_t * s;
} queue_t;

void queue_init(queue_t * q, char * buffx, int len, semaphore_t * s);
int queue_put(queue_t * q, char * data, int len);
void queue_put_b(queue_t * q, char * data, int len);

int queue_get(queue_t * q, char * data, int len);
void queue_get_b(queue_t * q, char * data, int len);

int queue_free(queue_t * q);
int queue_available(queue_t * q);
char queue_front(queue_t * q);
char queue_back(queue_t * q); 

#endif