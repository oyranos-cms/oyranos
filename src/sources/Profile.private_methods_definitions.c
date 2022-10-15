/* Static helper functions { */
/** @internal
 *  @memberof oyProfile_s
 *  @brief   check internal ICC profile ID
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/10 (Oyranos: 0.3.0)
 *  @date    2011/04/10
 */
int oyProfile_HasID_          ( oyProfile_s_      * s )
{
  int has_id = 0;

  if(s->block_ && s->size_ >= 132 )
  {
    char * data = s->block_;
    uint32_t * id = (uint32_t*)&data[84];

    if(id[0] || id[1] || id[2] || id[3])
      has_id = 1;
  }

  return has_id;
}

/** @internal
 *  @memberof oyProfile_s
 *  @brief   hash for oyProfile_s
 *
 *  Get ICC ID from profile or compute.
 +
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/0 (Oyranos: 0.1.8)
 *  @date    2011/04/10
 */
int oyProfile_GetHash_               ( oyProfile_s_      * s,
                                       int                 flags )
{
  int error = 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(s->block_ && s->size_)
  {
    int has_id = oyProfile_HasID_( s );

    oyObject_HashSet( s->oy_, 0 );
    if(flags & OY_COMPUTE ||
       !has_id)
    {
      error = oyProfileGetMD5( s->block_, s->size_, s->oy_->hash_ptr_ );
      if(error == -2)
        oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)s,
                         OY_DBG_FORMAT_"%s \"%s\": %d", OY_DBG_ARGS_,
                         oyProfile_GetText( (oyProfile_s*) s, oyNAME_DESCRIPTION ),
                         oyProfile_GetFileName( (oyProfile_s*) s, -1 ),
                         error );
    } else
    {
      char * data = s->block_;
      uint32_t id[4];
      int i;
      memcpy( id, &data[84], 16 );

      for(i = 0; i < 4; ++i)
        id[i] = oyValueUInt32( id[i] );
      memcpy(s->oy_->hash_ptr_, id, 16);
      error = 0;
    }

    if(error > 0)
      oyObject_HashSet( s->oy_, 0 );
  }
  return error;
}
/* } Static helper functions */

#include "oyranos_check.h"

/** @internal
 *  Function  oyProfile_FromMemMove_
 *  @memberof oyProfile_s
 *  @brief    Create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          pass through
 *  @param[out]   error_return   error codes
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/08/03
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
oyProfile_s_* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       int               * error_return,
                                       oyObject_s          object)
{
  oyProfile_s_ * s = oyProfile_New_( object );
  int error = 0,
      l_error = 0;
  icSignature vs;
  char * v = (char*)&vs;

  if(block  && *block && size)
  {
    s->block_ = *block;
    *block = 0;

    if(size > 128)
    {
      int r = oyCheckProfileMem_( s->block_, 128, 0, flags );
      if(r)
      {
        DBG_PROG1_S( "check failed %d", r )
        error = r;

        oyProfile_Release( (oyProfile_s**)&s );

        if(error_return) *error_return = error;
        return NULL;
      }
    } else
    {
      WARNc1_S( "too small for a ICC profile %d", size )
      error = 1;
    }    

    if(!s->block_)
      error = 1;
    else
      s->size_ = size;
  }

  if (!s->block_)
  {
    WARNc1_S( "%s", "no data" )
    error = 1;
  }

  /* Comparision strategies
      A
       - search for similiar arguments in the structure
       - include the affect of the arguments (resolve type -> filename)
       - exclude paralell structure elements (filename -> ignore blob)
      B
       - use always the ICC profiles md5
      C
       - always the flattened Oyranos profile md5

       - A higher level API can maintain its own cache depending on costs.
   */

  if(error <= 0)
  {
    l_error = oyProfile_GetHash_( s, flags );
    if(l_error != 0)
    {
      if(l_error > 0 || l_error < -1)
        WARNc1_S( "hash error %d", l_error )
      if(error <= 0 && l_error != 0)
        error = l_error;
    }
  }

  vs = (icSignature) oyValueUInt32( oyProfile_GetSignature((oyProfile_s*)s,oySIGNATURE_VERSION) );      

  if(error <= 0)
  {
    l_error = !oyProfile_GetSignature ( (oyProfile_s*)s, oySIGNATURE_COLOR_SPACE );

    if(l_error && v[0] < 5)
      WARNc1_S( "signature error %d", error )
    if(error <= 0 && l_error != 0 && v[0] < 5)
      error = l_error;
  }

  if(error <= 0)
  {
    s->names_chan_ = 0;
    s->channels_n_ = oyProfile_GetChannelsCount( (oyProfile_s*)s );
    l_error = (s->channels_n_ <= 0);
    if(error <= 0 && l_error != 0 && v[0] < 5)
      error = l_error;
  }

  if(error  >= 1)
  {
    icHeader *h = 0;
    icSignature sig = 0;

    h = (icHeader*) s->block_;

    if(h)
      sig = oyValueCSpaceSig( h->colorSpace );

    WARNc4_S("Channels <= 0 %d %s %s err:%d", s->channels_n_,
             oyICCColorSpaceGetName(sig),
             h?oyICCColorSpaceGetName(h->colorSpace):"",
             error)

    oyProfile_Release( (oyProfile_s**)&s );
  }

  if(error_return) *error_return = error;

  return s;
}

