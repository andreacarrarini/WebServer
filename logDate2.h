//
// Created by alessandro on 17/11/16.
//

#ifndef WEBSERVER_LOGDATE_H
#define WEBSERVER_LOGDATE_H

#define MAX 80
#include "readn_writen.h"

void writeLogDate2(int porta,int ip) {

    int fd,l;
    char indcli[128];
    char stringa[MAX];
    char tempo[24];
    time_t now;

    struct tm        /* struttura definita in &lt;time.h&gt;*/
            *tnow;


    fd = open ("log_servente", O_RDWR | O_CREAT | O_APPEND, 0644); /* apertura file di log con xor fra i flag O_RDWR, O_CREAT, O_APPEND, apre il file in lettura-scrittura, append e lo crea se serve*/

    time(&now);

    tnow = localtime(&now);

    strftime(tempo, 24, "%b %d %H:%M:%S %Y", tnow);

    inet_ntop(AF_INET, &ip, indcli, sizeof(indcli));

    l = strlen(indcli);

    snprintf(stringa, 55 + l, "%s Richiesta da nodo %s, porta %5d\n",
             tempo, indcli, ntohs(porta));

    printf("%s", stringa);

    writen(fd, stringa, strlen(stringa));   /* scrive nel file di log*/

    if (close(fd) < 0) {
        perror("errore in close");
        exit(EXIT_FAILURE);
    }
}

#endif //WEBSERVER_LOGDATE_H