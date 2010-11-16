/** @file oyranos_cmms.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    old CMM's framework
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/30
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_cmms.h"
#include "oyranos_elektra.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- structs, typedefs, enums --- */

/** @internal
    @brief the internal only used structure for external registred CMM functions
 */
typedef struct {
  oyOBJECT_e           type;           /*!< internal struct type oyOBJECT_EXTERNFUNC_S */
  char        * id;               /**< 4 letter short name */
  char        * name;             /**< short name */
  char        * description;      /**< long description */ /* TODO help license .. */
  char        * func_name;        /**< function for dlsym */
  oyWIDGET_e    opts_start;       /**< options numbers for oyGetOptionUITitle */
  oyWIDGET_e    opts_end;
  oyOption_t_ * option;         /**< the CMM options */
} oyExternFunc_s;


/** @internal
    @brief the internal only used structure for external registred CMM's
 */
typedef struct {
  oyOBJECT_e           type;           /**< internal struct type oyOBJECT_MODULE_S */
  char        cmm[5];           /**< 4 letter identifier */
  char       *name;             /**< short name */
  char       *description;      /**< long description */ /* TODO help license .. */
  int         groups_start;
  int         groups_end;       /**< the registred layouts frames */
  oyOption_t_*group;            /**< the oy_groups_description_ synonym */
  char               * lib_name;       /**< library to search for function */
  oyExternFunc_s *func;         /**< the registred functions of the CMM */
  int         funcs;            /**< number of provided functions */
  char       *xml;              /**< original xml text */
#if 0
  const char *domain;           /**< textdomain */
  const char *domain_path;      /**< textdomain path */
#endif
} oyModule_s;

/** @internal singleton */
struct {
  int          looked;
  oyModule_s *modul;
  int          n;
} oyModules_ = {0,NULL,0};

#if 0
static int  oy_data_globale_id_;
/** @internal a object containing data, file information and inform observers */
typedef struct {
    /* Zeigermaschine */
    unsigned char** zeiger_;
    size_t*     groesse_;
    int*        ref_n_;
    int         id_;
    char        *name_;                   /* z.B. Profilname */
    double      *letze_aen_zeit_;         /* letztes mal geaendert */
    double      *letze_ben_zeit_;         /* letztes mal benutzt */
} oyData_t_;

void
oyDataInit_ (oyData_t_ *data)
{
    data->id_ = oy_data_globale_id_;
    ++oy_data_globale_id_;
    data->ref_n_=           (int*)         calloc(sizeof( int ),1);
    data->zeiger_=          (unsigned char**)calloc(sizeof( unsigned char* ),1);
    data->groesse_=         (size_t*)      calloc(sizeof( size_t ),1);
    data->name_ =           (char*)        calloc(sizeof( char* ),1);
    data->letze_aen_zeit_ = (double*)      calloc(sizeof( double ),1);
    data->letze_ben_zeit_ = (double*)      calloc(sizeof( double ),1);
    *data->ref_n_ = 1; *data->zeiger_ = 0; *data->groesse_ = 0;
    *data->letze_aen_zeit_ = *data->letze_ben_zeit_ = 0;
}
#endif

/* --- static variables   --- */

/* --- internal API definition --- */

/** @internal CMM API */
oyModule_s*  oyModulGet_             ( const char        * id);
int          oyModulAdd_             ( oyModule_s        * cmm);
int          oyModulGetFromXML_      ( oyGROUP_e           group,
                                       char              * xml,
                                       oyModule_s        * cmm);
oyExternFunc_s *   oyModulesGetFunc_ ( const char        * cmm,
                                       const char        * id );
oyWIDGET_e   oyModulsGetNewOptionRange_( int               count);



/* small helpers */
#define OY_FREE( ptr ) if(ptr) { oyDeAllocateFunc_(ptr); ptr = 0; }

/* --- function definitions --- */


/* CMM support */


