/** @file oyranos_widgets.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    widget alpha APIs
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/02/09
 */

#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_monitor.h" /* TODO */
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#if !defined(_WIN32)
#include <dlfcn.h>
#endif
#include <math.h>



