#Version 0.9.5

###Andreas Sturmlechner (3):
* [conf]: fix CUPS macro names
* [core]: more typo and grammar fixes
* [core]: Typo fix: s/Promt/Prompt/

###Kai-Uwe Behrmann (349):
* [core]: fix compile warning in oy_io_core
* [core]: remove unused internal Path configuration APIs
* [core]: fix array size
* [docu]: fix typo
* [core]: remove unused oySetDefaultProfileBlock()
* [core]: skip redundant oy_domain_codeset declaration
* [core]: use braces around empty body in debug macros
* [core]: remove duplicate declaration for RegMatch
* [tools]: fix -pedantic compile warnings
* [core]: fix -pedantic compile warnings
* [tools]: check ICC ID in oy-profile -v
* [sources]: support ICC ID in oyProfile_FromFile()
* [alpha]: remove double defines
* [API]: avoid OY_COMPUTE flags ambiguity
* [docu]: add cmake flags section to README
* [sources]: add comments to OY_COMPUTE define
* [sources]: error messages only for wrong hash
* [core]: improve hash error message
* [tools]: adapt to oyConversion_CreateBasicPixelsFromBuffers()
* [core]: add more warnings to profile ID check
* [lcm2]: fix run time warnings
* [conf]: use Libs.private tag in oyranos.pc
* [qarz]: return sRGB for non colour server profile
* [core]: fix wrong free of default profile name
* [lcm2]: use oyLibNameCreate_ to support osX
* [core]: add internal oyLibNameCreate_()
* [sources]: add warning msg to cmm_info->init()
* [exmpl]: compile fix under osX
* [oyX1]: fix osX build
* [oydi]: fix color server activity detection
* [lcm2]: use lib names from cmake for dlopen
* [sources]: skip modules with erroneous init state
* [tools]: remove lcms2 header from oy-icc
* [oforms]: skip alpha header
* [core]: let cmake detect library prefix and suffix
* [sources]: fix compile warning in release mode
* [build]: sync RPM spec file with OBS
* [core]: remove too new attribute
* [core]: guard pthread_mutexattr_setrobust()
* [conf]: remove unused ELEKTRA_LIBS variable
* [conf]: relax about Elektra 0.7 spec requirement
* [conf]: remove ELEKTRA_LIBS from normal flags
* [conf]: support elektra-static in oy-config
* [tools]: fix compiler warning in oy-icc
* [i18n]: update Deutsch
* [tools]: make more strings translateable
* [test]: oyConversion_CreateBasicPixelsFromBuffers() API
* [sources]: use pixel types in oyConversion_CreateBasicPixelsFromBuffers() II
* [lcm2]: fix OpenMP threading issue for proofing
* [lcm2]: support lcms-2.6 cmsContext changes
* [lcm2]: add cmsContext function pointers
* [exmpl]: support -f fallback-icc in oy-test-device and oy-file
* [oyRE]: generate contexts and fallback profile
* [lraw]: remove matrix profile generation from raw-image node
* [docu]: pass all public headers to doxygen
* [oyX1]: let icc_profile.fallback be explicite
* [exmpl]: use "properties" backend call for verbose oy-test-device -l
* [oyRE]: route Configs_FromPattern() through Configs_Modify()
* [exmpl]: allow --device-name in oy-test-device -l
* [docu]: disambiguate description sentence in oyX1
* [exmpl]: show "supported_devices_info" in oy-test-device -v
* [exmpl]: support --device-name option in oy-test-device
* [oyRE]: rename message variable
* [docu]: describe the oy-file --profile option
* [lraw]: write meaningful profile name
* [core]: use posix signal condition with empty workload
* [sources]: unlock previously locked objects
* [sources]: init oyObject_s::_lock
* [oyRE]: set new module_api after API break
* [docu]: fix fragment markup for newer doxygen
* [source]: use pixel types in oyConversion_CreateBasicPixelsFromBuffers()
* [core]: init mutex attribute
* [core]: guard global objetc counter
* [core]: improve thread communication
* [test]: simplify graph call
* [docu]: describe basic function types
* [core]: skip allocation when file size is zero
* [sources]: release oyFilterNode_ToBlob() blob if not set
* [tools]: sync oy-icc -f icc with other formats -n option
* [sources]: move oyFilterGraph_ToBlob to FilterNode
* [exmpl]: keep image_display responsive while editing ICC options
* [test]: sync test_oy with test2 arguments
* [tools]: simplify graph call in oy-icc
* [tools]: oy-profile -lvn reduce prints to stdout
* [test]: increase fuzziness for Lab conversion
* [sources]: fix oyPointer_Set() release object ptr
* [lcms]: dlopen lcms
* [sources]: fix display of locale info in ICC text tags
* [tools]: move debug messages to stderr in oy-monitor
* [sources]: skip repeat context creation in oyConversion_RunPixels()
* [core]: internally expose oyReadFileSize_()
* [core]: fix printf formating for hashes
* [core]: support oyGetTempFileName_( NULL ) file name
* [core]: add internal oyGetTempFileName_()
* [docu]: add doxygen mainpage page name
* [templates]: set const oyStruct_s::type_ member
* [docu]: describe all oyStruct_s members
* [docu]: describe per filter context creation and caching
* [docu]: describe oyCMMapi6_GetConvertF
* [docu]: add object sources to internal Doxyfile
* [docu]: add brief doxygen group description
* [sources]: add oyFilterCore_S/GetBackendContext()
* [test]: select test by number
* [sources]: pass context in oyCMMuiGet_f definition
* [templates]: use type range check in oyCMMapiFilter derived object types
* [templates]: add oyCheckTypeRange_m macro
* [API]: reorder object types
* [sources]: add oyCMMui::parent member
* [core]: improve oyLibPathsGet_()
* [tools]: init OY_DEBUG in oy-xforms-modules
* [sources]: support oySIGNATURE_ATTRIBUTES in oyProfile_SetSignature()
* [sources]: fix wrong pointer casting
* [sources]: fix compiler warning in Hash object
* [templates]: fix modules listing in oy-xforms-moduls -l
* [build]: disable cmml
* [core]: fix compile warnings
* [conf]: fix oy-config script libraries
* [API]: add FilterNode_S/Get[Backend]Context()
* [template]: add CMMapiFilter:runtime_context
* [docu]: describe some Pointer function types
* [docu]: add modules declarations
* [docu]: fix typos
* [docu]: fix typo
* [exmpl]: update to actual GLee
* [oyIM]: fix named color device print out
* [oyIM]: optimise ncl2 reading
* [oyIM]: support ncl2 reading
* [core]: add oyICCGetColorSpaceWithChannelCount()
* [docu]: update alpha documentation
* [examples]: disambiguate file names
* [sources]: dealloc of NamedColors::suf/prefix
* [sources]: fix NamedColors member deallocation
* [sources]: make oyNamedColors_GetColorName() reentrant
* [oyIM]: add partitial ncl2 read support
* [oicc]: split header out
* [core]: relax warning about failed key reading
* [docu]: fix typos
* [oyra]: fix memory leak in scale filter
* [sources]: fix memory leak in oyProfile_GetFileName()
* [core]: fix modf compile warning
* [build]: explicitely link against elektra
* [core]: remove markers
* [core]: make oyKeySetGetNames_ more robust
* [core]: add oyStringListHas_()
* [test]: oyConfigs_FromDB() count
* [core]: relax conditions for oySearchEmptyKeyname_
* [test]: check oyEraseKey_()
* [core]: use local KDB handle in functions
* [core]: reduce internal oyranos_elektra.h includes
* [core]: remove unused internal APIs
* [core]: reduce compatibility macros
* [test]: simplify test key notation
* [test]: fix key overwrite test
* [core]: remove printfs
* [conf]: update development time
* [sources]: use generated oyLis_s code
* [tools]: save device link to file in oy-icc
* [tools]: add --device-link for image conversions in oy-icc
* [docu]: improve on oyConversion_CreateBasicPixelsFromBuffers
* [templates]: add dummy Lis Class
* [API]: spell color
* [sources]: implement private NamedColor functions
* [sources]: inherit NamedColors from List
* [templates]: add List object
* [sources]: add LIST enum
* [sources]: fix uninitialised variable in oyDeviceFromJSON
* [generator]: add listBaseName property
* [templates]: show origin of list member variable
* [build]: ignore missed rpmbuild
* [docu]: update required packages for ubuntu 13.04
* [sources]: fix oyNamedColor_Release()
* [docu]: reorganise Doxygen Module view
* [sources]: add oyNamedColor(s)_s objects
* [sources]: set API break to 0.9.5
* [API]: switch to _MoveInStruct naming convention
* [docu]: improve oyCMMapi4_Create() description
* [exmpl]: check ranges in scale filter
* [exmpl]: add scale to fit to image-display
* [exmpl]: export coordinates in Oy_Fl_Image_Widget.H
* [exmpl]: increment and decrement scaling factor
* [oyra]: implement basic scaling
* [sources]: fix channel -1 parameter oyImage_SetArray2dPointContinous()
* [core]: fix compile warnings
* [docu]: oyImage_G/SetPoint_f
* [sources]: add oyImage_GetSetPointF() API
* [exmpl]: use the scale filter in image-display
* [oyra]: add initial scale filter
* [docu]: move FilterGraph_xxx() to FilterGraph
* [i18n]: update Deutsch
* [source]: expose oyConnectorImaging_SetDataTypes()
* [docu]: describe oyCMMapi7_Create()
* [source]: add oyCMMapi7_Create()
* [source]: use public Connector copy function
* [source]: fix oyConnectorImaging_SetCapability()
* [exmpl]: fix crash through missed image in image-display
* [sources]: add oyObject_GetAlloc() _GetDeAlloc()
* [core]: allow oyStringListAdd_(n) to be NULL
* [source]: add oyConnectorImaging_SetDataTypes()/_SetCapability()
* [source]: make oyCMMapi4_Create(param context_type) optional
* [source]: add oyCMMui_Create()
* [sources]: complete oyCMMapi4_Create()
* [docu]: add link to registration string description
* [sources]: add oyCMMapi_Set + oyCMMapi_SetNext APIs
* [sources]: add oyCMMinfo_SetInitF() API
* [core]: correct some strings in oyCMMgetImageConnectorPlugText()
* [sources]: add oyCMMapi4_Create()
* [sources]: add oyCMMinfo_s::init to create runtime modules
* [test]: fix displaying of connector names in "CMMs"
* [templates]: move oyConnector_s code to template
* [exmpl]: fix ICC profile opening from imagename with spaces in image_display
* [tools]: add oy-xforms-modules -l option
* [core]: add warnings for missed device modules
* [conf]: a Xcm cmake macro is provided by the libXcm package
* [oyX1]: add more error messages
* [tools]: add version info to verbose output
* [build]: fix has insecure RUNPATHs
* [build]: synchronise -fPIC switch
* [build]: complete uninstall target
* [build]: fix local yajl static linking through -fPIC
* [build]: omit overwriting of desktop file
* [tools]: oy-profile add PCS count example
* [source]: omit profile with error
* [test]: reduce test time for named colours
* [core]: fix double object release
* [oyX1]: return issue for missed monitor object
* [core]: add oyCheckType_ for debugging
* [oyX1]: fix array access
* [lcm2]: improve context verification and relax
* [sources]: improve cache printer
* [core]: get old behaviour from oyStringSegments_()
* [core]: replace XYZ.icc with debian versions
* [core]: replace XYZ.icc with debian versions
* [test]: print test numbers
* [lcm2]: skip node id for context verification
* [cmms]: relax warnings
* [source]: renew hash after call to module
* [lcm2]: verify context by options
* [oicc]: modularise oiccConversion_Correct()
* [core]: fix zero string length segment count
* [core]: fix oyStringSegment_() crash
* [oicc]: add node to debug output to _Correct()
* [lcm2]: add debug hash + oyDL ID to CMM context
* [core]: switch to MD5 hashing
* [sources]: add oyPointer_G/SetId()
* [sources]: add oyPointer_s_::id member
* [templates]: support more PS viewers
* [templates]: show node options in oyFilterGraph_ToText
* [core]: add internal oyStringReplace_ helper
* [docu]: fix oy-icc options description
* [i18n]: reduce strings
* [core]: add debug output to Url helper
* [oydi]: skip initialisation for unprepared context
* [tools]: omit header for oy-profile -p option
* [tools]:  print header info in oy-profile
* [i18n]: update
* [sources]: support new enums in oyProfile_GetSignature()
* [core]: add new signature enums
* [core]: document swap functions
* [tools]: add oy-profile -i options
* [test]: relax monitor JSON issue
* [test]: allow reduced result lines to stdout
* [test]: adapt Interpolation tests to internal API change
* [oyjl]: 1.x compile fixes
* [tools]: add oy-profile-graph --illuminant option
* [core]: simplify value range in interpolator
* [core]: add CIE A and CIE D65 SPD tables
* [tools]: increase black body white point line precission
* [i18n]: update Deutsch
* [docu]: unify usage message
* [i18n]: add comments to po files
* [sources]: tolerate error in oyRankMapFromJSON()
* [test]: add testCMMRankMap()
* [sources]: add oyRankMapFromJSON() API
* [exmpl]: add oy-test-device -f openicc+rank-map option
* [sources]: adapt to internal oyjl API changes
* [oyjl]: make some APIs internally available
* [oyjl]: readd oyjl_value_text()
* [tools]: CMF colourise Planck spectra in oy-profile-graph
* [exmpl]: comment formula for Plancks radiation
* [tools]: accept CMM options in oy-icc
* [build]: link oy-icc against oforms
* [docu]: fix oy-file cli options
* [core]: rename 1964 CMF
* [core]: add black body white point data set
* [build]: adapt makefiles for oyjl
* [oyjl]: switch to yajl-2.0 tree struct
* [oyjl]: improve arg parsing
* [oyjl]: add oyjl_tree_to_json()
* [oyjl]: fix integer printing
* [source]: fix typo
* [CUPS]: add keys from PPD dynamically to rank map
* [exmpl]: fix device class in oy-test-device -f openicc-rank-map
* [sources]: add oyRankMapAppend API
* [sources]: fix counting in oyRankMap data
* [tools]: contrain central axes to L*ab graphs
* [i18n]: update Deutsch
* [docu]: update oy-profile-graph options
* [tools]: add oy-profile-graph --kelvin option
* [core]: add new linear interpolator types
* [core]: rename CMF arrays
* [core]: add basic internal ramp manipulations
* [i18n]: update Deutsch
* [core]: improve warnings for cli float parsing
* [tools]: oy-profile-graph --standard-observer is 1931
* [core]: add CIE 1931 CMF data
* [core]: add internal oyLinInterpolateRampU16()
* [exmpl]: fix oy-test-device --list-profiles option
* [oyX1]: set xcalib to screen if XRandR >= 1.2
* [oyX1]: use more internal XRandR information
* [source]: fix profile tag size in oy-profile -l
* [docu]: fix small typo
* [exmpl]: add -f openicc-rank-map to oy-test-device
* [sources]: add oyRankMapToJSON() API
* [exmpl]: add -f openicc-rank-map option to oy-file
* [core]: add icSigVideoCardGammaTable to oy_icc.h
* [tools]: fix oy-profile-install with absolute path
* [exmpl]: improve oy-test-device docu
* [i18n]: update Deutsch
* [exmpl]: fix compiler warning in oy-test-device
* [docu]: -f options for oy-file
* [lcms]: fix typo in macro name
* [core]: fix compiler warning
* [exmpl]: add oy-test-device -j to -f openicc
* [sources]: add device_class key in oyDeviceFromJSON()
* [exmpl]: polish oy-file options
* [source]: add warning to oyImage_ToFile()
* [oPNG]: use FilerNode options for "filename"
* [exmpl]: reduce code in oy-test-device
* [oPNG]: add error messages
* [oyra]: enrich error messages
* [CUPS]: fix description string for oyNAME_NAME
* [tools]: fix empty line for oy-monitor -l -d 0
* [lcm2]: optimise source image with gamma_linear="1"
* [lraw]: tag image with gamma_linear=1
* [lcms]: fix compiler warning
* [exmpl]: fix use of option in oy-test-device
* [exmpl]: add --only-db option to oy-test-devices
* [sources]: add "source" option for oyDeviceFromJSON()
* [exmpl]: polish oy-test-device options
* [oyRE]: fix device_class to camera
* [tools]: add more verbosity to oy-monitor -v
* [docu]: add cmake/COPYING-CMAKE-SCRIPTS
* [build]: fix debian changelog
* [build]: update debian changelog for 0.9.5
* [build]: be specific in deb about installed files
* [build]: remove outdated shlibs.local
* [tools]: improve image comment in oy-icc
* [oyra]: support comment option in ppm file_write node
* [conf]: bump version

###Markus Raab (1):
* [core]: support Elektra-0.8

###Peter Sarva (1):
* [conf]: fix Oyranos can not find LCMS, LCMS2 and Exiv2.

###Yiannis Belias (1):
* [sources]: fix potential crash in oyProfile_GetDevice()

#Version 0.9.4

###Kai-Uwe Behrmann (42):
* [qarz]: fix setting of monitor profiles
* [exmpl]: fix GL variable type conversions warnings
* [conf]: better detect iconv/libintl and GetText
* [core]: fix getting current policy on osX
* [core]: simplify string allocation
* [core]: honour user editing profiles on osX
* [core]: use available Lab from debians icc-profile II
* [oydi]: fix error message
* [tools]: fix argument parsing in oy-policy
* [conf]: omit not detected include variables
* [exmpl]: fix ARB pointer definition
* [oydi]: fix Linux Xcm includes
* [exmpl]: various compile fixes around Xcm for osX
* [qarz]: port the CS module to the new APIs
* [build]: link against Carbon framework on osX again
* [build]: link oyranos_cs.c in liboy_config
* [qarz]: include missed header
* [conf]: add cairo to osX include
* [core]: include locale.h properly
* [conf]: add HAVE_LOCALE_H
* [build]: add cmake version to yajl CMlists.txt
* [conf]: alternate grantlee download with curl
* [build]: interal yajl if needed
* [conf]: use CFLAGS in configure script
* [conf]: detect ltdl.h and intl,iconv libs
* [core]: fix alternate strcasecmp
* [conf]: skip already placed system macro
* [conf]: skip X11 extension checks on non X systems
* [conf]: skip no longer used config files
* [conf]: use pkg-config on win32
* [i18n]: fix typo
* [conf]: update debian/changelog to 0.9.4
* [build]: relax cmake requirement for ExternalProject
* [core]: add oyWIDGET_GROUP_DEVICES/INFORMATION + i18n
* [sources]: add oyProfileSignatureName()
* [sources]: add oySIGNATURE_ILLUMINANT header sig
* [core]: include config header to see USE_GETTEXT
* [docu]: update and format copying files
* [sources]: write n channel images
* [conf]: support cmake 2.6.4
* [conf]: build without LibRaw
* [docu]: update AUTHORS

#Version 0.9.3

###Kai-Uwe Behrmann (19):
* [docu]: update ChangeLog
* [core]: use available XYZ/Lab from debians icc-profiles
* [build]: fix html installation
* [conf]: bump version
* [docu]: update ChangeLog
* [tools]: fix compile warning
* [conf]: support configure --enable-debug option
* [sources]: initialise memory for strtod
* [tools]: show initial XCM status in qcmsevents
* [tools]: fix derived QtApplication class constructor
* [tools]: set Qt app resources initialisation
* [oyX1]: avoid XRandR expensive call again
* [tools]: support download of ICC profiles in oy-profiles
* [tools]: add oy-profile --ppmcie options
* [tools]: fix oy-monitor running without DISPLAY
* [oyjl]: carefully release yhandle
* [souces]: use oyEDITING_XYZ instead of "XYZ.icc"
* [lcms]: fix crash over missed output image
* [build]: gracefully fail on docu installation

#Version 0.9.2

###Adrian Page (1):
* [conf]: add FindLCMS.cmake

###Kai-Uwe Behrmann (208):
* [tools]: add oy-profile -f xml option
* [build]: add missed directory ownership
* [conf]: install OyConfig.cmake into lib path
* [build]: reduce linked libs per sub library
* [conf]: tell about all oyranos libs in FindOy.cmake
* [build]: omit creating unneeded directories
* [build]: package cmake file in debian
* [conf]: fix FindOyranos.cmake.in names
* [build]: require icc-profiles-free in debian
* [exmpl]: colour qcmevents icon when conversions occure
* [oydi]: fix conversion for region unaware colour server
* [sources]: add profile name aliases for Debian
* [oyX1]: remove XRRCrtcGamma check
* [build]: package translations files for debian
* [build]: add LibRaw + exiv2 dependenies to Fedora spec
* [build]: rename oy-dev to liboy-dev debian package
* [build]: disable oy-config-ltk on debian package
* [build]: skip non needed dependencies
* [build]: add liboyranos debian package
* [build]: add generated include path for internal elektra
* [build]: resort test target
* [conf]: add libraw -fPIC test
* [tools]: show profile ID with oy-profile -m option
* [build]: remove SO_VERSION and document
* [build]: let gcc use -fPIC
* [lraw]: fix includes
* [build]: tell in debian/rules to use cmake
* [conf]: use correct libraw include variable
* [docu]: add cmake to build dependencies in ubuntu
* [conf]: check for yajl_version.h
* [build]: rename elektra subpackage
* [build]: fixes for debian target
* [build]: fix small typo in rpm target
* [build]: use static paths in spec file
* [build]: add debian targets
* [oyX1]: support pur XRandR without Xinerama
* [conf]: use fltk-config --ldflags for FLTK deps
* [build]: adjuts cmake path in spec file
* [oyX1]: link to xxf86vm
* [build]: install cmake in proper place
* [core]: fix compiler warnings
* [conf]: add BuildRequires:  Mesa-libGLU-devel
* [conf]: skip env CFLAGS handling
* [build]: generate custom objects
* [build]: elektra allways
* [build]: add objects to library
* [build]: enable verbose RPM builds
* [build]: add libxml dependency to internal elektra
* [sources]: fix compiler warnings
* [core]: rename config.h to oy_config_internal.h
* [build]: add BuildRequires:  cmake
* [build]: internal elektra
* [oyX1]: use RR_Rotate_0 event
* [conf]: add versions to FindOyranos.cmake.in
* [exmpl]: use the installed icon in qcmsevents
* [oyX1]: remove oy-monitor-nvidia and XNVCtrl
* [build]: use old pixmap install path
* [conf]: add XDG_CONFIG_DIR cmake variable
* [build]: install oy-config-fltk
* [build]: fix xdg-autostart installation in spec file
* [build]: istall docu in proper place
* [build]: static libraries
* [build]: add so name versioning
* [build]: install oy-xforms-modules files
* [build]: install docu
* [build]: install docu
* [build]: install icons again
* [build]: adapt paths for extra install files
* [build]: avoid path overwriting
* [build]: remove parallel flags from spec file
* [conf]: allow insource builds
* [build]: reenable translations in image-display
* [build]: put oforms binaries near image-display
* [build]: simplified src/examples/oforms
* [build]: clean directory before building a tar ball
* [conf]: remove old build system files
* [build]: install two more man pages
* [conf]: write spec file
* [i18n]: simplify path names
* [build]: simplify gettext usage
* [conf]: use local executeable
* [conf]: fix gettext detection
* [i18n]: generate pot, translate po and install gmo files
* [conf]: add FindGetText.cmake for more verbosity
* [build]: install oy-icc
* [exmpl]: remove unused program
* [build]: install settings
* [conf]: remove unneeded cmake file
* [conf]: use system FindX11.cmake
* [build]: examples
* [build]: cmake formating
* [conf]: fix Yajl detection
* [core]: move WIN32 helper
* [build]: install hand written man pages
* [docu]: move man pages to doc/
* [docu]: fix typo
* [build]: readd docu from sources
* [build]: move doxygen files
* [build]: expose documentation sources
* [conf]: use FindLibXml2 non canonical include dir
* [conf]: use pkg-config in FindYajl.cmake
* [conf]: relax about version macro in lcms/2 cmake
* [build]: install oy-monitor-daemon
* [build]: remove unused cruft
* [build]: mv oy.pc.in to extras/
* [build]: install scripts
* [build]: install qcmsevents svg icon
* [build]: fix linking against OpenMP for lraw module
* [build]: qcmsevents
* [build]: oy-profile-graph
* [build]: simplify include_directories
* [build]: install some missing headers
* [build]: fix wrong installation variables
* [build] tools
* [conf]: require libXcm
* [conf]: minimalise configure script
* [test]: build test2 and add test-2 target
* [lraw]: fix linking
* [core]: add private oforms print helpers to liboy_modules
* [tools]: adapt to new config.h macros
* [test]: beautify test_oyranos output
* [build]: device modules
* [conf]: add FindCUPS.cmake
* [oyX1]: fix changed config macros
* [core]: readd OY_BINDIR macro
* [conf]: add FindExiv2.cmake and simplify
* [conf]: remove non supported macros from config.h
* [conf]: add X11, Xinerama, Xxf86vm and simplify
* [conf]: fix lcms version detection
* [build]: src/modules
* [build]: add RunTest.cmake wrapper for test_oyranos
* [conf]: move dependency checks up for config.h
* [conf]: massage config.h.in to match cmake macro names
* [conf]: add FindXfixes.cmake
* [conf]: add LCMS2.cmake and LibPNG.cmake
* [conf]: add PkgConfig section to FindLibRaw.cmake
* [conf]: isolate FindLibRaw.cmake II
* [conf]: isolate FindLibRaw.cmake
* [build]: remove double library detection
* [conf]: use cmake variables names in header
* [conf]: simplify platform detection code
* [build]: add test target
* [build]: move more code into src/liboy
* [build]: add platform test
* [test]: remove non available git variables
* [conf]: move devel date variables to top CMakeLists
* [build]: port liboy section to cmake
* [build]: remove wrong file listing
* [build]: fix dependency order
* [build]: port liboy_modules section to cmake
* [build]: add test-modules and test-config targets
* [build]: add liboy_config to cmake subdirectories
* [build]: move oyjl/ to src/
* [build]: move liboy_config files to src/
* [build]: post liboy_config section to cmake
* [build]: pass detected variables for xml2
* [conf]: add FindElektra.cmake
* [build]: mark install headers
* [build]: add test-object target
* [build]: check for libxml2
* [build]: add GENDIR variable
* [build]: port liboy_object section to cmake
* [build]: add PACKAGE_NAME PACKAGE_DESCRIPTION to cmake
* [build]: fix cmake for moved generated file path
* [docu]: correct include path
* [build]: mv remaining generated files to src/
* [build]: mv most generated files to src/
* [build]: move generated sources to common location
* [build]: switch to Debug target for oyAPIGenerator
* [build]: download and build grantlee in build path
* [generator]: debug template list
* [build]: add generate target
* [generator]: fix grantlee plugin lookup paths
* [build]: remove unneeded cmake parts
* [core]: add public lib capabilities header template
* [build]: add liboy_core cmake file
* [build]: add cmake macros and templates
* [build]: set test-core dependency
* [core]: move generator directory
* [sources]: merge directories
* [build]: add test-core target
* [conf]: add basic checks
* [build]: overhaul dist target
* [conf]: add local include paths
* [conf]: adapt config.h to cmake
* [conf]: add first version of configuration logging
* [build]: move test_core.c
* [core]: omit informations from git
* [build]: move files for liboy_core
* [build]: move sources and templates
* [build]: adapt debian control to ubuntu 12.04
* [generator]: support multiple output paths
* [conf]: fix lcms detection issue on debian
* [conf]: relax about lcms2 requirement
* [build]: add debsrc target
* [build]: add debian make target
* [build]: simplify fl_i18n clean
* [lcm2]: use per transform adaption state
* [exmpl]: install qcmsevents desktop file
* [build]: compile internal elektra before oy_elektra.o
* [docu]: update ubuntu build section
* [oyIM]: fix pseq crash
* [build]: adjust make distclean
* [build]: map test to check make target
* [build]: sync spec file with obs
* [conf]: bump version
* [docu]: show profile assignment after download from Taxi
* [sources]: add profile filename after installation

#Version 0.9.1

###Kai-Uwe Behrmann (26):
* [build]: package oy-icc for spec file
* [docu]: fix typo
* [exmpl]: adjust oy-profile-graph graphics
* [test]: check for zero string length
* [docu]: update doxygen page footer
* [core]: use oyProfile_Install() in oy-profiles
* [sources]: add oyProfile_Install()
* [sources]: add some error numbers
* [exmpl]: fix test_device print out
* [sources]: move oyDevicesFromTaxiDB() to oy-devices.h
* [core]: add oyStringCaseCmp_()
* [exmpl]: show Taxi DB search result for device
* [build]: install oy-icc
* [exmpl]: build taxi example
* [exmpl]: add taxi client
* [core]: add debug output
* [build]: build internal elektra before linking
* [build]: package yajl version include
* [core]: add debug infos for elektra
* [docu]: fix typo
* [docu]: reorder, describe and add code snippets
* [conf]: bump version
* [sources]: fix negative array offsets
* [core]: move debug message before critical call
* [oPNG]: fix to read palette PNGs

#Version 0.9.0

###Jean-Sebastien Pedron (10):
* [build]: FreeBSD doesnt use $PREFIX/lib64, only $PREFIX/lib
* [build]: Put source and build include/lib directories before system directories
* [build]: Pass $(LDFLAGS) and $(QT) to examples/makefile
* [build]: Create xcmsevents directory in examples/makefile
* [build]: Dont build qcmsevents if Qt isnt available
* [build]: Fix moc-qt4 detection
* [build]: Dont build image_display if FLTK isnt available
* [build]: allow one to specify the make(1) executable
* [build]: "make depend" requires -I$(SRCDIR)/oforms
* [build]: create tests/ if build dir != source dir

###Joseph Simon (2):
* [CUPS]: add getOyConfigPrinter_+ resetPPDChoices_
* [docu]: typo fixes and make the document flow better

