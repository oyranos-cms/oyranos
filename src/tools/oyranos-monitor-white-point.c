/** @file oyranos-monitor-white-point.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2017-2018  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    night vision admin tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and others
 *  @par License:
 *            BSD-3-Clause <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/09/19
 *
 *  A tool to set the white point dependent on local state of the sun.
 *  
 */

# include <stddef.h>

#define __USE_POSIX2 1
#include <stdio.h>                /* popen() */
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* usleep() */

#include "bb_100K.h"
#include "oyranos_color.h"
#include "oyranos_conversion.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_json.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "oyranos_threads.h"
#include "oyProfiles_s.h"

#define DBG_S_ if(oy_debug >= 1)DBG_S
int __sunriset__( int year, int month, int day, double lon, double lat,
                  double altit, int upper_limb, double *trise, double *tset );
int findLocation(oySCOPE_e scope, int dry);
int getLocation( double * lon, double * lat);
double getSunHeight( double year, double month, double day, double gmt_hours, double lat, double lon );
int getSunriseSunset( double * rise, double * set, int dry, int verbose );
int isNight(int dry);
int runDaemon(int dmode);
int setWtptMode( oySCOPE_e scope, int wtpt_mode, int dry );
void pingNativeDisplay();
int checkWtptState();
void updateVCGT();
double estimateTemperature( double cie_a, double cie_b );
double getTemperature                ( double              d );

// public oyjl API
/** @brief Type of widget */
typedef enum oyjlWIDGETTYPE_e {
    oyjlWIDGETTYPE_START,              /**< */
    oyjlWIDGETTYPE_CHOICE,             /**< list of choices */
    oyjlWIDGETTYPE_FUNCTION,           /**< computed list of choices */
    oyjlWIDGETTYPE_DOUBLE,             /**< IEEE floating point number with double precission */
    oyjlWIDGETTYPE_NONE,               /**< no value possible - the option is a flag like -v/--verbose */
    oyjlWIDGETTYPE_END                 /**< */
} oyjlWIDGETTYPE_e;

/** @brief Choice item */
typedef struct oyjlWidgetChoice_s {
  char * nick;                         /**< nick / ID as argument for a option */
  char * name;                         /**< i18n short name for labels */
  char * description;                  /**< i18n description sentence; can be "" */
  char * help;                         /**< i18n longer help text; can be "" */
} oyjlWidgetChoice_s;
void oyjlWidgetChoice_Release        ( oyjlWidgetChoice_s**choices );
typedef struct oyjlOptions_s oyjlOptions_s;
typedef struct oyjlOption_s oyjlOption_s;
typedef union oyjlWidget_u {
  struct choices {
    oyjlWidgetChoice_s * list;         /**< used for oyjlWIDGETTYPE_CHOICES; not needed when *getChoices* is set */
    int selected;                      /**< the currently selected choice */
  } choices;
  /** oyjlWIDGETTYPE_FUNCTION used for oyjlWIDGETTYPE_CHOICES; not needed when *choices* is set
   *  @param[in]   opt                 the option context
   *  @param[out]  selected            show the default; optional
   *  @param[in]   context             for more information
   *  @result                          the same as for the *choices::list* member; caller owns the memory
   */
  oyjlWidgetChoice_s * (*getChoices)( oyjlOption_s * opt, int * selected, oyjlOptions_s * context );
  struct dbl {
    double d;                          /**< default / recommendation */
    double start;
    double end;
    double tick;
  } dbl;                               /**< oyjlWIDGETTYPE_DOUBLE */
} oyjlWidget_u;

/** @brief abstract UI option */
struct oyjlOption_s {
  char type[4];                        /**< must be 'oiwi' */
  unsigned int flags;                  /**< unused */
  char o;                              /**< one letter option name; '-' and ' ' are reserved */
  const char * option;                 /**< string without white space, "my-option"; optional if *o* is present */
  const char * key;                    /**< DB key; optional */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * value_name;             /**< i18n value string; used only for option args; consider using upper case, e.g. FILENAME, NUMBER ... */
  oyjlWIDGETTYPE_e value_type;         /**< type for *choices* */
  oyjlWidget_u values;                 /**< the selectable values for the option; not used for oyjlWIDGETTYPE_NONE */
};

/**
    info to compile a Syntax line and check missing arguments
 */
typedef struct oyjlOptionGroup_s {
  char type [4];                       /**< must be 'oiwg' */
  unsigned int flags;                  /**< unused */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * mandatory;              /**< list of mandatory one letter options for this group of associated options */
  const char * optional;               /**< list of non mandatory one letter options for this group of associated options */
  const char * detail;                 /**< list of one letter options for this group of associated options to display */
} oyjlOptionGroup_s;

/**
 *   main command line, options and groups
 */
struct oyjlOptions_s {
  char type [4];                       /**< must be 'oiws' */
  oyjlOption_s * array;                /**< the options */
  oyjlOptionGroup_s * groups;          /**< groups of options, which form a command */
  void * user_data;                    /**< will be passed to functions; optional */
  int argc;                            /**< plain reference from main(argc,argv) */
  char ** argv;                        /**< plain reference from main(argc,argv) */
  void * private_data;                 /**< internal state; private, do not use */
};
int    oyjlOptions_Count             ( oyjlOptions_s     * opts );
int    oyjlOptions_CountGroups       ( oyjlOptions_s     * opts );
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       char                oc );
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       char              * ostring );
/** @brief option state */
typedef enum {
  oyjlOPTION_NONE,                     /**< untouched */
  oyjlOPTION_USER_CHANGED,             /**< passed in by user */
  oyjlOPTION_MISSING_VALUE,            /**< user error */
  oyjlOPTION_UNEXPECTED_VALUE,         /**< user error */
  oyjlOPTION_NOT_SUPPORTED,            /**< user error */
  oyjlOPTION_DOUBLE_OCCURENCE          /**< user error */
} oyjlOPTIONSTATE_e;
oyjlOPTIONSTATE_e oyjlOptions_Parse  ( oyjlOptions_s     * opts );
oyjlOPTIONSTATE_e oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       char                oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int );
char * oyjlOptions_ResultsToJson     ( oyjlOptions_s     * opts );
char * oyjlOptions_ResultsToText     ( oyjlOptions_s     * opts );
typedef struct oyjlUi_s oyjlUi_s;
void   oyjlOptions_PrintHelp         ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... );
oyjlOptions_s *    oyjlOptions_New   ( int                 argc,
                                       char             ** argv );
void * oyjlMemDup                    ( void              * ptr,
                                       size_t              size );
/** @brief Header section */
typedef struct oyjlUiHeaderSection_s {
  char type [4];                       /**< must be 'oihs' */
  const char * nick;                  /**< single word well known identifier; *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *openicc_modules_author*, *documentation* */
  const char * label;                  /**< i18n short string, in case this section nick is not well known; optional */
  const char * name;                   /**< i18n short content */
  const char * description;            /**< i18n optional second string; might contain a browsable url for further information, e.g. a link to the full text license, home page; optional */
} oyjlUiHeaderSection_s;

/** @brief Info for graphic UI's */
struct oyjlUi_s {
  char type [4];                       /**< must be 'oiui' */
  const char * app_type;               /**< "tool" or "module" */
  char         nick[8];                /**< four byte ID, e.g. "oyjl" */
  const char * name;                   /**< i18n short name for tool bars, app lists */
  const char * description;            /**< i18n name, version, maybe more for a prominent one liner */
  const char * logo;                   /**< file name body without path, for relocation, nor file type ending; typical a PNG or SVG icon; e.g. "lcms_icon" for lcms_icon.png or lcms_icon.svg; optional */
  /** We describe here a particular tool/module. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *openicc_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier. */
  oyjlUiHeaderSection_s * sections;
  oyjlOptions_s * opts;                /**< info for UI logic */
};
oyjlUi_s *   oyjlUi_New              ( int                 argc,
                                       char             ** argv );
int       oyjlUi_CountHeaderSections ( oyjlUi_s          * ui );
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick );
char *       oyjlUi_ToJson           ( oyjlUi_s          * ui,
                                       int                 flags );

// oyjl implementation
void oyjlWidgetChoice_Release        ( oyjlWidgetChoice_s**choices )
{
  int n = 0,i;
  oyjlWidgetChoice_s * ca;
  if(!choices || !*choices) return;
  ca = *choices;
  while(ca[n].nick[0] != '\000') ++n;
  for(i = 0; i < n; ++i)
  {
    oyjlWidgetChoice_s * c = &ca[i];
    if(c->nick) free(c->nick);
    if(c->name) free(c->name);
    if(c->description) free(c->description);
    if(c->help) free(c->help);
  }
  *choices = NULL;
  free(*choices);
}

int oyjlOptions_Count                ( oyjlOptions_s     * opts )
{
  int n = 0;
  while(memcmp(opts->array[n].type, "oiwi", 4) == 0) ++n;
  return n;
}
int oyjlOptions_CountGroups          ( oyjlOptions_s     * opts )
{
  int n = 0;
  while(memcmp(opts->groups[n].type, "oiwg", 4) == 0) ++n;
  return n;
}


enum {
  oyjlOPTIONSTYLE_ONELETTER = 0x01,
  oyjlOPTIONSTYLE_STRING = 0x02,
  oyjlOPTIONSTYLE_OPTIONAL = 0x04
};
void  oyjlOption_PrintArg            ( oyjlOption_s      * o,
                                       int                 style )
{
  if(!o) return;
  if(style & oyjlOPTIONSTYLE_OPTIONAL)
    fprintf( stderr, "[" );
  if(style & oyjlOPTIONSTYLE_ONELETTER)
    fprintf( stderr, "-%c", o->o );
  if(style & oyjlOPTIONSTYLE_ONELETTER && style & oyjlOPTIONSTYLE_STRING)
    fprintf( stderr, "|" );
  if(style & oyjlOPTIONSTYLE_STRING)
    fprintf( stderr, "--%s", o->option );
  if(o->value_name)
    fprintf( stderr, " %s", o->value_name );
  if(style & oyjlOPTIONSTYLE_OPTIONAL)
    fprintf( stderr, "]" );
  fprintf( stderr, " " );
}
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       char                oc )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->o && o->o == oc)
      return o;
    else
      o = NULL;
  }
  return o;
}
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       char              * ostring )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;
  char * str = oyjlStringCopy(ostring, malloc);
  char * t = strchr(str, '=');

  if(t)
    t[0] = '\000';

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->option && strcmp(o->option, str) == 0)
      return o;
    else
      o = NULL;
  }
  fprintf( stderr, "Option not found: %s\n", str );

  return o;
}
oyjlOPTIONSTATE_e oyjlOptions_Check  ( oyjlOptions_s     * opts )
{
  int i,j;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL, * b = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    for(j = i+1; j < nopts; ++j)
    {
      b = &opts->array[j];
      if(o->o == b->o)
      {
        fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Double occuring option"), b->o );
        return oyjlOPTION_DOUBLE_OCCURENCE;
      }
    }
  }
  return oyjlOPTION_NONE;
}

