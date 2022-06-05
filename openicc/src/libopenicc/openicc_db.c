/*  @file openicc_db.c
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2015-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2015/08/29
 */

#include "openicc_config_internal.h"
#include "oyjl_macros.h"
#include "openicc_db.h"
#include "xdg_bds.h"

#if HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif
#include <string.h>  /* strdup() */
#include <stdarg.h>  /* vsnprintf() */
#include <stdio.h>   /* vsnprintf() */


typedef struct {
  int dummy;
} openiccDummy_s;

struct openiccArray_s {
  openiccDummy_s ** array;
  int reserved_n;
};

int      openiccArray_Count          ( openiccArray_s    * array )
{
  int count = 0;
  while(array->array[count]) ++count;
  return count;
}

int      openiccArray_Add            ( openiccArray_s    * array,
                                       int                 add )
{
  int count = openiccArray_Count( array );

  if((count + 1) >= array->reserved_n)
  {
    int new_count = array->reserved_n * 5;
    openiccDummy_s ** ptrs;

    if(add > 1)
      new_count = count + add;

    ptrs = calloc( sizeof(openiccDummy_s*), new_count );
    if(!ptrs)
    {
      ERRc_S("%s new_count: %d", _("Could not alloc memory"), new_count );
      return 1;
    }

    memmove( ptrs, array->array, sizeof(openiccDummy_s*) * count );
    free( array->array );
    array->array = ptrs;
    array->reserved_n = new_count;
  }

  return 0;
}

int      openiccArray_Push           ( openiccArray_s    * array )
{
  return openiccArray_Add( array, 1 );
}


static char * openicc_scope_txt = NULL;
const char * openiccScopeGetString   ( openiccSCOPE_e      scope )
{

  if(!openicc_scope_txt)
    openicc_scope_txt = malloc(128);

  if(!openicc_scope_txt)
  {
    WARNcc_S( 0, "Out of memory", "" );
    return "----";
  }

  sprintf( openicc_scope_txt, "%s%s%s%s%s",
           scope == openiccSCOPE_USER_SYS ? "all" : "",
           scope & openiccSCOPE_USER ? (((scope & openiccSCOPE_USER) == scope)?"user":"user ") : "",
           scope & openiccSCOPE_SYSTEM ? (((scope & openiccSCOPE_SYSTEM) == scope)?"system":"system ") : "",
           scope & openiccSCOPE_OPENICC ? (((scope & openiccSCOPE_OPENICC) == scope)?"openicc":"openicc ") : "",
           scope & openiccSCOPE_MACHINE ? "machine" : "" );

  return openicc_scope_txt;
}
void           openiccLibRelease     ( void )
{
  if(openicc_scope_txt)
    free(openicc_scope_txt);
  openicc_scope_txt = NULL;
}

/** \addtogroup path_names
 *  @{
 */

/**
 *  @brief    obtain path string of OpenICC DB
 *
 *  @param[in]     scope               specify to intended user or system scope
 *  @return                            file path string
 */
char *        openiccDBGetJSONFile   ( openiccSCOPE_e      scope )
{
  const char * config_file = OPENICC_DB_PREFIX OPENICC_SLASH OPENICC_DB;
  /* Locate the directories where the config file is, */
  /* and where we should copy the profile to. */
  int npaths;
  xdg_error er;
  char **paths;
  char * db_file;
  

  if ((npaths = xdg_bds(&er, &paths, xdg_conf, xdg_write, 
                        (scope == openiccSCOPE_SYSTEM) ? xdg_local : xdg_user,
                        config_file)) == 0)
  {
    ERRc_S("%s %d", _("Could not find config"), scope );
    return NULL;
  }
  
  db_file = oyjlStringCopy( paths[0], malloc );
  xdg_free(paths, npaths);

  return db_file;
}
/*  @} *//* path_names */

/**
 *  @internal
 *  @brief    add a openiccConfig_s
 *  @memberof openiccDB_s
 */
