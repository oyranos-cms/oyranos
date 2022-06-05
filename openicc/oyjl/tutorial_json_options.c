#include "oyOptions_s.h"
#include <stdio.h>
void testOyjl( void );
int main (int argc OY_UNUSED, char ** argv OY_UNUSED)
{
  testOyjl( );

  return 0;
}

#include "oyranos_json.h"
void testOyjl(void)
{
  /* JSON string */
  const char * text = "{\"org\":{\"test\":[{\"s1key_a\":\"val_a\",\"s1key_b\":\"val_b\"},{\"s2key_c\":\"val_c\",\"s2key_d\":\"val_d\"}],\"key_e\":\"val_e_yyy\",\"key_f\":\"val_f\"}}";

  oyjl_val node = 0;
  char * json = 0;

  /* read JSON into C data struct */
  oyjl_val root = oyjlTreeParse2( text, 0, __func__, NULL );
  
  /* convert back to JSON */
  json = oyjlTreeToText( root, OYJL_JSON );

    fprintf( stderr, "root = oyjlTreeParse2( text ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* use a xpath to obtain a node */
  node = oyjlTreeGetValueF( root, 0, "org/test/[%d]", 1 );

    json = oyjlTreeToText( node, OYJL_JSON );
    fprintf( stderr, "node = oyjlTreeGetValueF( root, \"org/test/[%%d]\", 1 ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* use a xpath to remove a node */
  oyjlTreeClearValue( root, "org/test/[1]/" );

    json = oyjlTreeToText( node, OYJL_JSON );
    fprintf( stderr, "oyjlTreeClearValue( root, \"org/test/[1]/\" ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* use a xpath to get a new node in a existing tree */
  node = oyjlTreeGetValue( root, OYJL_CREATE_NEW, "org/add/opt" );

    json = oyjlTreeToText( root, OYJL_JSON );
    fprintf( stderr, "node = oyjlTreeGetValue( root, OYJL_CREATE_NEW, \"org/add/opt\" ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* set the new node to some string value  */
  oyjlValueSetString( node, "opt_value" );

    json = oyjlTreeToText( root, OYJL_JSON );
    fprintf( stderr, "oyjlValueSetString( node, \"opt_value\" ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* release memory */
  oyjlTreeFree ( root );

  /* use a xpath to create new tree */
  root = oyjlTreeNew( "new/tree/key" );

    json = oyjlTreeToText( root, OYJL_JSON );
    fprintf( stderr, "oyjlTreeNew( \"new/tree/key\" ):\n%s\n", json?json:"" );
    free(json); json = NULL;

  /* release memory */
  oyjlTreeFree( root );
}
