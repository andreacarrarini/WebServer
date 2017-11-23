//
// Created by andrea on 23/11/17.
//

#include <memory.h>
#include <structs.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <bits/sigaction.h>
#include <signal.h>
#include "structs.h"

// Used to block SIGPIPE sent from send function
void catch_signal(void) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
        error_found("Error in sigaction\n");
}

// Used to get mutex to access a memory
//  area shared by multiple execution flows
void lock(pthread_mutex_t *m) {
    if (pthread_mutex_lock(m) != 0)
        error_found("Error in pthread_mutex_lock\n");
}

// Used to release mutex
void unlock(pthread_mutex_t *m) {
    if (pthread_mutex_unlock(m) != 0)
        error_found("Error in pthread_mutex_unlock\n");
}

// Used waiting for the occurrence of an event
void wait_t(pthread_cond_t *c, pthread_mutex_t *m) {
    if (pthread_cond_wait(c, m) != 0)
        error_found("Error in pthread_cond_wait\n");
}

// Used to send a signal to a thread
void signal_t(pthread_cond_t *c) {
    if (pthread_cond_signal(c) != 0)
        error_found("Error in pthread_cond_signal\n");
}

