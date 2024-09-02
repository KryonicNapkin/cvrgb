CC 		= gcc
CFLAGS  = -std=c23 -g -Wall -Wextra -Wpedantic -Wconversion -lm 

include config.mk

all: cvrgb

cvrgb: 	
	$(CC) $(CFLAGS) -o $@ main.c converts.c 
	chmod 755 $@

install: all
	cp cvrgb ${DESTDIR}/

clean:
	rm -f ./cvrgb

uninstall:
	rm -f ${DESTDIR}/cvrgb