###Kai-Uwe Behrmann (531):
* [sources]: replace oyProfile_DeviceGet oyProfile_DeviceAdd
* [docu]: update required versions
* [build]: package image_display header
* [build]: update spec file
* [exampl]: package test_device.c
* [docu]: skip default image pixel access functions
* [docu]: improve image enum descriptions
* [docu]: generate and update Doxyfiles
* [docu]: describe oy_debug variable
* [docu]: clean up README
* [core]: split cli macros out
* [build]: add all include paths to examples
* [build]: compile examples test-device
* [exmpl]: adapt test_device to API changes
* [conf]: update configuration script
* [conf]: fix script warning
* [oyX1]: fix OY_ICC_xxx usage
* [build]: fix link order for examples
* [build]: fix lib link path for oforms
* [i18n]: fix german translations
* [API]: use XCM macros for X Color Management names
* [sources]: remove oyImage_GetPoint()
* [lraw]: generate ICC profile from camera matrix
* [oyRE]: silence debug output
* [core]: use verbosity variable in oy-profile
* [oyIM]: read icSigXYZType
* [core]: reduce debug noice
* [core]: add new debug output macros
* [lraw]: be explicite to oyOptions_Handle()
* [test]: use liboy_config calls in test_config.c
* [test]: advice oyOptions_Handle() more explicite
* [oyRE]: skip Olympus.SerialNumber for exiv2-0.23
* [build]: build fix for make check
* [core]: more caching of module accesses
* [core]: skip module lists and rank_map
* [lraw]: copy matrix operators
* [build]: run make check without installing
* [build]: add check-long target
* [sources]: fix 4.7.1 warnings
* [templates]: remove duplicate file
* [docu]: update README build instructions
* [sources]: fix useless warnings
* [lcm2]: set the default options to 0
* [exmpl]: use advanced options for nodes
* [oicc]: simplify oiccConversion_Correct()
* [templates]: reduce Elektra calls in oyOptions_ForFilter_
* [sources]: initialise Image::layout_ member
* [sources]: fix crashes in oyFilterSocket_SetData()
* [sources]: fix oyArray2d_Reset() for oyUINT8
* [API]: update generated code
* [sources]: export oyDataTypeGetSize + oyDataTypeToText
* [exmpl]: revert include in image_display
* [core]: fix lots of compile warnings
* [build]: fix oy-profile-graph compilation
* [exmpl]: fix crash in image_display --shader
* [exmpl]: oymd5icc add --short option
* [core]: oy-profiles check internal name extension
* [lraw]: fix filename passing in
* [lcm2]: add debug output to track arrays
* [oydi]: keep track of a modified ticket array
* [sources]: oyImage_FillArray reuses provided array
* [sources]: add oyArray2d_Reset()
* [oPNG]: synchronise to mster
* [exmpl]: unref used objects
* [oydi]: fix graph initialisation
* [core]: fix geometry exchange
* [cmms]: use FilerNode options for "filename"
* [sources]: add small debug info
* [sources]: properly initialise static object
* [oydi]: run filter with plug
* [sources]: warning about filter run without plug
* [sources]: initialise FilterNode::tags
* [exmpl]: declare debug symbol
* [build]: omit includedir/alpha/object
* [sources]: add StructList based cache API
* [sources]: rename oyHash_Get()
* [sources]: unhide oyConversion_GetGraph()
* [build]: fix installation of oyPixelAccess_s.h
* [templates]: move module declarations to private headers
* [sources]: fix compiler warning
* [oforms]: fix compilation
* [core]: register oyOBJECT_CMM_API_MAX
* [templates]: move internal header out of public
* [sources]: fix empty meta values
* [templates]: remove last occurence of UNHIDE_CMM macro
* [build]: pass Qt to examples make
* [build]: fix examples building
* [build]: install stable headers
* [build]: add new files to spec
* [build]: prepare for packaging
* [oforms]: minor compile fixes
* [exmpl]: adapt oy-convert to changed API
* [core]: remove config.h guards in oy_debug.h
* [exmpl]: minor compile fix for oy-profile-graph
* [exmpl]: adapt to generated object types oy-file
* [exmpl]: minor compile fix for oymd5icc
* [lcms]: remove non needed include
* [exmpl]: adapt to generated object types dFil
* [sources]: add oyArray2d_GetType()
* [exmpl]: adapt to generated object types dDev
* [exmpl]: fix includes in image2pdf
* [core]: skip the oyranos/ include prefix
* [build]: add libs to pc file
* [exmpl]: adapt to generated object types
* [sources]: check before mem access in FilterSocket_GetData
* [sources]: add missed image file functions
* [sources]: add PixelAccess::start_xy_old accessors
* [core]: canonise dot paths
* [sources]: add missed files
* [templates]: add missed files
* [generated]: add oy_profile.h
* [generated]: add code
* [API]: change environment variable prefix OYRANOS_xxx to OY_xxx
* [SANE]: adapt to generated object types
* [templates]: add oy_conversion_internal.template.h
* [core]: use complete type for static object
* [lcm2]: use correct geometry API
* [test]: fix "CMM devices details" test
* [lcm2]: set use of psid tag
* [sources]: return references objects
* [oyIM]: fix recently introduced wrong call order
* [cmms]: release referenced objects
* [templates]: relaxed type checking for Connector derived functions
* [templates]: allow overriding type checks in BaseList
* [templates]: allow overriding type checks in Base
* [templates]: add oyOBJECT_CONNECTOR_MAX_S
* [sources]: readd oyStruct_Allocate()
* [cmms]: update to changed oyProfiles_MoveIn() API
* [sources]: allocate list before oyProfiles_MoveIn
* [templates]: skip type check in oyCMMapiFilters_Get()
* [cmms]: update module_api
* [oyra]: replace oyraFilterNode_ImageRootContextToMem
* [test]: use short summary for incompatible CMMs
* [sources]: set API break formodules
* [core]: switch from OY_MODULE_PATHS
* [conf]: bump version to 0.8
* [templates]: add oyCMMapis_s.template.c
* [templates]: add Get_RefType block
* [templates]: catch missed api5
* [generator]: fix ambiguity as mentioned by gcc
* [build]: use CMAKE_PREFIX_PATH to detect grantlee
* [build]: switch to grantlee 0.2.0
* [test]: fix static object usage
* [sources]: fix Array2d accessor argument usage
* [sources]: set static object type
* [templates]: check arguments in StructList
* [templates]: add cpp "C" bands to private headers
* [build]: link correctly test2
* [test]: adapt to generated object types
* [test]: add tests for linking
* [build]: omit imaging and build config lib
* [sources]: rename oyRectangle_SamplesFromImage()
* [build]: move Profile out of liboy_object
* [test]: adapt to generated object types II
* [alpha]: remove some oy_alpha.h includes
* [sources]: rename oyOption_StructGet
* [sources]: move oyOptions_ForFilter implementation
* [core]: clean oy_alpha_internal.h
* [test]: adapt to generated object types
* [sources]: add oyArray2d_GetWidth/Height()
* [core]: oy-profile adapt to generated object types
* [sources]: add oyProfile_GetSize()
* [lraw]: adapt to generated object types
* [qarz]: adapt to generated object types
* [oyX1]: adapt to generated object types
* [oyRE]: adapt to generated object types
* [SANE]: adapt to generated object types
* [CUPS]: adapt to generated object types
* [lcms]: adapt to generated object types
* [oyra]: adapt to generated object types II
* [oyra]: adapt to generated object types
* [sources]: add oyPixelAccess_SetOutputImage()
* [sources]: fix typo
* [sources]: add more FilterNode accessors
* [sources]: add oyFilterCore_GetRegistration()
* [sources]: add oyArray2d_GetGeo1()
* [sources]: change oyProfileTag_Create() API
* [sources]: add oyConfig_GetOptions()
* [templates]: rename oyRankPad to oyRankMap
* [sources]: fix compile warning
* [oPNG]: fix some includes
* [templates] add oy_conversion
* [oydi]: adapt to generated object types
* [sources]: add FilterGraph accessor
* [oPNG] fix for palette PNGs crash
* [oPNG]: adapt to generated object types
* [sources]: modify oyImage_SetCritical() behaviour
* [sources]: initialise Image::tags
* [sources]: clearify oyFilterNode_Disconnect()
* [sources]: add new accessor to FilterSocket
* [sources]: adjust private Connector declaration
* [sources add new accessor to Array2d
* [sources]: modify oyRectangle_SetGeo1
* [sources]: add new Rectangle setter
* [sources]: add oyCMMapi7_Run + oyFilterNode_Run
* [oicc]: compile fixes
* [sources]: more accessors for FilterGraph FilterPlug
* [oyIM]: compile fixes
* [oyIM]: compile without ProfileTag internals
* [sources]: make CMMinfo::Icon static
* [sources]: add a few accessors to Profile
* [sources]: add missing member
* [core]: declare oyStruct_Allocate()
* [core]: objects may be omitted
* [lcms]: adapt to oyImage_GetPixelLayout API
* [build]: add Conversion
* [build]: add Connector ConnectorImaging
* [sources]: remove UNHIDE_CMM macro
* [build]: FilterCore FilterNodes oyFilterPlug(s)
* [build]: add FilterNode FilterSocket
* [templates]: move oyCMMsGetFilterApi_ out of alpha
* [build]: add FilterCore
* [build]: add CMMobjectType and Array2d
* [build]: add PixelAccess
* [build]: add CMMapiFilter
* [sources]: adapt Image type fcuntions to internal API
* [templates]: move oyICCColourSpaceToChannelLayout to oy_image.h
* [build]: add Image
* [build]: add liboyranos_imaging.a
* [templates]: fix variable definitions
* [build]: oy_conversion
* [sources]: fix CMMinfo compiling
* [sources]: rename CMMInfo to CMMinfo II
* [build]: add CMMinfo + CMMui
* [sources]: rename CMMInfo to CMMinfo
* [sources]: fix declaration of oyCMMui_GetTexts()
* [templates]: use oyIsOfTypeCMMapiFilter from oy-modules-internal.h
* [build]: add CMMapiFilters
* [core]: use new member access functions in oy-gamma
* [sources]: out includes into public_methods_decl
* [sources]: move some types out of provate headers
* [sources]: fix compile warnings
* [sources]: some simple compile fixes
* [sources]: oyCMMCanHandle_f is public
* [sources]: add missed declarations to Config
* [core]: avoid alpha dependencies for xml parsing
* [build]: compile various new objects
* [core]: simplify the oy-profiles example
* [sources]: update API_generated files
* [templates]: merge object functions
* [templates]: add missed headers
* [templates]: use new accessors in oyranos_modules
* [templates]: move oyFilterRegistrationModify
* [sources]: add accessors to FilterNode
* [sources]: add accessors to CMMui
* [sources]: add accessors to CMapi9
* [sources]: add accessors to CMMapi8
* [sources]: add accessors to CMMapi6
* [sources]: add oyCMMapi3_GetCanHandleF
* [sources]: add accessors to CMMapi
* [sources]: add accessors to CMMInfo
* [sources]: add member accessor to CMMobjectType
* [alpha]: move APIs out from oyranos
* [build]: new files
* [templates]: add oyranos_devices files
* [templates]: add oyranos_image files
* [core]: remove HAVE_LCMS bands from oyranos_icc.h
* [templates]: move code out of alpha
* [build]: translate oyranos_module.c
* [sources]: rename oyFilterNode_ContextSet_
* [sources]: update oyFilterNode_ContextSet_()
* [build]: honour build order for liboyranos_modules
* [sources]: rename DataGet to GetData
* [sources]: update oyFilterNode_DataSet
* [alpha]: move some more generated functions out
* [templates]: declare oyConnectorEventToText()
* [sources]: readd younger API clean
* [sources]: move oyConversion_CreateFromImage out
* [sources]: cloack class variable name
* [alpha]: move code out of alpha
* [sources]: move oyImage_WritePPM out of alpha
* [sources]: move oyImage_FillArray out of alpha
* [sources]: move oyStruct_GetId from alpha
* [alpha]: remove CMMInfo and CMMapis from alpha
* [sources]: move oyStruct_GetId to oyStruct_s
* [core]: oyBigEndian is better declared in core
* [core]: move low level colour APIs to own files
* [alpha]: move out Rectange API
* [gen]: update generated code
* [core]: use new member accessors
* [core]: move out oyObjectInfoStatic_s
* [templates]: move oyLAYOUT_e to oyranos_image.h
* [sources]: add oyRectangle_GetGeo1 to oyRectangle_s
* [sources]: add member accessors to oyPixelAccess_s
* [sources]: add member accessors to oyConnectorImaging_s
* [sources]: add accessors to oyImage_s
* [sources]: make oyImage_s pixel functions public
* [sources]: move oyImage_CombinePixelLayout2Mask_
* [sources]: add member accessors to osFilterSocket_s
* [sources]: add member accessors to oyFilterPlug_s
* [sources]: add new member accessors to oyProfile_s
* [sources]: sync oyProfile_s functions with master
* [sources]: update to newer oyTextIccDictMatch
* [templates]: add missing includes
* [core]: update oyStruct_GetInfo to new objects
* [core]: the dummy for ld is no longer needed
* [build]: add debug flags to oyAPIGenerator
* [core]: prepare for merge with master
* [alpha]: add "set_device_attributes" flag to oyProfile_DeviceAdd()
* [core]: fix memory leak in oyMakeDir_
* [yajl]: add oyjl_value_text() API
* [build]: fix spec file to match openSUSE packaging
* [CUPS]: fix C build warnings
* [build]: move file to match module sources
* [build]: fix liboyranos_module linking
* [build]: fix yajl on osX
* [build]: fix conflicting make variable
* [build]: require libraw and libyajl only >= oS-11.4
* [i18n]: update deutsch
* [docu]: fine tune help text
* [build]: if defaulting to detected external yajl
* [build]: copy yajl into sources and link if needed
* [yajl]: add internal version based on yail 1.0.12
* [conf]: detect a missed yajl and use internal one
* [i18n]: update deutsch in parts
* [core]: make oyranos-config-fltk help window resizeable
* [alpha]: fix over sensible error message
* [docu]: add oyranos-profile-install man page
* [conf]: hide oyranos-profile-install.desktop from menu
* [conf]: more robust yajl version detection
* [core]: fix typo in ICC tag name
* [i18n]: add new texts to oyranos.pot
* [core]: show help text by F1 in oyranos-config-fltk
* [core]: export HTML over oyranos-policy --help
* [core]: export core option desriptions to HTML
* [core]: add oyDescriptionToHTML() API
* [docu]: describe reason, why a missed ICC profile is bad
* [core]: fix string lenght in oyStringAddPrintf_()
* [docu]: fix man page typo
* [core]: remove unused path configuration code in oyranos-config-fltk
* [oforms]: remove hint to non yet available man pages
* [oforms]: support --help by oyranos-xforms
* [core]: use tooltips for oyranos-policy dump
* [i18n]: update deutsch in parts
* [core]: add long descriptions to core options
* [oforms]: remove printf
* [oforms]: set correct default choice
* [oicc]: set description as xf:help for xf:select1
* [oforms]: show description of headlines
* [oforms]: use xf:group labels on cmd line
* [oforms]: show the xf:group labels
* [lcm2]: add help descriptions to specific options
* [lcms]: add help descriptions to specific options
* [oicc]: move h3/h4 elements to <xf:group type=[h3,h4]>
* [core]: add a oyOption_t_::tooltip and make descriptions accessible
* [core]: add oyXFORMsModelGetAttrValue()
* [docu]: add dependency links
* [docu]: update some links
* [docu]: actualise doxy files
* [exmpl]: add second rendering for first display
* [exmpl]: omit non converted frames in GL
* [exmpl]: trim color region to max image size
* [exmpl]: preserve image tags
* [oyra]: check for missing file extension
* [exmpl]: optimise image texture size
* [exmpl]: draw only image area
* [alpha]: fix roi scaling in oyFilterPlug_ImageRootRun()
* [alpha]: catch wrong condition in oyImage_FillArray()
* [exmpl]: limit pixel access to image dimensions
* [exmpl]: center texture image
* [exmpl]: add OY_MAX macro
* [alpha]: add debug info about rois
* [core]: add DBG7_S macro
* [exmpl]: move ticket to Oy_Widget::pixel_access
* [exmpl]: disable write of debug images
* [exmpl]: support multi monitors in display module
* [exmpl]: move with source image extents
* [exmpl]: fix clut channel order BGR in o-icc
* [oyra]: fix image write back
* [exmpl]: allow display filter in non icc DAGs
* [alpha]: fix oyConversionShow_() output
* [alpha]: use DBGs_S macros in lcm2, oydi and oyra
* [core]: add DBGs_S(struct) style macros
* [exmpl]: fix texture showing
* [alpha]: warn about missed socket in node
* [exmpl]: display more detailed graphs
* [exmpl]: improve debug messages
* [exmpl]: update help text for image_display
* [alpha]: add more debug output for oy_debug_memory
* [core]: use getenv variable only if found
* [core]: adjust oyStringToDouble to glibcs strtod
* [core]: warn about vsnprintf issues
* [core]: use OY_PRINT_POINTER instead of %tX
* [core]: disable a oy_debug_memory warning
* [core]: add OY_PRINT_POINTER macro
* [oyIM]: set string end
* [alpha]: warn about no profile for device class
* [test]: add string list test
* [alpha]: improve messages
* [i18n]: update deutsch
* [lcm2]: support pre+post CLUT curves
* [exmpl]: fix help text of o-icc
* [oPNG]: fix wrong memory access from #6bd3fe8d
* [oPNG]: set time on file_write
* [oyra]: properly tag and run file_write.ppm
* [oyra]: warn for no found file_write node type
* [exmpl]: use oyImage_ToFile() in o-icc
* [oPNG]: implement file_write node
* [alpha]: add oyImage_ToFile API
* [oyra]: add file_write.meta node type
* [build]: fix link error in examples
* [alpha]: preserve alpha in oyConversion_CreateFromImage
* [docu]: update PACKAGING notes
* [exmpl]: make Oy_Fl_Shader_Box.h worth its name
* [exmpl]: use GLee easy extension library
* [exmpl]: allow conversion without icc node
* [alpha]: add more debug infos
* [exmpl]: fix typo
* [exmpl]: fix compile error
* [i18n]: update deutsch
* [docu]: update o-monitor man page and cli help
* [alpha]: add -f clut option to o-icc
* [alpha]: add --short option to o-monitor
* [build]: add oforms before install_bin
* [docu]: update build paths for o-icc
* [alpha]: add oyImage_GetPoint API
* [oicc]: fix enless loop, when no module is found
* [lraw]: inverse cam_xyz
* [alpha]: preserve rank value list
* [lcms]: set xyY->Y value for matrix profile
* [i18n]: update deutsch
* [lraw]: generate fallback ICC profile from cameraRAW
* [alpha]: adjust function versions
* [oyra]: use multiple threads for reading PPMs
* [exmpl]: generate hald and slice images in o-icc
* [build]: move oyranos-icc to examples
* [exmpl]: use file name from oyImage_s
* [alpha]: add file name to oyImage_s
* [i18n]: update deutsch
* [exmpl]: add --module to image_display
* [exmpl]: selectively set output bit depth
* [exmpl]: adapt to the new API
* [exmpl]: fix wrong image assinment
* [exmpl]: extent the oyConversion_FromImage* APIs
* [exmpl]: add debug output
* [alpha]: add oyShowConversion_() API
* [alpha]: show data in oyFilterGraph_ToText
* [build]: install oyranos-xforms-modules
* [oyIM]: fix mAB/mBA value scaling
* [alpha]: oyImage_DataSet -> oyImage_SetData rename
* [exmpl]: fix colour of *xy spectral line
* [exmpl]: fix the o-p-graph line width formula
* [docu]: add o-p-graph -d option
* [exmpl]: set o-p-graph colours to b/w
* [test]: fix wrong declaration
* [exmpl]: fix command line arg parsing
* [docu]: switch oyranos-p-graph default to CIE*ab
* [exmpl]: add CIE StdObserver data
* [alpha]: release failed profile on load
* [i18n]: update deutsch
* [alpha]: internally declare oyColourConvert_()
* [build]: package oyranos-profile-graph tool
* [exmpl]: add oyranos-profile-graph tool
* [core]: support float arguments for cli
* [exmpl]: sync menue style
* [core]: small simplification of oyLab2XYZ()
* [exmpl]: update svg2cairoscript
* [exampl]: add svg2cairoscript helper
* [alpha]: API clean
* [conf]: bump to v0.5
* [exmpl]: add Quit and ICC Examin button to image_display
* [source]: silently ignore empty options
* [exmpl]: switch from dcraw to lraw module
* [alpha]: add oyImage_WidthGet() API
* [exampl]: update image2pdf
* [alpha]: add oyPixelPrint()
* [lcm2]: add more verbosity for failed transforms
* [core]: allow new printf string without deallocation
* [alpha]: be verbose about failed node context
* [alpha]: modify oyDeviceGet() API
* [exmpl]: add --no-logo option to image_display
* [alpha]: add oyConversion_FromImage oyImage_PpmWrite
* [oyra]: support filter cast
* [alpha]: add oyImage_FromFile() API
* [core]: improve warning messages
* [core]: fix KDB_VERSION_NUM missmatch
* [core]: show elektra warnings
* [conf]: provide fallback for non static Elektra
* [core]: prepare for Elektra 0.8
* [build]: package image_display examples files
* [exmpl]: fix compile bug
* [conf]: BuildRequires libXcm >= 0.5.1
* [exampl]: add missed Xcm header
* [build]: add LCMS2_H to include paths
* [alpha]: use strcasecmp sort with posix
* [alpha]: effect and proofing in oyranos-icc -e -p
* [exmpl]: resize image_display image area
* [alpha]: directly use lcms in oyranos-icc tool
* [docu]: include generated docu in alpha
* [alpha]: sort profiles in oyranos-profiles -lf
* [alpha]: sorting of internal profile names
* [oyX1]: set atom_name for debug output
* [conf]: compile with debug macros by default
* [oyX1]: add more debug output
* [oyX1]: use AnyPropertyType in XGetWindowProperty
* [alpha]: bump all device module APIs
* [CUPS]: bump module API
* [CUPS]: check string lenght
* [alpha]: clearify conversion tool usage
* [alpha]: set profile in output image
* [alpha]: beautify PPM comment
* [core]: move ifunction declaration from wrong header
* [test]: fix wrong variable name
* [alpha]: remove some old key name space references
* [alpha]: remove unneeded header
* [alpha]: start colour conversion tool
* [core]: remove internal old device API
* [conf]: bump to version
* [oyra]: print debug output to stderr
* [oforms]: omit debug output with simple verbosity
* [oyra]: load referenced ICC profile in PPM
* [conf]: check for dynamic libs
* [conf]: expose paths in headers
* [docu]: add oforms example file
* [oforms]: clean up unused code
* [alpha]: print explicite file name
* [core]: list complete path+file name
* [core]: explicite per file name qsort
* [exmpl]: list per device profiles
* [CUPS]: reduce unwanted warning
* [alpha]: check device completeness for compare
* [exmpl]: clean nvmt code
* [exmpl]: add pre meta device example
* [docu]: mention Yajl optional dependency
* [core]: remove unused code
* [core]: use org.freedesktop.openicc namespace
* [docu]: update profile package names in README
* [core]: switch to OpenICC namespace
* [alpha]: skip non existing option
* [docu]: enable doxygen search engine
* [build]: sync with openSUSE spec file

###Milan Knizek (2):
* [i18n]: update Czech
* [i18n]: update Czech for fl_i18n

###Yiannis Belias (584):
* [templates]: Fixes to compile oyFilterPlug_s
* [build]: add include path for cmake
* [f_compare] Rewrite the awk script
* [templates] Adopt oyCMMGetMetaApis_ to "hidden struct" interface.
* [templates] Import oyCMMsGetMetaApis_() in oyranos_module.c
* [templates] Import oyCMMGetMetaApis_() in oyranos_module.c
* Create skeleton files for oyCMMapis_s
* [templates] Import oyObjectInfoStatic_s in oyranos_generic.[ch]
* [templates] Remove conflicting CMakeLists.txt
* [templates] Add include files in oyProfileTag_s.c
* [templates] Add new python gdb pretty printer: gdb.py
* [generator] Register python filetype
* [tempaltes] Include oyCMMapi3_s_.h in oyProfileTag_s.c
* [templates] Register CMMui, CMMobjectType with oyOBJECT_e
* Fixes to compile oyFilterNode_s.c
* [templates] Include oyConnector_s.h in oyCMMapi7_s_.h
* [templates] Add include files to oyCMMapi6_s_.h
* Fixes to compile oyCMMapi5_s.c
* Fixes to compile oyCMMapi4_s_.c
* Fixes to compile oyCMMapi_s_.c
* [sources] Update oyCMMapi6_s members from master
* Update oyCMMapi_Check_() from master
* [templates] Include parent headers
* Fixes to compile oyCMMapi3_s/oyCMMapi3_s_
* [templates] Use proper cast for parent destructors
* [review] [sources] Fix wrong var name
* [templates] Use proper cast for parent copy constructors
* [templates] Use proper cast for parent constructors
* Fixes to compile oyPixelAccess_s_.c
* [sources] Fix wrong object var name
* Fixes to compile oyFilterGraph_s.c
* Fixes to compile oyFilterNode_s_.c
* [templates] Import oyPointerRelease() in oyranos_object_internal.h
* [templates] Import oyCMMCacheListGetEntry_() in oyranos_module_internal.h
* Fixes to compile oyFilterNode_s.c
* [templates] Move oyClass_s typedef before any include files
* [sources] Adopt oyFilterNode_Observe_() to "hidden struct" interface.
* [sources] Import oyFilterNodeObserve_() in oyFilterNode_s_
* [templates] Import oyContextCollectData_() in oyranos_generic.[ch]
* [templates] Import FilterEdge defines to oyranos_conversion.h
* Fixes to compile oyFilterPlug_s
* Fixes to compile oyFilterCore_s
* [f_compare] Swithing branches is unnecessary
* [sources] Update oyOptions_ForFilter_ from master
* Fixes to compile oyArray2d_s_.c
* Fixes to compile oyArray2d_s.c
* [grantlee] Do not ignore a previous digit
* [templates] Import pixel mask description to oyranos_image_internal.h
* [templates] Create new oyranos_image_internal.h file
* [sources] Fix compile error - add casts
* [templates] Import oyCMMapi_Check_f in oyranos_module.h
* [templates] Import oyCMMsGetApi_(_) in oyranos_module_internal.[ch]
* [templates] Import oyCMMapiQuery(ies)_s in oyranos_module_internal.h
* [sources] Fix compile errors for oyConversion_s_.c
* [templates] Remove old object system include files
* [sources] Fix compile errors for oyConversion_s.c
* [templates] Add missing include files
* Create skeleton files for oyFilterNodes_s
* [sources] Adopt oyConversion_GetOnePixel() to "hidden struct" interface. (II)
* [review] [sources] Add oyImage_s width/height getters
* [sources] Import some needed oyImage_s function declarations
* [templates] Import defines to oyranos_object.h
* [templates] Import oyPIXEL_ACCESS_TYPE_e to oyranos_image.h
* [sources] Adopt oyPixelAccess_Create() to "hidden struct" interface.
* [sources] Adopt oyPixelAccess_ChangeRectangle() to "hidden struct" interface.
* [sources] Import public methods for oyPixelAccess_s
* [review] [sources] Implement the destructor for oyPixelAccess_s
* [sources] Implement the copy constructor for oyPixelAccess_s
* [sources] Implement the constructor for oyPixelAccess_s
* [sources] Adopt oyFilterGraph_ToText() to "hidden struct" interface.
* [sources] Adopt oyFilterGraph_ToBlob() to "hidden struct" interface.
* [sources] Adopt oyFilterGraph_SetFromNode() to "hidden struct" interface.
* [sources] Adopt oyFilterGraph_PrepareContexts() to "hidden struct" interface.
* [sources] Adopt oyFilterGraph_GetNode() to "hidden struct" interface.
* [sources] Adopt oyFilterGraph_FromNode() to "hidden struct" interface.
* [sources] Import public methods for oyFilterGraph_s
* [sources] [sources] Implement the destructor for oyFilterGraph_s
* [sources] Implement the copy constructor for oyFilterGraph_s
* [sources] Implement the constructor for oyFilterGraph_s
* Create skeleton files for oyFilterGraph_s
* [templates] Include the oyranos_image.h in all objects_image members
* [sources] Adopt oyConversion_GetImage() to "hidden struct" interface. (II)
* [templates] Add missing include files
* [sources] Implement the destructor for oyImage_s
* [templates] Adopt oyCMMsGetFilterApis_() to "hidden struct" interface.
* [templates] Import oyCMMsGetFilterApis_() in oyranos_module.template.c
* [templates] Add oyranos_module.template.c file
* Revert "* [templates]: deactiave Filter function"
* [sources] Adopt oyArray2d_SetFocus() to "hidden struct" interface.
* [sources] Adopt oyArray2d_RowsSet() to "hidden struct" interface.
* [sources] Adopt oyArray2d_ReleaseArray() to "hidden struct" interface.
* [sources] Adopt oyArray2d_DataSet() to "hidden struct" interface.
* [sources] Adopt oyArray2d_Create() to "hidden struct" interface.
* [sources] Adopt oyArray2d_DataCopy() to "hidden struct" interface.
* [sources] Import public methods for oyArray2d_s
* [sources] Adopt oyArray2d_ToPPM_() to "hidden struct" interface.
* [sources] Adopt oyArray2d_Create_() to "hidden struct" interface.
* [sources] Import private methods for oyArray2d_s
* [sources] Implement the destructor for oyArray2d_s
* [sources] Implement the copy constructor for oyArray2d_s
* [templates] Fix wrong header name
* [templates] Support C++ overloaded constructors
* [generator] Mark constructor like functions
* [templates] (C++) Keep the public struct pointer
* [generator] Add forgotten templates for C++
* [build] Change generator build type to Release
* Add objects_image to modules.sh
* [templates] Remove trailling space
* [generator] Overload renderFile()
* [generator] Continue commit b407342bd88ab19f6d9eb1fda9a3b4a262ed4f60
* [generator] Remove overloaded render() methods.
* [templates] Include oyPixelAccess_s_.h in oyFilterPlug_s.c
* Create skeleton files for oyArray2d_s
* [review] [sources] Now oyFilterPlugs_MoveIn(), returns int
* [templates] Add some include files
* [sources] Fix compile errors
* [templates] Mark argsCPP variables as safe
* [generator] Modify function arguments for C++
* [generator] Add an isValid() method to funcInfo
* [templates] Add a c_struct() method to C++ classes
* [sources] Update all public function prototypes
* [generator] Support the public API member functions
* [generator] Add isStatic property to FuncInfo
* [generator] Register the C++ base templates
* [templates] Update the C++ Base templates
* [generator] Add isVoid property to FuncInfo
* [generator] Automaticly create the C++ templates
* [generator] Rename the Class_* files to oyClass_*
* [generator] Ignore oyStruct_s/Null for FuncInfo
* [generator] Register *.hh template files
* [generator] Remove the unknown default value
* [generator] Add some checks
* [generator] Fix wrong type of FuncInfo list.
* [generator] Add destructor to ClassInfo
* [generator] Merge the FuncInfo Class with rest of code.
* [templates] Normalise template file names: partIV
* [templates] Normalise template file names: partIII
* [templates] Normalise template file names: partII
* [generator] Normalise template file names: partI
* [generator] Initiall support for C++ templates
* [templates] Add some include files
* [sources] Use the private class types as class members
* [sources] Import oyImage_s private typedefs
* [templates] Move oyPixel_t and defines to oyranos_image.h
* [templates] Move oyFILTER_REG_MODE_e to oyranos_conversion.h
* [sources] Adopt oyRectangle_Trim() to "hidden struct" interface.
* [sources] Adopt oyRectangle_Show() to "hidden struct" interface.
* [sources] Adopt oyRectangle_SetGeo() to "hidden struct" interface.
* [sources] Adopt oyRectangle_SetByRectangle() to "hidden struct" interface.
* [sources] Adopt oyRectangle_Scale() to "hidden struct" interface.
* [sources] Adopt oyRectangle_SamplesFromImage() to "hidden struct" interface.
* [sources] Adopt oyRectangle_Round() to "hidden struct" interface.
* [sources] Adopt oyRectangle_PointIsInside() to "hidden struct" interface.
* [sources] Adopt oyRectangle_Normalise() to "hidden struct" interface.
* [sources] Adopt oyRectangle_NewWith() to "hidden struct" interface.
* [sources] Adopt oyRectangle_NewFrom() to "hidden struct" interface.
* [sources] Adopt oyRectangle_MoveInside() to "hidden struct" interface.
* [sources] Adopt oyRectangle_IsInside() to "hidden struct" interface.
* [sources] Adopt oyRectangle_IsEqual() to "hidden struct" interface.
* [sources] Adopt oyRectangle_Index() to "hidden struct" interface.
* [sources] Adopt oyRectangle_CountPoints() to "hidden struct" interface.
* [sources] Import public methods for oyRectangle_s
* [sources] Implement the copy constructor for oyRectangle_s
* [review] Create skeleton files for oyRectangle_s
* [revert] Hide: oyConversion_GetGraph
* [sources] Adopt oyConversion_ToText() to "hidden struct" interface.
* [sources] Adopt oyConversion_Set() to "hidden struct" interface.
* [sources] Adopt oyConversion_RunPixels() to "hidden struct" interface.
* [sources] Adopt oyConversion_GetOnePixel() to "hidden struct" interface.
* [sources] Adopt oyConversion_GetNode() to "hidden struct" interface.
* [sources] Adopt oyConversion_GetImage() to "hidden struct" interface.
* [sources] Adopt oyConversion_GetGraph() to "hidden struct" interface.
* [sources] Adopt oyConversion_CreateBasicPixelsFromBuffers() to "hidden struct" interface.
* [sources] Adopt oyConversion_CreateBasicPixels() to "hidden struct" interface.
* [sources] Adopt oyConversion_Correct() to "hidden struct" interface.
* [sources] Import public methods for oyConversion_s
* [sources] Implement the destructor for oyConversion_s
* [sources] Implement the copy constructor for oyConversion_s
* Create skeleton files for oyConversion_s
* [sources] Adopt oyOptions_ForFilter_() to "hidden struct" interface.
* [sources] Adopt oyFilterCore_NewWith() to "hidden struct" interface.
* [templates] Move/update oyCMMGetText_f to oyranos_module.h
* [templates] Move oyWIDGET_EVENT_e to oyranos_module.h
* [templates] Move oyWIDGET_EVENT_e to oyranos_module.h
* [templates] Move oyCHANNELTYPE_e to oyranos_image.h
* [templates] Move oyDATATYPE_e to oyranos_image.h
* [templates] Add a new header file "oyranos_image.h"
* Add build info support to modules.sh
* [revert] Hide: oyFilterSocket_SignalToGraph()
* [docu] Update TODO
* [templates] Add some missing include files
* [sources] Fix wrong cast order
* [templates]: rename oyCMMptr_s to oyPointer_s V
* [templates] Define class name typedef before local include files.
* [templates] Move oyCONNECTOR_EVENT_e to oyranos_conversion.h
* [templates] Add a new header file "oyranos_conversion.h"
* Create skeleton files for oyPixelAccess_s
* Create skeleton files for oyImage_s
* [docu] Update README ubuntu packages
* [cmm] Remove oyConnectorImaging_s definitions:
* [sources] Implement the destructor for oyConnectorImaging_s
* [sources] Implement the copy constructor for oyConnectorImaging_s
* [sources] Implement the constructor for oyConnectorImaging_s
* [templates] Add include files to Connector_s_.h
* Create skeleton files for oyConnectorImaging_s
* [cmm] Remove oyCMMui_s definitions:
* [sources] Import oyCMMui_s private typedefs
* [cmm] Remove oyCMMobjectType_s definitions:
* [cmm] Remove oyCMMapi10_s definitions:
* [cmm] Remove oyCMMapi9_s definitions:
* [sources] Update CMMapi9 from master.
* [cmm] Remove oyCMMapi8_s definitions:
* [cmm] Remove oyCMMapi7_s definitions:
* [cmm] Remove oyCMMapi6_s definitions:
* [sources] Rename oyCMMapi4_selectFilter_() to oyCMMapi4_SelectFilter_()
* [cmm] Remove oyCMMapi4_s definitions:
* [cmm] Remove oyCMMapi3_s definitions:
* [alpha] Remove oyCMMapiFilters_s definitions:
* [templates] Cast parent object to oyPointer
* [templates] Use oyClass_New() instead of oyClass_New_()
* [cmm] Remove stale oyCMMapiFilter_s definitions.
* [cmm] Remove stale oyCMMapi_s definitions.
* [cmm] Remove oyCMMapi5_s definitions:
* [sources] Adopt oyFilterSocket_MatchImagingPlug() to "hidden struct" interface.
* [sources] Import public methods for oyCMMapi5_s
* [alpha] Remove oyCMMapi3_Query_()
* [cmm] Remove oyCMMapi_s private typedefs:
* [alpha] Remove oyCMMapi_s private function members:
* [sources] Update skeleton files for oyFilterPlugs_s
* [alpha] Remove oyFilterSocket_s function members:
* [sources] Adopt oyFilterSocket_SignalToGraph() to "hidden struct" interface.
* [sources] Adopt oyFilterSocket_Callback() to "hidden struct" interface.
* [sources] Import public methods for oyFilterSocket_s
* [sources] Implement the destructor for oyFilterSocket_s
* [sources] Implement the copy constructor for oyFilterSocket_s
* [alpha] Remove oyFilterPlug_s function members:
* [sources] Adopt oyFilterPlug_ResolveImage() to "hidden struct" interface.
* [sources] Adopt oyFilterPlug_ConnectIntoSocket() to "hidden struct" interface.
* [sources] Adopt oyFilterPlug_Callback() to "hidden struct" interface.
* [sources] Import public methods for oyFilterPlug_s
* [sources] Implement the destructor for oyFilterPlug_s
* [sources] Implement the copy constructor for oyFilterPlug_s
* [alpha] Remove oyFilterNode_s function members:
* [alpha] Remove oyFilterNode_s private function members:
* [sources] Adopt oyFilterNode_UiGet() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_ShowConnector() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_OptionsGet() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_NewWith() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetText() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetSocket() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetPlug() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetId() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetConnectorPos() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_EdgeCount() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_Disconnect() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_DataSet() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_DataGet() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_Create() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_ConnectorMatch() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_Connect() to "hidden struct" interface.
* [sources] Import public methods for oyFilterNode_s
* [sources] Adopt oyFilterNode_TextToInfo_() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetNextFromLinear_() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_GetLastFromLinear_() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_DataGet_() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_ContextSet_() to "hidden struct" interface.
* [sources] Adopt oyFilterNode_AddToAdjacencyLst_() to "hidden struct" interface.
* [sources] Import private methods for oyFilterNode_s
* [review] [sources] Implement the destructor for oyFilterNode_s
* [review] [sources] Implement the copy constructor for oyFilterNode_s
* [review] [sources] Implement the constructor for oyFilterNode_s
* [sources] Fix skeleton files for oyFilterNode_s
* [alpha] Remove oyFilterCore_s function members:
* [sources] Adopt oyFilterCore_GetText() to "hidden struct" interface.
* [sources] Adopt oyFilterCore_GetName() to "hidden struct" interface.
* [sources] Adopt oyFilterCore_CategoryGet() to "hidden struct" interface.
* [sources] Import public methods for oyFilterCore_s
* [sources] Adopt oyFilterCore_SetCMMapi4_() to "hidden struct" interface.
* [sources] Import private methods for oyFilterCore_s
* [sources] Implement the destructor for oyFilterCore_s
* [sources] Implement the copy constructor for oyFilterCore_s
* [sources] Implement the constructor for oyFilterCore_s
* [templates] WARNc_S already provides extra info
* [alpha] Remove oyConnector_s function members:
* [sources] Adopt oyConnector_GetMatch() to "hidden struct" interface.
* [sources] Adopt oyConnector_SetMatch() to "hidden struct" interface.
* [sources] Fix oyFilterCore_s members
* [sources] Adopt oyConnector_SetReg() to "hidden struct" interface.
* [sources] Adopt oyConnector_GetReg() to "hidden struct" interface.
* [sources] Adopt oyConnector_SetIsPlug() to "hidden struct" interface.
* [sources] Adopt oyConnector_IsPlug() to "hidden struct" interface.
* [sources] Import public methods for oyConnector_s
* [sources] Implement the destructor for oyConnector_s
* [sources] Implement the copy constructor for oyConnector_s
* [sources] Implement the constructor for oyConnector_s
* [sources] Update oyConnector_s from master
* [templates] Apply part of 33d635edb10dccdaee1822caebea2e27b13c99a9
* [sources] Apply part of 33d635edb10dccdaee1822caebea2e27b13c99a9
* [alpha] Apply part of 33d635edb10dccdaee1822caebea2e27b13c99a9
* [generator] Fix indentation
* [generator] Move Class.dox in generator/templates_Class
* [sources] Update oyProfiles_DeviceRank() from master
* [templates] Add oyranos_i18n.h to oyranos_devices_internal.c
* [templates] Add oyProfiles_s template files
* [revert] Hide: oyOptions_DoFilter()
* [templates] Add include files
* [revert] Hide CMMS
* Replace old version of oyConfig_New()
* [build] Add oyProfiles_s to liboyranos_object
* [alpha] Remove oyCheckType\_\_m from oyranos_alpha_internal.h
* [revert] Hide CMMS
* [templates] Add missing include files
* [build] Add oyProfileTag_s to liboyranos_object
* [templates] Insert custom functions to list classes.
* [templates] Add oyranos_string.h in Config_s.c
* [revert] Hide CMMS:
* [generator] Create all missing source files
* [build] Add oyConfigs_s to liboyranos_modules
* [build] Fix liboyranos_modules.so object files variable
* [alpha] Include oyranos_devices.h
* [review] Replace all assignments of oyProfiles_MoveIn()
* [templates] Fix compile warnings.
* [build] Add source files to new liboyranos_modules.so
* [oyranos_gamma] Fix compile warning
* [sources] Update oyProfile_FromMem() from master.
* [sources] Fix compile warning.
* [sources] Update oyProfile_FromMemMove_() from master.
* [sources] Update oyProfile_WriteHeader_() from master.
* [templates] Update oyDeviceGet() from master.
* [sources] Update oyProfile_FromStd() from master.
* [sources] Update oyProfile_FromFile_() from master.
* [sources] Update oyProfile_GetMem() from master.
* [build] Update to new generator arguments
* [generator] Support multiple template directories.
* [doc] Add yajl dependency for Arch
* [generator] Prettier printing
* [sources] Remove unnecessary [notemplates] tag.
* [generator] Simplify source file creation.
* [generator] Do not overwrite existing template files.
* Revert "[generator] Check first  if the template is readable"
* [alpha] Include oyConfigs_s.h
* [alpha] Remove oyConfigs_Release()
* [alpha] Remove oyConfigs_s Copy constructor.
* [alpha] Remove oyConfigs_New()
* [alpha] Remove oyConfigs_s definition
* [alpha] Fix compile wanrnings
* [sources] Rename oyConfigs_SelectSimiliars (II)
* [templates] Include oyranos_elektra.h in Option_s.c
* [sources] Fix compile warnings
* [sources] Remove static keword from:
* [templates] Add oyOptions_s_.h to Config_s_.h
* [sources] Fix compile warnings
[revert] Hide CMMS: oyPointer_LookUpFromObject()
* [templates] Add includes to Profile_s_.c
* [templates] Add OY_ERR macro to oyranos_object_internal.h
* [sources] Fix small compile errors
* [templates] Add include files to Profile_s.c
* [templates] Add oyranos_icc.h to Profile_s.c
* [review] [sources] Add 0 as 2nd arg of oyProfile_GetHash_
* [revert] Hide CMMS: oyPointer_LookUpFromText()
* [templates] Add oyranos_io.h to Profile_s.c
* [sources] Fix static functions and add defines.
* [revert] Hide CMMS: oyConfig_DomainRank
* [alpha] Remove oyOptions_SetDriverContext()
* [sources] Import oyOptions_SetDriverContext()
* [alpha] Remove oyOptions_Handle()
* [sources] Apply commit ee35646a7ec7c549776d2a4287332652f7fdfa4e
* [sources] Adopt oyOptions_Handle() to "hidden struct" interface.
* [alpha] Remove oyOptions_ForFilter()
* [alpha] Remove oyOptions_DoFilter()
* [sources] Update oyOptions_DoFilter() from master.
* [alpha] Remove stale declarations for oyOptions_s:
* [alpha] Remove oyOptions_s common list functions:
* [alpha] Remove oyOptions_s default functions:
* [alpha] Remove oyConfigDomainList()
* [sources] Apply part of commit for oyConfigDomainList()
* [alpha] Remove oyConfigs_Modify()
* [alpha] Remove oyConfigs_FromDomain()
* [alpha] Remove oyConfigs_FromDeviceClass()
* [alpha] Remove oyConfigs_FromDB()
* [sources] Apply commit 7ec2d7873eae91a9c616a30fbcfad932a9c3b0bc
* [alpha] Remove oyConfigs_s common list functions:
* [alpha] Remove oyOption_SetValueFromDB()
* [sources] Import oyOption_SetValueFromDB()
* [alpha] Remove stale declarations for
* [alpha] Remove oyOption_GetText()
* [alpha] Remove oyOption_GetRegistration()
* [alpha] Remove oyOption_GetId()
* [alpha] Remove oyOption_GetData()
* [alpha] Remove oyOption_Clear()
* [sources] Add oyConfig_FromRegistration() declaration.
* [alpha] Remove oyConfig_SaveToDB()
* [sources] Apply pending parts of commit:
* [alpha] Remove oyRankMapCopy()
* [alpha] Remove oyConfig_Has()
* [alpha] Remove oyConfig_GetDB()
* [alpha] Remove oyConfig_Get()
* [alpha] Remove oyConfig_FindString()
* [alpha] Remove oyConfig_Find()
* [alpha] Remove oyConfig_EraseFromDB()
* [sources] Update oyConfig_EraseFromDB() from master.
* [alpha] Remove oyConfig_DomainRank()
* [alpha] Remove oyConfig_Count()
* [alpha] Remove oyTextIccDictMatch().
* [alpha] Remove oyConfig_Compare().
* [templates] Import oyTextIccDictMatch()
* [sources] Apply pending parts of commit:
* [alpha] Remove oyConfig_ClearDBData() from oyranos_alpha.[ch]
* [alpha] Remove oyConfig_AddDBData() from oyranos_alpha.[ch]
* [sources] Apply commit 5fdbfd05e95b8e87c395c02a60fe526a747e38d9
* [sources] Apply part of commit for oyProfiles_DeviceRank()
* [sources] Apply commit 1c7fbc752592be05ffc871eadf969813a9aac10d
* [sources] Apply commit 68b4ec9c337469eb16c2f447abae82c9b59ad0f8
* [f_compare] [-s]: Show only diff output
* [f_compare] Add cleanup()
* [f_compare] Add branch:filename support
* Update help message for modules.sh
* [sources] Apply commit 43feb3f9535e512a3c7fdca5cf70a9f71cb25304
* [templates] Apply commit f1f0743b5dab813fb67e1e5dc4fb7f917aa8dcb8
* [templates] Include oyOptions_s.h in Config_s.c
* [revert] Hide CMMs
* Update the previous script for fast output.
* Script for pretty printing the imported classes
* Use vimdiff directly in previous script
* [alpha] Remove old function declarations
* [alpha] Remove oyDeviceRegistrationCreate_() from oyranos_alpha.c
* [alpha] Remove oyDeviceSelectSimiliar() from oyranos_alpha.c
* [alpha] Remove oyDeviceProfileFromDB() from oyranos_alpha.c
* [alpha] Remove oyDeviceSetProfile() from oyranos_alpha.c
* [alpha] Remove oyDeviceAskProfile2() from oyranos_alpha.c
* [alpha] Remove oyDeviceGetProfile() from oyranos_alpha.c
* [alpha] Remove oyDeviceGetInfo() from oyranos_alpha.c
* [alpha] Remove oyDeviceUnset() from oyranos_alpha.c
* [alpha] Remove oyDeviceSetup() from oyranos_alpha.c
* [alpha] Remove oyDeviceBackendCall() from oyranos_alpha.c
* [alpha] Remove oyDeviceGet() from oyranos_alpha.c
* [alpha] Remove oyDevicesGet() from oyranos_alpha.c
* Add helper script to compare function bodies.
* [templates] Add/Adopt oyDeviceSigGet() to oyranos_devices.c
* [build] Add Config and Profile
* [oyranos_gamma] Fix compile warning
* [alpha] Fix compile error: use private pointer
* [review] [alpha] Replace old oyConfig_s constructor
* [alpha] Fix compile errors: use private pointers
* [alpha] Properly release oy_profile_s_file_cache_ from oyAlphaFinish_()
* Fix compile warning - add cast
* [sources] Remove duplicate oyProfiles_s functions
* [generator] More verbose printing of filenames
* fix oyWIDGET_e cast
* [alpha] Include oyProfiles_s.h
* [templates] Include oyHash_s.h in Pointer_s.c
* [templates]: rename oyCMMptr_s to oyPointer_s IV
* [templates] Include oyStructList_s.h in ProfileTag_s.h
* [alpha] Remove oyConfig_s definition, constructor/destructor/...
* [alpha] Include oyProfile_s.h
* [alpha] Remove oyProfileTag_Release() from oyranos_alpha.c
* [alpha] Remove oyProfileTag_Copy() from oyranos_alpha.c
* [alpha] Remove oyProfileTag_New() from oyranos_alpha.c
* [alpha] Remove oyProfile_WriteTags_() from oyranos_alpha.c
* [alpha] Remove oyProfile_WriteTagTable_() from oyranos_alpha.c
* [alpha] Remove oyProfile_WriteHeader_() from oyranos_alpha.c
* [alpha] Move oyProfile_GetCMMText_() to sources/
* [sources] Adopt oyProfile_DeviceAdd() to "hidden struct" interface.
* [alpha] Move oyProfile_DeviceAdd() to sources/.
* [alpha] Remove oySIGNATURE_TYPE_e from oyranos_alpha.h
* [alpha] Remove old oyProfileTag_s definition
* [alpha] Remove old oyProfile_s definition
* [alpha] Remove oyProfile_New_() from oyranos_alpha.c
* [build] Arch: Remove out of tree build functions
* [sources] Rename oyPointer function names:
* [sources] Edit doxygen comments
* [alpha] Move oyPointerLookUpFromText() to templates
* [alpha] Move oyPointerLookUpFromObject() to templates
* [alpha] Remove oyStructList*() from oyranos_alpha.h
* templates] Add include files to oyranos_generic.template.c
* [build] Add oyranos_object.[ch] to makefile.in
* review] [sources] Adopt oyStructList_GetType_() to "hidden struct" interface.
* [alpha] Move oyStructList_GetType_() to templates/
* build] Update Oyranos-git Arch package
* review] [sources] Adopt oyCacheListGetEntry_() to "hidden struct" interface.
* [alpha] Move oyCacheListGetEntry_() to templates/
* [templates] Create new file oyranos_generic.template.c
* [alpha] Remove oyProfile_GetMD5() from oyranos_alpha.*
* review] [sources] Adopt oyProfile_GetMD5() to "hidden struct" interface.
* [sources] Import oyProfile_GetMD5()
* [alpha] Remove oyProfile_TagReleaseAt_() from oyranos_alpha*
* [alpha] Remove oyProfile_TagMoveIn_() from oyranos_alpha*
* [alpha] Remove oyProfile_GetTagCount_() from oyranos_alpha*
* [alpha] Remove oyProfile_GetTagByPos_() from oyranos_alpha*
* [alpha] Remove oyProfile_GetFileName_r() from oyranos_alpha*
* [alpha] Remove oyProfile_Hashed_() from oyranos_alpha.*
* [alpha] Remove oyProfile_Match_() from oyranos_alpha.*
* [alpha] Remove oyProfile_TagsToMem_() from oyranos_alpha*
* [alpha] Remove oyProfile_ToFile_() from oyranos_alpha*
* [alpha] Remove oyProfile_DeviceGet() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetFileName() from oyranos_alpha.*
* [alpha] Remove oyProfile_AddTagText() from oyranos_alpha.*
* [alpha] Remove oyProfile_TagReleaseAt() from oyranos_alpha.*
* [alpha] Remove oyProfile_TagMoveIn() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetTagCount() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetTagById() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetTagByPos() from oyranos_alpha.*
* [alpha] Remove oyProfile_Equal() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetMem() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetText() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetID() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetChannelNames() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetChannelName() from oyranos_alpha.*
* [alpha] Remove oyProfile_SetChannelNames() from oyranos_alpha.*
* [alpha] Remove oyProfile_SetSignature() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetSignature() from oyranos_alpha.*
* [alpha] Remove oyProfile_GetChannelsCount() from oyranos_alpha.*
* [alpha] Remove old oyProfile_s destructor
* [alpha] Remove old oyProfile_s copy constructor
* [alpha] Remove oyProfile_FromMD5() from oyranos_alpha.*
* [alpha] Remove oyProfile_FromSignature() from oyranos_alpha.*
* [alpha] Remove oyProfile_FromMem() from oyranos_alpha.*
* [alpha] Remove oyProfile_FromMemMove_() from oyranos_alpha.*
* [docu] Fix doxygen comments
* [alpha] Remove oyProfile_FromFile() from oyranos_alpha.*
* [alpha] Remove oyProfile_FromFile_() from oyranos_alpha.*
* [alpha] Remove oyProfile_FromStd() from oyranos_alpha.*
* [sources] Import oy_profile_s_std_cache_ from oyranos_alpha.c
* [sources] Import private static functions from oyranos_alpha.c
* [sources] Import oyProfile_Message_() from oyranos_alpha.c
* [alpha] oyProfiles_Release() is now in API_generated/
* [alpha] oyProfiles_Copy() is now in API_generated/
* [alpha] oyProfiles_New() is now in API_generated/
* review] [sources] Adopt oyProfiles_DeviceRank() to "hidden struct" interface.
* review] [sources] Adopt oyProfiles_Count() to "hidden struct" interface.
* review] [sources] Adopt oyProfiles_Get() to "hidden struct" interface.
* review] [sources] Adopt oyProfiles_ReleaseAt() to "hidden struct" interface.
* review] [sources] Adopt oyProfiles_MoveIn() to "hidden struct" interface.
* [sources] Remove unneded declarations
* [docu] Update doxygen comments for oyProfiles_ForStd()
* review] [sources] Adopt oyProfiles_Create() to "hidden struct" interface.
* [sources] Remove unneded declarations
* sources] Import public methods for oyProfiles_s
* [alpha] Remove stale oyProfileTag_s code
* Create skeleton files for oyProfiles_s
* review] [sources] Implement the destructor for oyProfileTag_s
* review] [sources] Implement the copy constructor for oyProfileTag_s
* sources] Adopt oyProfileTag_GetBlock() to "hidden struct" interface.
* sources] Adopt oyProfileTag_Get() to "hidden struct" interface.
* sources] Adopt oyProfileTag_Set() to "hidden struct" interface.
* sources] Adopt oyProfileTag_CreateFromData() to "hidden struct" interface.
* sources] Adopt oyProfileTag_CreateFromText() to "hidden struct" interface.
* sources] Adopt oyProfileTag_Create() to "hidden struct" interface.
* sources] Import public methods for oyProfileTag_s
* [docu] Update README with Arch build instructions
* [docu] Update doxygen comments for oyStruct_ObserverRemove_()
* [docu] Update doxygen comments for oyStruct_ObserverListGet_()
* [docu] Update doxygen comments for oyObserver_Copy_()
* [docu] Update doxygen comments for oyStructSignalForward_()
* [sources] Fix @note [notemplates]
* [sources] Remove oyObserver_s public methods definitions from template
* [sources] Remove oyObserver_s public methods declarations from template
* [sources] Remove oyObserver_s private methods definitions from template
* [sources] Remove oyObserver_s private methods declarations from template
* [sources] Remove oyObserver_s public members from template
* review] [sources] Adopt oyStructList_GetRaw_() to "hidden struct" interface.
* [docu] Update doxygen comments for oyStructList_GetParentObjType()
* review] [sources] Adopt oyStructList_GetType() to "hidden struct" interface.
* [docu] Update doxygen comments for oyStructList_Create()
* [docu] Update doxygen comments for oyStructList_GetName()
* [docu] Update doxygen comments for oyStructList_AddName()
* [docu] Update doxygen comments for oyStructList_MoveInName()
* [docu] Update doxygen comments for oyStructList_ObserverAdd()
* [docu] Update doxygen comments for oyStructList_Sort()
* [docu] Update doxygen comments for oyStructList_MoveTo()
* review] [sources] Adopt oyStructList_CopyFrom() to "hidden struct" interface.
* review] [sources] Adopt oyStructList_Clear() to "hidden struct" interface.
* review] [sources] Adopt oyStructList_GetID() to "hidden struct" interface.
* review] [sources] Adopt oyStructList_GetText() to "hidden struct" interface.
* [docu] Update doxygen comments for oyStructList_Count()
* review] [sources] Adopt oyStructList_ReleaseAt() to "hidden struct" interface.
* [docu] Update doxygen comments for oyStructList_GetRefType()
* review] [sources] Adopt oyStructList_GetRef() to "hidden struct" interface.
* [docu] Update doxygen comments for oyStructList_MoveIn()
* review] [sources] Adopt oyStructList_ReferenceAt_() to "hidden struct" interface.
* review] [sources] Adopt oyStructList_Get_() to "hidden struct" interface.
* [build] Arch Linux oyranos-git package
* [build] Move elektra Arch build files
* [docu]: Add LibRaw in Arch optional deps

