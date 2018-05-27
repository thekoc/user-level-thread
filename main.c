#include <stdio.h>
#include "coroutine.h"

#define N 4096

semaphore_t sem;
void increase(void *step) {
    int stepn = *((int *) step);
    int tid = get_current_tid();
    for (int i = 0; i < 30; i += stepn) {
        if (tid == 1) {
            if (i == 20) {
                uthread_signal(&sem);
            }
        } else if (tid == 2) {
            uthread_wait(&sem);
        } else if (tid == 3) {
            uthread_wait(&sem);
        }
        printf("tid: %d - current value is %d and step is %d\n", tid, i, stepn);
        uthread_yield();
    }
}


int main() {
    int steps[] = {1, 3, 5};
    sem.value = 0;
    uthread_init();
    for (int i = 0; i < 3; i++) {
        uthread_spawn(increase, steps + i, N);
    }
    uthread_start();
}

