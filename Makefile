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
	$(CC) -DJSMN_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@


test_coverage: test_default_lcov test_strict_lcov test_links_lcov test_strict_links_lcov test_iterator_lcov
	mkdir -p $@
	lcov --no-external --directory . --base-directory . -c -o $@/$@.info
	genhtml -o $@ -t "jsmn" --num-spaces 2 $@/$@.info

test_default_lcov: test/tests.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@ -coverage
	-./test/$@
	mv tests.gcda $@.gcda
	mv tests.gcno $@.gcno
test_strict_lcov: test/tests.c
	$(CC) -DJSMN_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@ -coverage
	-./test/$@
	mv tests.gcda $@.gcda
	mv tests.gcno $@.gcno
test_links_lcov: test/tests.c
	$(CC) -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@ -coverage
	-./test/$@
	mv tests.gcda $@.gcda
	mv tests.gcno $@.gcno
test_strict_links_lcov: test/tests.c
	$(CC) -DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@ -coverage
	-./test/$@
	mv tests.gcda $@.gcda
	mv tests.gcno $@.gcno
test_iterator_lcov: test/tests_iterator.c
	$(CC) -DJSMN_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@ -coverage
	./test/$@


jsmn_test.o: jsmn_test.c libjsmn.a

simple_example: example/simple.o libjsmn.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

jsondump: example/jsondump.o libjsmn.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

jsonprint: example/jsonprint.o libjsmn.a
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

example: simple_example jsondump jsonprint

clean:
	rm -f *.o example/*.o
	rm -f *.a *.so
	rm -f simple_example
	rm -f jsondump jsondump jsonprint
	rm -f test/coverage.info
	rm -f *.gcno *.gcda
	rm -rf test_coverage

.PHONY: all clean test example

