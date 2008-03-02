/** @file oyranos_debug.c
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
 *  @since    2005/02/01
 */

#include "oyranos_debug.h"

#define DEBUG 1
int level_PROG = 0;
clock_t oyranos_clock_ = 0;
int oy_debug = 0;


