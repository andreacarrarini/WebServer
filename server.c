#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <memory.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {

    typedef enum { false, true } bool;      //definisco il tipo bool

    bool thread_index[2];

    thread_index[0] = true;
    thread_index[1] = false;

    int a,b;
    if (thread_index[0] == true)
        printf("thread index 0 e' true\n");
    if (thread_index[1] == false)
        printf("thread index 1 e' false\n");
    return 0;



}

void spawn_child(void) {
    int count = 0;
    pid_t pid;
    while (count < 5) {
        if ((pid = fork()) != 0)
            if (pid == -1)
                perror("error in fork\n");
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
    int free;       //1 se occupato a servire una richiesta, 0 altrimenti
};


void *worker1_job(void *arg) {

}

void *worker2_job(void *arg) {

}

void child_job(void) {

    #define SERV_PORT 5193
    #define BACKLOG 10
    #define MAXLINE 1024

    int listensd, connsd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    struct thread_data td[2];   //una struct per thread

    td[0].free = 0;
    td[1].free = 0;

    pthread_t worker1;
    pthread_t worker2;

    struct thread_data *td = (struct thread_data *) arg;

    /*
    int listensd = td->listensd;
    int connsd = td->connsd;
    int len = td->len;

    struct sockaddr_in servaddr = *td->servaddr;
    struct sockaddr_in cliaddr = *td->cliaddr;
    */

    if ((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {     //creo la socket
        perror("errore in socket\n");
        exit(EXIT_FAILURE);
    }

    memset((char *) &servaddr, 0, sizeof(servaddr));     //azzero l'area di memoria di servaddr

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);       /* il server accetta richieste su ogni interfaccia di rete */
    servaddr.sin_port = htons(SERV_PORT);

    if ((bind(listensd, (struct sockaddr *) &servaddr, sizeof(servaddr))) < 0) {     /*"assigning a name to a socket",
 * prima di bind la socket esiste ma non ha assegnato alcun indirizzo */
        perror("errore in bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(listensd, BACKLOG) < 0) {       /*marco la socket identificata dal fd come passiva*/
        perror("errore in listen\n");
        exit(EXIT_FAILURE);
    }

    if (fcntl(listensd, F_SETFL, O_NONBLOCK) !=
        0) {        /*rendo la socket non bloccante quando in seguito uso accept*/
        perror("error making non-blocking socket\n");
        exit(EXIT_FAILURE);
    }

    for (;;) {

        int len = sizeof(cliaddr);
        socklen_t scklen = (socklen_t) len;

        if (td[0].free == 0) {

            if (connsd == accept(listensd, (struct sockaddr *) &cliaddr, &scklen) < 0) {     /*estrae la prima richiesta pendente
 * dalla coda di richieste della socket di ascolto, crea una socket di connessione, e ritorna un FD che la identifica,
 * la socket di ascolto non viene toccata */
                perror("errore in accept\n");
            }

            td[0].listensd = listensd;
            td[0].connsd = connsd;
            td[0].servaddr = &servaddr;
            td[0].cliaddr = &cliaddr;
            td[0].free = 1;

            if (pthread_create(&worker1, NULL, worker1_job, &td[0]) != 0) {
                perror("error in pthread_create\n");
                exit(EXIT_FAILURE);
            }
        } else if (td[1].free == 0) {

            if (connsd == accept(listensd, (struct sockaddr *) &cliaddr, &scklen) < 0) {     /*estrae la prima richiesta pendente
 * dalla coda di richieste della socket di ascolto, crea una socket di connessione, e ritorna un FD che la identifica,
 * la socket di ascolto non viene toccata */
                perror("errore in accept\n");
            }

            td[1].listensd = listensd;
            td[1].connsd = connsd;
            td[1].servaddr = &servaddr;
            td[1].cliaddr = &cliaddr;
            td[1].free = 1;

            if (pthread_create(&worker2, NULL, worker2_job, &td[1]) != 0) {
                perror("error in pthread_create\n");
                exit(EXIT_FAILURE);
            }

        else continue;  //errore: se sono occupati entrambi che faccio?????
        }
    }


    return;
}

