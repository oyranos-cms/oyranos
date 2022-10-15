/** @file oyjl_message.c
 *
 *  oyjl - message helpers
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2022  Kai-Uwe Behrmann  <ku.b@gmx.de>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <ctype.h>   /* isspace() tolower() isdigit() */
#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   /* time_t localtime() */
#include <unistd.h>
#include <errno.h>
#include <wchar.h>  /* wcslen() */

#include "oyjl.h"
#include "oyjl_i18n_internal.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"

/* --- Message_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */

/** @brief   the default message handler to stderr
 *
 *  @param         error_code          ::oyjlMSG_e codes
 *  @param         context_object      not used
 *  @param         format              your text
 *  @return                            error
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
                                       const char        * format,
                                       ... )
{
  char * text = NULL;
  int error = 0;
  const char * status_text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return 1)

  if(error_code == oyjlMSG_INFO) status_text = oyjlTermColor(oyjlGREEN,"Info: ");
  if(error_code == oyjlMSG_CLIENT_CANCELED) status_text = oyjlTermColor(oyjlBLUE,"Client Canceled: ");
  if(error_code == oyjlMSG_INSUFFICIENT_DATA) status_text = oyjlTermColor(oyjlRED,_("Insufficient Data:"));
  if(error_code == oyjlMSG_ERROR) status_text = oyjlTermColor(oyjlRED,_("Usage Error:"));
  if(error_code == oyjlMSG_PROGRAM_ERROR) status_text = oyjlTermColor(oyjlRED,_("Program Error:"));
  if(error_code == oyjlMSG_SECURITY_ALERT) status_text = oyjlTermColor(oyjlRED,_("Security Alert:"));

  if(status_text)
    fprintf( stderr, "%s ", status_text );
  if(text)
    fprintf( stderr, "%s\n", text );
  fflush( stderr );

  free( text ); text = 0;

  return error;
}

oyjlMessage_f     oyjlMessage_p = oyjlMessageFunc;

/** @brief   set a custom message listener
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int            oyjlMessageFuncSet    ( oyjlMessage_f       message_func )
{
  if(message_func)
    oyjlMessage_p = message_func;
  return 0;
}
/** @} *//* oyjl_core */
/* --- Message_Section --- */

