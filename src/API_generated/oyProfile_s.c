/** @file oyProfile_s.c

   [Template file inheritance graph]
   +-> oyProfile_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyProfile_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyProfile_s_.h"

#include "oyranos_devices_internal.h"
#include "oyranos_cache.h"
#include "oyranos_color.h"
#include "oyranos_color_internal.h"
#include "oyranos_io.h"
#include "oyranos_icc.h"
#include "oyConfig_s_.h"
#include "oyProfileTag_s_.h"
  


/** Function oyProfile_New
 *  @memberof oyProfile_s
 *  @brief   allocate a new Profile object
 */
OYAPI oyProfile_s * OYEXPORT
  oyProfile_New( oyObject_s object )
{
  oyObject_s s = object;
  oyProfile_s_ * profile = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  profile = oyProfile_New_( s );

  return (oyProfile_s*) profile;
}

/** Function  oyProfile_Copy
 *  @memberof oyProfile_s
 *  @brief    Copy or Reference a Profile object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     profile                 Profile struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyProfile_s* OYEXPORT
  oyProfile_Copy( oyProfile_s *profile, oyObject_s object )
{
  oyProfile_s_ * s = (oyProfile_s_*) profile;

  if(s)
  {
    oyCheckType__m( oyOBJECT_PROFILE_S, return NULL )
  }
  else
    return NULL;

  s = oyProfile_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyProfile_s" );

  return (oyProfile_s*) s;
}
 
/** Function oyProfile_Release
 *  @memberof oyProfile_s
 *  @brief   release and possibly deallocate a oyProfile_s object
 *
 *  @param[in,out] profile                 Profile struct object
 */
OYAPI int OYEXPORT
  oyProfile_Release( oyProfile_s **profile )
{
  oyProfile_s_ * s = 0;

  if(!profile || !*profile)
    return 0;

  s = (oyProfile_s_*) *profile;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  *profile = 0;

  return oyProfile_Release_( &s );
}



/* Include "Profile.public_methods_definitions.c" { */
/** Function  oyProfile_FromStd
 *  @memberof oyProfile_s
 *  @brief    Create from default color space settings
 *
 *  @param[in]    type           default color space
 *  @param[in]    flags          see -> oyProfile_FromFile()
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.9.6
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2014/04/04
 */
