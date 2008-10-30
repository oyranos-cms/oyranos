/** @file oyranos_elektra.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    elektra dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#include <kdb.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_io.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"

#ifndef KDB_VERSION_MAJOR
#define KDB_VERSION_MAJOR 0
#endif
#ifndef KDB_VERSION_MINOR
#define KDB_VERSION_MINOR 0
#endif
#define KDB_VERSION_NUM (KDB_VERSION_MAJOR*10000 + KDB_VERSION_MINOR*100)

#if KDB_VERSION_NUM >= 700
#define kdbGetString_m kdbGetString
#define kdbGetChildKeys(a,b,c,d) kdbGetByName(a,c,b,d)
#define ksAppendKeys ksAppend
#define KDBHandle KDB
#else
#define kdbGetString_m kdbGetValue
#endif

char oy_elektra_error_text[24];

char * oy__kdbStrError(int rc) { sprintf(oy_elektra_error_text, "elektra: %d", rc);
                                 return oy_elektra_error_text; }

#if KDB_VERSION_NUM > 600
#define kdbStrError(t) oy__kdbStrError(t)
#define errno rc
#endif

/* --- Helpers  --- */
#if 1
/*# define ERR if ((rc != KDB_ERR_OK && oy_debug) || rc > KDB_ERR_OK) \
              { printf("%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }*/
# define ERR  if(rc) \
                        oyMessageFunc_p( oyMSG_WARN, 0, "%s:%d %d: %s", \
                        __FILE__,__LINE__, \
                        rc < 0 ? errno : rc, kdbStrError(rc < 0 ? errno : rc));
#else
# define ERR
#endif

/* --- static variables   --- */

static int oyranos_init = 0;
int oy_warn_ = 1;
#define OY_WEB_RGB "sRGB.icc"

/* --- structs, typedefs, enums --- */


/* --- internal API definition --- */

/* elektra key list handling */
char*   oySearchEmptyKeyname_  (const char* keyParentName,
                                const char* keyBaseName);
KeySet* oyReturnChildrenList_  (const char* keyParentName,int* rc);



/* separate from the external functions */


#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

static KDBHandle * oy_handle_ = 0;

void oyOpen_ (void)
{
  if(!oyranos_init) {
    oy_handle_ = kdbOpen( /*&oy_handle_*/ );
    if(!oy_handle_)
      WARNc_S("Could not initialise Elektra.");
    oyranos_init = 1;
  }
}
void oyClose_() { /*kdbClose( &oy_handle_ );*/ }
/* @todo make oyOpen unnecessary */
void oyOpen  (void) { oyOpen_(); }
void oyClose (void) { oyClose_(); }


/* oyranos part */


oyComp_t_* oyGetDeviceProfile_sList          (const char* manufacturer,
                                           const char* model,
                                           const char* product_id,
                                           const char* host,
                                           const char* port,
                                           const char* attrib1,
                                           const char* attrib2,
                                           const char* attrib3,
                                           KeySet* profilesList,
                                           int   rc);



/* small helpers */

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current_, list ) \
   ksRewind( list );  \
   for( current_ = ksNext( list ); current_; current_ = ksNext( list )  )



/* --- function definitions --- */

