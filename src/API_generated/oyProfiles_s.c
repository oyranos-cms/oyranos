/** @file oyProfiles_s.c

   [Template file inheritance graph]
   +-> oyProfiles_s.template.c
   |
   +-> BaseList_s.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2020 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyProfiles_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_cache.h"
#include "oyranos_io.h"
#include "oyProfile_s_.h"
#include "oyProfiles_s_.h"
#include "oyConfig_s_.h"
  



/** Function oyProfiles_New
 *  @memberof oyProfiles_s
 *  @brief   allocate a new Profiles object
 */
OYAPI oyProfiles_s * OYEXPORT
  oyProfiles_New( oyObject_s object )
{
  oyObject_s s = object;
  oyProfiles_s_ * profiles = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  profiles = oyProfiles_New_( s );

  return (oyProfiles_s*) profiles;
}

/** Function  oyProfiles_Copy
 *  @memberof oyProfiles_s
 *  @brief    Copy or Reference a Profiles object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     profiles                 Profiles struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyProfiles_s* OYEXPORT
  oyProfiles_Copy( oyProfiles_s *profiles, oyObject_s object )
{
  oyProfiles_s_ * s = (oyProfiles_s_*) profiles;

  if(s)
  {
    oyCheckType__m( oyOBJECT_PROFILES_S, return NULL )
  }
  else
    return NULL;

  s = oyProfiles_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyProfiles_s" );

  return (oyProfiles_s*) s;
}
 
/** Function oyProfiles_Release
 *  @memberof oyProfiles_s
 *  @brief   release and possibly deallocate a oyProfiles_s object
 *
 *  @param[in,out] profiles                 Profiles struct object
 */
OYAPI int OYEXPORT
  oyProfiles_Release( oyProfiles_s **profiles )
{
  oyProfiles_s_ * s = 0;

  if(!profiles || !*profiles)
    return 0;

  s = (oyProfiles_s_*) *profiles;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

  *profiles = 0;

  return oyProfiles_Release_( &s );
}

/** Function oyProfiles_MoveIn
 *  @memberof oyProfiles_s
 *  @brief   add an element to a Profiles list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyProfiles_MoveIn          ( oyProfiles_s       * list,
                                       oyProfile_s       ** obj,
                                       int                 pos )
{
  oyProfiles_s_ * s = (oyProfiles_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_PROFILE_S)
  {
    if(!s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
    error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   

  return error;
}

/** Function oyProfiles_ReleaseAt
 *  @memberof oyProfiles_s
 *  @brief   release a element from a Profiles list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyProfiles_ReleaseAt       ( oyProfiles_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_PROFILES_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyProfiles_Get
 *  @memberof oyProfiles_s
 *  @brief   get a element of a Profiles list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyProfile_s * OYEXPORT
           oyProfiles_Get             ( oyProfiles_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(!error)
    return (oyProfile_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_PROFILE_S);
  else
    return 0;
}   

/** Function oyProfiles_Count
 *  @memberof oyProfiles_s
 *  @brief   count the elements in a Profiles list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyProfiles_Count           ( oyProfiles_s       * list )
{       
  int error = !list;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}

/** Function oyProfiles_Clear
 *  @memberof oyProfiles_s
 *  @brief   clear the elements in a Profiles list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyProfiles_Clear           ( oyProfiles_s       * list )
{       
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyProfiles_Sort
 *  @memberof oyProfiles_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyProfiles_Sort            ( oyProfiles_s       * list,
                                       int32_t           * rank_list )
{       
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "Profiles.public_methods_definitions.c" { */
int oyLowerStrcmpWrap_ (const void * a_, const void * b_)
{
  const char * a = *(const char **)a_,
             * b = *(const char **)b_;
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  return strcmp(a,b);
#endif
}

