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

/*
 * Used to create other threads
 * in the case in which the server load is rising
 */
void spawn_thread(struct threads_sync_struct *threads_sync_struct) {

    /*
     * Threads are created dynamically in need with the number of connections.
     * If the number of connections decreases, the number of active threads
     * is reduced in a phased manner so as to cope with a possible peak of connections.
     */
    if (threads_sync_struct -> connections >= threads_sync_struct -> min_active_threads_treshold * 2 / 3 &&
        threads_sync_struct -> active_threads <= threads_sync_struct -> min_active_threads_treshold) {
        int n_th;
        if (threads_sync_struct -> min_active_threads_treshold + MIN_THREAD_TRESHOLD / 2 <= MAX_CONNECTION) {
            n_th = MIN_THREAD_TRESHOLD / 2;
        } else {
            n_th = MAX_CONNECTION - threads_sync_struct -> min_active_threads_treshold;
        }
        if (n_th) {
            threads_sync_struct -> min_active_threads_treshold += n_th;
            initialize_thread(n_th, manage_connection, threads_sync_struct);
        }
    }
}

void initialize_thread(int threads_to_create, void *(*routine)(void *), void *arg) {

    struct threads_sync_struct *threads_sync_struct = (struct threads_sync_struct *) arg;

    int i, j;
    lock(threads_sync_struct -> mtx_sync_conditions);
    for (i = j = 0; i < threads_to_create && j < MAX_CONNECTION; ++j) {
        // -1 := slot with thread initialized; -2 := empty slot.
        if (threads_sync_struct -> client_socket_list[j] == -2) {
            threads_sync_struct -> client_socket_element = j;
            create_thread(routine, arg);

            threads_sync_struct -> client_socket_list[j] = -3;
            wait_t(threads_sync_struct -> th_start, threads_sync_struct -> mtx_sync_conditions);
            ++i;
        }
    }
    threads_sync_struct -> active_threads += threads_to_create;
    unlock(threads_sync_struct -> mtx_sync_conditions);
}

void create_thread(void *(*routine)(void *), void *arg) {

    pthread_t tid;
    errno = 0;
    if (pthread_create(&tid, NULL, routine, arg) != 0) {
        if (errno == EAGAIN || errno == ENOMEM)
            error_found("create_thread: Insufficient resources to create another thread\n");
        else
            error_found("create_thread: Error in pthread_create\n");
    }
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
        int old_thread = (threads_sync_struct -> min_active_threads_treshold - MIN_THREAD_TRESHOLD / 2) * 2 / 3;
        // To bring system to the default thread count
        if (!threads_sync_struct -> connections) {
            threads_sync_struct -> min_active_threads_treshold = MIN_THREAD_TRESHOLD;
            threads_number = threads_sync_struct -> active_threads - threads_sync_struct -> min_active_threads_treshold;
            threads_sync_struct -> threads_to_kill = 0;
            mark_to_kill(threads_number, threads_sync_struct);
            return;
        } else if (threads_sync_struct -> connections < old_thread) {
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

