Oyjl API - low level C helper interfaces
========================================

![](oyjl-args-qml/images/logo.png)


The Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and XML + YAML, string helpers, file reading, testing and argument handling.


Usage
-----
[oyjl Tool Syntax](docs/md/oyjl.md) [de](docs/md/oyjlde.md)
[oyjl-args Tool Syntax](docs/md/oyjlargs.md) [de](docs/md/oyjlargsde.md)
[oyjl-translate Tool Syntax](docs/md/oyjltranslate.md) [de](docs/md/oyjltranslatede.md)


Documentation
-------------
https://codedocs.xyz/oyranos‐cms/oyranos/group\_\_oyjl.html


Dependencies
------------
### Mandatory
* [Yajl](http://lloyd.github.com/yajl) - JSON API (or internall fallback)
* [libxml2](http://www.xmlsoft.org/) - for XML parsing (or internal fallback)
* libyaml

### Optional
* [Qt5](http://www.qt.io) - for a nice observer utility
* doxygen
* markdown
* [microhttpd](https://www.gnu.org/software/libmicrohttpd/) - oyjlArgsWeb renderer
* [gettext](https://www.gnu.org/software/gettext/) - i18n

Building
--------
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install

### Build Flags
... are typical cmake flags like CMAKE\_C\_FLAGS to tune compilation.

* CMAKE\_INSTALL\_PREFIX to install into paths and so on. Use on the command 
  line through -DCMAKE\_INSTALL\_PREFIX=/my/path .
