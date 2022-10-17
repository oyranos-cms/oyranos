/** @file oyranos_check.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    input / output  methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/11/17
 */

#include <sys/stat.h>
#ifdef HAVE_POSIX
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
/*#include "oyranos_cmms.h" */
#include "oyranos_check.h"
#include "oyranos_debug.h"
#include "oyranos_icc.h"
#include "oyranos_io.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_check.h"
#include "oyranos_sentinel.h"
#include "oyProfile_s.h"
/*#include "oyranos_xml.h" */

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */


/* oyranos part */




/* --- function definitions --- */


/* profile check API */

/** @internal
 *  supported flags are OY_ICC_VERSION_2 and OY_ICC_VERSION_4
 */
int      oyCheckProfile_             ( const char        * name,
                                       const char        * colorsig,
                                       int                 flags )
{
  char *fullName = 0;
  char* header = 0; 
  size_t size = 0;
  int r = 1;

  DBG_MEM_START

  /*if(name) DBG_NUM_S((name)); */
  fullName = oyFindProfile_(name, flags);
  if (!fullName)
  { WARNc2_S("%s %s", _("not found:"),name) }

  /* do check */
  if (oyIsFileFull_(fullName,"rb"))
  {
    size = 128;
    header = oyReadFileToMem_ (fullName, &size, oyAllocateFunc_); DBG_PROG
    if (size >= 128)
      r = oyCheckProfileMem_ (header, 128, colorsig, flags);
  }

  /* release memory */
  if(header && size)
    oyFree_m_(header);
  if(fullName) oyFree_m_(fullName);

  DBG_MEM_ENDE
  return r;
}

int      oyCheckProfileMem_          ( const void        * mem,
                                       size_t              size,
                                       const char        * colorsig,
                                       int                 flags )
{
  char* block = (char*) mem;
  int offset = 36;

  DBG_MEM_START

  if (block && size >= 128) 
  {
    if (block[offset+0] == 'a' &&
        block[offset+1] == 'c' &&
        block[offset+2] == 's' &&
        block[offset+3] == 'p' )
    {
      icHeader* h = (icHeader*)mem;
      icSignature vs = h->version;
      char * v = (char*)&vs;
      icProfileClassSignature prof_device_class = h->deviceClass;
      icProfileClassSignature device_class = (icProfileClassSignature)0;
      int v2 = OY_ICC_VERSION_2, v4 = OY_ICC_VERSION_4;

      if(colorsig)
        device_class = *((icProfileClassSignature*)colorsig);

      DBG_MEM_ENDE
      if((colorsig && memcmp(&prof_device_class,&device_class,4) != 0) ||
         ((flags & v2 || flags & v4) &&
           !((flags & v2 && v[0] == 2) ||
            (flags & v4 && v[0] == 4) 
            )))
        return 2;
      else
        return 0;
    } else {
      if(oy_warn_)
        WARNc4_S(" sign: %c%c%c%c ", (char)block[offset+0],
        (char)block[offset+1], (char)block[offset+2], (char)block[offset+3] );
      DBG_MEM_ENDE
      return 1;
    }
  } else {
    WARNc2_S("False profile - size = %d pos = %lu ", (int)size, (long int)block)

    DBG_MEM_ENDE
    return 1;
  }
}


/** @internal
 *  @brief md5 calculation
 *
 *  @param[in]  buffer         complete profiles buffer
 *  @param[in]  size           over all profile size
 *  @param[out] md5_return     buffer to write in the md5 digest (128 bytes)
 *                             in host byte order
 *  @return                    -1 wrong profile_id detected, 0 - good, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/24 (Oyranos: 0.1.x)
 *  @date    2009/08/15
 */
int
oyProfileGetMD5_       ( void       *buffer,
                         size_t      size,
                         unsigned char *md5_return )
{
  char* block = NULL;
  int error = 0;
  char profile_id[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  uint32_t * h = (uint32_t*)profile_id,
           * m = (uint32_t*)md5_return;
  int has_profile_id;

  DBG_PROG_START

  if (size >= 128) 
  {
    oyAllocHelper_m_( block, char, size, oyAllocateFunc_, return 1);
    memcpy( block, buffer, size);

    memcpy( profile_id, &block[84], 16 );

    /* process as described in the ICC specification */
    memset( &block[44], 0, 4 );  /* flags */
    memset( &block[64], 0, 4 );  /* intent */
    memset( &block[84], 0, 16 ); /* ID */

    error = oyMiscBlobGetMD5_(block, size, md5_return);
    has_profile_id = h[0] || h[1] || h[2] || h[3];

    /* Check if the profiles internal header ID differs. */
    if(oyValueUInt32(h[0]) != m[0] ||
       oyValueUInt32(h[1]) != m[1] ||
       oyValueUInt32(h[2]) != m[2] ||
       oyValueUInt32(h[3]) != m[3])
    {
      char tmp_hash[34], tmp_hash2[34];

      oySprintf_(tmp_hash, "%08x%08x%08x%08x", oyValueUInt32(h[0]),
                oyValueUInt32(h[1]), oyValueUInt32(h[2]), oyValueUInt32(h[3]));

      oySprintf_(tmp_hash2, "%08x%08x%08x%08x", m[0], m[1], m[2], m[3]);

      if(has_profile_id)
        WARNc2_S("internal: %s != compute: %s", tmp_hash, tmp_hash2)
      error = -1 - has_profile_id;
    } else
    if(oy_debug > 2)
    {
      oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*) NULL, 
                OY_DBG_FORMAT_ "[ICC md5]: %08x%08x%08x%08x", OY_DBG_ARGS_,
                oyValueUInt32(h[0]),
                oyValueUInt32(h[1]), oyValueUInt32(h[2]), oyValueUInt32(h[3]));
    }

    if(block) oyFree_m_ (block);

  } else
    error = 1;

  DBG_PROG_ENDE
  return error;
}

int
oyCheckPolicy_               ( const char * name )
{
  char* header = 0; 
  size_t size = 0;
  int r = 1;

  DBG_PROG_START

  /* do check */
  if (oyIsFileFull_(name,"rb"))
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



