//
// Created by koc on 5/23/18.
//

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "coroutine.h"
#include "tool.h"
#include "schedule.h"

thread_node *current = NULL;
int total_thread_number = 0;
typedef struct {
    coroutine* c;
    func f;
    void* arg;
    void* old_sp;
    void* old_fp;
} start_params;

#define get_sp(p) \
  asm volatile("movq %%rsp, %0" : "=r"(p))
#define get_fp(p) \
  asm volatile("movq %%rbp, %0" : "=r"(p))
#define set_sp(p) \
  asm volatile("movq %0, %%rsp" : : "r"(p))
#define set_fp(p) \
  asm volatile("movq %0, %%rbp" : : "r"(p))

enum { FRAME_SZ=8 }; //fairly arbitrary

// yield, next, start, this three functions are just internal implementation of coroutine
// and has been wrapped in uthread* functions. So you don't need to call nor modify them in
// future development.
void yield(coroutine* c) {
    if(!setjmp(c->callee_context)) {
        longjmp(c->caller_context, RUNNING);
    }
}

int next(coroutine* c) {
    int ret = setjmp(c->caller_context);
    if(!ret) {
        longjmp(c->callee_context, 1);
    }
    else {
        return ret == RUNNING;
    }
}

void start(coroutine* c, func f, void* arg, void *sp) {
    start_params* p = ((start_params*)sp) - 1;

    //save params before stack switching
    p->c = c;
    p->f = f;
    p->arg = arg;

    get_sp(p->old_sp);
    get_fp(p->old_fp);

    set_sp(p - FRAME_SZ);
    set_fp(p); //effectively clobbers p
    //(and all other locals)
    get_fp(p); //…so we read p back from $fp

    //and now we read our params from p
    p->c;
    if(!setjmp(p->c->callee_context)) {
        set_sp(p->old_sp);
        set_fp(p->old_fp);
        return;
    }
    (*p->f)(p->arg);
    longjmp(p->c->caller_context, DONE);
}

// Functions below needn't to be called nor modified throughout the later development.


int get_current_tid() {
    return current->context.tid;
}

void uthread_yield() {
    yield(&current->context.coroutine_context);
}


void uthread_init() {
    init_scheduler();
}

void uthread_spawn(func f, void *arg, int stack_size) {
    thread_node *node = malloc(sizeof(thread_node));
    node->context.f = f;
    node->context.arg = arg;
    node->context.state = READY;
    node->context.tid = ++total_thread_number;
    node->next = NULL;
    append_to_list(READY_LIST, node);
    size_t malloced_size = stack_size * sizeof(char);
    char *sp = (char *) malloc(malloced_size);
    node->context.sp = sp;
    sp = sp + malloced_size;
    start(&node->context.coroutine_context, f, arg, sp);
}

int uthread_exit() {
    pop_from_list(READY_LIST, -1);
    free(current);
}

void uthread_start() {
    current = get_next(get_thread_list(READY_LIST));
    while (current) {
        current->context.state = RUNNING;
        int ret = next(&current->context.coroutine_context);
        if (!ret) {
            current->context.state = DONE;
            uthread_exit();
        } else {
            current->context.state = READY;
        }
        current = get_next(get_thread_list(READY_LIST));
    }
}


void uthread_suspend(int tid) {
    suspend_by_tid(tid);
}

void uthread_resume(int tid) {
    resume_by_tid(tid);
}
