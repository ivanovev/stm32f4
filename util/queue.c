
#include "stdint.h"
#include "queue.h"

int enqueue(struct Queue *q, uint8_t data)
{
    q->q[q->head] = data;
    q->head = ((q->head + 1) == IO_BUF_SZ) ? 0 : q->head + 1;
    return 1;
}

int dequeue(struct Queue *q, uint8_t *buf, uint8_t newline)
{
    if(q->head == q->tail)
        return 0;
    int len = 0;
    uint32_t qptr = q->tail;
    while(1)
    {   
        qptr = (q->tail + len) % IO_BUF_SZ;
        if(qptr == q->head)
        {   
            if(newline)
                len = 0;
            else
                q->tail = q->head;
            break;
        }
        if(newline)
        {
            if(q->q[qptr] == '\r')
                continue;
            if(q->q[qptr] == '\n')
            {
                buf[len++] = '\n';
                buf[len++] = '\r';
                q->tail = q->head;
                break;
            }
        }
        buf[len++] = q->q[qptr];
    }
    buf[len] = 0;
    return len;
}

