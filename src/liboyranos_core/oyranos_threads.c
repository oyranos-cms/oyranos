/** @file oyranos_threads.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    thread methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2014/01/24
 *
 *  This file contains functions for thread handling.
 */

#include "oyranos_threads.h"

#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_string.h"

#include <oyBlob_s.h>
#include <oyStructList_s.h>


