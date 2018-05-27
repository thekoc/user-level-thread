//
// Created by koc on 5/23/18.
//

#include <stdlib.h>
#include "tool.h"
#include "coroutine.h"

thread_list_t *create_thread_list() {
    thread_list_t *list = malloc(sizeof(thread_list_t));
    thread_node_t *empty_node = malloc(sizeof(thread_node_t));
    empty_node->next = NULL;
    list->head = empty_node;
    list->tail = empty_node;
    list->length = 0;
    return list;
}

int append_thread(thread_list_t *list, thread_node_t *node) {
    list->tail->next = node;
    list->tail = node;
    list->length++;
    return list->length;
}

thread_node_t *pop_thread_by_tid(thread_list_t *list, int tid) {
    thread_node_t *last = list->head;
    thread_node_t *cn = list->head->next;
    while (cn) {
        if (cn->context.tid == tid) {
            if (cn == list->tail) {
                list->tail = last;
            }
            last->next = cn->next;
            list->length--;
            cn->next = NULL;
            return cn;
        }
        last = cn;
        cn = cn->next;
    }
    return NULL;
}

thread_node_t *pop_thread(thread_list_t *list, int pos) {
    // The next pointer of the returned node will be set to NULL
    if (pos < 0) {
        return pop_thread(list, list->length + pos);
    } else if (pos >= list->length) {
        return NULL;
    } else {
        thread_node_t *current_node = list->head->next;
        thread_node_t *last = list->head;
        for (int i = 0; i < pos; i++) {
            last = current_node;
            current_node = current_node->next;
        }
        last->next = current_node->next;
        list->length--;
        if (current_node == list->tail) {
            list->tail = last;
        }
        current_node->next = NULL;
        return current_node;
    }
}

thread_node_t *find_thread(thread_list_t*list,int tid) {
    thread_node_t *cn = list->head->next;
    while (cn) {
        if (cn->context.tid == tid) {
            return cn;
        }
        cn = cn->next;
    }
    return NULL;
}
