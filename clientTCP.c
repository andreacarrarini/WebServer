//
// Created by alessandro on 17/11/16.
//
#include "utilsock.h"
#define MAX 80

int
main (int argc, char *argv[])
{

  int sd, n, i, fd, scritti, letti;
  struct sockaddr_in serv_ind;
  in_port_t porta;
  char buffer[MAX], datil[MAX];

// controlla argomenti

  if (argc != 3)
  {
    printf ("\nErrore\n");
    printf ("Esempio di connessione per traferire file: \n");
    printf ("./client ip_servente porta (>1023) \n");
    exit (-1);
  }

// crea socket
  if ((sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror ("Errore in creazione socket");
    exit (-1);
  }

// indirizzo IP

  memset ((void *) &serv_ind, 0, sizeof (serv_ind));	// pulizia ind

  serv_ind.sin_family = AF_INET;
// ind di tipo INET

  porta = atoi (argv[2]);

  serv_ind.sin_port = htons (porta);
// porta a cui collegarsi

/* crea indirizzo usando inet_pton */

  if ((inet_pton (AF_INET, argv[1], &serv_ind.sin_addr)) <= 0)
  {
    perror ("Errore in creazione indirizzo");
    return -1;
  }

// stabilisce la connessione
  if (connect (sd, (struct sockaddr *) &serv_ind, sizeof (serv_ind)) < 0)
  {
    perror ("Errore nella connessione");
    exit (-1);
  }
  close (sd);
  close (fd);
  exit (0);
}
