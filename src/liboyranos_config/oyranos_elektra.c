/** @file oyranos_elektra.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    elektra dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include "oyranos_types.h"

#ifdef HAVE_POSIX
#include <unistd.h> /* geteuid() */
#endif

#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kdb.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_io.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_xml.h"

#define DBG_EL1_S DBG_NUM1_S
#define DBG_EL2_S DBG_NUM2_S
#define DBG_EL_S DBG_NUM_S

#ifndef KDB_VERSION_MAJOR
#define KDB_VERSION_MAJOR 0
#endif
#ifndef KDB_VERSION_MINOR
#define KDB_VERSION_MINOR 0
#endif
#define KDB_VERSION_NUM (KDB_VERSION_MAJOR*10000 + KDB_VERSION_MINOR*100)

#if KDB_VERSION_NUM >= 800
Key * error_key = 0;
#define kdbOpen_m() kdbOpen(error_key)
#define kdbClose_m(a) kdbClose(a, error_key)
#define kdbGetString_m kdbGetString
#define kdbGetChildKeys(a,b,c,d) oyGetByName(c,b)
#define kdbGetKey(a,b) oyGetKey(b)
#define kdbSetKey(a,b) oySetKey(b)
#define kdbRemove(a,b) oyRemoveFromDB(b)
#define ksAppendKeys ksAppend
#define KDBHandle KDB
#define keyIsDir(a) 0
#define keyRemove(a) 0
#define keySetComment(a,b) keySetMeta(a,"comment",b)

#elif KDB_VERSION_NUM >= 700
#define kdbOpen_m() kdbOpen()
#define kdbClose_m(a) kdbClose(a)
#define kdbGetString_m kdbGetString
#define kdbGetChildKeys(a,b,c,d) kdbGetByName(a,c,b,d)
#define ksAppendKeys ksAppend
#define KDBHandle KDB

#else
#define kdbGetString_m kdbGetValue
#endif

char oy_elektra_error_text[24] = {0};

char * oy__kdbStrError(int rc) { sprintf(oy_elektra_error_text, "elektra: %d", rc);
                                 return oy_elektra_error_text; }

#if KDB_VERSION_NUM > 600
#define kdbStrError(t) oy__kdbStrError(t)
#endif

#if KDB_VERSION_NUM >= 800
#define oyERR(k) { const Key *meta = NULL; keyRewindMeta(k); \
                   if(rc <= 0) { \
                     while((meta = keyNextMeta(k)) != 0) { \
                       WARNc3_S( "rc:%d %s:\t%s", rc, \
                                 oyNoEmptyString_m_( keyName(meta) ), \
                                 oyNoEmptyString_m_( keyString(meta) ) ); \
                     } \
                 } }
#else
#define oyERR(k)
#endif

/* --- Helpers  --- */

/* --- static variables   --- */

static int oyranos_init = 0;

/* --- structs, typedefs, enums --- */


/* --- internal API definition --- */

/* elektra key list handling */
KeySet* oyReturnChildrenList_  (const char* keyParentName,int* rc);



/* separate from the external functions */


#define oyDEVICE_PROFILE oyDEFAULT_PROFILE_END

/* test2.cpp references oy_handle_ for testing */
#if DEBUG_NEVER
static 
#endif
KDBHandle * oy_handle_ = 0;

void oyOpen_ (void)
{
  if(!oyranos_init) {
#if KDB_VERSION_NUM >= 800
    error_key = keyNew( KEY_END );
    DBG_EL1_S("error_key = keyNew( KEY_END ) == %s", error_key ? "good":"NULL");
#endif
    oy_handle_ = kdbOpen_m( /*&oy_handle_*/ );
    DBG_EL1_S("oy_handle_ = kdbOpen_m() == %s", oy_handle_ ? "good":"NULL");
    if(!oy_handle_)
      WARNc_S("Could not initialise Elektra.");
    oyranos_init = 1;
  }
}
void oyClose_() { /*kdbClose( &oy_handle_ );*/ }
/* @todo make oyOpen unnecessary */
void oyOpen  (void) { oyOpen_(); }
void oyClose (void) { oyClose_(); }
void oyCloseReal__() {
#if KDB_VERSION_NUM >= 800
                       int rc=
#endif
                       kdbClose_m( oy_handle_ ); oyERR(error_key)
                       oy_handle_ = 0;
                       oyranos_init = 0; }

