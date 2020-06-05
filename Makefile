# You can put your build options here
-include config.mk

test: test_default test_nonstrict test_lowmem test_nonstrict_lowmem test_permissiveprims test_permissivestrs test_primkeys test_single test_nonstrict_single
test_default: test/tests.c jsmn.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_nonstrict: test/tests.c jsmn.h
	$(CC) -DJSMN_NON_STRICT=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_lowmem: test/tests.c jsmn.h
	$(CC) -DJSMN_LOW_MEMORY=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_nonstrict_lowmem: test/tests.c jsmn.h
	$(CC) -DJSMN_NON_STRICT=1 -DJSMN_LOW_MEMORY=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_permissiveprims: test/tests.c jsmn.h
	$(CC) -DJSMN_PERMISSIVE_PRIMITIVES=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_permissivestrs: test/tests.c jsmn.h
	$(CC) -DJSMN_PERMISSIVE_STRINGS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_primkeys: test/tests.c jsmn.h
	$(CC) -DJSMN_PRIMITIVE_KEYS=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_single: test/tests.c jsmn.h
	$(CC) -DJSMN_SINGLE=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
	./test/$@
test_nonstrict_single: test/tests.c jsmn.h
	$(CC) -DJSMN_NON_STRICT=1 -DJSMN_SINGLE=1 $(CFLAGS) $(LDFLAGS) $< -o test/$@
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
	rm -f test/test_default test/test_nonstrict test/test_lowmem test/test_nonstrict_lowmem test/test_permissiveprims test/test_permissivestrs test/test_primkeys test/test_single test/test_nonstrict_single

.PHONY: clean test

