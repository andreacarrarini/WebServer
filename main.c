#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {

    /*
    char *a1 = "/home/giuseppe/Scaricati";
    char *a2 = "123.png";
    char *a3 = a1;
    char *a4 = "1234.png";

    int q = 50;

    convert_img(a1,a2,a3,a4,q);
     */

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
    char *percentage = "%";

    sprintf(final_comand, "convert %s/%s -resize %d%s %s/%s;exit", path, name, q, percentage, f_path, f_name);
    system(final_comand);
    free(final_comand);
    return 0;
}