int           openiccDB_AddScope     ( openiccDB_s       * db,
                                       const char        * top_key_name OI_UNUSED,
                                       openiccSCOPE_e      scope )
{
  int error = 0;
  const char * config_file = OPENICC_DB_PREFIX OPENICC_SLASH OPENICC_DB;
  int i;
  /* Locate the directories where the config file is, */
  /* and where we should copy the profile to. */
  int npaths;
  xdg_error er;
  char **paths;

  if ((npaths = xdg_bds(&er, &paths, xdg_conf, xdg_write, 
                        (scope == openiccSCOPE_SYSTEM) ? xdg_local : xdg_user,
                        config_file)) == 0)
  {
    ERRc_S("%s %d", _("Could not find config"), scope );
    return 1;
  }

  if(*openicc_debug)
    DBGc_S("%s", _("Paths:"));
  for(i=0; i < npaths; ++i)
    if(*openicc_debug)
      DBGc_S("%s", paths[i]);

  for(i = 0; i < npaths; ++i)
  {
    const char * db_file = paths[i];
    /* read JSON input file */
    int size = 0;
    char * text = NULL;

    if(openiccIsFileFull_( db_file, "r" ))
      text = openiccReadFile( db_file, &size );

    /* parse JSON */
    if(text)
    {
      int count = openiccArray_Count( (openiccArray_s*)&db->ks );
      openiccConfig_s * config = openiccConfig_FromMem( text );
      free(text); text = NULL;
      /* The file name is expected later on write. */
      openiccConfig_SetInfo ( config, db_file );

      /* reserve enough memory in list array */
      if( openiccArray_Push( (openiccArray_s*)&db->ks ))
      {
        ERRc_S("%s", _("Could not alloc memory") );
        return 1;
      }

      /* add new config to db */
      db->ks[count] = config;
    }
  }

  xdg_free(paths, npaths);

  return error;
}

/**
 *  @brief    create a new DB object
 *  @memberof openiccDB_s
 *
 *  Create a DB by scope. Locate the physical configuration DB's and 
 *  store them inside a single object.
 */
openiccDB_s * openiccDB_NewFrom      ( const char        * top_key_name,
                                       openiccSCOPE_e      scope )
{
  openiccDB_s * db;
  int error = 0;

  oyjlAllocHelper_m(db, openiccDB_s, 1, malloc, return db);

  db->type = openiccOBJECT_DB;
  db->top_key_name = oyjlStringCopy( top_key_name, malloc );
  if( !db->top_key_name ) { openiccDB_Release( &db ); return db; };
  db->scope = scope;
  db->ks_array_reserved_n = 10;
  oyjlAllocHelper_m( db->ks, openiccConfig_s*, db->ks_array_reserved_n, malloc, openiccDB_Release( &db ); return db );

  if(!error &&
     (db->scope == openiccSCOPE_USER_SYS || db->scope == openiccSCOPE_USER))
  {
    error = openiccDB_AddScope( db, top_key_name, openiccSCOPE_USER );
  }

  if(!error &&
     (db->scope == openiccSCOPE_USER_SYS || db->scope == openiccSCOPE_SYSTEM))
  {
    error = openiccDB_AddScope( db, top_key_name, openiccSCOPE_SYSTEM );
  }

  if(error)
    ERRc_S("%s: %s %d", _("Could not setup db objetc"), top_key_name, scope );

  return db;
}


/**
 *  @brief    free a DB object
 *  @memberof openiccDB_s
 */
void     openiccDB_Release           ( openiccDB_s      ** db )
{
  openiccDB_s * s;

  if(db)
  {
    int count, i;
    s = *db;

    if(!s)
      return;

    if( s->top_key_name )
      free( s->top_key_name );
    count = openiccArray_Count( (openiccArray_s*)&s->ks );
    for(i = 0; i < count; ++i)
      openiccConfig_Release( &s->ks[i] );
    if( s->ks )
      free( s->ks );
    s->ks_array_reserved_n = 0;
    free( s );
    *db = 0;
  }
}

