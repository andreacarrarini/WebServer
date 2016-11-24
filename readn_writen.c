#include <errno.h>
#include <unistd.h>

int readn(int fd, void *buf, size_t n) {
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr = buf;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno ==
                EINTR)     /* funzione interrotta da un segnale prima di aver  potuto leggere qualsiasi dato. */
                nread = 0;
            else
                return (-1);     /*errore */
        } else if (nread == 0) break;
        nleft -= nread;
        ptr += nread;
    }       //end while
    return nleft;
}

ssize_t writen(int fd, const void *buf, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = buf;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if ((nwritten < 0) && (errno == EINTR)) nwritten = 0;
            else return(-1);        /* errore */
        }
        nleft -= nwritten;
        ptr += nwritten;
    } /* end while */
    return(nleft);

}