oyjlOPTIONSTATE_e oyjlOptions_Parse  ( oyjlOptions_s     * opts )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  oyjlOption_s * o;

  /* parse the command line arguments */
  if(!opts->private_data)
  {
    int i, pos = 0;
    char ** result = (char**) calloc( 2, sizeof(char*) );
    result[0] = (char*) calloc( 65536, sizeof(char) );
    if((state = oyjlOptions_Check(opts)) != oyjlOPTION_NONE)
      return state;
    for(i = 1; i < opts->argc; ++i)
    {
      char * str = opts->argv[i];
      int l = strlen(str);
      char arg = ' ';
      char * long_arg = NULL;
      char * value = NULL;

      /* parse -a | -a value | -a=value | -ba | -ba value | -ba=value */
           if(l > 1 && str[0] == '-' && str[1] != '-')
      {
        int require_value, j;
        for(j = 1; j < l; ++j)
        {
          arg = str[j];
          o = oyjlOptions_GetOption( opts, arg );
          if(!o)
          {
            fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option not supported"), arg );
            state = oyjlOPTION_NOT_SUPPORTED;
            return state;
          }
          require_value = o->value_type != oyjlWIDGETTYPE_NONE;
          if( require_value )
          {
            value = NULL;
            if( j == l-1 && opts->argc > i+1 && (opts->argv[i+1][0] != '-' || strlen(opts->argv[i+1]) <= 1) )
            {
              value = opts->argv[i+1];
              ++i;
            }
            else if( str[j+1] == '=' )
            {
              ++j;
              value = &str[j+1];
              j = l;
            }
            else
            {
              fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option needs a argument"), arg );
              state = oyjlOPTION_MISSING_VALUE;
            }
            if(value)
            {
              int llen = 0;
              while(result[llen]) ++llen;
              result[0][pos] = arg;
              ++pos;
              oyjlStringListAddStaticString( &result, &llen, value, malloc, free );
            }
          }
          else if(!require_value && !(j < l-1 && str[j+1] == '='))
          {
            int llen = 0;
            while(result[llen]) ++llen;
            result[0][pos] = arg;
            ++pos;
            oyjlStringListAddStaticString( &result, &llen, "1", malloc, free );
          }
          else
          {
            fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option has a unexpected argument"), arg );
            state = oyjlOPTION_UNEXPECTED_VALUE;
            j = l;
          }
        }
      }
      /* parse --arg | --arg value | --arg=value */
      else if(l > 2 && str[0] == '-' && str[1] == '-')
      {
        int require_value;
        long_arg = &str[2];
        o = oyjlOptions_GetOptionL( opts, long_arg );
        if(!o)
        {
          fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option not supported"), long_arg );
          state = oyjlOPTION_NOT_SUPPORTED;
          return state;
        }
        require_value = o->value_type != oyjlWIDGETTYPE_NONE;
        if( require_value )
        {
          value = NULL;

          if( strchr(str, '=') != NULL )
            value = strchr(str, '=') + 1;
          else if( opts->argc > i+1 && opts->argv[i+1][0] != '-' )
          {
            value = opts->argv[i+1];
            ++i;
          }
          else
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option needs a argument"), long_arg );
            state = oyjlOPTION_MISSING_VALUE;
          }

          if(value)
          {
            int llen = 0;
            while(result[llen]) ++llen;
            result[0][pos] = o->o;
            ++pos;
            oyjlStringListAddStaticString( &result, &llen, value, malloc, free );
          }
        }
      }
      /* parse anonymous value, if requested */
      else
      {
        result[0][pos] = '-';
        ++pos;
      }
    }
    opts->private_data = result;
  }

  return state;
}
oyjlOPTIONSTATE_e oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       char                oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  ptrdiff_t pos = -1;
  const char * t;
  const char ** results;
  const char * list;

  /* parse the command line arguments */
  if(!opts->private_data)
    state = oyjlOptions_Parse( opts );
  if(state != oyjlOPTION_NONE)
    return state;

  results = opts->private_data;
  list = results[0];
  t = strrchr( list, oc );
  if(t)
  {
    pos = t - list;
    state = oyjlOPTION_USER_CHANGED;
  }
  else if(oc == ' ' && strlen(list))
  {
    if(result_int)
      *result_int = strlen(list);
    return oyjlOPTION_USER_CHANGED;
  }
  else
    return oyjlOPTION_NONE;

  t = results[1 + pos];

  if(result_string)
    *result_string = t;

  if(result_dbl)
    oyjlStringToDouble( t, result_dbl );
  if(result_int)
  {
    int l = strlen( list ), i,n = 0;
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(o->value_type == oyjlWIDGETTYPE_NONE)
    {
      for(i = 0; i < l; ++i)
        if(list[i] == oc)
          ++n;
      *result_int = n;
    } else
    {
      long lo = 0;
      if(oyjlStringToLong( t, &lo ) == 0)
        *result_int = lo;
    }
  }

  return state;
}

char * oyjlOptions_ResultsToJson     ( oyjlOptions_s     * opts )
{
  char * args = NULL,
       * rjson = NULL;
  char ** results = opts->private_data;
  oyjl_val root, value;
  int i,n;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;
  }

  args = results[0];
  n = strlen( args );
  root = oyjlTreeNew( "" );
  for(i = 0; i < n; ++i)
  {
    char a[4] = {0,0,0,0};
    a[0] = args[i];
    value = oyjlTreeGetValue( root, OYJL_CREATE_NEW, a );
    oyjlValueSetString( value, results[i+1] );
  }

  i = 0;
  oyjlTreeToJson( root, &i, &rjson );
  oyjlTreeFree( root );

  return rjson;
}
char * oyjlOptions_ResultsToText     ( oyjlOptions_s     * opts )
{
  char * args = NULL,
       * text = NULL;
  char ** results = opts->private_data;
  int i,n;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;
  }

  args = results[0];
  n = strlen( args );
  for(i = 0; i < n; ++i)
  {
    char a[4] = {args[i],0,0,0};
    char * value = results[i+1];
    oyjlStringAdd( &text, malloc, free, "%s:%s\n", a, value );
  }

  return text;
}

void  oyjlOptions_PrintHelpSynopsis  ( oyjlOptions_s     * opts,
                                       oyjlOptionGroup_s * g,
                                       int                 style )
{
  int i;
  int m = g->mandatory ? strlen(g->mandatory) : 0;
  int on = g->optional ? strlen(g->optional) : 0;

  if( m || on )
    fprintf( stderr, "%s ", opts->argv[0] );
  else
    return;

  for(i = 0; i < m; ++i)
  {
    char oc = g->mandatory[i];
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(oc == '|')
      fprintf( stderr, "| " );
    else if(!o)
    {
      printf("\n%s: option not declared: %c\n", g->name, oc);
      exit(1);
    }
    oyjlOption_PrintArg(o, style);
  }
  for(i = 0; i < on; ++i)
  {
    char oc = g->optional[i];
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(!o)
    {
      printf("\n%s: option not declared: %c\n", g->name, oc);
      exit(1);
    }
    oyjlOption_PrintArg(o, style | oyjlOPTIONSTYLE_OPTIONAL);
  }

  fprintf( stderr, "\n" );
}
static oyjlWidgetChoice_s ** oyjl_get_choices_list_ = NULL;
static int oyjl_get_choices_list_selected_[256];
oyjlWidgetChoice_s * oyjlOption_GetChoices_ (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts )
{
  if(!o) return NULL;

  if(!oyjl_get_choices_list_)
  {
    int i;
    for(i = 0; i < 256; ++i) oyjl_get_choices_list_selected_[i] = -1;
    oyjl_get_choices_list_ = calloc( sizeof(oyjlWidgetChoice_s*), 256 ); /* number of possible chars */
  }

  if( !oyjl_get_choices_list_[(int)o->o] ||
      (selected && oyjl_get_choices_list_selected_[(int)o->o] == -1) )
    oyjl_get_choices_list_[(int)o->o] = o->values.getChoices(o, selected ? &oyjl_get_choices_list_selected_[(int)o->o] : selected, opts );

  if(selected)
    *selected = oyjl_get_choices_list_selected_[(int)o->o];
  return oyjl_get_choices_list_[(int)o->o];
}
#include <stdarg.h> /* va_list */
void  oyjlOptions_PrintHelp          ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... )
{
  int i,ng;
  va_list list;
  int indent = 2;
  oyjlUiHeaderSection_s * section = NULL;
  fprintf( stderr, "\n");
  if(verbose)
    for(i = 0; i < opts->argc; ++i)
      fprintf( stderr, "\'%s\' ", opts->argv[i]);
  fprintf( stderr, "\n");

  va_start( list, motto_format );
  vfprintf( stderr, motto_format, list );
  va_end  ( list );

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return;

  if( ui && (section = oyjlUi_GetHeaderSection(ui, "documentation")) != NULL &&
      section->description )
    fprintf( stderr, "\n%s:\n  %s\n", _("Description"), section->description );

  fprintf( stderr, "\n%s:\n", _("Synopsis") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    fprintf( stderr, "  " );
    oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER );
  }

  fprintf( stderr, "\n%s:\n", _("Usage")  );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = g->detail ? strlen(g->detail) : 0,
        j,k;
    fprintf( stderr, "  %s\n", g->description  );
    if(g->mandatory && g->mandatory[0])
    {
      fprintf( stderr, "\t" );
      oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER );
    }
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      if(!o)
      {
        printf("\n%s: option not declared: %c\n", g->name, oc);
        exit(1);
      }
      for(k = 0; k < indent; ++k) fprintf( stderr, " " );
      switch(o->value_type)
      {
        case oyjlWIDGETTYPE_CHOICE:
          {
            int n = 0,l;
            while(o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              fprintf( stderr, "\t  -%c %s\t\t# %s\n", o->o, o->values.choices.list[l].nick, o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description );
          }
          break;
        case oyjlWIDGETTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlWidgetChoice_s * list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              fprintf( stderr, "\t  -%c %s\t\t# %s\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlWidgetChoice_Release( &list ); */
          }
          break;
        case oyjlWIDGETTYPE_DOUBLE:
          fprintf( stderr, "\t" );
          oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
          fprintf( stderr, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
          break;
        case oyjlWIDGETTYPE_NONE:
          fprintf( stderr, "\t" );
          oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
          fprintf( stderr, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
        break;
        case oyjlWIDGETTYPE_START: break;
        case oyjlWIDGETTYPE_END: break;
      }
    }
    if(d) fprintf( stderr, "\n" );
  }
  fprintf( stderr, "\n" );
}

