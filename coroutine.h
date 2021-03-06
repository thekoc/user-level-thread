//
// Created by koc on 5/23/18.
//

#ifndef USER_LEVEL_THREAD_COROUTINE_H
#define USER_LEVEL_THREAD_COROUTINE_H

#include <setjmp.h>
#include <sys/time.h>

#define SECOND 1000000

typedef enum { RUNNING=1, READY, SUSPENDED, BLOCKED, DONE } thread_state;
typedef void (*func)(void*);

typedef struct {
    jmp_buf callee_context;
    jmp_buf caller_context;
} coroutine_t;

typedef struct {
    int value;
} semaphore_t;

typedef enum { USER = 1, SYS, PROTECTED} thread_priority;

typedef struct {
    int tid;
    thread_state state;
    coroutine_t coroutine_context;
    func f;
    void *arg;
    void *sp;
    semaphore_t *semaphore;

    // properties for scheduling
    thread_priority priority;
    time_t run_time;           // total time the thread has run
    time_t assume_full_time;   // used in SRT schedule algorithm
    time_t left_time;          // equals assume_full_time minus run_time
    double burst_time;         // indicates how many times it been scheduled
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

// - User interfaces -

// Call it before using this library.
void uthread_init();

// Spawn a thread which will start after `uthread_start`.
void uthread_spawn(func f, void *arg, int stack_size, time_t total_time);

// Start to run all spawned threads. Returns if every thread is done or killed.
void uthread_start();

// Called by thread function itself. After calling, the caller will paused until the scheduler picked it again.
void uthread_yield();

// Suspend a thread by id.
void uthread_suspend(int tid);

// Resume a thread by id.
void uthread_resume(int tid);

// The P operation to the semaphore.
int uthread_wait(semaphore_t *semaphore);

// The V operation to the semaphore.
int uthread_signal(semaphore_t *semaphore);

// A self-explanatory function.
int get_current_tid();

#endif //USER_LEVEL_THREAD_COROUTINE_H