KeySet*
oyReturnChildrenList_ (const char* keyParentName, int* rc)
{
  int user_sys = oyUSER_SYS,
      ret = 0;
  KeySet*list_user = 0;
  KeySet*list_sys = 0;
  KeySet*list = ksNew(0);
  char  *list_name_user = NULL;
  char  *list_name_sys = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(list_name_user, char, MAX_PATH, 0, )
  oyAllocHelper_m_(list_name_sys, char, MAX_PATH, 0, )

  if( user_sys == oyUSER_SYS || user_sys == oyUSER ) {
    list_user = ksNew(0);
    sprintf(           list_name_user, "%s%s", OY_USER, keyParentName);
    if(!oy_handle_)
    {
      *rc = 1;
      return 0;
    }
    ret =
      kdbGetChildKeys( oy_handle_, list_name_user, list_user, KDB_O_SORT);
  }
  if( user_sys == oyUSER_SYS || user_sys == oySYS ) {
    list_sys = ksNew(0);
    sprintf(           list_name_sys, "%s%s", OY_SYS, keyParentName);
    if(!oy_handle_)
    {
      *rc = 1;
      return 0;
    }
    ret =
      kdbGetChildKeys( oy_handle_, list_name_sys, list_sys, KDB_O_SORT);
  }

  if(list_user)
    ksAppendKeys(list, list_user);
  if(list_sys)
    ksAppendKeys(list, list_sys);

  DBG_PROG_V(( (intptr_t)keyParentName ))
  DBG_PROG_S(( keyParentName ))
  DBG_PROG_V(( (intptr_t)ksGetSize(list) ))

  oyFree_m_( list_name_user )
  oyFree_m_( list_name_sys )

  DBG_PROG_ENDE
  return list;
}

char*
oySearchEmptyKeyname_ (const char* keyParentName, const char* keyBaseName)
{
  char* keyName = (char*)     calloc (strlen(keyParentName)
                                    + strlen(keyBaseName) + 24, sizeof(char));
  char* pathkeyName = NULL;
  int nth = 0, i = 1, rc=0;
  Key *key;
  char *name = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(name, char, MAX_PATH, 0, )
  sprintf(name, "%s%s", oySelectUserSys_(), keyParentName);

  oyAllocHelper_m_( pathkeyName, char,
                    oyStrlen_(name) + strlen(keyBaseName) + 24, 0, )

  key = keyNew( KEY_END );
  if(keyParentName)
    DBG_PROG_S((keyParentName));
  if(keyBaseName)
    DBG_PROG_S((keyBaseName));
  if(name)
    DBG_PROG_S((name));

    /* search for empty keyname */
    while (!nth)
    { sprintf (pathkeyName, "%s/%s%d", name, keyBaseName, i);

      if(!oy_handle_)
        return 0;
      keySetName( key, pathkeyName );

      rc=kdbGetKey( oy_handle_, key ); ERR
      if (rc)
        nth = i;
      i++;
    }
    sprintf (keyName, ("%s/%s"), OY_PATHS, pathkeyName);

  if(keyName)
    DBG_PROG_S((keyName));

  oyFree_m_( pathkeyName )
  oyFree_m_( name )

  DBG_PROG_ENDE
  return keyName;
} 

int
oyKeySetHasValue_     (const char* keyParentName, const char* ask_value)
{
  int result = 0;
  int rc=0;
  char* value = (char*) calloc (sizeof(char), MAX_PATH);
  KeySet *myKeySet;
  Key *current;

  DBG_PROG_START

  myKeySet = oyReturnChildrenList_( keyParentName, &rc ); ERR
        if(!myKeySet)
        {
          FOR_EACH_IN_KDBKEYSET( current, myKeySet )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S(( value ))
            if(strstr(value, ask_value) != 0 &&
               strlen(value) == strlen(ask_value))
            {
              DBG_PROG_S((value))
              result = 1;
              break;
            }
          }
        }
  ksDel (myKeySet);
  oyClose_();

  DBG_PROG_ENDE
  return result;
}