#Version 0.4.0

###Kai-Uwe Behrmann (166):
* [conf]: set actual date
* [core]: skip message warning
* [docu]: fix module name
* [i18n]: update deutsch
* [core]: fix compile warnings
* [core]: print return values as info
* [build]: fix spec Url
* [core]: set correct deallocation for vsnprintf
* [core]: vsnprintf length for allocation
* [oyX1]: fix HDMI2 EDID in XRandR reading
* [core]: improve OYRANOS_DEBUG_MEMORY output
* [core]: replace printf with normal copy
* [oyX1]: initialise variable
* [core]: remove one more oyChar macro
* [oyX1]: clearify the Taxi output
* [oyX1]: correct X11 error code interpretation
* [alpha]: show ICC DB profile at comparision debug
* [core]: add fallback to wget
* [oyIM]: relax about missing profile ID
* [build]: sync with openSUSE spec file
* [conf]: fix remote internal elaktra builds
* [test]: simplify APPLE macro
* [test]: fix test cases for osX
* [exmpl]: fix osX build of image_display
* [conf]: add plug-in paths to oyranos-config
* [conf]: add dependencies to pkg-config --libs
* [core]: compile fixes for win32
* [build]: correct link order
* [conf]: initialy write yajl Makefile
* [core]: add uintptr_t
* [alpha]: skip stdint.h
* [core]: use HAVE_POSIX for S_ISLNK() macro
* [conf]: detect -lc need
* [core]: omit types on \_\_MINGW64__
* [core]: add intptr_t
* [core]: take atomic type definitions from lcms2
* [conf]: export EXEC_END
* [conf]: detect win32
* [core]: fix various compile warnings
* [conf]: add missed libraries to link flags
* [oyX1]: sort Taxi ranks in oyranos-monitor
* [oyX1]: print message if zero Taxi profiles
* [oyX1]: enable screen profile/edid search
* [oyX1]: fix compile error
* [oyX1]: fix typo
* [oyX1]: warning for non created monitor object
* [exmpl]: add small help text to image_display
* [oyX1]: configure non perfect detected monitors
* [core]: read root key
* [oyX1]: add more detail to warning for XOpenDisplay
* [oyX1]: show better warning for failed XOpenDisplay
* [test]: substitute specific monitor names
* [test]: fix wrong memory access
* [alpha]: fix memory offsets
* [lcm2]: reduce verbosity
* [test]: fix uninitialised memory accesses
* [build]: skip removed file
* [conf]: always use -fPIC
* [exmpl]: add Oy_Widget.h to image_display
* [oyX1]: ignore model_id currently
* [docu]: precise licensing of public domain file(s)
* [oyX1]: account for wrong EDID_model_id
* [alpha]: skip caching of to be installed profiles
* [docu]: fix some typos in oyX1
* [build]: add make to required build packages
* [exmpl]: add virtual class Oy_Fl_Window_Base
* [oyX1]: set correct warn message
* [exmpl]: sort classes into separate files
* [exmpl]: simplify frame_data allocation
* [exmpl]: skip draw arealess widget in image_display
* [exmpl]: fix offscreen drawing in image_display
* [exmpl]: add FL_Tile to image_display
* [exmpl]: add display image container to Fl_Oy_Group
* [exmpl]: use Fl_Oy_Group in image_display
* [exmpl]: add oyConversion_FromImageForDisplay()
* [exmpl]: support no roi in oyDrawScreenImage()
* [exmpl]: add getEditingProfile() to helpers
* [conf]: use NoDisplay for xdg desktop script
* [exmpl]: rename widget classes to reflect usage
* [oyX1]: ignore zero sized rectangle in "set_xcm_region"
* [test]: results with lcms2
* [alpha]: fix various compile warnings
* [alpha]: add oyRectangle_GetGeo()
* [oyX1]: add "set_xcm_region" handler
* [exmpl]: add a API10 example for option handling
* [oyX1]: adapt to colorimetry names from libXcm
* [conf]: update from compicc
* [conf]: bump to 0.4
* [alpha]: switch to x_color_region_target
* [alpha]: add taxi download in oyranos-profiles
* [alpha]: ignore profile position during taxi download
* [core]: reenable --key=val cli options
* [alpha]: split out installProfile in oyranos-profiles
* [oyX1]: better oyranos-monitor --list-taxi output
* [alpha]: rename oyConfig_GetFromTaxiDB to oyConfig_GetBestMatchFromTaxiDB
* [alpha]: rename oyDeviceProfileFromTaxiDB to oyProfile_FromTaxiDB
* [core]: exactly match command line args
* [oyX1]: list Taxi DB profiles for a local monitor
* [alpha]: omit warnings in oyDeviceToJSON for empty values
* [alphy]: show oyDevicesFromTaxiDB()
* [oyjl]: add verbosity to test program
* [oyjl]: keep yail handle alive until end of oyjl root node
* [alpha]: add oyDeviceProfileFromTaxiDB()
* [core]: support big files in oyReadUrlToMem_()
* [core]: fix some compiler warnings
* [alpha]: fix some oyranos-profile crashes
* [oyX1]: oyranos-monitor can show ranking
* [alpha]: add tolerant float comparision to oyTextIccDictMatch()
* [core]: locale independent oyStringToDouble()
* [oyX1]: add infos to show a device during a add_meta call
* [sources]: relax on empty key warnings
* [oyX1]: output xinerama geometry during nvidia EDID setup
* [oyX1]: support broken TwinViewXineramaInfoOrder API
* [docu]: update openSUSE build dependency section
* [docu]: update ubuntu/debian build dependency section
* [alpha]: check for kdialog inside KDE
* [alpha]: fix empty meta values
* [oyX1]: fix include
* [core]: support more argument in debug makros
* [core]: fix wrong argument parsing
* [core]: catch missed profile name in oyranos-profiles
* [oyX1]: catch wrong screen count
* [alpha]: specific caching of colour transforms
* [alpha]: fix oyCMMapi_Check_ wrong data access
* [lraw]: remove unused functions
* [lcm2]: put xml profile name into node text
* [lcm2]: default to full adaption for absolute intent
* [lcm2]: support adaption state for absolute intent
* [build]: fix make clean in non build source tree
* [test]: add "CMM deviceLink"
* [oyIM]: fix byte swapping while writing psid
* [docu]: avoid byte order ambiguity in digest
* [oyIM]: fix wrong printed psid text
* [i18n]: fix typo
* [alpha]: support module search for generic objects
* [source]: better format XML output for unknown objects
* [source]: get text strings from oyStruct_GetText()
* [source]: support arbitrary object strings
* [oicc]: support the oyProfiles_s object generic access
* [source]: add oyNAME_XML_VALUE number
* [test]: add monitor unset test
* [oyX1]: return failed property removal
* [oyX1]: oyranos-monitor --daemon inactive with colour server
* [build]: package oyranos-monitor-daemon script
* [i18n]: update deutsch
* [oyX1]: add --unset to oyranos-monitor
* [oicc]: rename to actual profile set
* [oyX1]: add oyranos-monitor-daemon script
* [oyX1]: add --daemon to oyranos-monitor
* [oyX1]: remove oyranos-monitor -b option
* [oyX1]: use verbose without oy_debug in oyranos-monitor
* [oyX1]: gix device selection on oyranos-monitor
* [build]: fix out of tree install of policies
* [exmpl]: use Xcm strings for qcmsevents
* [core]: add some variable argument helper versions
* [core]: replace certain characters for URLs
* [oyjl]: check data before accessing
* [alpha]: expose oyDeviceCheckProperties internally
* [core]: add download helper
* [CUPS]: fix wrong object pasing
* [conf]: bump libXcm requirement to 0.5
* [test]: check for identical monitors
* [alpha]: skip identical device domains
* [core]: add stream reader oyReadFileSToMem_()
* [conf]: let missing libXcm fail
* [docu]: update ChangeLog

#Version 0.3.2

###Boris Manojlovic (1):
* [build]: fix-lib-order

###Joseph Simon (3):
* [CUPS]: add resetPPDAttributes_()
* [CUPS]: add getOyConfigPrinter_+ resetPPDChoices_
* [docu]: typo fixes and make the document flow better

