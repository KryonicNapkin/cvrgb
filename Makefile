CC 		= gcc
CFLAGS	= -std=c23 -g -Wall -Wextra -Wpedantic -Wconversion -lm 

include config.mk

all: cvrgb

cvrgb: main.c converts.c
	$(CC) $(CFLAGS) -o $@ $^ 
	chmod 755 $@

install: all
	cp cvrgb ${DESTDIR}/

clean:
	rm -f ./cvrgb

uninstall:
	rm -f ${DESTDIR}/cvrgb
