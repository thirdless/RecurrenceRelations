
#include "message.h"

void message_send(queue_t *q, int * src_id, int * dst_id, int * len, char * msg_data) {
    message_t msg = {*src_id, *dst_id, *len};

    queue_put_b(q, (char*)&msg, sizeof(message_t));
    
    queue_put_b(q, msg_data, *len);
}

void message_recv(queue_t *q, int * src_id, int * dst_id, int * len, char * msg_data) {
    
    message_t msg;

    while (queue_available(q) < sizeof(msg));

    queue_get(q, (char*)&msg, sizeof(message_t));

    while (queue_available(q) < msg.len);

    queue_get(q, msg_data, msg.len);

    *src_id = msg.src_id;
    *dst_id = msg.dst_id;
    *len = msg.len;
}
