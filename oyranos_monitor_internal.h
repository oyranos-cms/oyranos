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
 *
 * hardware API
 * 
 */

/* Date:      01. 02. 2005 */


#ifndef OYRANOS_MONITOR_INTERNAL_H
#define OYRANOS_MONITOR_INTERNAL_H

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */


struct oyMonitor_s_ {
  oyOBJECT_TYPE type;        /**< object type */
  char         *name;        /**< traditional display name - host:0 / :0 */
  char         *host;        /**< host name only - host */
  char         *identifier;  /**<  - 0.1_x_y_wxh */
  int           geo[6];      /**< display screen x y width height */
  Display      *display;     /**< logical display */
  int           screen;      /**< external screen number to call for X */
};
/** \internal  platformdependent */
typedef struct oyMonitor_s_ oyMonitor_s;

int         oyGetDisplay_                 ( const char *display_name,
                                            oyMonitor_s *disp );
const char* oyDisplayName_                ( oyMonitor_s *disp );
const char* oyDisplayHostName_            ( oyMonitor_s *disp );
const char* oyDisplayIdentifier_          ( oyMonitor_s *disp );
char*       oyDisplayScreenNumber_        ( oyMonitor_s *disp );
Display*    oyDisplayDevice_              ( oyMonitor_s *disp );
int         oyDisplayDeviceScreenNumber_  ( oyMonitor_s *disp );
int         oyDisplayNumber_              ( oyMonitor_s *disp );
int         oyDisplayScreen_              ( oyMonitor_s *disp );
int         oyDisplayX_                   ( oyMonitor_s *disp );
int         oyDisplayY_                   ( oyMonitor_s *disp );
int         oyDisplayWidth_               ( oyMonitor_s *disp );
int         oyDisplayHeight_              ( oyMonitor_s *disp );


int oyFree_       (void *oy_structure);


/** @brief \internal DDC struct */
struct oyDDC_EDID1_s_ {
 unsigned char sig[8];
 unsigned char MNF_ID[2];              /* [8] manufaturer ID */
 unsigned char MON_ID[2];              /* [10] model ID */
 unsigned char SER_ID[2];              /* [12] serial ID */
 unsigned char dummy_li[2];
 unsigned char WEEK;                   /* [16] Week */
 unsigned char YEAR;                   /* [17] + 1990 => Year */
 unsigned char major_version;          /* [18] */
 unsigned char minor_version;          /* [19] */
 unsigned char video_input_type;       /* [20] */
 unsigned char width;                  /* [21] */
 unsigned char height;                 /* [22] */
 unsigned char gamma_factor;           /* [23] */
 unsigned char DPMS;                   /* [24] */
 unsigned char rg;                     /* [25] colour information */
 unsigned char wb;                     /* [26] */
 unsigned char rY;                     /* [27] */
 unsigned char rX;                     /* [28] */
 unsigned char gY;                     /* [29] */
 unsigned char gX;                     /* [30] */
 unsigned char bY;                     /* [31] */
 unsigned char bX;                     /* [32] */
 unsigned char wY;                     /* [33] */
 unsigned char wX;                     /* [34] */
 unsigned char etiming1;               /* [35] */
 unsigned char etiming2;               /* [36] */
 unsigned char mtiming;                /* [37] */
 unsigned char stdtiming[16];          /* [38] */
 unsigned char text1[18];              /* [54] Product string */
 unsigned char text2[18];              /* [72] text 2 */
 unsigned char text3[18];              /* [90] text 3 */
 unsigned char text4[18];             /* [108] text 4 */
 unsigned char dummy_lii;
 unsigned char checksum;              /* [127] */
};

void oyUnrollEdid1_               (struct oyDDC_EDID1_s_ *edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func);


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR__INTERNAL_H */
