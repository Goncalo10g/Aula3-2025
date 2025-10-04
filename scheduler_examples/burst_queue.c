#include <ctype.h>
#include <stdint.h>

#include "burst_queue.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

int parse_burst_line(const char* line, burst_t* burst) {
    if (!line || !burst) return -1;

    burst->burst_time_ms = 0;
    burst->block_time_ms = 0;
    burst->nice = 0;
    burst->pages.count = 0;

    int burst_time, block_time;
    int parsed = sscanf(line, "%d,%d", &burst_time, &block_time);

    if (parsed < 1) {
        fprintf(stderr, "Failed to parse line: %s\n", line);
        return -1;
    }

    if (burst_time < 0 || burst_time > INT_MAX) {
        fprintf(stderr, "Invalid burst time: %d\n", burst_time);
        return -1;
    }

    burst->burst_time_ms = burst_time;

    if (parsed >= 2) {
        if (block_time < 0 || block_time > INT_MAX) {
            fprintf(stderr, "Invalid block time: %d\n", block_time);
            return -1;
        }
        burst->block_time_ms = block_time;
    }

    return 0;
}


int read_queue_from_file(burst_queue_t* queue, const char* filename) {
    if (!queue || !filename) return -1;

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return -1;
    }

    char line[MAX_LINE_LEN];
    int success_count = 0;

    while (fgets(line, sizeof(line), file)) {
        char* trimmed = line;
        while (isspace(*trimmed)) ++trimmed;

        if (*trimmed == '\0' || *trimmed == '#' || *trimmed == '\n' || *trimmed == '\r') continue;

        burst_t burst = {0};
        if (parse_burst_line(trimmed, &burst) == 0) {
            if (enqueue_burst(queue, &burst)) {
                success_count++;
            } else {
                fprintf(stderr, "Queue full or allocation failed\n");
                break;
            }
        } else {
            fprintf(stderr, "Skipping malformed line: %s", line);
        }
    }

    fclose(file);
    return success_count;
}


int enqueue_burst(burst_queue_t* q, const burst_t* burst) {
    burst_node_t* node = malloc(sizeof(burst_node_t));
    if (!node) return 0;

    node->burst = malloc(sizeof(burst_t));
    if (!node->burst) {
        free(node);
        return 0;
    }

    *node->burst = *burst;
    node->next = NULL;

    if (q->tail) {
        q->tail->next = node;
    } else {
        q->head = node;
    }
    q->tail = node;
    return 1;
}

burst_t* dequeue_burst(burst_queue_t* q) {
    if (!q || !q->head) return NULL;

    burst_node_t* node = q->head;
    burst_t* result = node->burst;

    q->head = node->next;
    if (!q->head)
        q->tail = NULL;

    free(node);
    return result;
}