/* oyranos part */



/* small helpers */

  /* ksNext uses the same entry twice in a 1 component KeySet, we avoid this */
#define FOR_EACH_IN_KDBKEYSET( current_, list ) \
   ksRewind( list );  \
   for( current_ = ksNext( list ); current_; current_ = ksNext( list )  )


#if KDB_VERSION_NUM >= 800
int oyGetByName(KeySet * conf, const char * base)
{
  Key *key = keyNew(base,KEY_END);
  int rc = kdbGet(oy_handle_, conf, key); oyERR(key)
  keyDel(key);
  return rc;
}

int  oyGetKey                        ( Key               * key )
{
  KeySet * ks;
  Key * result;
  int rc;

  ks = ksNew(0);
  DBG_EL1_S( "ks = ksNew( 0 ) == %s", ks ? "good":"NULL" );
  rc = kdbGet( oy_handle_, ks, key ); oyERR(key)
  DBG_EL1_S( "rc = kdbGet( oy_handle_, ks, key ) == %d", rc );

  result = ksLookup( ks, key, KDB_O_NONE);
  DBG_EL1_S( "result = ksLookup( ks, key, KDB_O_NONE) == %s", result ? "good":"NULL" );
  if(!rc && !result)
  {
    rc = -1;
    WARNc1_S( "keyString(key) == %s", oyNoEmptyString_m_(keyString(key)) );
    oyERR(key)
  }
  DBG_EL_S( "keyCopy( key, result )" );
  keyCopy( key, result );
  keyDel( result );
  DBG_EL_S( "keyDel( result )" );
  ksDel( ks );
  DBG_EL_S( "ksDel( ks )" );
  return rc;
}

int  oySetKey                        ( Key               * key )
{
  KeySet * ks;
  int rc;

  ks = ksNew(0);
  DBG_EL1_S( "ks = ksNew( 0 ) == %s", ks ? "good":"NULL" );
  rc = kdbGet( oy_handle_, ks, key ); oyERR(key)
  DBG_EL1_S( "kdbGet( oy_handle_, ks, key ) = %d", rc );
  if(!rc)
  {
    rc = kdbSet( oy_handle_, ks, key ); oyERR(key)
    DBG_EL1_S( "kdbSet( oy_handle_, ks, key ) = %d", rc );
  }
  ksDel( ks );
  DBG_EL_S( "ksDel( ks )" );
  return rc;
}

int  oyRemoveFromDB                  ( const char        * name )
{
  Key *key;
  KeySet * ks;
  int rc;
  
  key = keyNew(name,KEY_END);
  DBG_EL1_S( "key = keyNew( \"%s\", KEY_END) = %s", key ? "good":"NULL" );
  ks = ksNew(0);
  DBG_EL1_S( "ks = ksNew( 0 ) == %s", ks ? "good":"NULL" );
  rc = kdbGet(oy_handle_, ks, key); oyERR(key)
  DBG_EL1_S( "kdbGet( oy_handle_, ks, key ) = %d", rc );
  ksClear(ks);
  if(!rc)
  {
    rc = kdbSet( oy_handle_, ks, key ); oyERR(key)
    DBG_EL1_S( "kdbSet( oy_handle_, ks, key ) = %d", rc );
  }
  ksDel( ks );
  DBG_EL_S( "ksDel( ks )" );
  keyDel(key);
  DBG_EL_S( "keyDel( key )" );
  return rc;
}
#endif /* KDB_VERSION_NUM >= 800 */

/* --- function definitions --- */