int
oyAddKey_valueComment_ (const char* keyName,
                        const char* value,
                        const char* comment)
{
  int rc=0;
  Key *key;
  char *name = NULL;
  char *value_utf8 = NULL;
  char *comment_utf8 = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(name, char, MAX_PATH, 0, )
  oyAllocHelper_m_(value_utf8, char, MAX_PATH, 0, )
  oyAllocHelper_m_(comment_utf8, char, MAX_PATH, 0, )

  sprintf(name, "%s%s", oySelectUserSys_(), keyName);
  if(value && oyStrlen_(value))
    oyIconv( value, strlen(value) < MAX_PATH ? strlen(value) : MAX_PATH,
             value_utf8, 0, "UTF-8" );
  if(comment && oyStrlen_(comment))
    oyIconv( comment, strlen(comment) < MAX_PATH ? strlen(comment) : MAX_PATH,
             comment_utf8, 0, "UTF-8" );

  if (keyName)
    DBG_PROG_S(( keyName ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));
  if (!keyName || !strlen(keyName))
    WARNc_S( "no keyName given" );

  key = keyNew( KEY_END );
  keySetName( key, name );

  if(!oy_handle_)
    return 0;
  rc=kdbGetKey( oy_handle_, key );
  if(rc < 0)
    oyMessageFunc_p( oyMSG_WARN, 0, "%s:%d code:%d %s name:%s",
                     __FILE__,__LINE__, rc, kdbStrError(rc), name);
  if(value)
  {
    rc=keySetString (key, value_utf8);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN, 0, "%s:%d code:%d %s name:%s value:%s",
                       __FILE__,__LINE__, rc, kdbStrError(rc), name, value);
  }
  if(comment)
  {
    rc=keySetComment (key, comment_utf8);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN, 0, "%s:%d code:%d %s name:%s comment:%s",
                       __FILE__,__LINE__, rc, kdbStrError(rc), name, comment);
  }

  oyOpen_();
  rc=kdbSetKey( oy_handle_, key ); ERR
  if(rc < 0)
    oyMessageFunc_p( oyMSG_WARN, 0, "%s:%d code:%d %s name:%s",
                     __FILE__,__LINE__, rc, kdbStrError(rc), name);
  oyClose_();

  oyFree_m_( name )
  oyFree_m_( value_utf8 )
  oyFree_m_( comment_utf8 )

  DBG_PROG_ENDE
  return rc;
}

const char*
oySelectUserSys_()
{
  /* enable system wide keys for user root */
  if(geteuid() == 0)
    return OY_SYS;
  else
    return OY_USER;
}


int
oySetBehaviour_      (oyBEHAVIOUR_e type, int choice)
{
  int r = 1;

  DBG_PROG_START

  DBG_PROG2_S( "type = %d behaviour %d", type, choice )

  if ( (r=oyTestInsideBehaviourOptions_(type, choice)) == 1 )
  {
    const char *keyName = 0;

    keyName = oyOptionGet_((oyWIDGET_e)type)-> config_string;

      if(keyName)
      {
        char val[12];
        const char *com =
            oyOptionGet_((oyWIDGET_e)type)-> choice_list[ choice ];
        snprintf(val, 12, "%d", choice);
        r = oyAddKey_valueComment_ (keyName, val, com);
        DBG_PROG4_S( "%s %d %s %s", keyName, type, val, com?com:"" )
      }
      else
        WARNc1_S( "type %d behaviour not possible", type);
  }

  DBG_PROG_ENDE
  return r;
}

int
oyGetBehaviour_      (oyBEHAVIOUR_e type)
{
  char* name = 0;
  const char* key_name = 0;
  int c = -1;

  DBG_PROG_START

  DBG_PROG1_S( "type = %d behaviour", type )

  if( oyTestInsideBehaviourOptions_(type, 0) )
  {
    key_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    if(key_name)
    {
      name = oyGetKeyString_( key_name, oyAllocateFunc_ );
    }
    else
      WARNc1_S( "type %d behaviour not possible", type);
  }
  else
    WARNc1_S( "type %d behaviour not possible", type);

  if(name)
  {
    c = atoi(name);
    oyFree_m_( name )
  }

  if(c < 0)
    c = oyOptionGet_((oyWIDGET_e)type)-> default_value;

  DBG_PROG_ENDE
  return c;
}



