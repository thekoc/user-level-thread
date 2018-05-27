//
// Created by koc on 5/27/18.
//

#include "schedule.h"
#include "tool.h"
#include "coroutine.h"

static thread_list_t *ready_list;
static thread_list_t *suspended_list;
static thread_list_t *blocked_list;
static thread_list_t *all_threads;

thread_list_t *list_table[5];


thread_list_t *get_list_by_state(thread_state state) {
    switch (state) {
        case READY:
        case RUNNING:
            return ready_list;
        case SUSPENDED:
            return suspended_list;
        case BLOCKED:
            return blocked_list;
        default:
            return NULL;
    }
}

thread_node_t *get_next(thread_list_t *list) {
    thread_node_t *next_node = NULL;
    next_node = pop_thread(list, 0);
    append_thread(list, next_node);
    return next_node;
}


int init_scheduler() {
    ready_list = create_thread_list();
    suspended_list = create_thread_list();
    blocked_list = create_thread_list();
    all_threads = create_thread_list();
    list_table[READY_LIST] = ready_list;
    list_table[SUSPENDED_LIST] = suspended_list;
    list_table[BLOCKED] = blocked_list;
    list_table[ALL_THREADS] = all_threads;
}


int append_to_list(list_type lt, thread_node_t *node) {
    if (lt != ALL_THREADS) {
        thread_node_t *new_node = malloc(sizeof(thread_node_t));
        *new_node = *node;
        new_node->next = NULL;
        append_to_list(ALL_THREADS, new_node);
    }
    return append_thread(get_thread_list(lt), node);
}

thread_node_t *pop_from_list(list_type lt, int pos) {
    if (lt != ALL_THREADS) {
        pop_from_list(ALL_THREADS, pos);
    }
    return pop_thread(get_thread_list(lt), pos);
}

thread_list_t *get_thread_list(list_type lt) {
    return list_table[lt];
}

int suspend_by_tid(int tid) {
    thread_node_t *node = pop_thread_by_tid(ready_list, tid);
    if (!node) {
        node = find_thread(blocked_list, tid);
        if (!node) {
            return -1;
        }
    }
    node->context.state = SUSPENDED;
    append_thread(suspended_list, node);
    return 0;
}

int resume_by_tid(int tid) {
    thread_node_t *node = pop_thread_by_tid(suspended_list, tid);
    if (!node) {
        node = find_thread(blocked_list, tid);
        if (!node) {
            return -1;
        } else {
            node->context.state = BLOCKED;
            return 0;
        }
    } else {
        node->context.state = READY;
        append_thread(ready_list, node);
        return 0;
    }
}

int block_by_tid(int tid) {
    thread_node_t *node = pop_thread_by_tid(ready_list, tid);
    if (!node) {
        return -1;
    }
    node->context.state = BLOCKED;
    append_thread(blocked_list, node);
}

int unblock_by_tid(int tid) {
    thread_node_t *node = pop_thread_by_tid(ready_list, tid);
    if (!node) {
        return -1;
    }
    node->context.state = READY;
    append_thread(ready_list, node);
}