KeySet*
oyReturnChildrenList_ (const char* keyParentName, int* rc_ptr)
{
  int user_sys = oyUSER_SYS,
      rc = 0;
  KeySet*list_user = 0;
  KeySet*list_sys = 0;
  KeySet*list = ksNew(0);
  char  *list_name_user = NULL;
  char  *list_name_sys = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(list_name_user, char, MAX_PATH, 0,; )
  oyAllocHelper_m_(list_name_sys, char, MAX_PATH, 0,; )

  if( user_sys == oyUSER_SYS || user_sys == oyUSER ) {
    list_user = ksNew(0);
    sprintf(           list_name_user, "%s%s", OY_USER, keyParentName);
    if(!oy_handle_)
    {
      *rc_ptr = 1;
      return 0;
    }
    rc =
      kdbGetChildKeys( oy_handle_, list_name_user, list_user, KDB_O_SORT); oyERR(error_key)

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
  }
  if( user_sys == oyUSER_SYS || user_sys == oySYS ) {
    list_sys = ksNew(0);
    sprintf(           list_name_sys, "%s%s", OY_SYS, keyParentName);
    if(!oy_handle_)
    {
      *rc_ptr = 1;
      return 0;
    }
    rc =
      kdbGetChildKeys( oy_handle_, list_name_sys, list_sys, KDB_O_SORT); oyERR(error_key)

    if(rc > 0)
      DBG_NUM1_S("kdbGetChildKeys returned with %d", rc);
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

char* oySearchEmptyKeyname_ (const char* key_parent_name)
{
  const char * key_base_name = oySelectUserSys_();
  char* new_key_name = NULL;
  int nth = 0, i = 1, rc=0;
  Key *key = 0;
  char *name = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(name, char, MAX_PATH, 0, return 0 )
  sprintf(name, "%s%s", key_base_name, key_parent_name);

  oyAllocHelper_m_( new_key_name, char,
                    oyStrlen_(name) + strlen(key_base_name) + 24, 0,; )

  /* search for empty keyname */
  while (!nth)
  {
    sprintf (new_key_name, "%s" OY_SLASH "%d", name, i);

    if(!oy_handle_)
      return 0;

    key = keyNew( new_key_name, KEY_END );

    rc=kdbGetKey( oy_handle_, key ); oyERR(key)
    if( rc == -1 &&
        !keyIsDir( key ) &&
        !keyIsString(key) &&
        !keyIsBinary(key) )
      nth = i;
    keyDel( key );
    i++;
  }

  oyFree_m_( name )

  name = oyStringCopy_( new_key_name, oyAllocateFunc_ );
  sprintf (new_key_name, "%s", &name[oyStrlen_(key_base_name)]);
  oyFree_m_( name )

  DBG_PROG_ENDE
  return new_key_name;
} 

char **            oyKeySetGetNames_ ( const char        * key_parent_name,
                                       int               * n )
{
  int error = !key_parent_name || !n;
  int rc=0;
  char* current_name = (char*) calloc (sizeof(char), MAX_PATH);
  KeySet * my_key_set = 0;
  Key * current = 0;
  const char *name = NULL;
  char ** texts = 0;

  DBG_PROG_START

  name = key_parent_name;

  if(!error)
    *n = 0;

  if(!error)
    my_key_set = oyReturnChildrenList_( name, &rc );

  if(my_key_set)
  {
    FOR_EACH_IN_KDBKEYSET( current, my_key_set )
    {
      keyGetName(current, current_name, MAX_PATH);
      if(current_name &&
         oyStrstr_(current_name, name) &&
         !oyStrrchr_(&((char*)oyStrstr_(current_name, name))[oyStrlen_(name)+1],
                     OY_SLASH_C ) )
        oyStringListAddStaticString_( &texts, n,
                                      oyStrstr_(current_name, name),
                                      oyAllocateFunc_, oyDeAllocateFunc_);
    }
  }
  ksDel (my_key_set);
  oyClose_();

  DBG_PROG_ENDE
  return texts;
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

  myKeySet = oyReturnChildrenList_( keyParentName, &rc );
        if(myKeySet)
        {
          FOR_EACH_IN_KDBKEYSET( current, myKeySet )
          {
            keyGetName(current, value, MAX_PATH);
            if(strstr(value, ask_value) != 0 &&
               strlen(value) == strlen(ask_value))
            {
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
oyAddKey_valueComment_ (const char* key_name,
                        const char* value,
                        const char* comment)
{
  int rc=0,
      max_len;
  Key *key;
  char *name = NULL;
  char *value_utf8 = NULL;
  char *comment_utf8 = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(name, char, MAX_PATH, 0,; )
  oyAllocHelper_m_(value_utf8, char, MAX_PATH, 0,; )
  oyAllocHelper_m_(comment_utf8, char, MAX_PATH, 0,; )

  sprintf(name, "%s%s", oySelectUserSys_(), key_name);
  if(value && oyStrlen_(value))
  {
    max_len = strlen(value) < MAX_PATH ? strlen(value) : MAX_PATH;
    oyIconv( value, max_len, max_len,
             value_utf8, 0, "UTF-8" );
  }
  if(comment && oyStrlen_(comment))
  {
    max_len = strlen(comment) < MAX_PATH ? strlen(comment) : MAX_PATH;
    oyIconv( comment, max_len, max_len,
             comment_utf8, 0, "UTF-8" );
  }

  if (key_name)
    DBG_PROG_S(( key_name ));
  if (value)
    DBG_PROG_S(( value ));
  if (comment)
    DBG_PROG_S(( comment ));
  if (!key_name || !strlen(key_name))
    WARNc_S( "no key_name given" );

  key = keyNew( KEY_END );
  DBG_EL1_S( "key = keyNew( KEY_END ) == %s", key ? "good":"NULL" );
  keySetName( key, name );
  DBG_EL1_S( "keySetName( key, \"%s\" )", name );

  if(!oy_handle_)
    goto clean;
  rc=kdbGetKey( oy_handle_, key ); oyERR(key)
  DBG_EL1_S( "rc = kdbGetKey( oy_handle, key ) == %d", rc );
  if(rc < 0 && oy_debug)
    oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_"key new? code:%d %s name:%s",
                     OY_DBG_ARGS_, rc, kdbStrError(rc), name);
  if(value)
  {
    rc=keySetString (key, value_utf8);
    DBG_EL2_S( "rc = keySetString( key, \"%s\" ) == %d", value_utf8, rc);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_"code:%d %s name:%s value:%s",
                       OY_DBG_ARGS_, rc, kdbStrError(rc), name, value);
  }
  if(comment)
  {
    rc=keySetComment (key, comment_utf8);
    DBG_EL2_S( "rc = keySetComment( key, \"%s\" ) == %d", comment_utf8, rc);
    if(rc <= 0)
      oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_"code:%d %s name:%s comment:%s",
                       OY_DBG_ARGS_, rc, kdbStrError(rc), name, comment);
  }

  oyOpen_();
  rc=kdbSetKey( oy_handle_, key ); oyERR(key)
  DBG_EL1_S( "rc = kdbSetKey( oy_handle, key ) == %d", rc );
  if(rc < 0)
    oyMessageFunc_p( oyMSG_WARN, 0, OY_DBG_FORMAT_ "code:%d %s name:%s",
                     OY_DBG_ARGS_, rc, kdbStrError(rc), name);
  oyClose_();
  keyDel( key );
  DBG_EL_S( "keyDel( key )" );


  clean:
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
#ifdef HAVE_POSIX
  if(geteuid() == 0)
#endif
    return OY_SYS;
#ifdef HAVE_POSIX
  else
    return OY_USER;
#endif
}


int
oySetBehaviour_      (oyBEHAVIOUR_e type, int choice)
{
  int r = 1;

  DBG_PROG_START

  DBG_PROG2_S( "type = %d behaviour %d", type, choice )

  if ( (r=oyTestInsideBehaviourOptions_(type, choice)) == 1 )
  {
    const char *key_name = 0;

    key_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

      if(key_name)
      {
        char val[12];
        const char *com =
            oyOptionGet_((oyWIDGET_e)type)-> choice_list[ choice ];
        snprintf(val, 12, "%d", choice);
        r = oyAddKey_valueComment_ (key_name, val, com);
        DBG_PROG4_S( "%s %d %s %s", key_name, type, val, com?com:"" )
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

    if(oyWidgetTitleGet_( (oyWIDGET_e)type, 0,0,0,0 ) ==
       oyWIDGETTYPE_DEFAULT_PROFILE)
    {
      config_name = oyOptionGet_((oyWIDGET_e)type)-> config_string;

    } else if(type == oyDEVICE_PROFILE)
      {
        int len = strlen(OY_REGISTRED_PROFILES)
                  + strlen(fileName);
        char* key_name = (char*) calloc (len +10, sizeof(char)); DBG_PROG
        sprintf (key_name, "%s%s", OY_REGISTRED_PROFILES OY_SLASH, fileName); DBG_PROG
        r = oyAddKey_valueComment_ (key_name, com, 0); DBG_PROG
        DBG_PROG2_S( "%s %d", key_name, len )
        oyFree_m_ (key_name)
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

        list = oyReturnChildrenList_(OY_STD, &rc );
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
              kdbRemove ( oy_handle_, value ); oyERR(error_key)
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


int      oyKeyIsString_              ( const char        * full_key_name )
{
  Key * key = 0;
  int success = 0,
      rc = 0;

  /** check if the key is a binary one */
  key = keyNew( full_key_name, KEY_END );
  rc=kdbGetKey( oy_handle_, key ); oyERR(key)
  if(rc > 0)
    WARNc1_S("kdbGetKey returned with %d", rc);
  success = keyIsString(key);
  keyDel( key ); key = 0;

  return success;
}
int      oyKeyIsBinary_              ( const char        * full_key_name )
{
  Key * key = 0;
  int success = 0,
      rc = 0;

  /** check if the key is a binary one */
  key = keyNew( full_key_name, KEY_END );
  rc=kdbGetKey( oy_handle_, key ); oyERR(key)
  if(rc > 0)
    WARNc1_S("kdbGetKey returned with %d", rc);
  success = keyIsBinary(key);
  keyDel( key ); key = 0;

  return success;
}

oyPointer  oyGetKeyBinary__          ( const char        * full_key_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  oyPointer ptr = 0;
  Key * key = 0;
  int rc = 0;
  ssize_t new_size = 0;

  key = keyNew( full_key_name, KEY_END );
  rc=kdbGetKey( oy_handle_, key ); oyERR(key)
  if(rc > 0)
    WARNc1_S("kdbGetKey returned with %d", rc);
  if(keyIsBinary(key))
  {
    new_size = keyGetValueSize( key );
    oyAllocHelper_m_( ptr, uint8_t, new_size, allocate_func, return 0 )
    new_size = keyGetBinary( key, ptr, new_size );
    if(new_size)
      * size = new_size;
  }

  keyDel( key ); key = 0;

  return ptr;
}

/* public API implementation */

oyPointer  oyGetKeyBinary_           ( const char        * key_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  oyPointer ptr = 0;
  char* full_key_name = 0;

  if( !key_name || strlen( key_name ) > MAX_PATH-1 )
  { WARNc_S("wrong string format given");
    goto clean2;
  }

  full_key_name = (char*) oyAllocateFunc_ (MAX_PATH);

  if( !full_key_name )
    goto clean2;

  sprintf( full_key_name, "%s%s", OY_USER, key_name );

  if(!oy_handle_)
    goto clean2;

  if(oyKeyIsBinary_(full_key_name))
    ptr = oyGetKeyBinary__ ( full_key_name, size, allocate_func );

  if( !ptr || !*size )
  {
    sprintf( full_key_name, "%s%s", OY_SYS, key_name );
    if(oyKeyIsBinary_(full_key_name))
      ptr = oyGetKeyBinary__ ( full_key_name, size, allocate_func );
  }

  clean2:
  oyDeAllocateFunc_( full_key_name );

  DBG_PROG_ENDE
  return ptr;
}

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
  Key * key = 0;
  int success = 0;

  if( !key_name || strlen( key_name ) > MAX_PATH-1 )
  { WARNc_S("wrong string format given");
    goto clean3;
  }

  name = (char*) oyAllocateWrapFunc_( MAX_PATH, allocate_func );
  full_key_name = (char*) oyAllocateFunc_ (MAX_PATH);

  if( !name || !full_key_name )
    goto clean3;

  sprintf( full_key_name, "%s%s", OY_USER, key_name );

  name[0] = 0;
  if(!oy_handle_)
    goto clean3;

  /** check if the key is a binary one */
  key = keyNew( full_key_name, KEY_END );
  rc=kdbGetKey( oy_handle_, key ); oyERR(key)
  success = keyIsString(key);

  if(success)
    rc = keyGetString ( key, name, MAX_PATH );
  keyDel( key ); key = 0;

  if( rc || !strlen( name ))
  {
    sprintf( full_key_name, "%s%s", OY_SYS, key_name );
    key = keyNew( full_key_name, KEY_END );
    rc=kdbGetKey( oy_handle_, key ); oyERR(key)
    success = keyIsString(key);
    if(success)
      rc = keyGetString( key, name, MAX_PATH );
    keyDel( key ); key = 0;
  }


  oyDeAllocateFunc_( full_key_name );

  DBG_PROG_S((name))
  DBG_PROG_ENDE
  if(rc)
    return name;
  else
    return 0;

  clean3:
  if(name)
    oyDeAllocateFunc_(name);
  if(full_key_name)
    oyDeAllocateFunc_( full_key_name );
  return 0;
}

int                oyEraseKey_       ( const char        * key_name )
{
  int error = !key_name,
      rc = 0,
      success = 0;
  Key * key = 0,
      * current = 0;
  KeySet * ks = 0;
  char * name = NULL,
       * value = NULL;

  if(!oy_handle_)
    return 1;

  oyAllocHelper_m_( name, char, MAX_PATH, 0, return 1 )

  oySprintf_( name, "%s%s", oySelectUserSys_(), key_name );

  if(!error)
  {
    key = keyNew( name, KEY_END );
    rc = kdbGetKey( oy_handle_, key ); oyERR(key)

    if(!keyIsDir( key ))
    {
      if( keyRemove( key ) == 1 )
      {
        rc = kdbSetKey( oy_handle_, key ); oyERR(key)
        if(rc == 0)
        {
          DBG_PROG1_S( "removed key %s", name );
          success = 1;
        }
      }
      keyDel( key );

      if(success)
        return error;

      rc = kdbRemove ( oy_handle_, name ); oyERR(error_key)
      if(rc == 0)    
      {
        return error;
      }
    }

    rc = 0;
    ks = oyReturnChildrenList_( key_name, &rc ); /* rc == 0 */
    if(ks)
    {
      oyAllocHelper_m_( value, char, MAX_PATH, 0, return 1 )

      FOR_EACH_IN_KDBKEYSET( current, ks )
      {
        keyGetName(current, value, MAX_PATH);

        if(strstr(value, key_name) != 0)
        {
          rc = kdbRemove ( oy_handle_, value ); oyERR(error_key)
          if(rc == 0)
          {
            DBG_PROG1_S( "removed key %s", value );
          }
        }
      }

      oyFree_m_( value );
    }

    rc = kdbRemove ( oy_handle_, name ); oyERR(error_key)
    if(rc == 0)
    {
      DBG_PROG1_S( "removed key %s", name );
    }
  }


  oyFree_m_( name );

  return error;
}

