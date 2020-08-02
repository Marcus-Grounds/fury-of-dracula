#ifndef QUEUE
#include "Places.h"
#define QUEUE

typedef struct queue *Queue;

Queue createQueue(void);
int queueIsEmpty(Queue q);
void enqueue(Queue q, PlaceId place);
PlaceId dequeue(Queue q);
void dropQueue(Queue q);

#endif