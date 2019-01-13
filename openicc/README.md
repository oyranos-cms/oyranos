# OpenICC Configuration README
[![Build Status](https://travis-ci.org/OpenICC/config.svg?branch=master)](https://travis-ci.org/OpenICC/config)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/10868/badge.svg)](https://scan.coverity.com/projects/10868)
[![Documentation](https://codedocs.xyz/OpenICC/config.svg)](https://codedocs.xyz/OpenICC/config)
[![Codecov](https://codecov.io/gh/OpenICC/config/branch/master/graph/badge.svg)](https://codecov.io/gh/OpenICC/config)
[![Issues](https://img.shields.io/github/issues-raw/OpenICC/config.svg?style=flat-square)](https://github.com/OpenICC/config/issues)
[![License](https://img.shields.io/github/license/mashape/apistatus.svg)](http://www.opensource.org/licenses/mit-license.php)

![](http://www.openicc.info/images/OpenICC.svg)

The OpenICC configuration data base allows to store, share and manipulate
colour management informations.

Part of that is a file format based on JSON and a implementation of a 
according library for easy access.

### Features
* Access to OpenICC device JSON DB
* [Command Line Tools](src/tools/README.md)

### Links
* [Copyright](docs/COPYING.md) - MIT
* [ChangeLog](docs/ChangeLog.md)
* [Authors](docs/AUTHORS.md)
* [Code](https://github.com/OpenICC/config)
* [OpenICC](http://www.openicc.info) - open source color management discussion group
 

### Dependencies
* [Yajl](http://lloyd.github.com/yajl/) - a JSON parser library

#### Optional
* [gettext](https://www.gnu.org/software/gettext/) - i18n
* [LCOV](http://ltp.sourceforge.net/coverage/lcov.php) - coverage docu
* For the documentation use doxygen, graphviz and graphviz-gd packages.
  * [Doxygen v1.5.8 or higher is recommended](http://www.doxygen.org)

### Building
Supported are cmake builds and a very simple makefile.

    $ mkdir build && cd build
    $ cmake ..
    $ make
    $ make install

#### Build Flags
... are typical cmake flags like CMAKE\_C\_FLAGS to tune compilation.

* CMAKE\_INSTALL\_PREFIX to install into paths and so on. Use on the command 
  line through -DCMAKE\_INSTALL\_PREFIX=/my/path .
* USE\_GCOV - enable gcov/lcov compiler flags on the Unix platform and the coverage target
* USE\_SYSTEM\_OYJL - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_YAJL - fail if not found; default is shipped fallback version
* USE\_SYSTEM\_LIBXML2 - fail if not found; default is shipped fallback version
* ENABLE\_TESTS - default is ON
* ENABLE\_TOOLS - default is ON
* ENABLE\_QT - detect Qt5 and Qt4i; default is ON

### Known Bugs
* The source code provides currently no mechanism for a write lock.

