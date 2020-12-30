# Archivo Makefile usando cadenas para ejecucion

PROYECTO=principal
DESTDIR=/usr/local/bin
CFLAGS=-Wall
LFLAGS=-lm -lpthread
CC=gcc

# Source or file name .c
src=$(wildcard *.c)

all: $(PROYECTO)

# target : dependency
%.o: %.c
	$(CC) -c $< $(CFLAGS)

#$(PROYECTO): DependenciesList
$(PROYECTO): $(src:%.c=%.o)
	$(CC) $^ -o $@ $(LFLAGS)

.PHONY: clean

install: all
	if [ ! -d $(DESTDIR) ]; then \
		sudo mkdir $(DESTDIR); \
	fi; \
	sudo cp $(PROYECTO) $(DESTDIR)

uninstall:
	sudo rm $(DESTDIR)/$(PROYECTO)

clean:
	rm -f *.o $(PROYECTO)