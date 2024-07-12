# Oyranos README
[![Pipeline](https://gitlab.com/oyranos/oyranos/badges/master/pipeline.svg)](https://gitlab.com/oyranos/oyranos/-/pipelines)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/13943/badge.svg)](https://scan.coverity.com/projects/13943)
[![Documentation](https://codedocs.xyz/oyranos-cms/oyranos.svg)](https://codedocs.xyz/oyranos-cms/oyranos/)
[![Codecov](https://codecov.io/gh/oyranos-cms/oyranos/branch/master/graph/badge.svg)](https://codecov.io/gh/oyranos-cms/oyranos)
![coverage](https://gitlab.com/oyranos/oyranos/badges/master/coverage.svg?job=coverage)
[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

![](extras/icons/oyranos_logo.png)

The Color Management System (CMS) Oyranos allows the coordination of
device specific Informations (ICC profiles) und system wide settings.


### Links
* [Sources & Download](https://gitlab.com/oyranos/oyranos)
* [Documentation & Support](https://gitlab.com/oyranos/oyranos/-/issues)
* [Command Line Tools](src/tools/README.md)
* [ChangeLog](ChangeLog.md)
* [Copyright](COPYING.md) - BSD
* [Authors](AUTHORS.md)
* [Packaging](PACKAGING.md)
* [FAQ](doc/FAQ.md)


### Dependencies
#### From Sources
##### Mandatory
* [icc-profiles-openicc and icc-profiles-basiccolor](http://sourceforge.net/projects/openicc/files/) - default ICC profiles
* [libXinerama, libXrandr, libXfixes and libXxf86vm](http:/www.x.org) - for X11 support
* [Xcalib](https://codeberg.org/OpenICC/xcalib) - a vcgt/calibration loader, required on X11/Windows 
* [lcms and/or lcms2](http://www.littlecms.com) - CMM for color conversion

##### Optional
* [libxml2](http://www.xmlsoft.org/) - for XML parsing (or internal fallback)
* [libXcm >= 0.5.4] (https://gitlab.com/oyranos/libxcm) - for monitor support (or internal fallback)
* [Yajl](http://lloyd.github.com/yajl) - JSON API (or internal fallback)
* [Oyjl](https://gitlab.com/beku/oyjl) - Oyjl Shared Basics API (or internal fallback)
* [libOpenICC](https://github.com/OpenICC/config) - JSON/Yajl based DB API (or internal fallback)
* [Elektra](http://www.libelektra.org) - DB API and D-Bus messaging
* [FLTK](http://www.fltk.org) - (--enable-threads) is needed for the example Gui
* For the documentation use doxygen, graphviz and graphviz-gd packages.
  * [Doxygen v1.5.8 or higher is recommended](http://www.doxygen.org)
* [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) - coverage docu
* [Qt](http://www.qt.io) - for a nice observer utility
* [CUPS](http://www.cups.org) - for CUPS ICC configuration support
* [libraw](http://www.libraw.org) - for cameraRAW decoding
* [exiv2](http://www.exiv2.org/) - for cameraRAW decoding
* [SANE](http://www.sane-project.org/) - only with Oyranos SANE\_CAP\_COLOUR patch
* [Cairo](http://www.cairographics.org/) - for a tool and a example
* [gettext](https://www.gnu.org/software/gettext/) - i18n
* [microhttpd](https://www.gnu.org/software/libmicrohttpd/) - oyjlArgsWeb renderer

#### Debian/Ubuntu:
    sudo apt-get install locales build-essential git autoconf cmake ruby libxml2-dev libyajl-dev libdbus-1-dev gettext libfltk1.3-dev libjpeg-dev libpng-dev libcairo2-dev x11proto-xf86vidmode-dev x11proto-xext-dev libxxf86vm-dev libxrandr-dev libxinerama-dev libxpm-dev libxfixes-dev libxft-dev fluid ftgl-dev liblcms-dev libexiv2-dev libcups2-dev cmake
    # oyranos-0.9.7 can now be installed successfully.
    sudo apt-get install libqt4-dev                        # Synnefo
    sudo apt-get install kdelibs5-dev                      # KolorManager
    sudo apt-get install libgtk2.0-dev libglib2.0-dev libgutenprintui2-dev libopenexr-dev # CinePaint
    sudo apt-get install doxygen graphviz                  # for documentation

##### Optional
    ICC Examin to build afterwards. oyranos-config-fltk uses the profile viewer.

#### Fedora 17
    git
    gcc-c++
    elektra-devel
    exiv2-devel
    cmake
    cups-devel
    sane-backends-devel
    liblcms-devel
    liblcms2-devel
    icc-profiles-basiccolor-lstarrgb
    icc-profiles-lcms-lab
    icc-profiles-openicc-printing
    icc-profiles-openicc-rgb
    libtool
    libtool-ltdl-devel
    libxml2-devel
    libpng-devel
    libtiff-devel
    LibRaw-devel
    libXinerama-devel
    libXrandr-devel
    libXcm-devel
    fltk-devel
    fltk-fluid
    gettext
    yajl-devel
    doxygen
    graphviz
##### Optional
    cairo-devel
    qt-devel
    netpbm-progs
    exiv2-devel

#### Arch Linux
    extra/libxml2
    extra/libxrandr
    extra/libxinerama
    extra/libxfixes
    extra/libxxf86vm
    extra/lcms
    extra/lcms2
    extra/graphviz
    extra/doxygen
    community/yajl
    AUR/libxcm
    AUR/openicc (TODO verify if correct package)
##### Optional
    extra/qt
    extra/fltk
    community/libraw
    AUR/xcalib
    AUR/elektra

  NOTE1: If you have problems installing elektra from AUR,
  use this PKGBUILD: ./extras/arch.abs/elektra/PKGBUILD

  NOTE2: To build an Arch package of Oyranos from yor local git branch,
  use './extras/arch.abs/oyranos-git/PKGBUILD' and
  (a) replace '_gitroot' variable,
  (b) replace the branch name in line 44.

#### openSUSE Leap 15.5
    zypper install cmake color-filesystem gcc-c++ git autoconf libtool make patch pkgconfig desktop-file-utils xdg-utils update-desktop-files ghostscript-fonts-std Mesa-devel libltdl-devel cups-devel elektra libxml2-devel libyajl-devel libyaml-devel libmicrohttpd-devel liblcms-devel liblcms2-devel libjpeg-devel libpng-devel libtiff-devel libraw-devel libexiv2-devel libxcm-devel libXcm0 libSM-devel xorg-x11-libXfixes-devel xorg-x11-libXpm-devel libXinerama-devel libXrandr-devel libXxf86vm-devel xbacklight sane-backends-devel fltk-devel libXft-devel ftgl-devel glu-devel gettext gettext-tools cairo-devel netpbm
    # oyranos-0.9.7 can now be installed successfully.
    zypper install libqt4-devel libqt5-qtbase-common-devel          # Synnefo
    zypper install patterns-kde-devel_kde_frameworks                # KolorManager
    zypper install gtk2-devel libopenexr-devel gutenprint-devel     # CinePaint
    zypper install doxygen graphviz                                 # for documentation

### Building
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install

#### Build Flags
... are typical cmake flags like CMAKE\_C\_FLAGS to tune compilation.

* CMAKE\_INSTALL\_PREFIX to install into paths and so on. Use on the command 
  line through -DCMAKE\_INSTALL\_PREFIX=/my/path .
* XDG\_CONFIG\_DIR - path to install oyranos-monitor-setup.desktop; defaults to /etc/xdg/autostart/
* USE\_GCOV - enable gcov/lcov compiler flags on the Unix platform and the coverage target
* USE\_SYSTEM\_ELEKTRA - fail if not found
* USE\_SYSTEM\_LIBXCM - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_OPENICC - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_OYJL - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_YAJL - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_LIBXML2 - fail if not found; default is shipped fallback version
* ENABLE\_QT - detect Qt5 and Qt4i; default is ON
* ENABLE\_OPENMP - detect OpenMP support; default is ON
* ENABLE\_FLTK - default is ON
* ENABLE\_DOCU\_OYRANOS - generate with doxygen; default is ON
* ENABLE\_TESTS\_OYRANOS - default is ON
* ENABLE\_EXAMPLES\_OYRANOS - default is ON
* ENABLE\_SHARED\_LIBS\_OYRANOS - default is ON
* ENABLE\_STATIC\_LIBS\_OYRANOS - default is ON

### Maintenance
libOyjl is included as a subtree project. Its main use is in the Oyranos tree. However spin off projects use the OyjlArgs API already.
Checkout of the Oyranos tree includes the Oyjl sources - simple.
For git check in of changes in openicc/oyjl use the prefix in:
    $ git commit openicc/oyjl -m "\*[oyjl]: XXX"
For commiting to Oyjl project use:
    $ git commit openicc/oyjl # to register the libOyjl changes
    $ git stash # hide everything else. git is a bit picky here.
    $ git subtree push -P openicc/oyjl oyjl master
    $ git stash pop # restore the stash cache
For pulling in/update openicc/oyjl from the Oyjl upstream:
    $ git stash # hide everything else. git is a bit picky here.
    $ git subtree pull -P openicc/oyjl oyjl master
    $ git stash pop # restore the stash cache