#ifdef COMPILE_STATIC
#include "Gray-CIE_L.h"
#include "ISOcoated_v2_bas.h"
#include "ITULab.h"
#include "Lab.h"
#include "LStar-RGB.h"
#include "sRGB.h"
#include "XYZ.h"
#endif

/** @internal
 *  Function  oyProfile_FromFile_
 *  @memberof oyProfile_s
 *  @brief    Create from file
 *
 *  @param[in]    name           profile file name
 *  @param[in]    flags          see oyProfile_FromFile()
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles. OY_COMPUTE and OY_ICC_VERSION_2 and
 *  OY_ICC_VERSION_4 and OY_NO_REPAIR are supported too.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/25
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 */
oyProfile_s_ *  oyProfile_FromFile_  ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = 0;
  int error = 0;
  size_t size = 0;
  oyPointer block = 0;
  oyAlloc_f allocateFunc = 0;
  oyHash_s_ * entry = 0;
  char * file_name = NULL;
  char * hash = oyjlStringCopy( name, oyAllocateFunc_ );

  if(object)
    allocateFunc = object->allocateFunc_;

  if(!oyToNoCacheRead_m(flags) || !oyToNoCacheWrite_m(flags))
  {
    if(!oy_profile_s_file_cache_)
      oy_profile_s_file_cache_ = (oyStructList_s_*)oyStructList_Create( oyOBJECT_NONE, "oy_profile_s_file_cache_", 0 );

    if(!object)
    {
      if(!(flags & OY_SKIP_MTIME_CHECK))
      {
        char * info = NULL;
        oyFree_m_( hash );
        hash = NULL;
        oyjlAllocHelper_m( info, char, 128, malloc, return NULL );

        file_name = oyFindProfile_( name, flags );
        oyjlIsFile( file_name, "r", 0, info, 128 );
        oyjlStringAdd( &hash, 0,0, "%s:%s", name, info );
        oyFree_m_( info );
      }
      entry = (oyHash_s_*)oyCacheListGetEntry_ ( (oyStructList_s*)oy_profile_s_file_cache_, 0, hash );

      if(!oyToNoCacheRead_m(flags))
      {
        if(!file_name)
          file_name = oyFindProfile_( name, flags );
        s = (oyProfile_s_*) oyHash_GetPointer_( entry, oyOBJECT_PROFILE_S);
        if(s &&
           (flags & OY_ICC_VERSION_2 || flags & OY_ICC_VERSION_4))
        {
          icSignature vs = (icSignature) oyValueUInt32( oyProfile_GetSignature((oyProfile_s*)s,oySIGNATURE_VERSION) );      
          char * v = (char*)&vs;

          if((flags & OY_ICC_VERSION_2 || flags & OY_ICC_VERSION_4) &&
             !((flags & OY_ICC_VERSION_2 && v[0] == 2) ||
               (flags & OY_ICC_VERSION_4 && v[0] == 4)))
            oyProfile_Release( (oyProfile_s**)&s );
        }
        if(s)
        {
          oyHash_Release_( &entry );
          goto clean_oyProfile_FromFile_;
        }
      }
    }
  }

  if(name && !s)
  {
    if(!file_name)
      file_name = oyFindProfile_( name, flags );
    block = oyGetProfileBlock( file_name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  {
    int repair = 0;
    const char * t = file_name;
    uint32_t md5[4];

    if(block && size)
      s = oyProfile_FromMemMove_( size, &block, flags, &error, object );
#ifdef COMPILE_STATIC
    else
    {
      error = 0;
      /* START static inbuilds */
      if(name && name[0] && strcmp("Gray-CIE_L.icc",name) == 0)
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_Oyranos_Gray_CIE_L_icc_len,
                             (const oyPointer)_usr_share_color_icc_Oyranos_Gray_CIE_L_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-Gray-CIE_L_icc", oyAllocateFunc_ );
      }
      else if(name && name[0] && strcmp("ISOcoated_v2_bas.ICC",name) == 0)
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_basICColor_ISOcoated_v2_bas_ICC_len,
                             (const oyPointer)_usr_share_color_icc_basICColor_ISOcoated_v2_bas_ICC,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-ISOcoated_v2_bas_ICC", oyAllocateFunc_ );
      }
      else if(name && name[0] && strcmp("ITULab.icc",name) == 0)
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_Oyranos_ITULab_icc_len,
                             (const oyPointer)_usr_share_color_icc_Oyranos_ITULab_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-ITULab_icc", oyAllocateFunc_ );
      }
      else if(name && name[0] && strcmp("LStar-RGB.icc",name) == 0)
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_basICColor_LStar_RGB_icc_len,
                             (const oyPointer)_usr_share_color_icc_basICColor_LStar_RGB_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-LStar_RGB_icc", oyAllocateFunc_ );
      }
      else if(name && name[0] && (strcmp("Lab.icc",name) == 0 || strcmp("LCMSLABI.ICM",name) == 0))
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_lcms_Lab_icc_len,
                             (const oyPointer)_usr_share_color_icc_lcms_Lab_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-Lab_icc", oyAllocateFunc_ );
      }
      else if(name && name[0] && (strcmp("XYZ.icc",name) == 0 || strcmp("LCMSXYZI.ICM",name) == 0))
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_lcms_XYZ_icc_len,
                             (const oyPointer)_usr_share_color_icc_lcms_XYZ_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-XYZ_icc", oyAllocateFunc_ );
      }
      else if(name && name[0] && strcmp("sRGB.icc",name) == 0)
      {
        s = (oyProfile_s_ *) oyProfile_FromMem( _usr_share_color_icc_OpenICC_sRGB_icc_len,
                             (const oyPointer)_usr_share_color_icc_OpenICC_sRGB_icc,
                             0,NULL );
        if(s)
          file_name = oyStringCopy( "inbuild-sRGB_icc", oyAllocateFunc_ );
      }
      /* END static inbuilds */
    }