###Kai-Uwe Behrmann (183):
* [alpha]: add --name option to oyranos-monitor
* [build]: add license to spec file
* [build]: install oyranos-profile tool
* [conf]: avoid double library linking
* [conf]: sort link order
* [docu]: add oyranos-profile man page
* [build]: fix package names and desktop file installation
* [i18n]: update deutsch
* [yajl]: print text nodes
* [build]: package libraw device calibration embedd app
* [exmpl]: add libraw device calibration embedd app
* [core]: add OY_PARSE_INT_ARG2 for cli parsing
* [exmpl]: add --show to qcmsevents
* [oyX1]: do not fail
* [lraw]: render at full by default
* [alpha]: sensible file extension check
* [CUPS]: fix typo
* [CUPS]: document the device_context.PPD.ppd_file_t option
* [CUPS]: fall back to complete keyset
* [i18n]: fix typo in deutsch trasnlation
* [alpha]: be more relaxed on wrong arguments in oyConfDomain_GetText_
* [CUPS]: select colour option choice with fallbacks
* [CUPS]: use resetPPDAttributes_ instead of resetPPDChoices_
* [CUPS]: search for device context
* [CUPS]: mark "device_context" as "PPD.text"
* [oyX1]: use "set_device_attributes" with oyProfile_DeviceAdd()
* [alpha]: enable oyranos-profile -w -j options
* [test]: add "monitor JSON" test
* [alpha]: add oyDeviceFromJSON() API
* [conf]: expose static yajl objects in build scripts
* [alpha]: add oyDeviceFromJSON() API
* [alpha]: add "set_device_attributes" flag to oyProfile_DeviceAdd()
* [core]: fix memory leak in oyMakeDir_
* [yajl]: add oyjl_value_text() API
* [build]: fix spec file to match openSUSE packaging
* [CUPS]: fix C build warnings
* [build]: move file to match module sources
* [build]: fix liboyranos_module linking
* [build]: fix yajl on osX
* [build]: fix conflicting make variable
* [build]: require libraw and libyajl only >= oS-11.4
* [build]: if defaulting to detected external yajl
* [build]: copy yajl into sources and link if needed
* [yajl]: add internal version based on yail 1.0.12
* [conf]: detect a missed yajl and use internal one
* [i18n]: update deutsch
* [docu]: fine tune help text
* [i18n]: update deutsch in parts
* [core]: make oyranos-config-fltk help window resizeable
* [alpha]: fix over sensible error message
* [docu]: add oyranos-profile-install man page
* [conf]: hide oyranos-profile-install.desktop from menu
* [conf]: more robust yajl version detection
* [core]: fix typo in ICC tag name
* [i18n]: add new texts to oyranos.pot
* [core]: show help text by F1 in oyranos-config-fltk
* [core]: export HTML over oyranos-policy --help
* [core]: export core option desriptions to HTML
* [core]: add oyDescriptionToHTML() API
* [docu]: describe reason, why a missed ICC profile is bad
* [core]: fix string lenght in oyStringAddPrintf_()
* [docu]: fix man page typo
* [core]: remove unused path configuration code in oyranos-config-fltk
* [oforms]: remove hint to non yet available man pages
* [oforms]: support --help by oyranos-xforms
* [core]: use tooltips for oyranos-policy dump
* [i18n]: update deutsch in parts
* [core]: add long descriptions to core options
* [oforms]: remove printf
* [oforms]: set correct default choice
* [oicc]: set description as xf:help for xf:select1
* [oforms]: show description of headlines
* [oforms]: use xf:group labels on cmd line
* [oforms]: show the xf:group labels
* [lcm2]: add help descriptions to specific options
* [lcms]: add help descriptions to specific options
* [oicc]: move h3/h4 elements to <xf:group type=[h3,h4]>
* [core]: add a oyOption_t_::tooltip and make descriptions accessible
* [core]: add oyXFORMsModelGetAttrValue()
* [alpha]: adapt oyranos-profile to non enumerated JSON devices
* [lraw]: convert printf into message
* [oyRE]: accept DNG in is_raw()
* [yajl]: catch on minor user errors
* [build]: add oyranos-profile-install script to fix desktop file
* [lraw]: add crw and other endings
* [alpha]: use only JSON array in oyranos-profile -o
* [oyRE]: add std namespace to fix compile error
* [alpha]: add more UIs for oyranos-profiles --gui option
* [alpha]: skip the numbered JSON device level in oyranos-profiles
* [core]: add oyFindApplication()
* [core]: remove ignored entry
* [core]: add oyranos-profile-install xdg meny entry
* [core]: support file:// prefix
* [docu]: add man page for oyranos-profiles tool
* [alpha]: add oyranos-profile --gui --install
* [alpha]: show resolved user path name in oyranos-profiles
* [docu]: fix typo
* [docu]: move REPORT BUGS to BUGS man page section
* [core]: fix compile warning
* [build]: pachage qcmsevents man page
* [exmpl]: fix compile warnings
* [docu]: add qcmsevents man page
* [build}: test for yajl_version.h
* [core]: fix compile warnings
* [build]: use bzip2 for RPM source package
* [docu]: add oyranos-monitor-nvidia man page
* [build]: install man pages
* [docu]: add oforms tools man pages
* [docu]: fix web link in man page
* [build]: add liboyranos_modules dummy
* [oyX1]: fix typo
* [build]: omit double dependency tracking for oyranos_monitor.c
* [build]: add libyajl-devel to RPM spec file
* [oyX1]: include "prefix" key to meta tag
* [alpha]: omit "Could not open device" message
* [alpha]: remember data for serialised profile tags
* [alpha]: unset ID for modiefied in memory profiles
* [oyX1]: avoid included oyranos_monitor.c
* [alpha]: serialise before requesting ICC md5
* [build]: fix make output text
* [alpha]: write correct ID in oyranos-monitor
* [oyX1]: revert #5c9f114f return device from "add_meta" command
* [alpha]: use oyjl API in oyranos-profile
* [oyX1]: return device from "add_meta" command
* [yajl]: fix typo
* [build]: link oyjl and yajl to liboyranos
* [oyRE]: fix compiler warning
* [oyRE]: harmonise manufacturer, model and serial keys across device modules
* [yajl]: update to v2.0 API
* [yajl]: add reasy JSON access API based on yajl
* [alpha]: keys with double point trasnlate to a JSON array
* [oyRE]: harmonise meta tag namespace
* [oyX1]: add prefix key to deviec calibration indo
* [oyX1]: use add_meta key for meta tag embedding
* [conf]: require yajl
* [alpha]: prefix is one string value in JSON
* [oyX1]: prefix OPENICC_ key with OYRANOS_ because lack of interesst
* [alpha]: support -c and -s in oyranos-profile
* [alpha]: support -n and -p in oyranos-profile
* [build]: add oyranos-profile
* [core]: share cli macros
* [alpha]: oyranos-profile info tool
* [lcms]: adapt lcm2 OpenMP private cache
* [lcm2]: separate in and out channels count
* [alpha]: rank OPENICC_automatic_generated meta key lower
* [alpha]: strip namespace from "serial" meta key
* [oyX1]: add OPENICC_automatic_generated meta key
* [alpha]: accept full length meta key names
* [alpha]: add verbosity to implicite profiles
* [alpha]: use only Meta backends of ranked
* [lcm2]: set OpenMP private cache
* [build]: add hint about download_grantlee.sh
* [exmpl]: fix image_display start without image
* [oPNG]: add more channel debug indos
* [lcm2]: add more debug infos
* [exmpl]: fix memory leak at image_display exit
* [exmpl]: image_display defaults to OpenGL
* [exmpl]: centred placement in image_display
* [exmpl]: fix skewing in image_display GL view
* [conf]: prefere lcms2 as default
* [SANE]: compile without lcms1
* [lcm2]: fix ICC XYZ PCS scaling factor
* [lcm2]: normalise lcms2 XYZ to ICC/CIE*XYZ
* [alpha]: show avialable standard ICC paths
* [exmpl]: show osX path names in oyranos-profiles -p
* [build]: support install into path names with spaces
* [alpha]: fix empty profile access
* [alpha]: remove some more oyName_e references
* [oforms]: support stdin
* [core]: add oyReadStdinToMem_() helper
* [build]: rename internal module RPMs
* [docu]: no empty space as last character in man
* [build]: fix macros in comments
* [conf]: fix desktop file
* [docu]: include generated code out of tree
* [templates]: fix unused variable warnings
* [conf]: use System/Monitoring for qcmsevents
* [core]: avoid intptr_t and ptrdiff_t for pointer
* [sources]: cast from pointer to uintptr_t
* [sources]: cast from pointer to ptrdiff_t
* [core]: compare using ptrdiff_t
* [oyRE]: add missed include for ptrdiff_t
* [conf]: bump version

###Yiannis Belias (2):
* build]: Keep a PKGBUILD for Arch Linux elektra package
* docu]: add Arch Linux package dependencies

#Version 0.3.1

###Kai-Uwe Behrmann (30):
* [core]: use C compliant log2()
* [core]: revert previous #39a22a16
* [core]: add C header for clock()
* [sources]: use POSIX header for setlocale
* [exmpl]: add missing -lintl on osX
* [build]: use appropriate flags for FLTK oforms
* [oyIM]: do not swap mBA curve channel names
* [oyIM]: show correct precission for mAB nLUTs
* [oyIM]: support icSigLutBtoAType alias mBA
* [oyIM]: split out oyWriteIcSigLutAtoBTypeNlut()
* [oyIM]: set correct curv count
* [oyIM]: fix curve sequence size
* [i18n]: update deutsch
* [oyIM]: describe para data
* [oyIM]: send segmented curve along para
* [oyIM]: set parameters count once
* [oyIM]: describe parameter count
* [oyIM]: complete para-metric tag type
* [sources]: add oyStructList_GetName()
* [alpha]: add oyProfileTag_Get()
* [oyIM]: add curv and mAB reading
* [alpha]: add oyProfileTag_CreateFromData()
* [core]: add more ICC tag types
* [core]: add oyValueInt32()
* [alpha]: support id wildcart in oyImage_PpmWrite
* [alpha]: extract oyImage_PpmWrite() from oyra
* [alpha]: add id to debug device link
* [exmpl]: handle GL view events
* [exmpl]: move draw() out of base class
* [conf]: bump version

#Version 0.3.0

###Kai-Uwe Behrmann (350):
* [build]: avoid circular RPM requirement
* [build]: own the new "object" header directory
* [alpha]: oyConversion_Change -> oyPixelAccess_Ch.
* [lcms]: copy array for new tickets
* [alpha]: give structs a name
* [alpha]: ignore array allocation without size
* [lcms]: add missed array warning
* [lcm2]: notice missing array only with transform
* [alpha]: check errors in oyImage_FillArray()
* [alpha]: care about missed ticket array
* [alpha]: keep ditry ticket array
* [alpha]: split allocation and filling
* [alpha]: expose oyArray2d_SetFocus()
* [alpha]: compile fix for array variable
* [oyra]: move array back to initial position
* [test]: add various array testing
* [alpha]: fix oyArray2d_SetFocus() shifting
* [core]: use for rounding of floats
* [conf]: update date
* [alpha]: fix oyImage_ReadArray() rectangles
* [alpha]: fix wrong memory release
* [alpha]: partitial revert #0728c9cd
* [test]: free image data
* [alpha]: omit array comparision
* [oydi]: be more explicite with debug messages
* [oydi]: rename rectangles
* [exmpl]: silence debug output
* [exmpl]: clip pixel rectangle
* [test]: remove start position reset
* [alpha]: adjust debug messages
* [oyra]: allign input and output rectangles
* [lcm2]: add error message for missed ticket array
* [oyra]: use same output like input region
* [alpha]: dont exceed array width
* [exmpl]: simplify the OpenGL widgets drawing
* [exmpl]: support threading in image_display
* [test]: small lower right output rectangle RoI
* [alpha]: oyFilterPlug_ImageRootRun() uses ticket
* [alpha]: add oyArray2d_SetFocus()
* [alpha]: oyPixelAccess_s::start_xy is now double
* [oyra]: scale origin to local image coordinates
* [oydi]: remove unneeded variables
* [test]: fix one pixel rectangle test
* [alpha]: add oyConversion_ChangeRectangle()
* [alpha]: partitial revert #d0e01567
* [test]: Region of Image conversion
* [test]: omit iterator testing
* [alpha]: partitially revert 542fa3a1
* [alpha]: more lightwight array row referencing
* [alpha]: reference array rows in filter calls
* [alpha]: omit forward iteration
* [alpha]: fix array assignment without copy
* [alpha]: change oyConversion_GetOnePixel() API
* [test]: check OnePixel
* [oyX1]: print xcalib system call
* [build]: require more lightweight icc-profiles
* [exmpl]: add "--set" option to oymd5icc
* [alpha]: write profile ID
* [core]: improve error messages for profile ID
* [alpha]: select profile ID in oyProfile_GetMD5()
* [core]: probe file access in oyIsFileFull_()
* [alpha]: export error from oyProfile_FromMemMove_
* [i18n]: update deutsch
* [exmpl]: add iterate over profiles to cli docu
* [sources]: fix md5 hash check
* [sources]: fix return value
* [lraw]: render at half size
* [lraw]: omit debug image
* [lraw]: debug message for obtained profile
* [alpha]: add missing icc_profile
* [lraw]: image pass through of config options
* [oyRE]: add meta tag
* [lraw]: clean code
* [docu]: describe new lraw options
* [oyRE]: add namespaces
* [lraw]: fix namespaces
* [alpha]: support number input for meta tag
* [alpha]: support n names spaces for meta tag
* [oyRE]: fix getting context variable
* [alpha]: check before freeing pointer
* [sources]: omit Elektra makro
* [alpha]: fix symbol name
* [alpha]: add oyConversion_GetNode()
* [lcms]: V4 value range for Cmyk
* [lcm2]: add module specific defaults to hash
* [lcm2]: add core options to hash text
* [alpha]: correct previous commit
* [alpha]: improve warnings for BasicPixels
* [lcms]: use V4 float value ranges
* [lcm2]: rename layout funtion
* [alpha]: oyConversion_CreateBasicPixelsFromBuffers
* [alpha]: swap oyConversion_FromBuffers() args
* [source]: revert previous #18564c6
* [sources]: extent the oyOptions_SetFromText() API
* [sources]: more verbose error message
* [sources]: improved check in oyStruct_CheckType
* [core]: switch DevS to ICC types
* [exmpl]: fix typo
* [oyra]: fix crash with missed image extension
* [core]: add dummy oyFilterMessageFunc
* [sources]: copy all in oyOptions_SetOpts()
* [alpha]: add oyConversion_FromBuffers()
* [lcm2]: add "proofing_effect"
* [test]: check "create_profile.proofing_effect"
* [lcms]: add "proofing_effect" creation
* [alpha]: free unused memory in oyOptions_Handle
* [alpha]: let oyOptions_Handle cover more modules
* [oyX1]: shrink search for optins command
* [i18n]: update deutsch(de)
* [docu]: fix typo
* [conf]: sync base script with ICC Examin
* [exmpl]: place verbosity on request
* [exmpl]: fix parsing of multiple arguments
* [exmpl]: add --use_gl to image_display
* [exmpl]: add Oy_Fl_Shader_Box
* [API]: oyFilterMessageFunc -> oyMessageFunc
* [alpha]: check for new default Cmyk profile
* [lcms]: fix proofing bug
* [sources]: fix oyPointer_s string release warnings
* [sources]: move Pointer to objects_generic
* [templates]: remove date and version per object
* [templates]: document file date, not file start
* [exmpl]: add Oy_Fl_Box::observeICC()
* [exmpl]: format Oy_Widget::observeICC()
* [sources]: no static only strings in oyPointer
* [core]: add internal oyStringFree_()
* [sources]: release inheritance array
* [sources]: add warnings around static inheritance
* [core]: fix oyStruct_GetInfo output
* [sources]: rename oyCMMptr_s to oyPointer_s III
* [sources]: complete previous commit db257d53
* [sources]: rename oyCMMptr_s to oyPointer_s
* [docu]: undocument deviceSettingsType
* [docu]: update doxygen files
* [build]: update RPM for liboyranos_object
* [SANE]: adapt to oyCMMapiGetText_f changes
* [test]: add compile test for liboyranos_object
* [core]: add missing string for object enum
* [core]: switch to new openicc-data set for Cmyk
* [qarz]: adapt to oyCMMapiGetText_f changes
* [oyRE]: oyCMMapiGetText_f + oyConnectorImaging_s
* [exmpl]: detect colour server only with NCR
* [sources]: fix member copy declaration
* [templates]: make contructor obligatory
* [lcm2]: add inheritance to oyConnectorImaging_s
* [alpha]: add oyObjectInfoStatic_s
* [sources]: add oyStruct_CheckType()
* [templates]: set types in order of inheritance
* [templates]: adapt oyObject_s functions #696bd6
* [templates]: adapt oyObject_Ref to oyObject_s
* [templates]: trace inheritance in oyObject_s
* [alpha]: remove check for copy object
* [alpha]: fix crash for missed rendering context
* [templates]: break oyObject_s_ struct
* [exmpl]: fix crash in image_dispay class
* [CUPS]: adapt oyCMMapiGetText_f in more modules
* [test]: fix compile error for oyCMMapiGetText_f
* [exmpl]: adapt plugins to compile again
* [alpha]: remove oyName_e from oyImageConnector_s
* [alpha]: add image connector text helpers
* [alpha]: remove private class headers
* [templates]: add oyXXXs_Sort() and _Clear()
* [templates]: clean old macro
* [sources]: add oyCMMptr_SetSize()
* [alpha]: fix compiler warnings
* [sources]: add oyOption_GetValueString()
* [alpha]: change the oyCMMGetText_f API
* [sources]: make oyName_s opaque
* [alpha]: remove oyObject_s code
* [test]: switch to new Connector API
* [sources]: add oyObject_CopyNames()
* [alpha]: add oyConnector_s member access APIs
* [alpha]: remove oyConnector_s::oyName_s type
* [build]: swap build order for object and core
* [test]: fix crash after non loaded library
* [conf]: set complete library version on Darwin
* [core]: apply file filter not on whole path
* [conf]: add -gdwarf-2 for debugging in Xcode
* [alpha]: add module version debug info for failed check
* [test]: fix crash for no loaded module
* [exmpl]: avoid internal APIs
* [alpha]: add oyProfile_GetMd5()
* [sources]: add oyStruct_GetAllocator()
* [qarz]: compile fixes for hidden internal APIs
* [docu]: lcms/oyIM move from oyName to oyOption
* [sources]: move oyName_s to internal structs
* [sources]: add oyOption_GetFlags()
* [build]: verbose install libraries
* [exmpl]: show move cursor in image_display
* [test]: switch back to oyConfig_EraseFromDB()
* [alpha]: oyConfig_AddDBData() accepts key path
* [build]: refresh no generated code after clean
* [sources]: move oyOptions_SaveToDB() out
* [test]: add test for oyRegistrationEraseFromDB()
* [alpha]: change oyOptions_SaveToDB() API
* [alpha]: add oyRegistrationEraseFromDB()
* [test]: check oyConfig_EraseFromDB()
* [alpha]: honour object arg in oyOptions_ForFilter
* [test]: fix compile warning
* [sources]: initialise variable for oyStructList_s
* [sources]: fix some compiler warnings
* [alpha]: fix copying of oyProfiles_s
* [sources]: fix wrong object deallocation
* [core]: fix string list end
* [oyRE]: better format camera model list
* [alpha]: lessen module API warning
* [exmpl]: update modules to 0.3.0
* [API]: require module_api versions >= 0.3.0
* [conf]: bump version to 0.3.0
* [conf]: add liboyranos_object to pkg-config
* [sources]: remove smartnes in oyHash_IsOf_()
* [sources]: add oyHash_GetType()
* [alpha]: adapt to to changed oyCMMptr_New_() API
* [templates]: fix outdated object pointer
* [build]: use oyAPIGenerator out of source tree
* [docu]: describe how to build oyAPIgenerator
* [build]: generate sources
* [docu]: move files to generator
* [templates]: fix C++ guards
* [build]: statical library
* [sources]: add oyThreadLockingSet()
* [SANE]: adapt to oyBlob_s and oyCMMptr_s APIs
* [oyX1]: adapt to oyBlob_s API changes
* [API]: add oyBlob_s and oyHash_s
* [SANE]: adapt to changed object API
* [core]: include missed internal header
* [API]: add oyOption_s and other core objects
* [API]: add object oyName_s
* [core]: remove string function macros
* [core]: build always string function wrappers
* [build]: fix make deb
* [build]: fix out of source tree compiling
* [oyRE]: fix compile issue
* [API]: add generated code for oyStruct_s
* [API]: add object oyStruct_s
* [build]: install new liboyranos_object
* [sources]: sync object_core with main git
* [docu]: fix function modification date
* [build]: add new module library target
* [sources]: update Configs to current git
* [build]: change variable reference
* [sources]: remove oyHash_CopyRef_
* [generator]: update modified sourses
* [docu]: add license file - new BSD
* [docu]: update build instructions and README
* [build]: compile generator from top makefile
* [sources]: make Config visible again
* [sources]: fix some first Profile compile issues
* [build]: link dynamical
* [sources]: update Config to actual git
* [docu]: enhance object description and fix dates
* [build]: fix minor issues
* [build]: explicitely linke libraries
* [build]: link against all basic libraries
* [build]: in old directory layout
* [generator]: move to own directory
* [build]: use simple makefile
* [build]: restructure diretories
* [sources]: update to two funcs in core
* [build]: compress package with bzip2
* [build]: link against local lib for oforms
* [build]: fix pkg-config file to new library
* [build]: bundle and clean test_core
* [build]: update spec to new liboyranos_core
* [build]: link against core
* [sources]: add oyranos_core.h
* [templates]: add oyranos_object.templates.c
* [sources]: header adjustments to fix compilation
* [templates]: add static oyObject_e enum
* [cmake]: compile oyStruct_s.c
* [saources]: add more value APIs
* [sources]: compile Elektra APIs if requested
* [sources]: remove non core API headers
* [templates]: add oyBlob_s data accessors
* [core]: move version API to core
* [core]: move internaly to core headers
* [alpha]: add Blob accessor functions
* [core]: split core code out of io
* [API]: no object reference in messaging APIs
* [core]: move oyExportResetSentinel_ to core
* [core]: fix compile warning
* [core]: fix linking with liboyranos_core.a
* [oyX1]: support cloned outputs
* [core]: show only existing messages
* [core]: split core sentinel functions out
* [core]: move basic object info provider to core
* [build]: fix test_core target
* [build]: install liboyranos_core.a
* [core]: fix long messages
* [CUPS]: fix string allocation size
* [core]: add oyMessageFormat()
* [core]: add oyStruct_GetInfo()
* [build]: move text functions out of core
* [core]: build liboyranos_core.a
* [core]: split core out of CM functions
* [core]: remove old XML based module API
* [includes]: add Oyranos core includes
* [templates]: update data
* [templates]: add missed headers
* [templates]: add missed oyTextboolean_()
* [templates]: deactiave Filter function
* [templates]: fix template name
* [templates]: readd enum to string function
* [templates]: substiture private function
* [templates]: fix compiler warning
* [templates]: not namespaces in C compiled lib
* [templates]: fix compiler warning
* [templates]: fix foreign member access
* [sources]: add StructList::parent_type_ public
* [sources]: print object type
* [sources]: fix compiler warning
* [sources]: move code up in object hierarchy
* [sources]: access Option::source
* [sources]: fix compiler errors
* [sources]: make oyObject_UnRef() public
* [sources]: CMMptr fix typos, exchange macro
* [sources]: add member accessors
* [cmake]: add debug flags
* [sources]: sort non object code out
* [conf]: sync with ICC Examin
* [build]: install libraw test file
* [conf]: correct libraw testing
* [docu]: update versions and requirements
* [docu]: fix doxygen version
* [conf]: test more careful libraw
* [conf]: use -fPIC only on 64bit builds
* [exmpl]: fix example compilation
* [build]: add libraw-devel-static to spec
* [conf]: remove oyranos-icc
* [build]: add forms package and lraw + oyRE
* [docu]: ifx some doxygen warnings
* [build]: switch to oyranos-xforms-modules name
* [conf]: search for libraw.pc
* [build]: properly install oFORMS tools
* [build]: install qcmsevents png icon
* [build]: switch to libraw.pc
* [conf]: update qcmsevents desktop file
* [docu]: more requirements details
* [alpha]: add oyConversion_GetGraph()
* [alpha]: cache generated fallback profile
* [oyX1]: relase resources
* [oyX1]: embed meta tag in generated ICC profile
* [oyX1]: generate more unique profile name
* [exmpl]: simplify code
* [core]: make profiling instrumentation available
* [conf]: bump version
* [oyX1]: fix compile issue with XRandR headers
* [docu]: add Yiannis Belias to AUTHORS
* [docu]: strongly require OpenICC-data
* [docu]: fix typo

###Nicolas Chauvet (1):
* [conf]: apply Fedora patch for pkg-config

###Stephen Kelly (2):
* Use GRANTLEE_PLUGIN_PATH instead of /usr/local/lib
* Delete engine pointer.

