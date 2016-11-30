//
// Created by giuseppe on 18/11/16.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


/*---------------IMAGE HANDLER-------------------
 * (1)  attesa di richiesta;
 * (2)  arriva la richiesta, con nome file e fattore di qualità;
 * (3)  check sulla cache;
 *      se il file è presente in cache passa a (4), se non è presente passa a (5);
 * (4)  prendi il file dalla cache e restituiscilo in output;
 *      ritorna  al passo (1);
 * (5)  crea il nuovo file, scrivilo in cache (aggiornala);
 *      ritorna al passo (4);
 */


int main() {

    mkdir("/tmp/web_server_cache", 0777);
    //rmdir("/tmp/web_server_cache");




    char *path = "/home/giuseppe/Scaricati";
    char *name = "123.png";
    char *f_path = "/tmp/web_server_cache";
    char *f_name = (char *) malloc(5 + strlen(name));

    int q = 50;

    sprintf(f_name, "%s_q%d", name, q);         //sbaglia perchè lascia il .png al centro e non alla fine

    convert_img(path,name,f_path,f_name,q);

    free(f_name);


    return 0;
}

/*
 * -------------------CONVERT_IMG----------------------------
 *
 * crea una nuova immagine ridimensionata.
 * prende in input il percorso dell'immagine da ridimensionare;
 * il percordo di destinazione e il fattore di qualità;
 * viene creata una stringa da passare come comando al terminale;
 * questo comando fa uso della funzione convert presente nella
 * libreria imagemagick.
 *
 * comand format: "convert path/name -resize q% s_path/s_name;exit"
 */


int convert_img(char *path, char *name, char *f_path, char *f_name, int q){

    char *final_comand = (char *) malloc(30 + strlen(path) + strlen(name) + strlen(f_path) + strlen(f_name));
    char *percent = "%";

    sprintf(final_comand, "convert %s/%s -resize %d%s %s/%s;exit", path, name, q, percent, f_path, f_name);
    system(final_comand);
    free(final_comand);
    return 0;
}

