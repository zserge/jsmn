# You can put your build options here
-include config.mk

CFLAGS:=${CFLAGS} -std=c89 -Wpedantic

test: test_default test_default_low_memory test_permissive test_permissive_low_memory
test_default: test/tests.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_default_low_memory: test/tests.c
	$(CC) -DJSMN_LOW_MEMORY $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@

test_permissive: test/tests.c
	$(CC) -DJSMN_PERMISSIVE $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_permissive_low_memory: test/tests.c
	$(CC) -DJSMN_PERMISSIVE -DJSMN_LOW_MEMORY $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@

simple_example: example/simple.c
	$(CC) $(LDFLAGS) $< -o $@

jsondump: example/jsondump.c
	$(CC) $(LDFLAGS) $< -o $@

fmt:
	clang-format -i jsmn.h test/*.[ch] example/*.[ch]

lint:
	clang-tidy jsmn.h --checks='*'

clean:
	rm -f *.o example/*.o
	rm -f simple_example
	rm -f jsondump
	rm -f test/test_default
	rm -f test/test_default_low_memory
	rm -f test/test_permissive
	rm -f test/test_permissive_low_memory

.PHONY: clean test
