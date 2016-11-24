#include <unistd.h>
// interfaccia Unix standard
#include <errno.h>
// codici di errore
#include <sys/types.h>
// tipi predefiniti
#include <arpa/inet.h>
// per convertire ind. IP
#include <sys/socket.h>
// socket
#include <stdio.h>
// i/o
#include <stdlib.h>
// utilita’standard
#include <string.h>
// stringhe
#include <fcntl.h>
// file a basso livello
#include <time.h>
//include per time

/*

int
leggisock (int fd, void *buf, size_t conta)

 //legge "conta" byte da un descrittore
//da usare al posto di read quando fd e’ un socket di tipo stream

{
  int mancano, letti;
  mancano = conta;

  while (mancano > 0)
    {

      letti = read (fd, buf, mancano);

      if ((letti < 0) && (errno != EINTR))

	return (letti);
// errore

      else if (letti == 0)

	break;
// EOF

      mancano = mancano - letti;

      buf = buf + letti;

    }

  return (conta - mancano);
}

int
scrivisock (int fd, void *buf, size_t conta)
{

  int mancano, scritti;

  mancano = conta;

  while (mancano > 0)
    {

      scritti = write (fd, buf, mancano);

      if ((scritti <= 0) && (errno != EINTR))

	return (scritti);
// errore

      mancano = mancano - scritti;

      buf = buf + scritti;

    }

  return (conta - mancano);
}
*/