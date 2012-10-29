/** @internal
 +  @file oyranos_helper_macros_cli.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    helper macros
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
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
                        if(oy_debug) printf(#opt "=%d\n",opt)
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
#define OY_PARSE_FLOAT_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = atof( argv[pos+1] ); \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = atof( &argv[pos][i+2] ); \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
                            wrong_arg = "-" #opt; \
                          i = 1000; \
                        } else wrong_arg = "-" #opt; \
                        if(oy_debug) printf(#opt "=%g\n",opt)
#define OY_PARSE_FLOAT_ARG2( opt, arg ) \
                        if( pos + 1 < argc && argv[pos][i+strlen(arg)+1] == 0 ) \
                        { opt = atof(argv[pos+1]); \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+strlen(arg)+1] == '=') \
                        { opt = atof(&argv[pos][i+strlen(arg)+2]); \
                          i = 1000; \
                        } else wrong_arg = "-" arg; \
                        if(oy_debug) fprintf(stderr,arg "=%g\n",opt)
#define OY_PARSE_STRING_ARG( opt ) \
                        if( pos + 1 < argc && argv[pos][i+1] == 0 ) \
                        { opt = argv[pos+1]; \
                          if( opt == 0 && strcmp(argv[pos+1],"0") ) \
                            wrong_arg = "-" #opt; \
                          ++pos; \
                          i = 1000; \
                        } else if(argv[pos][i+1] == '=') \
                        { opt = &argv[pos][i+2]; \
                          if( opt == 0 && strcmp(&argv[pos][i+2],"0") ) \
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
                        if(oy_debug) printf(arg "=%s\n",opt)
#define OY_IS_ARG( arg ) \
                        ((strlen(argv[pos])-2 == strlen(arg) || \
                          (strlen(argv[pos])-2 > strlen(arg) && \
                           argv[pos][strlen(arg)+2] == '=')) && \
                         memcmp(&argv[pos][2],arg, strlen(arg)) == 0)


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_HELPER_MACROS_CLI_H */
