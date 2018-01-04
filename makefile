CC = gcc
CFLAGS = -c -Wall -D MAGICKCORE_HDRI_ENABLE=0 -D MAGICKCORE_QUANTUM_DEPTH=16
DEPS = functions.h structs.h
LDFLAGS =

SOURCES = server.c error_functions.c file_functions.c image_functions.c thread_functions.c cache_functions.c

include_imagemagick = -I /usr/local//include
lib_magickwand = -L /usr/local/lib -l MagickWand-7.Q16HDRI
lib_magickcore = -L /usr/local/lib -l MagickCore-7.Q16HDRI

INC = $(include_imagemagick)
LIB = $(lib_magickwand) $(lib_magickcore) -lm -lpthread

OBJ = server.o error_functions.o file_functions.o image_functions.o thread_functions.o cache_functions.o
EXECUTABLE = WebServer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJ) 
	$(CC) $(LDFLAGS) $(OBJ) $(INC) $(LIB) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

cleanobjs:
	rm -f $(OBJECTS)

