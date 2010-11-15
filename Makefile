CFLAGS=-Wall -W -std=c89

all: jsmn_demo

jsmn_demo: jsmn.o
	gcc $(LDFLAGS) jsmn.o -o $@

jsmn.o: jsmn.c jsmn.h
	gcc $(CFLAGS) -c jsmn.c -o $@

clean:
	rm -f jsmn.o
	rm -f jsmn_demo
