//
// Created by alessandro on 01/12/16.
//
#include "utilsock.h"
#include "get_userAgent.h"
#include <stdio.h>
#include <string.h>

int main ()
{

    char str[] ="GET /books/search.aspx?query=23k9j HTTP/1.1\n"
                "Accept: image/gif, image/xxbitmap, image/jpeg, image/pjpeg,\n"
                "Accept-Language: en-gb,en-us;q=0.5\n"
                "Referer: http://miaapplicazione.miosito.com/books/default.aspx\n"
                "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\n";


    char str2[] ="GET /books/search.aspx?query=23k9j HTTP/1.1\n"
            "Accept: image/gif, image/xxbitmap, image/jpeg, image/pjpeg,\n"
            "Accept-Language: en-gb,en-us;q=0.5\n"
            "Referer: http://miaapplicazione.miosito.com/books/default.aspx\n"
            "User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1)\n";


    printf ("GET: %s\n",get(str));
    printf("\n");
    printf ("USER-AGENT: %s\n",userAgent(str2));
    
    return (EXIT_SUCCESS);
}
