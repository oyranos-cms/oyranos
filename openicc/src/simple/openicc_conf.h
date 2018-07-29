#ifndef OPENICC_CONF_H
#define OPENICC_CONF_H

#define UNIX 
/* #undef APPLE */
/* #undef BSD */
/* #undef _WIN32 */
/* #undef HAVE_OPENMP */
#define HAVE_C
/* #undef HAVE_LTDL */
/* #undef HAVE_DL */
#define HAVE_M
#define HAVE_YAJL
/* #undef HAVE_LCMS */
/* #undef HAVE_LCMS2 */
#define USE_GETTEXT
#define HAVE_DIRENT_H
#define HAVE_LANGINFO_H
#define HAVE_LOCALE_H
#define HAVE_INTTYPES_H
#define HAVE_STDINT_H
#ifndef DEBUG 
#define DEBUG 1
#endif

#endif /* OPENICC_CONF_H */
