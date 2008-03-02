/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @internal API
 * 
 */

/* Date:      31. 01. 2005 */


#ifndef OYRANOS_DEBUG_H
#define OYRANOS_DEBUG_H


#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Debug system */
#define DEBUG 1

/* debugging variable - set 0 to off (default), set 1 to switch debugging on */
extern int oy_debug;
extern clock_t oyranos_clock_;
extern int level_PROG;

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
#define WARN {int dbg = oy_debug; oy_debug = 1; \
                    DBG_S((_("Warning"))) oy_debug = dbg;}
#define WARN_S(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_S((_("Warning : ")); printf txt) oy_debug = dbg;}
#define WARN_V(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_V((_("Warning : ")); printf txt) oy_debug = dbg;}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OYRANOS_DEBUG_H */
