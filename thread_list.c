//
// Created by koc on 5/27/18.
//

#include "thread_list.h"
#include "tool.h"
#include "coroutine.h"

thread_list *ready_list;
thread_list *suspended_list;
thread_list *all_threads;

thread_list *list_table[4];


thread_list *get_list_by_state(thread_state state) {
    switch (state) {
        case READY:
        case RUNNING:
            return ready_list;
        case SUSPENDED:
            return suspended_list;
        default:
            return NULL;
    }
}


int init_thread_lists() {
    ready_list = create_thread_list();
    suspended_list = create_thread_list();
    all_threads = create_thread_list();
    list_table[READY_LIST] = ready_list;
    list_table[SUSPENDED_LIST] = suspended_list;
    list_table[ALL_THREADS] = all_threads;
}


int append_to_list(list_type lt, thread_node *node) {
    if (lt != ALL_THREADS) {
        thread_node *new_node = malloc(sizeof(thread_node));
        *new_node = *node;
        append_to_list(ALL_THREADS, new_node);
    }
    return append_thread(get_thread_list(lt), node);
}

thread_node *pop_from_list(list_type lt, int pos) {
    if (lt != ALL_THREADS) {
        pop_from_list(ALL_THREADS, pos);
    }
    return pop_thread(get_thread_list(lt), pos);
}

thread_list *get_thread_list(list_type lt) {
    return list_table[lt];
}

int suspend_by_tid(int tid) {
    thread_node *node = find_thread(ready_list, tid);
    if (!node) {
        return -1;
    }
    node->context.state = SUSPENDED;
    delete_thread_by_tid(ready_list, tid);
    append_thread(suspended_list, node);
}