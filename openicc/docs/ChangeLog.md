# ChangeLog

# Version 0.1.0

### Kai-Uwe Behrmann (267):
	* [build]: properly spec package html docu
	* [build]: fix spec file
	* [core]: compile fix
	* [oyjl]: fix compile warnings
	* [core]: fix setting initial JSON file
	* [oyjl]: fix double freeing
	* [core]: add location in fo to error messages
	* [test]: JSON node delete
	* [core]: implement openiccDBSetString("delete")
	* [oyjl]: add oyjl_tree_free_node()
	* [core]: add internal openiccIsFileFull_()
	* [core]: declare openiccMessageFunc without openicc specifics
	* [core]: export openiccOBJECT_e
	* [core]: hide more oyjl symbols
	* [core]: set openiccDBSetString() messages to error
	* [core]: complete openiccDBSetString()
	* [oyjl]: fix unchecked array access
	* [core]: start implement openiccDBSetString()
	* [oyjl]: fix message string line break
	* [test]: add values to oyjl_tree
	* [oyjl]: add oyjl_value_set_string()
	* [docu]: fix section header in README.md
	* [docu]: remove obsolete TODO on README
	* [test]: improve oyjl_tree_get_value() tree generation print
	* [oyjl]: support generation of nodes in oyjl_tree_get_value()
	* [test]: fix the json generation check
	* [oyjl]: the keys are owned by the tree object and not const
	* [core]: check before release of memory
	* [test]: create root object before generating json tree
	* [oyjl]: fix message func print out
	* [oyjl]: add oyjl_tree_internal.h for shared macros
	* [core]: fix compile warnings
	* [oyjl]: implement oyjl message function
	* [oyjl]: add oyjl_tree_free_content()
	* [core]: remove non use code parts
	* [docu]: fix grouping of member db functions
	* [CI]: remove non needed commands in travis
	* [docu]: show more badges on github top page
	* [build]: remove the coverage.info.cleanup file
	* [CI]: fix clang travis
	* [CI]: add make coverage to travis
	* [CI]: specify info path to codecov script
	* [oyjl]: remove misleading xpath name from public API
	* [oyjl]: add flags arg to oyjl_tree_get_value*()
	* [oyjl]: add oyjl_string_to_long()
	* [core]: move all string functions to oyjl
	* [conf]: next travis for codecov
	* [build]: keep the coverage.info report
	* [conf]: try without make coverage
	* [config]: iterate over travis + codecov
	* [conf]: specify the root path to codecov/gcov
	* [conf]: set codecov output to verbose
	* [conf]: run coverage upload directly after make coverage from travis
	* [conf]: fix shell syntax for travis
	* [conf]: exclude clang from make coverage target
	* [test]: fix coverage target for travis
	* [conf]: require lcov on travis.ci
	* [conf]: fix GCOV flag for travis
	* [conf]: try on the fly code coverage report
	* [test]: place fallback for non available xdg config
	* [core]: fix just counting in openiccDB_GetKeyNames()
	* [core]: implement openiccDB_GetString()
	* [test]: DB functions
	* [core]: export openiccGetShortKeyFromFullKeyPath() ++
	* [core]: implement openiccDB_GetKeyNames()
	* [core]: add oyjl_string_list_free_doubles + oyjl_string_list_add_list
	* [docu]: add test.c examples to life doxy
	* [docu]: try to enable examples to life docu
	* [docu]: show openicc_config_read.c code fragments from main page
	* [docu]: convert docs to markdown
	* [docu]: link to openicc.info for more actual content
	* [docu]: add minimalistic tutorial on first page
	* [docu]: link example into doxy
	* [test]: update to changed API and test more oyjl stuff
	* [core]: support openiccConfig_GetKeyNames(child_levels) add
	* [core]: add oyjl_tree_to_xpath()
	* [core]: add openiccConfig_GetStrings()
	* [docu]: API is too long in README headline
	* [docu]: fix links in README for life doxy
	* [docu]: rename link as Development
	* [docu]: clearify kind of source link
	* [docu]: fix link to repo in doxy
	* [docu]: somehow the header gets messed up - skip
	* [docu]: set header and footer for online docu
	* [docu]: move @internal up for doxygen
	* [docu]: makr oyjl stuff as internal
	* [docu]: hide non documented stuff
	* [docu]: link to generated docu
	* [docu]: support online code generation
	* [docu]: fix code and bug-report links after the repository change
	* [docu]: remove XDG TODO as done by openiccGetInstallPath()
	* [core]: rename openiccConfig_CountDevices to openiccConfig_DevicesCount
	* [core]: add openiccConfig_GetStringf()
	* [core]: rename variable to not sugest freeing
	* [core]: add openiccConfig_GetString()
	* [core]: fix leak in oyjl
	* [core]: relax for most openiccConfig_GetKeyNames() args
	* [test]: use the independent static object for coverage
	* [core]: test object type info for messages
	* [core]: add openiccOBJECT_e to object types
	* [docu]: add travis icon on README.md
	* [test]: cover one more case of test file name
	* [test]: adapt test file path
	* [test]: show file name
	* [build]: list travis source path
	* [test]: add possible test path
	* [build]: add icc-profiles-free to CI
	* [conf]: relax valgrind usage for check on old ubuntu
	* [build]: add valgrind to travis installs
	* [core]: use own code for yajl tree parsing
	Revert "* [core]: skip code duplicates with libyajl in oyjl"
	* [build]: remove coverity
	* [build]: use travis
	* [docu]: link to optional dependencies
	* [test]: add "JSON handling" for low level stuff
	* [core]: implement openiccConfig_GetKeyNames()
	* [core]: skip code duplicates with libyajl in oyjl
	* [build]: use static linking for test_openicc + check
	* [core]: include oyjl_tree.h
	* [core]: add oyjl for xpath style JSON handling
	* [docu]: improve section names
	* [docu]: setup Doxygen
	* [core]: split config source file
	* [core]: fix DB pointer release
	* [test]: add "ODB" openiccDB_s test
	* [core]: let openiccDB_NewFrom() open some configs
	* [core]: set DB symbol names properly
	* [test]: add XDG
	* [core]: detect OI_DEBUG environment var in openiccInit()
	* [core]: add OI_DEBUG macro
	* [core]: fix new symbol name II
	* [core]: fix new symbol name
	* [core]: name openiccConfig_s variables as singles
	* [test]: fix the device test fro previous commit
	* [i18n]: update deutsch
	* [core]: rename OpeniccConfigs_s to openiccConfig_s
	* [xdg]: declare xdg_bds(spath) static
	* [build]: install db header
	* [core]: declare DB APIs
	* [build]: adjust url
	* [build]: add debian files
	* [conf]: fix i18n pot file name
	* [build]: add spec file
	* [conf]: adjust package name
	* [build]: install openicc-device
	* [build]: skip non needed link libraries
	* [build]: make the dist target working
	* [conf]: set the OpenIccConfig.cmake values
	* [build]: install into down letter include/openicc
	* [docs]: fix typo
	* [i18n]: remove non needed file
	* [core]: improve debug output
	* [test]: add non json parse test
	* [core]: add openiccGetInstallPath() API
	* [core]: rename some internal functions
	* [core]: expose more configure time variables
	* [core]: rename OpeniccConfigAlloc_f to openiccAlloc_f
	* [core]: add OpeniccConfigDeAlloc_f type
	* [docs]: remove unused file
	* [test]: fix compile warning
	* [build]: remove autotools support
	* [test]: support different directory level for coverage target
	* [core]: internally expose openiccMessageFunc declaration
	* [core]: remove openiccConfigsSearch() API
	* [test]: fix directory test
	* [test]: adapt to device json from git
	* [test]: add "i18n" and "Device JSON handling"
	* [conf]: check for locale.h
	* [exmpl]: enable i18n
	* [core]: fix two leaks
	* [core]: set gettext domain to OpenICC
	* [core]: add openiccInit() API
	* [conf]: set default locale path
	* [conf]: import variables in openicc_conf.h
	* [core]: avoid redefined _() macro
	* [core]: fix leakage - close FILE handle
	* [test]: fix compile warnings and close leak
	* [test]: add IO and String tests
	* [exmpl]: fix json output of openicc-config-read
	* [core]: show more io functions internally
	* [test]: add openicc-test
	* [core]: add OPENICC_VERSION macro
	* [docs]: clean some old strings
	* [core]: add runtime openiccVersion() check
	* [conf]: use autotools friendly version macros
	* [core]: fix pure file name passing to openiccExtractPathFromFileName_()
	* [build]: fix gcov usage
	* [docs]: add TODO section
	* [docs]: move some docs out of the top directory
	* [docs]: improve README
	* [build]: rename libopenicc to libOpenICC
	* [core]: use new macro names
	* [core]: rm OPENICC_SETTINGS/DEVICES_DB + add OPENICC_DB
	* [conf]: better sync library version with cmake builds
	* [build]: move utils -> src/tools
	* [util]: support op-device -j option without -l
	* [util]: fix o-device -ld 0 option combination
	* [build]: update autotools files
	* [conf]: add coverage make target
	* [build]: add cmake module to simplify lcov usage
	* [build]: place the cmake config file under lib/cmake
	* [conf]: fix cmake generated versions in header
	* [conf]: fix cmake version macro names
	* [conf]: build extras/openicc.pc with cmake
	* [conf]: install cmake into lib/cmake/openicc
	* [build]: add openicc-config-read to cmake
	* [build]: add cmake files
	* [i18n]: add gettext translations
	* [build]: move sources under src/libopenicc path
	* [build]: move includes under src
	* [util]: add openicc-device -c option
	* [utils]: rename openicc-device -p to -d option
	* [util]: remove empty DB file for openicc-device -ew
	* [core]: add openicc-device -e option for erase
	* [core]: fix leaks
	* [core]: properly prefix string functions
	* [build]: add make/test target
	[core]: update xdg_bds to ArgyllCMS-1.5.0
	annotate windows call for getenv("APPDATA")
	* [core]: improve read from stdin
	* [util]: add openicc-device -w option
	* [core]: improve POSIX detection
	* [core]: add openiccWriteFile() API
	* [build]: fix for oS-13.2
	* [exmpl]: update to API changes
	* [core]: fix compile warnings about unused vars
	* [core]: skip message without sense
	* [conf]: fix bootstraping
	* [docu]: move README to markup
	* [dev]: add devices to appropriate device class
	* [core]: export openiccConfigs_GetClasses() API
	* [core]: fix openiccConfigs_GetClasses() return
	* [conf]: fix CFLAGS access
	* [dev]: add -s option to --show-path
	* [dev]: add --show-path option
	* [dev]: lookup device DB from xdg
	* [core]: add internal openiccOpenFile() helper
	* [core]: extent openiccConfigs_DeviceGetJSON
	* [build]: add include dirs to makefile
	* [core]: use xdg_bds DB lookup from ucmm
	* [core]: declare internal openiccStringAddPrintf_
	* [core]: fix openiccMessageFunc definition
	* [core]: add OPENICC_DB_PREFIX
	* [build]: add more internal header paths
	* [core]: add common ucmm files
	* [core]: add OpenICC paths
	* [core]: allocate variable length string one time
	* [build]: use libdir path
	* [conf]: enable silent-rules
	* [conf]: copy missing autotools files
	* [docu]: make example more sound
	* [docs]: fix the example to match the spec
	* [core]: add internal command line maro header
	* [build]: package internal header
	* [core]: add openicc-device util
	* [core]: fix openiccMessage_f definition
	* [core]: add messaging functions
	* [build]: add autotools files
	* [build]: move sources to own directories
	* [docu]: add more documentation files
	* [docu]: add README, COPYING and AUTHORS
	* [core]: add proper header bands
	* [docu]: add initial per function descriptions
	* [exmpl]: add more comments and a new example snippet
	* [docu]: add example JSON
	* [build]: add make clean target
	* [core]: switch to arrays
	* [core]: initial code

### Markus Raab (2):
	fix XDG_CONFIG_DIRS variable name
	fix typo


