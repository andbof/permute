ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -D_GNU_SOURCE
AM_CFLAGS = -g -O2 -Wall -Werror -Wformat -Wformat-security -Wformat-nonliteral -Wformat=2 -ftrapv -Wno-unused-parameter -Wtype-limits -fstack-protector-all -Wno-format-y2k -fno-strict-aliasing
AM_TESTS_ENVIRONMENT = \
                       export builddir="$(builddir)"; \
                       export PACKAGE_NAME="$(PACKAGE_NAME)"; \
                       export PACKAGE_VERSION="$(PACKAGE_VERSION)";

TESTS = \
        test/str_to_uint \
        test/permute \
        test/version_option \
        test/depth

bin_PROGRAMS = permute

check_PROGRAMS = \
                 test/str_to_uint \
                 test/permute

dist_check_SCRIPTS = \
                     test/depth \
                     test/version_option

permute_SOURCES = \
                  src/main.c \
                  src/permute.c \
                  src/permute.h \
                  src/str_to_uint.c \
                  src/str_to_uint.h

test_permute_CPPFLAGS = -I$(srcdir)/src
test_permute_SOURCES = \
                       test/src/test_permute.c \
                       src/permute.c \
                       src/permute.h

test_str_to_uint_CPPFLAGS = -I$(srcdir)/src
test_str_to_uint_SOURCES = \
                          test/src/test_str_to_uint.c \
                          src/str_to_uint.c \
                          src/str_to_uint.h

@VALGRIND_CHECK_RULES@