OYAPI oyProfile_s * OYEXPORT oyProfile_FromStd (
                                       oyPROFILE_e         type,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = 0;
  char * name = 0;

  if(type)
    name = oyGetDefaultProfileName ( type, oyAllocateFunc_ );

  if(name)
    s = oyProfile_FromFile_( name, flags, object );

  if(!s && name && name[0])
  {
    /* try some aliases */
    /* START Debian icc-profiles icc-profiles-icc */
    if(strcmp("XYZ.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "LCMSXYZI.ICM", flags, object );
    }
    else if(strcmp("Lab.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "LCMSLABI.ICM", flags, object );
    }
    else if(strcmp("LStar-RGB.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "eciRGB_v2.icc", flags, object );
    }
    else if(strcmp("sRGB.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "sRGB.icm", flags, object );
    }
    else if(strcmp("ISOcoated_v2_bas.ICC",name))
    {
      s = oyProfile_FromFile_( "ISOcoated_v2_eci.icc", flags, object );
      if(!s)
        s = oyProfile_FromFile_( "Fogra27L.icm", flags, object );
    }
    /* END Debian icc-profiles icc-profiles-icc */
  }

  if(s)
    s->use_default_ = type;
  else if(name && name[0])
  {
    int count = 0, i;
    char * text = 0;
    char ** path_names = oyProfilePathsGet_( &count, oyAllocateFunc_ );
    for(i = 0; i < count; ++i)
    {
      STRING_ADD( text, path_names[i] );
      STRING_ADD( text, "\n" );
    }

    if(strcmp("LCMSXYZI.ICM",name) == 0 ||
       strcmp("LCMSLABI.ICM",name) == 0 ||
       strcmp("LStar-RGB.icc",name) == 0 ||
       strcmp("sRGB.icc",name) == 0 ||
       strcmp("ISOcoated_v2_bas.ICC",name) == 0
      )
    {
      oyMessageFunc_p( oyMSG_ERROR, NULL,
                       OY_DBG_FORMAT_ "\n\t%s: \"%s\"\n\t%s\n\t%s\n%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"),name,
                _("You can get them from http://sf.net/projects/openicc"),
                _("install in the OpenIccDirectory icc path"), text );
    } else
    {
      const char * t = NULL;
      oyMSG_e msg_type = oyMSG_ERROR;
      if(!name || !name[0])
        name = oyGetDefaultProfileName ( type, oyAllocateFunc_ );
      oyWidgetTitleGet( (oyWIDGET_e) type, NULL, &t, NULL, NULL );
      /* without name from core we can ignore proof, effect and above profile types */
      if(type >= oyPROFILE_PROOF && (!name || !name[0]))
        msg_type = oyMSG_DBG;
      oyMessageFunc_p( msg_type, NULL,
                       OY_DBG_FORMAT_ "\n\t%s \"%s\": \"%s\"\n\t%s\n%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"), t, name,
                name&&name[0]?_("install in the OpenIccDirectory icc path"):"", name&&name[0]?text:"" );
    }
  }

  oyProfile_GetID( (oyProfile_s*)s );

  if(name)
    oyFree_m_( name );

  return (oyProfile_s*)s;
}

oyProfile_s * oyProfile_FromMatrix   ( double              pandg[9],
                                       int                 icc_profile_flags,
                                       const char        * name  )
{
  oyProfile_s * p = 0;
  oyOption_s * primaries = oyOption_FromRegistration( "//" 
                    OY_TYPE_STD 
                    "/color_matrix."
                    "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                    0);
  oyOptions_s * opts = oyOptions_New(0),
              * result = 0;
  int pos = 0, error OY_UNUSED;

  /* red */
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  /* green */
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  /* blue */
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  /* white */
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  /* gamma */
  oyOption_SetFromDouble( primaries, pandg[pos], pos, 0 ); pos++;
  error = oyOptions_MoveIn( opts, &primaries, -1 );
  error = oyOptions_SetFromInt( &opts,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    icc_profile_flags, 0, OY_CREATE_NEW );

  oyOptions_Handle( "//"OY_TYPE_STD"/create_profile.icc",
                               opts,"create_profile.icc_profile.color_matrix",
                               &result );
  p = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                       oyOBJECT_PROFILE_S );

  error = oyProfile_AddTagText( p, icSigProfileDescriptionTag, name);
  error = oyProfile_AddTagText( p, icSigCopyrightTag, "ICC License 2011");

  oyOptions_Release( &result );
  oyOptions_Release( &opts );

  return p;
}


/** Function  oyProfile_FromName
 *  @memberof oyProfile_s
 *  @brief    Create from name
 *
 *  Supported are profiles with absolute path names, profiles in OpenICC 
 *  profile paths and profiles relative to the current working path. 
 *  Search will occure in this order. Hash names, internal descriptions 
 *  and wildcards can be used.
 *
 *  @param[in]    name           name can be profile file name, ICC ID, 
 *                               wildcard or internal description.
 *  Following are some examples.
 *  A possible file name could be "sRGB.icc". A valid ICC ID would be
 *  "7fb30d688bf82d32a0e748daf3dba95d".
 *  A internal description could be "sRGB".
 *  Supported name wildcards are
 *  - "rgb" for default editing RGB
 *  - "cmyk" for default editing CMYK profile
 *  - "gray" for default editing Gray profile
 *  - "lab" for default editing CIE*Lab profile
 *  - "xyz" for default editing CIE*XYZ profile
 *  - "web" for default web profile sRGB
 *  - "rgbi" for default assumed RGB profile
 *  - "cmyki" for default assumed CMYK profile
 *  - "grayi" for default assumed Gray profile
 *  - "labi" for default assumed CIE*Lab profile
 *  - "xyzi" for default assumed CIE*XYZ profile
 *  - "proof" for default simulation profile
 *  - "effect" for default effect profile
 *  @param[in]    flags          flags are OY_NO_CACHE_READ, OY_NO_CACHE_WRITE, OY_COMPUTE
 *  - ::OY_NO_CACHE_READ and ::OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *  - ::OY_COMPUTE lets newly compute ID
 *  - ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *  - ::OY_NO_REPAIR skip automatic adding a ID hash if missed, useful for pure analysis
 *  - ::OY_SKIP_NON_DEFAULT_PATH ignore profiles outside of default paths
 *  @param[in]    object         the optional base
 *
 *  @see oyProfile_FromFile() and oyProfile_FromMD5()
 *
 *  @code
    // get editing "rgb" profile for selected / configured CMM
    oyOptions_s * options = NULL; // the selection options can come from command line
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                                                        "//" OY_TYPE_STD "/icc_color",
                                                                        options, 0 );
    oyProfile_s * p = oyProfile_FromName( "rgb", icc_profile_flags, NULL );
    @endcode
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/05/06
 *  @since   2014/05/06 (Oyranos: 0.9.6)
 */
OYAPI oyProfile_s * OYEXPORT oyProfile_FromName (
                                const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s * s = NULL;
  uint32_t md5[4];
  int old_oy_warn_ = oy_warn_;

  if(!name)
  {
    oyMessageFunc_p( oyMSG_WARN,(oyStruct_s*)NULL,
                     OY_DBG_FORMAT_ "\"name\" arg missed", OY_DBG_ARGS_ );
    return NULL;
  }

  /* try file name */
  oy_warn_ = 0;
  s = oyProfile_FromFile( name, flags, object );
  oy_warn_ = old_oy_warn_;

  if(s)
    return s;

  /* try ICC ID */
  if(!s && name && strlen(name) == 32)
  {
    if(oy_debug)
      fprintf(stderr, "try ICC ID: \"%s\"\n", name);
    sscanf(name, "%08x%08x%08x%08x", &md5[0],&md5[1],&md5[2],&md5[3] );
    s = oyProfile_FromMD5( md5, flags, object );
  }

  /* try wildcard */
  if(!s)
  {
    char * fn = NULL;
    if(oy_debug)
      fprintf(stderr, "try wildcard: \"%s\"\n", name);
    if(strcmp(name,"rgb") == 0)
      fn = oyGetDefaultProfileName( oyEDITING_RGB, oyAllocateFunc_ );
    else if(strcmp(name,"web") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_WEB, oyAllocateFunc_ );
    else if(strcmp(name,"cmyk") == 0)
    {
      fn = oyGetDefaultProfileName( oyEDITING_CMYK, oyAllocateFunc_ );
      if(!fn) { WARNc2_S("%s \"%s\"", _("Default profile not found:"), name ); }
      else if(oy_debug)
        fprintf(stderr, "Found: %s \"%s\"\n", name, fn);
    }
    else if(strcmp(name,"gray") == 0)
      fn = oyGetDefaultProfileName( oyEDITING_GRAY, oyAllocateFunc_ );
    else if(strcmp(name,"lab") == 0)
      fn = oyGetDefaultProfileName( oyEDITING_LAB, oyAllocateFunc_ );
    else if(strcmp(name,"xyz") == 0)
      fn = oyGetDefaultProfileName( oyEDITING_XYZ, oyAllocateFunc_ );
    else if(strcmp(name,"xyze") == 0)
    { double primaries_and_gamma[9]={ 1.0,0.0, 0.0,1.0, 0.0,0.0, 0.5,0.5, 1.0};
      s = oyProfile_FromMatrix( primaries_and_gamma, flags, "XYZ D*E" );
    }
    else if(strcmp(name,"rgbi") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_RGB, oyAllocateFunc_ );
    else if(strcmp(name,"cmyki") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_CMYK, oyAllocateFunc_ );
    else if(strcmp(name,"grayi") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_GRAY, oyAllocateFunc_ );
    else if(strcmp(name,"labi") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_LAB, oyAllocateFunc_ );
    else if(strcmp(name,"xyzi") == 0)
      fn = oyGetDefaultProfileName( oyASSUMED_XYZ, oyAllocateFunc_ );
    else if(strcmp(name,"proof") == 0)
      fn = oyGetDefaultProfileName( oyPROFILE_PROOF, oyAllocateFunc_ );
    else if(strcmp(name,"effect") == 0)
      fn = oyGetDefaultProfileName( oyPROFILE_EFFECT, oyAllocateFunc_ );

    if(fn)
    {
      s = oyProfile_FromFile( fn, flags, object );
      oyFree_m_( fn );
    }
  }

  /* try internal name */
  if(!s)
  {
    oyProfile_s * p = 0;
    char ** names = NULL;
    uint32_t count = 0, i;
    const char * t = 0;

    names = /*(const char**)*/ oyProfileListGet ( NULL, &count, oyAllocateFunc_ );

    for(i = 0; (uint32_t)i < count; ++i)
    {
      p = oyProfile_FromFile( names[i], flags ,0 );

      t = oyProfile_GetText(p, oyNAME_DESCRIPTION);
      if(t && strcmp(t,name) == 0)
      {
        s = p;
        break;
      }
      oyProfile_Release( &p );
    }

    oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  }

  if(!s)
    WARNc2_S("%s \"%s\"", _("profile not found:"), name );

  return s;
}

/** Function  oyProfile_FromFile
 *  @memberof oyProfile_s
 *  @brief    Create from file
 *
 *  Supported are profiles with absolute path names, profiles in OpenICC 
 *  profile paths and profiles relative to the current working path. 
 *  Search will occure in this order.
 *
 *  @param[in]    name           profile file name
 *  @param[in]    flags          OY_NO_CACHE_READ, OY_NO_CACHE_WRITE, OY_COMPUTE - compute ID, OY_NO_REPAIR
 *  @param[in]    object         the optional base
 *
 *  flags supports 
 *  - ::OY_NO_CACHE_READ and ::OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *  - ::OY_COMPUTE lets newly compute ID
 *  - ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *  - ::OY_NO_REPAIR skip automatic adding a ID hash if missed, useful for pure analysis
 *  - ::OY_SKIP_NON_DEFAULT_PATH ignore profiles outside of default paths
 *  - ::OY_NO_LOAD do not load profile, create path name fragment; with a given "meta:" prefix, 
 *    the string is parsed right after the prefix as key until the semicolon ';' sign appears.
 *    after that the string is taken as value. A star '*', at begin or end, is kept in the profile
 *    name property for following sub string matching, but it is removed from the value string.
 *  - ::OY_SKIP_MTIME_CHECK ignore checking time stamp; old bahaviour for faster caching 
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/06/03
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromFile            ( const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s_ * s = 0;

  if(flags & OY_NO_LOAD)
  {
    if(!name)
    {
      oyMessageFunc_p( oyMSG_ERROR, NULL,
                       OY_DBG_FORMAT_ "\"name\" arg missed",OY_DBG_ARGS_);
      return NULL;
    }

    s = (oyProfile_s_*) oyProfile_New(object);
    if(!s)
    {
      oyMessageFunc_p( oyMSG_ERROR, NULL,
                       OY_DBG_FORMAT_ "oyProfile_New() failed", OY_DBG_ARGS_);
      return NULL;
    }

    s->file_name_ = oyStringCopy_( name, s->oy_->allocateFunc_ );

    if(strstr(name,"meta:") != 0)
    {
      oyRankMap * rank_map = 0;
      const char * rank_map_text = "{\"org\":{\"freedesktop\":{\"openicc\":{\"rank_map\":{\"meta\":[{\"%s\": [2,-1,0,\"matching\",\"not matching\",\"key not found\"]}]}}}}}";
      const char * filter_text = "{\"org\":{\"freedesktop\":{\"openicc\":{\"config\":{\"meta\":[{\"%s\": \"%s\"}]}}}}}",
                 * filter_registration = "org/freedesktop/openicc/config/meta";

      char * key = oyStringCopy( strstr(name,"meta:") + 5, oyAllocateFunc_ );
      char * value = strchr(key, ';'), * jrank = NULL, * jfilter = NULL;
      if(value)
      {
        char * t = oyStringCopy( value + 1, oyAllocateFunc_ );
        value[0] = '\000';
        oyjlStringReplace( &t, "*", "", NULL,NULL );
        value = t;
        if(!value)
        {
          value = t;
          value[0] = '\000';
        }
      } else
      {
        oyMessageFunc_p( oyMSG_WARN, NULL,
                       OY_DBG_FORMAT_ "could not parse value: %s %d\n"
                       "need a string of form: \"meta:key;value\"",
                       OY_DBG_ARGS_, name, flags );
        oyFree_m_(key);
        oyProfile_Release( (oyProfile_s**)&s );
        return (oyProfile_s*)s;
      }

      oyStringAddPrintf( &jrank, oyAllocateFunc_,oyDeAllocateFunc_,
                         rank_map_text, key );
      oyStringAddPrintf( &jfilter, oyAllocateFunc_,oyDeAllocateFunc_,
                         filter_text, key, value );

      oyConfig_FromJSON( filter_registration, jfilter, 0,0, &s->meta_ );
      oyRankMapFromJSON( jrank, NULL, &rank_map, malloc );
      oyConfig_SetRankMap( s->meta_, rank_map );
      oyRankMapRelease( &rank_map, free );
      oyFree_m_(jfilter);
      oyFree_m_(jrank );
      oyFree_m_(key);
      oyFree_m_(value);
    }

    if(oy_debug)
      oyMessageFunc_p( oyMSG_DBG, NULL,
                       OY_DBG_FORMAT_ "path only profile: %s %d",
                       OY_DBG_ARGS_, name, flags );
    return (oyProfile_s*)s;
  }

  s = oyProfile_FromFile_( name, flags, object );

  if(flags & OY_COMPUTE)
  /* gives a warning message in case of error */
  { int r OY_UNUSED = oyProfile_GetHash_( s, flags ); }

  oyProfile_GetID( (oyProfile_s*)s );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromMem
 *  @memberof oyProfile_s
 *  @brief    Create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromMem             ( size_t            size,
                                const oyPointer   block,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s_ * s = 0;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 0;

  if(block && size)
  {
    oyAllocHelper_m_( block_, char, size, object ? object->allocateFunc_:0,
                      error = 1 );

    if(!error)
    {
      size_ = size;
      error = !memcpy( block_, block, size );
    }
  }

  s = oyProfile_FromMemMove_( size_, &block_, flags, &error, object );

  oyProfile_GetID( (oyProfile_s*)s );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromSignature
 *  @memberof oyProfile_s
 *  @brief    Create a fractional profile from signature
 *
 *  @param[in]    sig            signature
 *  @param[in]    type           type of signature to set
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
             oyProfile_FromSignature(  icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = oyProfile_New_( object );
  int error = !s;

  if(error <= 0)
    error = oyProfile_SetSignature( (oyProfile_s*)s, sig, type );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromMD5
 *  @memberof oyProfile_s
 *  @brief    Look up a profile from it's md5 hash sum
 *
 *  @param[in]    md5            hash sum
 *  @param[in]    flags          flags are OY_NO_CACHE_READ, OY_NO_CACHE_WRITE, OY_COMPUTE
 *  - ::OY_NO_CACHE_READ and ::OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *  - ::OY_COMPUTE lets newly compute ID
 *  - ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *  - ::OY_NO_REPAIR skip automatic adding a ID hash if missed, useful for pure analysis
 *  - ::OY_SKIP_NON_DEFAULT_PATH ignore profiles outside of default paths
 *  @param[in]    object         the optional base
 *  @return                      a profile
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/01/03
 *  @since   2009/03/20 (Oyranos: 0.1.10)
 */
OYAPI oyProfile_s * OYEXPORT oyProfile_FromMD5 (
                                       uint32_t          * md5,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s * s = 0, * tmp = 0;
  int error = !md5,
      equal = 0;
  char ** names = 0;
  uint32_t count = 0, i = 0;

  if(error)
    return 0;

  if(error <= 0)
  {
    names = oyProfileListGet_ ( NULL, 0, &count );

    for(i = 0; i < count; ++i)
    {
      if(names[i])
      {
        if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          /* ICC ID's are not relyable so we recompute it here */
          tmp = oyProfile_FromFile( names[i], flags, object );

        if(!tmp)
          continue;

        if(tmp->oy_->hash_ptr_)
          equal = memcmp( md5, tmp->oy_->hash_ptr_, OY_HASH_SIZE );
        else
          equal = 1;
        if(equal == 0)
        {
          s = tmp;
          break;
        }

        oyProfile_Release( &tmp );
      }
    }

    oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  }

  return s;
}

/** Function oyProfile_FromTaxiDB
 *  @memberof oyProfile_s
 *  @brief   look up a profile of a device from Taxi DB
 *
 *  The function asks the online ICC Taxi DB for a profile. It is therefore
 *  blocking and can cause a serious delay before returning.
 *
 *  The TAXI_id option is expected to come from 
 *  oyConfig_GetBestMatchFromTaxiDB() or oyDevicesFromTaxiDB().
 *
 *  @param[in]     options             - "TAXI_id" shall provide a string
 *                                       for device driver parameter selection
 *  @param[in]     object              the optional object
 *  @return                            the resulting profile
 *
 *  @version Oyranos: 0.3.3
 *  @since   2012/01/08 (Oyranos: 0.3.3)
 *  @date    2012/01/08
 */
OYAPI oyProfile_s * OYEXPORT oyProfile_FromTaxiDB (
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  int error = !options;
  oyProfile_s * p = NULL;
  oyOptions_s * s = options;
  size_t size = 0;
  char * mem = NULL;
  const char * taxi_id = NULL;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return p )

  if(error > 0)
  {
    WARNc_S( "No options provided. Give up." );
    return p;
  }

  taxi_id = oyOptions_FindString( options, "TAXI_id", 0 );

  if(taxi_id)
    mem = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                            "https://icc.opensuse.org/profile/%s/profile.icc",
                            taxi_id );
  else
    WARNc_S("No TAXI_id provided, Do not know what to download.");

  if(mem && size)
  {
    p = oyProfile_FromMem( size, mem, 0, object );
    oyFree_m_( mem ); size = 0;
  }

  return p;
}

/** Function oyProfile_Install
 *  @memberof oyProfile_s
 *  @brief   Install a ICC profile
 *
 *  Without options, the ICC profile will be installed into the users 
 *  ICC profile path. For obtaining a useful path name see @ref path_names.
 *
 *  @param[in]     profile             the profile
 *  @param[in]     scope               the scope to install to; default is 
 *                                     oySCOPE_USER
 *  @param[in]     options
 *                                     - "path" can provide a string
 *                                       for manual path selection
 *                                     - "device" = "1" - write to device paths
 *                                     - "test" = "1" - skip write
 *  @return                            error
 *                                     - oyOK - success
 *                                     - >= 1  error
 *                                     - oyERROR_USER msg -> wrong or missing
 *                                       argument
 *                                     - oyERROR_DATA_WRITE msg -> path can not
 *                                       be written
 *                                     - oyERROR_DATA_AMBIGUITY msg ->
 *                                       "Profile already installed"
 *                                     - oyCORRUPTED msg -> profile not useable
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/04/04
 *  @since   2012/01/13 (Oyranos: 0.9.1)
 */
OYAPI int OYEXPORT oyProfile_Install ( oyProfile_s       * profile,
                                       oySCOPE_e           scope,
                                       oyOptions_s       * options )
{
  int error = !profile ? oyERROR_USER : 0;
  oyProfile_s * s = profile,
              * p = 0;
  size_t size = 0;
  char * data = 0;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  const char * t = 0;
  char * desc = 0;
  char * fn = 0;
  char * pn = 0;
  int test = oyOptions_FindString( options, "test", "1" ) != NULL;

  oyCheckType__m( oyOBJECT_PROFILE_S, return oyERROR_USER )

  if(error > 0)
  {
    WARNc_S( "No profile provided for installation. Give up." );
    return error;
  }

  /** 1. construct a profile name */
  desc = oyStringCopy_( oyProfile_GetText( s, oyNAME_DESCRIPTION ), oyAllocateFunc_ );

  if(desc && desc[0])
  {
    char * ext = 0;
    int len;

    len = strlen( desc );
    for(i = 0; (int)i < len; ++i)
      if(desc[i] == OY_SLASH_C)
        desc[i] = '-';

    /** 1.1 add user profile path name by default or custom from "path" option
     */
    if(oyOptions_FindString( options, "path", 0 ) != NULL)
    {
      STRING_ADD( fn, oyOptions_FindString( options, "path", 0 ) );
      STRING_ADD( fn, OY_SLASH );
    } else
    {
      fn = oyGetInstallPath( oyPATH_ICC, scope, oyAllocateFunc_ );
      if(fn && fn[ strlen(fn) - 1 ] != OY_SLASH_C)
        STRING_ADD( fn, OY_SLASH );
    }

    /** 1.2 for "device" = "1" option add 
     *      xxx/devices/device_class_description_xxx/ */
    if(oyOptions_FindString( options, "device", "1" ) != NULL)
    {
      STRING_ADD( fn, "devices" OY_SLASH );
      STRING_ADD( fn, oyICCDeviceClassDescription(
                      oyProfile_GetSignature( s, oySIGNATURE_CLASS ) ) );
      STRING_ADD( fn, OY_SLASH );
    }

    /** 1.3 add ".icc" suffix as needed */
    t = strrchr(desc, '.');
    STRING_ADD( pn, desc );
    if(t)
    {
      ++t;
      STRING_ADD( ext, t );
      if(oyStringCaseCmp_(ext,"icc") != 0 &&
         oyStringCaseCmp_(ext,"icm") != 0)
      {
        oyFree_m_(ext);
        ext = 0;
      }
    }
    if(!ext)
      STRING_ADD( pn, ".icc" );
    else
      oyFree_m_(ext);

    STRING_ADD( fn, pn );
  } else
  {
    WARNcc1_S( s, "%s",_("Profile contains no description") );
    return oyCORRUPTED;
  }

  /** 2. check if file or description name exists */
  names = oyProfileListGet_ ( NULL, 0, &count );
  for(i = 0; (uint32_t)i < count; ++i)
  {
    p = oyProfile_FromFile(names[i], 0,0);
    t = oyProfile_GetText(p, oyNAME_DESCRIPTION);

    if(t && oyStringCaseCmp_(desc, t) == 0)
    {
      WARNcc2_S( s, "%s: \"%s\"", _("Profile already installed"), t );
      error = oyERROR_DATA_AMBIGUITY;
    }

    if(names[i] && oyStringCaseCmp_(names[i], fn) == 0)
    {
      WARNcc2_S( s, "%s: \"%s\"", _("Profile already installed"), names[i] );
      error = oyERROR_DATA_AMBIGUITY;
    }

    oyProfile_Release( &p );

    if(error != 0) break;
  }

  oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  if(error != 0)
    goto install_cleanup;

  p = oyProfile_FromFile( pn, OY_SKIP_NON_DEFAULT_PATH, 0 );
  if(p)
  {
    WARNcc2_S(s, "%s: \"%s\"", _("Profile exists already"), pn);
    error = oyERROR_DATA_AMBIGUITY;
    goto install_cleanup;
  }

  /** 3. open profile */
  data = oyProfile_GetMem( s, &size, 0, oyAllocateFunc_ );
  if(data && size && !test)
  {
    /** 3.1 write profile */
    error = oyProfile_ToFile_( (oyProfile_s_*)s, fn );
    if(error)
    {
      WARNcc2_S(s, "%s: \"%s\"", _("Can not write profile"), fn);
      error = oyERROR_DATA_WRITE;
    } else
    {
      oyProfile_s_ * s_ = (oyProfile_s_*)s;
      if(s_->file_name_)
        oyDeAllocateFunc_( s_->file_name_ );
      s_->file_name_ = oyStringCopy_( fn, s_->oy_->allocateFunc_ );
    }
  } else if(!test)
  {
    WARNcc1_S( s, "%s",_("Could not open profile") );
    error = oyERROR_DATA_READ;
  }

  install_cleanup:
  if(desc) oyFree_m_(desc);
  if(pn) oyFree_m_(pn);
  if(fn) oyFree_m_(fn);
  if(data) oyFree_m_(data);
  oyProfile_Release( &p );

  return error;
}

/** Function  oyProfile_GetChannelsCount
 *  @memberof oyProfile_s
 *  @brief    Number of channels in a color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI int OYEXPORT
oyProfile_GetChannelsCount( oyProfile_s * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->channels_n_)
    return s->channels_n_;

  s->channels_n_ = oyICCColorSpaceGetChannelCount( s->sig_ );

  return s->channels_n_;
}

/** Function  oyProfile_GetSignature
 *  @memberof oyProfile_s
 *  @brief    Get ICC color space signature
 *
 *  @code
    // show some profile properties
    oyProfile_s * p = ...; // get from somewhere
    icSignature vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );      
    char * v = (char*)&vs;
    printf("  created %d.%d.%d %d:%d:%d\n", 
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_YEAR),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MONTH),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_DAY),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_HOURS),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MINUTES),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_SECONDS)
          );
    printf("  pcs: %s  color space: %s version: %d.%d.%d\n", 
          oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PCS) ),
          oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_COLOR_SPACE) ),
          (int)v[0], (int)v[1]/16, (int)v[1]%16
          );
    @endcode
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI icSignature OYEXPORT
oyProfile_GetSignature ( oyProfile_s * profile,
                         oySIGNATURE_TYPE_e type )
{
  icHeader *h = 0;
  icSignature sig = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  icUInt64Number ui64;
  icUInt32Number * two32 = (icUInt32Number *) &ui64;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->sig_ && type == oySIGNATURE_COLOR_SPACE)
    return s->sig_;

  if(!s->block_)
  {
    if(type == oySIGNATURE_COLOR_SPACE)
      sig = s->sig_ = icSigXYZData;
    return sig;
  }

  h = (icHeader*) s->block_;

  switch(type)
  {
  case oySIGNATURE_COLOR_SPACE:       /* color space */
       sig = s->sig_ = oyValueCSpaceSig( h->colorSpace ); break;
  case oySIGNATURE_PCS:                /* profile connection space */
       sig = oyValueCSpaceSig( h->pcs ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       sig = oyValueUInt32( h->size ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       sig = oyValueUInt32( h->cmmId ); break;
  case oySIGNATURE_VERSION:            /* version */
       sig = oyValueUInt32( h->version ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       sig = oyValueUInt32( h->deviceClass ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       sig = oyValueUInt32( h->magic ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       sig = oyValueUInt32( h->platform ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       sig = oyValueUInt32( h->flags ); break;
  case oySIGNATURE_ATTRIBUTES:         /* various ICC header attributes */
       memcpy( ui64, h->attributes, sizeof(ui64));
       sig = (icSignature) two32[0]; break;
  case oySIGNATURE_ATTRIBUTES2:        /* various ICC header attributes */
       memcpy( ui64, h->attributes, sizeof(ui64));
       sig = (icSignature) two32[1]; break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       sig = oyValueUInt32( h->manufacturer ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       sig = oyValueUInt32( h->model ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       sig = oyValueUInt32( h->renderingIntent ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       sig = oyValueUInt32( h->creator ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       sig = oyValueUInt16( h->date.year ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.month ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       sig = oyValueUInt16( h->date.day ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.hours ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.minutes ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.seconds ); break;
  case oySIGNATURE_ILLUMINANT:         /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.X ); break;
  case oySIGNATURE_ILLUMINANT_Y:       /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.Y ); break;
  case oySIGNATURE_ILLUMINANT_Z:       /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.Z ); break;
  case oySIGNATURE_MAX: break;
  }

  return sig;
}

/** Function  oyProfile_SetSignature
 *  @memberof oyProfile_s
 *  @brief    Set signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     sig                 signature
 *  @param[in]     type                type of signature to set
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;
  icUInt32Number * two32;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0 && type == oySIGNATURE_COLOR_SPACE)
  {
    if(sig)
      s->sig_ = sig;
    else
      s->sig_ = icSigXYZData;
    return error;
  }

  if(error <= 0 && !s->block_)
  {
    oyStruct_AllocHelper_m_( block_, char, size_, s, error = 1 );

    if(error <= 0)
    {
      s->block_ = block_;
      s->size_ = size_;
    }
  }

  if(error <= 0)
    h = (icHeader*) s->block_;

  if(error <= 0)
  switch(type)
  {
  case oySIGNATURE_COLOR_SPACE:       /* color space */
       break; /* already handled */
  case oySIGNATURE_PCS:                /* profile connection space */
       h->pcs = oyValueCSpaceSig( sig ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       h->size = oyValueUInt32( sig ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       h->cmmId = oyValueUInt32( sig ); break;
  case oySIGNATURE_VERSION:            /* version */
       h->version = oyValueUInt32( sig ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       h->deviceClass = oyValueUInt32( sig ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       h->magic = oyValueUInt32( sig ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       h->platform = oyValueUInt32( sig ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       h->flags = oyValueUInt32( sig ); break;
  case oySIGNATURE_ATTRIBUTES:         /* various ICC header attributes */
       two32 = (icUInt32Number *) h->attributes;
       two32[0] = oyValueUInt32( sig ); break;
  case oySIGNATURE_ATTRIBUTES2:        /* various ICC header attributes */
       two32 = (icUInt32Number *) h->attributes;
       two32[1] = oyValueUInt32( sig ); break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       h->manufacturer = oyValueUInt32( sig ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       h->model = oyValueUInt32( sig ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       h->renderingIntent = oyValueUInt32( sig ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       h->creator = oyValueUInt32( sig ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       h->date.year = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       h->date.month = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       h->date.day = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       h->date.hours = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       h->date.minutes = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       h->date.seconds = oyValueUInt16( sig ); break;
  case oySIGNATURE_ILLUMINANT:         /* creation time in UTC */
       h->illuminant.X = oyValueInt32( sig ); break;
  case oySIGNATURE_ILLUMINANT_Y:       /* creation time in UTC */
       h->illuminant.Y = oyValueInt32( sig ); break;
  case oySIGNATURE_ILLUMINANT_Z:       /* creation time in UTC */
       h->illuminant.Z = oyValueInt32( sig ); break;
  case oySIGNATURE_MAX: break;
  }

  return error;
}

/** Function  oyProfile_SetChannelNames
 *  @memberof oyProfile_s
 *  @brief    Set channel names
 *
 *  The function should be used to specify extra channels or unusual color
 *  layouts like CMYKRB. The number of elements in names_chan should fit to the
 *  channels count or to the color space signature.
 *
 *  You can let single entries empty if they are understandable by the
 *  color space signature. Oyranos will set them for you on request.
 *
 *  @param[in]     profile             profile
 *  @param[in]     names_chan          pointer to channel names 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI void OYEXPORT
oyProfile_SetChannelNames            ( oyProfile_s       * profile,
                                       oyObject_s        * names_chan )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = !s;

  if(error)
    return;

  oyCheckType__m( oyOBJECT_PROFILE_S, return )

  if(names_chan && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    s->names_chan_[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
      {
        s->names_chan_[i] = oyObject_Copy( names_chan[i] );
      }
  }
}

/** Function  oyProfile_GetChannelNames
 *  @memberof oyProfile_s
 *  @brief    Get channel names
 *
 *  @param[in]     profile             the profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
OYAPI const oyObject_s * OYEXPORT
oyProfile_GetChannelNames           ( oyProfile_s   * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = 0;
  icColorSpaceSignature sig = oyProfile_GetSignature( profile, oySIGNATURE_COLOR_SPACE );

  if(!profile)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!s->names_chan_ && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    if(!s->names_chan_)
      error = 1;
    if(error <= 0)
    {
      s->names_chan_[ n ] = NULL;
      for( ; i < n; ++i )
      {
        s->names_chan_[i] = oyObject_NewFrom( s->oy_, "oyProfile_s::names_chan_" );
        if(!s->names_chan_[i])
          error = 1;
        else
          error = oyObject_SetNames( s->names_chan_[i],
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_NICK ),
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_NAME ),
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_DESCRIPTION )
                      );
      }
    }
  }

  if(error <= 0 && s->names_chan_)
    return (const oyObject_s*) s->names_chan_;
  else
    return 0;
}

/** Function  oyProfile_GetChannelName
 *  @memberof oyProfile_s
 *  @brief    Get a channels name
 *
 *  A convinience function to get a single name with a certain type.
 *
 *  @param[in] profile  address of a Oyranos named color structure
 *  @param[in] pos      position of channel 
 *  @param[in] type     sort of text 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI const char   * OYEXPORT
oyProfile_GetChannelName           ( oyProfile_s   * profile,
                                     int                pos,
                                     oyNAME_e           type )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( (oyProfile_s*)s );
  const oyChar * text = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if( 0 <= pos && pos < n )
    return "-";

  if(!s->names_chan_)
    oyProfile_GetChannelNames ( (oyProfile_s*)s );

  if(s->names_chan_ && s->names_chan_[pos])
    text = oyObject_GetName( s->names_chan_[pos], type );

  return text;
}

/** Function  oyProfile_GetID
 *  @memberof oyProfile_s
 *  @brief    Get unique name
 *
 *  the returned string is identical to oyNAME_NICK
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  const oyChar * text = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    text = oyObject_GetName( s->oy_, oyNAME_NICK );

  if(error <= 0 && !text)
  {
    char * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, char, 1024, 0, error = 1 );

#if 0
    /* A short number represents a default profile,
     * The length should not exceed OY_HASH_SIZE.
     *
     * Default profiles can change. Thus this kind of ID is not unique.
     */
    if(s->use_default_ && !found && error <= 0)
    {
      oySprintf_(temp, "%d", s->use_default_);
      if(oyStrlen_(temp) < OY_HASH_SIZE)
        found = 1;
    }
#endif

    /* Do we have a file_name_? */
    if(s->file_name_ && !found && error <= 0)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(temp[0])
        found = 1;
    }

    /* Do we have a hash_? */
    if(error <= 0)
    {
      if(!oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s, 0 );

      if(error <= 0)
      {
        uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
        if(i)
          oySprintf_(!temp[0] ? temp : &temp[strlen(temp)], ":%08x%08x%08x%08x", i[0], i[1], i[2], i[3]);
        else if(!(temp && strlen(temp)))
          oySprintf_(!temp[0] ? temp : &temp[strlen(temp)], "                " );
        if(temp[0])
          found = 1;
      }
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp, oyNAME_NICK );

    oyFree_m_( temp );

    if(error <= 0)
      text = oyObject_GetName( s->oy_, oyNAME_NICK );
  }

  return text;
}

/** Function  oyProfile_Equal
 *  @memberof oyProfile_s
 *  @brief    Check if two profiles are equal by their hash sum
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/03 (Oyranos: 0.1.8)
 *  @date    2009/04/16
 */
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB )
{
  int equal = 0;

  if(profileA && profileB)
    equal = oyObject_HashEqual( profileA->oy_, profileB->oy_ );

  return equal;
}

/** @memberof oyProfile_s
 *  @brief    Get a presentable name
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XML element
 *  - oyNAME_NICK - the file name and hash ID
 *  - oyNAME_DESCRIPTION - profile internal name (icSigProfileDescriptionTag)
 *  - oyNAME_REGISTRATION - ICC MD5 hash sum in a string with 32 chars
 *  - oyNAME_JSON - contains non expensive "id", internal "name" + "hash" keys,
 *    "id" can use type, file name and ICC hash if already present
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/24
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 */
OYAPI const oyChar* OYEXPORT oyProfile_GetText (
                                       oyProfile_s       * profile,
                                       oyNAME_e            type )
{
  const char * text = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && type == oyNAME_NICK)
    text = oyProfile_GetID( (oyProfile_s*)s );

  if(error <= 0 && !text)
    if(type <= oyNAME_DESCRIPTION)
      text = oyObject_GetName( s->oy_, type );

  if(error <= 0 && !(text && text[0]))
  {
    char * temp = 0,
         * tmp2 = 0;
    int found = 0;

    oyAllocHelper_m_( temp, char, 1024, 0, error = 1 );

    /* Ask the CMM? */
    if(!found && error <= 0 &&
       type == oyNAME_DESCRIPTION)
    {
      {
        union { icTagSignature sig; char c[4]; } mluc_desc_v2 = { .c={'d','s','c','m'} };
        tag = oyProfile_GetTagById( (oyProfile_s*)s, oyValueUInt32( mluc_desc_v2.sig ) );
        if(!tag)
          tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileDescriptionTag );
        texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);

        if(texts_n && texts[0] && texts[0][0])
        {
          int i;
          char * select = NULL;
          if(texts_n == 1)
            select = texts[0];
          else
          for(i = 0; i < texts_n; ++i)
          {
            char * t = texts[i];
            if(t && (t[0] == 'e' || t[0] == 'E') && (t[1] == 'n' || t[1] == 'N') && (t[2] == '_' || t[2] == ':'))
            {
              if(strchr(t,':'))
                select = strchr(t,':') + 1;
              else
                select = t;
            }
          }
          if(!select && texts[0])
            select = texts[0];

          if(select)
          {
            memcpy(temp, select, oyStrlen_(select));
            temp[oyStrlen_(select)] = 0;
            found = 1;
          }

          oyStringListRelease_( &texts, texts_n, tag->oy_->deallocateFunc_ );
        } else
          /* we try to get something as oyNAME_NAME */
        if(s->file_name_ && s->file_name_[0])
        {
          size_t len = oyStrlen_(s->file_name_);
          if(strrchr(s->file_name_,'/'))
          {
            tmp2 = oyStrrchr_(s->file_name_,'/')+1;
            len = oyStrlen_( tmp2 );
            memcpy( temp, tmp2, len );
          } else
            memcpy( temp, s->file_name_, len );
          temp[len] = 0;
          found = 1;
        }
        oyProfileTag_Release( &tag );
      }
    }

    if(type == oyNAME_NAME)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      char * file_name = oyProfile_GetFileName_r( s, 0, oyAllocateFunc_ );

      if(oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s, 0 );

      if(s->use_default_ && error <= 0)
        oyWidgetTitleGet( (oyWIDGET_e)s->use_default_, 0, &text, 0, 0 );

      oySprintf_( temp, "<profile use_default=\"%s\" file_name=\"%s\" hash=\"",
             oyNoEmptyName_m_(text),
             oyNoEmptyName_m_(file_name) );
      if(i)
        oySprintf_( &temp[oyStrlen_(temp)], "%08x%08x%08x%08x\"",
             i[0], i[1], i[2], i[3] );
      else
        oySprintf_( &temp[oyStrlen_(temp)], "                \"" );
      if(!file_name)
      {
        const char * desc = oyProfile_GetText( (oyProfile_s*)s, oyNAME_DESCRIPTION );
        oySprintf_( &temp[oyStrlen_(temp)], " name=\"%s\"",
             oyNoEmptyName_m_(desc) );
      }
      oySprintf_( &temp[oyStrlen_(temp)], " />" );

      if(file_name) { free(file_name); file_name = 0; }
      found = 1;
    }
    else if(type == oyNAME_JSON)
    {
      oyjl_val root = oyjlTreeNew(""), val;
      const char * name = oyProfile_GetText( profile, oyNAME_DESCRIPTION );
      const char * id = oyProfile_GetFileName( profile, -1 );
      char * json = NULL; int i = 0;
      uint32_t * h = (uint32_t*)s->oy_->hash_ptr_;

      if(id)
      {
        val = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "id" );
        oyjlValueSetString( val, id );
      }
      if(name)
      {
        val = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "name" );
        oyjlValueSetString( val, name );
      }
      if(id || name)
        found = 1;
      if(h)
      {
        char * hash = NULL;
        oyjlStringAdd( &hash, 0,0, "%08x%08x%08x%08x",
             h[0], h[1], h[2], h[3] );
        val = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "hash" );
        oyjlValueSetString( val, hash );
        free(hash);
      }
      oyjlTreeToJson( root, &i, &json );
      if(json && strlen(json) < 1024)
      {
        strcpy(temp, json);
        free(json); json = NULL;
      }
      oyjlTreeFree( root );
    }
    else if(type == oyNAME_REGISTRATION)
    {
      uint32_t * h = (uint32_t*)s->oy_->hash_ptr_;
      if(h)
      {
        char * hash = NULL;
        oyFree_m_( temp );
        oyjlStringAdd( &hash, 0,0, "%08x%08x%08x%08x",
             h[0], h[1], h[2], h[3] );
        text = temp = hash;
        found = 1;
      }
    }

    if(!found)
    {
      text = oyProfile_GetID( (oyProfile_s*)s );
      if(text && text[0])
        found = 1;
    }

    /* last rescue */
    if(!found && oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s, 0 );

    if(!found && error <= 0)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      oySprintf_(temp, "%08x%08x%08x%08x", i[0], i[1], i[2], i[3]);
      if(temp[0])
        found = 1;
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp && temp[0] ? temp:text?text:"", type );

    oyFree_m_( temp );

    if(error <= 0)
      text = oyObject_GetName( s->oy_, type );
  }

  return text;
}

/** Function  oyProfile_GetMem
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile in memory
 *
 *  The prefered memory comes from the unmodified original memory.
 *  Otherwise a previously modified tag list is serialised into memory.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2010/04/16
 */
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s,
      i;
  uint32_t md5[4];
  char * data;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0 && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->size_ && s->block_ && !s->tags_modified_)
    {
      block = oyAllocateWrapFunc_( s->size_, allocateFunc );
      error = !block;
      if(error <= 0)
        error = !memcpy( block, s->block_, s->size_ );
      if(error <= 0 && size)
        *size = s->size_;

    } else
    if( oyStructList_Count( s->tags_ ))
    {
      block = oyProfile_TagsToMem_ ( s, size, allocateFunc );
      s->tags_modified_ = 0;
      s->use_default_ = 0;
      if(s->file_name_)
        profile->oy_->deallocateFunc_( s->file_name_ );
      s->file_name_ = 0;
      if(s->block_ && s->size_)
        profile->oy_->deallocateFunc_( s->block_ );
      s->size_ = 0;
      s->block_ = oyAllocateWrapFunc_( *size,
                                             profile->oy_->allocateFunc_ );
      error = !memcpy( s->block_, block, *size );
      if(error <= 0)
        s->size_ = *size;
      oyObject_SetNames( profile->oy_, 0,0,0 );
      oyProfile_GetText(profile, oyNAME_NICK);
      oyProfile_GetText(profile, oyNAME_NAME);
      oyProfile_GetText(profile, oyNAME_DESCRIPTION);
    }

    /* get actual ICC profile ID */
    oyProfile_GetMD5( profile, flag, md5 );

    /* Write ICC profile ID into memory */
    for(i = 0; i < 4; ++i)
      md5[i] = oyValueUInt32( md5[i] );
    data = block;
    if(data && (int)*size >= 132)
      memcpy( &data[84], md5, 16 );
  }

  if(s)
    oyObject_UnLock( s->oy_,__FILE__,__LINE__ );

  return block;
}
/** Function  oyProfile_GetSize
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile in memory size
 *
 *  The prefered memory comes from the unmodified original memory.
 *  Otherwise a previously modified tag list is serialised into memory.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2012/10/05 (Oyranos: 0.5.0)
 */
OYAPI size_t OYEXPORT
                   oyProfile_GetSize ( oyProfile_s       * profile,
                                       uint32_t            flag )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  size_t size = 0;
  char * data;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->size_ && s->block_ && !s->tags_modified_)
    {
      return s->size_;

    } else
    {
      data = oyProfile_GetMem( profile, &size, flag,
                               profile->oy_->allocateFunc_ );
      if(data && size)
      { profile->oy_->deallocateFunc_( data ); } data = 0;
    }
  }

  return size;
}

