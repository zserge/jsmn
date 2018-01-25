# You can put your build options here
-include config.mk

BASEEXAMPLES=simple jsondump jsmndump
EXAMPLES=$(BASEEXAMPLES:%=%_example)
STRICT_EXAMPLES=$(EXAMPLES:%_example=%_strict_example)
TESTS=test_default test_strict test_links test_strict_links

all: libjsmn.a libjsmn_strict.a

lib%.a: %.o
	$(AR) rc $@ $^

%.o: %.c jsmn.h
	$(CC) -c $(CFLAGS) $< -o $@

%_strict.o: %.c jsmn.h
	$(CC) -DJSMN_STRICT=1 -c $(CFLAGS) $< -o $@

test: $(TESTS)
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

jsmn_test.o: jsmn_test.c libjsmn.a

examples: $(EXAMPLES)
strict_examples: $(STRICT_EXAMPLES)

%_strict_example: example/%.o libjsmn_strict.a
	$(CC) -DJSMN_STRICT=1 $(LDFLAGS) $^ -o $@

%_example: example/%.o libjsmn.a
	$(CC) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o example/*.o
	rm -f *.a *.so
	rm -f $(EXAMPLES)
	rm -f $(STRICT_EXAMPLES)
	rm -f $(TESTS:%=test/%)

.PHONY: all clean test

