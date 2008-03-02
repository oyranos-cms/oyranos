/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann 
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
 *
 * helpers
 * 
 */

/* Date:      August 2004 */

#ifndef OYRANOS_HELFER_H
#define OYRANOS_HELFER_H

#define _(text) text

#include <time.h>


/* Debug system */
extern int level_PROG;
extern clock_t oyranos_clock_;

#define DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC

#define DBG_T_ printf ("%s:%d %s() %02f ",__FILE__,__LINE__,__func__, DBG_UHR_);
#define LEVEL { int i; for (i = 0; i < level_PROG; i++) printf (" "); }
#define DBG if(oy_debug) { LEVEL printf("        "); DBG_T_ printf ("\n");}
#define DBG_S(txt) if(oy_debug) { LEVEL printf("        "); DBG_T_ printf txt ; printf("\n"); }
#define DBG_V(txt) if(oy_debug) { LEVEL printf("        "); DBG_T_ printf(#txt " %d\n", txt);}
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
#define DBG_PROG_START if(oy_debug) { level_PROG++; int i; for (i = 0; i < level_PROG; i++) printf("+"); printf (" Start: "); DBG_T_ printf("\n"); }
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
                    DBG_S(("Warning")) oy_debug = dbg;}
#define WARN_S(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_S(("Warning : "); printf txt) oy_debug = dbg;}
#define WARN_V(txt) {int dbg=oy_debug; oy_debug = 1; \
                    DBG_V(("Warning : "); printf txt) oy_debug = dbg;}


/* mathematical helpers */

#define MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define HYP(a,b)    pow((a)*(a) + (b)*(b),1.0/2.0)
#define HYP3(a,b,c) pow( (a)*(a) + (b)*(b) + (c)*(c) , 1.0/2.0)
#define RUND(a)     ((a) + 0.5)


#endif /* OYRANOS_HELFER_H */
