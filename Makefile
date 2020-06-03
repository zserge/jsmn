# You can put your build options here
-include config.mk

test: test_default test_permissive test_links test_permissive_links
test_default: test/tests.c jsmn.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_permissive: test/tests.c jsmn.h
	$(CC) -DJSMN_DEFINES=1 -DJSMN_PERMISSIVE=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_links: test/tests.c jsmn.h
	$(CC) -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_permissive_links: test/tests.c jsmn.h
	$(CC) -DJSMN_DEFINES=1 -DJSMN_PERMISSIVE=1 -DJSMN_PARENT_LINKS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@

simple_example: example/simple.c jsmn.h
	$(CC) $(LDFLAGS) $< -o $@

jsondump: example/jsondump.c jsmn.h
	$(CC) $(LDFLAGS) $< -o $@

fmt:
	clang-format -i jsmn.h test/*.[ch] example/*.[ch]

lint:
	clang-tidy jsmn.h --checks='*'

clean:
	rm -f *.o example/*.o
	rm -f simple_example
	rm -f jsondump
	rm -f test/test_default test/test_permissive test/test_links test/test_permissive_links

.PHONY: clean test

