CC 		= gcc
CFLAGS  = -lm -g -Wall -Wextra

all:
	$(CC) $(CFLAGS) -o cvrgb main.c converts.c 