###Yiannis Belias (553):
* Create skeleton files for oyFilterPlugs_s
* Create skeleton files for oyFilterPlugs_s
* Create skeleton files for oyFilterSocket_s
* generator] Ignore *~ files
* Create skeleton files for oyConnector_s
* generator] Fix bug of wrong input name to render()
* generator] Return the new filename of rendered template
* generator] Print New/Old class status
* generator] Automaticly create the *.private_custom_definitions.c
* generator] Add a ClassTemplates::getNewClasses() member function
* generator] Make debug message less scary :)
* cmake] Add ClassTemplates.cc to build system
* generator] Move previous functionality of main()
* generator] Add the interface of new ClassGenerator class
* doc] Update TODO
* generator] Remove unused code
* generator] Make comment more explicit
* templates] Add include files to CMMapi4_s_.h
* Create skeleton files for oyFilterNode_s
* templates] Add include files to oyCMMapi3_s_.h
* templates] Add 2 include files to oyranos_module.h
* Create skeleton files for oyFilterCore_s
* doc] Update README
* templates] Fix blank line on top of file
* cmake] Update CMakeLists.txt file to better configure
* generator] Fix the getTemplateParents() function
* templates] Print the template file inheritance graph in the source file header
* generator] Make the standard dirs global
* doc] Update TODO
* review] [sources] Adopt oyHash_GetPointer_() to "hidden struct" interface.
* review] [sources] Adopt oyHash_SetPointer_() to "hidden struct" interface.
* review] [sources] Adopt oyHash_IsOf_() to "hidden struct" interface.
* review] [sources] Adopt oyHash_Get_() to "hidden struct" interface.
* review] [sources] Adopt oyHash_CopyRef_() to "hidden struct" interface.
* sources] Import private methods for oyHash_s
* review] [sources] Implement the destructor for oyHash_s
* review] [sources] Implement the copy constructor for oyHash_s
* templates] Enable customize if branch in oyClass_Copy_()
* review] [sources] Implement the constructor for oyHash_s
* templates] Fix typo
* sources] Fix export declarations of public api for oyProfile_s
* review] [sources] Adopt oyProfile_DeviceGet() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetFileName() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_AddTagText() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_TagReleaseAt() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_TagMoveIn() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetTagCount() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetTagById() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetTagByPos() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetMem() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetText() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_Equal() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetID() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetChannelName() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetChannelNames() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_SetChannelNames() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_SetSignature() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetSignature() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetChannelsCount() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromMD5() to "hidden struct" interface.
* sources] Remove duplicate function definition
* review] [sources] Adopt oyProfile_FromSignature() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromMem() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromFile() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromStd() to "hidden struct" interface.
* templates] Include files in oyranos_generic_internal.h
* templates] oyClass_s_.c of group objects_generic includes oyranos_generic_internal.h
* templates] Add 2 new files for Generic Objects API
* templates] Include files in oyProfile_s.h
* templates] Remove include file
* templates] oyClass_s_.c of group module_api includes oyranos_module_internal.h
* templates] New Module APIs private include file.
* templates] Include icc34.h in oyProfile_s
* sources] Import public methods for oyProfile_s
* templates] Add Profile API doxygen description
* templates] Add oyCacheListGetEntry_() to oyranos_object_internal.h and remove oyCMMsGetFilterApis_()
* templates] Include oyranos_icc.h in oyProfile_s_.c
* sources] Fix missing oyProfileTag_s cast
* review] [sources] Adopt oyProfile_WriteTagTable_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_WriteTags_() to "hidden struct" interface.
* templates] Include oyProfileTag_s_.h in oyProfile_s_.c
* sources] Add some defines to oyProfile.h
* review] [sources] Adopt oyProfile_WriteHeader_() to "hidden struct" interface.
* review] [sources] Rename oyProfile_DeviceAdd() to oyProfile_DeviceAdd_()
* review] [sources] Adopt oyProfile_DeviceAdd() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_TagReleaseAt_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_TagMoveIn_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetTagCount_() to "hidden struct" interface.
* review] [sources] Adopt GetTagByPos_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_GetFileName_r() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_Hashed_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_Match_() to "hidden struct" interface.
* templates] Include oyStructList_s_.h in Profile_s_.c
* templates] Include oyHash_s_.h instead of oyHash_s.h
* review] [sources] Adopt oyProfile_FromFile_() to "hidden struct" interface. [Part II]
* templates] Include oyHash_s.h in Profile_s_.c
* Create skeleton files for oyHash_s
* review] [sources] Adopt oyProfile_ToFile_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_TagsToMem_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromFile_() to "hidden struct" interface.
* review] [sources] Adopt oyProfile_FromMemMove_() to "hidden struct" interface.
* templates] Add include files to oyProfile_s_.h
* templates] Add oySTATUS_e to oyranos_object.h
* templates] Add icc34.h to oyProfileTag_s_.h
* Create skeleton files for oyProfileTag_s
* doc] Update README
* cmake] Look in the source dir for include files, too
* doc] Update TODO
* sources] Import private methods for oyProfile_s
* sources] Fix missing _ in allocateFunc_
* templates] Include oyranos_profile.h in oyProfile_s.h
* templates] Add a new header file for the profiles API group
* review] [sources] Implement the destructor for oyProfile_s
* review] [sources] Implement the copy constructor for oyProfile_s
* review] [sources] Implement the constructor for oyProfile_s
* templates] Include files in oyProfile_s_.h
* templates] Add objects_profile in oyOBJECT_e
* Create skeleton files for oyProfile_s
* review] [sources] Adopt oyDeviceSelectSimiliar() to "hidden struct" interface.
* templates] Include files in oyranos_devices.c
* templates] Add OY_ERR macro to oyranos_devices_internal.h
* templates] Include "oyranos_object_internal.h" in oyranos_devices.c
* review] [sources] Adopt oyDevicesGet() to "hidden struct" interface.
* templates] Add include files in oyranos_devices.[ch]
* sources] Adopt arguments of private functions to private classes in oyOptions_s
* sources] Add 2 private functions to devices_handling group
* templates] Remove from oyranos_devices_internal.* 2 functions.
* doc] Update TODO
* review] [sources] Adopt oyDeviceProfileFromDB() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceSetProfile() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceAskProfile2() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceGetInfo() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceUnset() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceSetup() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceBackendCall() to "hidden struct" interface.
* review] [sources] Adopt oyDeviceGet() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetDeviceTextKey_() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetDeviceTextKey_() to "hidden struct" interface.
* templates] Add oyranos_devices* to cmake
* templates] Import the Device API functions
* templates] Include <oyranos_sentinel.h> in Config_s.c
* templates] Add oyCMMsGetFilterApis_() to oyranos_object_internal.h
* tempaltes] Add include files to oyCMMapi8_s_.h
* templates] Include "oyCMMapi_s.h" in CMMInfo_s_.h
* doc] Update TODO
* templates] Override block in oyCMMapiFilters_MoveIn()
* Create skeleton files for oyCMMapiFilters_s
* templates] Add include files to oyConfig_s.c
* sources] Fix wrong pointer type
* review] [sources] Adopt oyConfigDomainList() to "hidden struct" interface.
* review] [sources] Change oyConfigs_SelectSimiliars to oyConfigs_SelectSimilars (typo)
* review] [sources] Adopt oyConfigs_Modify() to "hidden struct" interface.
* review] [sources] Adopt oyConfigs_FromDB() to "hidden struct" interface.
* review] [sources] Adopt oyConfigs_FromDeviceClass() to "hidden struct" interface.
* review] [sources] Adopt oyConfigs_FromDomain() to "hidden struct" interface.
* sources] Import public methods for oyConfigs_s
* sources] Remove duplicate function declarations
* review] [templates] Change the error check
* templates] Override block:MoveIn_Observed
* templates] Add missing flags to oyStructList_MoveIn
* Create skeleton files for oyConfigs_s
* templates] Add include files to oyConfig_s.h
* templates] Import oyRankPad struct
* doc] Update TODO
* review] [sources] Add an extra oyConfig_s constructor to replace the old non-default one
* review] [sources] Adopt oyRankMapCopy() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_Get() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_Count() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_Find() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_Has() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_FindString() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_DomainRank() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_Compare() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_EraseFromDB() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_SaveToDB() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_GetDB() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_ClearDBData() to "hidden struct" interface.
* review] [sources] Adopt oyConfig_AddDBData() to "hidden struct" interface.
* sources] Import public methods for oyConfig_s
* review] [sources] Implement the destructor for oyConfig_s
* review] [sources] Implement the copy constructor for oyConfig_s
* templates] Add error checking to copy constructor
* review] [sources] Implement the constructor for oyConfig_s
* Create skeleton files for oyConfig_s
* sources] Import oyCMMapi10_s private typedef
* sources] Import oyCMMapi9_s private typedef
* sources] Import oyCMMapi8_s private typedefs
* sources] Import oyCMMapi4_s private typedefs
* sources] Import oyCMMapi6_s private typedef
* sources] Import oyCMMapi7_s private typedef
* templates] Import common typedef for api4&5 in oyranos_module.h
* sources] Import oyCMMapi5_s private typedefs
* Move oyCMMapi_s private typedefs to CMMapi.private.h
* Create skeleton files for oyCMMui_s
* sources] Import typedefs of oyCMMobjectType_s
* Create skeleton files for oyCMMobjectType_s
* sources] Add more include files to CMMapi.private_methods_definitions.c
* sources] Add the oyCMMProfileTag_*_f typedefs to CMMapi3.private.h
* sources] Include oyCMMapi3_s_.h in CMMapi.private_methods_definitions.c
* templates] Add an ending CMM member in oyOBJECT_e
* doc] Update TODO
* templates] Group oyOBJECT_e members by group
* review] [sources] Adopt oyCMMapi3_Query_() to "hidden struct" interface.
* sources] Import private methods for oyCMMapi3_s
* Create skeleton files for oyCMMapi3_s
* grantlee] Fix CamelcaseToUnderscoreFilter to also work with numbers
* templates] Call the parent destructors
* templates] Call the parent copy constructors
* templates] Call the parent constructors
* generator] ClassTemplates uses a new Null ClassInfo as parent of Struct
* generator] Correct typo
* generator] oyStruct_s returns "" as parentName
* generator] Have the oyStruct_s class info avaliable, just in case
* generator] Add support for a private header file
* sources] Add oyRegistrationData_s private struct
* sources] Import and adopt oyCMMapi4_selectFilter_()
* Create skeleton files for oyCMMapi4_s
* Create skeleton files for oyCMMapi10_s
* Create skeleton files for oyCMMapi9_s
* doc] Update TODO
* Create skeleton files for oyCMMapi8_s
* Create skeleton files for oyCMMapi6_s
* Create skeleton files for oyCMMapi7_s
* Create skeleton files for oyCMMapi5_s
* generator] First try for getTemplateParents()
* templates] Intended usage of parents variable
* doc] Update TODO
* sources] Remove CMMapi members from CMMapiFilter
* templates] Replace Django with Qt date formatting
* Create skeleton files for oyCMMapiFilter_s
* sources] Fix "type" -> "type_"
* templates] Add the oyIcon_s struct to oyranos_module.h
* templates] Add the oyCMMGetText_f typedef to oyranos_module.h
* templates] Add oyCMMInfo_s_.h to CMMapi_s_.h
* sources] Private methods take the hidden class pointer
* template] Remove forgotten filter test argument
* grantlee] Get correctly the argument string
* review] [sources] Implement the destructor for oyCMMInfo_s
* review] [sources] Implement the copy constructor for oyCMMInfo_s
* Create skeleton files for oyCMMInfo_s
* doc] Update TODO
* build] Count new grantlee_extensions/ files
* sources] Mark CMMapi as internal and [notemplates]
* templates] Update last 2 occurences of deprecated enumName
* templates] Update to use the new "underscore" and "tr" filters
* grantlee] Add a new "tr" filter
* generator] Remove support for camel2under()
* templates] Include oyranos_module.h from CMMapi_s_.h
* templates] Add a new header file "oyranos_module.h"
* doc] Update TODO
* Rename grantlee plugin to "gsoc_extensions"
* templates] Load the custom extensions library
* grantlee] Import the camel2under() function as a filter
* grantlee] Add the boilerplate code for qt plugins
* grantlee] Include the grantlee use file
* grantlee] Add grantlee_extensions directory
* generator] Find custom grantlee plugins when run from build dir
* review] [sources] Adopt oyCMMapi_selectFilter_() to "hidden struct" interface.
* review] [sources] Adopt oyCMMapi_CheckWrap_() to "hidden struct" interface.
* review] [sources] Adopt oyCMMapi_Check_() to "hidden struct" interface.
* sources] Import private methods for oyCMMapi_s
* Create skeleton files for oyCMMapi_s
* doc] Update TODO
* review] [sources] Adopt oyCMMptr_ConvertData() to "hidden struct" interface.
* review] [sources] Adopt oyCMMptr_Set() to "hidden struct" interface.
* review] [sources] Adopt oyCMMptrLookUpFromText() to "hidden struct" interface.
* sources] Adopt oyCMMptr_Set_() to "hidden struct" interface.
* sources] Import public methods for oyCMMptr_s
* review] [sources] Adopt oyCMMptr_Set_() to "hidden struct" interface.
* sources] Import private methods for oyCMMptr_s
* review] [sources] Implement the destructor for oyCMMptr_s
* review] [sources] Implement the copy constructor for oyCMMptr_s
* review] [sources] Implement the constructor for oyCMMptr_s
* Create skeleton files for oyCMMptr_s
* templates] Use the new underscores filter
* doc] Update TODO
* sources] Ifdef oyOption_SetValueFromDB() to be used when elektra is imported
* sources] Fix compile error: use the private Blob class
* templates] Add oyBlob_s private header to oyOption_s.c
* templates] Include the public class header from the private class header.
* review] [sources] Adopt oyBlob_SetFromStatic() to "hidden struct" interface.
* review] [sources] Adopt oyBlob_SetFromData() to "hidden struct" interface.
* sources] Import public methods for oyBlob_s
* review] [sources] Implement the destructor for oyBlob_s
* review] [sources] Implement the copy constructor for oyBlob_s
* review] [sources] Implement the constructor for oyBlob_s
* templates] Fix logical error
* Create skeleton files for oyBlob_s
* Ifdef oyOptions_Handle() to hide until oyCMMApiFilters gets imported
* doc] Update TODO
* review] [sources] Adopt oyOptions_ObserverAdd() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SaveToDB() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetSource() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_FindData() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetFromData() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_MoveInStruct() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_GetType() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetFromDouble() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_FindDouble() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_SetFromInt() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_FindInt() to "hidden struct" interface.
* sources] Ifdef oyOptions_DoFilter() to hide until elektra gets imported
* review] [sources] Adopt oyOptions_SetFromText() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_FindString^C to "hidden struct" interface.
* sources] Fix compile errors
* templates] Add oyranos_sentinel.h to Options_s.c
* templates] Add the private include file of the content
* sources] Ifdef oyOptions_ForFilter() to only be used when oyFilterCore is imported.
* templates] Fix compile errors: use private structures
* review] [sources] Adopt oyOptions_Find() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_CountType() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_GetText() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_Filter() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_DoFilter() to "hidden struct" interface.
* review] [sources] Adopt oyOptions_CopyFrom() to "hidden struct" interface.
* templates] Add needed #include
* sources] Fix variable name compile error
* templates] Fix compile error
* sources] Ifdef oyOptions_ForFilter_ to only be used when oyFilterCore is imported.
* Add proper support for libxml in the auto-generated code
* review] [sources] Adopt oyOptions_Set() to "hidden struct" interface.
* templates] Add missing #include to oyOptions_s.h
* templates] Add filter registration declarations to oyranos_object.h
* review] [sources] Adopt oyOptions_Add() to "hidden struct" interface.
* templates] Add a new oyClassPriv_m( var ) macro
* templates] Add oyBOOLEAN_e to oyranos_object.h
* review] [sources] Implement the copy constructor for oyOption_s
* review] [sources] Adopt oyOptions_ParseXML_() to "hidden struct" interface.
* sources] Import forgotten oyOptions_ParseXML_()
* review] [sources] Adopt oyOptions_ForFilter_() to "hidden struct" interface.
* templates] Update templates to new underscoreName variable
* generator] Add a new underscoreName variable to templates
* sources] FIX: oyOptions_ForFilter_() sliped in twice!
* doc] Update TODO
* sources] Import private methods for oyOptions_s
* build] Print total lines of code
* doc] Update TODO
* review] [sources] Adopt oyOptions_FromBoolean() to "hidden struct" interface.
* sources] Import public methods for oyOptions_s
* sources] Add #defines to oyOptions_s.h
* templates] Fix typo for previous commit
* templates] Add the Observer part in oyOptions_MoveIn()
* templates] Add support for overriding parts of oyClass_MoveIn()
* templates] Add missing #include file
* templates] Better support for adding custom #include
* Add missing #include  and make Option_s templates hand-edited
* doc] Update TODO
* tempaltes] Import oyOPTIONATTRIBUTE_e enum
* sources] Add missing cast
* templates] Replace old oyOption_New constructor
* templates] Fix typo, public not private struct
* templates] Fix to use private oyOption_s_
* review] [sources] Adopt oyOption_StructGet() to "hidden struct" interface.
* review] [sources] Adopt oyOption_StructMoveIn() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetValueFromDB() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetRegistration() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetRegistration() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetData() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetFromData() to "hidden struct" interface.
* review] [sources] Adopt oyOption_Clear() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetValueDouble() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetFromDouble() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetValueInt() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetFromInt() to "hidden struct" interface.
* sources] Fix wrong input pointer
* review] [sources] Adopt oyOption_GetValueText() to "hidden struct" interface.
* review] [sources] Adopt oyOption_SetFromText() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetText() to "hidden struct" interface.
* review] [sources] Adopt oyOption_GetId() to "hidden struct" interface.
* Update TODO
* review] [sources] Adopt oyOption_FromDB() to "hidden struct" interface.
* review] [sources] oyOption_New(const char*registration,oyObject_s object)
* doc] Update TODO
* sources] Remove old custom (copy)constructor and destructor
* sources] Remove oyOption_FromStatic_(). It is deprecated
* sources] Remove spaces from end of line
* sources] Import public methods for oyOption_s
* review] [sources] Adopt oyOption_s private methods to "hidden struct" interface.
* sources] Import private methods for oyOption_s
* review] [sources] Implement the destructor for oyOption_s
* review] [sources] Implement the copy constructor for oyOption_s
* review] [sources] Implement the copy constructor for oyOption_s
* sources] Add oyValue_u definitions in oyOption.h
* Create skeleton files for oyOption_s
* sources] Add missing oyStructList_Create() function
* templates] Add proper #include files
* review] [sources] Fix to use the private structs
* templates] Adopt templates to new class.enumName
* sources] Add [notemplates] tag to StructList.dox
* review] [sources] Fix typo parent_type -> parent_type_
* generator] Add a new property enumName to grantlee
* sources] Import the member functions of oyStructList_s
* review] [sources] Implement the destructor for oyStructList_s
* review] [sources] Implement the copy constructor for oyStructList_s
* templates] Catch error from copy constructor
* Create skeleton files oyStructList_s
* review] [templates] The oyObject_Copy_() function is
* Remove the API_proposed directory (unused)
* generator] Check first  if the template is readable
* generator] Check if template files can be opened.
* build] Compile auto-generated source by default
* FIXME] [templates] Temporary workaround for oyOBJECT_STRUCT_LIST_S
* review] [templates] Use hidden the structures to get private members.
* Add the oyObserver_s class to the objects_generic templates.
* review] [templates] Move the OY_HASH_SIZE definition to Object_s.h
* sources] Emptying sources/Object.members.h (unused)
* templates] Add the math.h for pow().
* FIXME] Temporary workarount for not yet existent oyOBJECT_NAMED_COLOURS_S.
* review] [sources] Move the locking functions to the oyStruct_s files.
* templates] Include the oyranos_texts.h file in oyranos_object_internal.h
* templates] Add #includes for compiling oyObject_s
* sources] Remove pointer from oyObject_s
* templates] Add missing #includes
* sources] Remove pointer from oyObject_s, as its
* Remove the extends keyword from objects_generic classes.
* Remove the oyName_s class from object system
* Fix typo: "to" -> "dst"
* templates] Adding a new template file.
* sources] Fix errors preventing compilation
* sources] Put the oyranos message functions declarations
* templates] Add some include files for correct compilation
* templates] Add a cast
* templates] Include the definition of oyObject_s
* templates] Introduce a new header file
* doc] Show the cyclic comile time dependency of classes.
* tempaltes] Break #include compilation cycle
* templates] A "list" should include the container class definition
* generator] Auto-generated template files are read-only.
* templates] Fix some typos, like name -> privName
* doc] Add an oyStruct_s dependency diagram
* Update README
* templates] Fix to use the new {{ class.content }}
* generator] Add a new member to ClassInfo for "list" classes
* templates] Fix a few small errors and better ...
* sources] Import the oyOptions_s class, part1.
* templates] Add support for "list" classes
* generator] Add support for "list" classes
* templates] Include oyStruct_s.h & oyObject_s.h
* source] First proper import of the Name class
* Add proper support for custom object members manipulation
* templates] Adapt the template files to work with previous commit.
* Add the oyObject_s class
* tempaltes] Rreplace all occurences of "obj" variable
* templates] Fix oyTest* templates to print correctly
* Temporary workaround for templates not exporting
* templates] Put the base members in their own block
* generator] Add support to create classes from different templates
* templates] Fix wrong baseName -> name
* ClassTemplates] Split createTemplates() to createSources()
* Track the TODO file
* templates] Rearrange all templates to fit previous commit
* templates] Reorganise to use comment guards
* ClassTemplates] Turn getAllClasses() into a const function
* ClassTemplates] Declare sourceFiles as const
* generator] Support a [notemplates] tag in <class>.doxy files
* generator] Add support for class.parent variable in grantlee
* templates] Remove _s suffix from Struct templates
* templates] Keep only needed stuff from oyranos_alpha.h
* templates] Include the headers from oyranos source tree
* Move some typedefs from oyranos_alpha.h to Struct_s.public.h
* generator] Add some more properties to ClassInfo
* generator] Add ClassInfo::parseSourceFiles()
* extract] Update to new oyAPIGenerator arguments
* templates] Add an CMakeLists.txt file
* generator] Treat template files differently.
* cmake] Fix some errors to build the auto-generated tests
* generator] Update globbing to ignore new opaque Class templates
* templates] Add the generic templates needed for previous commit
* templates] Add support for classes with opaque pointer interface
* templates] Update to use class.privName
* main.cc] Fix 3-space indentation to 2-space
* generator] Add error handling for I/O directories
* generator] Load all template subdirectories automaticly
* generator] Have hardcoded default I/O directories
* templates] Create a template directory for each object group
* generator] Put each new template inside templates/<group>/
* generator] Recurse into templates/ dir for *template* files
* cmake] Build the auto-generated QtTests
* templates] Add an automatic test using QtTest
* cmake] Do not build tests by default
* generator] Allow template files with .cc extension
* templates] Add a new oyranos_object.h file
* templates] The generated <class>_s.[ch] files are read-only
* templates] Add FIXME comments for clarification
* templates] BUG: Remove members start/stop comment
* templates] Add a missing declaration of *_Copy\_\_()
* templates] Fix a missing variable
* Add a command line programm to test the Qt regexps
* grantlee] Add a class.brief tag
* templates] Also print the parent child members
* templates] Fix Base_s_* to use class.baseName
* Name] Add the doxygen doc
* Templates] Add missing ""
* oyStruct_s] Create correctly the public functions
* Use Struct_s.members.h for oyStruct_s members
* Create a dummy class for oyStruct_s use whithin templates
* Add the doxygen documentation to public Class header
* Fix: Use a QRegExp, else all Class instances get replaced
* Depricate the class_name and class_base_name
* Correctly replace the included Base* file
* Add a convenievce string
* Insert the current class + all classes information
* Add a getAllClasses() member to ClassTemplates
* Fix a compile error:
* Remove: findClasses(), allClasses, doxOnlyClasses
* Add a static getAllClasses method
* Add a doxOnly property to ClassInfo
* Use list of ClassInfo instead of plain QStringList
* Update .gitignore
* Add missing parenthesis in qWarning
* Fix typo in cmake
* Backslashes need to be escaped in regexps
* Fix typos preventing compile
* Setup cmake for Qt moc files
* Add some qDebug messages
* Add missing directory names to filename strings
* Remove "*.dox" from automaticly created source files
* Remove Class.dox from detected classes
* Create a new ClassInfo class
* Create a new ClassTemplates class.
* Class.dox is the basic template for creating a new Class
* Rename Class_s*.template.[ch] -> Class_s*.[ch]
* Add .gitignore file
* Add generic Class_s* templates
* Add oyName_s in sources/
* Add sources/ as template dir
* Fix #include files
* Add a new template class: Name
* Add class doxygen comments
* correct typo /nase/base/
* Fix typos
* StructList will be created from BaseList
* The object system now defaults to the following structure.
* Add proposed oyObject
* Add proposed oyObserver
* Add proposed oyName
* Add proposed oyStruct
* Separate the actual code from template code
* The process of code creation is split in 2 stages
* Add oyStructList
* Base.* is the base of all structures
* Include header file without _s
* Rename header.txt to source_file_header.txt
* New template sceme using hidden structures
* Add file_name variable to the Grantlee::Context
* Update README
* First try to make the generator more clever.
* Add a new ClassMethods block & fix indentation
* Hard coded plugin path is a bug!
* Add a CMMapi7 test template
* Add a README
* Move for loop from shell script to main.cc
* Initial commit

#Version 0.2.0

###Kai-Uwe Behrmann (184):
* [conf]: bump to 0.2.0
* [core]: support cinema and Rgb print proofing
* [docu]: move OpenICC-data to reuirements section
* [docu]: update ChangeLog
* [API]: bump API due to removed key in #45b7ec2f
* [oyX1]: require Xcm
* [docu]: reverse new Changelog entries
* [core]: differenciate user data and config paths
* [oyX1]: add -d option to oyranos-monitor
* [docu]: update file header
* [build]: package missed source files
* [docu]: update oyranos-monitor usage
* [oyX1]: write to stdout in oyranos-monitor -f
* [build]: install autostart script
* [oyX1]: be more careful with pointers
* [core]: omit ~ for path name
* [exmpl]: support profile filtering
* [core]: preserve OY_PROFILE_PATH_USER_DEFAULT
* [conf]: switch from ~/.color to ~/.config/color
* [build]: oyranos-profiles
* [build]: Fedoras FLTK forgot about pthread - add
* [i18n]: pick ICC Examin fix for osX locale
* [oforms]: add missed header under osX
* [build]: install basic oforms tools
* [oforms]: sync FLTK with cmd line options
* [oforms]: cmd line + -O and - -x option
* [oforms]: for args pick options from -i and modify
* [oforms]: collect options in the cmd line parser
* [oforms]: fix some crashes in oyranos-xforms
* [alpha]: fix crashes in oyOption_GetText()
* [oforms]: skip &nbsp
* [oform]: use oyranos-config-fltk i18n init
* [oforms]: convert UTF-8 XML to FLTK locale
* [oforms]: switch to plastic for fltk
* [oyX1]: switch rank_map partitialy to old keys
* [oyX1]: require "EDID_" prefix for meta tag
* [alpha]: add option to oyProfile_DeviceAdd()
* [oyX1]: duplicate core keys for Oyranos
* [devices]: "icc_profile_class" and "key_prefix"
* [lcms]: support DL in image conversions
* [lcms]: add more debuging informations
* [alpha]: clear modified profile informations
* [lcms]: check pointer usage
* [oicc]: omit outdated warning
* [alpha]: correct error in oyConversion_Correct()
* [lcms]: fix option names "LCMS2"->"LCMS"
* [oyX1]: reduce registration key generation calls
* [oyX1]: match XRandR naming the Xinerama one
* [oyX1]: parse ATI Xorg log for multiple devices
* [core]: add one more warning macro
* [oyX1]: check Xcm return value
* [oyra]: lowercase file name extension matching
* [lraw]: add .dng extension
* [build]: add oyranos-xfoms-module in oforms/
* [oforms]: add module options -> XFORMS tool
* [oforms]: remove node dependency
* [build]: omit optimisation in oforms from FLTK
* [oforms]: sync with CLI with FLTK arguments #1
* [oforms]: render choice labels in CLI renderer
* [oforms]: omit unused arg in oyranos-xforms-fltk
* [i18n]: update deutsch
* [oforms]: add -l -h -f opions to CLI renderer
* [oforms]: document new member variable
* [oforms]: change silent into print element
* [oforms]: one stage parsing in CLI renderer
* [alpha]: correct pattern matching
* [oicc]: specify pattern to "icc"
* [i18n]: update deutsch
* [lraw]: add XForms layout and fixes
* [core]: add debug warnings for string funcions
* [alpha]: check for unsymetric registrations
* [exmpl: add oyranos-profile
* [core]: support OpenIccDirectoryProposal 0.2
* [docu]: list Fedora build requirements
* [build]: make Fedora happy with -lpthread
* [alpha]: warn for no meta module
* [core]: check fixed paths before searching in
* [test]: omit oyX1 on Apple
* [qarz]: fix compile warnings
* [oyX1]: ICC meta Tag for Monitor Profiles - D3
* [core]: fix compile warning
* [oyRE]: omit sane include
* [oyX1]: correct fallback profile name
* [oyX1]: use "C" locale for colorimetric parsing
* [oyX1]: add device infos, even not from EDID
* [oyX1]: add missed EDID workaround
* [oyX1]: correct EDID size check
* [lcm2]: correct debugging ICC profile name
* [lcm2]: add flags debugging informations
* [oicc]: clear "front" attribute exception
* [alpha]: add oyConversion_Correct() sees flags
* [API]: remove OY_DEFAULT_RENDERING_HIGH_PRECISSION
* [alpha]: better oyOptions_Add option comparision
* [lcm2]: keep sequence in abstract profiles
* [lcm2]: precalculation replaces rendering_high_precission
* [oicc]: remove common rendering_high_precission
* [build]: add FLTK_H to certain oforms files
* [alpha]: error message if no OpenICC-data
* [docu]: require OpenICC data package
* [oyRE]: add missed sources
* [lraw]: switch to linear, 16-bit output
* [build]: add oyRE
* [oyRE]: integrate Yiannis module
* [conf]: detect exiv2
* [alpha]: fix oyFilterRegistrationMatch() _-
* [test]: a few test cases for oyTextIccDictMatch
* [alpha]: support ICC Dict style string matching
* [alpha]: add oyTextIccDictMatch ()
* [core]: fix string to number conversions #926c96
* [alpha]: fix device profile resolving #670c23
* [core]: string to number convenience converters
* [docu]: add inline comments
* [core]: add posix dirent API wrapper
* [core]: still need unistd.h
* [core]: fix local header conflict
* [cmms]: fix oyCMMInit_f declarations
* [alpha]: move oyStructList_AddName() convenience
* [cmms]: switch message var to individual name
* [core]: mark posix calls and includes
* [core]: move basic code out of oyranos_helper.h
* [core]: split string from text APIs
* [oyX1]: fix missed _ICC_PROFILE_IN_X_VERSION
* [oyX1]: setup is working again
* [alpha]: fix typo
* [oyX1]: add common funcion
* [docu]: update dependencies
* [oyX1]: use oyGetDisplayNameFromPosition2()
* [core]: deprecate oyGetDisplayNameFromPosition()
* [core]: add i18n setup messages
* [qarz]: correct some messages and names
* [core]: fix wrong profile warning on osX
* [docu]: update PACKAGING file
* [oyX1]: add --module(s) to oyranos-monitor
* [qarz]: omit message of non found monitors
* [qarz]: renaming of oyMonitor structure
* [qarz]: separate function names from oyX1
* [core]: avoid custom string allocation
* [core]: avoid MAX_PATH in a few places
* [oyX1]: fix zero memory access
* [oyX1]: fix wrong string access
* [build]: add new target
* [qarz]: sRGB for monitors on osX > 10.6.0
* [core]: add oyOSxVersionAtRuntime()
* [qarz]: fix compile warning
* [oyX1]: split CS parts from oyX1 into qarz
* [conf]: detect Xcm even without X11
* [i18n]: omit global cflags
* [exmpl]: add global make flags
* [core]: more osX fixes
* [alpha]: build fl_i18n with osX
* [alpha]: separate CS code from X11 module
* [alpha]: implicite device profile by meta tag
* [alpha]: link duplicate tag contents
* [docu]: add libxpm-dev dependency in README
* [docu]: libxmu-dev is a debian build dependency
* [oyX1]: check edid object access
* [core]: see local files in oyFindProfile_()
* [core]: fix write simulation profile to XML
* [core]: use STRING_ADD in elder cmm code
* [core]: sort policy list
* [oyX1]: add EDID -> ICC meta tag embedding
* [oyX1]: add custom "add-edid-meta-to-icc" command
* [alpha]: fix wrong struct access in options
* [oyX1]: add the --device-meta-tag option
* [oyX1]: split out oyX1DeviceFillEdid()
* [core]: extent internal iconv wrapper
* [alpha]: correct padding of meta tag records
* [alpha]: reduce development messages
* [alpha]: fix wrong memory access
* [alpha]: switch oyProfile_DeviceGet to 'meta' tag
* [docu]: fix missed monitor API
* [alpha]: move oyProfile_DeviceAdd to public APIs
* [alpha]: add device to a oyProfile_s object
* [core]: allocate maximum possible character size
* [core]: add internal oyIconvGet
* [lraw]: add more file types endings
* [core]: add ICC dict tag data structure
* [alpha]: fix signature setting
* [lcm2]: fix threading
* [lraw]: update reader properties
* [oydi]: explicite printf %d int
* [conf]: sync script with CompIcc
* [conf]: bump version
* [docu]: update ChangeLog

###Yiannis Belias (1):
* [lraw]: add device query from RAW.cc

#Version 0.1.12

###Kai-Uwe Behrmann (24):
* [exmpl]: add window title to image_display
* [core]: add clrt and clro tag types
* [oyIM]: read 'clro' tag type
* [conf]: bump version to 0.1.12
* [oyIM]: correct count value for 'clro'
* [oyIM]: complete ICC 'clrt' tag type
* [core]: add icSigColorantTableOutType
* [docu]: adjust colour channel tag names
* [conf]: explicite disable internal Elektra shared
* [exmpl]: remove xcmsevents
* [core]: remove EDID and X CM Events API
* [oyX1]: remove EDID parse example code
* [oyIM]: variable translations
* [oyIM]: modify oyStringFrommluc()
* [docu]: remove authors from removed data
* [exmpl]: add qcmsevents desktop file
* [build]: package image_display files
* [build]: own the settings dir in RPM spec
* [docu]: add detail about OY_MODULE_PATHS variable
* [build]: add libXmu to RPM spec
* [build]: add mandriva BuildRequires for RPM
* [conf]: fix typo in desktop file
* [build]: add suse_update_desktop_file RPM macro
* [oyX1]: new "edid":"refresh" option with "list"

#Version 0.1.11

###Kai-Uwe Behrmann (127):
* [SANE]: Add missing function prototypes and %s in printf
* [oyRE]: Change the output format of camera devices
* [oyRE]: Better formatting when printing devices_info
* [scan]: Add support for getting the profile from DB
* [oyRE]: Adopt to api8 changes
* [conf]: bump version
* [conf]: check src dir config.h and remote build
* [build]: change link order in pkg-config --libs
* [alpha]: add oyProfileTag_GetBlock()
* [build]: make xdg optional for installation
* [build]: fix install shell script
* [build]: install mime script
* [build]: give message for non xdg-mime
* [oyra]: and CUPS remove unused variables
* [alpha]: more checks in oyFilterNode_ContextSet_
* [test]: remove direct lcms calls
* [oyIM]: fix copy of empty string for psid tag
* [alpha]: give error oyNamedColour_SetColourStd
* [alpha]: prefere similiar version modules
* [test]: give up on error
* [alpha]: fix memory in oyProfile_GetFileName()
* [oyIM]: spot non mluc inside psid tag
* [alpha]: accept zero in oyProfileTag_GetText
* [alpha]: suspected typo correction
* [alpha]: add oyProfileTag_s::size_check_
* [oyIM]: fix psid and pseq tag reading
* [lcm2]: initial littleCMS 2 module
* [exmpl]: hotplug for compiz
* [docu]: update version infos
* [alpha]: skip profile release
* [test]: place some checks
* [lcm2]: omit colour space check
* [test]: remove old lcms results
* [test]: clean up
* [alpha]: adjust module removal
* [oydi]: update macros to libXcm
* [alpha]: tweak module selection
* [modules]: CMM_NICK is mostly implementation detail
* [alpha]: add oyFilterRegistrationModify()
* [oyra]: fix parameter exchange
* [lcm2]: add some error checks
* [lcm2]: support oyFLOAT
* [oydi]: turn warnings into debug messages
* [alpha]: omit api_reg memory warning
* [oyra]: fix float PPM loading
* [alpha]: add oyStruct_GetId()
* [core]: show oyOption_s in oyMessageFunc_
* [lcm2]: use THR profile open
* [core]: show oyProfile_s in oyMessageFunc_
* [lcm2]: mo OpenMP cmsCreateProofingTransformTHR
* [oyIM]: remove debug printf
* [build]: switch to XCM_H make variable
* [build]: remove external Elektra dependency
* [core]: avoid thread issues during messaging
* [conf]: add --disable-fltk configure options
* [ui-fltk]: fix profile listing
* [core]: fix multiple string listing
* [ui-fltk]: update during refreshOptions() only
* [API]: sync oyWIDGET_e with oyBEHAVIOUR_e
* [exmpl]: move colour_desktop to CompIcc
* [build]: make lcms2 optional
* [exmpl]: fix icons for qcmsevents
* [exmpl]: fix typo
* [docu]: remove externalised authors
* [build]: remove more colour_desktop references
* [exmpl]: initialise xcmsevents memory
* [build]: remove ICC profiles
* [build]: install icons
* [CUPS]: change to ColorKeyWords[x]
* [build]: add git ignore file
* [SANE]: remove currently unused bits
* [SANE]: place in tree
* [build]: build SANE module
* [SANE]: fix includes
* [build]: add SANE package to spec file
* [docu]: clearify oyNamedColour_Creates ref arg
* [oyIM]: correct mmod and add ndin
* [docu]: staigthen on mini date
* [exmpl]: add missed icon for qcmsevents
* [exmpl]: allow non i18n build in image_display
* [build]: --disable-fltk left overs
* [docu]: update image_display date
* [SANE]: clean up code
* [alpha]: check arguments in signal setter
* [exmpl]: clean Fl_Oy_Box::setImage
* [exmpl]: add graphFromImageFileName to image_display
* [exmpl]: further split FLTK and Oyranos code
* [docu]: fix typo
* [oPNG]: initial commit
* [oPNG]: add modules/oyranos_cmm_oPNG.c
* [build]: link oPNG with libpng
* [docu]: more explicite oyArray2d_s::array2d ownership
* [alpha]: add oyArray2d_RowsSet()
* [oPNG]: basic loading of RGB images
* [oPNG]: support inlaced PNGs
* [alpha]: fix recent introduced allocation
* [docu]: inform about ICC assignment for PPM
* [oPNG]: default to oyASSUMED_GRAY for grayscale
* [oydi]: remove unneeded warning
* [core]: default to Gray-CIE_L.icc
* [build]: fix installation of oyranos-config-fltk
* [build]: add oPNG to RPM
* [exmpl]: add oyObject_s arg to oyGraphFromImageFileName
* [exmpl]: adapt image_display to helper changes
* [exmpl]: rename to oyConversion_FromImageFileName
* [exmpl]: rename to oyDrawScreenImage()
* [exmpl]: code reorganisation in image_display
* [exmpl]: rename Fl_Oy_Box to Oy_Fl_Box
* [build]: decouple uninstall from install target
* [build]: link lcms to SANE module
* [conf]: add --icondir
* [build]: make check without installation
* [conf]: fix icondir typo and default install path
* [docu]: describe OY_MODULE_PATHS
* [build]: support make check without install
* [build]: omit oyranos_xml_to_po.sh
* [build]: remove compiz RPM build requirement
* [exmpl]: split files in image_display
* [exmpl]: add oyranos_display_helper_classes_fltk.cpp
* [build]: sync infos with examples sub dir
* [oPNG]: add oPNG_ICC shell variable
* [alpha]: expand oyColourConvert_ for n pixels
* [oPN]: extract a oyImage_FromPNG()
* [alpha]: add oyCMMsGetFilterApis_() meta select
* [build]: detect YAJL
* [alpha]: remove unused internal function
* [alpha]: fix DSO caching

###Pino Toscano (1):
* [build]: fix make check

###Yiannis Belias (216):
* first commit
* add gitignore file
* add file save to tiff support
* Add support for ICC profile embedding in tiff
* Ignore tiff images
* Ignore scan executable
* Add scan.c to cmake
* Add a reference implementation program for SANE
* Ignore read() error when the full image is downloaded
* Initial import of SANE & raw/exif backends
* Just a program to test the provided Oyranos backends
* update .gitignore
* cmake support for install & build the backends
* Rank map for exif/libraw
* Implement the SANEGetDevices function
* Function that creates a rank map for SANE options
* edit SANE rank map
* edit GetText functions
* rename .c RAW backend to .cc
* ... and adapt cmake
* ... and adapt oyranos_cmm_oyRE.cc to compile with c++
* update libraw rank map options
* Initial LibRaw support
* hello world for SANE backend
* cmake: link SANE backend with libsane
* cmake: add outest executable
* cmake: add oytest executable: final
* oytest will perform tests for the backends
* SANE: InstrumentFromName_(): first implementation
* oyRE: Fix "deprecated conversion from string constant to char*" warning
* oyCreateRankMap_() function now creates the final rank map array
* [sane/oyRE]: convert instrument to device name
* [oytest]: convert test application to C++
* [SANE/oyRE]: GetDevices() returns the number of devices
* Add usefull comments to oyConfigsFromPattern()
* [SANE]: Trick to make the pnm backend appear
* [oytest]: Add test_backend() function
* [SANE]: GetDevices() returns list of names,vendors,models,types.
* [cmake]: link oyRE with libexiv2
* [SANE]: Implement the "properties" call:
* [SANE]: Remove 'hello world' message
* [oytest]: Define DBG macro, sometimes usefull...
* [online help]: Add support for backend usage help
* [cmake]: Link the RAW/EXIF backend with LibRaw-Lite
* [online help]: add the *_help.c files
* [online help]: ... and not ignore them :)
* initial camera list support
* [oytest]: Select tests from cmdline
* [help]: Rearange *help.c files
* [SANE]: Add couple TODO comments
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: Split up and rewrite GetDevices()
* [SANE]: Rearange Configs_FromPattern()
* [oytest]: Check if no devices were found
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: Rearange Configs_FromPattern()
* [oytest]: Create a convinience function
* [oytest]: Add a few helper functions
* [SANE]: Rearange Configs_FromPattern()
* [oytest]: New helper func & correct registration
* [oytest]: Update tests 2,3 to use the new functions
* [indent]: Use GNU indent to correct all source files
* [SANE]: Rearange Configs_FromPattern()
* [SANE]: device_context support to properties call
* [SANE]: Correct the usage of driver_version
* [SANE]: Complete previous commit
* [SANE]: Replace DeviceFromName_ with DeviceFromContext_
* [SANE]: Finalize "properties" section.
* [indent]: Correct the rank map initializations
* [online help]: Handle elinks not present case.
* [oyRE]: Rearrange ConfigsFromPattern(), list section
* [oyRE]: Rearrange ConfigsFromPattern(), part II
* [oyRE]: Rewrite DeviceFromContext()
* [oyRE]: Rearrange device_handle section
* [oyRE]: "device_handle" proper support
* [oyRE]: Implement is_raw() helper function
* [oyRE]: Correct numerous small compiler errors
* [oyRE]: Copy rank map in properties section, too.
* [oyRE]: Remove unused GetDevices() function
* [online help]: Update the *_help.html files
* [oyRE/SANE]: Update the SetFromDouble() calls.
* [oyRE]: Fix segfault. Test for auto_ptr value, too.
* [scan]: Import to Oyranos. init() converted first.
* [scan]: Fix some compiler errors.
* [scan]: Update main().
* [scan]: Remove select_device().
* [scan]: Get the device hanlde from init().
* [scan]: Convert scan_it() to Oyranos. Also,
* [raw2tiff]: Print the version number.
* [oytest]: Correct missing %s in printf.
* [scan]: Correct bugs, add debug output
* [scan]: Change the previous options to int
* [SANE]: Fix bug where only the first device was found.
* [SANE]: Housekeeping.
* [scan]: Fix missing "properties" command,
* [scan]: Fix not initialised sane handle bug.
* [SANE]: Fix error message.
* [SANE]: Print proper SANE error messages.
* [scan]: Get the SANE parameters before sane_start()
* [SANE]: Fix unsigned int underflow bug.
* [helper.c]: Add a file with some helper functions.
* [sane patch]: This is a patch to apply to the..
* [README]: Update README file
* [cmake]: Remove harcoded prefix & unused LibRaw vars
* [raw2tiff]: Align the class RAW members
* [helper]: Add namespace for C++ use.
* [raw2tiff]: Add oyranos support.
* [oyRE]: Fix: Remove 2 unwanted calls returning error
* [RAW]: RAW::GetColorInfo() rearrange a few lines.
* [oyRE]: Fix for value increment in macro argument
* [oyRE]: Correct DFC_OPT_ADD_FLOAT_ARR macro.
* [shiva]: Add 3 simple shiva filters
* [cmake]: Now modules install in lib/colour/modules/
* [SANE]: Fix negtive index integer bug
* [oyRE]: Uncomment the now working double list macros
* [README]: Update pnm install instructions.
* [SANE]: Configs_FromPattern(): Always "return 0"
* [shiva]: Add 2 new filters
* [cmake]: Link both backends with liboyranos
* [raw2tiff]: Save a 16bit per channel image.
* [doc]: Update LibRaw instructions in README
* [helper]: Support printing of more oyranos structs:
* [SANE]: Replace blob with CMMptr in device handle.
* [helper]: Update output formatting in print_option()
* [scan]: Add some assertions.
* [scan]: Fix missing OY_SLASH
* [helper]: Use a custom version of oyValueTypeText()
* [helper]: Correct printing of CMMptr handle.
* [scan]: Fix to use the right pointer.
* [SANE]: Change sane_handle from blob to CMMptr.
* [scan]: Fix compiler warning
* [SANE]: Update the global driver_version handling.
* [SANE]: Add #include for previous commit
* [SANE]: Add a CMM name  prefix to printfs
* [SANE]: Fix endless loop bug.
* [SANE]: Print out a message at sane_exit()
* Revert "[scan] Add support for getting the profile from DB"
* [SANE/oyRE]: Update to Oyranos API addition
* [README]: Add tip for faster SANE startup
* [oyRE]: Add support for more camera properties.
* [raw2tiff]: Print dcraw options to dcraw.txt
* [raw2tiff]: Add support for getting the profile
* [oyre/SANE]: Update to latest Oyranos API changes
* Update author/copyrights
* [SANE]: Update a comment
* [SANE]: Rearrange for Configs_Modify #1
* [SANE]: Integrate check_driver_version() to Configs_Modify
* [SANE]: Rearrange for Configs_Modify #2
* [SANE]: Rearrange for Configs_Modify #2
* [SANE]: Deallocate device
* [SANE]: Rearrange for Configs_Modify #4
* [SANE]: Add g_error variable
* [SANE]: Comment cleanup
* [SANE]: Fix compile errors
* [SANE]: Check if device_context exists
* [SANE]: Fix missing if(..) crash
* [SANE]: Add Enter/Leave Message
* [SANE]: Update Configs_FromPattern #1
* [SCAN]: driver_version is now in backend_core
* [gitignore]: Ignore ppm files
* [SANE]: Update Configs_FromPattern #2
* [SANE]: Check for device_context=NULL case
* [SANE]: Do not return device_context/device_handle
* [SANE]: Add options to info output
* [SANE]: Do not return the driver_version option
* [SANE]: Add some informative messages
* [SANE]: Get device_name from device_name option
* [SANE]: inverse if/else
* [SANE]: device_handle should be in ->data
* [SANE]: Better handling of device_name in Configs_Modify
* [SANE]: Remove error variable
* [SANE]: Correctly alloc/dealloc for device_name
* [SANE]: tmp becomes NULL after oyOptions_MoveIn
* [SANE]: *_opt_dev become NULL after oyOptions_MoveIn
* [SANE]: Be nice and close not needed SANE devices
* [SANE]: Adopt to api8 changes
* [Fix]: Add missing closing brace
* [SANE]: Fix "strcmp() == 0" checks
* [oyRE]: Fix "strcmp() == 0" checks
* [SANE]: Create new dir for sane patches
* [SANE]: Replace gamma tables with a gamma value
* [SANE]: Remove unused variable
* [SANE]: Patch for the plustek SANE backend
* [SANE/oyRE]: Update CMM_BASE_REG
* [scan]: deallocate devices
* [SANE]: Replace gamma tables with a gamma value
* [cmake]: Copy FindLCMS.cmake from kdelibs trunk
* [cmake]: search for lcms and abort if not found
* [scan]: Add a printout only option
* [SANE]: Do not filter out inactive sane options for now
* [SANE]: Save all SANE options as strings.
* [SANE]: Fix: device_handle option goes to ::data
* [SANE]: Fix: oyNAME_NAME option goes to ::backend_core
* [oyRE]: Add "error handling" to Configs_Modify
* [oyRE]: Add "help section" to Configs_Modify
* [oyRE]: init driver_version for Configs_Modify
* [SANE]: Remove context/handle from Configs_Modify
* [oyRE]: Configs_Modify(): Scan input
* [oyRE]: Configs_Modify(): Add "command_list" section
* [oyRE]: Configs_Modify(): Add "command_properties" section
* [oyRE]: Add printfs for debugging
* [oyRE]: Update DeviceFromHandle()
* [oyRE]: device_context holds a pointer, not the libraw object
* [oyRE]: Add a DeviceFromHandle_opt() function
* [oyRE]: Fix: a device_name is always created by Configs_FromPattern()
* [oyRE]: Remove device_handle from the top of...
* [SANE]: Shorten prefix for error messages
* [RAW]: 3 changes:
* [cmake]: Update support for LibRaw-0.9.0
* Update README instructions for previous commit
* [oyRE]: gamma_16bit field has been removed
* [oyRE]: Enable filtering_mode, which is now in LibRaw
* [cmake]: Remove debug message


