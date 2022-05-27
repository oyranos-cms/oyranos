/** @file oyranos_helper.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/09/02
 */


#include "oyranos_core.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_icc.h"

#include "md5.h"
#include "lookup3.h" /* oy_hashlittle */


#include <stdio.h>
#include <string.h>

intptr_t oy_observe_pointer_ = 0;

/* --- internal API definition --- */
static int oy_allocs_count_ = 0;
/** @brief    Aktivate memory debugging
 *
 *  Maps to ::OY_DEBUG_MEMORY environment variable.
 *
 *  @see @ref debug_vars
 */
int oy_debug_memory = 0;
/** @brief    Activate object debugging
 *
 *  select object id
 *
 *  Maps to ::OY_DEBUG_OBJECTS environment variable.
 *
 *  @see @ref debug_vars
 */
signed int oy_debug_objects = -1;
/** @brief    Aktivate signal debugging
 *
 *  Maps to ::OY_DEBUG_SIGNALS environment variable.
 *
 *  @see @ref debug_vars
 */
int oy_debug_signals = 0;
const char * oy_backtrace = 0;
#ifndef NO_OPT
#define OY_USE_ALLOCATE_FUNC_POOL_ 0
#endif

#if OY_USE_ALLOCATE_FUNC_POOL_
static int oy_alloc_count_ = 0;
#define OY_ALLOCATE_FUNC_POOL_CHUNK_ 128
#define OY_ALLOCATE_FUNC_POOL_SIZE_ 64
static int oy_allocate_func_pool_used_[OY_ALLOCATE_FUNC_POOL_SIZE_] = {
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 64
            ,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,

0,0,0,0,0, 0,0,0,0,0,
0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 100
       ,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 128
,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0
#endif
};
static size_t oy_allocate_func_pool_size_[OY_ALLOCATE_FUNC_POOL_SIZE_] = {
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 64
            ,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,

0,0,0,0,0, 0,0,0,0,0,
0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 100
       ,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 128
,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0
#endif
};
static oyPointer oy_allocate_func_pool_[OY_ALLOCATE_FUNC_POOL_SIZE_] = {
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 64
            ,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,

0,0,0,0,0, 0,0,0,0,0,
0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 100
       ,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0
#endif
#if OY_ALLOCATE_FUNC_POOL_SIZE_ >= 128
,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0,0,0,
0,0,0,0,0, 0,0,0
#endif
};
#endif

/* internal memory handling */
void* oyAllocateFunc_           (size_t        size)
{
  /* we have most often to work with text arrays, so initialise with 0 */
  void *ptr = 0;
#if OY_USE_ALLOCATE_FUNC_POOL_
  int empty = -1;
  int i;

  if(size <= OY_ALLOCATE_FUNC_POOL_CHUNK_ || size == 512 || size == 1024)
  {
    for(i = 0; i < OY_ALLOCATE_FUNC_POOL_SIZE_; ++i)
      if(oy_allocate_func_pool_size_[i] == size &&
         oy_allocate_func_pool_used_[i] <= 0)
      {
        ptr = oy_allocate_func_pool_[i];
        oy_allocate_func_pool_used_[i] = 1;
        if(oy_debug_memory)
          printf( "%s:%d pos:%d size=%d reused\n", __FILE__,__LINE__,
                  i, (int)size );
        return ptr;
      } else if(oy_allocate_func_pool_size_[i] == 0)
      {
        empty = i;
        break;
      }

    if(ptr == 0)
    {
      if(empty >= 0)
      {
        oy_allocate_func_pool_size_[empty]= size <= OY_ALLOCATE_FUNC_POOL_CHUNK_
                                            ? OY_ALLOCATE_FUNC_POOL_CHUNK_:size;
        oy_allocate_func_pool_[empty] = ptr =
                                   malloc( oy_allocate_func_pool_size_[empty] );
        oy_allocate_func_pool_used_[empty] = 1;
      } else
      {
        /* Clear the pool. Following strategies are possible at different cost:
           a) free all unused and loose all used pointers
           b) free all double occurences of unused pointers
           c) observe all pointers for several times and remove unused ones
         */

        /* (a) */
        int n = 0;
        for(i = 0; i < OY_ALLOCATE_FUNC_POOL_SIZE_; ++i)
          if(oy_allocate_func_pool_used_[i] <= 0)
          {
            free(oy_allocate_func_pool_[i]);
            ++n;
          }

        if(oy_debug_memory)
        {
          oy_allocs_count_ -= n;
          printf( "%s:%d %d pointers freed in pool clear => %d\n",
                  __FILE__,__LINE__, n, oy_allocs_count_);
        }

        memset( oy_allocate_func_pool_, 0,
                sizeof(oyPointer) * OY_ALLOCATE_FUNC_POOL_SIZE_);
        memset( oy_allocate_func_pool_size_, 0,
                sizeof(size_t) * OY_ALLOCATE_FUNC_POOL_SIZE_);
        memset( oy_allocate_func_pool_used_, 0,
                sizeof(int) * OY_ALLOCATE_FUNC_POOL_SIZE_);
      }
    }
  }



  if(ptr == 0)
#endif
  {
    if(oy_debug_memory != 0 || size == 0)
      printf( "%lu + %lu byte allocate %d "OY_DBG_FORMAT_ ,
              (unsigned long) size, (unsigned long) 2*sizeof(long),
              oy_allocs_count_, OY_DBG_ARGS_);

    /* sizeof(long) is for better alignment and less valgrind positives 
     * 2*sizeof(long) helped on a oS-12.1 instal with glibc-2.14.1-14.27.1.x86_64 */
    ptr = malloc (size + 2 * sizeof(long));

    if(oy_debug_memory != 0)
      printf( " "OY_PRINT_POINTER"\n", (intptr_t)ptr);
  }

  if( !ptr )
  {
    WARNc1_S( "Can not allocate %d byte.", (int)size );
  }
    else if(oy_debug_memory != 0)
  {
#if OY_USE_ALLOCATE_FUNC_POOL_
    oy_alloc_count_ += size;
    printf( OY_DBG_FORMAT_"%d allocate %d  %d\n", OY_DBG_ARGS_,
            oy_allocs_count_, (int)size, oy_alloc_count_ );
#endif
    ++oy_allocs_count_;
  }

  return ptr;
}