/** Function  oyProfile_GetTagByPos
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/06
 *  @since   2008/02/06 (Oyranos: 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                 oyProfile_GetTagByPos(oyProfile_s       * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  tag = oyProfile_GetTagByPos_( s, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}

/** Function  oyProfile_GetTagById
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag by its tag signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     id                  icTagSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                 oyProfile_GetTagById( oyProfile_s       * profile,
                                       icTagSignature      id )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  oyProfileTag_s * tag = 0,
                 * tmp = 0;
  int i = 0, n = 0;
  icTagSignature tag_id_ = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    n = oyProfile_GetTagCount_( s );

  if(error <= 0 && n)
  {
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );
    for(i = 0; i < n; ++i)
    {
      tmp = oyProfile_GetTagByPos_( s, i );
      tag_id_ = 0;

      if(tmp)
        tag_id_ = oyProfileTagPriv_m(tmp)->use;

      if(tag_id_ == id)
      {
        tag = tmp; tmp = 0;
        break;
      } else
        oyProfileTag_Release( &tmp );
    }
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return tag;
}

/** Function  oyProfile_GetTagCount
 *  @memberof oyProfile_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
OYAPI int OYEXPORT
                   oyProfile_GetTagCount( oyProfile_s    * profile )
{
  int n = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
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
    oyProfileTag_s * tag = 0;
    if(s)
      oyObject_Lock( s->oy_, __FILE__, __LINE__ );

    tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );

    if(s)
      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return n;
}

/** Function  oyProfile_TagMoveIn
 *  @memberof oyProfile_s
 *  @brief    Add a tag to a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
OYAPI int OYEXPORT
                   oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s, i,n;
  oyProfileTag_s * tag = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );


  if(error <= 0)
  {
    /** Initialise tag list. */
    n = oyProfile_GetTagCount_( s );

    /** Avoid double occurencies of tags. */
    for( i = 0; i < n; ++i )
    {
      tag = oyProfile_GetTagByPos_( s, i );
      if(oyProfileTagPriv_m(tag)->use == oyProfileTagPriv_m((*obj))->use)
      {
        oyProfile_TagReleaseAt_(s, i);
        n = oyProfile_GetTagCount_( s );
      }
      oyProfileTag_Release( &tag );
    }
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyProfile_TagReleaseAt
 *  @memberof oyProfile_s
 *  @brief    Remove a tag from a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/03/11
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
                   oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(s && s->type_ == oyOBJECT_PROFILE_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0)
  {
    error = oyStructList_ReleaseAt ( s->tags_, pos );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyProfile_AddTagText
 *  @memberof oyProfile_s
 *  @brief    Add a text tag
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/18
 *  @since   2009/10/18 (Oyranos: 0.1.10)
 */
OYAPI int OYEXPORT
                   oyProfile_AddTagText ( oyProfile_s       * profile,
                                          icSignature         signature,
                                          const char        * text )
{
  oyStructList_s * list = 0;
  int error = 0;
  oyProfileTag_s * tag = 0;
  icTagTypeSignature tt = icSigTextType;
  icSignature vs = oyValueUInt32( oyProfile_GetSignature( profile,
                                                         oySIGNATURE_VERSION) );
  char * v = (char*)&vs;
  int version_A = (int)v[0]/*,
      version_B = (int)v[1]/16,
      version_C =  (int)v[1]%16*/;

  if(version_A <= 3 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigScreeningDescTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigTextDescriptionType;
  else if(version_A >= 4 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigCopyrightTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigMultiLocalizedUnicodeType;

  list = oyStructList_Create( oyOBJECT_NONE, "oyProfile_AddTagText", 0);
  error = oyStructList_AddName( list, text, -1, oyNAME_NAME );

  if(!error)
  {
    tag = oyProfileTag_Create( list, signature, tt, 0, 0);
    error = !tag;
  }

  oyStructList_Release( &list );

  if(tag)
    error = oyProfile_TagMoveIn ( profile, &tag, -1 );

  return error;
}

/** Function  oyProfile_GetFileName
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or as a
 *  from Oyranos defaults the associated filename will simply be retuned.
 *
 *  @param         profile             the profile
 *  @param         dl_pos              -1, or the position in a device links
 *                                     source chain
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
OYAPI const char * OYEXPORT oyProfile_GetFileName (
                                           oyProfile_s       * profile,
                                           int                 dl_pos )
{
  const char * name = 0;
  oyProfile_s * tmp = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  oyProfileTag_s * psid = 0;
  char ** texts = 0;
  int32_t   texts_n = 0;
  char *  hash = 0,
       *  txt = 0;
  char    tmp_hash[34];
  int       dl_n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
    if(dl_pos >= 0)
    {
      psid = oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileSequenceIdentifierTag );
      texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);
      oyProfileTag_Release( &psid );

      if(texts && texts_n > 1+dl_pos*5+2)
      {
        dl_n = 1 + dl_pos*5+2;
        hash = texts[dl_n];
      }
    }


    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else
    {
      names = oyProfileListGet_ ( NULL, 0, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(tmp)
          {
            if(hash)
            {
              uint32_t * h = (uint32_t*)tmp->oy_->hash_ptr_;
              if(h)
                oySprintf_(tmp_hash, "%08x%08x%08x%08x", h[0], h[1], h[2], h[3]);
              else
                oySprintf_(tmp_hash, "                " );
              if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
              {
                name = names[i];
                oyProfile_Release( &tmp );
                break;
              }
            } else
            if(oyProfile_Equal( (oyProfile_s*)s, tmp ))
            {
              name = names[i];
              oyProfile_Release( &tmp );
              break;
            }

            oyProfile_Release( &tmp );
          }
        }
      }

      if(!name && hash)
      {
        uint32_t md5[4];
        sscanf(hash, "%08x%08x%08x%08x", &md5[0],&md5[1],&md5[2],&md5[3] );
        tmp = oyProfile_FromMD5( md5, 0, NULL );
        name = oyStringCopy_( oyProfile_GetFileName(tmp, -1), s->oy_->allocateFunc_ );
        oyProfile_Release( &tmp );
      }

      if(hash)
      {
        char * key = oyAllocateFunc_(80);
        txt = oyFindProfile_( name, 0 );
        sprintf( key, "//"OY_TYPE_STD"/profile.icc/psid_%d", dl_pos );
        oyOptions_SetFromString( &s->oy_->handles_,
                               key,
                               txt,
                               OY_CREATE_NEW );
        if(txt) oyDeAllocateFunc_( txt );
        name = oyOptions_FindString( s->oy_->handles_,
                                     key, 0 );
        oyFree_m_( key );
      } else
      {
        s->file_name_ = oyFindProfile_( name, 0 );
        name = oyStringCopy_( s->file_name_, s->oy_->allocateFunc_ );
        if(s->file_name_)
          oyDeAllocateFunc_( s->file_name_ );
        s->file_name_ = (char*)name;
      }

      if(names)
        oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }

    if(texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return name;
}

/** Function  oyProfile_GetDevice
 *  @memberof oyProfile_s
 *  @brief    Obtain device information from a profile
 *
 *  @code
    oyConfig_s * device = oyConfig_FromRegistration( "//" OY_TYPE_STD "/config", object );
    oyProfile_GetDevice( profile, device ); @endcode
 *
 *  @param[in]     profile             the profile
 *  @param[in,out] device              the device description
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2010/10/26
 */
OYAPI int OYEXPORT
                   oyProfile_GetDevice ( oyProfile_s     * profile,
                                         oyConfig_s      * device )
{
  int error = !profile, l_error = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  oyConfig_s_ * device_ = (oyConfig_s_*)device;
  oyProfileTag_s * tag = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  int i,
      dmnd_found = 0, dmdd_found = 0, serial_found = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigMetaDataTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts_n > 0)
    {
      for(i = 2; i+1 < texts_n && error <= 0; i+=2)
      {
        if(!texts[i+0])
          continue;

        if(strcmp(texts[i+0],"model") == 0) dmdd_found = 1;
        if(strcmp(texts[i+0],"manufacturer") == 0) dmnd_found = 1;
        if(strcmp(texts[i+0],"serial") == 0) serial_found = 1;

        error = oyOptions_SetRegistrationTextKey_( &device_->backend_core,
                                                   device_->registration,
                                                   texts[i+0], texts[i+1] );
      }

      if(!serial_found)
      {
        /* search for a key ending on _serial to strip namespaces */
        for(i = 2; i+1 < texts_n && error <= 0; i+=2)
        {
          int key_len, s_len;

          if(!texts[i+0])
            continue;

          key_len = strlen(texts[i+0]);
          s_len = strlen("serial");

          if(key_len > s_len &&
             strcmp(&texts[i+0][key_len-s_len-1],"_serial") == 0)
          {
            error = oyOptions_SetRegistrationTextKey_( &device_->backend_core,
                                                 device_->registration,
                                                 "serial", texts[i+1] );
            DBG_NUM1_S("added serial: %s", texts[i+1]);
            break;
          }
        }
      }
    }
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    oyProfileTag_Release( &tag );
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigDeviceModelDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmdd_found)
      error = oyOptions_SetRegistrationTextKey_( &device_->backend_core,
                                                 device_->registration,
                                                 "model", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    oyProfileTag_Release( &tag );
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigDeviceMfgDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmnd_found)
      error = oyOptions_SetRegistrationTextKey_( &device_->backend_core,
                                                 device_->registration,
                                                 "manufacturer", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
    oyProfileTag_Release( &tag );
  }

  l_error = oyOptions_SetSource( device_->backend_core,
                                 oyOPTIONSOURCE_FILTER); OY_ERR

  return error;
}

/** Function  oyProfile_GetMD5
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile md5 hash sum
 *
 *  The ICC profiles ID is returned. On request it can be recomputed through
 *  the OY_COMPUTE flag. That computed ID will be used internally as a hash
 *  value. The original profile ID can always be obtained through the
 *  OY_FROM_PROFILE flags until writing of the profile.
 *
 *  @param[in,out] profile             the profile
 *  @param[in]     flags
 *                                     - OY_COMPUTE - calculate the hash
 *                                     - OY_FROM_PROFILE - original profile ID
 *  @param[out]    md5                 the ICC md5 based profile ID
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 *  @date    2011/04/10
 */
int                oyProfile_GetMD5  ( oyProfile_s       * profile,
                                       int                 flags,
                                       uint32_t          * md5 )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->tags_modified_)
    flags |= OY_COMPUTE;

  if(!oyProfile_Hashed_(s) ||
     flags & OY_COMPUTE ||
     s->tags_modified_)
    error = oyProfile_GetHash_( s, flags );

  if(oyProfile_Hashed_(s))
  {
    if(!(flags & OY_FROM_PROFILE))
      memcpy( md5, s->oy_->hash_ptr_, OY_HASH_SIZE );
    else
    if(s->block_ && s->size_ >= 132)
    {
      int i;
      char * data = s->block_;
      memcpy( md5, &data[84], 16 );
      for(i = 0; i < 4; ++i)
        md5[i] = oyValueUInt32( md5[i] );
    } else
      error = -3;
  }
  else if(error > 0)
    error += 1;
  else
    error = 1;

  return error;
}