oyjlOptions_s * oyjlOptions_New      ( int                 argc,
                                       char             ** argv )
{
  oyjlOptions_s * opts = calloc( sizeof(oyjlOptions_s), 1 );
  memcpy( opts->type, "oiws", 4 );

  opts->argc = argc;
  opts->argv = argv;
  return opts;
}
oyjlUi_s *   oyjlUi_New              ( int                 argc,
                                       char             ** argv )
{
  oyjlUi_s * ui = calloc( sizeof(oyjlUi_s), 1 );
  memcpy( ui->type, "oiui", 4 );
  ui->opts = oyjlOptions_New( argc, argv );
  return ui;
}
void         oyjlTreeSetValueString  ( oyjl_val            root,
                                       const char        * key,
                                       const char        * value )
{
  oyjl_val v = oyjlTreeGetValue( root, OYJL_CREATE_NEW, key );
  oyjlValueSetString( v, value );
}
int       oyjlUi_CountHeaderSections ( oyjlUi_s          * ui )
{
  int n = 0;
  while(memcmp(ui->sections[n].type, "oihs", 4) == 0) ++n;
  return n;
}
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick )
{
  oyjlUiHeaderSection_s * section = NULL;
  int i, count = oyjlUi_CountHeaderSections(ui);
  for(i = 0; i < count; ++i)
    if( strcmp(ui->sections[i].nick, nick) == 0 )
      section = &ui->sections[i];
  return section;
}
char *       oyjlUi_ToJson           ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * t = NULL, num[64];
  oyjl_val root, key;
  int i,n,ng;

  root = oyjlTreeNew( "" );
  oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/openicc_module_api_version", "1" );
  if(ui->type)
  {
    oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/type", ui->app_type );
    if(strcmp( ui->app_type, "tool" ) == 0)
      oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/label", _("Tool") );
    else if(strcmp( ui->app_type, "module" ) == 0)
      oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/label", _("Module") );
  }
  oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/nick", ui->nick );
  oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/name", ui->name );
  oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/description", ui->description );
  if(ui->logo)
    oyjlTreeSetValueString( root, "org/freedesktop/openicc/modules/[0]/logo", ui->logo );

  n = oyjlUi_CountHeaderSections( ui );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/information/[%d]/%s", i, "type" );
    oyjlValueSetString( key, s->nick );
    key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/information/[%d]/%s", i, "label" );
    if(s->label)
      oyjlValueSetString( key, s->label );
    else
    {
      if(strcmp(s->nick, "manufacturer") == 0) oyjlValueSetString( key, _("Manufacturer") );
      else if(strcmp(s->nick, "copyright") == 0) oyjlValueSetString( key, _("Copyright") );
      else if(strcmp(s->nick, "license") == 0) oyjlValueSetString( key, _("License") );
      else if(strcmp(s->nick, "url") == 0) oyjlValueSetString( key, _("Url") );
      else if(strcmp(s->nick, "support") == 0) oyjlValueSetString( key, _("Support") );
      else if(strcmp(s->nick, "download") == 0) oyjlValueSetString( key, _("Download") );
      else if(strcmp(s->nick, "sources") == 0) oyjlValueSetString( key, _("Sources") );
      else if(strcmp(s->nick, "development") == 0) oyjlValueSetString( key, _("Entwicklung") );
      else if(strcmp(s->nick, "openicc_module_author") == 0) oyjlValueSetString( key, _("OpenICC Module Author") );
      else if(strcmp(s->nick, "documentation") == 0) oyjlValueSetString( key, _("Documentation") );
      else oyjlValueSetString( key, _(s->nick) );
    }
    key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/information/[%d]/%s", i, "name" );
    oyjlValueSetString( key, s->name );
    if(s->description)
    {
      key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/information/[%d]/%s", i, "description" );
      oyjlValueSetString( key, s->description );
    }
  }

  ng = oyjlOptions_CountGroups( ui->opts );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &ui->opts->groups[i];
    oyjlOptions_s * opts = ui->opts;

    if(!(g->mandatory && g->mandatory[0]))
      continue;

    key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/%s", i, "name" );
    oyjlValueSetString( key, g->name );
    key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/%s", i, "description" );
    oyjlValueSetString( key, g->description );
    if(g->help)
    {
      key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/%s", i, "help" );
      oyjlValueSetString( key, g->help );
        printf("found help: %s\n", g->help);
    }

    int d = g->detail ? strlen(g->detail) : 0,
        j;
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "key" );
      if(!o->key)
        sprintf(num, "%c", o->o);
      oyjlValueSetString( key, o->key?o->key:num );
      key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "name" );
      oyjlValueSetString( key, o->name );
      key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "description" );
      oyjlValueSetString( key, o->description );
      if(o->help)
      {
        key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "help" );
        oyjlValueSetString( key, o->help );
      }

      switch(o->value_type)
      {
        case oyjlWIDGETTYPE_CHOICE:
          {
            int n = 0,l;
            key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
            sprintf( num, "%d", o->values.choices.selected );
            oyjlValueSetString( key, num );
            while(o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
            {
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
              oyjlValueSetString( key, o->values.choices.list[l].nick );
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
              oyjlValueSetString( key, o->values.choices.list[l].name );
            }
          }
          break;
        case oyjlWIDGETTYPE_FUNCTION:
          {
            int n = 0,l, selected;
            oyjlWidgetChoice_s * list = oyjlOption_GetChoices_(o, &selected, opts );
            if(list)
              while(list[n].nick[0] != '\000')
                ++n;
            if(0 <= selected && selected < n && strlen(list[selected].nick))
            {
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              oyjlValueSetString( key, list[selected].nick );
            }
            for(l = 0; l < n; ++l)
            {
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
              oyjlValueSetString( key, list[l].nick );
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
              oyjlValueSetString( key, list[l].name );
            }
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlWidgetChoice_Release( &list ); */
          }
          break;
        case oyjlWIDGETTYPE_DOUBLE:
          key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
          sprintf( num, "%g", o->values.dbl.d ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "start" );
          sprintf( num, "%g", o->values.dbl.start ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "end" );
          sprintf( num, "%g", o->values.dbl.end ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "tick" );
          sprintf( num, "%g", o->values.dbl.tick ); oyjlValueSetString( key, num );
          break;
        case oyjlWIDGETTYPE_NONE:
          key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
          oyjlValueSetString( key, "0" );
          {
            int l; char t[4];
            for(l = 0; l < 2; ++l)
            {
              sprintf(t, "%d", l);
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
              oyjlValueSetString( key, t );
              key = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "org/freedesktop/openicc/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
              oyjlValueSetString( key, l?_("Yes"):_("No") );
            }
          }
        break;
        case oyjlWIDGETTYPE_START: break;
        case oyjlWIDGETTYPE_END: break;
      }
    }
  }
  i = 0;
  oyjlTreeToJson( root, &i, &t );

  return t;
}
void * oyjlMemDup                    ( void              * ptr,
                                       size_t              size )
{
  void * dest = malloc(size);
  if(dest)
    memcpy( dest, ptr, size );
  return dest;
}
// TODO: explicite allow for non option bound arguments, for syntax checking - use '-' as special option inside oyjlOptionGroup_s::mandatory
// TODO: export man page
// end of oyjl