int
oySetProfile_      (const char* name, oyPROFILE_e type, const char* comment)
{
  int r = 1;
  const char *fileName = 0, *com = comment;

  DBG_PROG_START

  /* extract filename */
  if (name && strrchr(name , OY_SLASH_C))
  {
    fileName = strrchr(name , OY_SLASH_C);
    fileName++;
  } else
    fileName = name;

  DBG_PROG2_S( "name = %s type %d", name, type )

  if ( name == 0 || !oyCheckProfile_ (fileName, 0) )
  {
    const char* config_name = 0;
    DBG_PROG2_S("set fileName = %s as %d profile\n",fileName, type)
    if ( type < 0 )
      WARNc2_S( "%s %d", _("default profile type does not exist:"), type  );

    if(oyWidgetTitleGet_( type, 0,0,0,0 ) == oyWIDGETTYPE_DEFAULT_PROFILE)
    {
      config_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;
#ifdef __APPLE__
      /* these settings are not persistent (osX 10.4) */
      /*if (0)*/
      {
        OSType dataColorSpace = 0;
        CMError err;

        switch(type)
        {
      case oyEDITING_RGB:            /**< Rgb Editing (Workspace) Profile */
                dataColorSpace = cmRGBData; break;
      case oyEDITING_CMYK:           /**< Cmyk Editing (Workspace) Profile */
                dataColorSpace = cmCMYKData; break;
      case oyEDITING_XYZ:            /**< XYZ Editing (Workspace) Profile */
                dataColorSpace = cmXYZData; break;
      case oyEDITING_LAB:            /**< Lab Editing (Workspace) Profile */
                dataColorSpace = cmLabData; break;
      case oyEDITING_GRAY:           /**< Gray Editing (Workspace) Profile */
                dataColorSpace = cmGrayData; break;

      case oyASSUMED_RGB:            /**< standard RGB assumed source profile */
      case oyASSUMED_WEB:            /**< std internet assumed source static_profile*/
      case oyASSUMED_CMYK:           /**< standard Cmyk assumed source profile*/
      case oyASSUMED_XYZ:            /**< standard XYZ assumed source profile */
      case oyASSUMED_LAB:            /**< standard Lab assumed source profile */
      case oyASSUMED_GRAY:           /**< standard Gray assumed source profile*/
      case oyPROFILE_PROOF:          /**< standard proofing profile */
      case oyDEFAULT_PROFILE_START:
      case oyDEFAULT_PROFILE_END:
                break;
        }


        if(dataColorSpace != 0)
        {
          CMProfileLocation loc;
          CMProfileRef prof=NULL;
          const char *profil_basename;
          char * profil_pathname = oyGetPathFromProfileName( name, oyAllocateFunc_);
          char* file_name = oyAllocateFunc_(MAX_PATH);
          FSRef   ref;
          Boolean isDirectory;

          if(name && strrchr(name,OY_SLASH_C))
            profil_basename = strrchr(name,OY_SLASH_C)+1;
          else
            profil_basename = name;


          /*loc.locType = cmPathBasedProfile;*/
          snprintf( file_name/*loc.u.pathLoc.path*/, 255, "%s%s%s",
                    profil_pathname, OY_SLASH, profil_basename);

          loc.locType = cmFileBasedProfile;

          /*err = FSMakeFSSpec ( 0, 0, file_name, &loc.u.fileLoc.spec);*/
          err = FSPathMakeRef ( (unsigned char*) file_name, &ref, &isDirectory);
          err = FSGetCatalogInfo ( &ref, 0, NULL, NULL,
                                   &loc.u.fileLoc.spec, NULL );

          err = CMOpenProfile ( &prof, &loc );
          if(err) return err;

          err = CMSetDefaultProfileBySpace ( dataColorSpace, prof);
          if(err) return err;
          err = CMCloseProfile( prof );

          oyFree_m_(file_name);
          oyFree_m_( profil_pathname );
          return err;
        }
      }
#endif
    } else if(type == oyDEVICE_PROFILE)
      {
        int len = strlen(OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* keyName = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (keyName, "%s%s", OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (keyName, com, 0); DBG_PROG
        DBG_PROG2_S( "%s %d", keyName, len )
        oyFree_m_ (keyName)
      }
      else
        WARNc2_S( "%s %d", _("default profile type does not exist:"), type );
      
    
    if(config_name)
    {
      if(name) {
        r = oyAddKey_valueComment_ (config_name, fileName, com);
        DBG_PROG3_S( "%s %s %s",config_name,fileName,com?com:"" )
      } else {
        KeySet* list;
        Key *current;
        char* value = (char*) calloc (sizeof(char), MAX_PATH);
        int rc = 0;

        DBG_PROG

        list = oyReturnChildrenList_(OY_STD, &rc ); ERR
        if(!list)
        {
          FOR_EACH_IN_KDBKEYSET( current, list )
          {
            keyGetName(current, value, MAX_PATH);
            DBG_NUM_S( value )
            if(strstr(value, config_name) != 0 &&
               strlen(value) == strlen(config_name))
            {
              DBG_PROG_S(value)
              if(!oy_handle_)
                return 1;
              kdbRemove ( oy_handle_, value );
              break;
            }
          }
        }

        DBG_NUM_S( value )

        oyFree_m_ (list) DBG_PROG
        oyFree_m_ (value) DBG_PROG
        oyClose_(); DBG_PROG
        r = rc;
      }
      DBG_PROG_V( r )
    }
  }

  DBG_PROG_ENDE
  return r;
}


/* public API implementation */

/**@brief read Key value
 *
 *  1. ask user
 *  2. if user has no setting ask system
 */
char*
oyGetKeyString_ ( const char       *key_name,
                 oyAlloc_f         allocate_func )
{
  char* name = 0;
  char* full_key_name = 0;
  int rc = 0;

  if( !key_name || strlen( key_name ) > MAX_PATH-1 )
  { WARNc_S("wrong string format given");
    return 0;
  }

  name = (char*) oyAllocateWrapFunc_( MAX_PATH, allocate_func );
  full_key_name = (char*) oyAllocateFunc_ (MAX_PATH);

  if( !name || !full_key_name )
    return 0;

  sprintf( full_key_name, "%s%s", OY_USER, key_name );

  name[0] = 0;
  if(!oy_handle_)
    return 0;
  rc = kdbGetString_m ( oy_handle_, full_key_name, name, MAX_PATH );

  if( rc || !strlen( name ))
  {
    sprintf( full_key_name, "%s%s", OY_SYS, key_name );
    rc = kdbGetString_m ( oy_handle_, full_key_name, name, MAX_PATH );
  }

  free( full_key_name );

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  if(rc)
    return name;
  else
    return 0;
}


/* device profiles API */

char*
oyGetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    oyAlloc_f     allocate_func)
{
  char* profileName = 0;
  int rc=0;

  oyComp_t_ *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet *profilesList;

  DBG_PROG_START

  profilesList = 
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR

  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileName;
  }

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    int count = 0;
    foundEntry = 0;
    DBG_PROG1_S( "matchList->begin->next: %d\n", (int)(intptr_t)matchList->begin->next )
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      DBG_PROG2_S( "testEntry %d count: %d\n", (int)(intptr_t)testEntry, count++ )
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)));

    /* 7. tell about the profile and its hits */
    if(foundEntry)
    {
      char *fileName = 0;
      int len;
      if (foundEntry->name)
        DBG_PROG_S( foundEntry->name );
      if (foundEntry->name &&
          strlen(foundEntry->name) &&
          strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      len = strlen (fileName)+1;
      profileName = (char*) oyAllocateWrapFunc_( len, allocate_func );
      sprintf (profileName, fileName);

      DBG_PROG_S(foundEntry->name)
      DBG_PROG_S(profileName)
      oyDestroyCompList_ (matchList);
    }
  }

  ksDel (profilesList);
  oyClose_();

  DBG_PROG_ENDE
  return profileName;
}

