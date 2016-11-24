//
// Created by alessandro on 17/11/16.
//
#include "utilsock.h"
#include "logDate2.h"

#define MAX 80
#define BACKLOG 20

int
main (int argc, char *argv[])
{

    int sd, accsd, fd, fdw, l, scritti, letti;

    struct sockaddr_in serv_ind, cliente;

    char buffer[MAX], datil[MAX], indcli[128];

    socklen_t lung;

    pid_t pid;

    in_port_t porta;

    if (argc != 2)
    {
        printf ("\nErrore\n");
        printf ("Attivare con ./servente_c porta(>1023)\n");
        exit (-1);
    }


// crea socket
    if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("Errore nella creazione del Socket");
        exit (-1);
    }

// indirizzo ip
    memset ((void *) &serv_ind, 0, sizeof (serv_ind));

// ind di tipo INET
    serv_ind.sin_family = AF_INET;

// porta
    porta = atoi (argv[1]);
    serv_ind.sin_port = htons (porta);

// connessioni da ogni ip
    serv_ind.sin_addr.s_addr = htonl (INADDR_ANY);

// bind
    if (bind (sd, (struct sockaddr *) &serv_ind, sizeof (serv_ind)) < 0)
    {
        perror ("Errore nella bind");
        exit (-1);
    }

// listen
    if (listen (sd, BACKLOG) < 0)
    {
        perror ("Errore nella listen");
        exit (-1);
    }

    printf ("Servente in ascolto sulla porta %d\n", porta);
    printf ("’Ctrl+c’ per chiuderlo\n\n");

    lung = sizeof (struct sockaddr_in);

// accetta connessione
    while (1)
    {
        if ((accsd = accept (sd, (struct sockaddr *) &cliente, &lung)) < 0)
        {
            perror ("Errore nella accept");
            exit (-1);
        }
        if ((pid = fork ()) < 0)
        {
            perror ("Errore nella fork");
            exit (-1);
        }

        if (pid == 0)
        {
// figli
            close (sd);
            writeLogDate2(serv_ind.sin_port,serv_ind.sin_addr.s_addr) ; // passo la porta e  l'indirizzo
            exit (0);
        }

        else
        {
// padre
            close (accsd);
        }

    }
    close (fd);
    close (fdw);
    exit (0);
}