void  oyDeAllocateFunc_           (void*       block)
{
#if OY_USE_ALLOCATE_FUNC_POOL_
  int i;

  /* here is a constant work done */
  for(i = 0; i < OY_ALLOCATE_FUNC_POOL_SIZE_; ++i)
    if(oy_allocate_func_pool_[i] == block)
    {
      memset( block, 0, oy_allocate_func_pool_size_[i] );
      oy_allocate_func_pool_used_[i] = 0;

      if(oy_debug_memory)
      {
        printf( OY_DBG_FORMAT_"found block with pos:%d size=%d", OY_DBG_ARGS_,
                i, (int)oy_allocate_func_pool_size_[i] );
        printf( " "OY_PRINT_POINTER"\n", (intptr_t)ptr);
      }

      return;
    }
#endif

  if( !block ) {
    WARNc_S( "Memory block is empty." )
  } else
  {
    if(oy_debug_memory != 0)
    {
      printf( OY_DBG_FORMAT_"%d remaining "OY_PRINT_POINTER"\n", OY_DBG_ARGS_,
              --oy_allocs_count_, (intptr_t)block );
    }
    free( block );
  }
}

void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func)
{
  if(allocate_func)
    return allocate_func (size);
  else
    return oyAllocateFunc_ (size);
}


/** @internal
 *  @brief hash calculation
 *
 *  @param[in]     buffer              some buffer
 *  @param[in]     size                size of buffer
 *  @param[in]     flags               zero or oyHASH_MD5 or oyHASH_L3
 *  @param[out]    digest              result in OY_HASH_SIZE * 2 bytes
 *
 *  @version Oyranos: 0.9.7
 *  @date    2022/05/26
 *  @since   2013/03/13 (Oyranos: 0.9.5)
 */
int                oyMiscBlobGetHash_( const void        * buffer,
                                       size_t              size,
                                       uint32_t            flags,
                                       unsigned char     * digest )
{
  int error = 0;
  DBG_PROG_START

  if (digest) 
  {
    if(flags & oyHASH_L3)
    {
      uint32_t dig;

      memset( digest, 0, OY_HASH_SIZE * 2 );

      dig = oy_hashlittle( buffer, size, 0 );
      sprintf( (char*) digest, "%d", dig );
    }
    else
    {
      error = oyMiscBlobGetMD5_( buffer, size, digest );
    }

    if(oy_debug >= 4)
    {
      uint32_t * id = (uint32_t *)digest;
      oyMessageFunc_p( oyMSG_DBG, 0, OY_DBG_FORMAT_
                       "%08x%08x%08x%08x", OY_DBG_ARGS_,
                       id[0],id[1],id[2],id[3] );
    }

  } else {
    WARNc3_S ("False memory - size = %d pos = %lu digest = %lu",
              (int)size, (long int)buffer, (long int)digest);

    error = 1;
  }

  DBG_PROG_ENDE
  return error;
}

/** @internal
 *  @brief md5 calculation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
int                oyMiscBlobGetMD5_ ( const void        * buffer,
                                       size_t              size,
                                       unsigned char     * digest )
{
  DBG_PROG_START

  if (digest) 
  {
    oy_md5_state_t state;
    uint32_t * i = (uint32_t*)digest;
    int j;

    oy_md5_init(   &state );
    oy_md5_append( &state, (const md5_byte_t *)buffer, size );
    oy_md5_finish( &state, digest );

    /* correct byteorder to big endian */
    for(j = 0; j < 4; ++j)
      i[j] = oyValueUInt32(i[j]);

    DBG_PROG_ENDE
    return 0;

  } else {
    WARNc3_S ("False memory - size = %d pos = %lu digest = %lu",
              (int)size, (long int)buffer, (long int)digest);

    DBG_PROG_ENDE
    return 1;
  }
}

uint32_t           oyMiscBlobL3_     ( const void        * buffer,
                                       size_t              size )
{
  uint32_t ret = 0;

  ret = oy_hashlittle( buffer, size, 0 );

  return ret;
}