/**
 *  @brief    get a value
 *  @memberof openiccDB_s
 *
 *  @param[in]     db                  a data base object
 *  @param[in]     xpath               key name to ask for
 *  @param[out]    value               found value; optional
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccDB_GetString (
                                       openiccDB_s       * db,
                                       const char        * xpath,
                                       const char       ** value )
{
  int error = !db || !xpath;

  if(error == 0)
  {
    int count = openiccArray_Count( (openiccArray_s*)&db->ks ), i;
    for(i = 0; i < count; ++i)
    {
      error = openiccConfig_GetString( db->ks[i], xpath, value );
      if(error == 0) break;
    }
  }

  return error;
}

/**
 *  @brief    get a filtered list of key names
 *  @memberof openiccDB_s
 *
 *  @param[in]     db                  a data base object
 *  @param[in]     xpath               top key name to filter for
 *  @param[in]     child_levels        how deeply nested child levels are
 *                                     desired; 0 - means all levels
 *  @param[in]     alloc               user allocation function; optional -
 *                                     default: malloc
 *  @param[out]    key_names           found full keys with path part; optional
 *  @param[out]    n                   number of found keys; optional
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int                openiccDB_GetKeyNames (
                                       openiccDB_s       * db,
                                       const char        * xpath,
                                       int                 child_levels,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc,
                                       char            *** key_names,
                                       int               * n )
{
  int error = !db || !xpath;

  if(!error)
  {
    int count = openiccArray_Count( (openiccArray_s*)&db->ks ), i;
    char ** ks = NULL;
    int     ks_n = 0;
    for(i = 0; i < count; ++i)
    {
      char ** ks_tmp = NULL;
      int     ks_tmp_n = 0;
      
      error = openiccConfig_GetKeyNames( db->ks[i], xpath, child_levels, alloc, &ks_tmp, &ks_tmp_n );
      if(ks_tmp)
      {
        oyjlStringListAddList( &ks, &ks_n, (const char **)ks_tmp, ks_tmp_n,
                                   alloc, dealloc );
        oyjlStringListRelease( &ks_tmp, ks_tmp_n, dealloc );
        oyjlStringListFreeDoubles( ks, &ks_n, dealloc );
      }
    }

    if(key_names)
      *key_names = ks;
    else
      oyjlStringListRelease( &ks, ks_n, dealloc );
    if(n) *n = ks_n;
  }

  return error;
}

/** \addtogroup OpenICC_config
 *  @{
 */

/**
 *  @brief    get a plain key name
 *
 *  This function takes in a key of pattern:
 *  "path1/path2/key.attribute" and returns a pure key: "key" without path
 *  parts or attributes.
 *
 *  @param[in]     key                 a key name string
 *  @param[in]     temp                a temporary string to be freed by the user
 *  @return                            the short key name
 */
const char * openiccGetShortKeyFromFullKeyPath( const char * key, char ** temp )
{
  /* key starts after the path */
  const char * key_short = strrchr( key, '/' ),
             * k;
  char * k_temp = NULL;
  if(key_short) ++key_short; else key_short = key;
  /* The key ends with before '.', which are essentially attributes, 
   *  or end of string. Cut '.' parts off. */
  k = strchr( key_short, '.' );
  if(k)
  {
    k_temp = oyjlStringCopy( key_short, malloc );
    if(!k_temp)
      ERRc_S( "could not allocate : %s", key_short );
    else
    {
      k_temp[strlen(key_short) - strlen(k)] = '\000';
      key_short = k_temp;
    }
  }
  *temp = k_temp;
  return key_short;
}

/**
 *  @brief    set a key name to a value
 *
 *  @param[in]     keyName             a key name string; e.g. "org/freedesktop/openicc/foo"
 *  @param[in]     scope               specify to intended user or system scope
 *  @param[out]    value               a value string; e.g. "bar"
 *  @param[in]     comment             a comment string;
 *                                     The keyName will be deleted with
 *                                     value=NULL and comment="delete".
 *  @return                            0 - success, >=1 - error, <0 - issue
 */
