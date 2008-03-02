
# $Id: Makefile.am 904 2006-08-16 22:24:14Z pertusus $

# bin_PROGRAMS = localeinfo
# localeinfo_SOURCES = localeinfo.c

#AM_CFLAGS=-g -O2

#all-local:
#	(cd include && $(MAKE) all)
#	(cd backends/$(default_backend) && $(MAKE) $(AM_MAKEFLAGS) libelektra_$(default_backend)_a-$(default_backend).$(OBJEXT))
#if EXPERIMENTAL
#	(cd backends/ini && $(MAKE) $(AM_MAKEFLAGS) \
#		libelektra_ini_a-ini.$(OBJEXT) \
#		libelektra_ini_a-helpers.$(OBJEXT) \
#		libelektra_ini_a-parser.$(OBJEXT) \
#	)
#endif

std_subdirs = include libloader libelektra backends kdb

DIST_SUBDIRS = $(std_subdirs)
SUBDIRS = $(std_subdirs)
EXTRA_DIST = Doxyfile