/**
 *  Function  oyProfile_AddDevice
 *  @memberof oyProfile_s
 *  @brief    Add device and driver informations to a profile
 *
 *  oyProfile_AddDevice() is for storing device/driver informations in a 
 *  ICC profile. So the profile can be sent over internet and Oyranos, or 
 *  an other CMS, can better match to a device/driver on the new host.
 *  The convention what to place into the ICC profile is dependent on each
 *  device class and its actual driver or driver type.
 *  The meta data is stored in the ICC 'meta' tag of type 'dict'.
 *
 *  @param[in,out] profile             the profile
 *  @param[in]     device              device and driver informations
 *  @param[in]     options             - "key_prefix_required" : prefix
 *                                       accept only key names with the prefix
 *                                       Separation by point '.' is allowed.
 *                                     - "set_device_attributes"="true"
 *                                       will write "manufacturer", "model",
 *                                       "mnft" and "model_id" keys to the
 *                                       appropriate profile tags and fields.
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2011/08/21
 */
#if 0
TODO find a general form. Do we want to support the mluc type or is that better
up to a specialised GUI?
int                oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device )
{
  int error = !profile;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  oyProfileTag_s_ * pddt = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
      pddt = (oyProfileTag_s_*)oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileDetailDescriptionTag_ );

      /* icSigProfileDetailDescriptionTag_ */
      if(error <= 0 && !pddt)
      {
        oyStructList_s * list = 0;

        list = oyStructList_Create( oyOBJECT_NONE, "oyProfile_AddDevice", 0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &device, 0,
                                     OY_OBSERVE_AS_WELL );

        if(error <= 0)
        {
          pddt = (oyProfileTag_s_*)oyProfileTag_Create( list, icSigProfileDetailDescriptionTag_,
                                                        icSigProfileDetailDescriptionTag_,
                                                        OY_MODULE_NICK, 0);
          error = !pddt;
        }

        oyStructList_Release( &list );

        if(pddt)
        {
          error = oyProfile_TagMoveIn_( s, &pddt, -1 );
          ++s->tags_modified_;
        }
      }
  }

  return error;
}
#else
int                oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = 0;
  int i,j, len, size, block_size, pos;

  char ** keys,
       ** values,
        * key,
        * val;
  const char * r;
  void * string = 0;
  const char * key_prefix_required = oyOptions_FindString( options,
                                            "key_prefix_required", 0 );
  const char * key_prefix = oyConfig_FindString( device, "prefix", 0 );
  const char * prefix = 0;
  char ** key_prefix_texts = 0;
  int key_prefix_texts_n = 0;
  int * key_prefix_texts_len = 0;
  char * manufacturer=0, * model=0, *mnft=0, *model_id=0;

  /* get just some device */
  oyOption_s * o = 0;
  oyConfig_s * d = device;

  oyProfile_s * p = profile;
  oyProfileTag_s * dict_tag;
  icDictTagType * dict;
  icNameValueRecord * record;

  int n = 0;
  int count = 0;

  if(key_prefix_required)
    prefix = key_prefix_required;
  else if(key_prefix)
    prefix = key_prefix;

  if(prefix)
  {
    key_prefix_texts = oyStringSplit_( prefix,'.',
                                       &key_prefix_texts_n, oyAllocateFunc_);
    oyAllocHelper_m_( key_prefix_texts_len,int,key_prefix_texts_n, 0, return 1);
    for(j = 0; j < key_prefix_texts_n; ++j)
      key_prefix_texts_len[j] = strlen( key_prefix_texts[j] );

    if(!key_prefix)
    {
      o = oyOption_New( NULL );
      oyOption_SetRegistration( o, "////prefix" );
      oyOption_SetFromString( o, prefix, 0 );
      oyOptions_MoveIn( *oyConfig_GetOptions(d,"backend_core"), &o, 0 );
    }
  }

  /* count valid entries */
  n = oyConfig_Count( d );
  for(i = 0; i < n; ++i)
  {
    char * reg = 0;
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    reg = oyFilterRegistrationToText( r, oyFILTER_REG_MAX, oyAllocateFunc_ );
    val = oyOption_GetValueText( o, oyAllocateFunc_ );
    if(val)
    {
      int pass = 1;

      if(prefix)
      {
        int len = strlen( reg );
        if(key_prefix_required)
          pass = 0;
        if(strcmp(reg,"prefix") == 0)
          pass = 1;
        else
        for(j = 0; j < key_prefix_texts_n; ++j)
        {
          if(len >= key_prefix_texts_len[j] &&
             memcmp( key_prefix_texts[j], reg, key_prefix_texts_len[j]) == 0)
            pass = 1;
          if(pass && len > key_prefix_texts_len[j])
          {
            if( strcmp( reg+key_prefix_texts_len[j], "manufacturer") == 0 )
              manufacturer = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "model") == 0 )
              model = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "mnft") == 0 )
              mnft = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "model_id") == 0 )
              model_id = oyStringCopy_( val, oyAllocateFunc_ );
          }
        }
      }

      if(pass)
      {
        DBG_PROG2_S("%s: %s", reg, val );
        ++count;
      }
    }
    if(reg) oyDeAllocateFunc_(reg);
    if(val) oyDeAllocateFunc_(val);
    oyOption_Release( &o );
  }

  /* collect data */
  size = 16 /* or 24 or 32*/
         * n + sizeof(icDictTagType),
  block_size = size;
  pos = 0;

  keys = oyAllocateFunc_( 2 * count * sizeof(char*));
  values = oyAllocateFunc_( 2 * count * sizeof(char*));
  for(i = 0; i < n; ++i)
  {
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    key = oyFilterRegistrationToText( r, oyFILTER_REG_MAX,
                                             oyAllocateFunc_ );
    val = oyOption_GetValueText( o, oyAllocateFunc_ );
    if(val)
    {
      int pass = 1;

      if(key_prefix_required)
      {
        len = strlen( key );
        pass = 0;
        if(strcmp(key,"prefix") == 0)
          pass = 1;
        else
        for(j = 0; j < key_prefix_texts_n; ++j)
        {
          if(len >= key_prefix_texts_len[j] &&
             memcmp( key_prefix_texts[j], key, key_prefix_texts_len[j]) == 0)
            pass = 1;
        }
      }

      /* reject duplicate keys */
      for(j = 0; j < (pos-1); ++j)
        if(strcmp(keys[j],key) == 0)
        {
          if(strcmp(values[j],val) != 0)
            WARNc3_S( "duplicate key[%s] found with different values: \"%s\"/\"%s\"",
                      r, values[j],val );
          pass = 0;
        }

      if(pass)
      {
        keys[pos] = key;
        values[pos] = oyStringCopy_(val,oyAllocateFunc_);
        DBG_PROG3_S("%s: %s (%s)", key, val, r );
        len = strlen( key ) * 2;
        len = len + (len%4 ? 4 - len%4 : 0);
        block_size += len;
        len = strlen( val ) * 2;
        len = len + (len%4 ? 4 - len%4 : 0);
        block_size += len;
        block_size += + 2;
        ++pos;
        key = 0;
      }
    }
    if(key) { oyDeAllocateFunc_( key ); key = 0; }
    if(val) { oyDeAllocateFunc_(val); val = 0; }
    oyOption_Release( &o );
  }

  count = pos;
  dict = calloc(sizeof(char), block_size);
  dict->sig = oyValueUInt32( icSigDictType );
  dict->number = oyValueUInt32( count );
  dict->size = oyValueUInt32( 16 );

  pos = size;
  for(i = 0; i < count; ++i)
  {
    record = (icNameValueRecord*)((char*)dict + sizeof(icDictTagType) + 16 * i);

    len = 0;
    string = NULL;
    error = oyIconvGet( keys[i], &string, &len, "UTF-8", "UTF-16BE",
                        oyAllocateFunc_ );
    record->name_string_offset = oyValueUInt32( pos );
    len = strlen( keys[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->name_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    oyFree_m_( string );
    pos += len;

    len = 0;
    string = NULL;
    error = oyIconvGet( values[i], &string, &len, "UTF-8", "UTF-16BE", 
                        oyAllocateFunc_ );
    record->value_string_offset =  oyValueUInt32( pos );
    len = strlen( values[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->value_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    pos += len;
    oyFree_m_( string );
  }

  dict_tag = oyProfileTag_New(NULL);
  error = oyProfileTag_Set( dict_tag, icSigMetaDataTag, icSigDictType,
                            oyOK, block_size, (oyPointer*)&dict );
  if(error <= 0)
    error = oyProfile_TagMoveIn( p, &dict_tag, -1 );

  if(oyOptions_FindString( options, "set_device_attributes", "true" ))
  {
      uint32_t model_idi = 0;
      const char * t = 0;
      char * data;
      size_t size = 0;
      icHeader * header = 0;

      oyProfileTag_s * tag = oyProfile_GetTagByPos( p, 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      oyPointer tag_block = 0;

      data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
      header = (icHeader*) data;
      t = mnft;
      if(t)
        sprintf( (char*)&header->manufacturer, "%s", t );
      t = model_id;
      if(t)
        model_idi = atoi( t );
      model_idi = oyValueUInt32( model_idi );
      memcpy( &header->model, &model_idi, 4 );

      oyStruct_AllocHelper_m_( tag_block, char, 132, tag, return 0 );
      error = !memcpy( tag_block, data, 132 );
      error = oyProfileTag_Set( tag, (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, &tag_block );
      t = manufacturer;
      if(t)
        error = oyProfile_AddTagText( p, icSigDeviceMfgDescTag, t );
      t =  model;
      if(t)
        error = oyProfile_AddTagText( p, icSigDeviceModelDescTag, t );
      if(data && size)
        oyFree_m_( data );
  }

  oyStringListRelease_( &keys, count, oyDeAllocateFunc_ );
  oyStringListRelease_( &values, count, oyDeAllocateFunc_ );
  if(key_prefix_texts_n)
  {
    oyStringListRelease_( &key_prefix_texts, key_prefix_texts_n,
                          oyDeAllocateFunc_ );
    oyDeAllocateFunc_( key_prefix_texts_len );
  }

  if(manufacturer) oyFree_m_( manufacturer );
  if(model) oyFree_m_( model );
  if(model_id) oyFree_m_( model_id );
  if(mnft) oyFree_m_( mnft );

  return error;
}
#endif


/**
 *  @brief get the ICC*XYZ coordinates of a white point
 *
 *  @param[in]     profile             the ICC profile object if interesst
 *  @param[out]    ICC_XYZ             ICC*XYZ trio in 0.0 - 2.0 range
 *  @return                            0 - success; -1 - no white point available; < 1 - error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/03/14
 *  @since   2017/06/06 (Oyranos: 0.9.7)
 */
int      oyProfile_GetWhitePoint     ( oyProfile_s       * profile,
                                       double            * ICC_XYZ )
{
  int error = -1;
  oyMAT3 CHAD = { { { {1,0,0} },
                    { {0,1,0} },
                    { {0,0,1} } } },
         CHAD_INVERSE;
  if(profile)
  {
    oyProfileTag_s * wtpt = NULL, * chad = NULL;
    oyStructList_s * s = NULL;
    int count, i,j;
    wtpt = oyProfile_GetTagById( profile, icSigMediaWhitePointTag ); 

    s = oyProfileTag_Get( wtpt );
    count = oyStructList_Count( s );
    for(j = 0; j < count; ++j)
    {
      oyOption_s * opt = (oyOption_s*) oyStructList_GetType( s, j,
                                                            oyOBJECT_OPTION_S );
      if(opt && strstr( oyOption_GetRegistration( opt ), "icSigXYZType" ) != NULL)
      {
        double XYZ[3] = { oyOption_GetValueDouble( opt, 0 ),
                          oyOption_GetValueDouble( opt, 1 ),
                          oyOption_GetValueDouble( opt, 2 ) };
        for(i = 0; i < 3; ++i)
          ICC_XYZ[i] = XYZ[i];
        error = 0;
      }
    }

    oyProfileTag_Release( &wtpt );
    oyStructList_Release( &s );


    /* apply inverse chromatic adaption,
     * it is used to adapt ICC v4 default D50 white point to absolute XYZ */
    chad = oyProfile_GetTagById( profile, icSigChromaticAdaptionMatrix ); 

    s = oyProfileTag_Get( chad );
    count = oyStructList_Count( s );
    for(j = 0; j < count; ++j)
    {
      oyOption_s * opt = (oyOption_s*) oyStructList_GetType( s, j,
                                                             oyOBJECT_OPTION_S );
      const char * reg = oyOption_GetRegistration( opt );
      if(opt && strstr( reg, "array" ) != NULL)
      {
        int k;
        int count = oyOption_GetValueDouble(opt, -1) + 0.5;
        if(count != 9)
        {
          WARNc1_S("\"chad\" expected to have 9 values but contained: %s", count);
        } else
          for(k = 0; k < 3; ++k)
            for(i = 0; i < 3; ++i)
              CHAD.v[k].n[i] = oyOption_GetValueDouble( opt, k*3+i );
      }
    }
    if(oy_debug)
      oyMessageFunc_p( oyMSG_DBG, NULL, OY_DBG_FORMAT_ "found chad:\n%s",
                       OY_DBG_ARGS_, oyMAT3show(&CHAD) );
    if(chad)
    {
      oyVEC3 D50, r;
      for(i = 0; i < 3; ++i)
        D50.n[i] = ICC_XYZ[i];
      oyMAT3inverse( &CHAD, &CHAD_INVERSE );
      oyMAT3eval( &r, &CHAD_INVERSE, &D50 );
      for(i = 0; i < 3; ++i)
        ICC_XYZ[i] = r.n[i];
      if(oy_debug)
      {
        double cie_a = 0.0, cie_b = 0.0, Lab[3];
        oyXYZ2Lab(ICC_XYZ,Lab); cie_a = Lab[1]/256.0+0.5; cie_b = Lab[2]/256.0+0.5;
        double temperature = oyEstimateTemperature( cie_a, cie_b, NULL );
        oyMessageFunc_p( oyMSG_DBG, NULL,
                       OY_DBG_FORMAT_ "ICC absolute wtpt: %f %f %f  ~ %d Kelvin", OY_DBG_ARGS_,
                       ICC_XYZ[0], ICC_XYZ[1], ICC_XYZ[2], cie_a, cie_b, (int)temperature );
      }
    }

    oyProfileTag_Release( &chad );
    oyStructList_Release( &s );
  }

  return error;
}

/**
 *  @brief Find string match in meta tag
 *
 *  @param[in]     profile             the object
 *  @param[out]    key                 the key inside the meta tag
 *  @param[out]    value               the expected value or NULL for any value
 *  @return                            matching string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/22
 *  @since   2018/02/22 (Oyranos: 0.9.7)
 */
const char * oyProfile_FindMeta      ( oyProfile_s       * profile,
                                       const char        * key,
                                       const char        * value )
{
  const char * text = NULL;
  oyProfileTag_s * tag = NULL;
  if(profile)
    tag = oyProfile_GetTagById( profile, icSigMetaDataTag );

  if( tag )
  {
    int j;
    int32_t texts_n = 0, tag_size = 0;
    char ** texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                          &tag_size, oyAllocateFunc_ );
    for(j = 0; j < texts_n; ++j)
    {
      if( strcmp(texts[j],key) == 0 &&
          texts_n > (j+1) )
      {
        if(!value ||
            (value && oyTextIccDictMatch(texts[j+1], value, 0, '/', ',' )))
        {
          oyObject_SetName( profile->oy_, texts[j+1], oyNAME_PATTERN );
          text = oyObject_GetName( profile->oy_, oyNAME_PATTERN );
          break;
        }
      }
    }
    oyStringListRelease( &texts, texts_n, oyDeAllocateFunc_ );

    oyProfileTag_Release( &tag );
  }

  return text;
}


/* } Include "Profile.public_methods_definitions.c" */

