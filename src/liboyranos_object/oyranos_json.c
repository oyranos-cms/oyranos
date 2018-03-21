/** @file oyranos_json.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2016-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    oyjl dependent functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/11/17
 */

#include "oyranos.h"
#include "oyranos_core.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_json.h"
#include "oyranos_config_internal.h"

#include "../../oyjl/oyjl_tree.c"

int          oyJson                  ( const char        * json )
{
  int c,i = 0;
  /* first simple check */
  do {
    c = json[i++];
  } while( c == ' ' || c == '\t' || c == '\n' || c == '\r' );
  if(c == '[' || c == '{')
    return oyNAME_JSON;
  else
    return 0;
}

oyjl_val     oyJsonParse             ( const char        * json )
{
  int error = !json;
  oyjl_val root;
  char error_buffer[256] = {0};

  if(error)
    return NULL;

  root = oyjl_tree_parse( json, error_buffer, 256 );
  if(error_buffer[0] != '\000')
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR:\t\"%s\"\n", OY_DBG_ARGS_, error_buffer );

  return root;
}

char **      oyJsonPathsFromPattern  ( oyjl_val            root,
                                       const char        * key )
{
  char ** list = NULL;
  int list_n = 0;

  char ** paths = NULL;
  int paths_n = 0, i;

  oyjl_tree_to_paths( root, 1000000, NULL, 0, &paths );
  while(paths && paths[paths_n]) ++paths_n;

  if(paths)
  {
    for(i = 0; i < paths_n; ++i)
    {
      const char * path = paths[i];
      if(oyjl_path_match(path, key, OYJL_PATH_MATCH_LAST_ITEMS))
        oyStringListAddStaticString( &list, &list_n, path, oyAllocateFunc_, oyDeAllocateFunc_ );
    }
    oyjl_string_list_release( &paths, paths_n, free );

  }

  return list;
}

char *       oyJsonFromModelAndUi    ( const char        * data,
                                       const char        * ui_text,
                                       oyAlloc_f           allocate_func )
{
  char * text = NULL;
  oyjl_val droot = oyJsonParse( data ),
           uiroot = oyJsonParse( ui_text );

  int paths_n = 0, i;
  char ** paths = oyJsonPathsFromPattern( uiroot, "groups//options//nick");
  while(paths && paths[paths_n]) ++paths_n;

  if(!droot)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR: data\n%s\n", OY_DBG_ARGS_, data );
  if(!uiroot)
    oyMessageFunc_p( oyMSG_WARN, NULL, OY_DBG_FORMAT_ "ERROR: ui_text\n%s\n", OY_DBG_ARGS_, ui_text );


  for(i = 0; i < paths_n; ++i)
  {
    char * path = paths[i];
    /* get the key node */
    oyjl_val uiv = oyjl_tree_get_value( uiroot, 0, path ),
             dv = NULL, v = NULL;
    /* get the key name */
    char * key = oyjl_value_text( uiv, oyAllocateFunc_ ),
         * value = NULL;
    /* get the value node by the key */
    if(key)
      dv = oyjl_tree_get_value( droot, 0, key );
    /* get the value */
    if(dv)
      value = oyjl_value_text( dv, oyAllocateFunc_ );
    /* write the value into the options "default" key */
    if(value && key && strchr(key,'/'))
    {
      char * t = strrchr(path,'/');
      t[0] = 0;
      v = oyjl_tree_get_valuef( uiroot, OYJL_CREATE_NEW, "%s/default", path );
      oyjl_value_set_string( v, value );
    }
    if(key) oyFree_m_( key );
    if(value) oyFree_m_( value );
  }

  i = 0;
  oyjl_tree_to_json( uiroot, &i, &text );

  oyjl_tree_free( uiroot );
  oyjl_tree_free( droot );
  oyjl_string_list_release( &paths, paths_n, free );

  if(allocate_func && allocate_func != oyAllocateFunc_)
  {
    char * t = oyStringCopy( text, allocate_func );
    oyDeAllocateFunc_(text);
    text = t; t = 0;
  }

  return text;
}

/* just for debuging */
char *       oyJsonPrint             ( oyjl_val            root )
{
  char * json = NULL;
  int level = 0;
  oyjl_tree_to_json( root, &level, &json );
  return json;
}
