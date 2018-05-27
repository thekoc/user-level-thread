//
// Created by koc on 5/23/18.
//

#ifndef USER_LEVEL_THREAD_COROUTINE_H
#define USER_LEVEL_THREAD_COROUTINE_H

#include <setjmp.h>


typedef enum { RUNNING=1, READY, SUSPENDED, DONE } thread_state;
typedef void (*func)(void*);

typedef struct {
    jmp_buf callee_context;
    jmp_buf caller_context;
} coroutine;

typedef struct {
    int tid;
    thread_state state;
    coroutine coroutine_context;
    func f;
    void *arg;
    void *sp;
} thread_context;

typedef struct thread_node {
    thread_context context;
    struct thread_node *next;
} thread_node;

typedef struct {
    thread_node *head;
    thread_node *tail;
    int length;
} thread_list;


void uthread_init();
void uthread_spawn(func f, void *arg, int stack_size);
void uthread_start();
void uthread_yield();

#endif //USER_LEVEL_THREAD_COROUTINE_H