#Version 0.1.10

###Joe (5):
* [CUPS]: add GSoC2009 device backend
* [CUPS]: access to profiles on a server
* [CUPS]: fix of profile_name option
* [CUPS]: add local installation of a server profile
* [CUPS]: compile with new oyCMMapi8_s

###Kai-Uwe Behrmann (1304):
* [conf]: add option --disable-static
* [build]: set SRCDIR to ./ by default
* [conf]: switch to v0.1.10
* [build]: on more conservative systems
* [build]: next try for check target on Debian
* [docu]: update dependency list
* [docu]: add missed license file
* [build]: remove liboyranos_moni.so
* [fix]: oyranos-monitor profile.icc not complete bug
* [build]: sort dependencies
* [alpha]: split out internal alpha functions
* [docu]: formatting
* [and]: clearer expression same logic
* [alpha]: avoid sprintf on stack memory
* [alpha]: more work on oyCMMapi5_s
* [alpha]: fix crash for wrong options
* [alpha]: fix non oyCMMapi4_s library version selecion
* [conf]: let configure not fail with the --quick option
* [build]: RPM build requirements
* build]: prepare oyra filter from meta backend separation
* [alpha]: insert CMMapi dummys for 6 and 7
* [alpha]: spit oyCMMsGetApi4_ from oyCMMsGetApi_
* [build]: RPM: better library and devel separation, add ldconfig
* [alpha]: simplify oyCMMsGetApi4_() and integrate with oyCMMsGetApi_()
* [alpha]: remove oyFILTER_TYPE_e and work with registration directly
* [fix] oyLibPathsGet_() for selecting paths according to ithe subdir arg
* [API]: add OY_LOCALEDIR environment variable
* [alpha]: put the lcms module into the icc registration name space
* [alpha]: add id_ member to oyCMMapi[4,5]_s
* [alpha]: minor cleanup around the internal CMM APIs
* [alpha]: objectify oyCMMInfo_s and remove oyFilter_s::lib_name_
* [alpha]: complete oyCMMapi5_s Oyranos side APIs
* [alpha]: add "oyIM" meta module
* [conf]: add --cmmsubpath switch
* [build]: build and install modules in two groups
* [alpha]: move the static APIs from oyra to oyIM
* [build]: add oyranos_alpha_internal.h and oyranos_cmm_oyIM.h
* [conf]: sort modules into new paths
* [alpha]: add oyCMMInfo_s::oyCMMInfoGetText_f
* [alpha]: change oyCMMsGetApi\_\_ to support API iterating
* [alpha]: substitute "generic" with "imaging"
* [alpha]: last "generic" to "imaging" usage keyword change
* [fix]: move function definitions out of header files
* [alpha]: work on oyCMMapi4-7_s
* [alpha]: add oyCMMptr_LookUp(); remove unwanted members from oyCMMapi4_s
* [alpha]: oyCMMapi4_s::oyCMMFilterNode_GetText() is optional
* [alpha]: rename oyCONNECTOR_EVENT_es to abstract from data types
* [lcms]: CMM side cache ordering, add effect and simulation options
* [alpha]: update to backend API changes
* [alpha]: rename oyStruct_TypeToText() to oyStructTypeToText()
* [alpha]: change various Filter api fields
* [alpha]: oyStruct_TypeToText -> deprecated header section
* [docu]: move around of sources to better group inside the documentation
* [docu]: split CMM into backend and frontend parts
* [API]: change OY_MODULE_NICK to "oyIM"
* [lcms]: add psid and Info tags
* [alpha]: add oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT to backend events
* [lcms]: switch lcmsFilterPlug_CmmIccRun to cmsDoTransform
* [alpha]: fix oyCMMptr_ConvertData() registration level
* [docu]: bump inline docu version to 0.1.10
* [and]: exchange stringAdd with STRING_ADD
* [alpha]: replace static xforms string with a function
* [alpha]: simplify oyCMMInfoGetText_f to oyCMMGetText_f
* [alpha]: update to alpha API changes
* [alpha]: CMM API docu
* [alpha]: fix regression
* [alpha]: detailed filter selection in oyFilterRegistrationMatch()
* [alpha]: start icSigDeviceSettingsTag "DevS"
* [alpha]: implement "DevS" and format code
* [alpha]: add oyOption_SetFromData and oyOption_GetData
* [docu]: add ICC specific files
* [alpha]: define deviceSettingsType
* [API]: add oyI18Nreset()
* [test]: add oyOption_s and I18N
* [test]: use public oyI18Nreset()
* [alpha]: rename oyProfile_AddTag to oyProfile_TagMoveIn
* [build]: link lcms module against Oyranos
* [docu]: update years and format
* [alpha]: add internal oyStringFromData_()
* [alpha]: add oyBlob_s and update oyProfileTag_GetText()
* [alpha]: rename oyStruct_release_f to oyPointer_release_f
* [alpha]: add descriptions to oyIMProfileTag_GetValues()
* [test]: fix a memory leak and crash
* [i18n]: update german translations
* [alpha]: data in oyOptions_s are handled by oyBlob_s
* [fix]: oyStringSplit_() empty variable
* [alpha]: separate oyProfile_TagMoveIn()
* [alpha]: add oyProfileTag_CreateFromText()
* [fix]: oyStructList_ReleaseAt()
* [fix]: missing Xorg device crash fix
* [fix]: remove automatisms from monitor functions
* [conf]: add Xrandr detection
* [API]: remove oyGetMonitorInfo() from public API
* [alpha]: modify oyGetMonitorInfo()
* [alpha]: adapt backends to new oyGetMonitorInfo_f
* [oyX1]: add XRandR properties to oyMonitor_s
* [i18n]: translate new messages to german (de)
* [and]: tune device profile hit weighting
* [fix]: monitor geometry expression ambiguity
* [conf]: XRandR detection starts with v1.2
* [build]: fix image2pdf.c building
* [and]: oyranos_monitor.h misses oyranos_alpha.h
* [build]: reorder oyranos-monitor-nvidia objects
* [and]: cheap monitor funcs call to Xinerama only
* [and]: check pointers before release
* [and]: fix some leaks in oyranos-monitor
* [oyX1]: new oyMonitor_getProperty_()
* [build]: copy logo to inline documentation
* [docu]: update doxygen dependencies
* [docu]: new header and footer for inline docu
* [docu]: move inline docu top copyright -> footer
* [oyX1]: add oyGetScreens_lib()
* [alpha]: add basic oyConfig_s structure
* [docu]: use doxygens "extends" and "memberof"
* [docu]: add header and footer to inline docu
* [build]: move configure.sh -> configure_tests.sh
* [build]: update other configure files
* [build]: add missing doxygen files to packaging
* [build]: fix for osX missing variable
* [oyX1]: fix missing function declaration
* [build]: move "oyX1" into the colour plug-in path
* [alpha]: add several internal backend loading helpers
* [alpha]: add oyCMMapiBase_s to cover oyCMMapi[5,8]_s
* [and]: internally show oyGetAllScreenNames()
* [oyIM]: update to oyCMMuiGet_f
* [oyX1]: start with oyCMMapi8_s
* [alpha]: add oyConfigs_s and APIs
* [alpha]: add oyCMMsGetFilterApis_() and oyConfigDomainList()
* [alpha]: add registration parameter to oyOption_New()
* [test]: updates for oyOption_New(), oyOptions_SetFromText()
* [build]: change oyX1 paths in RPM spec
* [alpha]: declare oyOptions_New() and change oyOption_SetFromText()
* [alpha]: allocate oyConfig_s::options
* [alpha]: fix oyOptions_SetFromText()
* [oyX1]: implement oyCMMapi8_s "list" call
* [i18n]: add comments
* [i18n]: update translation files
* [and]: move text helper definitions to oyranos_texts.h
* [oyX1]: support user pointer for tracability
* [oyX1]: complete oyCMMapi8_s "properties" call
* [alpha]: OY_ADD_ALWAYS for oyOptions_SetFromText
* [alpha]: show oySearchEmptyKeyname_() internally
* [and]: add omments about used ICC enums
* [oyX1]: add oyX1MonitorProfileSetup, oyX1MonitorProfileUnset
* [oyX1]: add "setup" and "unset"
* [alpha]: add oyConfig_Add, oyConfig_Save
* [alpha]: chance backend oyConfigs_FromPattern_f
* [and]: sort comments for translators
* [i18n]: update de
* [and]: add oyKeySetGetNames_()
* [alpha]: add xxx_FromDB() functions
* [alpha]: set some integer pointers to uint32_t
* [alpha]: update
* [alpha]: load binary keys from Elektra
* [alpha]: more oyConfig(s)_s work
* [test]: update to oyOption_New()
* [alpha]: define oyRankPad structure
* [oyX1]: add oyX1_rank_map to oyX1
* [alpha]: add oyRankPadCopy()
* [alpha]: use rank map in oyConfig_Compare()
* [alpha]: add oyOptions_SetSource()
* [alpha]: set rank_map in oyX1 and bug fixes
* [and]: add oyEraseKey_() for Elektra DB
* [oyX1]: add oyX1Region_FromDevice()
* [oyX1]: add "display_geometry" option to "list" call
* [alpha]: add oyDevicesList(), oyConfig_EraseFromDB() ...
* [i18n]: update deutsch
* [oyX1]: fix function name typo
* [oyX1]: add "display_name" option to "list" call
* [alpha]: add oyConfigs_ForDeviceClass()
* [alpha]: fix search in oySetMonitorProfile() ...
* [oyX1]: fix registration for "list" options ...
* [oyX1]: avoid XDeleteProperty() without valid Atom
* [alpha]: new oyActivateDeviceProfile() and oyProfile_FromDB()
* [oyX1]: add "device_name" filter and "icc_profile" to "list" call
* [oyX1]: remove lots of APIs
* [and]: add -l option to oyranos-monitor
* [alpha]: add verbose flag to oyDevicesList()
* [alpha]: rename oyDevicesList -> oyInstrumentList
* [alpha]: add oyCMMapiFilters_s APIs
* [alpha]: move struct definition
* [alpha]: device -> instrument
* [alpha]: more API renaming
* [oyX1]: move "property" call to oyX1InstrumentFromName_()
* [alpha]: more API renaming
* [test]: add oyInstrumentXxx()s
* [build]: link oyX1 to Oyranos
* [alpha]: more instrument API cleanup
* [oyX1]: describe interface changes
* [alpha]: add oyInstrumentSetProfile()
* [fix]: basic macro
* [oyX1]: fix overwriting of option
* [alpha]: fix minor bugs and crashes
* [alpha]: data splitting in oyConfig_s
* [oyX1]: remove message function
* [alpha]: get some moni functionality back
* [alpha]: add oyOptions_SetRegistrationTextKey_()
* [alpha]: more corrections, shuffling and cleaning
* [exmpl]: add device backend example
* [exmpl]: elaborate for the device backend
* [test]: add -l option to cli
* [alpha]: oyConfigs_FromDB() returns error
* [test]: add extensive tests to test2.cpp
* [alpha]: add "instrument_name" to "oyranos-monitor -l"
* [oyX1]: fix wrong variable arg
* [alpha]: fix oyConfig_SaveToDB() for multiple options
* [test]: add oyInstrumentProfileFromDB()
* [API]: fix oyGetMonitorProfile weak display_name arg
* [alpha]: fix wrong error report
* [and]: omit warning for new created DB keys
* [alpha]: changing error sensitivity
* [oyX1]: return issue for non found server profile
* [alpha]: add oyInstrumentAskProfile()
* [and]: minor code move
* [oyX1]: document non profile found behaviour
* [alpha]: make oyInstrumentGet() really cheap
* [oyX1]: better display_geometry match in rank_map
* [oyX1]: ignore xcalib error
* [alpha]: oyConfig_Compare() avoid false positives
* [test]: improve some "monitor" messages
* [alpha]: access actual object
* [test]: check all available monitors
* [alpha]: more introduction for graphs and backends
* [alpha]: instrument intro
* [alpha]: fix dlclose(0) crash
* [alpha]: add oyInstrumentGetInfo example
* [test]: formating of monitor properties
* [alpha]: describe oyInstrumentGetInfos type argument
* [alpha]: add oyInstrumentUnset()
* [oyX1]: "properties" call for all instruments
* [alpha]: move oyNAME_DESCRIPTION infos to core
* [oyX1]: fix newline ending EDID texts
* [test]: use cost effective intrument calls
* [fix]: fix wrong allocator in oyCopyString_
* [oyX1]: give up on wrong "instrument_name" string
* [oyX1]: empty "list" and "properties" calls
* [test]: add 2 devices to dDev dummy backend
* [fix]: honour oyProfileListGet() allocateFunc arg
* [exmpl]: correct the instrument order
* [alpha]: make oyranos_alpha_internal.h c++ ready
* [alpha]: accept "//" in oyFilterRegistrationMatch()
* [test]: add testCMMsShow()
* [alpha]: revert 85c467
* [alpha]: better warning for dlopen error
* [alpha]: add filter selector to Conversion APIs
* [alpha]: fix oyFilterNode_ContextSet_()
* [alpha]: update oyra to previous rename
* [alpha]: oyFilter_OptionsGet() assign option by default
* [alpha]: add missing definitions
* [oyra]: write PPMs
* [alpha]: change oyOptions_SetFromText() arg
* [docu]: hint about oyCMMapi8_s examples code
* [oyra]: add PPM reading
* [alpha]: add roi and pixel array to oyPixelAccess_s
* [alpha]: rename oyConversion_Run -> oyConversion_RunPixel
* [lcms]: add warnings for missed profiles
* [alpha]: allow oyUINT8 for oyArray2d_Create()
* [lcms]: allow non oyDOUBLE data
* [and]: add more warning macro formats
* [alpha]: call oyFilterSocket_Callback with plug
* [alpha]: remove some additional checks
* [alpha]: add oyConversion_GetImage()
* [alpha]: add internal oyCheckType_m macro
* [and]: add alpha object helper
* [oyX1]: check for return value
* [alpha]: switch to new oyCheckType\_\_m
* [i18n]: update deutsch
* [lcms]: fix value precission bug
* [alpha]: rename/add oyRegion_s API
* [alpha]: fix oyImage_FillArray()
* [oyra]: add oyranos_cmm_oyra_image_ppm.c
* [alpha]: add missing members to oyPixelAccess_s copy and release
* [alpha]: rename oyConversion_LinFilterAdd()/_LinOutputAdd()
* [alpha]: implement oyFilterNode_EdgeCount()
* [test]: update
* [alpha]: accept non order edges
* [alpha]: reorder graph structure members
* [alpha]: add oyFilterNode_GetId()
* [alpha]: build fix
* [alpha]: implement oyFilterNode_Connect()
* [alpha]: add object references to oyFilterNode_GetAdjacencyList()
* [oyX1]: fix non XRandR build
* [and]: add string macro
* [alpha]: rename oyFilter_Xxx to oyFilterCore_Xxx
* [oyra]: add "image/regions" filter
* [alpha]: add oyFilterNodes_s list object
* [alpha]: add oyFilterGraph_s object
* [alpha]: add oyConversion_PreProcess()
* [fix]: close a memory leak for text processing
* [and]: simplify empty text string replacements
* [alpha]: add graph to job ticket
* [fix]: a servre crash was fixed in oyCMMsGetFilterApis_
* [lcms]: return error and set the graph to "dirty" for missed context
* [and]: apply fixes to code script
* [test]: update and minor leak fixes
* [oyra]: write_ppm is no longer a output filter
* [alpha]: add oyOptions_s convenience functions
* [alpha]: new oyOptions_MoveInStruct()
* [alpha]: rename some members to clearer terms
* [alpha]: add oyImage_PixelLayoutGet() + oyImage_TagsGet()
* [alpha]: move oyFilterCore_OptionsGet to oyFilterNode_OptionsGet
* [alpha]: bunch of oyFilterNode_s accessors
* [alpha]: add oyFilterGraph_s functions
* [alpha]: add oyConversion_Set(), deprecate oyConversion_Linxxx
* [alpha]: various fixes and changes
* [alpha]: update filters to core changes
* [oyX1]: change formatting of warnings
* [oyX1]: omit one message as its emitted in the caller as well
* [alpha]: fix tagged node search
* [oyra]: add "image/display" filter
* [build]: check dependencies of more files
* [alpha]: add tag selection to oyFilterGraph_GetNode()
* [alpha]: oyToDataType_m provides a oyDATATYPE_e
* [lcms]: make it a mediator
* [oyra]: make ppm_reading more robust
* [oyra]: select child filter more specifically
* [oyX1]: pass the job ticket id to messages
* [lcms]: parallelise
* [build]: update RPM spec file
* [test]: update test files
* [alpha]: repair oyConversion_CreateBasic() and oyColourConvert_()
* [lcms]: reduce verbosity
* [oyra]: close memory leak
* [alpha]: add oyRegion_SamplesFromImage()
* [alpha]: change oyImage_Fill-/ReadArray APIs
* [oyra]: change write on array
* [and]: add colour mime types and lcms logo
* [oyX1]: osX compile fix
* [build]: fix logo to docu copying
* [docu]: correct data flow direction graph
* [oyX1]: start interfacing osX CoreGraphics
* [alpha]: reset intermediate array change
* [oyX1]: move "display" filter from oyra to oyX1
* [conf]: check for Xcolor and Xfixes
* [alpha]: add oyProfile_FromMD5()
* [oyX1]: more Xcolor integration
* [conf]: add --enable-devel option
* [alpha]: increase compatible version plug-in rank
* [oyX1]: disable DISPLAY variable checks on osX
* [oyX1]: switch from deprecated DM to CG APIs
* [alpha]: filter double APIs in oyCMMsGetFilterApis_()
* [alpha]: improve oyRegion_Show() output
* [oyX1]: abstract osX device to oyMonitor_s
* [alpha]: add oyOption_StructGet()
* [alpha]: fix oyDeviceUnset()
* [oyX1]: use "device_region" for oyX1 oyRegion_s request
* [oyIM]: add debugging for oyIM mate backend
* [fix]: profile size reporting in oyranos-monitor
* [test]: extend testMonitor
* [oyX1]: print DB keyset name with -v option
* [docu]: repair broken arguments
* [alpha]: support keys in oyFilterRegistrationMatch
* [oyX1]: split display filter out
* [test]: add oyFilterRegistrationMatch key matching
* [oydi]: set window reative coordinates in XcolorRegion
* [oydi]: group Xfixes includes with Xcolor
* [build]: relaxe on missed msgcat
* [conf]: msgcat warn only with gettext
* [exmpl]: add X11 example applications
* [exmpl]: extend the documentation
* [build]: remove examples from top build level
* [build]: add oydi to RPM
* [build]: add missing file
* [and] add oyFinish_() for cleanup
* [add]: oy_debug_memory variable
* [and]: close some memory leaks
* [exmpl]: add _NET_COLOR_DESKTOP support
* [alpha]: switch to dynamic object hashing
* [conf]: repair Xcolor detection
* [alpha]: rename oyRegion_s to oyRectangle_s
* [and]: add alpha cleanup
* [test]: update
* [build]: include example directory into check target
* [oydi]: remove old XColorRegion
* [alpha]: fix for empty image pointer
* [exmpl]: print warnings in xcmsevents
* [oydi]: fix Xcolor window region change detection
* [exmpl]: plain formatting
* [docu]: mention Tomas Carnecky in AUTHORS
* [oyIM]: add rendering_high_precission option
* [lcms]: support cmsFLAGS_NOTPRECALC
* [exmpl]: switch colour_desktop CMS
* [exmpl]: free resources for colour_desktop
* [alpha]: abstract data types in oyConnector_s
* [exmpl]: activate XRandR again
* [docu]: add hint for ColourWiki updates
* [build]: include includedir into example builds
* [build]: support out of source tree builds
* [and]: add ITULab.icc
* [exmpl]: make xcmsevents start more verbose
* [oydi]: robuster Xwindow positions
* [alpha]: abtract connectors compile
* [API]: set OY_TYPE_STD to "imaging"
* [alpha]: export oyObject_UnRef() internally
* [alpha]: move some general CMM functions to oyranos_cmm.c
* [build]: clean examples directory
* [build]: enable absolute paths
* [exmpl]: add target
* Revert "* [exmpl]: add target"
* [build]: avoid missing cobjects warning
* [conf]: fix check of correct dependency detection
* [test]: update tests/test_oyranos
* [build]: correction to dc0f30c3
* [docu]: remove old bug entry
* [alpha]: design a new device/driver API
* [build]: move most modules into subdirectories
* [build]: be sensitive to localy generated headers
* [alpha]: delete driver split
* [alpha]: undo driver introduction from 5265b841
* [build]: deactivate oycu
* [alpha]: rename oyConfig_Check_f to oyConfig_Rank_f
* [alpha]: define oyOptions_SetDriverContext()
* [alpha]: move all backend calls into one "command" option
* build]: moving of the oyIM module
* [build]: move oyIM to the modules directory
* [oyX1]: fix empty string pointer
* [exmpl]: fix call to "command" key
* [build]: fix some out of source build issues
* [oyX1]: increase verbosity for oyranos-monitor -v
* [oyIM]: add 'nvmt'
* [alpha]: implement oyOptions_SetDriverContext() ...
* [i18n]: update deutsch
* [docu]: correct Profile API overview
* [oyX1]: improve command line parsing in oyranos-monitor
* [oyX1]: add long options to oyranos-monitor
* [docu]: extend oyranos-monitor man page
* [docu]: describe screen selection ...
* [docu]: relayout the oyranos-policy man page
* [oyX1]: fix the new oyranos-monitor -s option
* [build]: disable oycu dummy lib
* [alpha]: add oyStructList_Sort()
* [alpha]: increase robustness of oyConfig_Compare
* [alpha]: add oyProfile_DeviceGet() and ...
* [alpha]: prefere 'pddt' over 'dmnd' and 'dmdd'
* [i18n]: update fl_i18n to ICC Examin
* [alpha]: oyConfig_Compare() soften matches ...
* [and]: unroll loop
* [and]: remove unused code snippet
* [conf]: beautify path variable
* [test]: add testCMMnmRun for named colours
* [and]: speed up oyGetKeyString_()
* [alpha]: new oyOPTIONATTRIBUTE_VIRTUAL_DB flag
* [alpha]: virtualise DB access and other optimisations ...
* [alpha]: fixes for last commit
* [alpha]: remove oyOPTIONATTRIBUTE_VIRTUAL_DB ...
* [alpha]: correct positioning during repeated oyConversion_RunPixels()
* [and]: avoid one allocation in oyStringSplit_()
* [build]: add test2c target
* [alpha]: shorten oyObject_HashEqual()
* [alpha]: access image data over oyConversion_GetImage() (cherry picked from commit 76d71471bb1fd52f577fd8ebde8682aee88eb9e1)
* [exmpl]: reduce more image calls (cherry picked from commit e5e71021c31e26c127c476358c187edcef213541)
* [alpha]: in the fly initialise processing data
* [alpha]: data copy only for root node
* [oydi]: pass oyCMMFilterPlug_Run() status (cherry picked from commit df4805df35e905ae672c72a2c4c95281f5e9243b)
* [alpha]: skip rectangle copy into the same image (cherry picked from commit f5d463688fa710b0a419b76cb25bf9d39791b5e8)
* [alpha]: add int32_t manipulators to oyOption(s)_s API (cherry picked from commit 8eae24b81f9bb021f47a4fa9e405b9aaac8e630d)
* [test]: add testOptionInt
* [test]: remove "properties" call
* [alpha]: change the oyFILTER_REG_e
* [alpha]: oyFilterRegistrationToText supports masks
* [test]: add testOptionsCopy
* [alpha]: skip non understood XML sequence (cherry picked from commit 451a9aa35d1aa87007c5050cee786ddd83936c44)
* [exmpl]: add a "generator" branch target
* [exmpl]: attach a "resolve" request
* [exampl]: undo test case in image_display
* [build]: support dependency tracking of sub directories
* [and]: add non allocating string functions
* [lcms]: send oyCONNECTOR_EVENT_OK for a missed context
* [alpha]: no allocations in oyFilterRegistrationMatch()
* [alpha]: omit oyCONNECTOR_EVENT_OK event reporting
* [alpha]: remove oyStringSegment_()
* [and]: compile fix to oyranos_texts.h
* [build]: link lcms to test2
* [lcms]: write own xforms cache identifier
* [oyIM]: cache dlopened libs in Oyranos
* [fix]: wrong deallocator in osX oyFlattenProfileProc()
* [alpha]: add oyCMMapiNumberToChar() ...
* [alpha]: add object pool
* [test]: set elektra handle global
* [fix]: wrong memory deallocation
* [and]: wrap string C functions
* [and]: use a simple allocation pool
* [test]: add testStringRun() and profiling tests
* [test]: compiled with -DNO_OPT
* [and]: switch memory pool off
* [alpha]: use double lenth for has keys
* [alpha]: simplify oyCacheListGetEntry_()
* [oyX1]: fix X -geometry conformance
* [alpha]: fix iterator shift on graph start
* [exmpl]: Gerhard Fürnkranz for code delivering in AUTHORS
* [alpha]: move oyFilterPlug_ResolveImage up ...
* [alpha]: change oyRectangle_SamplesFromImage() API
* [alpha]: put more oyRectangle_s on the stack
* [oyra]: place "root" nodes oyRectangle_s on the stack
* [alpha]: omit oyFilterNode_OptionsGet in oyPixelAccess_Create()
* [test]: new scaling of times
* [alpha]: remove linear graph functions
* [test]: update test.c to previous commit
* [test]: more api7/8 specific infos in test2 "CMMs show"
* [alpha]: correct plug/socket name missmatches
* [alpha]: allow non copy in oyImage_FillArray()
* [test]: switch to a action per second output
* [exmpl]: fix colour_desktop filter names
* [alpha]: fix regression from 1f9b7620
* [exmpl]: update colour_desktop to fbbfc4c4
* [alpha]: change oyBlob_s; add oyFilterGraph_ToBlob()
* [and]: oyFindProfile_ return zero for no success
* [and]: fix definition of oyStrcpy_
* [fix]: correct usage of oyProfile_GetFileName()
* [lcms]: support explicite verbose oyDL output
* [alpha]: oyFilterNode_ContextSet_() supports new argument
* [docu]: add minor corrections
* [conf]: check for libraw
* [oyra]: add "filename" option
* [and]: make oyranos_io.h C++ compilable
* [lraw]: add libraw cameraRAW plug-in ...
* [oyra]: correct file I/O option
* [exmpl]: add "dFil" a stripped example filter
* [conf]: make lraw optional and integrate with RPM
* [build]: check for compiz before compiling the example
* [build]: check in examples for cairo-pdf
* [oyX1]: no command fallback
* [alpha]: skip non sense calls in oyDevicexxx
* [oyX1]: fix empty deallocation
* [oyX1]: fix of calling directly malloc()
* [test]: correct usage of "command" backend option
* [alpha]: move oyFilterPlug_ImageRootRun() to core
* [oydi]: fix too many Xorg connections
* [lraw]: basic cameraRAW loading
* [alpha]: give warning for profile creation from no data
* [oydi]: correct usage of "command" backend option
* [exmpl]: compiz v0.8.2 compile fix
* [alpha]: save one image copy in oyConversion_RunPixels()
* [alpha]: fix oyImage_FillArray() allocations
* [alpha]: fix integer division for oyImage_s::viewport
* [docu]: mark older oyPixelAccess_s iterator concepts
* [alpha]: refine oyImage_FillArray()s do_copy argument
* [oyra]: copy image data only in root image filter
* [alpha]: add oyArray2d_ToPPM_()
* [oyra]: omit copy of same unchanged data
* [alpha]: use offsets in oyFilterPlug_ImageRootRun()
* [alpha]: support offsets in oyConversion_RunPixels
* [exmpl]: add image moving to image_display
* [alpha]: robuster profile load after "setup" call
* [docu]: add sketch to oyPixelAccess_s
* [docu]: tell source of automatic generated files
* [oyX1]: fix cli parsing for -x/y args
* [oydi]: require the X Display to be passed
* [exmpl]: image_display supports "display_id"
* [exmpl]: add Xmu signal handler to xcmsevents
* [alpha]: add texts list to oyCMMInfo_s
* [oydi]: small code formatting
* [exmpl]: complete 3ef38636 oyCMMInfo_s::texts
* [and]: shorten debug message output
* [exmpl]: fix array exeeding in image_display
* [lcms]: add "help" text and support cmsFLAGS_PRESERVEBLACK
* [alpha]: disable oyCMMapi1_s
* [docu]: comment to oyProfile_DeviceAdd()
* [lcms]: remove oyCMMapi1_s
* [lcms}: support effect and simulation profiles
* [lcms]: fix gray scale rendering
* [alpha]: change oyFilterCore_New() API use default options
* [and]: build fix for change in oyranos_debug.h
* [alpha]: adapt option registrations
* [alpha]: oyFilterNode_OptionsGet() returns node options
* [docu]: inline page for writing filters and device backends
* [docu]: bring all CMM function name defines to the top
* [exmpl]: convert instrument to device name
* [exmpl]: complete previous
* [docu]: clearify about oyConfig_AddDBData
* [docu]: comment oyCMMapi8_s::rank_map
* [and]: osX compile fix
* [build]: correct test_oyranos path
* [lcms]: tag with the registration string
* [oyra]: start with new meta file load filter
* [docu]: collect and link the registration string infos
* [alpha]: define oyCMMOptions_Check_f
* [docu]: oyConfigDomainList to oyDeviceGet
* [test]: improve "CMM devices listing"
* [test]: complete previous commit
* [alpha]: allow '_' in front of atrributes in registrations
* [lcms]: add underscores to registration string
* [docu]: oyDeviceGetInfo example code
* [build]: integrate CUPS module
* [docu]: add maintainance entry
* [build]: add subdir dependency
* [alpha]: support oyCMMptr_s in oyOption_GetData()
* [alpha]: add oyCMMptr_s funtions
* [alpha]: key extraction from registration
* [alpha]: catch on missing arguments
* [alpha]: ignore missing devices for oyConfigs_FromDeviceClass()
* [fix]: byte order of md5 checksum
* [exmpl]: add oymd5icc
* [oyX1]: add "help" call
* [alpha]: add oyCMMapi9_s for plugable policy support
* [oyIM]: conform to registration rules
* [alpha]: add oyFilterMessageFunc()
* [alpha]: support oyCMMspi9_s as oyCMMapiFilter_s
* [oicc]: add ICC policy module
* [test]: support oyCMMapi9_s in test2 "CMMs show"
* [alpha]: fix getting default options from oyCMMapi9_s
* [alpha]: add oyConversion_Correct()
* [alpha]: implement oyConversion_Correct()
* [exmpl]: request verbose output for oyConversion_Correct()
* [oicc]: give some verbose infos
* [exmpl]: fix image_display moving
* [alpha]: add oyBlob_ReleaseNoPtr() for const oyBlob_s::ptr
* [alpha]: give oyConnector_s::connector_type more meaning
* [alpha]: add apis 8 and 9 to oyFilterRegistrationMatch()
* [alpha]: new connector selection
* [alpha]: oyConversion_RunPixels() fix crashes
* [docu]: clear about oyBlob_SetFromData allocations
* [alpha]: substitute oyBlob_ReleaseNoPtr with oyBlob_SetFromData
* [alpha]: copy missed oyBlob_s::type
* [i18n]: removed some c-formats and updated deutsch
* [alpha]: no implicite options in oyFilterCore_New()
* [alpha]: disambiguate OY_OPTIONSOURCE_FILTER/oyOPTIONSOURCE_FILTER
* [lcms]: support "proof_soft" and "proof_hard" options
* [lcms]: no device class checking
* [oicc]: support policy options
* [exmpl]: fix compile warning
* [lcms]: implement oyCMMapi4_s::oyCMMuiGet
* [lcms]: fix device link creation with proofing
* [oicc]: fix proofing flag policy handling
* [alpha]: add oyFilterNode_UiGet()
* [and]: fix compile warning
* [test]: fix single monitor test with test2
* [alpha]: assign/copy options in oyFilterCore_New()
* [oydi]: copy options to newly created ICC nodes
* [alpha]: rename oyConversion_CreateBasic() to xxxPixels()
* [alpha]: remove duplicated definition
* [alpha]: additional data checks for oyOption_s reading
* [lcms]: one more check for opyOption_s reading
* [docu]: oyConversion_CreateBasicPixels arguments
* [alpha]: add new channel types
* [alpha]: remove one oyChar
* [alpha]: add oyOption_SetFromDouble/xxx_GetValuexxx
* [doc]: add Joe Simon as CUPS backend author
* [conf]: oyIM modules path now (lib)/colour/modules
* [lcms]: add more checks
* [oydi]: add late image data resolving
* [build]: add "oicc" and "CUPS" modules to RPM
* [alpha]: add oyImage_ProfileGet()
* [exmpl]: image_display uses "oydi"s data resolving
* [build]: skip directory part for GNU soname linking
* [build]: separate INCL from CFLAGS
* [build]: skip string length warnings
* [alpha]: remove obsolete ':' in oyOption_GetValueText()
* [test]: support the "supported_devices_info"
* [test]: add failing oyOption_s case
* [test]: 5a88a1f1 correction
* [test]: add "Set oyOptions_s" to test2.cpp
* [alpha]: fix for f7d5558c and 7762729f
* [conf]: SunOS/osX/BSD fixes
* [conf]: define MOVE for makefiles
* [build]: correct fluid expanding for non root builds
* [fix]: crash on too small ICC profile blocks
* [oyX1]: "icc_profile" set always a answere
* [alpha]: adapt oyDeviceAskProfile()
* [conf]: better support --disable-libXxxx
* [CUPS]: set data correctly
* [alpha]: fix oyOption_GetValueText() for oyVAL_STRUCT
* [test]: show "icc_profile" option
* [docu]: fix typo in oyranos.3
* [docu]: start building man pages
* [lcms]: fix UI string
* [CUPS]: fix Oyranos profile opening
* [oyX1]: add XRandR "EDID" name
* [docu]: fix some doxygen related issues
* [oyX1]: add verbose output for xcalib
* [fix]: "oyranos-monitor -v" needs to do a setup
* [and]: add OYRANOS_BACKTRACE variable
* [doxu]: add OYRANOS_BACKTRACE variable
* [CUPS]: some more compile fixing ...
* [CUPS]: remove the Gray default profile
* [CUPS]: support the specified "cupsICCProfile"s
* [CUPS]: "cupsICCProfile"
* [CUPS]: rename to CUPSgetProfiles
* [CUPS]: fix for n profiles in one CUPS device
* [CUPS]: add missed "cupsICCProfile" qualifiers
* [CUPS]: fix valgrind memory error
* [alpha]: add oyConfigs_Modify_f to oyCMMapi8_s
* [oyX1]: implement oyX1Configs_Modify
* [alpha]: do not set the same text array twice
* [oyX1]: reduce double functionality
* [test]: fix test2.cpp compilation
* [oyX1]: fix oyranos-monitor regression
* [oX1]: fix for "properties" call
* [CUPS]: add device list only if one exists
* [docu]: doxyment oyCMMapi8_s::oyConfigs_Modify
* [docu]: play wit hdoxygen parameters
* [alpha]: add oyConfigs_Modify
* [alpha]: deprecate oyCMMapi1_s and oyCMMapi2_s
* [alpha]: use oyConfigs_Modify in oyDeviceBackendCall
* [CUPS]: add basic "list" support in oyConfigs_Modify
* [CUPS]: merge Configs_Modify with Configs_FromPattern
* [test]: fix testCMMDevicesListing() in test2.cpp
* [alpha]: the debugger likes function scope
* [CUPS]: activate DeviceAttributes_()
* [build]: move monitor sources to modules/devices
* [exmpl]: update dDev to new oyCMMapi8_s
* [oyX1]: rename internal usage function
* [oyX1]: fix typo
* [conf]: set month
* [docu]: formatting
* [alpha]: update oyCMMapi_Check_()
* [alpha]: add oyDeviceSimiliarFromDB()
* [test]: show oyDeviceSimiliarFromDB() in test2
* [alpha]: generalise to oyDeviceSelectSimiliar()
* [sand]: initial commit to box
* [xforms]: collect element data
* [conf]: fix shell expression
* [conf]: test for CUPS availablility
* [build]: use CUPS detection
* [fix]: Solaris build warnings
* [build]: use make defaults in example/
* [build]: omit hardcoded -Wall in examples
* [build]: substitute CPP with CXX in example/
* [build]: observe files
* [conf]: test for SANE
* [xforms]: collect relevant nodes
* [xforms]: start UI builder framework
* [alpha]: add new object type
* [alpha]: correct oyOBJECT_UI_HANDLER_S
* [xforms]: update to Oyranos object typing
* [alpha]: oyOptions_GetText iterates into options
* [xforms]: write XFORMS as of the mozilla plugin
* [lcms]: refere xf:model with absolute path
* [xforms]: simplify default namespace declaration
* [lcms]: explicite choices label
* [docu]: oyOptions_ForFilter
* [API]: deprecate OY_TOP_INTERNAL
* [alpha]: update to OY_TOP_INTERNAL change
* [alpha]: add XML namespace in oyCMMapi9_s
* [alpha]: 542d4d63 part two
* [oicc]: add namespace ...
* [alpha]: sort oyOptions_GetText output alphabetical
* [xforms]: function separation ...
* [alpha]: omit redundant XML levels
* [alpha]: fix typo
* [alpha]: fix oyOptions_ParseXML_()
* [alpha]: reverse filter and common layouts
* [alpha]: add XFORMS APIs
* [alpha]: the declarations to 517ee570
* [docu]: add xml files to include XFORMS APIs
* [xforms]: export most code to Oyranos
* [alpha]: remove oyCMMapi_s::oyCMMCanHandle_f
* [alpha]: detailed module checks
* [oicc]: and lcms dont use HTML tables in XFORMS
* [oyX1]: translate
* [alpha]: separate XMLs
* [xforms]: add oy_ui_cmd_line_handlers
* [docu]: update the module framework dot graph
* [docu]: clear the module APIs text
* [docu]: substitute "backend" with "module"
* [xforms]: embedd locale info into XML
* [xforms]: add cli options naming scheme
* [alpha]: module oyCMMapi4_s API change
* [alpha]: add manufacturer
* [i18n]: update deutsch
* [alpha]: add oyOptions_s double calls
* [oyX1]: export EDID colour primaries
* [exampl]: report EDID changes in xcmsevents
* [exampl]: show EDID content in xcmsevents
* [lcms]: tell options in debug mode
* [alpha]: rename oyCMMDataTypes_s
* [docu]: decribe oyCMMobjectTypes_s::id
* [alpha]: remove oyCMMobjectGetText_f::pos
* [alpha]: tune oyCMMobjectGetText_f
* [alpha]: add oyStruct_GetText()
* [lcms]: fix tracking for proofs
* [exmpl]: fix typo in "colour_desktop"
* [docu]: precise the module system descriptions
* [i18n]: fix typo deutsch
* [xforms]: move labels
* [docu]: disambiguate "device_name" for oyConfigs_Modify
* [oforms]: move handlers to Oyranos core ...
* [build]: rename the tool binary
* [xforms]: default CSS for oyXFORMsFromModelAndUi
* [oicc]: add assumed source profiles to XFORMS
* [oicc]: add h4 tags to XFORMS
* [xforms]: parse h4 HTML tag
* [xforms]: omit XFORMS leading slash of ref element
* [alpha]: copy setlocale string
* [xforms]: silent mode with all tags
* [i18n]: update deutsch
* [oforms]: add module option parsing
* [oforms]: support option value passing by space
* [oforms]: move sand box to oforms
* [build]: adapt to new directory
* [oforms]: separate command line + add FLTK XFORMS UI
* [conf]: fix missing directory for remote builds
* [oforms]: move XFORMS code to oforms directory
* [build]: update to cover new oforms code
* [build]: add missed test files
* [oicc]: omit non visible headlines in XFORMS
* [alpha]: support oyASSUMED_WEB in oyProfiles_ForStd()
* [core]: add oyGROUPs to atstic options
* [oforms]: fix crash for non symetric items
* [oicc]: use the cores static options APIs
* [oicc]: get options from old settings API
* [i18n]: try to fix charset oyranos-xforms-fltk
* [oforms]: add oyFormsArgs_s object
* [oforms]: use new oyFormsArgs_s object
* [oforms]: remove old argument struct; add ::forms
* [oforms]: track XFORMS model changes in FLTK
* [oforms]: display XFORMS model changes
* [build]: add new oforms files
* [build]: oforms test build with fl_i18n
* [build]: add oyranos-forms-fltk-devel
* [core]: add "attr:ref" hint to option from XFORMS node
* [alpha]: add oyOBJECT_FORMS_ARGS_S
* [oyX1]: export "mnft" property from EDID
* [oyX1]: provide "edid_colours_text.Xorg" info
* [alpha]: new oyProfile_AddTagText()
* [oyX1]: fix memory leak
* [build]: link lcms to oyranos-monitor
* [core]: add new file i/o helper
* [oyX1]: oyranos-monitor add -f and -o options
* [docu]: update oyranos-monitor
* [i18n]: update deutsch
* [test]: omit outdated monitor test
* [build]: support in path building for oforms
* [core]: better match to C++ for oyWriteMemToFile_()
* [oicc]: complete registration string
* [oforms]: add cli options for FLTK example
* [alpha]: add oyOptions_Set()/_SetOpts()
* [alpha]: fix oyOption_GetData() ...
* [example]: add options editing in image_display
* [alpha]: remove old unused code
* [alpha]: add signal API
* [alpha]: add more signal functions
* [conf]: format CUPS version
* [CUPS]: httpAssembleURIf() seems new to CUPS v1.2
* [core]: give a warning about a missed string...
* [test]: add osX locale info
* [core]: add OYRANOS_DEBUG_SIGNALS environment variable
* [alpha]: fall back to known types in oyStructList_GetType_()
* [alpha]: fall back to known types in oyStruct_GetText()
* [alpha]: correct signal direction
* [alpha]: add oySignalToString()
* [alpha]: serial oyFilterSocket_SignalToGraph()
* [docu]: link and clean deprecated top "sw"
* [exampl]: observe graph and deprecate FLTK view
* [exampl]: add dirty flag to image_display
* [alpha]: rely on removal for node context updates
* [alpha]: add oyStruct_ObserversCopy()
* [alpha]: add a models list to each observer
* [oydi]: clone observation state to copied display nodes
* [alpha]: make oyStruct_ObserverRemove() precise
* [oicc]: proofing intent is advanced stuff
* [conf]: FLTK >= v1.3 check for UTF8 selection
* [oforms]: better i18n selection
* [ui-fltk]: use correct internal UTF-8 macro
* [core]: default to UTF-8 encoding for XFORMS
* [core]: add "rendering_gamut_warning" and xxx"_high_precission"
* [lcms]: give a better format to hash text
* [exmpl]: move strange option out of node data
* [alpha]: beautify node description
* [docu]: small corrections
* [alpha]: fix compiler warnings
* [test]: update to changed oyOptions_ForFilter_
* [lcms]: merge proofing profiles into multi profile chain
* [alpha]: fix too early hash object release
* [docu]: oyCMMptr_LookUp() parameters
* [alpha]: oyCMMptr_LookUp naming
* [alpha]: add oyCMMptrLookUpFromText() to CMM API
* [alpha]: omit custom allocator to oyCMMptr_s
* [lcms]: cache abstract proofing profiles
* [lcms]: closer to native gamut marking
* [docu]: fix some doxygen errors
* [lcms]: fix simulation option handling
* [oforms]: locale fallback
* [exampl]: create XFORMS locally in "image_display"
* [oforms]: change -i option for oyranos-xforms-fltk
* [lcms]: correct gamut check
* [core]: simplify some UI strings
* [build]: link "image_display" with fl_i18n
* [oforms]: better window title
* [oforms]: fix FLTK widget placing for select1 items
* [exmpl]: generate XFORMS in "image_display"
* [i18n]: update deutsch
* [oforms]: stretch FLTK widgets somewhat
* [alpha]: oyProfileTag_Set() free memory pointer
* [build]: add exmaples to translations
* [i18n]: update deutsch
* [core]: add oyStruct_AllocHelper_m_ macro
* [oyIM]: work with zeroed memory
* [oyIM]: skip inappropriate memory during "psid" read
* [oforms]: add close button to FLTK dialog
* [oforms]: add hint field to FLTK panel
* [alpha]: add oyOptions_SetFromData() +
* [alpha]: fix documentation
* [oyX1]: fix use of uninitialised variable
* [lcms]: C++ compile fixes
* [alpha]: oyOptions_FindData() fix argument type
* [oforms]: add extra data to oyFormsArgs_s
* [oforms]: support parsing of multiple subelements
* [alpha]: g/setters for the oyFormsArgs_s extra data
* [oforms]: adapt to changed oyUiHandler_s
* [oforms]: add hint field to FLTK
* [alpha]: adapt to oforms changes
* [core]: add libxml2 helpers to parse XFORMS
* [oforms]: new oyUiHandler_s behaviour
* [alpha]: restructure oyUiHandler_s
* [oforms]: oyFormsCallback_s with generic pointer
* [oforms]: parse libxml2 instead of Oyranos objects
* [oforms]: add help view to FLTK window
* [oforms]: clean unused functions
* [oforms]: clean help_view content
* [alpha]: oyFilterCore_s object member clearing
* [docu]: correct the registration description
* [alpha]: rename oyOBJECT_FORMS_CALLBACK_S
* [alpha]: add oyCallback_s object
* [oforms]: adapt to new oyCallback_s
* [oyIM]: rename meta module file
* [oforms]: fix xpath to registration mapping
* [core]: fix compiler warnings
* [alpha]: fix missing language crash
* [core]: CMSetDefaultProfileBySpace is deprecated
* [fltk]: check for i18n compilation
* [core]: switch from deprecated osX CM APIs
* [oyX1]: support display informations
* [core]: split osX ColorSync part out
* [build]: activate osX code
* [core]: deactivate deprecated osX code
* [core]: add headers fo oyranos_cs.c
* [test]: robuster i18n test
* [test]: bypass a empty oyLanguage()
* [test]: explicite string for non i18n check
* [conf]: update date
* [oyX1]: shorter report on no monitors case
* [core]: populate oyWriteMemToFile2_() for oyranos_gamma.c
* [core]: move away from old monitor APIs, part one
* [docu]: declare elder monitor APIs as deprecated
* [core]: elder monitor APIs are deprecated
* [core]: remove more deprecated monitor API calls, part two
* [test]: omit old monitor API
* [exmpl]: omit monitor API
* [oyX1]: support osX EDID
* [alpha]: add oyConfig_Has()
* [oyX1]: switch to a generic colour data key name
* [oyX1]: increase verbosity in debug mode
* [exmpl]: show alternative ICC profile names in xcmsevents
* [core]: support temp file writing in oyWriteMemToFile2_()
* [core]: correct new flag reading
* [alpha]: add new oyCMMapi10_s module type
* [lcms]: add "create_profile" filter
* [oyX1]: use "C" text formating
* [alpha]: add oyOptions_Handle() to access oyCMMapi10_s
* [alpha]: meaningfull return code in oyOptions_FindDouble()
* [alpha]: add oyDeviceAskProfile2()
* [oyX1]: provide a fallback "icc_profile" on request
* [oyX1]: remove direct lcms dependency from "oyranos-monitor" command
* [build]: omit liblcms for "oyranos-monitor"
* [core]: fix compiler warnings
* [oyX1]: show "colour_matrix" verbosely
* [alpha]: switch oyOption_GetValueText() to %g format
* [oyX1]: remove unused code
* [oyX1]: add four EDID fields
* [oyX1]: add parse_edid library
* [oyX1]: switch to new EDID parsing API
* [oyX1]: implement XEdidPrintString()
* [oyX1]: minor fixes
* [core]: fix invalid memory access (KM crash)
* [oyX1]: add minimalistic EDID example
* [exmpl]: adapt xcmsevents to changed oyUnrollEdid1_
* [oyX1]: fix osX compiler error
* [oyX1]: add key defines for parse_edid
* [alpha]: change oyCMMui_s and oyCMMapi8_s
* [core]: fix compiler warning
* [alpha]: remove static oyFilterCore_s::name_
* [alpha]: update to module API changes
* [test]: add OpenMP test files
* [conf]: check for OpenMP
* [test]: add better time for SMP
* [build]: support external XNVCtrl
* [conf]: fix previous commit
* [CUPS]: i18n for menu entry
* [exmpl]: beautyfy code of "dFil"
* [oyX1]: failed try to sync nvidia with Xinerama
* [oyX1]: fix device port naming
* [alpha]: fix some thread issues
* [alpha]: more thread fixes
* [exmpl]: fix compiler warning
* [and]: modify code generator
* [alpha]: add oyConfDomain_s
* [test]: test oyConfDomain_s
* [alpha]: add oyConfDomain_xx APIs
* [oyX1]: fix missing string types
* [alpha]: implement more of oyConfDomain_xx
* [test]: update to previous changes
* conf]: set libraw includes
* [i18n]: update deutsch
* [alpha]: add test for device UI strings
* [alpha]: clear TODO hint
* [CUPS]: add i18n support
* [i18n]: update deutsch
* [core]: send warning for non installed policies
* [oicc]: check for non available policies
* [oforms]: catch failed filter creation
* [build]: honour CFLAGS in oforms/
* [oforms]: add XFORMS helpers
* [test]: add oforms output to test2 CMMs
* [oforms]: improve help text view
* [oforms]: remove unused printout
* [oyX1]: support TwinViewXineramaInfoOrder
* [core]: code introspection
* [test]: FF compatibility
* [oforms]: add scroll and nicer choice scaling
* [build]: fix examples compilation
* [build]: use xdg utils to install mime
* [build]: more verbosity with examples
* [oyX1]: fix Apple EDID
* [oyX1]: add model_id to edid_icc header
* [core]: open files in binary mode
* [alpha]: write ICC conforming text tag types
* [exmpl]: compile fixes for compiz-0.8.4
* [exmpl]: create ppm for testing image_display
* [build]: fix oforms building
* [exmpl]: xcmsevents compile fixes for C++
* [oicc]: support proofing options
* [docu]: precise a option description
* [exmpl]: add a CCSM icon for compiz
* [i18n]: add CCSM text about the desktop plug-in
* [build]: install the new compiz files
* [exmpl]: simlify xcmsevents
* [oyX1]: swap UI string order
* [core]: support non core messages to be passed
* [core]: add oyStringAddPrintf_
* [oyX1]: one string print out in oyranos-monitor
* [exmpl]: xcmsevents - move prints to messages
* [exmpl]: add small xcmsevents based app for Qt
* [exmp]: change file mode
* [conf]: correct include paths
* [exmpl]: reduce compile warnings
* [oyX1]: describe newer options
* [oyX1]: fix oyranos-monitor -l output
* [oyX1]: hunt XGetAtomName() bug
* [oyX1]: complete ICC in X 0.3
* [build]: fix some compile warnings on linux
* [exmpl]: add debug hook into colour_desktop
* [oyX1]: fix compile warnings
* [conf]: fix include dir in oyranos-config script
* [oyX1]: fix DB key naming
* [oyX1]: oyranos-monitor -e: erase from DB
* [alpha]: more detailed warning for empty option
* [oyX1]: skip setting identical properties
* [exmpl]: fix message in debug mode
* [exmpl]: switch off advanced debugging in compiz
* [oyX1]: EDID colour_matrix check
* [alpha]: fix wrong memory access
* [build]: clean local nvidia lib
* [alpha]: add oyOption_GetRegistration
* [alpha]: add more consistency checks
* [alpha]: support DB entries in oyConfig_Compare
* [alpha]: fix oyConfigs_FromDB,oyDeviceSetProfile
* [alpha]: add oyConfigs_SelectSimiliars()
* [conf]: set new epoch
* [docu]: correct including of Oyranos header(s)
* [core]: fix file writing
* [core]: add oyPolicyFileNameGet()
* [core]: fix oyranos-policy to select a policy
* [i18n]: update Deutsch
* [core]: adapt policy settings
* [alpha]: bug fixes for module loading
* [build]: switch mime application/vnd.iccprofile
* [docu]: increase icon size
* [oyX1]: add _ICC_DEVICC_PROFILE atom
* [exmpl]: fix xcmsevents_common event loop
* [conf]: fix pkg-config include path
* [build]: external make install
* [exmpl]: use the XEvent supplied Display
* [exmpl]: ignore QX11Info
* [exmpl]: add "device.icc_profile"
* [docu]: update write_device_modules section
* [build]: adjust file permissions
* [exmpl]: ppm creation is optional
* [conf]: implicitely set lib64
* [exmpl]: add build variable to image2pdf
* [exmpl]: correct some typos in xcmsevents
* [alpha]: check argument in oyHash_SetPointer_
* [build]: update spec file
* [oyX1]: fix typo
* [core]: fix wrong memory release
* [exmpl]: support 0.4 DRAFT2 in colour_desktop
* [exmpl]: setup profiles in colour_desktop
* [exmpl]: clean some warnings in colour_desktop
* [exmpl]: allow a reverse setupICCprofileAtoms()
* [exmpl]: unset _ICC_(DEVICE_)PROFILE(_xxx)
* [exmpl]: careful variable handling in colour_desktop
* [exmpl]: omit own window with no c->display
* [exmpl]: add qcmsevents icon resources
* [exmpl]: split qcmsevents.cpp to compile
* [exmpl]: add a project file for qcmsevents
* [exmpl]: build qcmsevents
* [exmpl]: qcmsevents task bar _NET_COLOR_DESKTOP
* [exmpl]: no window on startup with qcmsevents
* [exmpl]: add oyMSG_DISPLAY_ERROR to qcmsevents
* [oyX1]: remove _ICC_PROFILE_IN_X_VERSION_STRING
* [exmpl]: unify _NET_COLOR_DESKTOP displaying
* [exmpl]: change _NET_COLOR_DESKTOP content
* [alpha]: turn flags into options
* [core]: correct warning for zero data writing
* [API]: add ICC in X macros
* [oyX1]: use the ICC in X macros
* [oyX1]: add the "colour_server" option
* [alpha]: change oyDeviceGetProfile, removeoyDeviceAskProfile
* [exmpl]: _NET_COLOR_DESKTOP add capabilities
* [oyX1]: revert icc_profile option omitting
* [oyX1]: request icc_profile.net_color_region_target
* [API]: correct oyCheckProfile/oyCheckProfileMem
* [exmpl]: keep context local in colour_desktop
* [exmpl]: check atom only for property events
* [alpha]: optimise cache searches add param to oyCacheListGetEntry_()
* [exmpl]: contrain caching in colour_desktop
* [exmpl]: use compiz private data caching
* [exmpl]: no warning for taking correctly ended session
* [build]: add oforms target to makefile.in
* [conf]: copy oforms makefile to build directory
* [build]: allow out of source build with oforms
* [oforms]: fix string passing to FLTK
* [alpha]: fix graph signaling bug
* [i18n]: fix typo
* [oydi]: reduce warning message to debug
* [build]: clean libXNVCtrl.a
* [alpha]: omit gv option to display graph
* [oicc]: favour DB values over build in defaults
* [oicc]: enable gamut warning from policy module
* [exmpl]: _NET_COLOR_DESKTOP only if ready
* [exmpl]: try to update a graph
* [alpha]: omit DB profiles from capable backends
* [alpha]: beautify profile check in oyDeviceAskProfile2
* [docu]: describe device profile functions
* [oyX1]: return issue on missed EDID info
* [oyX1]: ignore issues and report in oyranos-monitor
* [exmpl]: colour_desktop intermediate fix
* [alpha]: avoid unnecessary successive error
* [exmpl]: change debug level in colour_desktop
* [exmpl]: colour_desktop optimised shader generation
* [exmpl]: rename some variable in colour_desktop
* [exmpl]: colour_compiz ignore sRGB monitor profiles
* [exmpl]: ignore own profile changes in colour_desktop
* [oyX1]: switch XFixes property to network order
* [exmpl]: print nicer window name in xcmsevents
* [exmpl]: fix hang on compiz+colour_desktop start
* [exmpl]: comment on previous commit
* [exmpl]: more logic for shader cache
* [alpha]: add oyProfile_s::tags_modified_ member
* [alpha]: fix alter during deserialised of ICC profiles
* [test]: add profile tag non modification tracking
* [exmpl]: bugfixes for 0.4 DRAFT2 colour_desktop
* [oydi]: improve debug messages
* [exmpl]: change warn to info in image_display
* [alpha]: signals for changed options
* [exmpl]: fix compiler warning
* [oicc]: fix DB overwrites user options
* [test]: fix test2 "oyOption_s integers"
* [test]: show oyOption_SetFromxxx bug
* [oyX1]: support HDMI EDIDs
* [conf]: add libdir to static oyranos-config flags
* [exmpl]: fix colour_desktop resize bug
* [exmpl]: complete previous commit
* [spec]: fix spelling and rm packager tag
* [build]: add OpenMP tests to source package
* [build]: add missing files to source package
* [build]: fix rpmlint warnings
* [data]: fix default profile sizes
* [conf]: remove $ROOT_DIR reference in pkg-config
* [build]: add %dir to rpm spec
* [exmpl]: fix compiz-0.7.6 builds
* [conf]: detect cairo, compiz and Qt
* [alpha]: include locale.h for LC_NUMERIC
* [oform]: avoid cmd line initialisation
* [oforms]: correct select1 for cmd
* [build]: let move not fail
* [build]: use configures COPY command
* [core]: message array exceeding
* [core]: reset message string
* [oyIM]: omit zero strings
* [oyIM]: omit next empty string
* [oyIM]: correct mcro usage
* [exmpl]: correct compiz macro usage
* [exmpl]: correct link order
* [exmpl]: fix namespace compile error
* [core]: change namspace order
* [build]: allow examples to fail
* [conf]: add i18n libs to oyranos-config
* [build]: fix compiler warning
* [core]: fix inadequate pointer freeing
* [conf]: switch to LRaw-0.9
* [build]: correct link order for oforms
* [alpha]: do not touch user supplied pointer
* [test]: adjust integers tests
* [alpha]: ignore oyOption_SetFromInt/Double flags
* [alpha]: oyDeviceSelectSimiliar with full options
* [test]: split device listings
* [test]: add profile to "devices listing" test
* [CUPS]: check string release
* [conf]: update month
* [alpha]: check for mnft and model before DB query
* [build]: change PCS profile RPM revision number
* [exmpl]: corrections to net-color spec
* [build]: add examples file to source package
* [build]: add compiz config xml to source package
* [oyX1]: use global DEBUG macro only
* [build]: keep verbosity the same as in top dir
* [build]: adapt to elder Qt moc
* [lcms]: omit openMP trotling
* [build]: remove qt3 requirement for fedora RPMs
* [CUPS]: improve debug formatting
* [core]: support path in OpenICC path search
* [alpha]: fix object name duplication
* [build]: add libs to make fedora happy
* [build]: add fedora build flags for examples
* [fix]: RPM package groups
* [build]: change permissions on devel example
* [build]: fix RPM summary-not-capitalized
* [build]: remove main() from lib
* [oforms]: security fix
* [buidl]: fix RPM Group
* [alpha]: fix some printf format warnings
* [build]: correct link order for oforms
* [alpha]: fix more security errors
* [build]: fix missed link libs
* [build]: correct icc package groups
* [build]: sync with oscs RPM BuildRequires
* [build]: add RPM dirs to icc packages
* [build]: require dynamic core before modules
* [docu]: add Device Protocol 1.0
* [conf]: add debian packageing file
* [build]: add deb make target
* [core]: add backtrace call
* [exmpl]: fix compiler security warnings
* [alpha]: add oyCMMapi7_s::properties member
* [exmpl]: avoid ICC_PROFILE_IN_X atoms
* [oyra]: tell about PPM input module capabilities
* [core]: add headers for backtrace function
* [build]: fix shell syntax error
* [alpha]: fix false memory access
* [exmpl]: enrich debug infos in the compiz plugin
* [oyX1]: ignore empty profile
* [oyX1]: skip duplicate options
* [alpha]: fix zero oyOptions_s::oy_struct access
* [alpha]: add oyFilterRegistrationMatchKey
* [alpha]: fix profile copy check
* [alpha]: repair args fro profile copy
* [alpha]: add warning for non written file
* [alpha]: add warning for non accessible profile
* [core]: return always a file name
* [i18n]: update deutsch
* [alpha]: rename from libXcolor to libXcm
* [build]: update examples to libXcm
* [build]: improve deb description
* [build]: update to libXcm, rename to liboyranos0
* [build]: set correct soname
* [build]: rename devel RPM
* [build]: set RPM file ownership
* [exmpl]: add xcm flags
* [exmpl]: correct xcm flags
* [API]: remove namespace from headers
* [conf]: sync configure script
* [conf]: fix Elektra detection error message
* [core]: remove more namespace references
* [build]: do not remove build root by ourself
* [API]: increase OYRANOS_API to 16
* [build]: support parallel build for RPM
* [build]: move oyranos-config into liboyranos-devel
* [docu]: improve ChangeLog generation
* [build]: add more spec requirements
* [CUPS]: fix access of empty PPD
* [CUPS]: add PPD colourKeyWords support
* [core]: add 64x64 sized logo
* [alpha]: add common plugin message func
* [oyIM]: add meta tag reading
* [build]: reduce optimisation for examples
* [oyra]: omit unused variable
* [oyra]: generic image loading
* [exmpl]: use generic file loading
* [i18n]: actualise deutsch
* [oforms]: add -f option for front end options
* [i18n]: translate deutsch
* [alpha]: more argument checks for oyStruct_ObserverAdd
* [test]: fix wrong access
* [oyIM]: move oyFilterSocket_MatchImagingPlug out
* [core]: fix warning in oyICCTagTypeName()
* [test]: fix some possible crashes
* [alpha]: move oyConnector match checking
* [alpha]: support multiple meta backend ends
* [core]: support data paths
* [core]: fix suffix comparision
* [exmpl]: translate comment
* [oicc]: fix memory access after translation
* [alpha]: cut registration keys from first point
* [docu]: split alpha toppics off
* [conf]: add lcms2 test
* [build]: separate alpha documentation and RPM
* [ICC]: exchange eciRGB by LStar-RGB
* [ICC]: exchange the profiles for previous commit
* [build]: add missed alpha Doxyfile
* [docu]: update ChangeLog
* [oicc]: complete memory access fix
* [docu]: add last entries to ChangeLog
* [build]: package missed oforms file