#endif /* COMPILE_STATIC */

    if(error < -1)
    {
      oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)s,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"", OY_DBG_ARGS_,
                _("Wrong ICC profile id detected"), t?t:OY_PROFILE_NONE );
      repair = 1;
    } else
    if(error == -1)
    {
      if(oy_debug == 1)
        oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)s,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"", OY_DBG_ARGS_,
                _("No ICC profile id detected"), t?t:OY_PROFILE_NONE );
      repair = 1;

    }

    /* set ICC profile ID */
    if(repair && !(flags & OY_NO_REPAIR))
    {
      error = oyProfile_GetMD5( (oyProfile_s*)s, OY_COMPUTE, md5 );
      if(oyIsFileFull_( file_name, "wb" ))
      {
        error = oyProfile_ToFile_( s, file_name );
        if(!error)
          oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)s,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"", OY_DBG_ARGS_,
                _("ICC profile id written"), t?t:OY_PROFILE_NONE );
      }
    }

    /* We expect a incomplete filename attached to s and try to correct this. */
    if(error <= 0 && !file_name && s && s->file_name_)
    {
      if(!file_name)
        file_name = oyFindProfile_( s->file_name_, flags );
      if(file_name && s->oy_->deallocateFunc_)
      {
        s->oy_->deallocateFunc_( s->file_name_ );
        s->file_name_ = 0;
      }
    }

    if(error <= 0 && file_name && s)
      s->file_name_ = oyStringCopy_( file_name, s->oy_->allocateFunc_ );

    if(error <= 0 && s && !s->file_name_)
      error = 1;
  }

  if(error <= 0 && s && entry)
  {
    if(!oyToNoCacheWrite_m(flags))
    {
      /* 3b.1. update cache entry */
      error = oyHash_SetPointer_( entry, (oyStruct_s*)s );
#if 0
    } else {
      int i = 0, n = 0, pos = -1;

      n = oyStructList_Count( (oyStructList_s*)oy_profile_s_file_cache_ );
      for( i = 0; i < n; ++i )
        if((oyStruct_s*)entry == oyStructList_Get_( oy_profile_s_file_cache_,i))
          pos = i;
      if(pos >= 0)
        oyStructList_ReleaseAt( (oyStructList_s*)oy_profile_s_file_cache_, pos );
#endif
    }
  }

  if(error >= 1 && s)
    oyProfile_Release( (oyProfile_s**)&s );