int      openiccDBSetString          ( const char        * keyName,
                                       openiccSCOPE_e      scope,
                                       const char        * value,
                                       const char        * comment )
{
  const char * xpath = keyName;
  openiccDB_s * db = openiccDB_NewFrom( keyName, scope );
  int error = keyName ? 0 : -1;

  if(!db)
  {
    error = 1;
    ERRcc_S( db, "%s [%s]/%s",
             _("Could not create a intermediate db object for"),
             openiccScopeGetString(scope), keyName?keyName:"" );
  }

  if(!error)
  {
    oyjl_val root, root_tmp = NULL;
    char * file_name;

    if(openiccArray_Count( (openiccArray_s*)&db->ks ))
    {
      root = db->ks[0]->oyjl;
      file_name = oyjlStringCopy( db->ks[0]->info, malloc );
    }
    else
    {
      openiccDB_Release( &db );
      root = root_tmp = (oyjl_val) calloc( sizeof(struct oyjl_val_s), 1 );
      file_name = openiccDBGetJSONFile( scope );
    }
    if(!file_name)
    {
      ERRc_S("%s", _("Could not alloc memory"));
      error = 1;
    }

    if(!error && root)
    {
      oyjl_val o = oyjlTreeGetValue( root, OYJL_CREATE_NEW, xpath );
      if(o)
      { 
        if(value == NULL && comment && strcmp(comment,"delete") == 0)
        {
          oyjlTreeClearValue( root, keyName );
        } else
          error = oyjlValueSetString( o, value );
        if(error)
        {
          ERRcc_S( db, "%s [%s]/%s",
                   _("Could not set JSON string for"),
                   openiccScopeGetString(scope), keyName?keyName:"" );
        } else
        {
          char * json = NULL;
          int size = 0, result = 0;

          json = oyjlTreeToText( root, OYJL_NO_MARKUP );
          if(json)
          {
            size = strlen(json);
            if(size)
            {
              result = openiccWriteFile( file_name, json, size );
              if(result != size)
              { error = 1;
                ERRcc_S( db, "%s [%s]/%s",
                         _("Writing failed for"), file_name,
                         openiccScopeGetString(scope), keyName?keyName:"" );
              }
            }
            else
            { error = 1;
              ERRcc_S( db, "%s [%s]/%s",
                       _("No JSON content obtained for"),
                       openiccScopeGetString(scope), keyName?keyName:"" );
            }

            free( json ); json = NULL;
          }
          else
          { error = 1;
            ERRcc_S( db, "%s [%s]/%s",
                     _("No JSON obtained for"),
                     openiccScopeGetString(scope), keyName?keyName:"" );
          }
        }
      }
      else
      { error = 1;
        ERRcc_S( db, "%s [%s]/%s",
                 _("Could not obtain JSON node for"),
                 openiccScopeGetString(scope), keyName?keyName:"" );
      }

    } else
    { error = 1;
      ERRcc_S( db, "%s [%s]/%s",
               _("Could not create root JSON node for"),
               openiccScopeGetString(scope), keyName?keyName:"" );
    }
    if(root_tmp) oyjlTreeFree(root_tmp);
    openiccDB_Release( &db );
    if(file_name) free(file_name);
  }

  return error;
}

/**
 *  @brief    get a empty key name to add a new config group to a array
 *
 *  @param[in]     keyParentName       a key name string
 *  @param[in]     scope               specify to intended user or system scope
 *  @return                            the free key name from the array
 */
char *   openiccDBSearchEmptyKeyname ( const char        * keyParentName,
                                       openiccSCOPE_e      scope )
{
  char * key = NULL;
  int end = 0;
  const char * xpath = keyParentName;
  openiccDB_s * db = openiccDB_NewFrom( xpath, scope );
  int error = !db || !xpath,
      xpath_is_array = 1;

  if(error == 0)
  {
    int count = openiccArray_Count( (openiccArray_s*)&db->ks ), i;
    for(i = 0; i < count; ++i)
    {
      oyjl_val o = oyjlTreeGetValue( db->ks[i]->oyjl, 0, xpath );
      error = !o ? -1:0;
      if(o && !OYJL_IS_ARRAY(o))
        xpath_is_array = 0;
      end = oyjlValueCount( o );
      if(error == 0) break;
    }
  }

  openiccDB_Release( &db );

  if(xpath_is_array)
    oyjlStringAdd( &key, 0,0, "%s/[%d]", keyParentName, end );

  return key;
}

/*  @} *//* OpenICC_config */