static uint32_t icc_profile_flags = 0;
static oyjlWidgetChoice_s * linear_effect_choices_ = NULL;
static oyjlWidgetChoice_s * getLinearEffectProfileChoices (
                                                  oyjlOption_s      * o,
                                                  int               * selected,
                                                  oyjlOptions_s     * opts OY_UNUSED )
{
    int choices = 0, current = -1;
    oyProfiles_s * patterns = oyProfiles_New( NULL ),
                 * profiles = 0;

    if(!selected && linear_effect_choices_)
      return linear_effect_choices_;

    // only linear effects
    oyProfile_s * pattern = oyProfile_FromFile( "meta:EFFECT_linear;yes", OY_NO_LOAD, NULL );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    if(icc_profile_flags == 0)
      icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
    profiles = oyProfiles_Create( patterns, icc_profile_flags, NULL );
    oyProfiles_Release( &patterns );
    choices = oyProfiles_Count( profiles );

    if(choices)
    {
      int i;
      char * value = NULL;
      oyjlWidgetChoice_s * c = calloc(1+choices+1, sizeof(oyjlWidgetChoice_s));

      if(o->o == 'g' && selected)
        value = oyGetPersistentString( OY_DISPLAY_STD "/night_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      else if(o->o == 'e' && selected)
        value = oyGetPersistentString( OY_DISPLAY_STD "/sunlight_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      else if(selected)
        *selected = 0;

      if(c)
      {
        linear_effect_choices_ = c;

        /* first entry is for unsetting */
        i = 0;
        c[i].nick = strdup("-");
        c[i].name = strdup(_("[none]"));
        c[i].description = strdup("");
        c[i].help = strdup("");

        /* profile choices */
        for(i = 0; i < choices; ++i)
        {
          oyProfile_s * p = oyProfiles_Get( profiles, i );
          const char * nick = oyProfile_GetText( p, oyNAME_NICK );

          c[i+1].nick = strdup(nick);
          c[i+1].name = strdup(oyProfile_GetText( p, oyNAME_DESCRIPTION ));
          c[i+1].description = strdup(oyProfile_GetFileName( p, 0));
          c[i+1].help = strdup("");

          if(selected && value && strcmp(value, nick) == 0)
            *selected = i+1;

          oyProfile_Release( &p );
        }
        /* empty choice for end of list */
        c[i+1].nick = malloc(4);
        c[i+1].nick[0] = '\000';
      }

      return c;
    } else
      return NULL;
}
double getTemperature                ( double              d )
{
  double cie_a = 0.0, cie_b = 0.0, XYZ[3], Lab[3];
  oyGetDisplayWhitePoint( 1 /* automatic */, XYZ );
  oyXYZ2Lab(XYZ,Lab); cie_a = Lab[1]/256.0+0.5; cie_b = Lab[2]/256.0+0.5;
  double temperature = estimateTemperature( cie_a, cie_b );
  if(temperature)
    return temperature;
  else
    return d;
}
double getDoubleFromDB               ( const char        * key,
                                       double              fallback )
{
  double d = fallback, tmp = 0;
  char * value = oyGetPersistentString( key, 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  if(value && oyjlStringToDouble( value, &tmp ) == 0)
    d = tmp;
  if(value)
    oyDeAllocateFunc_( value );
  return d;
}
static oyjlWidgetChoice_s * white_point_choices_ = NULL;
static int white_point_choices_selected_ = -1;
oyjlWidgetChoice_s * getWhitePointChoices       ( oyjlOption_s      * o,
                                                  int               * selected,
                                                  oyjlOptions_s     * opts OY_UNUSED )
{
    uint32_t flags = 0;
    int choices = 0, current = -1;
    const char ** choices_string_list = NULL;
    int error = 0;

    if(!selected && white_point_choices_)
      return white_point_choices_;

    error = oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                 oyNAME_NAME, &choices,
                                 &choices_string_list, &current );
    if(!error)
    {
      int i;
      long l = -1;
      char * value = NULL;
      oyjlWidgetChoice_s * c = calloc(choices+1, sizeof(oyjlWidgetChoice_s));
      if(c)
      {
        white_point_choices_ = c;
        for(i = 0; i < choices; ++i)
        {
          char * v = malloc(12);

          sprintf(v, "%d", i);
          c[i].nick = v;
          c[i].name = strdup(choices_string_list[i]);
          c[i].description = strdup("");
          c[i].help = strdup("");

          if(i == 1) /* automatic */
          {
            double temperature = getTemperature(0);
            if(temperature)
              oyStringAddPrintf( &c[i].name, 0,0, " %g %s ", temperature, _("Kelvin") );
          }
          if((int)i == current && o->o == 'w' && 0 /* not possible, as the result is used over different options */)
            oyStringAddPrintf( &c[i].name, 0,0, " *" );
        }
        c[i].nick = malloc(4);
        c[i].nick[0] = '\000';
      }
      white_point_choices_selected_ = current;
      if(selected)
        *selected = current;
      if(o->o == 'n' && selected)
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_night", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value && oyjlStringToLong( value, &l ) == 0)
          *selected = l;
      } else if(o->o == 's' && selected)
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_sunlight", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value && oyjlStringToLong( value, &l ) == 0)
          *selected = l;
      }

      return c;
    } else
      return NULL;
}
const char * jcommands = "{\n\
  \"command_set\": \"oyranos-monitor-white-point\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"oyranos-monitor-white-point\",\n\
  \"command_get_args\": [\"-j\"]\n\
}";
void myOptionsFill                   ( oyjlOptions_s       * opts )
{
  DBG_S_( oyPrintTime() );
  /* nick, name, description, help */
  oyjlWidgetChoice_s d_choices[] = {{"0", _("Deactivate"), _("Deactivate"), ""},
                                    {"1", _("Autostart"), _("Autostart"), ""},
                                    {"2", _("Activate"), _("Activate"), ""},
                                    {"","","",""}};
  oyjlOption_s oarray[] = {
  /* type,   flags, o, option, key, name, description, help, value_name, value_type, values */
    {"oiwi", 0, 'd', "daemon", NULL, _("daemon"), _("Control user daemon"), NULL, "0|1|2", oyjlWIDGETTYPE_CHOICE, {.choices.list = oyjlMemDup( d_choices, sizeof(d_choices) )} },
    {"oiwi", 0, 'm', "modes", NULL, _("Modes"), _("Show white point modes"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'w', "white-point", NULL, _("Mode"), _("Set white point mode"), NULL, "0|1|2|3|4|5|6|7", oyjlWIDGETTYPE_FUNCTION, {.getChoices = getWhitePointChoices} },
    {"oiwi", 0, 'n', "night-white-point", NULL, _("Night Mode"), _("Set night time mode"), NULL, "0|1|2|3|4|5|6|7", oyjlWIDGETTYPE_FUNCTION, {.getChoices = getWhitePointChoices} },
    {"oiwi", 0, 's', "sun-white-point", NULL, _("Day Mode"), _("Set day time mode"), NULL, "0|1|2|3|4|5|6|7", oyjlWIDGETTYPE_FUNCTION, {.getChoices = getWhitePointChoices} },
    {"oiwi", 0, 'a', "automatic", NULL, _("Temperature"), _("A value from 2700 till 8000 Kelvin is expected to show no artefacts"), NULL,
      /*  The white point profiles will be generated in many different shades, which will explode
       *  conversion cache. Thus we limit the possible shades to 100 kelvin steps, which in turn
       *  limits to around 100 profiles per monitor white point. */
      _("KELVIN"), oyjlWIDGETTYPE_DOUBLE, {.dbl.start = 1100, .dbl.end = 10100, .dbl.tick = 100, .dbl.d = getTemperature(5000)} },
    {"oiwi", 0, 'g', "night-effect", NULL, _("Night effect"), _("Set night time effect"), _("A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes ."), _("ICC_PROFILE"), oyjlWIDGETTYPE_FUNCTION, {.getChoices = getLinearEffectProfileChoices} },
    {"oiwi", 0, 'e', "sunlight-effect", NULL, _("Sun light effect"), _("Set day time effect"), _("A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes ."), _("ICC_PROFILE"), oyjlWIDGETTYPE_FUNCTION, {.getChoices = getLinearEffectProfileChoices} },
    {"oiwi", 0, 'l', "location", NULL, _("location"), _("Detect location by IP adress"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'o', "longitude", NULL, _("Longitude"), _("Set Longitude"), NULL, _("ANGLE_IN_DEGREE"), oyjlWIDGETTYPE_DOUBLE,
      {.dbl.start = -180, .dbl.end = 180, .dbl.tick = 1, .dbl.d = getDoubleFromDB( OY_DISPLAY_STD "/longitude", 0 )} },
    {"oiwi", 0, 'i', "latitude", NULL, _("Latitude"), _("Set Latitude"), NULL, _("ANGLE_IN_DEGREE"), oyjlWIDGETTYPE_DOUBLE,
      {.dbl.start = -90, .dbl.end = 90, .dbl.tick = 1, .dbl.d = getDoubleFromDB( OY_DISPLAY_STD "/latitude", 0 )} },
    {"oiwi", 0, 'r', "sunrise", NULL, _("Sunrise"), _("Show local time, used geographical location, twilight height angles, sun rise and sun set times"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 't', "twilight", NULL, _("Twilight"), _("Set Twilight angle"), NULL, _("ANGLE_IN_DEGREE|0:rise/set|-6:civil|-12:nautical|-18:astronomical"), oyjlWIDGETTYPE_DOUBLE,
      {.dbl.start = 18, .dbl.end = -18, .dbl.tick = 1, .dbl.d = getDoubleFromDB( OY_DISPLAY_STD "/twilight", 0 )} },
    {"oiwi", 0, 'z', "system-wide", NULL, _("system wide"), _("System wide DB setting"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'j', "oi-json", NULL, _("OpenICC UI Json"), _("Get OpenICC Json UI declaration"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'J', "oi-json-command", NULL, _("OpenICC UI Json + command"), _("Get OpenICC Json UI declaration incuding command"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'h', "help", NULL, _("help"), _("Help"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'v', "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'y', "dry-run", NULL, "dry run", "dry run", NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"oiwi", 0, 'u', "hour", NULL, "hour", "hour", NULL, NULL, oyjlWIDGETTYPE_DOUBLE, {.dbl.start = 0, .dbl.end = 48, .dbl.tick = 1, .dbl.d = 0} },
    {"oiwi", 0, 'c', "check", NULL, "check", "check", NULL, NULL, oyjlWIDGETTYPE_NONE, {} },
    {"",0,0,0,0,0,0,0, NULL, oyjlWIDGETTYPE_END, {}}
  };
  opts->array = oyjlMemDup( oarray, sizeof(oarray) );

  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name, description, help, mandatory, optional, detail */
    {"oiwg", 0, _("Mode"), _("Actual mode"), NULL, "wa", "zv", "wa" },
    {"oiwg", 0, _("Night Mode"), _("Nightly appearance"), NULL, "n", "gzv", "ng" },
    {"oiwg", 0, _("Day Mode"), _("Sun light appearance"), NULL, "s", "ezv", "se" },
    {"oiwg", 0, _("Location"), _("Location and Twilight"), NULL, "l|oi", "tzv", "loit"},
    {"oiwg", 0, _("Daemon Service"), _("Run sunset daemon"), NULL, "d", "v", "d" },
    {"oiwg", 0, _("Misc"), _("General options"), NULL, "", "", "zmrjJvh" },
    {"",0,0,0,0,0,0,0}
  };
  double night = isNight(0);
  oyjlOptionGroup_s ng;
  if(night == 1)
  {
    memcpy( &ng, &groups[0], sizeof(oyjlOptionGroup_s) );
    memcpy( &groups[0], &groups[1], sizeof(oyjlOptionGroup_s) );
    memcpy( &groups[1], &ng, sizeof(oyjlOptionGroup_s) );
  } else if(night == 0)
  {
    memcpy( &ng, &groups[0], sizeof(oyjlOptionGroup_s) );
    memcpy( &groups[0], &groups[2], sizeof(oyjlOptionGroup_s) );
    memcpy( &groups[2], &ng, sizeof(oyjlOptionGroup_s) );
  }
  opts->groups = oyjlMemDup( groups, sizeof(groups));
  DBG_S_( oyPrintTime() );
}
void myOptionsRelease                ( oyjlOptions_s      ** opts )
{
  if(*opts) free(*opts);
  *opts = NULL;
}
void myUiFill                        ( oyjlUi_s            * ui )
{
  DBG_S_( oyPrintTime() );
  oyjlUiHeaderSection_s s[] = {
    /* type, nick, label, name, description */
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.oyranos.org" },
    { "oihs", "copyright", NULL, "Copyright 2018 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.github.com/oyranos-cms/oyranos/issues", NULL },
    { "oihs", "download", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "sources", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "development", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "openicc_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.openicc.info", _("The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected.") },
    { "", NULL, NULL, NULL, NULL }
  };
  ui->app_type = "tool";
  memcpy( ui->nick, "oyNM", 4 );
  ui->name = _("Night Manager");
  ui->description = _("Oyranos Night Manager");
  ui->logo = "oyranos_logo";
  ui->sections = oyjlMemDup( s, sizeof(s) );
  DBG_S_( oyPrintTime() );
}


oySCOPE_e scope = oySCOPE_USER;
double hour_ = -1.0; /* ignore this default value */

int main( int argc , char** argv )
{
  unsigned i;
  int error = 0;
  /* the functional switches */
  int wtpt_mode = -1;
  int wtpt_mode_night = -1;
  int wtpt_mode_sunlight = -1;
  const char * sunlight_effect = NULL;
  const char * night_effect = NULL;
  double temperature = 0.0;
  int show = 0;
  int json = 0;
  int json_command = 0;
  int dry = 0;
  int location = 0;
  double longitude = 360;
  double latitude = 360;
  int sunrise = 0;
  double twilight = -1000;
  char * value = NULL;
  double rise = 0.0,
         set = 0.0;
  int daemon = -1;
  int check = 0;
  int help = 0;
  int verbose = 0;
  oyjlOptions_s * opts;
  oyjlUi_s * ui;

  if(getenv(OY_DEBUG))
  {
    int value = atoi(getenv(OY_DEBUG));
    if(value > 0)
      oy_debug += value;
    DBG_S_( oyPrintTime() );
  }
  if(oy_debug)
    fprintf(stderr, " %.06g %s\n", DBG_UHR_, oyPrintTime() );

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  DBG_S_( oyPrintTime() );
  ui = oyjlUi_New( argc, argv );
  opts = ui->opts;
  myOptionsFill( opts );
  myUiFill( ui );
  /* parse the options */
  oyjlOPTIONSTATE_e state = oyjlOptions_Parse( opts );
  DBG_S_( oyPrintTime() );
  if(state != oyjlOPTION_NONE)
  {
    fputs( _("... try with --help|-h option for usage text. give up"), stderr );
    fputs( "\n", stderr );
    exit(1);
  }
  oyjlOptions_GetResult( opts, 'v', NULL, NULL, &verbose );
  if(verbose > 1)
    oy_debug += verbose -1;

  if(verbose)
  {
    char * json = oyjlOptions_ResultsToJson( opts );
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = oyjlOptions_ResultsToText( opts );
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );
  }

  /* assign command line options to variables */
  oyjlOptions_GetResult( opts, 'd', NULL, NULL, &daemon );
  oyjlOptions_GetResult( opts, 'a', NULL, &temperature, NULL);
  oyjlOptions_GetResult( opts, 'r', NULL, NULL, &sunrise);
  oyjlOptions_GetResult( opts, 'l', NULL, NULL, &location);
  oyjlOptions_GetResult( opts, 'n', NULL, NULL, &wtpt_mode_night);
  oyjlOptions_GetResult( opts, 's', NULL, NULL, &wtpt_mode_sunlight);
  oyjlOptions_GetResult( opts, 'o', NULL, &longitude, NULL);
  oyjlOptions_GetResult( opts, 'i', NULL, &latitude, NULL);
  oyjlOptions_GetResult( opts, 't', NULL, &twilight, NULL);
  oyjlOptions_GetResult( opts, 'w', NULL, NULL, &wtpt_mode);
  oyjlOptions_GetResult( opts, 'j', NULL, NULL, &json);
  oyjlOptions_GetResult( opts, 'J', NULL, NULL, &json_command);
  oyjlOptions_GetResult( opts, 'm', NULL, NULL, &show);
  oyjlOptions_GetResult( opts, 'z', NULL, NULL, &dry);
  oyjlOptions_GetResult( opts, 'u', NULL, &hour_, NULL);
  oyjlOptions_GetResult( opts, 'g', &night_effect, NULL, NULL);
  oyjlOptions_GetResult( opts, 'e', &sunlight_effect, NULL, NULL);
  oyjlOptions_GetResult( opts, 'c', NULL, NULL, &check);
  oyjlOptions_GetResult( opts, 'h', NULL, NULL, &help);
  if(help || oyjlOptions_GetResult( opts, ' ', NULL, NULL, NULL) == oyjlOPTION_NONE)
  {
    int use_option_defaults = 1;
    opts->user_data = &use_option_defaults;
    oyjlOptions_PrintHelp( opts, ui, oy_debug, "%s v%d.%d.%d %s\n", argv[0],
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a monitor white point handler") );
    exit (0);
  }

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));


  if(temperature != 0.0)
  {
    int i = (temperature - 1000) / 100;
    double xyz[3] = { bb_100K[i][0], bb_100K[i][1], bb_100K[i][2] };
    double XYZ[3] = { xyz[0]/xyz[1], 1.0, xyz[2]/xyz[1] }, oldXYZ[3];
    double Lab[3], oldLab[3];
    double cie_a = 0.0, cie_b = 0.0;
    char * comment = NULL;
    double old_temperature = 0;
    oyXYZ2Lab( XYZ, Lab);

    oyGetDisplayWhitePoint( 1 /* automatic */, oldXYZ );
    oyXYZ2Lab( oldXYZ, oldLab);
    cie_a = oldLab[1]/256.0 + 0.5;
    cie_b = oldLab[2]/256.0 + 0.5;

    old_temperature = estimateTemperature( cie_a, cie_b );
    if(old_temperature)
      oyStringAddPrintf( &comment, 0,0, "Old Temperature:: %g ", old_temperature );
    oyStringAddPrintf( &comment, 0,0, "Old cie_a: %f cie_b: %f ", cie_a, cie_b );
    cie_a = Lab[1]/256.0 + 0.5;
    cie_b = Lab[2]/256.0 + 0.5;
    oyStringAddPrintf( &comment, 0,0, "New Temperature: %.00f cie_a: %f cie_b: %f", temperature, cie_a, cie_b );
    fprintf (stderr, "%s %s\n", _("Automatic white point"), comment );
    if(dry == 0)
    {
      oySetDisplayWhitePoint( XYZ, scope, comment );
      pingNativeDisplay();
    }
    oyFree_m_( comment );
  }

  if(wtpt_mode >= 0)
  {
    error = setWtptMode( scope, wtpt_mode, dry );
    return error;
  }


  if(show == 1)
  {
    uint32_t flags = 0;
    int choices = 0, current = -1;
    const char ** choices_string_list = NULL;
    error = oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                 oyNAME_NAME, &choices,
                                 &choices_string_list, &current );
    if(!error)
    {
      fprintf(stderr, "%s\n", _("Choices:"));
      for(i = 0; (int)i < choices; ++i)
      {
        const char * t = "";
        char * tmp = NULL;
        if(i == 1) /* automatic */
        {
          double temperature = getTemperature(0);
          if(temperature)
          {
            oyStringAddPrintf( &tmp, 0,0, "%g %s ", temperature, _("Kelvin") );
            t = tmp;
          }
        }
        printf("-w %u # %s %s%s\n", i, choices_string_list[i], t, (int)i == current ? "*":" ");
        if(tmp) oyFree_m_(tmp);
      }
    }
  }


  if(location)
    error = findLocation(scope, dry);

  if(twilight != -1000)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", twilight);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD OY_SLASH "/twilight", scope, value, NULL );
    oyFree_m_(value);
  }

  if(longitude != 360)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", longitude);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/longitude", scope, value, NULL );
    oyFree_m_(value);
  }

  if(latitude != 360)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", latitude);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/latitude", scope, value, NULL );
    oyFree_m_(value);
  }

  if(wtpt_mode_night != -1)
  {
    oyStringAddPrintf(&value, 0,0, "%d", wtpt_mode_night);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_night", scope, value, NULL );
    oyFree_m_(value);
  }

  if(wtpt_mode_sunlight != -1)
  {
    oyStringAddPrintf(&value, 0,0, "%d", wtpt_mode_sunlight);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_sunlight", scope, value, NULL );
    oyFree_m_(value);
  }

  if(night_effect != NULL && dry == 0)
    oySetPersistentString( OY_DISPLAY_STD "/night_effect", scope,
                           (night_effect[0] && night_effect[0] != '-') ?
                            night_effect : NULL, NULL );

  if(sunlight_effect != NULL && dry == 0)
    oySetPersistentString( OY_DISPLAY_STD "/sunlight_effect", scope,
                           (sunlight_effect[0] && sunlight_effect[0]!='-') ?
                            sunlight_effect : NULL, NULL );

  if(sunrise)
  {
    error = getSunriseSunset( &rise, &set, dry, 1 );
  }

  if(daemon != -1)
    error = runDaemon(daemon);

  if(json)
    puts( oyjlUi_ToJson( ui, 0 ) );
  if(json_command)
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = strdup(jcommands);
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    puts( json_commands );
  }

  if(check)
    checkWtptState( dry );

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  if(oy_debug)
    fprintf(stderr, " %.06g %s\n", DBG_UHR_, oyPrintTime() );
  return error;
}