clean_oyProfile_FromFile_:

  oyHash_Release_( &entry );
  oyFree_m_( hash );

  if(file_name)
    oyFree_m_( file_name );

  return s;
}
/** @internal
 *  Function  oyProfile_TagsToMem_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(error <= 0)
  {
    size_t size_ = 0;

    oyPointer icc_header = 0;
    oyPointer icc_tagtable = 0;

    /* 1. header */
    icc_header = oyProfile_WriteHeader_( profile, &size_ );

    error = !icc_header;

    /* 2. tag table */
    if(error <= 0)
    {
      icc_tagtable = oyProfile_WriteTagTable_( profile, &size_ );
      error = !icc_tagtable;
    }

    /* 3. tags */
    if(error <= 0)
    {
      block = oyProfile_WriteTags_( profile, &size_, icc_header, icc_tagtable,
                                    allocateFunc );
      error = !block;
    }

    if(error <= 0)
    {
      *size = size_;
    }

    oyFree_m_(icc_header);
    oyFree_m_(icc_tagtable);
  }

  return block;
}

/** @internal
 *  Function  oyProfile_ToFile_
 *  @brief    Save from in memory profile to file
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
 */
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name )
{
  oyProfile_s_ * s = profile;
  int error = !s || !file_name;
  oyPointer buf = 0;
  size_t size = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0)
  {
    buf = oyProfile_GetMem ( (oyProfile_s*)s, &size, 0, 0 );
    if(buf && size)
    error = oyWriteMemToFile_( file_name, buf, size );

    if(buf) oyDeAllocateFunc_(buf);
    size = 0;
  }

  return error;
}

