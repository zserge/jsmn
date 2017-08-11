# Copyright 2017 Luke Dashjr
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the standard MIT license.  See COPYING for more details.

lib_LTLIBRARIES = libjsmn.la
libjsmn_la_SOURCES = jsmn.c
libjsmn_la_LDFLAGS = -version-info $(LIBJSMN_SO_VERSION) -no-undefined

libjsmn_includedir = $(includedir)
libjsmn_include_HEADERS = jsmn.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = jsmn.pc

dist_noinst_SCRIPTS = autogen.sh
dist_doc_DATA = LICENSE README.md

jsondump_SOURCES = example/jsondump.c
jsondump_LDADD = libjsmn.la

simple_example_SOURCES = example/simple.c
simple_example_LDADD = libjsmn.la

if USE_TOOL
bin_PROGRAMS = jsondump
endif

if NEVER_ENABLED
noinst_PROGRAMS = simple_example
endif

TESTS = test_default test_strict test_links test_strict_links
if NEVER_ENABLED
noinst_PROGRAMS += $(TESTS)
endif

test_default_SOURCES = test/tests.c

test_strict_SOURCES = test/tests.c
test_strict_CPPFLAGS = -DJSMN_STRICT=1

test_links_SOURCES = test/tests.c
test_links_CPPFLAGS = -DJSMN_PARENT_LINKS=1

test_strict_links_SOURCES = test/tests.c
test_strict_links_CPPFLAGS = -DJSMN_STRICT=1 -DJSMN_PARENT_LINKS=1

# Compatibility with old Makefile:

test: check

.PHONY: test
