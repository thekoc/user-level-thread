//
// Created by koc on 5/27/18.
//

#ifndef USER_LEVEL_THREAD_THREAD_LIST_H
#define USER_LEVEL_THREAD_THREAD_LIST_H

#include "coroutine.h"
#include <stdlib.h>
typedef enum {
    READY_LIST = 1,
    SUSPENDED_LIST,
    BLOCKED_LIST,
    ALL_THREADS,
} list_type;

// Call this function before any scheduling.
int init_scheduler(void);

// Returns the next scheduled thread.
thread_node_t *get_next();

// Append the node at the end of given list_type.
int append_to_list(list_type lt, thread_node_t *node);

thread_node_t *pop_from_list(list_type lt, int pos);

// Add the node to the suspended_list and remove it from ready_list if it is in the ready_list.
int suspend_by_tid(int tid);

// Remove the node from the suspended_list and add it to ready_list if it is not blocked.
int resume_by_tid(int tid);

// Add the node to the blocked list and remove if from ready_list.
int block_by_tid(int tid);
int unblock_by_tid(int tid);

// Returns the corresponding list depends on list_type.
thread_list_t *get_thread_list(list_type lt);

#endif //USER_LEVEL_THREAD_THREAD_LIST_H