/** Function oyProfiles_Create
 *  @memberof oyProfiles_s
 *  @brief   get a list of installed profiles
 *
 *  @param[in]     patterns            a list properties, e.g. classes;
 *                                     Only matching profiles are selected.
 *                                     If NULL, all profiles are accepted.
 *  @param         flags               
 *                                     - see oyProfile_FromFile()
 *                                     - ::OY_ALLOW_DUPLICATES - do not filter
 *                                     duplicate profile ID's
 *  @param         object              the optional object
 *  @return                            the found and selected profiles
 *
 *  @code
    // Put all ICC Display Class profiles in "profiles"
    icSignature profile_class = icSigDisplayClass;
    oyProfile_s * pattern = 0;
    oyProfiles_s * patterns = oyProfiles_New( 0 ),
                 * profiles = 0;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );

    // only display profiles
    pattern = oyProfile_FromSignature( profile_class, oySIGNATURE_CLASS, 0 );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    // ... and only profiles installed in system paths
    char * text = oyGetInstallPath( oyPATH_ICC, oySCOPE_SYSTEM, oyAllocateFunc_ );
    pattern = oyProfile_FromFile( text, OY_NO_LOAD, NULL );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    profiles = oyProfiles_Create( patterns, icc_profile_flags, 0 );
    oyProfiles_Release( &patterns );@endcode
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/08/31
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       uint32_t            flags,
                                       oyObject_s          object)
{
  oyProfiles_s * s = oyProfiles_New( object ),
               * tmps = oyProfiles_New( object );
  const char * name = "profile cache from oyProfiles_Create()";
  int error = 0;

  oyProfile_s * tmp = 0, * pattern = 0;
  char  ** names = 0;
#if !defined(HAVE_POSIX)
  char *t;
#endif
  uint32_t names_n = 0, i = 0, j = 0, n = 0,
           patterns_n = oyProfiles_Count(patterns);
  int sorts = 0;
  const char ** sort = NULL;

  error = !s;

  if(error <= 0)
  {
    names = oyProfileListGet_ ( NULL, flags, &names_n );

    if(!(flags & OY_ALLOW_DUPLICATES))
      flags |= OY_COMPUTE;

    if(oyProfiles_Count( oy_profile_list_cache_ ) != (int)names_n)
    {
      oyProfiles_s * l = oyProfiles_New(0);
      sorts = names_n;
      sort = oyAllocateFunc_(sorts*sizeof(const char*)*2);
      for(i = 0; i < names_n; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          {
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE | flags, 0 );
#if !defined(HAVE_POSIX)
            t = 0;
            oyStringAdd_(&t, oyProfile_GetText(tmp, oyNAME_DESCRIPTION), oyAllocateFunc_, 0);
            n = strlen(t);
            /* the following upper caseing is portable,
             * still strcasecmp() might be faster? */
            for(j = 0; j < n; ++j)
              if(isalpha(t[j]))
                t[j] = tolower(t[j]);
            sort[i*2] = t;
#else
            sort[i*2] = oyProfile_GetText(tmp, oyNAME_DESCRIPTION);
#endif
            sort[i*2+1] = names[i];
            oyProfiles_MoveIn( l, &tmp, -1 );
          }
        }
      }
      qsort( sort, sorts, sizeof(char**)*2, oyLowerStrcmpWrap_ );
      for(i = 0; (int)i < sorts; ++i)
      {
        tmp = oyProfile_FromFile( sort[i*2+1], OY_NO_CACHE_WRITE | flags, 0 );
        oyProfiles_MoveIn(tmps, &tmp, -1);
#if !defined(HAVE_POSIX)
        t = (char*)sort[i*2];
        oyFree_m_(t);
#endif
      }
      oyProfiles_Release(&l);
      oyProfiles_Release(&oy_profile_list_cache_);
      oyObject_SetNames( tmps->oy_, name,name,name );
      oy_profile_list_cache_ = tmps;
      oyFree_m_(sort);
    }
    else
      oyProfiles_Release( &tmps );

    n = oyProfiles_Count( oy_profile_list_cache_ );
    if(oyProfiles_Count( oy_profile_list_cache_ ) != (int)names_n)
      WARNc2_S("updated oy_profile_list_cache_ differs: %d %d",n, names_n);
    oyStringListRelease_( &names, names_n, oyDeAllocateFunc_ ); names_n = 0;

    for(i = 0; i < n; ++i)
    {
        int good = 1;

        tmp = oyProfiles_Get( oy_profile_list_cache_, i );

        if(patterns_n > 0)
        {
          good = 0;
          for(j = 0; j < patterns_n; ++j)
          {
            if(tmp)
              pattern = oyProfiles_Get(patterns, j);

            if(oyProfile_Match_( (oyProfile_s_*)pattern, (oyProfile_s_*)tmp ))
              good = 1;

            oyProfile_Release( &pattern );

            if(good) break;
          }
        }

        if(good &&
           !(flags & OY_ALLOW_DUPLICATES))
        {
          int count = oyProfiles_Count( s ),
              j;
          for(j = 0; j < count; ++j)
          {
            oyProfile_s * b = oyProfiles_Get( s, j );
            if(oyProfile_Equal(b, tmp))
              good = 0;
            oyProfile_Release( &b );
          }
        }

        if(good)
          oyProfiles_MoveIn( s, (oyProfile_s**)&tmp, -1);

        oyProfile_Release( &tmp );
    }
  }

  return s;
}

