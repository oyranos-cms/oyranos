/** @file oyjl_io_sgare.c
 *
 *  oyjl - file i/o and other basic helpers
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

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_io_internal.h"
#include "oyjl_version.h"

/* --- IO_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
/** \addtogroup oyjl_io
 *  @{ *//* oyjl_io */

#ifndef WARNc_S
#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
#endif

char * oyjlGetCurrentDir_ ()
{
# if defined(_WIN32)
  char * path = NULL;
  DWORD len = 0;

  len = GetCurrentDirectory(0,NULL);

  if(len)
    oyjlAllocString_m_( path, len+1,
                      oyAllocateFunc_, return NULL );
    
  if(len && path)
  {
    int i;

    len = GetCurrentDirectory( len+1, path );

    for(i=0; i < len; ++i)
      if(path[i] == '\\')
        path[i] = '/';
  } else
    WARNc_S("Could not get \"PWD\" directory name");

  return path;
# else
  char * name = oyjlStringCopy( getenv("PWD"), 0 );

  if(!name)
    WARNc_S("Could not get \"PWD\" directory name");

  return name;
# endif
}
const char * oyjlGetHomeDir_ ()
{
# if defined(_WIN32)
  static CHAR path[MAX_PATH];
  static int init = 0;


  if(init)
    return path;

  init = 1;

  if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, 0, &path[0])))
  {
    int len = strlen(path), i;
    for(i=0; i < len; ++i)
      if(path[i] == '\\')
        path[i] = '/';
  }
  else
    WARNc_S("Could not get \"HOME\" directory name");

  return path;
# else
  const char* name = getenv("HOME");

  if(!name)
    WARNc_S("Could not get \"HOME\" directory name");
  return name;
# endif
}
/* resolve for consume in fopen() */
char * oyjlResolveDirFileName_ (const char* name)
{
  char * newName = NULL;
  const char * home = NULL;

  if(!name)
  {
    WARNc_S ("no name");
    return NULL;
  }

  /* user directory */
  if (name[0] == '~')
  {
    home = oyjlGetHomeDir_();
    oyjlStringAdd( &newName, 0,0, "%s%s", home, &name[0]+1 );

  } else
  {
    /* relative names - where the first sign is no directory separator */
    if (name[0] != OYJL_SLASH_C
 #ifdef _WIN32
    /* ... and no windows drive */
        && name[1] != ':'
 #endif
       )
    {
      char * pw = oyjlGetCurrentDir_();
      const char * t = name;

      oyjlStringAdd( &newName, 0,0, "%s%c", pw, OYJL_SLASH_C );
      if(name[0] == '.' &&
         name[1] == '/')
        t = &name[2];
      oyjlStringPush( &newName, t, 0,0 );
      free(pw);
    } else
      /* nothing to do - just copy */
      newName = oyjlStringCopy( name, 0 );
  }

  return newName;
}

/** @} *//* oyjl_io */
/** @} *//* oyjl_core */

/* --- IO_Section --- */

