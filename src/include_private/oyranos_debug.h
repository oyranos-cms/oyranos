/** @internal
 *  @file oyranos_debug.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    internal helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/31/01
 *
 *  Do not use in non Oyranos projects.
 */




#ifndef OYRANOS_DEBUG_H
#define OYRANOS_DEBUG_H


#include <string.h>
#include <time.h>

#include "oyranos_config_internal.h"
#include <oyjl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @internal
 *  debugging variable - set 0 to off (default), set 1 to switch debugging on */
extern clock_t oyranos_clock_;
extern int level_PROG;
extern int oy_debug_write_id;

double             oyClock           ( );
double             oySeconds         ( );
time_t             oyTime            ( );
void               oySplitHour       ( double              hours,
                                       int               * hour,
                                       int               * minute,
                                       int               * second );
double   oyGetCurrentLocalHour       ( double              time,
                                       int                 gmt_diff_sec );
double   oyGetCurrentGMTHour         ( int               * gmt_to_local_time_diff_sec );
const char *       oyPrintTime       ( );

#if defined(__GNUC__)
# define  OY_DBG_FORMAT_ "%s "
# define  OY_DBG_ARGS_   oyjlFunctionPrint( __func__, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__ )
#else
# define  OY_DBG_FORMAT_ "%s:%d "
# define  OY_DBG_ARGS_   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif

/* Debug system */

#define DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC

#if DEBUG == 1
#define DBG oyMessageFunc_p(oyMSG_DBG,0,OY_DBG_FORMAT_, OY_DBG_ARGS_);
#define DBG_S(txt) oyMessageFunc_p(oyMSG_DBG,0,OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,txt);
#define DBG1_S(format,arg) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg);
#define DBG2_S(format,arg,arg2) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg,arg2);
#define DBG3_S(format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg,arg2,arg3);
#define DBG4_S(format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4);
#define DBG5_S(format,arg,arg2,arg3,arg4,arg5) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5);
#define DBG6_S(format,arg,arg2,arg3,arg4,arg5,arg6) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6);
#define DBG7_S(format,arg,arg2,arg3,arg4,arg5,arg6,arg7) oyMessageFunc_p( oyMSG_DBG,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7);
#define DBG_V(var) oyMessageFunc_p(oyMSG_DBG,0,OY_DBG_FORMAT_ #var ": %d", OY_DBG_ARGS_,(int)var);
#define DBG_START {++level_PROG; oyMessageFunc_p( oyMSG_DBG,0,"Start: " OY_DBG_FORMAT_, OY_DBG_ARGS_ );}
#define DBG_ENDE  {oyMessageFunc_p( oyMSG_DBG,0,"  End: " OY_DBG_FORMAT_, OY_DBG_ARGS_ ); --level_PROG;}
#define DBGs(s) oyMessageFunc_p(oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_, OY_DBG_ARGS_);
#define DBGs_S(s,txt) oyMessageFunc_p(oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,txt);
#define DBGs1_S(s,format,arg) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg);
#define DBGs2_S(s,format,arg,arg2) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg,arg2);
#define DBGs3_S(s,format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format,OY_DBG_ARGS_,arg,arg2,arg3);
#define DBGs4_S(s,format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4);
#define DBGs5_S(s,format,arg,arg2,arg3,arg4,arg5) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5);
#define DBGs6_S(s,format,arg,arg2,arg3,arg4,arg5,arg6) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6);
#define DBGs7_S(s,format,arg,arg2,arg3,arg4,arg5,arg6,arg7) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7);
#define DBGs8_S(s,format,arg,arg2,arg3,arg4,arg5,arg6,arg7,arg8) oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)s,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7,arg8);
#else
#define DBG {;}
#define DBG_S(txt) {;}
#define DBG_V(var) {;}
#define DBG1_S(a,b) {;}
#define DBG2_S(a,b,c) {;}
#define DBG3_S(a,b,c,d) {;}
#define DBG4_S(a,b,c,d,e) {;}
#define DBG5_S(a,b,c,d,e,f) {;}
#define DBG6_S(a,b,c,d,e,f,h) {;}
#define DBG7_S(a,b,c,d,e,f,h,i) {;}
#define DBG8_S(a,b,c,d,e,f,h,i,j) {;}
#define DBG9_S(a,b,c,d,e,f,h,i,j,k) {;}
#define DBG_START {;}
#define DBG_ENDE  {;}
#define DBGs(s) {;}
#define DBGs_S(s,txt) {;}
#define DBGs1_S(s,a,b) {;}
#define DBGs2_S(s,a,b,c) {;}
#define DBGs3_S(s,a,b,c,d) {;}
#define DBGs4_S(s,a,b,c,d,e) {;}
#define DBGs5_S(s,a,b,c,d,e,f) {;}
#define DBGs6_S(s,a,b,c,d,e,f,h) {;}
#define DBGs7_S(s,a,b,c,d,e,f,h,i) {;}
#define DBGs8_S(s,a,b,c,d,e,f,h,i,j) {;}
#endif

