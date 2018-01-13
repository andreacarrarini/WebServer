//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

// Used to block SIGPIPE sent from send function
void catch_signal(void) {

    fprintf(stderr, "catch_signal\n");

    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
        error_found("Error in sigaction\n");
}

// Used to get mutex to access a memory
//  area shared by multiple execution flows
void lock(pthread_mutex_t *m) {

    fprintf(stderr, "lock\n");

    if (pthread_mutex_lock(m) != 0)
        error_found("Error in pthread_mutex_lock\n");
}

// Used to release mutex
void unlock(pthread_mutex_t *m) {

    fprintf(stderr, "unlock\n");

    if (pthread_mutex_unlock(m) != 0)
        error_found("Error in pthread_mutex_unlock\n");
}

// Used waiting for the occurrence of an event
void wait_t(pthread_cond_t *c, pthread_mutex_t *m) {

    fprintf(stderr, "wait_t\n");

    if (pthread_cond_wait(c, m) != 0)
        error_found("Error in pthread_cond_wait\n");
}

// Used to send a signal to a thread
void signal_t(pthread_cond_t *c) {

    fprintf(stderr, "signal_t: 1\n");

    if (pthread_cond_signal(c) != 0)
        error_found("Error in pthread_cond_signal\n");

    fprintf(stderr, "signal_t: END\n");
}

// Used by kill_th function
void for_kill(int n_th, struct th_sync *k) {

    fprintf(stderr, "for_kill\n");

    int i, j;

    for (i = j = 0; i < n_th && j < MAXCONN; ++j)
        if (k -> clients[j] == -1) {
            k -> clients[j] = -2;
            signal_t(k -> threads_cond_list + j);
            ++i;
        }

    // If there are not enough threads ready (with -1 flag)
    if (i < n_th)
        k -> to_kill = n_th - i;
}

// Used to kill threads
void kill_th(struct th_sync *k) {

    fprintf(stderr, "kill_th\n");

    int n_th = 0;

    if (k -> th_act_thr > MINTH) {
        int old_thr = (k -> th_act_thr - MINTH / 2) * 2 / 3;
        // To bring system to the default thread count
        if (!k -> connections) {
            k -> th_act_thr = MINTH;
            n_th = k -> th_act - k -> th_act_thr;
            k -> to_kill = 0;
            for_kill(n_th, k);
            return;
        } else if (k -> connections < old_thr) {
            // Gradual deallocation
            if (k -> th_act_thr == MAXCONN) {
                if (!(n_th = (MAXCONN - MINTH) % (MINTH / 2)))
                    n_th = MINTH / 2;
            } else {
                n_th = MINTH / 2;
            }
            k -> th_act_thr -= n_th;
        }
    }

    if (k -> to_kill) {
        n_th += k -> to_kill;
        k -> to_kill = 0;
    }

    for_kill(n_th, k);
}