#include "oyConfig_s_.h"
/** @internal
 *  Function  oyProfile_Match_
 *  @memberof oyProfile_s
 *  @brief    Check if a profiles matches by some properties
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Match_        ( oyProfile_s_      * pattern,
                                       oyProfile_s_      * profile )
{
  int32_t match = 1;
  int i;
  icSignature pattern_sig, profile_sig;

  if(pattern && profile)
  {
    /*match = oyProfile_Equal_(pattern, profile);*/ /* too expensive */

    if(pattern->meta_)
    /** support meta tag patterns */
    {
      oyConfig_s * p_device = 0, * device = pattern->meta_;
      oyConfig_s_ * d = (oyConfig_s_*)device;
      oyOptions_s * old_db = 0;
      const char path_separator = OY_SLASH_C, key_separator = ',';
      int flags = 0;
      int32_t rank;

      p_device = oyConfig_FromRegistration( d->registration, 0 );

      /* oyConfig_Match assumes its options in device->db, so it is filled here.*/
      if(!oyOptions_Count( d->db ))
      {
        old_db = d->db;
        d->db = d->backend_core;
      }

      oyProfile_GetDevice( (oyProfile_s*) profile, p_device );
      rank = 0;

      if(pattern->file_name_ && strchr(pattern->file_name_,'*'))
        flags |= OY_MATCH_SUB_STRING;

      oyConfig_Match( p_device, device, path_separator, key_separator, flags, &rank );
      if(oyConfig_FindString( p_device, "OYRANOS_automatic_generated", "1" ) ||
         oyConfig_FindString( p_device, "OPENICC_automatic_generated", "1" ))
      {
        DBG_NUM2_S( "found OPENICC_automatic_generated: %d %s",
                    rank, strrchr(oyProfile_GetFileName((oyProfile_s*)profile,-1),'/')+1);
        /* substract serial number and accound for possible wrong model_id */
        if(oyConfig_FindString( p_device, "serial", 0 ))
          rank -= 13;
        else
          rank -= 2;
        DBG_NUM1_S("after serial && OPENICC_automatic_generated: %d", rank);
      }

      if(old_db)
        d->db = old_db;

      if(rank <= 0)
        match = 0;
      oyConfig_Release( &p_device );

    } else
    if(pattern->file_name_)
    /** support file name patterns */
    {
      const char * p_fn = profile->file_name_ ?
                          profile->file_name_ :
                          oyProfile_GetFileName( (oyProfile_s*) profile, -1);
      if(strstr(p_fn, pattern->file_name_) == NULL)
        match = 0;
    } else
    if(match)
    /** support signature patterns */
    {
      match = 1;
      for( i = 0; i < (int)oySIGNATURE_MAX; ++i)
      {
        pattern_sig = oyProfile_GetSignature( (oyProfile_s*)pattern, (oySIGNATURE_TYPE_e) i );
        profile_sig = oyProfile_GetSignature( (oyProfile_s*)profile, (oySIGNATURE_TYPE_e) i );

        if(pattern_sig && profile_sig && pattern_sig != profile_sig)
        {
          match = 0;
          break;
        }
      }
    }
  }

  return match;
}

