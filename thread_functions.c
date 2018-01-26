//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

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

// Used by kill_th function
void for_kill(int n_th, struct threads_sync_struct *k) {

    int i, j;

    for (i = j = 0; i < n_th && j < MAX_CONNECTION; ++j)
        if (k -> client_socket_list[j] == -1) {
            k -> client_socket_list[j] = -2;
            signal_t(k -> threads_cond_list + j);
            ++i;
        }

    // If there are not enough threads ready (with -1 flag)
    if (i < n_th)
        k -> threads_to_kill = n_th - i;
}

// Used to kill threads
void kill_th(struct threads_sync_struct *k) {

    int n_th = 0;

    if (k -> min_active_threads_treshold > MIN_THREAD_TRESHOLD) {
        int old_thr = (k -> min_active_threads_treshold - MIN_THREAD_TRESHOLD / 2) * 2 / 3;
        // To bring system to the default thread count
        if (!k -> connections) {
            k -> min_active_threads_treshold = MIN_THREAD_TRESHOLD;
            n_th = k -> active_threads - k -> min_active_threads_treshold;
            k -> threads_to_kill = 0;
            for_kill(n_th, k);
            return;
        } else if (k -> connections < old_thr) {
            // Gradual deallocation
            if (k -> min_active_threads_treshold == MAX_CONNECTION) {
                if (!(n_th = (MAX_CONNECTION - MIN_THREAD_TRESHOLD) % (MIN_THREAD_TRESHOLD / 2)))
                    n_th = MIN_THREAD_TRESHOLD / 2;
            } else {
                n_th = MIN_THREAD_TRESHOLD / 2;
            }
            k -> min_active_threads_treshold -= n_th;
        }
    }

    if (k -> threads_to_kill) {
        n_th += k -> threads_to_kill;
        k -> threads_to_kill = 0;
    }

    for_kill(n_th, k);
}

