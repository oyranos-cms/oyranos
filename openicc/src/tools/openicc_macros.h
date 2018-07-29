/** openicc_macros.h
 *
 *  @par License: 
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             (c) 2011-2017 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  @brief    helper macros
 */

#ifndef OPENICC_MACROS_H
#define OPENICC_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(USE_GETTEXT) && !defined(_)
#define _(text) text
#endif

#include "openicc_version.h"

extern openiccMessage_f     openiccMessage_p;

#define DBG(context, format_,...) openiccMessage_p( openiccMSG_DBG, context, \
                                                format_, __VA_ARGS__)
#define WARN(context, format_,...) openiccMessage_p( openiccMSG_WARN, context, \
                                                format_, __VA_ARGS__)
#define ERR(context, format_,...) openiccMessage_p( openiccMSG_ERROR, context, \
                                                format_, __VA_ARGS__)


/* command line parsing macros */
/* allow "--opt val" and "--opt=val" syntax */
#define OY_PARSE_INT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atoi( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atoi( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) fprintf(stderr,#opt "=%d\n",opt)
#define OY_PARSE_INT_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = atoi(argv[pos+1]); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = atoi(&argv[pos][i+strlen(arg)+2]); \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) fprintf(stderr,arg "=%d\n",opt)
#define OY_PARSE_FLOAT_ARG2( opt, arg, min, max, std ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { oyStringToDouble(argv[pos+1], &opt); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { oyStringToDouble(&argv[pos][i+strlen(arg)+2], &opt); \
                          i = 1000; \
                        } else \
                        { char * t = 0; const char * slash = strlen(arg)==1?"-":"--"; \
                          oyStringAddPrintf_( &t, 0,0, "%s%s (%s%s %g)", slash,arg, slash,arg, std );\
                          wrong_arg = t; \
                          i = 1000; \
                        } \
                        if(min > opt || opt > max) \
                        { char * t = 0; const char * slash = strlen(arg)==1?"-":"--"; \
                          oyStringAddPrintf_( &t, 0,0, "%s%s=%g %s %s=%g %s=%g", slash,arg,opt, _("value out of range"), _("minimal"), min, _("maximal"), max ); \
                          wrong_arg = t; \
                          i = 1000; \
                        } \
                        if(oy_debug) fprintf(stderr,"%s=%g\n", arg, opt)
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1]?argv[pos+1]:"1","0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2]?&argv[pos][i+2]:"1","0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) fprintf(stderr, #opt "=%s\n",opt)
#define OY_PARSE_STRING_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = &argv[pos][i+strlen(arg)+2]; \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) fprintf(stderr,arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        ((strlen(argv[pos])-2 == strlen(arg) || \
                          (strlen(argv[pos])-2 > strlen(arg) && \
                           argv[pos][strlen(arg)+2] == '=')) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)

#define verbose oy_debug

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OPENICC_MACROS_H */
