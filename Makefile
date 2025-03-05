CC = cc
CFLAGS = -g -Wall -Wpedantic

.PHONY: main clean build

minimizer.o: minimizer.h minimizer.c
	$(CC) $(CFLAGS) -c -o minimizer.o minimizer.c

main: main.c minimizer.h minimizer.o
	$(CC) $(CFLAGS) -o main minimizer.o main.c

build: main
	mv main css-unreadabilitier

clean:
	rm -f *.o
	rm main
