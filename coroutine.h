//
// Created by koc on 5/23/18.
//

#ifndef USER_LEVEL_THREAD_COROUTINE_H
#define USER_LEVEL_THREAD_COROUTINE_H

#include <setjmp.h>


typedef enum { RUNNING=1, READY, SUSPENDED, BLOCKED, DONE } thread_state;
typedef void (*func)(void*);

typedef struct {
    jmp_buf callee_context;
    jmp_buf caller_context;
} coroutine_t;

typedef struct {
    int tid;
    thread_state state;
    coroutine_t coroutine_context;
    func f;
    void *arg;
    void *sp;
} thread_context_t;

typedef struct thread_node_t {
    thread_context_t context;
    struct thread_node_t *next;
} thread_node_t;

typedef struct {
    thread_node_t *head;
    thread_node_t *tail;
    int length;
} thread_list_t;


void uthread_init();
void uthread_spawn(func f, void *arg, int stack_size);
void uthread_start();
void uthread_yield();
void uthread_suspend(int tid);
void uthread_resume(int tid);
int get_current_tid();

#endif //USER_LEVEL_THREAD_COROUTINE_H
