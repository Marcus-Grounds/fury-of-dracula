
#include <stdio.h> 
#include <assert.h>
#include <stdlib.h>
#include "queue.h" 
#include "Places.h"
  

typedef struct queueNode *QueueNode;
struct queueNode {
	PlaceId place;
	QueueNode next;
};


// A structure to represent a queue 
typedef struct queue *Queue;
struct queue {
	QueueNode head;
	QueueNode tail;
	int count;
};

  
// function to create a queue 
// of given capacity. 
// It initializes size of queue as 0 
Queue createQueue(void) {
	Queue q = malloc(sizeof(struct queue));
	q->head = NULL;
	q->tail = NULL;
	q->count = 0;
	return q;
}

QueueNode newNode(PlaceId place) {
	QueueNode new = malloc(sizeof(struct queueNode));
	new->next = NULL;
	new->place = place;
	return new;
}
// Queue is full when size becomes 
// equal to the capacity 
int queueIsEmpty(Queue q) {
	return !(q->count);
}
  
PlaceId dequeue(Queue q) {
	assert(q != NULL);
	if (q->head == NULL) return NOWHERE;
	PlaceId place = q->head->place;
	QueueNode delNode = q->head;
	q->head = q->head->next;
	free(delNode);
	q->count--;
	return place;
}

// Function to add an item to the queue. 
// It changes tail and size 
void enqueue(Queue q, PlaceId place) {
	assert(q != NULL);
	QueueNode new = newNode(place);
	if (q->head == NULL) {
		q->head = new;
		q->tail = new;
	} else {
		q->tail->next = new;
		q->tail = new;
	}
	q->count++;

}
// Function to remove an item from queue. 
// It changes head and size 
void dropQueue(Queue q) {
	assert(q != NULL);
	for (QueueNode curr = q->head; curr != NULL; curr = curr->next) {
		free(curr);
	}
	free(q);
}
