/*  @file openicc_string.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#include "openicc_config_internal.h"

#if HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif
#include <string.h>  /* strdup() */
#include <stdarg.h>  /* vsnprintf() */
#include <stdio.h>   /* vsnprintf() */

#include "oyjl_core.c"