oyModule_s*
oyModulGet_        (const char *id)
{
  int i;
  if(oyModules_.modul && oyModules_.n)
    for(i = 0; i < oyModules_.n; ++i)
    {
      if(memcmp( id, oyModules_.modul[i].cmm, 4 ) == 0)
      {
        return &oyModules_.modul[i];
      }
    }
  return NULL;
}

oyExternFunc_s * oyModulesGetFunc_   ( const char        * cmm,
                                       const char        * id )
{
  oyModule_s *m = 0;
  oyExternFunc_s *result = NULL;
  int i;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
  {
    for(i = 0; i < m->funcs; ++i)
    {
      int func_id = *((int*)id);
      int m_func_id = *((int*)m->func[i].id);

      if(func_id == m_func_id)
      {
        result = &m->func[i];
        break;
      }
    }
  }

  return result;
}

int
oyModulRemove_     (const char *id)
{
  int i,
      error = 0,
      pos = 0;
  oyModule_s *ptr = NULL;

  oyModules_.looked = 1;

  oyAllocHelper_m_(ptr, oyModule_s, oyModules_.n-1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].cmm, id) == 0)
      ++i;
    else
      ptr[pos] = oyModules_.modul[i];
    ++pos;
  }

  if(oyModules_.modul) oyDeAllocateFunc_(oyModules_.modul);
  oyModules_.modul = ptr;
  --oyModules_.n;
  oyModules_.looked = 0;

  return error;
}

int
oyModulAdd_        (oyModule_s *cmm)
{
  int i,
      error = 0;
  oyModule_s *ptr = NULL;

  oyModules_.looked = 1;

  oyAllocHelper_m_(ptr, oyModule_s, oyModules_.n+1, oyAllocateFunc_, return 1)
  for(i = 0; i < oyModules_.n; ++i)
    ptr[i] = oyModules_.modul[i];
  ptr[oyModules_.n] = *cmm;
  if(oyModules_.modul) oyDeAllocateFunc_(oyModules_.modul);
  oyModules_.modul = ptr;
  ++oyModules_.n;
  oyModules_.looked = 0;

  return error;
}


/** @internal
 *  @brief get
 *
 *  @since Oyranos: version 0.1.x
 *  @date  11 december 2007 (API 0.1.x)
 */
char**
oyModulsGetNames_( int        *count,
                   oyAlloc_f     alloc_func )
{
  char **ids = NULL;
  int    i;

  *count = 0;
  oyAllocHelper_m_( ids, char*, oyModules_.n, alloc_func, return NULL)

  for( i = 0; i < oyModules_.n; ++i)
  {
    oyAllocHelper_m_( ids[i], char, 5, alloc_func, return NULL);

    oySnprintf1_( ids[i], 5, "%s", oyModules_.modul[i].cmm );
  }
  *count = oyModules_.n;
  return ids;
}

oyGROUP_e
oyRegisterGroups_(char *cmm, char *id, char *name, char *tooltip)
{
  return oyGroupAdd_(cmm, id, name, tooltip);
}


