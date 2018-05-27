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

int init_scheduler(void);
thread_node_t *get_next(thread_list_t *list);
int append_to_list(list_type lt, thread_node_t *node);
thread_node_t *pop_from_list(list_type lt, int pos);
int suspend_by_tid(int tid);
int resume_by_tid(int tid);
int block_by_tid(int tid);
int unblock_by_tid(int tid);
thread_list_t *get_thread_list(list_type lt);

#endif //USER_LEVEL_THREAD_THREAD_LIST_H
