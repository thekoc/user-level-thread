//
// Created by koc on 5/23/18.
//

#ifndef USER_LEVEL_THREAD_TOOL_H
#define USER_LEVEL_THREAD_TOOL_H

#include "coroutine.h"

thread_list_t *create_thread_list();
int append_thread(thread_list_t *list, thread_node_t *node);
thread_node_t *pop_thread_by_tid(thread_list_t *list, int tid);
thread_node_t *pop_thread(thread_list_t *list, int pos);
thread_node_t *find_thread(thread_list_t*list,int tid);
#endif //USER_LEVEL_THREAD_TOOL_H
