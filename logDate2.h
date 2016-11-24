//
// Created by alessandro on 17/11/16.
//

#ifndef LOGGING_LOGDATE_H
#define LOGGING_LOGDATE_H

#endif //LOGGING_LOGDATE_H

#define MAX 80

void writeLogDate2(int cliente, int l) {

    int fd;
    char indcli[128];
    char stringa[MAX];
    char tempo[24];
    time_t now;
// struttura definita in &lt;time.h&gt;
    struct tm
            *tnow;

    // apertura file di log con xor fra i flag O_RDWR, O_CREAT, O_APPEND
// apre il file in lettura-scrittura, append e lo crea se serve
    fd = open ("log_servente", O_RDWR | O_CREAT | O_APPEND, 0644);

    time(&now);

    tnow = localtime(&now);

    strftime(tempo, 24, "%b %d %H:%M:%S %Y", tnow);

    inet_ntop(AF_INET, &cliente, indcli, sizeof(indcli));

    l = strlen(indcli);

    snprintf(stringa, 55 + l, "%s Richiesta da nodo %s, porta %5d\n",
             tempo, indcli, ntohs(cliente));

    printf("%s", stringa);
// scrive nel file di log
    write(fd, stringa, strlen(stringa));
}