/** Function  oyProfiles_ForStd
 *  @memberof oyProfiles_s
 *  @brief    Get a list of installed profiles
 *
 *  Allow for a special case with oyDEFAULT_PROFILE_START in the color_space
 *  argument, to select all possible standard color profiles, e.g. for 
 *  typical color conversions.
 *
 *  oyASSUMED_WEB will result in exactly one profile added as long as it is
 *  available in the file paths.
 *
 *  @param[in]     std_profile_class   standard profile class,
 *                                     see ::oyPROFILE_e ;
 *                                     e.g. oyEDITING_RGB
 *  @param         flags               see oyProfile_FromFile()
 *  @param[out]    current             get the color_space profile position
 *  @param         object              a optional object
 *  @return                            the profile list
 *
 *  @par Example - get all standard RGB profiles:
 *  @code
    // Get all ICC profiles, which can be used as assumed RGB profile
    oyPROFILE_e type = oyEDITING_RGB;
    int current = 0,
        size, i;
    oyProfile_s * temp_prof = 0;
    oyProfiles_s * iccs = 0;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
 
    iccs = oyProfiles_ForStd( type, icc_profile_flags, &current, 0 );

    size = oyProfiles_Count(iccs);
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      // Show the profile internal and file names on the command line
      printf("%s %d: \"%s\" %s\n", i == current ? "*":" ", i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, -1));
      oyProfile_Release( &temp_prof );
    } @endcode
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/02/25
 *  @since   2008/07/25 (Oyranos: 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT oyProfiles_ForStd 
                                     ( oyPROFILE_e         std_profile_class,
                                       uint32_t            flags,
                                       int               * current,
                                       oyObject_s          object)
{
  oyPROFILE_e type = std_profile_class;
    char * default_p = 0;
    int i, val = -1;

    char  * temp = 0,
          * text = 0;
    uint32_t size = 0;
    oyProfiles_s * iccs = NULL, * patterns = NULL;
    oyProfile_s * profile = NULL, * temp_prof = NULL;
    icSignature csp;

    if(type == oyASSUMED_WEB)
    {
      profile = oyProfile_FromStd( type, flags, object );
      iccs = oyProfiles_New( object );
      if(current)
      {
        if(profile)
          *current          = 0;
        else
          *current          = -1;
      }
      oyProfiles_MoveIn( iccs, &profile, 0 );
      return iccs;
    }

    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ ||
       type == oyEDITING_LAB ||
       type == oyASSUMED_LAB ||
       type == oyEDITING_RGB ||
       type == oyASSUMED_RGB ||
       type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF ||
       type == oyPROFILE_EFFECT ||
       type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
      default_p = oyGetDefaultProfileName( (oyPROFILE_e)type, oyAllocateFunc_);

    patterns = oyProfiles_New( object );

    /* prepare the patterns according to the profile type */
    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ)
    {
      csp = icSigXYZData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_LAB ||
       type == oyASSUMED_LAB)
    {
      csp = icSigLabData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_RGB ||
       type == oyASSUMED_RGB)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    /* support typical output Rgb device for cinema and print proofing */
    if(type == oyPROFILE_PROOF)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF)
    {
      csp = icSigCmykData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    /* support abtract profiles */
    if(type == oyPROFILE_EFFECT)
    {
      csp = icSigLabData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigAbstractClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      csp = icSigXYZData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigAbstractClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
    {
      csp = icSigGrayData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyDEFAULT_PROFILE_START)
    {
      profile = oyProfile_FromSignature( icSigColorSpaceClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigInputClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigOutputClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigDisplayClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }

    /* get the profile list */
    iccs = oyProfiles_Create( patterns, flags, 0 );

    /* detect the default profile position in our list */
    size = oyProfiles_Count(iccs);
    if(default_p)
    for( i = 0; i < (int)size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      text = oyStringCopy_( oyProfile_GetFileName(temp_prof, -1),
                            oyAllocateFunc_ );
      temp = oyStrrchr_( text, '/' );
      if(temp)
        ++temp;
      else
        temp = text;

      if(oyStrstr_( temp, default_p) &&
         oyStrlen_( temp ) == oyStrlen_(default_p))
      {
        val = i;
        i = (int)size; /* break */
      }

      oyProfile_Release( &temp_prof );
      oyDeAllocateFunc_( text );
    }

    if(current)
      *current          = val;

    if(default_p)
      oyFree_m_( default_p );
    oyProfiles_Release( &patterns );

  return iccs;
}

/** Function  oyProfiles_DeviceRank
 *  @memberof oyProfiles_s
 *  @brief    Sort a profile list according to a given device
 *
 *  Profiles which match the device will placed according to a rank value on 
 *  top of the list followed by the zero ranked profiles.
 *
 *  @code
    // Get all ICC profiles, which can be used as assumed RGB profile
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB,
                                               icc_profile_flags, 0,0 );
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(p_list) *
                                             sizeof(int32_t) );
    // Sort the profiles according to eaches match to a given device
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );

    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( p_list, i );
      // Show the rank value, the profile internal and file names on the command line
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    } @endcode
 *
 *  @param[in,out] list                the to be sorted profile list
 *  @param[in]     device              filter pattern
 *  @param[in,out] rank_list           list of rank levels for the profile list
 *
 *  @version Oyranos: 0..1
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2012/02/01
 */
int              oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                         oyConfig_s      * device,
                                         int32_t         * rank_list )
{
  return oyProfiles_Rank( list, device, OY_SLASH_C, '.', 0, rank_list );
}

/** Function  oyProfiles_Rank
 *  @memberof oyProfiles_s
 *  @brief    Sort a profile list according to a given pattern
 *
 *  Profiles which match the patern will placed according to a rank value on 
 *  top of the list followed by the zero ranked profiles.
 *
 *  Get all ICC profiles, which can be used as assumed RGB profile:
 *  @dontinclude test2.cpp
    @skip Get all ICC profiles, which can be used as assumed RGB profile
    @until Done print
 *
 *  @param[in,out] list                the to be sorted profile list
 *  @param[in]     device              filter pattern with rank_map
 *  @param         path_separator      a char to split into hierarchical levels
 *  @param         key_separator       a char to split key strings
 *  @param         flags               options:
 *                                     - OY_MATCH_SUB_STRING - find sub string;
 *                                       default is whole word match
 *  @param[in,out] rank_list           list of rank levels for the profile list
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/01/05
 *  @since   2017/01/05 (Oyranos: 0.9.7)
 */
OYAPI int OYEXPORT oyProfiles_Rank   ( oyProfiles_s      * list,
                                       oyConfig_s        * device,
                                       char                path_separator,
                                       char                key_separator,
                                       int                 flags,
                                       int32_t           * rank_list )
{
  int error = !list || !device || !rank_list;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;
  int i,n;
  int32_t rank;
  oyProfile_s * p = 0;
  oyConfig_s * p_device = 0;
  oyConfig_s_ * d = (oyConfig_s_*)device;
  oyOptions_s * old_db = 0;

  if(!list)
    return 0;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  p_device = oyConfig_FromRegistration( d->registration, 0 );
  n = oyProfiles_Count( list );

  error = !memset( rank_list, 0, sizeof(int32_t) * n );

  /* oyConfig_Match assumes its options in device->db, so it is filled here.*/
  if(!oyOptions_Count( d->db ))
  {
    old_db = d->db;
    d->db = d->backend_core;
  }

  for(i = 0; i < n; ++i)
  {
    p = oyProfiles_Get( list, i );

    oyProfile_GetDevice( p, p_device );
    rank = 0;

    error = oyConfig_Match( p_device, device, path_separator, key_separator, flags, &rank );
    if(oyConfig_FindString( p_device, "OYRANOS_automatic_generated", "1" ) ||
       oyConfig_FindString( p_device, "OPENICC_automatic_generated", "1" ))
    {
      DBG_NUM2_S( "found OPENICC_automatic_generated: %d %s",
                  rank, strrchr(oyProfile_GetFileName(p,-1),'/')+1);
      /* substract serial number and accound for possible wrong model_id */
      if(oyConfig_FindString( p_device, "serial", 0 ))
        rank -= 13;
      else
        rank -= 2;
      DBG_NUM1_S("after serial && OPENICC_automatic_generated: %d", rank);
    }
    rank_list[i] = rank;

    oyOptions_Clear( oyConfigPriv_m(p_device)->backend_core );
    oyProfile_Release( &p );
  }

  if(!error)
    error = oyStructList_Sort( s->list_, rank_list );

  if(old_db)
    d->db = old_db;

  oyConfig_Release( &p_device );

  return error;
}


/* } Include "Profiles.public_methods_definitions.c" */