void pingNativeDisplay()
{
  /* ping X11 observers about option change
   * ... by setting a known property again to its old value
   */
  oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
  oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
  oyOptions_Release( &opts );
}

int setWtptMode( oySCOPE_e scope, int wtpt_mode, int dry )
{
  int choices = 0;
  const char ** choices_string_list = NULL;

  int error = 0;
  
  if(dry == 0)
  {
    oySetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT, scope, wtpt_mode );
    updateVCGT();
  }
  /* ping X11 observers about option change
   * ... by setting a known property again to its old value
   */
  if(!error)
    pingNativeDisplay();
  else
    fprintf(stderr, "error %d in oySetBehaviour(oyBEHAVIOUR_DISPLAY_WHITE_POINT,%d,%d)\n", error, scope, wtpt_mode);

  {
    int current = -1;
    uint32_t flags = 0;
    error = oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                 oyNAME_NAME, &choices,
                                 &choices_string_list, &current );
    if(current == wtpt_mode)
      fprintf (stderr, "%s => %s\n", _("New white point mode"),
               (wtpt_mode<choices && choices_string_list) ? choices_string_list[wtpt_mode] : "----");
    else
      fprintf (stderr, "%s %s => %d != %d\n", _("!!! ERROR"), _("New white point mode"),
               wtpt_mode, current);
    oyOptionChoicesFree( oyWIDGET_DISPLAY_WHITE_POINT, &choices_string_list, choices );
  }

  return error;
}


void updateVCGT()
{
  DBG_S_( oyPrintTime() );
  if(oyDisplayColorServerIsActive()) return;
  size_t size = 0;
  char * result = oyReadCmdToMem_( "oyranos-monitor -l | wc -l", &size, "r", malloc );
  char * tmpname = oyGetTempFileName_( NULL, "vcgt.icc", 0, oyAllocateFunc_ );
  if(!result) return;
  int count = atoi(result), i;
  char * cmd = NULL;
  fprintf(stderr, "monitor count: %d\n", count);
  free(result);

  DBG_S_( oyPrintTime() );
  for(i = 0; i < count; ++i)
  {
    int r OY_UNUSED;
    oyStringAddPrintf(&cmd, 0,0, "oyranos-monitor -c -f vcgt -d %d -o %s", i, tmpname);
    fputs(cmd, stderr); fputs("\n", stderr );
    r = system( cmd );
    oyFree_m_(cmd);
    oyStringAddPrintf(&cmd, 0,0, "oyranos-monitor -g -d %d %s", i, tmpname);
    fputs(cmd, stderr); fputs("\n", stderr );
    r = system( cmd );
    oyFree_m_(cmd);
    //remove( tmpname );
  }
  DBG_S_( oyPrintTime() );
}

double estimateTemperature( double cie_a_, double cie_b_ )
{
  double temperature = 0;

  if(cie_a_ != 0.0 && cie_b_ != 0.0)
  {
    int i;
    for(i = 1; i <= bb_100K[0][2]; ++i)
    {
      double xyz[3] = { bb_100K[i][0], bb_100K[i][1], bb_100K[i][2] };
      double XYZ[3] = { xyz[0]/xyz[1], 1.0, xyz[2]/xyz[1] };
      double Lab[3];
      double cie_a = 0.0, cie_b = 0.0;
      oyXYZ2Lab( XYZ, Lab);

      cie_a = Lab[1]/256.0 + 0.5;
      cie_b = Lab[2]/256.0 + 0.5;
      if(fabs(cie_a - cie_a_) < 0.0001 &&
         fabs(cie_b - cie_b_) < 0.0001)
      {
        temperature = bb_100K[0][0] + i * bb_100K[0][1];
        break;
      }
    }
  }

  return temperature;
}