/** @internal
 *  Function  oyProfile_Hashed_
 *  @memberof oyProfile_s
 *  @brief    Check if a profile has a hash sum computed
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Hashed_       ( oyProfile_s_      * s )
{
  if(s && s->type_ == oyOBJECT_PROFILE_S)
    return oyObject_Hashed_( s->oy_ );
  else
    return 0;
}

/** @internal
 *  Function  oyProfile_GetFileName_r
 *  @memberof oyProfile_s
 *  @brief    get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or
 *  from a Oyranos default profile, the associated filename will simply be
 *  retuned.
 *
 *  @param         profile             the profile
 *  @param         flags               see oyProfile_FromFile()
 *  @param         allocateFunc        memory allocator
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
char *       oyProfile_GetFileName_r ( oyProfile_s_      * profile,
                                       uint32_t            flags,
                                       oyAlloc_f           allocateFunc )
{
  char * name = 0;
  oyProfile_s * tmp = 0;
  oyProfile_s_ * s = profile;
  int error = !s;
  char ** names = 0;
  uint32_t count = 0, i = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
    if(s->file_name_)
    {
      name = s->file_name_;
    } else
    {
      names = oyProfileListGet_ ( NULL, flags, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(oyProfile_Equal( (oyProfile_s*)s, tmp ))
          {
            name = names[i];
            break;
          }

          oyProfile_Release( &tmp );
        }
      }

      oyProfile_Release( &tmp );

      name = oyFindProfile_( name, 0 );
      if(name)
      {
        s->file_name_ = oyStringCopy_( name, s->oy_->allocateFunc_ );
        oyDeAllocateFunc_( name );
      }
      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }
  }

  name = oyStringCopy_( s->file_name_, allocateFunc );

  return name;
}

/** @internal
 *  Function  oyProfile_GetTagByPos_
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag
 *
 *  Non thread save
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s_    * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = profile;
  int error = !profile;
  int n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && profile->type_ != oyOBJECT_PROFILE_S)
    error = 1;

  if(error <= 0)
  {
    s = profile;
    n = oyStructList_Count( profile->tags_ );
  }

  if(error <= 0 && n)
  {
    tag = (oyProfileTag_s*) oyStructList_GetRef( profile->tags_, pos );
    return tag;
  }

  /* parse the ICC profile struct */
  if(error <= 0 && s->block_)
  {
    icSignature magic = oyProfile_GetSignature( (oyProfile_s*)s, oySIGNATURE_MAGIC );
    icSignature profile_cmmId = oyProfile_GetSignature( (oyProfile_s*)s, oySIGNATURE_CMM );
    char profile_cmm[5] = {0,0,0,0,0};
    icProfile * ic_profile = s->block_;
    int min_icc_size = 132 + sizeof(icTag);

    error = (magic != icMagicNumber);

    profile_cmmId = oyValueUInt32( profile_cmmId );
    error = !memcpy( profile_cmm, &profile_cmmId, 4 );
    if(error) { WARNc_S("Unable to copy CMM name"); }
    profile_cmmId = 0;

    if(error <= 0 && s->size_ > (size_t)min_icc_size)
    {
      uint32_t tag_count = 0;
      icTag *tag_list = 0;
      int i = 0;
      oyProfileTag_s_ * tag_ = oyProfileTag_New_( 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      oyPointer tag_block = 0;

      oyStruct_AllocHelper_m_( tag_block, char, 132, tag_, return 0 );
      error = !memcpy( tag_block, s->block_, 132 );
      error = oyProfileTag_Set( (oyProfileTag_s*)tag_,
                                (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, &tag_block );
      if(error <= 0)
      {
        error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );
        if(error > 0)
          WARNc_S("Unable to copy CMM name");
      }

      if(0 == pos)
      {
        tag = oyProfileTag_Copy( (oyProfileTag_s*)tag_, 0 );
      }
      error = oyProfile_TagMoveIn_( s, (oyProfileTag_s**)&tag_, -1 );


      tag_count = oyValueUInt32( ic_profile->count );

      tag_list = (icTag*)&((char*)s->block_)[132];

      /* parse the profile and add tags to the oyProfile_s::tags_ list */
      for(i = 0; (size_t)i < tag_count; ++i)
      {
        icTag *ic_tag = &tag_list[i];
        size_t offset = oyValueUInt32( ic_tag->offset );
        size_t tag_size = oyValueUInt32( ic_tag->size );
        char *tmp = 0;
        char **texts = 0;
        int32_t texts_n = 0;
        int j;
        oySTATUS_e status = oyOK;
        icTagSignature sig = oyValueUInt32( ic_tag->sig );
        icTagTypeSignature tag_type = 0;

        oyProfileTag_s * tag_ = oyProfileTag_New( 0 );

        tag_block = 0;

        if((offset+tag_size) > s->size_)
          status = oyCORRUPTED;
        else
        {
          icTagBase * tag_base = 0;

          oyStruct_AllocHelper_m_( tag_block, char, tag_size, tag_, return 0 );
          tmp = &((char*)s->block_)[offset];
          error = !memcpy( tag_block, tmp, tag_size );
          if(error) { WARNc_S("Unable to copy CMM name"); }

          tag_base = (icTagBase*) tag_block;
          tag_type = oyValueUInt32( tag_base->sig );
        }

        error = oyProfileTag_Set( tag_, sig, tag_type,
                                  status, tag_size, &tag_block );
        if(error <= 0)
          error = oyProfileTag_SetOffset( tag_, offset );
        if(error <= 0)
          error = oyProfileTag_SetCMM( tag_, profile_cmm );

        if(oy_debug > 3)
        {
          size_t size = 0;
          oyProfileTag_GetBlock( tag_, 0, &size, 0 );
          DBG_PROG5_S("%d[%d @ %d]: %s %s",
            i, (int)size, (int)oyProfileTag_GetOffset(tag_),
            oyICCTagTypeName( oyProfileTag_GetType(tag_) ),
            oyICCTagDescription( oyProfileTag_GetUse(tag_) ) );
          texts = oyProfileTag_GetText((oyProfileTag_s*)tag_,&texts_n,0,0,0,0);
          for(j = 0; j < texts_n; ++j)
            DBG_PROG2_S("%s: %s", oyProfileTag_GetLastCMM(tag_), texts[j]?texts[j]:"");
          if(texts_n && texts)
            oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
        }

        if(i == pos-1)
        {
          tag = oyProfileTag_Copy( tag_, 0 );
        }

        if(error <= 0)
          error = oyProfile_TagMoveIn_( s, &tag_, -1 );
      }
    }
  }

  return tag;
}