#define DBG_NUM if(oy_debug > 1)DBG
#define DBG_NUM_S if(oy_debug > 1)DBG_S
#define DBG_NUM1_S if(oy_debug > 1)DBG1_S
#define DBG_NUM2_S if(oy_debug > 1)DBG2_S
#define DBG_NUM3_S if(oy_debug > 1)DBG3_S
#define DBG_NUM4_S if(oy_debug > 1)DBG4_S
#define DBG_NUM5_S if(oy_debug > 1)DBG5_S
#define DBG_NUM6_S if(oy_debug > 1)DBG6_S
#define DBG_NUM7_S if(oy_debug > 1)DBG7_S
#define DBG_NUM8_S if(oy_debug > 1)DBG8_S
#define DBG_NUM9_S if(oy_debug > 1)DBG9_S
#define DBGs_NUM if(oy_debug > 1)DBGs
#define DBGs_NUM_S if(oy_debug > 1)DBGs_S
#define DBGs_NUM1_S if(oy_debug > 1)DBGs1_S
#define DBGs_NUM2_S if(oy_debug > 1)DBGs2_S
#define DBGs_NUM3_S if(oy_debug > 1)DBGs3_S
#define DBGs_NUM4_S if(oy_debug > 1)DBGs4_S
#define DBGs_NUM5_S if(oy_debug > 1)DBGs5_S
#define DBGs_NUM6_S if(oy_debug > 1)DBGs6_S
#define DBGs_NUM7_S if(oy_debug > 1)DBGs7_S
#define DBGs_NUM8_S if(oy_debug > 1)DBGs8_S
#define DBGs_NUM9_S if(oy_debug > 1)DBGs9_S
#define DBG_NUM_V if(oy_debug > 1)DBG_V

#define DBG_PROG if(oy_debug > 2)DBG
#define DBG_PROG_S if(oy_debug > 2)DBG_S
#define DBG_PROG1_S if(oy_debug > 2)DBG1_S
#define DBG_PROG2_S if(oy_debug > 2)DBG2_S
#define DBG_PROG3_S if(oy_debug > 2)DBG3_S
#define DBG_PROG4_S if(oy_debug > 2)DBG4_S
#define DBG_PROG5_S if(oy_debug > 2)DBG5_S
#define DBG_PROG6_S if(oy_debug > 2)DBG6_S
#define DBGs_PROG if(oy_debug > 2)DBGs
#define DBGs_PROG_S if(oy_debug > 2)DBGs_S
#define DBGs_PROG1_S if(oy_debug > 2)DBGs1_S
#define DBGs_PROG2_S if(oy_debug > 2)DBGs2_S
#define DBGs_PROG3_S if(oy_debug > 2)DBGs3_S
#define DBGs_PROG4_S if(oy_debug > 2)DBGs4_S
#define DBGs_PROG5_S if(oy_debug > 2)DBGs5_S
#define DBGs_PROG6_S if(oy_debug > 2)DBGs6_S
#define DBGs_PROG7_S if(oy_debug > 2)DBGs7_S
#define DBGs_PROG8_S if(oy_debug > 2)DBGs8_S
#define DBGs_PROG9_S if(oy_debug > 2)DBGs9_S
#define DBG_PROG_V if(oy_debug > 2)DBG_V
#define DBG_PROG_START if(oy_debug > 2)DBG_START
#define DBG_PROG_ENDE  if(oy_debug > 2)DBG_ENDE

#define DBG_MEM if(oy_debug > 3)DBG
#define DBG_MEM_S if(oy_debug > 3)DBG_S
#define DBG_MEM1_S if(oy_debug > 3)DBG1_S
#define DBG_MEM2_S if(oy_debug > 3)DBG2_S
#define DBG_MEM3_S if(oy_debug > 3)DBG3_S
#define DBG_MEM4_S if(oy_debug > 3)DBG4_S
#define DBG_MEM5_S if(oy_debug > 3)DBG5_S
#define DBG_MEM6_S if(oy_debug > 3)DBG6_S
#define DBG_MEM_V if(oy_debug > 3)DBG_V
#define DBG_MEM_START  if(oy_debug > 3)DBG_START
#define DBG_MEM_ENDE   if(oy_debug > 3)DBG_ENDE

#define WARNcc(ptr)    oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_, OY_DBG_ARGS_ ); 
#define WARNcc_S(ptr,txt)   oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,txt);
#define WARNcc1_S(ptr,format,arg) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg);
#define WARNcc2_S(ptr,format,arg,arg2) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2);
#define WARNcc3_S(ptr,format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3);
#define WARNcc4_S(ptr,format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4);
#define WARNcc5_S(ptr,format,arg,arg2,arg3,arg4,arg5) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5);
#define WARNcc6_S(ptr,format,arg,arg2,arg3,arg4,arg5,arg6) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6);
#define WARNcc7_S(ptr,format,arg,arg2,arg3,arg4,arg5,arg6,arg7) oyMessageFunc_p( oyMSG_WARN,ptr,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7);

