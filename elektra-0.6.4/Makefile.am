# $Id: Makefile.am 902 2006-08-15 11:54:15Z pertusus $

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = libltdl src
EXTRA_DIST=elektra.pc elektratools.pc elektra.spec elektra.mandriva.spec elektra.xml 

pkgconfigdir = $(ulibdir)/pkgconfig
pkgconfig_DATA = elektra.pc elektratools.pc

RPMARCH=`rpmbuild --showrc | awk '/^build arch/ {print $$4}'`

rpm: dist
	mkdir -p rpmdir/BUILD \
	rpmdir/SPECS \
	rpmdir/SOURCES \
	rpmdir/SRPMS \
	rpmdir/RPMS/$(RPMARCH)
	rpmbuild --clean -ta $(PACKAGE)-$(VERSION).tar.gz --define "_topdir $$PWD/rpmdir"
	@echo "============================================================"
	@echo "Finished - the packages are in rpmdir/RPMS and rpmdir/SRPMS!"

