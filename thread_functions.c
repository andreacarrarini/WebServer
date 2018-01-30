//
// Created by andrea on 23/11/17.
//

#include "structs.h"
#include "functions.h"

// Used to block SIGPIPE sent from send function
void catch_signal(void) {

    struct sigaction struct_sigaction;
    sigemptyset(&struct_sigaction.sa_mask);
    struct_sigaction.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &struct_sigaction, NULL) == -1)
        error_found("catch_signal: Error in sigaction\n");
}

// Used to get mutex to access a memory
//  area shared by multiple execution flows
void lock(pthread_mutex_t *mutex) {

    if (pthread_mutex_lock(mutex) != 0)
        error_found("lock: Error in pthread_mutex_lock\n");
}

// Used to release mutex
void unlock(pthread_mutex_t *mutex) {

    if (pthread_mutex_unlock(mutex) != 0)
        error_found("unlock: Error in pthread_mutex_unlock\n");
}

// Used waiting for the occurrence of an event
void wait_t(pthread_cond_t *cond, pthread_mutex_t *mutex) {

    if (pthread_cond_wait(cond, mutex) != 0)
        error_found("wait_t: Error in pthread_cond_wait\n");
}

// Used to send a signal to a thread
void signal_t(pthread_cond_t *cond) {

    if (pthread_cond_signal(cond) != 0)
        error_found("signal_t: Error in pthread_cond_signal\n");
}

// Used by kill_thread function
void mark_to_kill(int threads_number, struct threads_sync_struct *threads_sync_struct) {

    int i, j;

    for (i = j = 0; i < threads_number && j < MAX_CONNECTION; ++j)
        if (threads_sync_struct -> client_socket_list[j] == -1) {
            threads_sync_struct -> client_socket_list[j] = -2;
            signal_t(threads_sync_struct -> threads_cond_list + j);
            ++i;
        }

    // If there are not enough threads ready (with -1 flag)
    if (i < threads_number)
        threads_sync_struct -> threads_to_kill = threads_number - i;
}

// Used to kill threads
void kill_thread(struct threads_sync_struct *threads_sync_struct) {

    int threads_number = 0;

    if (threads_sync_struct -> min_active_threads_treshold > MIN_THREAD_TRESHOLD) {
        int old_thr = (threads_sync_struct -> min_active_threads_treshold - MIN_THREAD_TRESHOLD / 2) * 2 / 3;
        // To bring system to the default thread count
        if (!threads_sync_struct -> connections) {
            threads_sync_struct -> min_active_threads_treshold = MIN_THREAD_TRESHOLD;
            threads_number = threads_sync_struct -> active_threads - threads_sync_struct -> min_active_threads_treshold;
            threads_sync_struct -> threads_to_kill = 0;
            mark_to_kill(threads_number, threads_sync_struct);
            return;
        } else if (threads_sync_struct -> connections < old_thr) {
            // Gradual deallocation
            if (threads_sync_struct -> min_active_threads_treshold == MAX_CONNECTION) {
                if (!(threads_number = (MAX_CONNECTION - MIN_THREAD_TRESHOLD) % (MIN_THREAD_TRESHOLD / 2)))
                    threads_number = MIN_THREAD_TRESHOLD / 2;
            } else {
                threads_number = MIN_THREAD_TRESHOLD / 2;
            }
            threads_sync_struct -> min_active_threads_treshold -= threads_number;
        }
    }

    if (threads_sync_struct -> threads_to_kill) {
        threads_number += threads_sync_struct -> threads_to_kill;
        threads_sync_struct -> threads_to_kill = 0;
    }

    mark_to_kill(threads_number, threads_sync_struct);
}