/** @internal
 *  Function  oyProfile_GetTagCount_
 *  @memberof oyProfile_s
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int                oyProfile_GetTagCount_ ( oyProfile_s_      * profile )
{
  int n = 0;
  oyProfile_s_ *s = profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && !s->tags_)
    error = 1;

  if(error <= 0)
    n = oyStructList_Count( s->tags_ );

  if(error <= 0 && !n)
  {
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );
  }

  return n;
}

/** @internal
 *  Function  oyProfile_TagMoveIn_
 *  @memberof oyProfile_s
 *  @brief    Add a tag to a profile
 *
 *  non thread save
 *
 *  The profile is needs probably be marked as modified after calling this
 *  function.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/12/29
 */
int          oyProfile_TagMoveIn_    ( oyProfile_s_      * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s_ * s = profile;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(error <= 0)
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );

  return error;
}

/** @internal
 *  Function  oyProfile_TagReleaseAt_
 *  @memberof oyProfile_s
 *  @brief    Remove a tag from a profile
 *
 *  Non thread save
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
int          oyProfile_TagReleaseAt_ ( oyProfile_s_      * profile,
                                       int                 pos )
{
  oyProfile_s_ * s = profile;
  return oyStructList_ReleaseAt ( s->tags_, pos );
}

/** @internal
 *  Function  oyProfile_WriteHeader_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.3.2
 *  @date    2011/07/05
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s_      * profile,
                                       size_t            * size )
{
  char * block = 0;
  char h[5] = {"head"};
  uint32_t * hi = (uint32_t*)&h;

  /* not sure where we could damage the profile block and need a parallel header block */
  if(profile && profile->block_ && profile->size_ > 132)
  {
    /* copy header with all modifications into new block */
    block = oyAllocateFunc_ (132);
    memset( block, 0, 132 );
    memcpy( block, profile->block_, 128 );
    *size = 132;

  } else
  if(profile && profile->tags_)
  {
    int n = oyProfile_GetTagCount_( profile );
    oyProfileTag_s_ * tag = (oyProfileTag_s_*)oyProfile_GetTagByPos_ ( profile, 0 );

    if(n && tag->use == *hi && tag->block_ && tag->size_ >= 128)
    {
      block = oyAllocateFunc_ (132);
      if(block)
      {
        memset( block, 0, 132 );
        memcpy( block, tag->block_, 128 );
        /* unset profile ID */
        memset( &block[84], 0, OY_HASH_SIZE );
        *size = 132;
      }
    }

    oyProfileTag_Release( (oyProfileTag_s**)&tag );
  }

  return (oyPointer)block;
}

