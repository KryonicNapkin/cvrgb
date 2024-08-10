CC 		= gcc
CFLAGS  = -lm -g -Wall -Wextra

include config.mk

all: cvrgb

cvrgb: 	
	$(CC) $(CFLAGS) -o $@ main.c converts.c 

install:
	cp cvrgb ${DESTDIR}/

clean:
	rm ./cvrgb

uninstall:
	rm -f ${DESTDIR}/cvrgb