#if 0
char**
oyGetDeviceProfile_s               (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    int** number)
{
  char** profileNames = 0;
  char*  profileName = 0;
  int    rc;

  oyComp_t_ *matchList = 0,
         *testEntry = 0,
         *foundEntry = 0;
  KeySet* profilesList;

  DBG_PROG_START

  kdbOpen();

  /* TODO merge User and System KeySets in oyReturnChildrenList_ */
  profilesList = oyReturnChildrenList_(OY_USER OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList) {
    oyClose_();

    DBG_PROG_ENDE
    return profileNames;
  }

  matchList = oyGetDeviceProfile_sList (manufacturer, model, product_id,
                                        host, port, attrib1, attrib2, attrib3,
                                        profilesList, rc);

  /* 6. select the profile from the match list with the most hits */
  if (matchList)
  {
    int max_hits = 0;
    foundEntry = 0;
    for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
    {
      if (testEntry->hits > max_hits)
      {
        foundEntry = testEntry;
        max_hits = testEntry->hits;
      }
    }
    if(foundEntry) DBG_PROG_S ((printComp (foundEntry)))

    /* 7. tell about the profile and its hits */
    {
      char *fileName = 0;

      if (strrchr(foundEntry->name , OY_SLASH_C))
      {
        fileName = strrchr(foundEntry->name , OY_SLASH_C);
        fileName++;
      }
      else
        fileName = foundEntry->name;

      profileName = (char*) calloc (strlen (fileName)+1, sizeof(char));
      sprintf (profileName, fileName);
      /* @TODO add profileName to profileNames */

      DBG_PROG_S((foundEntry->name))
      DBG_PROG_S((profileName))
      oyDestroyCompList_ (matchList);
    }
  }

  ksDel (profilesList);
  kdbClose();

  DBG_PROG_ENDE
  return profileNames;
}
#endif

  /**
   * @internal
   * Search description
   *
   * This routine describes the A approach
   *   - registred profiles with assigned devices
   *
   * -# take all arguments and walk through the named devices list \n
   *    //  named devices consist of an key with the profile name + attributes\n
   *    //  it is not allowed to have two profiles with the same name\n
   *    //  it is allowed to have different profiles for the same attribute :(\n
   *    //  specify more attributes to make an decission presumable\n
   *    //   or maintain profiles, erasing older and invalid ones
   * -# test if attributes matches the value of the key, count the hits
   * -# search the profile in an match list
   * -# add the profile to the match list if not found
   * -# increase the hits counter in the macht list for that profile
   * -# select the profile from the match list with the most hits
   * -# tell about the profile and its hits
   *
   * @todo approach B:\n
   * no attributes are assigned beside certain keyword ("monitor", "scanner")\n
   * scan profile tags for manufacturer, device descriptions ... \n<ul>
   * <li> When to start an automatic registration run?</li>\n
   * <li> include profile tag editing?</li>
   * </ul>
   * @todo other things:\n
   * <ul>
   * <li> spread weighting? 3 degrees are sufficient How to merge in the on
   *       string approach?</li></ul>
   *
   */


