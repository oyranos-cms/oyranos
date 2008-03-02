/** @file oyranos_debug.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    internal helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/31/01
 */


/* Dont use in non Oyranos projects. */


#ifndef OYRANOS_DEBUG_H
#define OYRANOS_DEBUG_H


#include <time.h>

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* debugging variable - set 0 to off (default), set 1 to switch debugging on */
extern int oy_debug;
extern clock_t oyranos_clock_;
extern int level_PROG;

#if defined(OY_CONFIG_H)

/* Debug system */
#define DEBUG 1

#define DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC

#define DBG_T_ printf ("%s:%d %s() %02f ",__FILE__,__LINE__,__func__, DBG_UHR_);
#define LEVEL { int i; for (i = 0; i < level_PROG; i++) printf (" "); }
#define DBG if(oy_debug) { LEVEL printf("        "); DBG_T_ printf ("\n");}
#define DBG_S(txt) if(oy_debug) { LEVEL printf("        "); DBG_T_ printf txt ; printf("\n"); }
#define DBG_V(txt) if(oy_debug) { int val = (int) txt; LEVEL printf("        "); DBG_T_ printf(#txt " %d\n", val);}
#if DEBUG == 1
#define DBG_MEM DBG
#define DBG_MEM_S(txt) DBG_S(txt)
#define DBG_MEM_V(txt) DBG_V(txt)
#else
#define DBG_MEM
#define DBG_MEM_S(txt)
#define DBG_MEM_V(txt)
#endif
#if DEBUG == 1
#define DBG_NUM DBG
#define DBG_NUM_S(txt) DBG_S(txt)
#define DBG_NUM_V(txt) DBG_V(txt)
#else
#define DBG_NUM
#define DBG_NUM_S(txt)
#define DBG_NUM_V(txt)
#endif
#if DEBUG == 1
#define DBG_PROG DBG
#define DBG_PROG_START if(oy_debug) { int i; level_PROG++; for (i = 0; i < level_PROG; i++) printf("+"); printf (" Start: "); DBG_T_ printf("\n"); }
#define DBG_PROG_ENDE if(oy_debug) { int i; for (i = 0; i < level_PROG; i++) printf ("-"); printf(" Ende:  "); DBG_T_ level_PROG--; printf("\n"); }
#define DBG_PROG_S(txt) DBG_S(txt)
#define DBG_PROG_V(txt) DBG_V(txt)
#else
#define DBG_PROG
#define DBG_PROG_START
#define DBG_PROG_ENDE
#define DBG_PROG_S(txt)
#define DBG_PROG_V(txt)
#endif

#endif /* defined OY_CONFIG_H */

#define WARNc {int dbg = oy_debug; oy_debug = 1; \
                    DBG_S((_("Warning"))) oy_debug = dbg;}
#define WARNc_S(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_S((_("Warning : ")); printf txt) oy_debug = dbg;}
#define WARNc_V(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_V((_("Warning : ")); printf txt) oy_debug = dbg;}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OYRANOS_DEBUG_H */