int
oyModulGetFromXML_( oyGROUP_e           group,
                    char             *xml,
                    oyModule_s      *cmm)
{

  /* allocate memory */
  char *value = 0,
       *cmm_reg = 0,
       *cmm_group = 0,
       *groups = 0,
      **groupa,
       *funcs,
      **func,
       *options,
      **option,
       *choices,
      **choice;
  int   count = 0, count2, count3;
  int   i, j, k;
  int   err = 0;
  int base_complete = 1;
  int first_group_n = 0;
  int group_modules = 0;

  DBG_PROG_START


#if 0
  value = oyXMLgetValue_(xml, "oyDOMAIN");
  if(strlen(domain) &&
     value && strlen(value))
    cmm->domain = value;
  else
    cmm->domain = domain;
  domain = cmm->domain;
  value = oyXMLgetValue_(xml, "oyDOMAIN_PATH");
  if(value && strlen(value))
    cmm->domain_path = value;
  else
    cmm->domain_path = OY_LOCALEDIR;
  domain_path = cmm->domain_path;
#endif

  oyI18NInit_();


  cmm_reg = oyXMLgetValue_(xml, "oyCMM_REGISTER");
  cmm->xml = xml;
  cmm_group= oyXMLgetValue_(cmm_reg, "oyCMM_GROUP");
  value = oyXMLgetValue_(cmm_group, "oyID");
  if(value && strlen(value) == 4)
    oySnprintf1_( cmm->cmm, 5, "%s", value );
  else
    base_complete = 0;
  value = oyXMLgetValue_(cmm_group, "oyNAME");
  if(value && strlen(value))
    cmm->name = value;
  else
    base_complete = 0;
  value = _( oyXMLgetValue_(cmm_group, "oyDESCRIPTION") );
  if(value && strlen(value))
    cmm->description = value;
  groups = oyXMLgetValue_(cmm_group, "oyGROUPS");
  groupa = oyXMLgetArray_(groups, "oyGROUP", &count);

  group_modules = count;
  cmm->group = NULL;
  oyAllocHelper_m_( cmm->group, oyOption_t_, group_modules,
                    oyAllocateFunc_, return -1; );
  for(i = 0; i < group_modules; ++i)
  {
    oyGROUP_e oy_group;
    char *txt0, *txt1, *txt2;
    oyOption_t_ *opt = cmm->group;
    int pos = 0;

    txt0 =    oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML");
    txt1 = _( oyXMLgetValue_(groupa[i], "oyNAME") );
    txt2 = _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION") );

#   define oySET_OPTIONS_M_( t_, id_, ca_n, ca1, ca2, ca3, labl, desc, \
                             ch_n, ch0, ch1, ch2, ch3, \
                             conf, xml) { \
      pos = id_; \
      opt[pos]. type = t_; \
      opt[pos]. id = id_; \
      opt[pos]. category[0] = ca_n; \
      opt[pos]. category[1] = ca1; \
      opt[pos]. category[2] = ca2; \
      opt[pos]. category[3] = ca3; \
      opt[pos]. flags = 0; \
      opt[pos]. name = labl; \
      opt[pos]. description = desc; \
      opt[pos]. choices = ch_n; \
      opt[pos]. choice_list[0] = ch0; \
      opt[pos]. choice_list[1] = ch1; \
      opt[pos]. choice_list[2] = ch2; \
      opt[pos]. choice_list[3] = ch3; \
/*      opt[pos]. choice_desc[0] = cd0; \
      opt[pos]. choice_desc[1] = cd1; \
      opt[pos]. choice_desc[2] = cd2; \
      opt[pos]. choice_desc[3] = cd3;*/ \
      opt[pos]. config_string = conf; \
      opt[pos]. config_string_xml = xml; \
    }

    oySET_OPTIONS_M_( oyWIDGETTYPE_GROUP_TREE, i, 0,
      0, 0, 0,
      txt1,
      txt2,
      0, /* choices */
      NULL, NULL, NULL, NULL,
      NULL,
      txt0)

    oy_group = oyRegisterGroups_( cmm->cmm, txt0, txt1, txt2 );

    if(i == 0)
      first_group_n = oy_group;
    
    /*DBG_S(("oyGROUP[%d]: %s", i, groupa[i])); */
    DBG2_S("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyCONFIG_STRING_XML"))
    DBG2_S("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyNAME")) )
    DBG2_S("   [%d]: %s", i, _( oyXMLgetValue_(groupa[i], "oyDESCRIPTION")) )
    DBG2_S("   [%d]: %s", i, oyXMLgetValue_(groupa[i], "oyNIX"))
  }
  cmm->groups_start = first_group_n;
  cmm->groups_end   = cmm->groups_start + group_modules - 1;

  funcs =   oyXMLgetValue_(cmm_reg, "oyFUNCTIONS");
  value = oyXMLgetValue_(funcs, "oyDYNLOAD_LIB");
  if(value)
    cmm->lib_name = value;
  func =    oyXMLgetArray_(funcs, "oyFUNCTION", &count);
  cmm->func = NULL;
  oyAllocHelper_m_(cmm->func, oyExternFunc_s, count,
                               oyAllocateFunc_, return -1; );
  cmm->funcs = count;
  for(i = 0; i < count; ++i)
  {
    cmm->func[i].type        =   oyOBJECT_EXTERNFUNC_S;
    cmm->func[i].id          =   oyXMLgetValue_(func[i], "oyID");
    cmm->func[i].name        = _(oyXMLgetValue_(func[i], "oyNAME"));
    cmm->func[i].description = _(oyXMLgetValue_(func[i], "oyDESCRIPTION"));
    cmm->func[i].func_name   =   oyXMLgetValue_(func[i], "oyDYNLOAD_FUNC");
    DBG1_S("     : %s", cmm->func[i].id)
    DBG1_S("     : %s", cmm->func[i].name)
    DBG1_S("     : %s", cmm->func[i].description)
    DBG1_S("     : %s", cmm->func[i].func_name)
    options = oyXMLgetValue_(func[i], "oyWIDGETS");
    option =  oyXMLgetArray_(options, "oyWIDGET", &count2);
    cmm->func[i].option = NULL;
    if(count2)
      oyAllocHelper_m_(cmm->func[i].option, oyOption_t_, count2,
                                  oyAllocateFunc_, return -1; );
    for(j = 0; j < count2; ++j)
    {
      int group_n = 0;
      char **grs = NULL;
      char *type = NULL;
      /*DBG_S(("oyWIDGET_e[%d]: %s", i, option[i])); */
      DBG1_S("       : %s", oyXMLgetValue_(option[j], "oyID"))

      grs = oyXMLgetArray_(option[j], "oyGROUP", &group_n);
      if(group_n && grs)
      for( k = 0; k < OY_MIN(group_n,group_modules); ++k)
      {
        int cat_intern = atoi(grs[k]) - oyGROUP_EXTERN;

        cmm->func[i].option[j].category[k] = atoi(grs[k]);
        if(cat_intern >= 0)
          DBG1_S("       => %s",
                 _( oyXMLgetValue_(groupa[ cat_intern ], "oyNAME") ));
      }
      if(group_n < 10)
        cmm->func[i].option[j].category[ group_n ] = oyGROUP_START;

      cmm->func[i].option[j].name = _( oyXMLgetValue_(option[j], "oyNAME") );
      cmm->func[i].option[j].description = _( oyXMLgetValue_(option[j], "oyDESCRIPTION") );
      DBG1_S("       : %s", _( oyXMLgetValue_(option[j], "oyNAME") ))
      DBG1_S("       : %s", _( oyXMLgetValue_(option[j], "oyDESCRIPTION") ))

      type = oyXMLgetValue_(option[j], "oyWIDGET_TYPE");
      if(type)
      {
        if      (strcmp(type,"oyWIDGETTYPE_BEHAVIOUR") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_BEHAVIOUR;
        else if (strcmp(type,"oyWIDGETTYPE_DEFAULT_PROFILE") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_DEFAULT_PROFILE;
        else if (strcmp(type,"oyWIDGETWIDGETTYPE_PROFILE") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_PROFILE;
        else if (strcmp(type,"oyWIDGETTYPE_INT") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_INT;
        else if (strcmp(type,"oyWIDGETTYPE_FLOAT") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_FLOAT;
        else if (strcmp(type,"oyWIDGETTYPE_CHOICE") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_CHOICE;
        else if (strcmp(type,"oyWIDGETTYPE_VOID") == 0)
          cmm->func[i].option[j].type = oyWIDGETTYPE_VOID;
        else
          WARNc2_S("%s %s", _("Did not find a type for option:"),
                   cmm->func[i].option[j].name);
      }

      switch (cmm->func[i].option[j].type)
      {
        case oyWIDGETTYPE_START:
        case oyWIDGETTYPE_GROUP_TREE:
        case oyWIDGETTYPE_BEHAVIOUR:
        case oyWIDGETTYPE_DEFAULT_PROFILE:
        case oyWIDGETTYPE_PROFILE:
        case oyWIDGETTYPE_INT:
        case oyWIDGETTYPE_FLOAT:
        case oyWIDGETTYPE_LIST:
             break;
        case oyWIDGETTYPE_CHOICE:
             choices = oyXMLgetValue_(option[j], "oyCHOICES");
             choice = oyXMLgetArray_(choices, "oyNAME", &count3);
             cmm->func[i].option[j].choices = count3 <= 10 ? count3 : 10;
             for(k = 0; k < cmm->func[i].option[j].choices; ++k)
             {
               cmm->func[i].option[j].choice_list[k] = _( choice[k] );
               DBG1_S("         : %s", _(choice[k]))
             }
             break;
        case oyWIDGETTYPE_VOID:
        case oyWIDGETTYPE_END:
             break;
      }
      cmm->func[i].option[j].config_string = oyXMLgetValue_(option[j], "oyCONFIG_STRING");
      cmm->func[i].option[j].config_string_xml = oyXMLgetValue_(option[j], "oyCONFIG_STRING_XML");
      DBG1_S("       : %s", oyXMLgetValue_(option[j], "oyCONFIG_STRING"))
      DBG1_S("       : %s", oyXMLgetValue_(option[j], "oyCONFIG_STRING_XML"))
    }
    cmm->func[i].opts_start  =   oyModulsGetNewOptionRange_(count2);
    cmm->func[i].opts_end    =   cmm->func[i].opts_start + count2 - 1;
  }

  oy_domain = OY_TEXTDOMAIN;
  oy_domain_path = OY_LOCALEDIR;
  oyI18NInit_();

  DBG_PROG_ENDE
  return err;
}

int
oyModulRegisterXML_(oyGROUP_e           group,
                    const char       *xml)
{

  int   err = 0;
  oyModule_s cmm;
  char *xml_ = NULL;

  DBG_PROG_START

  oyAllocHelper_m_(xml_, char, strlen(xml)+1, oyAllocateFunc_, return -1; )
  sprintf(xml_,"%s",xml);

  oyModulGetFromXML_(group, xml_, &cmm);
  oyModulAdd_(&cmm);

  DBG_S((oyModulPrint_(cmm.cmm)));

  DBG_PROG_ENDE
  return err;
}

int          oyModulesScan_          ( int                 flags )
{
  int n = 0, i;
  char ** list = 0;

  list = oyDataFilesGet_( &n, "color/cmms", oyALL, oyUSER_SYS,
                            "cmms", "oy_cmm_register", ".xml", malloc );
  for(i = 0; i < n; ++i)
  {
    size_t size = 0;
    char* xml = oyReadFileToMem_ ( list[i], &size,
                                   oyAllocateFunc_ );

    DBG2_S("Pfad[%d]: %s", i, list[i])

    if(xml)
    {
      oyModulRegisterXML( oyGROUP_START, xml );

      oyDeAllocateFunc_(xml);
      xml = 0;
    }
  }

  oyStringListRelease_( &list, n, free );

  return n;
}


/** @internal @brief print all information out */
char*
oyModulPrint_   ( const char       *modul )
{
  char *text = NULL,
       *tmp  = NULL;
  int   len  = 1024, i, j, k;

  oyModule_s *mod = oyModulGet_(modul);

  DBG_PROG_START

# define add_s() STRING_ADD( text, tmp );
# define set_m(text_) text_ ? text_ : "(null)"

  if(!mod)
  {
    STRING_ADD( text, "Could not find " );
    STRING_ADD( text, set_m(modul) );
    return text;
  }

  oyAllocHelper_m_( tmp, char, len, oyAllocateFunc_, return NULL );
  oySnprintf1_( tmp, 1024, "Modul: %s\n", set_m(modul) ); add_s()
  oySnprintf1_( tmp, 1024, " Name: %s\n", set_m(mod->name) ); add_s()
  oySnprintf2_( tmp, 1024, "       %d - %d\n", mod->groups_start, mod->groups_end ); add_s()
  oySnprintf1_( tmp,   80, "  xml: %s", set_m(mod->xml) ); add_s()
  oySnprintf_( tmp, 1024, "\n" ); add_s()
  /*oySnprintf_( tmp, 1024, "  domain: %s path: %s\n", mod->domain, mod->domain_path ); add_s()*/

  oySnprintf_( tmp, 1024, "  \n" ); add_s()
  oySnprintf_( tmp, 1024, "  \n" ); add_s()


  for(i = 0; i < mod->funcs; ++i)
  {
    int options_n = 0;
    if(mod->func[i].opts_end != oyWIDGET_GROUP_START)
      options_n = mod->func[i].opts_end - mod->func[i].opts_start + 1;

    oySnprintf4_( tmp, 1024, "    F %d[%d] %s (%s)\n", i, mod->funcs,
            set_m(mod->func[i].name), set_m(mod->func[i].description) ); add_s()
    oySnprintf3_( tmp, 1024, "             %s::%s %s\n", set_m(mod->lib_name),
              set_m(mod->func[i].func_name), set_m(mod->func[i].id) ); add_s()
    for(j = 0; j < options_n; ++j)
    {
      oySnprintf2_( tmp, 1024, "             O %s (%s)\n",
                set_m(mod->func[i].option[j].name),
                set_m(mod->func[i].option[j].description) ); add_s()
     
      oySnprintf_( tmp, 1024, "             G" ); add_s()
      for( k = 0; k < 10; ++k )
      { oyGROUP_e g = mod->func[i].option[j].category[k];
        if(g)
        {
          int module_group = g - oyGROUP_EXTERN;
          if(module_group >= 0)
          {
            oySnprintf3_( tmp, 1024, " %d %s (%s)", g,
                      set_m(mod->group[module_group].name),
                      set_m(mod->group[module_group].description) ); add_s()
          }
        }
      }
      oySnprintf_( tmp, 1024, "\n" ); add_s()

      switch (mod->func[i].option[j].type)
      {
        case oyWIDGETTYPE_START:
        case oyWIDGETTYPE_GROUP_TREE:
        case oyWIDGETTYPE_BEHAVIOUR:
        case oyWIDGETTYPE_DEFAULT_PROFILE:
        case oyWIDGETTYPE_PROFILE:
        case oyWIDGETTYPE_INT:
        case oyWIDGETTYPE_FLOAT:
        case oyWIDGETTYPE_LIST:
             break;
        case oyWIDGETTYPE_CHOICE:
             for(k = 0; k < mod->func[i].option[j].choices; ++k)
             {
               oySnprintf3_( tmp, 1024, "               C %d[%d] %s\n", k,
                         mod->func[i].option[j].choices,
                         set_m(mod->func[i].option[j].choice_list[k]) ); add_s()
             }
             break;
        case oyWIDGETTYPE_VOID:
        case oyWIDGETTYPE_END:
             break;
      }
    }
  }

  oyDeAllocateFunc_(tmp);

  DBG_PROG_ENDE
  return text;
}

/** @internal
 *  ask for free oyWIDGET ID's to register the new ones.
 */
oyWIDGET_e
oyModulsGetNewOptionRange_(int count)
{
  int i, j;
  int last = OY_STATIC_OPTS_;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    for ( j = 0; j < oyModules_.modul[i].funcs; ++j)
    {
      int options_n = oyModules_.modul[i].func[j].opts_end -
                      oyModules_.modul[i].func[j].opts_start + 1;
      if( oyModules_.modul[i].func[j].opts_start > (last + count + 1) )
      {
        DBG_PROG_ENDE
        /* just the first occurence */
        return last + 1;
      }
      last = oyModules_.modul[i].func[j].opts_start + options_n;
    }
  }

  DBG_PROG_ENDE
  return last + 1;
}

/** @internal
 *  map a oyWIDGET to a oyOption_t_ in dynamic oyModules_
 */
oyOption_t_*
oyModulsUIOptionSearch_ (oyWIDGET_e       id)
{
  int i, j;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    for ( j = 0; j < oyModules_.modul[i].funcs; ++j)
    {
      if( oyModules_.modul[i].func[j].opts_start <= id &&
          id <= oyModules_.modul[i].func[j].opts_end )
      {
        DBG_PROG_ENDE
        /* just the first occurence */
        return &oyModules_.modul[i].func[j].option[
                  id - oyModules_.modul[i].func[j].opts_start ];
      }
#     if 0
      for( k = 0; k < options_n; ++k )
      {
      }
#     endif
    }
  }

  DBG_PROG_ENDE
  return NULL;
}

const char*
oyModulGetName_  (const char *cmm)
{
  oyModule_s *m = 0;
  char *result = NULL;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
    result = m->name;

  DBG_PROG_ENDE
  return result;
}

const char*
oyModulGetDescription_  (const char *cmm)
{
  oyModule_s *m = 0;
  char *result = NULL;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
    result = m->description;


  DBG_PROG_ENDE
  return result;
}

const char*
oyModulGetXml_  (const char *cmm)
{
  oyModule_s *m = 0;
  char *result = NULL;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
    result = m->xml;

  DBG_PROG_ENDE
  return result;
}

const char*  oyModulGetFunc_         ( const char        * cmm,
                                       const char        * id,
                                       const char       ** lib_name )
{
  oyModule_s *m = 0;
  char *result = NULL;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
  {
    oyExternFunc_s * f = oyModulesGetFunc_(cmm, id);
    if(f && f->func_name && m->lib_name)
    {
      if(lib_name)
        *lib_name = m->lib_name;

      result = f->func_name;
    }
  }

  DBG_PROG_ENDE
  return result;
}

/*const char*
oyModulGetDomain_  (const char *cmm)
{
  int i;
  const char *result = NULL;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].domain ;
  }

  DBG_PROG_ENDE
  return result;
}

const char*
oyModulGetDomainPath_  (const char *cmm)
{
  int i;
  const char *result = NULL;

  DBG_PROG_START

  for(i = 0; i < oyModules_.n; ++i)
  {
    if(strcmp(oyModules_.modul[i].id, cmm) == 0)
      result = oyModules_.modul[i].domain_path ;
  }

  DBG_PROG_ENDE
  return result;
}*/

void
oyModulGetGroups_  (const char *cmm, int *start, int *count)
{
  oyModule_s *m = 0;

  DBG_PROG_START

  m = oyModulGet_(cmm);
  if(m)
  {
    if(start)
      *start = m->groups_start;
    if(count)
      *count = m->groups_end - m->groups_start + 1;
  }

  DBG_PROG_ENDE
}

#if 0
const char*
oyModulGetGroupUITitle_ (oyGROUP_e     group, const char **tooltip,
                         const char**config_string_xml)
{
  DBG_PROG_START

  int i, start, count;

  for(i = 0; i < oyModules_.n; ++i)
  {
    {
      start = oyModules_.modul[i].groups_start;
      count = oyModules_.modul[i].groups_end - oyModules_.modul[i].groups_start + 1;
      if(start <= group && group <= start + count - 1)
      {
        int modul_group = group-start;
        if(tooltip)
          *tooltip = oyModules_.modul[i].group[modul_group].description;
        if(config_string_xml)
          *config_string_xml = oyModules_.modul[i].group[modul_group].config_string_xml;

        DBG_PROG_ENDE
        return oyModules_.modul[i].group[modul_group].name;
      }
    }
  }

  DBG_PROG_ENDE
  return NULL;
}
#endif



void
oyModulsRefreshI18N_  (void)
{
  int i;

  DBG_PROG_START

  /* refresh CMM's */
  for( i = 0; i < oyModules_.n; ++i)
  { 
    oyModulGetFromXML_ ( oyGROUP_START, oyModules_.modul[i].xml,
                       &oyModules_.modul[i] );
  }

  DBG_PROG_ENDE
}

void
oyModulRefreshI18N_  (const char *cmm)
{
  oyModule_s *m = 0;

  DBG_PROG_START

  /* refresh CMM */
  m = oyModulGet_(cmm);
  if(m)
    oyModulGetFromXML_ ( oyGROUP_START, m->xml, m );

  DBG_PROG_ENDE
}