###Soren Sandmann (1):
* [oyX1]: parse EDID colour primaries

###Yiannis Belias (7):
* [docu]: add Ubuntu package dependencies
* [docu]: mention more Ubuntu dependencies
* [fix]: possible string length for oyMessageFunc_
* [docu]: for last commit
* [alpha]: bug fix for string array option
* [alpha]: fix oyCMMptr_Set()
* [alpha]: fix for missing argument to strcmp

###kwizart (3):
* [fix]: man-1_3 patch
* [fix]: man-1_3 patch build part
* [docu]: convert ChangeLog to utf-8

#Version 0.1.9

###Kai-Uwe Behrmann (144):
* [doc]: minor changes \
* [doc]: minor changes \
* [alpha]: remove unused plugin APIs \
* [alpha]: move image data out of oyFilterNode_s; add profile to oyConversion_CreateBasic
* [alpha]: add oyStruct_s     * oy_struct to oyValue_u
* [alpha]: revert adding of ia profiles argument from oyConversion_CreateBasic
* [alpha]: oyModuleGetActual() selects from the oyFILTER_TYPE_e    type arg
* [alpha]: generalise oyColourConversion_GetID_ -> oyColourContextGetID_()
* [alpha]: header change to 70e80a4  oyModuleGetActual()s new arg type
* [alpha]: remove old oyModuleGetActual declaration
* [alpha]: split the ICC support code out to a new oyranos_icc.c
* [docu]: smaller doxygen output (git builds are broken since yesterday)
* [alpha]: complete 6e660e38 new oyranos_icc.c
* [alpha]: add a parant_type_ member to oyStructList_s
* [alpha]: add oyObject_Hashed_ internally
* [alpha]: add oyStructList_s::parent_type_ (see d9d58953fe)
* [alpha]: add oyValueTypeText() to output XFORM text
* [alpha]: move oyFilter_s::image_ -> oyFilterSocket_s::data
* [docu]: try to make the scheme graphis more clear
* [alpha]: add oyFilter_Node_s::backend_data to hold the CMM context
* [alpha]: add oyOption_GetText to serialise to XFORMS text
* [alpha]: add oyContextGetID_() to work on oyFilterNode_s
* [alpha]: add oyFilterNode_ContextSet_() and oyFilterNode_GetNextFromLinear_()
* [alpha]: corrects the c5a8c01dd patch applying
* [lcms]: get the image data correctly after the previous changes
* [conf]: switch to v0.1.9
* [alpha]: change CMM APIs to see oyFilterNode_s for context creation
* [conf]: let configure fail for missed libraries, provide --disable-libxxx
* [alpha]: fix a wrong access in the graph
* [test]: update to changed alpha API
* [alpha]: more fixes to run test.cs simple graph
* [alpha]: remove the oyFilter_s::profiles_ member
* [build]: beautify compiling by removing C++ comments from C sources
* [alpha]: move oyFilter_TextToInfo -> oyFilterNode_TextToInfo; add oyStructList_GetID
* [alpha]: adding oyFilterNode_DataGet_()
*[alpha]: new oyOptions_Find()
* [API]: fix defaults for internal profile settings
* [alpha]: remove one arg of oyOption_New()
* [API]: one more step for XML/Elektra namespace synchronisation
* [alpha]: remove oyOptions_s ranges; add source hint
* [alpha]: implement oyOption_FromStatic_()
* [alpha]: minor change
* [alpha]: add default settings as XFORMS to "oyra" oyranos_cmm_oyra_settings.c
* [docu]: update to new download www page
* [alpha]: oyOptions_FromDefaults() -> oyOptions_ForFilter() + oyOptions_FromText() + oyFilterRegistrationToType()
* [lcms]: add CMM specific options
* [docu]: formatting and minor tweaks
* [conf]: check for libxml2
* [build]: link against libxml2
* [and]: add internal oyXMLgetElement_() to parse xpath expressions
* [alpha]: complete oyOptions_FromText() oyOptions_FromBoolean() oyOptions_Add()
* [docu]: update to "XML Plug-in options" ideas on ColourWiki
* [alpha]: mv oyranos_cmm_oyra_settings.c to oyranos_cmm_oyra_icc.c
* [alpha]: add backend meta API oyCMMapi5_s
* [and]: remove unneeded XML levels from lcms options
* [and]: rearrange API pointers
* [and]: rearrange API pointers
* [API]: move root key name from "colour" to "shared"
* [docu]: formatting
* [alpha]: add oyCMMMetaGetApi_ to access oyCMMapi5_s
* [and]: polish oyOptions_FromBoolean() and oyOption_GetText()
* [fix]: oyGetDefaultProfileName() for empty Elektra key
* [build]: set libxml2 include options
* [alpha]: new oyOption_SetFromText() and oyOptions_GetText()
* [docu]: correct text info for oyranos-monitor --help
* [and]: allow for roundtripping oyOptions_s <-> XML
* [alpha]: remove oyOption_s::name member and merge with registration
* [API]: OY_TYPE_STD  "default" "colour"
* [and]: update to last Elektra namespace proposal on ColourWiki
* [alpha]: sync between oyOPTIONSOURCE_e and oyOPTIONDEFAULTS_e
* [alpha]: remove "colour_icc"/oyFILTER_TYPE_COLOUR_ICC registration type
* [and]: oyOptions_Add() does not doublicate options in a set
* [alpha]: add oyOPTIONATTRIBUTE_FRONT/"front" to the registration option level
* [fix]: missed allocation of ::value in oyOption_Copy_()
* [alpha]: split oyFilter_New() in smaller internal components
* [alpha]: allow for a non binary answeres for boolean tests
* [alpha]: add oyStructList_Clear(), oyStructList_CopyFrom()
* [alpha]: add oyOptions_DoFilter()
* [alpha]: priorise "shared" in oyOptions_Add()
* [alpha]: add oyOptions_SetFromText()
* [lcms]: mark one option as advanced
* [alpha]: shuffle oyOPTIONSOURCE_e and oyOPTIONATTRIBUTE_e enums
* [alpha]: polish the Elektra namespace implementation
* [build]: make synonym to docu
* [docu]: typo
* [alpha]: allow for obtaining the meta and filter options separately
* [conf]: add xml2 to pkg-config and oyranos-config tools
* [build]: fix configuration for test2 target
* [conf]: add xml2 to pkg-config tool
* [and]: wrap standard string functions into Oyranos macros
* [and]: undefine default DEBUG in oyranos_debug.h
* [fix]: warning about unknown std. profile type in oyProfiles_FromStd()
* [and]: fix a compile warning
* [fix]: include missed profiles for oyProfile_ForStd()
* [build]: fix [ 2249349 ] configure failed
* [alpha]: add rendering_gamut_warning option
* [alpha]: move options handling into the filter for oyCMMapi1_s
* [API]: export oy_debug to provide a grip for external message functions
* [and]: add function call level intending again to debug output
* [and]: add \_\_func\_\_ info to debug output for \_\_GNUC\_\_
* [and]: remove OYRANOS_DEBUG checking
* [and]: rework the debug system
* [alpha]: add oyOption_GetValueText()
* [and]: add oyCloseReal\_\_ to close Elektra
* [and]: add oyExportReset_()
* [test]: start with testing in Oyranos
* [build]: make check target points to tests/test_oyranos
* [docu]: formatting
* [and]: fix for oyCloseReal\_\_()
* [alpha]: update oyOption_Match_() to suit current registration definition
* [test]: add profile reading and monitor profiles tests
* [build]: require a partitial install to satisfy a make check
* [build]: FreeBSD RM
* [build]: show git sha1 of git master
* [conf]: explicitely provide commands to sub directory makefiles
* [test]: allow "make check" for non git builds
* [conf]: remove GNU "echo -e" extension
* [test]: avoid redundant libxml2 call
* [build]: remove GNU "echo -e" extension
* [conf]: remove directory name guessing
* [test]: precise profile count fail
* [build]: move FLTK_H flags out of general C compiling
* [conf]: use current compiler for test
* [build]: couple make install_bin with uninstall_bin
* [alpha]: add oyCMMlibMatchesCMM()
* [alpha]: rename oyCMMptr_s::cmm to oyCMMptr_s::lib_name and update APIs
* [alpha]: rename oyFilter_s::cmm\_ to oyFilter_s::lib_name\_
* [test]: allow filtering
* [fix]: test for X11 connection during communication
* [build]: fix test_oyranos app building on Solaris
* [conf]: set "rm" for osX builds
* [and]: more warnings in oyProfile_FromMemMove_()
* [fix]: SunC byte order macro bug
* [and]: fix SunC compile warning
* [docu]: update ChangeLog
* [docu]: update man pages
* [fix]: mem bug for too long messages
* [build]: allow DESTDIR variable with spaces
* [and]: adjust debug output
* [build]: fix installation to DESTDIR with empty spaces
* [build]: BUILDDIR changes
* [alpha]: add full qualified librarynames to oyCMMsGetLibNames_
* [build]: smaller compile messages
* [build]: repair trivial message


