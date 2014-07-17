
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "util/util.h"

typedef struct Queue {
    volatile uint16_t head, tail;
    uint8_t q[IO_BUF_SZ];
} Queue;

int enqueue(struct Queue *q, uint8_t data);
int dequeue(struct Queue *q, uint8_t *buf, uint8_t newline);

#endif

