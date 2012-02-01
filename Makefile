# You can put your build options here
-include config.mk

all: libjsmn.a 

libjsmn.a: jsmn.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c $(CFLAGS) $< -o $@

test: jsmn_test
	./jsmn_test

jsmn_test: jsmn_test.o
	$(CC) -L. -ljsmn $< -o $@

clean:
	rm -f jsmn.o jsmn_test.o
	rm -f jsmn_test
	rm -f libjsmn.a

.PHONY: all clean test

