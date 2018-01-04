CC = gcc
CFLAGS = -c -Wall -D MAGICKCORE_HDRI_ENABLE=0 -D MAGICKCORE_QUANTUM_DEPTH=16
LDFLAGS =

SOURCES = server.c cache_functions.c error_functions.c file_functions.c image_functions.c readn_writen.c thread_functions.c

include_imagemagick = -I /usr/local//include
lib_magickwand = -L /usr/local/lib -l MagickWand-7.Q16HDRI
lib_magickcore = -L /usr/local/lib -l MagickCore-7.Q16HDRI

INC = $(include_imagemagick)
LIB = $(lib_magickwand) $(lib_magickcore) -lm -lpthread

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = WebServer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) $(INC) $(LIB) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

cleanobjs:
	rm -f $(OBJECTS)