oyComp_t_*
oyGetDeviceProfile_sList           (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    KeySet *profilesList,
                                    int   rc)
{
  /* 1. take all arguments and walk through the named devices list */
  int i = 0, n = 0;
  char* name  = (char*) calloc (MAX_PATH, sizeof(char));
  char* value = (char*) calloc (MAX_PATH, sizeof(char));
  const char **attributs = NULL;
  Key *current;
  oyComp_t_ *matchList = 0,
            *testEntry = 0;

  DBG_PROG_START

  oyAllocHelper_m_(attributs, const char*, 8 * sizeof (const char*), 0, )

  attributs[0] = manufacturer;
  attributs[1] = model;
  attributs[2] = product_id;
  attributs[3] = host;
  attributs[4] = port;
  attributs[5] = attrib1;
  attributs[6] = attrib2;
  attributs[7] = attrib3;

# if 1
  for (i = 0; i <= 7; ++i)
    DBG_PROG2_S ("%ld %ld", (long int)attributs[i], (long int)model);
# endif


  if (profilesList)
  {
    FOR_EACH_IN_KDBKEYSET( current, profilesList )
    {
      n = 0;
      keyGetString (current, value, MAX_PATH);
      keyGetName   (current, name,  MAX_PATH);

      /* 2. test if attributes matches the value of the key, count the hits */
      for (i = 0; i < 8; i++)
      {
        DBG_PROG2_S ("%d: %s", i, attributs[i])
        if (value && attributs[i] &&
            (strstr(value, attributs[i]) != 0))
        {
          /*  Here comes a ranking for attributes, which should be better placed
           *  in the profile or keys value itself for flexibility.
           */
          if      (i == 0) n += 2;
          else if (i == 1) n += 2;
          else if (i == 2) n += 5;
          else if (i == 4) n += 4;
          else             ++n;
          DBG_PROG1_S( "attribute count n = %d", n );
        }
      }

      if (n >= 5)
      { /* 3. search the profile in an match list */
        int found = 0; DBG_PROG
        if (matchList)
        {
          for (testEntry=matchList->begin; testEntry; testEntry=testEntry->next)
          {
            DBG_PROG2_S( "%s %s", testEntry->name, name )
            if (testEntry->name && strlen(name) &&
                strstr(testEntry->name, name) != 0)
            { DBG_PROG1_S( "%s", strstr(testEntry->name, name) )
              found = 1;
              WARNc1_S("double occurency of profile %s", testEntry->name)
              /* anyway increase the hits counter if attributes fits better */
              if (testEntry->hits < n)
                testEntry->hits = n;
            }
          }
        }
        /* 4. add the profile to the match list if not found (normal case) */
        if (!found)
        {
          DBG_PROG1_S( "new matching profile found %s", name )
          matchList = oyAppendComp_ (matchList, 0);
          DBG_PROG_S (printComp (matchList))
          /* 5. increase the hits counter in the match list for that profile */
          oySetComp_ ( matchList, name, value, n );
          DBG_PROG_S (printComp (matchList))
        }
      }
    }
  } else
    WARNc_S ("No profiles yet registred to devices");

  oyFree_m_ (attributs)
  oyFree_m_ (name)
  oyFree_m_ (value)

  DBG_PROG_ENDE
  return matchList;
}


