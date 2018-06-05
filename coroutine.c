//
// Created by koc on 5/23/18.
//

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "coroutine.h"
#include "tool.h"
#include "schedule.h"

thread_node_t *current = NULL;
int total_thread_number = 0;
typedef struct {
    coroutine_t* c;
    func f;
    void* arg;
    void* old_sp;
    void* old_fp;
} start_params;

#ifdef __linux__ 

#define get_sp(p) \
    asm volatile("movq %%rsp, %0" : "=r"(p))
#define get_fp(p) \
    asm volatile("movq %%rbp, %0" : "=r"(p))
#define set_sp(p) \
    asm volatile("movq %0, %%rsp" : : "r"(p))
#define set_fp(p) \
    asm volatile("movq %0, %%rbp" : : "r"(p))

#elif _WIN32
#define get_sp(p) \
    __asm { mov rsp, p }
#define get_fp(p) \
    __asm { mov rbp, p }
#define set_sp(p) \
    __asm { mov p, rsp}
#define set_fp(p) \
    __asm { mov p, rbp }
#endif


enum { FRAME_SZ=8 }; //fairly arbitrary

// yield, next, start, this three functions are just internal implementation of coroutine_t
// and has been wrapped in uthread* functions. So you don't need to call nor modify them in
// future development.
void yield(coroutine_t* c) {
    if(!setjmp(c->callee_context)) {
        longjmp(c->caller_context, RUNNING);
    }
}

int next(coroutine_t* c) {
    int ret = setjmp(c->caller_context);
    if(!ret) {
        longjmp(c->callee_context, 1);
    }
    else {
        return ret == RUNNING;
    }
}

void start(coroutine_t* c, func f, void* arg, void *sp) {
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
    if(!setjmp(p->c->callee_context)) {
        set_sp(p->old_sp);
        set_fp(p->old_fp);
        return;
    }
    (*p->f)(p->arg);
    longjmp(p->c->caller_context, DONE);
}


time_t get_time_in_microseconds() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    time_t time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    return time_in_micros;
}

int get_current_tid() {
    return current->context.tid;
}

void uthread_yield() {
    yield(&current->context.coroutine_context);
}


void uthread_init() {
    init_scheduler();
}

void uthread_spawn(func f, void *arg, int stack_size, time_t total_time) {
    thread_node_t *node = malloc(sizeof(thread_node_t));
    thread_context_t *context = &node->context;
    context->f = f;
    context->arg = arg;
    context->state = READY;
    context->tid = ++total_thread_number;
    context->semaphore = NULL;
    context->burst_time = 0;
    context->run_time = 0;
    context->assume_full_time = total_time;
    context->left_time = context->assume_full_time;
    context->priority = USER;
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
    return 0;
}

void uthread_start() {
    current = get_next();
    while (current) {
        current->context.state = RUNNING;
        time_t start = get_time_in_microseconds();
        int ret = next(&current->context.coroutine_context);
        time_t interval = get_time_in_microseconds() - start;
        current->context.run_time += (interval);
        current->context.burst_time += 1;
        current->context.left_time -= interval;
        printf("==== in scheduling tid: %d, left  time %li\n", get_current_tid(), current->context.left_time / 1000000);
        if (!ret) {
            current->context.state = DONE;
            uthread_exit();
        } else {
            if (current->context.state == RUNNING) {
                current->context.state = READY;
            }
        }
        current = get_next();
    }
}


void uthread_suspend(int tid) {
    suspend_by_tid(tid);
}

void uthread_resume(int tid) {
    resume_by_tid(tid);
}

int uthread_wait(semaphore_t *semaphore) {
    current->context.semaphore = semaphore;
    semaphore->value--;
    if (semaphore->value < 0) {
        block_by_tid(current->context.tid);
    }
    uthread_yield();
    return 0;
}

int uthread_signal(semaphore_t *semaphore) {
    semaphore->value++;
    thread_list_t *blocked_list = get_thread_list(BLOCKED_LIST);
    thread_node_t *node = blocked_list->head->next;
    if (semaphore->value >= 0) {
        while (node) {
            if (node->context.semaphore == semaphore) {
                unblock_by_tid(node->context.tid);
            }
            node = node->next;
        }
    }
    uthread_yield();
    return 0;
}