int findLocation(oySCOPE_e scope, int dry)
{
  int error = 0;

  {
    size_t size = 0;
    char * geo_json = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                                        "http://freegeoip.net/json/", NULL ),
	 * value = NULL;
    oyjl_val root = 0;
    oyjl_val v = 0;
    double lon = 0,
	   lat = 0;

    if(geo_json)
    {
      char * t = oyAllocateFunc_(256);
      root = oyjlTreeParse( geo_json, t, 256 );
      if(t[0])
        WARNc2_S( "%s: %s\n", _("found issues parsing JSON"), t );
      oyFree_m_(t);
      if(oy_debug)
        fprintf(stderr, "%s\n", geo_json);
    } else
      error = 1;

    if(root)
    {
      char * json = NULL;
      int level = 0;
      v = oyjlTreeGetValuef( root, 0, "latitude", NULL );
      value = oyjlValueText( v, oyAllocateFunc_ );
      if(oyStringToDouble( value, &lat ))
        error = 1;
      v = oyjlTreeGetValuef( root, 0, "longitude", NULL );
      value = oyjlValueText( v, oyAllocateFunc_ );
      if(oyStringToDouble( value, &lon ))
        error = 1;
      oyFree_m_(value);
      oyjlTreeToJson( root, &level, &json );

      if(lat != 0.0 && lon != 0.0)
      {
        #define PRINT_VAL( key, name )\
        v = oyjlTreeGetValuef( root, 0, key, NULL ); \
        value = oyjlValueText( v, oyAllocateFunc_ ); \
	if(value) \
        { if(value[0]) \
            fprintf( stderr, "%s: %s\n", name, value ); \
          oyFree_m_(value); \
        }
        PRINT_VAL( "time_zone", _("Time Zone") )
        PRINT_VAL( "country_name", _("Country") )
        PRINT_VAL( "city", _("City") )
        #undef PRINT_VAL

#ifdef HAVE_LOCALE_H
        char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
        setlocale(LC_NUMERIC, "C");
#endif
        printf( "%g %g\n", lat,lon);

        oyStringAddPrintfC(&value, 0,0, "%g", lat);
        if(dry == 0)
          oySetPersistentString( OY_DISPLAY_STD "/latitude", scope, value, NULL );
        oyFree_m_(value);
        oyStringAddPrintfC(&value, 0,0, "%g", lon);
        if(dry == 0)
          oySetPersistentString( OY_DISPLAY_STD "/longitude", scope, value, NULL );
        oyFree_m_(value);

#ifdef HAVE_LOCALE_H
        setlocale(LC_NUMERIC, save_locale);
        if(save_locale)
          free( save_locale );
#endif
      } else
        error = 1;

      oyjlTreeFree(root);
    } else
      error = 1;
  }

  return error;
}

int getLocation( double * lon, double * lat)
{
  int need_location = 0;
  char * value = NULL;
 
  value = oyGetPersistentString( OY_DISPLAY_STD "/latitude", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  if(value && oyStringToDouble( value, lat ))
    fprintf(stderr, "lat = %g / %s\n", *lat, value);
  if(value)
  {
    oyFree_m_(value);
  } else
    need_location = 1;
  value = oyGetPersistentString( OY_DISPLAY_STD "/longitude", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  if(value && oyStringToDouble( value, lon ))
    fprintf(stderr, "lon = %g / %s\n", *lon, value);
  if(value)
  {
    oyFree_m_(value);
  } else
    need_location = 1;

  return need_location;
}

double oyNormaliseHour(double hour)
{
  if(hour > 24.0)
    return hour - 24.0;
  if(hour < 0.0)
    return hour + 24.0;
  else
    return hour;
}

#define oyGetCurrentGMTHour_(arg) ((hour_ != -1.0) ? hour_ + oyGetCurrentGMTHour(arg)*0.0 : oyGetCurrentGMTHour(arg))
int getSunriseSunset( double * rise, double * set, int dry, int verbose )
{
  double lat = 0.0,
         lon = 0.0;
  double twilight = 0;
  int year,month,day;
  int r;
  char * value = NULL;
 
  if(getLocation(&lon, &lat))
  {
    findLocation( scope, dry );
    getLocation(&lon, &lat);
  }

  {
    time_t  cutime;         /* Time since epoch */
    struct tm       *gmt;
    char time_str[24];

    cutime = time(NULL); /* time right NOW */
    gmt = localtime(&cutime);
    strftime(time_str, 24, "%Y", gmt);
    year  = strtol(time_str,NULL,10);
    strftime(time_str, 24, "%m", gmt);
    month = strtol(time_str,NULL,10);
    strftime(time_str, 24, "%d", gmt);
    day   = strtol(time_str,NULL,10);
  }

  value =
      oyGetPersistentString(OY_DISPLAY_STD "/twilight", 0, oySCOPE_USER_SYS, oyAllocateFunc_);
  if(value && oyStringToDouble( value, &twilight ))
    fprintf(stderr, "twilight = %g / %s\n", twilight, value);
  if(value)
  { oyFree_m_(value);
  }

  r = __sunriset__( year,month,day, lon,lat,
                    (twilight==0)?-35.0/60.0:twilight, 0, rise, set );
  if(r > 0)
    fprintf(stderr, "sun will not get below twilight today\n");
  if(r < 0)
    fprintf(stderr, "sun will not get above twilight today\n");
  {
    int hour, minute, second, gmt_diff_second;
    double elevation;

    oyGetCurrentGMTHour_( &gmt_diff_second );
    oySplitHour( oyGetCurrentLocalHour( oyGetCurrentGMTHour_(0), gmt_diff_second ), &hour, &minute, &second );
    elevation = getSunHeight( year, month, day, oyGetCurrentGMTHour_(0), lat, lon );
    if(verbose)
      fprintf( stdout, "%d-%d-%d %d:%.2d:%.2d",
             year, month, day, hour, minute, second );
    oySplitHour( oyGetCurrentLocalHour( *rise, gmt_diff_second ), &hour, &minute, &second );
    if(verbose)
      fprintf( stdout, " %s: %g° %g° %s: %g° (%s: %g°) %s: %d:%.2d:%.2d",
             _("Geographical Position"), lat, lon, _("Twilight"), twilight, _("Sun Elevation"), elevation, _("Sunrise"), hour, minute, second );
    oySplitHour( oyGetCurrentLocalHour( *set,  gmt_diff_second ), &hour, &minute, &second );
    if(verbose)
      fprintf( stdout, " %s: %d:%.2d:%.2d\n",
             _("Sunset"), hour, minute, second );
  }

  return r;
}

int isNight(int dry)
{
  int    diff;
  double dtime, rise, set;

  dtime = oyGetCurrentGMTHour_(&diff);

  if( getSunriseSunset( &rise, &set, dry, 0 ) == 0 )
  {
    if(rise < dtime && dtime <= set)
    /* day time */
      return 0;
    else
      return 1;
  }
  return -1;
}

/* check the sunrise / sunset state */
int checkWtptState(int dry)
{
  int error = 0;
  int cmode;
  char * effect = NULL;

  int    diff;
  double dtime, rise, set;

  int choices = 0;
  const char ** choices_string_list = NULL;
  char * value;
  int active = 1;

  DBG_S_( oyPrintTime() );

  /* settings changed on disk, need to reread */
  oyGetPersistentStrings( NULL );
  DBG_S_( oyPrintTime() );

  value = oyGetPersistentString(OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0,
                                oySCOPE_USER_SYS, oyAllocateFunc_);
  if(!value || strcmp(value,"oyranos-monitor-white-point") != 0)
    active = 0;
  if(value)
  { oyFree_m_(value);
  }
  if(!active)
    return -1;


  dtime = oyGetCurrentGMTHour_(&diff);

  DBG_S_( oyPrintTime() );
  {
    int current = -1;
    uint32_t flags = 0;
    error = oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                 oyNAME_NAME, &choices,
                                 &choices_string_list, &current );
  }

  DBG_S_( oyPrintTime() );
  if( choices_string_list && getSunriseSunset( &rise, &set, dry, 0 ) == 0 )
  {
    int new_mode = -1;
    char * new_effect = NULL;
    int use_effect = 0;

    cmode = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
    effect = oyGetPersistentString( OY_DEFAULT_EFFECT_PROFILE, 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
    fprintf (stderr, "%s: %s %s: %s\n", _("Actual white point mode"), cmode<choices?choices_string_list[cmode]:"----",
            _("Effect"), oyNoEmptyString_m_(effect));

    if(rise < dtime && dtime <= set)
    /* day time */
    {
      char * value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_sunlight", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(value)
      {
        new_mode = atoi(value);
        oyFree_m_(value);
      } else /* defaut to D65 for daylight */
        new_mode = 4;

      new_effect = oyGetPersistentString( OY_DISPLAY_STD "/sunlight_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(new_effect)
        ++use_effect;
      else
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/night_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value)
        {
          ++use_effect;
          oyFree_m_(value);
        }
      }
      if(dry == 0)
        if(!oyExistPersistentString( OY_DISPLAY_STD "/night", "0", 0, oySCOPE_USER_SYS ))
          oySetPersistentString( OY_DISPLAY_STD "/night", scope, "0", NULL );

    } else
    /* night time */
    {
      char * value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_night", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(value)
      {
        new_mode = atoi(value);
        oyFree_m_(value);
      } else /* defaut to D50 for night light */
        new_mode = 2;

      new_effect = oyGetPersistentString( OY_DISPLAY_STD "/night_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(new_effect)
        ++use_effect;
      else
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/sunlight_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value)
        {
          ++use_effect;
          oyFree_m_(value);
        }
      }
      if(dry == 0)
        if(!oyExistPersistentString( OY_DISPLAY_STD "/night", "1", 0, oySCOPE_USER_SYS ))
          oySetPersistentString( OY_DISPLAY_STD "/night", scope, "1", NULL );
    }

    if( (new_mode != cmode) ||
        ((effect?1:0) != (new_effect?1:0) ||
         (effect && new_effect && strcmp(effect, new_effect) != 0)))
    {
      fprintf(  stderr, "%s: %s %s: %s\n", _("New white point mode"), new_mode<choices?choices_string_list[new_mode]:"----",
                _("Effect"), oyNoEmptyString_m_(new_effect) );

      if(dry == 0 && use_effect)
      {
        if(!new_effect || (strcmp(new_effect,"-") == 0 ||
                           strlen(new_effect) == 0 ))
        {
          oySetPersistentString( OY_DEFAULT_EFFECT_PROFILE, scope, NULL, NULL );
          oySetPersistentString( OY_DEFAULT_EFFECT, scope, "0", NULL );
        } else
        {
          oySetPersistentString( OY_DEFAULT_EFFECT_PROFILE, scope, new_effect, NULL );
          oySetPersistentString( OY_DEFAULT_EFFECT, scope, "1", NULL );
        }
      }

      if(cmode != new_mode)
        error = setWtptMode( scope, new_mode, dry );
    }

    if(effect) oyFree_m_(effect);
    if(new_effect) oyFree_m_(new_effect);
  }

  DBG_S_( oyPrintTime() );
  oyOptionChoicesFree( oyWIDGET_DISPLAY_WHITE_POINT, &choices_string_list, choices );

  return error;
}