/** @internal
 *  Function  oyProfile_WriteTags_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *
 *  Call in following order:
 *         -  oyProfile_WriteHeader_
 *         -  oyProfile_WriteTagTable_
 *         -  oyProfile_WriteTags_
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTags_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAlloc_f           allocateFunc )
{
  char * block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size &&
                icc_header && icc_list);

  if(error <= 0)
  {
    int n = 0, i;
    size_t len = 0;

    n = oyProfile_GetTagCount_( profile );
    block = (char*) oyAllocateFunc_(132 + n * sizeof(icTag));
    if(!block)
    { WARNc_S("Unable to allocate memory"); return NULL; }

    if(error <= 0)
    {
      memset( block, 0, 132 + n * sizeof(icTag) );
      error = !memcpy( block, icc_header, 132 );
      if(error) { WARNc_S("Unable to copy CMM name"); }
    }

    len = 132;

    if(error <= 0)
    {
      error = !memcpy( &block[len], icc_list, (n-1) * sizeof(icTag) );
      if(error) { WARNc_S("Unable to copy CMM name"); }
    }

    len += sizeof(icTag) * (n-1);

    if(!error)
    for(i = 0; i < n - 1; ++i)
    {
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char * temp = 0;
      icTagList* list = (icTagList*) &block[128];
      oyProfileTag_s_ * tag = (oyProfileTag_s_*)oyProfile_GetTagByPos_ ( profile, i + 1 );
      size_t size = 0;

      if(error <= 0)
        error = !tag;

      if(error <= 0)
        size = tag->size_;

      if(error <= 0 && tag->use == *hi)
      {
        oyProfileTag_Release( (oyProfileTag_s**)&tag );
        continue;
      }

      if(error <= 0)
      {
        list->tags[i].sig = oyValueUInt32( (icTagSignature)tag->use );
        list->tags[i].offset = oyValueUInt32( (icUInt32Number)len );
        list->tags[i].size = oyValueUInt32( (icUInt32Number)size );
        temp = (char*) oyAllocateFunc_ ( len + size +
                                               (size%4 ? 4 - size%4 : 0));
        if(temp)
          memset( temp, 0, len + size + (size%4 ? 4 - size%4 : 0));
        else
        {
          WARNc_S("Unable to allocate memory");
          oyFree_m_(block);
          oyProfileTag_Release( (oyProfileTag_s**)&tag );
          return NULL;
	}
      }

      if(error <= 0)
      {
        error = !memcpy( temp, block, len );
        if(error) { WARNc_S("Unable to copy CMM name"); }
      }
      if(error <= 0)
      {
        error = !memcpy( &temp[len], tag->block_, tag->size_);
        if(error) { WARNc_S("Unable to copy CMM name"); }
        len += size + (size%4 ? 4 - size%4 : 0);
      }

      if(error <= 0)
      {
        oyDeAllocateFunc_(block);
        block = temp;

        oyProfileTag_Release( (oyProfileTag_s**)&tag );
      }
      temp = 0;
    }

    if(error <= 0)
    {
      char h[5] = {OY_MODULE_NICK};
      uint32_t * hi = (uint32_t*)&h;
      icProfile* p = 0;
      icHeader* header = 0;
      oyPointer temp = oyAllocateWrapFunc_( len, allocateFunc );

      if(!temp)
      {
        oyFree_m_(block);
        return block;
      }
      if(error <= 0)
      {
        error = !memcpy( temp, block, len );
        if(error) { WARNc_S("Unable to copy CMM name"); }
      }

      oyDeAllocateFunc_( block );
      block = temp; temp = 0;

      p = (icProfile*) block;
      p->count = oyValueUInt32( (icUInt32Number) n - 1);

      header = (icHeader*) block;
      header->size = oyValueUInt32( (icUInt32Number) len);
#if 0 /* we dont override the CMM's id */
      header->creator = *hi;
#endif
#if defined(__APPLE__)
      oySprintf_( h, "APPL" );
#elif defined(_WIN32)
      oySprintf_( h, "MSFT" );
#else
      oySprintf_( h, "*nix" );
#endif
      header->platform = *hi;
      *size = len;
    }
  }

  return block;
}

/** @internal
 *  Function  oyProfile_WriteTagTable_
 *  @memberof oyProfile_s
 *  @brief    Get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTagTable_( oyProfile_s_      * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(error <= 0)
  {
    int n = oyProfile_GetTagCount_( profile );
    size_t size = 0;

    size = sizeof (icTag) * n;
    error = !size;

    if(error <= 0)
    {
      block = oyAllocateFunc_( size );
      error = !block;
    }

    if(error <= 0)
      error = !memset( block, 0, size );
  }

  return block;
}

#if 0
/** @brief get a CMM specific pointer
 *  @memberof oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
/*
oyChar *       oyProfile_GetCMMText_ ( oyProfile_s       * profile,
                                       oyNAME_e            type,
                                       const char        * language,
                                       const char        * country )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyChar * name = 0;
  char cmm_used[] = {0,0,0,0,0};

  if(error <= 0)
  {
    oyCMMProfile_GetText_t funcP = 0;
    oyPointer_s  * cmm_ptr = 0;


    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_CMM_API1_S,
                                      0, 0, cmm_used );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMProfile_GetText;
    }

    if(*(uint32_t*)&cmm_used)
      cmm_ptr = oyProfile_GetCMMPtr_( s, cmm_used );

    if(funcP && cmm_ptr)
    {
      name = funcP(cmm_ptr, type, language, country, s->oy_->allocateFunc_);

      oyCMMdsoRelease_( cmm_used );
    }
  }

  return name;
}
*/
#endif
