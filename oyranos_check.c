/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
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

/** @file @internal
 *  @brief input / output  methods
 */

/* Date:      17. 11. 2006 */

#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
/*#include "oyranos_cmms.h" */
#include "oyranos_check.h"
#include "oyranos_debug.h"
#include "oyranos_io.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_check.h"
#include "oyranos_sentinel.h"
/*#include "oyranos_xml.h" */

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */


/* oyranos part */



/* --- Helpers  --- */
/* small helpers */
#if 1
#define ERR if (rc<=0 && oy_debug) { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif


/* --- function definitions --- */


/* profile check API */

int
oyCheckProfile_                    (const char* name,
                                    const char* coloursig)
{
  char *fullName = 0;
  char* header = 0; 
  size_t size = 0;
  int r = 1;

  DBG_PROG_START

  /*if(name) DBG_NUM_S((name)); */
  fullName = oyFindProfile_(name);
  if (!fullName)
    WARNc_S(("%s not found",name))
  else
    ;/*DBG_NUM_S((fullName)); */

  /* do check */
  if (oyIsFileFull_(fullName))
  {
    size = 128;
    header = oyReadFileToMem_ (fullName, &size, oyAllocateFunc_); DBG_PROG
    if (size >= 128)
      r = oyCheckProfile_Mem (header, 128, coloursig);
  }

  /* release memory */
  if(header && size)
    oyFree_m_(header);
  if(fullName) oyFree_m_(fullName);

  DBG_NUM_S(("oyCheckProfileMem = %d",r))

  DBG_PROG_ENDE
  return r;
}

int
oyCheckProfile_Mem                 (const void* mem, size_t size,
                                    const char* coloursig)
{
  char* block = (char*) mem;
  int offset = 36;

  DBG_PROG_START

  if (size >= 128) 
  {
    if (block[offset+0] == 'a' &&
        block[offset+1] == 'c' &&
        block[offset+2] == 's' &&
        block[offset+3] == 'p' )
    {
      DBG_PROG_ENDE
      return 0;
    } else {
      if(oy_warn_)
        WARNc_S((" sign: %c%c%c%c ", (char)block[offset+0],
        (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] ));
      DBG_PROG_ENDE
      return 1;
    }
  } else {
    WARNc_S (("False profile - size = %d pos = %lu ", (int)size, (long int)block))

    DBG_PROG_ENDE
    return 1;
  }
}

#include "md5.h"

int
oyProfileGetMD5_       ( void       *buffer,
                         size_t      size,
                         char       *md5_return )
{
  char* block = NULL;

  DBG_PROG_START

  if (size >= 128) 
  {
    oy_md5_state_t state;
    md5_byte_t digest[16];

    oyAllocHelper_m_( block, char, size, oyAllocateFunc_, return 1);
    memcpy( block, buffer, size);

    memset( &block[44], 0, 4 );  /* flags */
    memset( &block[64], 0, 4 );  /* intent */
    memset( &block[84], 0, 16 ); /* ID */

    oy_md5_init(   &state );
    oy_md5_append( &state, (const md5_byte_t *)block, size );
    oy_md5_finish( &state, digest );

    memcpy( md5_return, digest, 16 );

    if(block) oyFree_m_ (block);

    DBG_PROG_ENDE
    return 0;
  } else {
    WARNc_S (("False profile - size = %d pos = %lu ", (int)size, (long int)block))

    DBG_PROG_ENDE
    return 1;
  }
}

int
oyCheckPolicy_               ( const char * name )
{
  char* header = 0; 
  size_t size = 0;
  int r = 1;

  DBG_PROG_START

  /* do check */
  if (oyIsFileFull_(name))
  {
    size = 128;
    header = oyReadFileToMem_ (name, &size, oyAllocateFunc_); DBG_PROG
    if (size >= 128)
      if(memcmp(  header, OY_POLICY_HEADER, strlen(OY_POLICY_HEADER)) == 0)
        r = 0;
  }

  /* release memory */
  if(header && size)
    oyFree_m_(header);

  DBG_PROG_ENDE
  return r;
}



