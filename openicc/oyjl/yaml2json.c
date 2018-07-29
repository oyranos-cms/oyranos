#include <yaml.h>

#include <stdlib.h>
#include <stdio.h>

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>

#include <stdio.h>
#include <yaml.h>
#include "oyjl.h"


int getCount( yaml_node_t * n )
{
  int i = 0;
  switch( n->type )
  {
    case YAML_SCALAR_NODE: i = n->data.scalar.length; break;
    case YAML_SEQUENCE_NODE: i = n->data.sequence.items.top - n->data.sequence.items.start; break;
    case YAML_MAPPING_NODE: i = n->data.mapping.pairs.top - n->data.mapping.pairs.start; break;
    default: break;
  }
  return i;
}

int getId( yaml_node_t * n, int index, int key )
{
  int id = 0;
  if( n->type == YAML_SEQUENCE_NODE )
    id = n->data.sequence.items.start[index];

  if( n->type == YAML_MAPPING_NODE )
  {
    if(key == 1)
      id = n->data.mapping.pairs.start[index].key;
    else
      id = n->data.mapping.pairs.start[index].value;
  }

  return id;
}

int number_detection = 1;
int readNode( yaml_document_t * doc, yaml_node_t * node, int is_key, char ** json )
{
  int error = 0;
  int count = getCount( node ), i;
  if( node->type == YAML_SCALAR_NODE )
  {
    char * t = (char*)node->data.scalar.value, * tmp = NULL;
    if(t && strstr(t, ":\\ "))
      t = tmp = oyjlStringReplace( t, ":\\ ", ": ", 0, 0);
    if(t)
    {
      double d;
      int err = -1;
      if(number_detection && is_key != 1)
        err = oyjlStringToDouble( t, &d );
      if(err == 0)
        oyjlStringAdd( json, 0,0, "%s", t );
      else
        oyjlStringAdd( json, 0,0, "\"%s\"", t );
    }
    if(tmp) free(tmp);
  }
  if( node->type == YAML_SEQUENCE_NODE )
  {
    oyjlStringAdd( json, 0,0, "[");
    for(i = 0; i < count && !error; ++i)
    {
      int id = getId( node, i, 0 );
      yaml_node_t * n =
      yaml_document_get_node( doc, id );
      error = readNode(doc, n, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
    }
    oyjlStringAdd( json, 0,0, "]");
  }
  if( node->type == YAML_MAPPING_NODE )
    for(i = 0; i < count; ++i)
    {
      int key_id = getId( node, i, 1 );
      int val_id = getId( node, i, 0 );
      yaml_node_t * key =
      yaml_document_get_node( doc, key_id );
      yaml_node_t * val =
      yaml_document_get_node( doc, val_id );

      if(i == 0) oyjlStringAdd( json, 0,0, "{");

      error = readNode(doc, key, 1, json);
      if( key->type == YAML_SCALAR_NODE &&
          !error )
      {
        oyjlStringAdd( json, 0,0, ":");
      }

      error = readNode(doc, val, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
      else if( i == count - 1 ) oyjlStringAdd( json, 0,0, "}");
    }
  return error;
}

int main(int argc, char *argv[])
{
    int number;
    int start = 0;
    int error = 0;

    if(argc > 1 && strcmp(argv[1],"-n") == 0)
    {
      fprintf(stderr, "skip number detection\n");
      number_detection = 0;
      ++start;
    }

    if (argc-start < 2) {
        printf("Usage: [-n (skip number detection)] %s file1.yaml ...\n", argv[start]);
        return 0;
    }

    for (number = start + 1; number < argc && !error; number ++)
    {
        FILE *fp;
        yaml_parser_t parser;
        yaml_document_t document;
        yaml_node_t * root = NULL;
        char * json = NULL, * text = NULL;
        oyjl_val jroot;
        char error_buffer[256] = {0};
        int level = 0;

        if(strcmp(argv[number],"-") == 0)
          fp = stdin;
        else
          fp = fopen(argv[number], "rb");
        assert(fp);

        assert(yaml_parser_initialize(&parser));

        yaml_parser_set_input_file(&parser, fp);

        yaml_parser_load(&parser, &document);
        root = yaml_document_get_root_node(&document);
        error = readNode( &document, root, 1, &json );

        /* reformat */
        jroot = oyjlTreeParse( json, error_buffer, 256 );
        if(error_buffer[0] != '\000')
            fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
        oyjlTreeToJson( jroot, &level, &text );
        puts( text );

        yaml_parser_delete(&parser);
        yaml_document_delete(&document);

        if(fp != stdin)
          assert(!fclose(fp));
    }

    return error;
}
