#include "mlfq.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

#define TIME_SLICE_MS 500
#define NUM_QUEUES 3
#define PRIORITY_BOOST_TIME 10000

static queue_t priority_queues[NUM_QUEUES];
static int priority_boost_timer = 0;
static int initialized = 0;

static void init_mlfq() {
    if (!initialized) {
        for (int i = 0; i < NUM_QUEUES; i++) {
            priority_queues[i].head = NULL;
            priority_queues[i].tail = NULL;
        }
        priority_boost_timer = 0;
        initialized = 1;
    }
}

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    init_mlfq();

    pcb_t *new_task;
    while ((new_task = dequeue_pcb(rq)) != NULL) {
        new_task->priority = 0;
        enqueue_pcb(&priority_queues[0], new_task);
    }

    priority_boost_timer += TICKS_MS;
    if (priority_boost_timer >= PRIORITY_BOOST_TIME) {
        priority_boost_timer = 0;

        for (int i = 1; i < NUM_QUEUES; i++) {
            pcb_t *task;
            while ((task = dequeue_pcb(&priority_queues[i])) != NULL) {
                task->priority = 0;
                enqueue_pcb(&priority_queues[0], task);
            }
        }
    }

    if (*cpu_task) {
        (*cpu_task)->ellapsed_time_ms += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            free((*cpu_task));
            (*cpu_task) = NULL;
        } else {
            uint32_t time_running = current_time_ms - (*cpu_task)->slice_start_ms;
            if (time_running >= TIME_SLICE_MS) {
                int current_priority = (*cpu_task)->priority;
                int new_priority = (current_priority + 1 < NUM_QUEUES) ? current_priority + 1 : NUM_QUEUES - 1;

                (*cpu_task)->priority = new_priority;
                (*cpu_task)->slice_start_ms = 0;
                enqueue_pcb(&priority_queues[new_priority], *cpu_task);
                *cpu_task = NULL;
            }
        }
    }

    if (*cpu_task == NULL) {
        for (int i = 0; i < NUM_QUEUES; i++) {
            if (priority_queues[i].head != NULL) {
                *cpu_task = dequeue_pcb(&priority_queues[i]);
                if (*cpu_task != NULL) {
                    (*cpu_task)->slice_start_ms = current_time_ms;
                    break;
                }
            }
        }
    }
}