#Version 0.1.8

###Inaki Larranaga Murgoitio (1):
* [po]: Basque translations from ICC Examin

###Kai-Uwe Behrmann (249):
* [conf]: detect distribution on Linux; correct comment
* [alpha]: tighten the oyCMMapi3_s profile tag API
* [oyra]: pseq tag for device link profiles
* [API]: support custom messaging with oyMessageFuncSet
* [build]: move -L./ in front to link local libraries
* [conf]: remove local elektra configuring
* [conf]: remove configure output
* [conf]: fix Elektra version check
* [and]: fl_i18n updates from ICC Examin
* [conf]: set compiling to verbose by default
* [build]: omit liboyranos_alpha and all local elektra targets
* [fix]: compile fix
* [fix]: fix for missed display/screen name
* [conf]: correct configure and remove old stuff in make
* [and]: osX debug system adaptions
* [conf]: more osX configure verbosity
* [and]: cleaning up
* [API]: remove "[none]" dummy profile from oyProfileListGet_ output
* [API]: switch back to naming first intent perceptual
* [oyra]: support icSigWCSProfileTag type
* [API]: continue even with missed Elektra handle
* [i18n]: unicode improvements
* [i18n]: remove wchar_t as it is wrong interface for UTF-16
* [alpha]: declare a point access handler (still needs implementation)
* [alpha]: move some CMM specific declarations
* [i18n]: replace sprintf with memcpy to avoid crashes on certain characters in wcslen
* [API]: move some specific functions to the icc header
* [and]: avoid some valgrind warnings
* [API]: fit to our function naming convention
* [API]: fit to our function naming convention
* [API]: replace parsed UI exchange with XML serialised
* [i18n]: fix codeset for ICC mluc
* [and]: remove lots of outdated functions and snipets
* [and]: avoid printf with filenames
* [fix]: fix iconverted string too short on output
* [fix]: fix freeing of already freed pointer bug
* [and]: remove unneeded type
* [alpha]: oyProfile_GetText can get normal icSigProfileDescriptionTag
* [and]: update
* [alpha]: more work on the filter APIs
* [and]: fix lcmsCMMColourConversion_FromMem
* [and]: play with oyCMMapi4_s filter API
* [alpha]: more work on filters
* [alpha]: integrate oyImageHandler_s into oyImage_s
* [alpha]: implement first oyConversions_s functions
* [and]: update
* [alpha]: implement more of oyValue_u, oyOption_s, oyOptions_s and oyFilter_s APIs
* [and]: correct string splitting
* [alpha]: propose registration scheme for plug-ins : with input from Markus Raabe
* [alpha]: implementation around the plugin registration
* [and]: update
* [build]: static linking with elektra
* [alpha]: declare and implement oyOptions_FromBoolean
* [alpha]: filter options in oyra
* [alpha]: declare image-root filter correctly
* [alpha]: more thoughts on resource handling and pixel access
* [and]: hide path manipulators
* [and]: remove static policy entries, as they get overwritten during runtime
* [API]: change typedefed funtion suffixes
* [API]: OYRANOS_VERSION_NAME becomes a string
* [alpha]: generalise nodes by oyNode_s
* [API]: change typedefed funtion suffixes
* [oyra]: implement more of a Image/Root filter
* [test]: test pixel access
* [API]: remove static policy names; they are detected dynamically by the files
* [alpha]: rename oyProfileList_s to oyProfiles_s
* [API]: make oyMessage_f() oyStruct_s context aware
* [alpha]: allow casting of oyFilterNode_s to oyNode_s
* [fix]: properly initialise pointer; fixes a crash in oyranos-policy
* [and]: cleaner output, direct error output to stderr
* [and]: set correctly to parent and children nodes
* [alpha]: let oyFilter_s::image_ be refered always from main parent
* [test]: small test cases for cmm_api4
* [alpha]: add "org.oyranos.image.image.output" filter and oyraFilter_ImageOutputGetNext
* [API]: remove path manipulation APIs
* [fix]: fix NULL pointer access crash for kminfo
* [and]: remove duplicated message
* [and]: more Options related error checking
* [and]: provide platform identifers
* [and]: remove path manipulator functions
* [fix]: convince nl_langinfo API
* [alpha]: export oyProfile_Equal()
* [alpha]: export oyraIconv nach oyIconv()
* [and]: make oyranos locale aware in oyranos-config-fltk
* [and]: make Oyranos locale aware for Elektra
* [API]: oyPoliciesEqual()/oyPolicyNameGet_() is now based on key values
* [alpha]: more filter backend API work
* [and]: update
* [and]: allow for recursive directory writing
* [conf]: provide more configure time details
* [API]: new oyPolicySaveActual() API
* [doc]: extended options set and documentation
* [i18n]: i18n update
* [API]: enshure only the first path from xdg_config_home is used in oyPolicySaveActual()
* [API]: add useful defaults to oyXDGPathsGet_()
* [and]: change options and describe
* [fix]: fix empty string crash
* [fix]: fix empty string crash
* [and]: convert from old message function
* [and]: integrate new oyProfiles_ForStd() API into oyOptionChoicesGet()
* [doc]: update docu
* [doc]: add internal documentation formatter
* [doc]: add a code snippet to oyProfiles_ForStd() function docu
* [conf]: clean a sub directory before configuring
* [doc]: clean and add to documentation
* [alpha]: the widget enums are removed + a a bug fix for better pixel channel detection
* [and]: update
* [alpha]: introducing a oyConnector_s to oyFilterNodes_s
* [doc]: slight documentaion formatting
* [alpha]: add oyDEFAULT_PROFILE_START as argument for oyProfiles_ForStd()
* [doc]: add license infos
* [alpha]: add oyArray2d_s struct and use it in oyImage_s
* [doc]: add more documentation and remove Concept page from code docu
* [alpha]: simplify the internal structure dependencies
* [fix]: fix to [ 1955643 ] oyranos-monitor and xinerama VCGT second monitor not working
* [and]: update
* [and]: fix date macros in configure
* [and]: add filename and line to a command line printf
* [API]: remove unneeded path function
* [and]: add a strzct type to oyRecursivePaths_ callback
* [fix]: repair configure script as suggested by Milan Knízek
* [and]: rename internal oyImage_CombinePixelLayout2Mask_
* [and]: set correct data type for test
* [and]: turn configure date variables to numbers
* [build]: write library path into library for osX
* [API]: rename oyGetMonitorProfileName to oyGetMonitorProfileNameFromDB
* [cli]: add monitor profile querying to oyranos-monitor and document in the man page
* [fix]: fix multiple counting in FLTK choice
* [new]: Oyranos/Cairo/RawPhoto coding example
* [new]: integrate the new example into makefile.in
* [API]: publish the include directory where Oyranos headers are placed
* [doc]: add a nice frame around images
* [doc]: better code arrangement and 16-bit path until Cairo
* [build]: minor variable name correction for make
* [and]: update to Elektra v0.7.rc5
* [and]: omitt irrelevant elektra messages
* [fix]: correct DBG macro usage in oyranos_monitor_nvidia.c
* [pack]: split display into monitor and monitor-nvidia, rename panel to ui-fltk
* [and]: prepare for a removed kdbStrError()
* [and]: remove kdbStrError now
* [build]: slightly more verbose end of make
* [build]: small make fix
* [doc]: [graph] lots of smaller changes, more documenting and formatting, test update
* [and]: oyConversion_CreateBasic() is functional
* [alpha]: oyConversion_Init removed
* [doc]: oyConversion_CreateBasic is demonstrated
* [build]: split oyranos_cmm_oyra.c into several files
* [alpha]: add oyOptions_HasString() validation query
* [alpha]: rename to oyOptions_FindString()
* [alpha]: formulate more about default behaviour options handling
* [and]: update to svn and remove deprecated Elektra error API usage
* [and]: more config keys thinking
* [and]: more config key thinking
* [alpha]: apply suggested namespace scheme
* [alpha]: namespace change applied to parsing
* [doc]: describe debian dependencies : thanks to Markus Raabe
* [alpha]: update the plug-ins to the key namespace changes
* [build]: remove CMMs during make uninstall
* [fix]: oyActivateMonitorProfile() does only fill a empty server configuration
* [po]: remove incomplete translation
* [po]: update translations
* [po]: some esperanto translations
* [fix]: fix for the elektra-0.7.0 release
* [and]: update
* [and]: update
* [prof]: add default profiles
* [pack]: correct documentation packaging
* [prof]: add missing profile
* [doc]: documentation
* [doc]: add plain text intro to alpha documentation
* [and]: clean some entries
* [build]: be more verbose during make
* [pack]: straighten the ICC profile RPM names
* [cli]: add -p option and document
* [fix]: fix some overruns
* [po]: update and translate de
* [doc]: sort documentation
* [and]: minor tweaks
* [conf]: keep config.log during configuring in tact
* [doc]: add a comment to oyPolicySaveActual()
* [doc]: more documentation cleaning
* [doc]: fix doxygen warnings
* [doc]: merge alpha documentaion
* [doc]: merge alpha documentation
* [alpha]: monitor API is available as plug-in
* [build]: remove references for direct linking to liboyranos_monitor
* [conf]: the configure flag --enable-verbose is not required for make rpm
* [pack]: remove the alpha-devel RPM package as we integrated the documentation
* [doc]: correct image object documentation references
* [API]: remove oyranos_monitor.h from installed files
* [conf]: sync configure.sh with ICC Examin
* [and]: fix complile warning
* [and]: oyranos-monitor seems broken
* [and]: some fixes for the monitor backend
* [and]: additional monitor API bugfixes
* [and]: remove high level configuration checks for FLTK and Elektra
* [doc]: correct link to be named ColourWiki in documentation
* [build]: let make react more sensible to changes in configuration files
* [test]: improve the test case
* [and]: fix enum missmatches pointed out on Solaris
* [and]: fix enum missmatches pointed out on Solaris
* [conf]: fix config.log script for BSD shells
* [conf]: more fixes for BSD shell scripts
* [and]: fix enum missmatch warning
* [and]: fix enum mismatch
* [and]: test macro enum mismatch warning
* [and]: add missing errno.h include in oyranos_elektra.c
* [build]: fix for building on osX
* [and]: fix warning on osX and Solaris about const missing for a argument
* [conf]: avoid rpath for non linux builds
* [build]: complet clean in examples directory
* [conf]: test for dso name linker flags
* [conf]: verbose dso link name option during configure
* [and]: fix
* [conf]: add fallback for dso link name option
* [conf]: add test file
* [build]: fix solaris link error
* [build]: fix no getext compiling
* [build]: fix for non gettext build in oyranos_alpha.c
* [and]: take posix definition of iconv()
* [build]: remove GCC specifix -mimpure-text option
* [build]: add LCMS_H include path to lcms module; fixes build error
* [build]: add LCMS_H include path to general includes
* [and]: fix SunC compiler warnings
* [and]: fix SunC compiler warnings
* [and]: fix non used statement for options parsing
* [and]: remove unused statments in oyranos_alpha.c
* [fix]: fix some enum missmatch errors in oyranos_alpha.c
* [conf]: add intptr test to configure
* [conf]: add intptr test file
* [conf]: detect compiler settings for 32/64-bit builds suring configure
* [build]: let the make install target not fail due to missing profiles
* [doc]: give credit to translators and contributors of data
* [doc]: correct mentioning of licenses : zlib
* [doc]: some hints for ChangeLogs
* [doc]: update ChangeLog


###Milan Knízek (3):
* [po]: add czesh translation
* [po]: more translation
* [po]: more translations

###Pascal LACROIX (1):
* [po]: french translations from ICC Examin

###2007-07-18
* and: sensible for non EDID monitors, new wighting by display geometry
* cli: add -v for oyranos-monitor, oyranos_monitor_nvidia.c, oyranos_policy.c

###2007-07-17
* and: include MMs Flmm_Tabs for the config panel
* i18n: FLTK utf-8 aware config panel, set locale accordingly

###2007-07-03
* API: move oyInPlaning section from oyranos.h to oyranos_internal.h
* API: start oyranos_icc.h API thinking, like oyValueUInt16
* API: ship icc34.h for oyranos_icc.h

###2007-07-01
* conf: custom fltk-config; FLTK utf-8 detection

###2007-06-14
* fix: CFLAGS and the like variables are not multiplied anymore
* conf: support configure option: --disable-dependency-tracking

###2007-05-20.
* and: default settings are not set at installation, they are now inside
* new: Apple specific code to get profile paths and default profiles
* API: new oyPolicySet function : Load a group of policy settings from file.
* and: new oyCheckPolicy_ internal function : test for OY_POLICY_HEADER
* and: adapt oyranos-config-fltk to the new policy API - oyWIDGET_POLICY
* arg: oyWidgetTitleGet_ argument flags contains oyLAYOUT_NO_CHOICES
* and: oyranos-config-fltk now updates settings instead of recontructing
* macro: OY_DEFAULT_EDITING_GRAY_PROFILE OY_DEFAULT_ASSUMED_GRAY_PROFILE
* enum: oyGROUP_EXTERN moved from 100 -> 200
* enum: new assignments to oyWIDGET_TYPE and oyDEFAULT_PROFILE
* i18n: map C string functions with oy internal macros
* prof: add Gray.icc
* and: move declarations out of individual headers to oyranos_internal.h
* and: gerneralise oyRecursivePaths_ search API (profiles + settings)

###2007-05-17.
* enum: new options: oyWIDGET_POLICY oyWIDGET_PATHS
* enum: new suboptions: oyWIDGET_EDITING_GRAY oyWIDGET_ASSUMED_GRAY

###2007-05-14.
* conf: support configure option: --with-x (for osX)
* new: support osX first display profile

#Version 0.1.7

###2007-04-02
* new: distclean make target

###2007-03-28
* new: inbuild default options as fall back to Home+Office policy
* and: change usual macro names to specific OY_ ones to avoid conflicts
* and: avoid some warnings during the monitor detection
* fix: bugfix for wrong named XYZ key, oyASSUMED_CMYK -> oyASSUMED_XYZ

###2007-03-25
* new: install oyranos-kdb_static as debug tool for Oyranos keys

###2007-03-15
* and: create and manage profiles RPMs from the top directory
* and: more fine grained configuration
* and: better debug builds and warnings
* fix: fix many compiler warnings
* and: continue a compilation to the extent what is possible

###2007-02-20
* new: man pages
* new: oyGetDisplayNameFromPosition, convenient oyGetScreenFromPosition
* fix: position handling in oyranos-monitor

###2007-02-06
* new: config check for existence of more libraries (needed for NetBSD)
* new: gettext is not assumed but checked in configure
* new: add fltk-config --post for osX
* fix: locale handling in oyranos-config-fltk for osX

###2007-02-01
* fix: remove all alloca calls for NetBSD (alloca is not in Posix)

###2007-02-02
* add: Nvidia in authors list for the included libXNVCtrl

###2007-01-08
* new: add version info to library
* and: change md5 symbol names to avoid possible name collisions

###2006-12-22
* conf: add --with-fltk-config= and --enable-debug configure switch

###2006-11-17
* new: md5 support by L. Peter Deutschs md5 (sf.net: 2002-04-13 09:00)

###2006-11-03
* fix: add static liboyranos_elektra.a to oyranos-config

###2006-11-02
* new: config.site support for CC CXX CFLAGS CXXFLAGS LDFLAGS variables
* fix: Solaris build - shell syntax
* new: include libXNVCtrl in configuration

###2006-10-26
* neu: include Elektra 0.6.4 for QA (--internal-elektra=no to disable)

###2006-09-27
* fix: build issues on OpenBSD
* fix: for missed code after label

###2006-09-22
* and: more precise warning texts
* and: update de.po
* fix: call of oyranos-monitor-nvidia for non edid atom

###2006-09-21
* fix: bug in oyExportStart_
* and: add install path to oyranos-monitor-nvidia call in oy_moni.c
* new: filter double profile occurencies in oyOptionChoicesGet_()

###2006-09-20
* new: first external functions call for init (oyExportStart_(EXPORT_*))
* fix: double and default paths setting in oyPathAdd_
* and: oyranos-config : devel.rpm -> standard.rpm

###2006-09-19
* and: include body tag for xml settings export
* and: link to setting from behaviour documentation
* and: put many warnings into recursive profile search code
* add: a extensive profile path recursion test to test2.cpp

###2006-09-18
* and: include nv files into build system
* and: handle Elektra more carefully during configuration
* and: document oyranos-policy commandline tool

###2006-09-17
* and: changed title for available languages
* and: activate commandline tools i18n
* and: update tools documentation

###2006-09-16
* fix: set default policy on install time if no config keys are found

###2006-09-15
* fix: wrong setlocale settings from library code
* and: add a argument to fl_initialise_locale wether to use setlocale

###2006-09-14
* new: oyranos-policy commandline tool to set and read a policy

###2006-08-11
* and: update oyranos_config_fltk.fl

###2006-08-10
* new: oyWidgetListGet

###2006-08-06
* and: renaming most of the GUI APIs
* and: oyOptionUITitleGet => oyWidgetTitleGet
* and: oyOPTION... => oyWIDGET...

###2006-08-02
* and: modified a string in groups and updated the settings xmls

###2006-07-29
* and: fixed fl_i18n for destroying locale settings

###2006-07-28
* and: do a german translation

###2006-07-26
* new: parse xml for pot file generation
* new: install doxygen files
* and: adapt oyranos.spec for doc directories
* new: install cmm description xml files in /usr/share/color/cmms

###2006-07-19
* and: update documentation
* and: rename some internal types in oyranos_monitor_internal.h

###2006-07-10
* new: CMM API / XML parsen
* and: internal dynamic data handling

###2006-07-09
* new: add compatibleWithAdobeRGB1998.icc from Graeme Gill
* chn: move oyranos-sRGB to oyranos-RGB for inclusion of above profile

###2006-07-06
* new: add Lab.icc and XYZ.icc symlinks; update settings accordingly

###2006-06-22
* fix: more Xinerama / screen bugs
* fix: turn my edid hack into programing
* new: better parse edid

###2006-06-18
* fix: Xinerama / screen bugs
* neu: make oyranos_monitor_nvidia.c Xinerama aware
* und: rename oyActivateMonitorProfile to oyActivateMonitorProfiles
* und: internal rename oy_display_s to oy_monitor_s

###2006-06-07
* und: version 0.1.6
* neu: rearrange ARCH in configure

###2006-06-02
* und: remove oyGetBehaviourUITitle

###2006-05-28
* und: release of 0.1.5
* neu: start bsd support
* und: change build system
* neu: FLTK UI
* conf: allow system specific colour directories at configure time
* und: USERCOLORDIR SYSCOLORDIR ICCDIRNAME SETTINSDIRNAME in config.h
* und: the above macros are additional to /usr/share/color ~/.color
* neu: in SETTINSDIRNAME Oyranos policies are installed
* neu: elekra-0.6
* neu: multi monitor detection and handling
* und: changed Behaviour and GUI APIs
* neu: Solaris support

###2006-04-03
* und: release of 0.1.4

###2006-02-08
* neu: oyReadXMLPolicy_

###2006-02-07
* neu: oyPolicyToXML

###2005-11-22
* neu: Optionen für GrundVerhalten
* und: oyranos-config-flu wird nicht mehr statisch gelinkt

###2005-11-21
* und: Wiki aktualisieren
* neu: statisches OY_DEFAULT_ASSUMED_WEB_PROFILE
* neu: Integration der standard_profiles in das Übersetzungssystem
* neu: oyranos.pc und oyranos_monitor.pc

###2005-10-20
* neu: elektra-0.5.1
* neu: rpm + devel rpm

###2005-10-18
* neu: elektra-0.5.0.5
* und: release prepare

###2005-09-13
* fix: 0 EDID atom crash (no monitor attached)

###2005-09-07
* neu: Benutzer und System Einstellungen werden berücksichtigt
* neu: oyOpen oyClose sind überflüssig

###2005-09-06
* neu: rekursive Dateisuche; Link Erkennen weiter schwierig
* und: oyActivateMonitor vereinfacht
* und: statisches und dynamische Verknüpfen mit Elektra

###2005-09-05
* neu: DefaultProfile Funktionen umgeschrieben
* neu: oyDEFAULT_PROFILE zum Bezeichnen der Standardprofile
* und: Quelltextdokumentation

###2005-09-04
* neu: oyAllocFunc_t für benutzereigene Speicherreservierungen
* und: Quelltextdokumentation

###2005-09-03
* neu: Quelltextdokumentation

###2005-08-31
* und: elektra-0.5.0.4
* fix: linking libelektra_default.a 

###2005-06-30
* neu: _ICC_PROFILE von Ross Burton <http://www.burtonini.com/computing/x-icc-profiles-spec-0.1.html>

###2005-06-29
* fix: oyComp Funktionen

###2005-06-28
* und: elektra-0.5

###2005-05-26
* fix: free Keyset with kdb not free

###2005-05-09
* und: rename all colour space functions to include Input except workspace
* und: update README

###2005-02-03
* neu: Monitorprofil setzen und lesen
* neu: oyranos-gamma lädt ICC vcgt tag in Grafikkarte

###2005-02-01
* neu: Monitor Erkennen

###2004-12-7
* neu: Speicherblöcke mit Profildaten abfragen
* allgm: gcc 3.4

###2004-11-30
* neu: interne und externe API scheiden
* allgm: einige Funktionen kompakter
* neu: liboyranos Erstellen und Einbinden

###2004-11-29
* neu: DEBUG makros
* neu: Datei Suche in Pfadliste
* neu: Profilprüfung - Signatur
* allg: Bereinigen doppelter Texte
* neu: Datei und Verzeichnis Tests

###2004-11-27
* info: Anfrage Oyranos auf freedesktop.org zu hosten
* info: Artikel auf behrmann.info
* info: diskussion mit elektra liste

###2004-11-26
* neu: Pfade scheinen stabil konfigurierbar
* info: oyranos.org und oyranos.com angemeldet

###2004-11-25
* Start des Projektes
* neu: Konzeption
* neu: Spielen mit Elektra
* neu: test
