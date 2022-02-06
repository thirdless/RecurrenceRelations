
#include "queue.h"

typedef struct {
    int src_id;
    int dst_id;
    int len;
} message_t;

void message_send(queue_t *q, int * src_id, int * dst_id, int * len, char * msg_data);

void message_recv(queue_t *q, int * src_id, int * dst_id, int * len, char * msg_data);