#define WARNc          oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_, OY_DBG_ARGS_ );
#define WARNc_S(txt)   oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,txt);
#define WARNc1_S(format,arg) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg);
#define WARNc2_S(format,arg,arg2) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2);
#define WARNc3_S(format,arg,arg2,arg3) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3);
#define WARNc4_S(format,arg,arg2,arg3,arg4) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4);
#define WARNc5_S(format,arg,arg2,arg3,arg4,arg5) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5);
#define WARNc6_S(format,arg,arg2,arg3,arg4,arg5,arg6) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6);
#define WARNc7_S(format,arg,arg2,arg3,arg4,arg5,arg6,arg7) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7);
#define WARNc8_S(format,arg,arg2,arg3,arg4,arg5,arg6,arg7,arg8) oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ format, OY_DBG_ARGS_,arg,arg2,arg3,arg4,arg5,arg6,arg7,arg8);

void oy_backtrace_();


/* object tracing */
#define OY_TRACE_       int * ids_old = 0;
#define OY_TRACE_START_ if(oy_debug_objects > 0) \
    ids_old = oyObjectGetCurrentObjectIdList();
#define OY_TRACE_END_( t )   if(oy_debug_objects > 0) \
    oyObjectIdListShowDiffAndRelease( &ids_old, t );

#ifdef HAVE_BACKTRACE
#define OY_BACKTRACE_PRINT { \
  char * text = oyjlBT(0); \
  fprintf( stderr, "%s", text ); \
  free(text); \
}
#define OY_BACKTRACE_STRING(count) { \
          int j, nptrs; \
          void *buffer[BT_BUF_SIZE]; \
          char **strings; \
\
          nptrs = backtrace(buffer, BT_BUF_SIZE); \
\
          strings = backtrace_symbols(buffer, nptrs); \
          if( strings == NULL ) \
          { \
            perror("backtrace_symbols"); \
          } else \
          { \
            int start = nptrs-1; \
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) ); \
            oyjlStringAdd( &text, 0,0, "\n" ); \
            for(j = start; j >= 0; j--) \
            { \
              if(count > 0) \
              { \
                if(j == count) \
                  oyjlStringAdd( &text, 0,0, "(...)->"); \
                else if(j > count) \
                  continue; \
              } \
              if(oy_debug) \
                oyjlStringAdd( &text, 0,0, "%s\n", strings[j]); \
              else \
              { \
                char * t = NULL, * txt = NULL, * line_number = NULL; \
                const char * line = strings[j], \
                           * tmp = strchr( line, '(' ), \
                           * addr = strchr( tmp, '[' ); \
                if(addr) \
                { \
                  char * command = NULL; \
                  size_t size = 0; \
                  char * prog = oyjlStringCopy( line, 0 ); \
                  char * addr2 = oyjlStringCopy( addr+1, 0 ); \
                  addr2[strlen(addr2)-1] = '\000'; \
                  txt = strchr( prog, '(' ); \
                  if(txt) txt[0] = '\000'; \
                  oyjlStringAdd( &command, 0,0, "addr2line -e %s %s -si", prog, addr2 ); \
                  line_number = oyReadCmdToMem_( command, &size, "r", NULL ); \
                  if(line_number) \
                    line_number[strlen(line_number)-1] = '\000'; \
                  txt = strchr( line_number, '(' ); \
                  if(txt) txt[-1] = '\000'; \
                  oyFree_m_(addr2); \
                  oyFree_m_(command); \
                  oyFree_m_(prog); \
                } \
                if(tmp) t = oyStringCopy( &tmp[1], NULL ); \
                else t = oyStringCopy( line, NULL ); \
                txt = strchr( t, '+' ); \
                if(txt) txt[0] = '\000'; \
                if(j > 0 && (strstr(strings[j-1], t) != NULL) ) \
                  oyFree_m_(t); \
                if(t) \
                { \
                  if(j==0) \
                  { \
                    oyjlStringAdd( &text, 0,0, "%s", t ); \
                    oyjlStringAdd( &text, 0,0, "(%s) ", line_number ? line_number : ""); \
                  } \
                  else \
                  { \
                    oyjlStringAdd( &text, 0,0, "%s", t); \
                    oyjlStringAdd( &text, 0,0, "(%s)->", line_number ? line_number : ""); \
                  } \
                  oyFree_m_(t); \
                } \
                oyFree_m_(line_number); \
              } \
            } \
            free(strings); \
          } \
}
#else
#define OY_BACKTRACE_PRINT { ; }
#define OY_BACKTRACE_STRING(x) { ; }
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OYRANOS_DEBUG_H */
