#include "sjf.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
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
        }
    }
    if (*cpu_task == NULL) {
        queue_elem_t *current = rq->head;
        queue_elem_t *shortest = NULL;
        queue_elem_t *shortest_prev = NULL;
        queue_elem_t *prev = NULL;
        uint32_t shortest_time = UINT32_MAX;

        while (current != NULL) {
            if (current->pcb->time_ms < shortest_time) {
                shortest_time = current->pcb->time_ms;
                shortest = current;
                shortest_prev = prev;
            }
            prev = current;
            current = current->next;
        }

        if (shortest != NULL) {
            if (shortest_prev) {
                shortest_prev->next = shortest->next;
            } else {
                rq->head = shortest->next;
            }
            if (shortest == rq->tail) {
                rq->tail = shortest_prev;
            }

            *cpu_task = shortest->pcb;
            free(shortest);
        }
    }
}