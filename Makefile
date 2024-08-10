CC 		= gcc
CFLAGS  = -lm -g -Wall -Wextra

all: cvrgb

cvrgb: 	
	$(CC) $(CFLAGS) -o cvrgb main.c converts.c 

install:
	cp -r cvrgb /usr/local/bin/

clean:
	rm ./cvrgb

uninstall:
	rm -f /usr/local/bin/cvrgb
