CC ?= gcc
CFLAGS = -O3 -ffast-math -march=native -flto -s
WFLAGS = -Wall -Wextra

wavwriter:
	$(CC) $(CFLAGS) $(WFLAGS) -o wavwriter main.c

clean:
	rm -f wavwriter