#ifdef HAVE_DBUS
#include "oyranos_dbus_macros.h"
oyDBusFilter_m
oyWatchDBus_m( oyDBusFilter )
oyFinishDBus_m
int oy_dbus_config_changed = 0;
static void oyMonitorCallbackDBus    ( double              progress_zero_till_one OY_UNUSED,
                                       char              * status_text,
                                       int                 thread_id_ OY_UNUSED,
                                       int                 job_id OY_UNUSED,
                                       oyStruct_s        * cb_progress_context )
{
  const char * key;
  int verbose = oyOption_GetValueInt( (oyOption_s*)cb_progress_context, 0 );
  if(!status_text) return;
  DBG_S_( oyPrintTime() );

  oyGetPersistentStrings(NULL);

  key = strchr( status_text, '/' );
  if(key)
    ++key;
  else
    return;
  if(!verbose) return;

  if( strstr(key, OY_STD "/ping") == NULL && /* let us ping */
      strstr(key, OY_DISPLAY_STD) == NULL && /* all display variables */
      strstr(key, OY_DEFAULT_DISPLAY_WHITE_POINT) == NULL && /* oySetDisplayWhitePoint() */
      strstr(key, OY_DEFAULT_EFFECT) == NULL && /* effect switch changes */
      strstr(key, OY_DEFAULT_EFFECT_PROFILE) == NULL /* new effect profile */
    )
    return;
  if( /* skip XY(Z) and listen only on Z to reduce flicker */
      strstr(key, OY_DEFAULT_DISPLAY_WHITE_POINT_X) != NULL ||
      strstr(key, OY_DEFAULT_DISPLAY_WHITE_POINT_Y) != NULL || 
      /* skip XY(Z) with elektra style array indixes */
      strstr(key, OY_DISPLAY_STD OY_SLASH "display_white_point_XYZ/#0") != NULL ||
      strstr(key, OY_DISPLAY_STD OY_SLASH "display_white_point_XYZ/#1") != NULL
    )
    return;

  char * v = oyGetPersistentString( key, 0, scope, oyAllocateFunc_ );
  if(!v)
    fprintf(stderr, "%s: no value\n", key );
  else
  {
    fprintf(stderr, "%s:%s\n", key,v);
    oyFree_m_(v);
  }

  checkWtptState( 0 );
  updateVCGT();

  /* Clear the changed state, before a new check. */
  oy_dbus_config_changed = 1;
  DBG_S_( oyPrintTime() );
}
#endif /* HAVE_DBUS */


int runDaemon(int dmode)
{
  int error = 0, id, active = 1;
  double hour_old = 0.0;

  if(dmode == 0) /* stop service */
  {
    /* erase the key */
    oySetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, scope, NULL, NULL );
    active = 0; 
    return error;
  }
  else
  if(dmode == 1) /* check if service is desired */
  {
    char * value = 
      oyGetPersistentString(OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, 0,
		            oySCOPE_USER_SYS, oyAllocateFunc_);
    if(!value || strcmp(value,"oyranos-monitor-white-point") != 0)
      active = 0;

    oyFree_m_(value);
  }
  else /* dmode >= 2 => start service */
    oySetPersistentString( OY_DEFAULT_DISPLAY_WHITE_POINT_DAEMON, scope, "oyranos-monitor-white-point", "CLI Daemon" );

  /* ensure all keys are setup properly
   * before we lock the DBus connection by listening */
  if(active)
    checkWtptState( 0 );

#ifdef HAVE_DBUS
  oyStartDBusObserver( oyWatchDBus, oyFinishDBus, oyMonitorCallbackDBus, OY_DISPLAY_STD, NULL )
  if(id)
    fprintf(stderr, "oyStartDBusObserver ID: %d\n", id);

  while(1)
  {
    double hour = oyGetCurrentGMTHour_( 0 );
    double repeat_check = 1.0/60.0; /* every minute */

    oyLoopDBusObserver( hour, repeat_check, oy_dbus_config_changed, checkWtptState(0) )

    /* delay next polling */
    oySleep( 0.25 );
  }

  return error;
#endif /* HAVE_DBUS */
}


/*  ---------------- 8< ------------------ */
/*#include "sunriset.h"*/
extern const char* timezone_name;
extern long int timezone_offset;

#define TMOD(x) ((x)<0?(x)+24:((x)>=24?(x)-24:(x)))
#define DAYSOFF(x) ((x)<0?"(-1) ":((x)>=24?"(+1) ":""))

#define HOURS(h) ((int)(floor(h)))
#define MINUTES(h) ((int)(60*(h-floor(h))))

#define ABSSS(x) ((x)<0?-(x):(x))

/* A macro to compute the number of days elapsed since 2000 Jan 0.0 */
/* (which is equal to 1999 Dec 31, 0h UT)                           */
/* Dan R sez: This is some pretty fucking high magic. */
#define days_since_2000_Jan_0(y,m,d) \
    (367L*(y)-((7*((y)+(((m)+9)/12)))/4)+((275*(m))/9)+(d)-730530L)

/* Some conversion factors between radians and degrees */

#ifndef PI
 #define PI        3.1415926535897932384
#endif

#define RADEG     ( 180.0 / PI )
#define DEGRAD    ( PI / 180.0 )

/* The trigonometric functions in degrees */

#define sind(x)  sin((x)*DEGRAD)
#define cosd(x)  cos((x)*DEGRAD)
#define tand(x)  tan((x)*DEGRAD)

#define atand(x)    (RADEG*atan(x))
#define asind(x)    (RADEG*asin(x))
#define acosd(x)    (RADEG*acos(x))
#define atan2d(y,x) (RADEG*atan2(y,x))

/* Following are some macros around the "workhorse" function __daylen__ */
/* They mainly fill in the desired values for the reference altitude    */
/* below the horizon, and also selects whether this altitude should     */
/* refer to the Sun's center or its upper limb.                         */


/* This macro computes the length of the day, from sunrise to sunset. */
/* Sunrise/set is considered to occur when the Sun's upper limb is    */
/* 50 arc minutes below the horizon (this accounts for the refraction */
/* of the Earth's atmosphere).                                        */
/* The original version of the program used the value of 35 arc mins, */
/* which is the accepted value in Sweden.                             */
#define day_length(year,month,day,lon,lat)  \
        __daylen__( year, month, day, lon, lat, -50.0/60.0, 1 )

/* This macro computes the length of the day, including civil twilight. */
/* Civil twilight starts/ends when the Sun's center is 6 degrees below  */
/* the horizon.                                                         */
#define day_civil_twilight_length(year,month,day,lon,lat)  \
        __daylen__( year, month, day, lon, lat, -6.0, 0 )

/* This macro computes the length of the day, incl. nautical twilight.  */
/* Nautical twilight starts/ends when the Sun's center is 12 degrees    */
/* below the horizon.                                                   */
#define day_nautical_twilight_length(year,month,day,lon,lat)  \
        __daylen__( year, month, day, lon, lat, -12.0, 0 )

/* This macro computes the length of the day, incl. astronomical twilight. */
/* Astronomical twilight starts/ends when the Sun's center is 18 degrees   */
/* below the horizon.                                                      */
#define day_astronomical_twilight_length(year,month,day,lon,lat)  \
        __daylen__( year, month, day, lon, lat, -18.0, 0 )


/* This macro computes times for sunrise/sunset.                      */
/* Sunrise/set is considered to occur when the Sun's upper limb is    */
/* 35 arc minutes below the horizon (this accounts for the refraction */
/* of the Earth's atmosphere).                                        */
#define sun_rise_set(year,month,day,lon,lat,rise,set)  \
        __sunriset__( year, month, day, lon, lat, -35.0/60.0, 1, rise, set )

/* This macro computes the start and end times of civil twilight.       */
/* Civil twilight starts/ends when the Sun's center is 6 degrees below  */
/* the horizon.                                                         */
#define civil_twilight(year,month,day,lon,lat,start,end)  \
        __sunriset__( year, month, day, lon, lat, -6.0, 0, start, end )

/* This macro computes the start and end times of nautical twilight.    */
/* Nautical twilight starts/ends when the Sun's center is 12 degrees    */
/* below the horizon.                                                   */
#define nautical_twilight(year,month,day,lon,lat,start,end)  \
        __sunriset__( year, month, day, lon, lat, -12.0, 0, start, end )

/* This macro computes the start and end times of astronomical twilight.   */
/* Astronomical twilight starts/ends when the Sun's center is 18 degrees   */
/* below the horizon.                                                      */
#define astronomical_twilight(year,month,day,lon,lat,start,end)  \
        __sunriset__( year, month, day, lon, lat, -18.0, 0, start, end )


/* Function prototypes */

double __daylen__( int year, int month, int day, double lon, double lat,
                   double altit, int upper_limb );

int __sunriset__( int year, int month, int day, double lon, double lat,
                  double altit, int upper_limb, double *rise, double *set );

void sunpos( double d, double *lon, double *r );

void sun_RA_dec( double d, double *RA, double *dec, double *r );

double revolution( double x );

double rev180( double x );

double GMST0( double d );

/*
SUNRISET.C - computes Sun rise/set times, start/end of twilight, and
             the length of the day at any date and latitude
Written as DAYLEN.C, 1989-08-16
Modified to SUNRISET.C, 1992-12-01
(c) Paul Schlyter, 1989, 1992
Released to the public domain by Paul Schlyter, December 1992
*/


#include <stdio.h>
#include <math.h>
#include <time.h>

/*#include "sunriset.h"*/

/* The "workhorse" function for sun rise/set times */

int __sunriset__( int year, int month, int day, double lon, double lat,
                  double altit, int upper_limb, double *trise, double *tset )
/***************************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value IS critical in this function!            */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing rise/set     */
/*               times, and to zero when computing start/end of       */
/*               twilight.                                            */
/*        *rise = where to store the rise time                        */
/*        *set  = where to store the set  time                        */
/*                Both times are relative to the specified altitude,  */
/*                and thus this function can be used to comupte       */
/*                various twilight times, as well as rise/set times   */
/* Return value:  0 = sun rises/sets this day, times stored at        */
/*                    *trise and *tset.                               */
/*               +1 = sun above the specified "horizon" 24 hours.     */
/*                    *trise set to time when the sun is at south,    */
/*                    minus 12 hours while *tset is set to the south  */
/*                    time plus 12 hours. "Day" length = 24 hours     */
/*               -1 = sun is below the specified "horizon" 24 hours   */
/*                    "Day" length = 0 hours, *trise and *tset are    */
/*                    both set to the time when the sun is at south.  */
/*                                                                    */
/**********************************************************************/
{
      double  d,  /* Days since 2000 Jan 0.0 (negative before) */
      sr,         /* Solar distance, astronomical units */
      sRA,        /* Sun's Right Ascension */
      sdec,       /* Sun's declination */
      sradius,    /* Sun's apparent radius */
      t,          /* Diurnal arc */
      tsouth,     /* Time when Sun is at south */
      sidtime;    /* Local sidereal time */

      int rc = 0; /* Return cde from function - usually 0 */

      /* Compute d of 12h local mean solar time */
      d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

      /* Compute local sideral time of this moment */
      sidtime = revolution( GMST0(d) + 180.0 + lon );

      /* Compute Sun's RA + Decl at this moment */
      sun_RA_dec( d, &sRA, &sdec, &sr );

      /* Compute time when Sun is at south - in hours UT */
      tsouth = 12.0 - rev180(sidtime - sRA)/15.0;

      /* Compute the Sun's apparent radius, degrees */
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitide altit: */
      {
            double cost;
            cost = ( sind(altit) - sind(lat) * sind(sdec) ) /
                  ( cosd(lat) * cosd(sdec) );
            if ( cost >= 1.0 )
                  rc = -1, t = 0.0;       /* Sun always below altit */
            else if ( cost <= -1.0 )
                  rc = +1, t = 12.0;      /* Sun always above altit */
            else
                  t = acosd(cost)/15.0;   /* The diurnal arc, hours */
      }

      /* Store rise and set times - in hours UT */
      *trise = tsouth - t;
      *tset  = tsouth + t;

      return rc;
}  /* __sunriset__ */



