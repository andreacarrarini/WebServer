#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {



}

void spawn_child(void) {
    int count = 0;
    pid_t pid;
    while (count < 5) {
        if ((pid = fork()) != 0)
            if (pid == -1)
                perror("error in fork/n");
            if (pid > 0)    //processo padre
                continue;
        else child_job();   //processi figli
        count++;
    }
    return;
}

struct thread_data {
    pthread_t tid;
    int listensd, connsd;
    struct sockaddr_in *servaddr, *cliaddr;
    int len;
};

void child_job(void) {

    #define SERV_PORT 5193
    #define BACKLOG 10
    #define MAXLINE 1024

    //pthread_t listener, worker1, worker2;   //un listener e 2 (per ora) worker
    int listensd, connsd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    struct thread_data td[3];   //una struct per thread

    td[0].listensd = listensd;
    td[0].connsd = connsd;
    td[0].servaddr = &servaddr;
    td[0].cliaddr = &cliaddr;


    if (pthread_create(&listener, NULL, listener_job(), &td[0]) != 0) {
        perror("error in pthread_create/n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&worker1, NULL, worker1_job(), &td[1]) != 0) {
        perror("error in pthread_create/n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&worker2, NULL, worker2_job(), &td[2]) != 0) {
        perror("error in pthread_create/n");
        exit(EXIT_FAILURE);
    }
    return;
}

void *listener_job(void *arg) {
    struct thread_data *td = (struct thread_data *)arg;
    int listensd = td->listensd;
    int connsed = td->connsd;
    struct sockaddr_in *servaddr = td->servaddr;
    struct sockaddr_in *cliadrr = td->cliaddr;


}