int
oyEraseDeviceProfile_              (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3)
{
  char* profile_name = 0;
  int rc=0;
  KeySet* profilesList = 0;
  Key *current;
  char* value;

  DBG_PROG_START

  DBG_PROG

  profilesList =
   oyReturnChildrenList_(OY_REGISTRED_PROFILES, &rc ); ERR
  if(!profilesList)
  {
    oyClose_();

    DBG_PROG_ENDE
    return rc;
  }

  value = (char*) calloc (sizeof(char), MAX_PATH);
  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                      host, port, attrib1, attrib2, attrib3,
                                      oyAllocateFunc_);

  DBG_PROG1_S("profile_name %s", profile_name )

  FOR_EACH_IN_KDBKEYSET( current, profilesList )
  {
    keyGetName(current, value, MAX_PATH);
    DBG_NUM_S( value )
    if(profile_name &&
       strstr(value, profile_name) != 0) {
      DBG_PROG_S((value))
      kdbRemove ( oy_handle_, value ); 
      break;
    }
  }

  DBG_NUM_S( value )

  if(profilesList) ksDel(profilesList); DBG_PROG
  oyFree_m_ (value) DBG_PROG
  oyFree_m_ (profile_name) DBG_PROG
  oyClose_(); DBG_PROG

  DBG_PROG_ENDE
  return rc;
}



