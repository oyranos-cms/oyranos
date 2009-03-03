/* cc -g -Wall -pedantic -o oyranos_gen_api oyranos_gen_api.c */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h> /* toupper */
#include <sys/time.h>
#include <time.h> /* strftime */

#include "oyranos_version.h"

void usage (char**argv)
{
                        fprintf(stderr,"\n");
                        fprintf(stderr, "usage:\n" );
                        fprintf(stderr, "       %s class_base_name\n", argv[0] );
                        fprintf(stderr, "   or\n" );
                        fprintf(stderr, "       %s [-l|--list] class_base_name\n", argv[0] );
                        fprintf(stderr, "example:\n" );
                        fprintf(stderr, "       %s FilterLink\n", argv[0] );
 
                        fprintf(stderr,"\n");
                        fprintf(stderr,"\n");
                        exit (0);
}

int main( int argc , char** argv )
{
  int list = 0, i, len;
  char * class_base = 0,
       * class_name = 0,
       * class_name_singular = 0,
       * class_enum = 0,
       * class_enum_singular = 0,
       * tmp = 0,
       * date = 0,
       * space = 0;
  const char * start = "           ";

  if(argc > 1)
  {
    int pos = 1;
    fprintf(stderr,"argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            switch (argv[pos][1])
            {
              case 'l':
                        list = 1;
                        break;
              case 'h':
              default:
                        usage(argv);
                        break;
            }
            break;
        default:
            class_base = (char*)argv[pos];
      }
      ++pos;
    }
  } else
    usage(argv);
 
  tmp = malloc(1024);
  sprintf(tmp, "oy%s_s", class_base);
  class_name = tmp; tmp = 0;
  class_name_singular = malloc(1024);
  sprintf(class_name_singular, "oy%s", class_base);
  class_name_singular[strlen(class_name_singular)-1] = 0;
  sprintf( &class_name_singular[strlen(class_name_singular)], "_s" );

  class_enum = malloc(1024);
  sprintf( class_enum, "oyOBJECT" );
  class_enum_singular = malloc(1024);
  sprintf( class_enum_singular, "oyOBJECT" );
  i = 0;
  while (class_base[i])
  {
    if(strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", class_base[i]) )
    {
      sprintf( &class_enum[strlen(class_enum)], "_" );
      sprintf( &class_enum_singular[strlen(class_enum_singular)], "_" );
    }

    sprintf( &class_enum[strlen(class_enum)], "%c", toupper(class_base[i]) );
    sprintf( &class_enum_singular[strlen(class_enum_singular)], "%c", toupper(class_base[i]) );
    ++i;
  }
  sprintf( &class_enum[strlen(class_enum)], "_S" );
  class_enum_singular[strlen(class_enum_singular)-1] = 0;
  sprintf( &class_enum_singular[strlen(class_enum_singular)], "_S" );

  {
        time_t  cutime;         /* Time since epoch */
        struct tm       *gmt;
        char time_str[24];

        cutime = time(NULL); /* time right NOW */
        gmt = gmtime(&cutime);
        strftime(time_str, 24, "%Y/%m/%d", gmt);
    date = malloc(1024);
    sprintf(date,time_str);
    
  }

  len = strlen( class_base );
  space = malloc(128);
  space[0] = 0;
  for(i = 0; i < 13-len; ++i)
    sprintf( &space[strlen(space)], " " );

  fprintf(stderr, "-------- declarations : ------\n");
  fprintf(stdout, "  %s,   /**< %s */\n", class_enum, class_name );

  printf("\n");
  printf("/** @struct  %s\n", class_name );
  if(list)
  printf(" *  @brief   a %s list\n", class_base);
  else
  printf(" *  @brief   a %s object\n", class_base);
  printf(" *  @extends oyStruct_s\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("typedef struct {\n");
  printf("  oyOBJECT_e           type_;          /**< struct type %s */ \n", class_enum);
  printf("  oyStruct_Copy_f      copy;           /**< copy function */\n");
  printf("  oyStruct_Release_f   release;        /**< release function */\n");
  printf("  oyObject_s           oy_;            /**< base object */\n");
  printf("\n");
  if(list)
  printf("  oyStructList_s     * list_;          /**< the list data */\n");
  printf("} %s;\n", class_name);
  printf("\n");
  printf("OYAPI %s * OYEXPORT\n", class_name);
  printf("%soy%s_New       %s( oyObject_s          object );\n", start,
                                                            class_base, space);
  printf("OYAPI %s * OYEXPORT\n", class_name);
  if(list)
  printf("%soy%s_Copy      %s( oy%s_s%s * list,\n", start,
                           class_base, space, class_base, space);
  else
  printf("%soy%s_Copy      %s( oy%s_s%s * obj,\n", start,
                           class_base, space, class_base, space);
  printf("                                       oyObject_s          object);\n");
  printf("OYAPI int  OYEXPORT\n");
  if(list)
  printf("%soy%s_Release   %s( %s%s** list );\n", start,
                           class_base, space, class_name, space);
  else
  printf("%soy%s_Release   %s( %s%s** obj );\n", start,
                           class_base, space, class_name, space);
  if(list)
  {
  printf("\n");
  printf("\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_MoveIn    %s( %s%s * list,\n", start,
                           class_base, space, class_name, space);
  printf("                                       %s%s ** ptr,\n",
                           class_name_singular, space);
  printf("                                       int                 pos );\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_ReleaseAt %s( %s%s * list,\n", start,
                           class_base, space, class_name, space);
  printf("                                       int                 pos );\n");
  printf("OYAPI %s * OYEXPORT\n", class_name_singular);
  printf("%soy%s_Get       %s( %s%s * list,\n", start,
                           class_base, space, class_name, space);
  printf("                                       int                 pos );\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_Count     %s( %s%s * list );\n", start,
                           class_base, space, class_name, space);
  }
  printf("\n");
  fprintf(stderr, "-------- definitions : ------\n");

  printf("\n");
  /* --- oyClass_New --- */
  printf("/** Function oy%s_New\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  if(list)
  printf(" *  @brief   allocate a new %s list\n", class_base);
  else
  printf(" *  @brief   allocate a new %s object\n", class_base);
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI %s * OYEXPORT\n", class_name);
  printf("%soy%s_New       %s( oyObject_s          object )\n", start,
                           class_base, space);
  printf("{\n");
  printf("  /* ---- start of common object constructor ----- */\n");
  printf("  oyOBJECT_e type = %s;\n", class_enum);
  printf("# define STRUCT_TYPE %s\n", class_name);
  printf("  int error = 0;\n");
  printf("  oyObject_s    s_obj = oyObject_NewFrom( object );\n");
  printf("  STRUCT_TYPE * s = 0;\n");
  printf("\n");
  printf("  if(s_obj)\n");
  printf("    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));\n");
  printf("\n");
  printf("  if(!s || !s_obj)\n");
  printf("  {\n");
  printf("    WARNc_S(_(\"MEM Error.\"));\n");
  printf("    return NULL;\n");
  printf("  }\n");
  printf("\n");
  printf("  error = !memset( s, 0, sizeof(STRUCT_TYPE) );\n");
  printf("\n");
  printf("  s->type_ = type;\n");
  printf("  s->copy = (oyStruct_Copy_f) oy%s_Copy;\n", class_base);
  printf("  s->release = (oyStruct_Release_f) oy%s_Release;\n", class_base);
  printf("\n");
  printf("  s->oy_ = s_obj;\n");
  printf("\n");
  printf("  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );\n");
  printf("# undef STRUCT_TYPE\n");
  printf("  /* ---- end of common object constructor ------- */\n");
  printf("\n");
  if(list)
  printf("  s->list_ = oyStructList_Create( s->type_, 0, 0 );\n");
  printf("\n");
  printf("  return s;\n");
  printf("}\n");
  printf("\n");
  /* --- oyClass_Copy_ --- */
  printf("/** @internal\n" );
  printf(" *  Function oy%s_Copy_\n", class_base);
  printf(" *  @memberof %s\n", class_name );
  printf(" *  @brief   real copy a %s object\n", class_base);
  printf(" *\n");
  printf(" *  @param[in]     obj                 struct object\n");
  printf(" *  @param         object              the optional object\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("%s * oy%s_Copy_\n", class_name, class_base);
  printf("                                     ( oy%s_s%s * obj,\n",
                           class_base, space);
  printf("                                       oyObject_s          object )\n");
  printf("{\n");
  printf("  %s * s = 0;\n", class_name);
  printf("  int error = 0;\n");
  if (!list)
  printf("  oyAlloc_f allocateFunc_ = 0;\n");
  printf("\n");
  printf("  if(!obj || !object)\n");
  printf("    return s;\n");
  printf("\n");
  printf("  s = oy%s_New( object );\n", class_base);
  printf("  error = !s;\n");
  printf("\n");
  printf("  if(!error)\n");
  if (list)
  printf("    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );\n");
  else
  {
  printf("  {\n");
  printf("    allocateFunc_ = s->oy_->allocateFunc_;\n");
  printf("  }\n");
  }
  printf("\n");
  printf("  if(error)\n");
  printf("    oy%s_Release( &s );\n", class_base);
  printf("\n");
  printf("  return s;\n");
  printf("}\n");
  printf("\n");
  /* --- oyClass_Copy --- */
  printf("/** Function oy%s_Copy\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  if(list)
  printf(" *  @brief   copy or reference a %s list\n", class_base);
  else
  printf(" *  @brief   copy or reference a %s object\n", class_base);
  printf(" *\n");
  printf(" *  @param[in]     obj                 struct object\n");
  printf(" *  @param         object              the optional object\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI %s * OYEXPORT\n", class_name);
  printf("%soy%s_Copy      %s( %s%s * obj,\n", start,
                           class_base, space, class_name, space);
  printf("                                       oyObject_s          object )\n");
  printf("{\n");
  printf("  %s * s = obj;\n", class_name);
  printf("\n");
  printf("  if(!obj)\n");
  printf("    return 0;\n");
  printf("\n");
  printf("  oyCheckType__m( %s, return 0 )\n", class_enum);
  printf("\n");
  printf("  if(obj && !object)\n");
  printf("  {\n");
  printf("    s = obj;\n");
  printf("    oyObject_Copy( s->oy_ );\n");
  printf("    return s;\n");
  printf("  }\n");
  printf("\n");
  printf("  s = oy%s_Copy_( obj, object );\n", class_base);
  printf("\n");
  printf("  return s;\n");
  printf("}\n");
  printf(" \n");
  /* --- oyClass_Release --- */
  printf("/** Function oy%s_Release\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  if(list)
  printf(" *  @brief   release and possibly deallocate a %s list\n", class_base);
  else
  printf(" *  @brief   release and possibly deallocate a %s object\n", class_base);
  printf(" *\n");
  printf(" *  @param[in,out] obj                 struct object\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_Release   %s( %s%s** obj )\n", start,
                           class_base, space, class_name, space);
  printf("{\n");
  printf("  /* ---- start of common object destructor ----- */\n");
  printf("  %s * s = 0;\n", class_name);
  printf("\n");
  printf("  if(!obj || !*obj)\n");
  printf("    return 0;\n");
  printf("\n");
  printf("  s = *obj;\n");
  printf("\n");
  printf("  oyCheckType__m( %s, return 1 )\n", class_enum);
  printf("\n");
  printf("  *obj = 0;\n");
  printf("\n");
  printf("  if(oyObject_UnRef(s->oy_))\n");
  printf("    return 0;\n");
  printf("  /* ---- end of common object destructor ------- */\n");
  printf("\n");
  if(list)
  printf("  oyStructList_Release( &s->list_ );\n");
  printf("\n");
  printf("  if(s->oy_->deallocateFunc_)\n");
  printf("  {\n");
  printf("    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;\n");
  printf("\n");
  printf("    oyObject_Release( &s->oy_ );\n");
  printf("\n");
  printf("    deallocateFunc( s );\n");
  printf("  }\n");
  printf("\n");
  printf("  return 0;\n");
  printf("}\n");
  printf("\n");
  printf("\n");

  if(list)
  {
   /* --- oyClass_MoveIn --- */
  printf("/** Function oy%s_MoveIn\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  printf(" *  @brief   add a element to a %s list\n", class_base);
  printf(" *\n");
  printf(" *  @param[in]     list                list\n");
  printf(" *  @param[in,out] obj                 list element\n");
  printf(" *  @param         pos                 position\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_MoveIn    %s( %s%s * list,\n", start,
                           class_base, space,class_name, space);
  printf("                                       %s%s ** obj,\n",
                           class_name_singular, space);
  printf("                                       int                 pos )\n");
  printf("{\n");
  printf("  %s * s = list;\n", class_name);
  printf("  int error = 0;\n");
  printf("\n");
  printf("  if(s)\n");
  printf("    oyCheckType__m( %s, return 1 )\n", class_enum);
  printf("\n");
  printf("  if(obj && *obj && (*obj)->type_ == %s)\n", class_enum_singular);
  printf("  {\n");
  printf("    if(!s)\n");
  printf("    {\n");
  printf("      s = oy%s_New(0);\n", class_base);
  printf("      error = !s;\n");
  printf("    }                                  \n");
  printf("\n");
  printf("    if(!error && !s->list_)\n");
  printf("    {\n");
  printf("      s->list_ = oyStructList_Create( s->type_, 0, 0 );\n");
  printf("      error = !s->list_;\n");
  printf("    }\n");
  printf("      \n");
  printf("    if(!error)\n");
  printf("      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );\n");
  printf("  }   \n");
  printf("  \n");
  printf("  return error;\n");
  printf("}\n");
  printf("\n");
   /* --- oyClass_ReleaseAt --- */
  printf("/** Function oy%s_ReleaseAt\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  printf(" *  @brief   release a element from a %s list\n", class_base);
  printf(" *\n");
  printf(" *  @param[in,out] list                the list\n");
  printf(" *  @param         pos                 position\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_ReleaseAt %s( %s%s * list,\n", start,
                           class_base, space, class_name, space);
  printf("                                       int                 pos )\n");
  printf("{ \n");
  printf("  int error = !list;\n");
  printf("  %s * s = list;\n", class_name);
  printf("\n");
  printf("  if(!error)\n");
  printf("    oyCheckType__m( %s, return 1 )\n", class_enum);
  printf("\n");
  printf("  if(!error && list->type_ != %s)\n", class_enum);
  printf("    error = 1;\n");
  printf("  \n");
  printf("  if(!error)\n");
  printf("    oyStructList_ReleaseAt( list->list_, pos );\n");
  printf("\n");
  printf("  return error;\n");
  printf("}\n");
  printf("\n");
   /* --- oyClass_Get --- */
  printf("/** Function oy%s_Get\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  printf(" *  @brief   get a element of a %s list\n", class_base);
  printf(" *\n");
  printf(" *  @param[in,out] list                the list\n");
  printf(" *  @param         pos                 position\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI %s * OYEXPORT\n", class_name_singular);
  printf("%soy%s_Get       %s( %s%s * list,\n", start,
                           class_base, space, class_name, space);
  printf("                                       int                 pos )\n");
  printf("{       \n");
  printf("  int error = !list;\n");
  printf("  %s * s = list;\n", class_name);
  printf("\n");
  printf("  if(!error)\n");
  printf("    oyCheckType__m( %s, return 0 )\n", class_enum);
  printf("\n");
  printf("  if(!error)\n");
  printf("    return (%s *) oyStructList_GetRefType( list->list_, pos, %s ); \n", class_name_singular, class_enum_singular);
  printf("  else  \n");
  printf("    return 0;\n");
  printf("}   \n");
  printf("\n");
  /* --- oyClass_Count --- */
  printf("/** Function oy%s_Count\n", class_base );
  printf(" *  @memberof %s\n", class_name );
  printf(" *  @brief   count the elements in a %s list\n", class_base);
  printf(" *\n");
  printf(" *  @param[in,out] list                the list\n");
  printf(" *  @return                            element count\n");
  printf(" *\n");
  printf(" *  @version Oyranos: %s\n", OYRANOS_VERSION_NAME );
  printf(" *  @since   %s (Oyranos: %s)\n", date, OYRANOS_VERSION_NAME);
  printf(" *  @date    %s\n", date);
  printf(" */\n");
  printf("OYAPI int  OYEXPORT\n");
  printf("%soy%s_Count     %s( %s%s * list )\n", start,
                           class_base, space, class_name, space);
  printf("{       \n");
  printf("  int error = !list;\n");
  printf("  %s * s = list;\n", class_name);
  printf("\n");
  printf("  if(!error)\n");
  printf("    oyCheckType__m( %s, return 0 )\n", class_enum);
  printf("\n");
  printf("  if(!error)\n");
  printf("    return oyStructList_Count( list->list_ );\n");
  printf("  else return 0;\n");
  printf("}\n");
  printf("\n");
  fprintf(stderr, "-------- definitions end ------\n");

  }

  return 0;
}
