# You can put your build options here
-include config.mk



all: libjsmn.a example

libjsmn.a: jsmn.o jsmn_iterator.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c $(CFLAGS) $< -o $@

test: test_default test_strict test_links test_strict_links test_iterator
test_default: test/tests.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict: test/tests.c
	$(CC) -DJSMN_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_links: test/tests.c
	$(CC) -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_strict_links: test/tests.c
	$(CC) -DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_iterator: test/tests_iterator.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@

jsmn_test.o: jsmn_test.c libjsmn.a

simple_example: example/simple.o libjsmn.a
	$(CC) $(LDFLAGS) $^ -o $@

jsondump: example/jsondump.o libjsmn.a
	$(CC) $(LDFLAGS) $^ -o $@

jsonprint: example/jsonprint.o libjsmn.a
	$(CC) $(LDFLAGS) $^ -o $@

example: simple_example jsondump jsonprint

clean:
	rm -f jsmn.o jsmn_test.o example/simple.o
	rm -f libjsmn.a
	rm -f simple_example
	rm -f jsondump

.PHONY: all clean test example

