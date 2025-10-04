#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

typedef enum {
    TASK_COMMAND = 0,
    TASK_BLOCKED,
    TASK_RUNNING,
    TASK_STOPPED,
    TASK_TERMINATED,
} task_status_en;

typedef struct pcb_st{
    int32_t pid;
    task_status_en status;
    uint32_t time_ms;
    uint32_t ellapsed_time_ms;
    uint32_t slice_start_ms;
    uint32_t sockfd;
    int32_t priority;
} pcb_t;

typedef struct queue_elem_st queue_elem_t;
typedef struct queue_elem_st {
    pcb_t *pcb;
    queue_elem_t *next;
} queue_elem_t;

typedef struct queue_st {
    queue_elem_t* head;
    queue_elem_t* tail;
} queue_t;

pcb_t *new_pcb(int32_t pid, uint32_t sockfd, uint32_t time_ms);
int enqueue_pcb(queue_t* q, pcb_t* task);
pcb_t* dequeue_pcb(queue_t* q);
queue_elem_t *remove_queue_elem(queue_t* q, queue_elem_t* elem);

#endif