/** @internal
 +  @file oyranos_helper_macros_cli.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    helper macros
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/11/16
 *
 */


#ifndef OYRANOS_HELPER_MACROS_CLI_H
#define OYRANOS_HELPER_MACROS_CLI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
                        { oyjlStringToDouble(argv[pos+1], &opt); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { oyjlStringToDouble(&argv[pos][i+strlen(arg)+2], &opt); \
                          i = 1000; \
                        } else \
                        { char * t = 0; const char * slash = strlen(arg)==1?"-":"--"; \
                          oyStringAddPrintf_( &t, 0,0, "%s%s (%s%s %g)", slash,arg, slash,arg, std );\
                          wrong_arg = t; \
                          i = 1000; \
                        } \
                        if(min > opt || opt > max) \
                        { char * t = 0; const char * slash = strlen(arg)==1?"-":"--"; \
                          oyStringAddPrintf_( &t, 0,0, "%s%s=%g %s %s=%g %s=%g", slash,arg,opt, _("value out of range"), _("minimal"), (double)min, _("maximal"), (double)max ); \
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


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_MACROS_CLI_H */