/* The "workhorse" function */


double __daylen__( int year, int month, int day, double lon, double lat,
                   double altit, int upper_limb )
/**********************************************************************/
/* Note: year,month,date = calendar date, 1801-2099 only.             */
/*       Eastern longitude positive, Western longitude negative       */
/*       Northern latitude positive, Southern latitude negative       */
/*       The longitude value is not critical. Set it to the correct   */
/*       longitude if you're picky, otherwise set to to, say, 0.0     */
/*       The latitude however IS critical - be sure to get it correct */
/*       altit = the altitude which the Sun should cross              */
/*               Set to -35/60 degrees for rise/set, -6 degrees       */
/*               for civil, -12 degrees for nautical and -18          */
/*               degrees for astronomical twilight.                   */
/*         upper_limb: non-zero -> upper limb, zero -> center         */
/*               Set to non-zero (e.g. 1) when computing day length   */
/*               and to zero when computing day+twilight length.      */
/**********************************************************************/
{
      double  d,  /* Days since 2000 Jan 0.0 (negative before) */
      obl_ecl,    /* Obliquity (inclination) of Earth's axis */
      sr,         /* Solar distance, astronomical units */
      slon,       /* True solar longitude */
      sin_sdecl,  /* Sine of Sun's declination */
      cos_sdecl,  /* Cosine of Sun's declination */
      sradius,    /* Sun's apparent radius */
      t;          /* Diurnal arc */

      /* Compute d of 12h local mean solar time */
      d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0;

      /* Compute obliquity of ecliptic (inclination of Earth's axis) */
      obl_ecl = 23.4393 - 3.563E-7 * d;

      /* Compute Sun's position */
      sunpos( d, &slon, &sr );

      /* Compute sine and cosine of Sun's declination */
      sin_sdecl = sind(obl_ecl) * sind(slon);
      cos_sdecl = sqrt( 1.0 - sin_sdecl * sin_sdecl );

      /* Compute the Sun's apparent radius, degrees */
      sradius = 0.2666 / sr;

      /* Do correction to upper limb, if necessary */
      if ( upper_limb )
            altit -= sradius;

      /* Compute the diurnal arc that the Sun traverses to reach */
      /* the specified altitide altit: */
      {
            double cost;
            cost = ( sind(altit) - sind(lat) * sin_sdecl ) /
                  ( cosd(lat) * cos_sdecl );
            if ( cost >= 1.0 )
                  t = 0.0;                      /* Sun always below altit */
            else if ( cost <= -1.0 )
                  t = 24.0;                     /* Sun always above altit */
            else  t = (2.0/15.0) * acosd(cost); /* The diurnal arc, hours */
      }
      return t;
}  /* __daylen__ */


/* This function computes the Sun's position at any instant */

void sunpos( double d, double *lon, double *r )
/******************************************************/
/* Computes the Sun's ecliptic longitude and distance */
/* at an instant given in d, number of days since     */
/* 2000 Jan 0.0.  The Sun's ecliptic latitude is not  */
/* computed, since it's always very near 0.           */
/******************************************************/
{
      double M,         /* Mean anomaly of the Sun */
             w,         /* Mean longitude of perihelion */
                        /* Note: Sun's mean longitude = M + w */
             e,         /* Eccentricity of Earth's orbit */
             E,         /* Eccentric anomaly */
             x, y,      /* x, y coordinates in orbit */
             v;         /* True anomaly */

      /* Compute mean elements */
      M = revolution( 356.0470 + 0.9856002585 * d );
      w = 282.9404 + 4.70935E-5 * d;
      e = 0.016709 - 1.151E-9 * d;

      /* Compute true longitude and radius vector */
      E = M + e * RADEG * sind(M) * ( 1.0 + e * cosd(M) );
      x = cosd(E) - e;
      y = sqrt( 1.0 - e*e ) * sind(E);
      *r = sqrt( x*x + y*y );              /* Solar distance */
      v = atan2d( y, x );                  /* True anomaly */
      *lon = v + w;                        /* True solar longitude */
      if ( *lon >= 360.0 )
            *lon -= 360.0;                   /* Make it 0..360 degrees */
}

void sun_RA_dec( double d, double *RA, double *dec, double *r )
{
  double lon, obl_ecl;
  double xs, ys, zs OY_UNUSED;
  double xe, ye, ze;
  
  /* Compute Sun's ecliptical coordinates */
  sunpos( d, &lon, r );
  
  /* Compute ecliptic rectangular coordinates */
  xs = *r * cosd(lon);
  ys = *r * sind(lon);
  zs = 0; /* because the Sun is always in the ecliptic plane! */

  /* Compute obliquity of ecliptic (inclination of Earth's axis) */
  obl_ecl = 23.4393 - 3.563E-7 * d;
  
  /* Convert to equatorial rectangular coordinates - x is unchanged */
  xe = xs;
  ye = ys * cosd(obl_ecl);
  ze = ys * sind(obl_ecl);
  
  /* Convert to spherical coordinates */
  *RA = atan2d( ye, xe );
  *dec = atan2d( ze, sqrt(xe*xe + ye*ye) );
      
}  /* sun_RA_dec */


/******************************************************************/
/* This function reduces any angle to within the first revolution */
/* by subtracting or adding even multiples of 360.0 until the     */
/* result is >= 0.0 and < 360.0                                   */
/******************************************************************/

#define INV360    ( 1.0 / 360.0 )

double revolution( double x )
/*****************************************/
/* Reduce angle to within 0..360 degrees */
/*****************************************/
{
      return( x - 360.0 * floor( x * INV360 ) );
}  /* revolution */

double rev180( double x )
/*********************************************/
/* Reduce angle to within -180..+180 degrees */
/*********************************************/
{
      return( x - 360.0 * floor( x * INV360 + 0.5 ) );
}  /* revolution */


/*******************************************************************/
/* This function computes GMST0, the Greenwhich Mean Sidereal Time */
/* at 0h UT (i.e. the sidereal time at the Greenwhich meridian at  */
/* 0h UT).  GMST is then the sidereal time at Greenwich at any     */
/* time of the day.  I've generelized GMST0 as well, and define it */
/* as:  GMST0 = GMST - UT  --  this allows GMST0 to be computed at */
/* other times than 0h UT as well.  While this sounds somewhat     */
/* contradictory, it is very practical:  instead of computing      */
/* GMST like:                                                      */
/*                                                                 */
/*  GMST = (GMST0) + UT * (366.2422/365.2422)                      */
/*                                                                 */
/* where (GMST0) is the GMST last time UT was 0 hours, one simply  */
/* computes:                                                       */
/*                                                                 */
/*  GMST = GMST0 + UT                                              */
/*                                                                 */
/* where GMST0 is the GMST "at 0h UT" but at the current moment!   */
/* Defined in this way, GMST0 will increase with about 4 min a     */
/* day.  It also happens that GMST0 (in degrees, 1 hr = 15 degr)   */
/* is equal to the Sun's mean longitude plus/minus 180 degrees!    */
/* (if we neglect aberration, which amounts to 20 seconds of arc   */
/* or 1.33 seconds of time)                                        */
/*                                                                 */
/*******************************************************************/

double GMST0( double d )
{
      double sidtim0;
      /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
      /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
      /* add these numbers, I'll let the C compiler do it for me.  */
      /* Any decent C compiler will add the constants at compile   */
      /* time, imposing no runtime or code overhead.               */
      sidtim0 = revolution( ( 180.0 + 356.0470 + 282.9404 ) +
                          ( 0.9856002585 + 4.70935E-5 ) * d );
      return sidtim0;
} /* GMST0 */
/*  ---------------- 8< ------------------ */

double getSunHeight( double year, double month, double day, double gmt_hours,
                     double lat, double lon )
{
  double  d = days_since_2000_Jan_0(year,month,day) + 0.5 - lon/360.0,
      sr,         /* Solar distance, astronomical units */
      sdec,       /* Sun's declination */
      sRA,        /* Sun's Right Ascension */
      sidtime,    /* Local sidereal time */
      t,          /* Sun's local hour angle */
      hs,         /* Sun's Height sinus */
      h,          /* Sun's Height */
      A;          /* Sun's Azimut */

  int hour,minute,second;
  oySplitHour( oyGetCurrentLocalHour( gmt_hours, 0 ), &hour, &minute, &second );
  if(oy_debug)
  fprintf( stderr, "GMT:\t%02d:%02d:%02d\n", hour,minute,second );
  if(oy_debug)
  fprintf( stderr, "JD (GMT 12:00):\t%fd\n", d + 2451545.0 + lon/360.0 );
  if(oy_debug)
  fprintf( stderr, "JD0 (GMT 12:00):\t%fd\n", d + lon/360.0 );
  if(oy_debug)
  fprintf( stderr, "LMST:\t%fd\n", d + gmt_hours/24.0*365.25/366.25 );

  /* Compute local sideral time of this moment */
  sidtime = revolution( GMST0(d) + 360.*gmt_hours/24.*365.25/366.25 + lon );
  oySplitHour( oyGetCurrentLocalHour( sidtime/15., 0 ), &hour, &minute, &second );
  if(oy_debug)
  fprintf( stderr, "Local Mean Sidereal Time:\t%02d:%02d:%02d\n", hour,minute,second );

  sun_RA_dec( d, &sRA, &sdec, &sr );
  if(oy_debug)
  fprintf( stderr, "Rectaszension:\t%g°\n", sRA);
  if(oy_debug)
  fprintf( stderr, "Declination:\t%g°\n", sdec);
  t = sidtime - sRA;
  if(oy_debug)
  fprintf( stderr, "Sun's Hourly Angle:\t%g° (%gh)\n", t, t/15.);
  A = atand( sind( t ) /
             ( cosd( t )*sind( lon ) - tand( sdec )*cosd( lon ) )
           );
  if(oy_debug)
  fprintf( stderr, "Sun's Azimut:\t%g°\n", revolution(A-180.0) );
  hs = cosd( sdec )*cosd( t )*cosd( lat ) + sind( sdec )*sind( lat );
  h = asind( hs );
  if(oy_debug)
  fprintf( stderr, "Sun's Height:\t%g° sin(%g)\n", h, hs );

  return h;
}
