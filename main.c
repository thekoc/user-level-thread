#include <stdio.h>
#include "coroutine.h"

#define N 4096
void increase(void *step) {
    int stepn = *((int *) step);
    for (int i = 0; i < 30; i += stepn) {
        if (get_current_tid() == 1) {
            if (i < 20) {
                uthread_suspend(2);
            } else {
                uthread_resume(2);
            }
        }
        printf("tid: %d: current value is %d and step is %d\n", get_current_tid(), i, stepn);
        uthread_yield();
    }
}


int main() {
    int steps[] = {2, 3, 5};
    uthread_init();
    for (int i = 0; i < 3; i++) {
        uthread_spawn(increase, steps + i, N);
    }
    uthread_start();
}

