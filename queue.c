#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

#define INPUT
#define OUTPUT

// extern atomic_int msg_cnt;

void queue_init(queue_t * q, char * buff, int len, semaphore_t * s) {
	q->wrpos = 0;
	q->rdpos = 0;
	q->buff = buff; // malloc
	q->len = len;
	q->full = 0;
	q->s = s;
}

static int queue_full(queue_t *q) {
//	int new_wrpos = (q->wrpos == q->len - 1) ? 0 : 
q->wrpos + 1;
//	return (new_wrpos == q->rdpos);
	return q->full;
}

static int queue_empty(queue_t *q) {
	return !q->full && (q->wrpos == q->rdpos);
}

void queue_put_b(queue_t * q, INPUT char * data, int len) {
	int put_cnt = 0;
	do {
		int x;
		x = queue_put(q, data + put_cnt, len-put_cnt);					
		put_cnt += x;
	} while (put_cnt != len);
}

int queue_put(queue_t * q, INPUT char * data, int len) {
	int ret = 0;
	
	// msg_cnt++;

	P(q->s);

	while (!queue_full(q)) {
		if (ret<len) {

			q->buff[q->wrpos] = data[ret++];
			q->wrpos = (q->wrpos == q->len - 1) ? 0 : q->wrpos + 1;
			if (q->rdpos == q->wrpos) {
				q->full = 1;
			}	
				
		} else {
			break;
		}
	}

	V(q->s);	

	return ret;
}

void queue_get_b(queue_t * q, OUTPUT char * data, int len) {
	while (queue_available(q) < len);
	queue_get(q, data, len);
}

int queue_get(queue_t * q, OUTPUT char * data, int len) {
	int ret = 0;
	
	P(q->s);

	while (!queue_empty(q)) {
		if (ret<len) {
            data[ret++] = q->buff[q->rdpos];
            q->rdpos = (q->rdpos == q->len - 1) ? 0 : q->rdpos + 1;
			q->full = 0;
        } else {
        	break;
    	}
	}

	V(q->s);

	return ret;
}

int queue_free(queue_t * q) {
	int ret = 0;
	if (q->rdpos <= q->wrpos) {
		ret = q->rdpos + q->len - q->wrpos;
	// } else if(q->rdpos == q->wrpos){
	// 	ret = 0;
	} else {
		ret = q->rdpos - q->wrpos;
	}
	return ret;
}

int queue_available(queue_t * q) {
	int ret = q->len - queue_free(q);
	return ret;
}

char queue_front(queue_t * q) {
	char ret = -1;
	if (!queue_empty(q)) {
		ret = q->buff[q->rdpos];
	}
	return ret;
}

char queue_back(queue_t * q) {
	char ret = -1;
	if (!queue_empty(q)) {
		int new_wrpos = (q->wrpos == 0) ? q->len - 1 : q->wrpos - 1;
		ret = q->buff[new_wrpos];
	}
	return ret;
}
