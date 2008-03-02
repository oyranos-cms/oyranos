
# $Id: Makefile.am 910 2006-08-20 14:19:13Z mraab $

DIST_SUBDIRS = filesys
SUBDIRS = $(backends)

#if EXPERIMENTAL
#SUBDIRS += gconf ini template
#endif

#if HAVE_GLIBC_MNTENT
#SUBDIRS += fstab
#endif

#if HAVE_UCRED
#SUBDIRS += daemon
#endif

install-exec-hook:
	cd $(DESTDIR)$(backenddir) && \
	test -L libelektra-default.so || \
	$(LN_S) libelektra-@default_backend@.so libelektra-default.so
	cd $(DESTDIR)$(backenddir) && \
	test -L libelektra-ddefault.so || \
	$(LN_S) libelektra-@default_dbackend@.so libelektra-ddefault.so

