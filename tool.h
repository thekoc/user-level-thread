//
// Created by koc on 5/23/18.
//

#ifndef USER_LEVEL_THREAD_TOOL_H
#define USER_LEVEL_THREAD_TOOL_H

#include "coroutine.h"

thread_list *create_thread_list();
int append_thread(thread_list *list, thread_node *node);
int delete_thread_by_tid(thread_list *list, int tid);
thread_node *pop_thread(thread_list *list, int pos);
thread_node *find_thread(thread_list*list,int tid);
#endif //USER_LEVEL_THREAD_TOOL_H
