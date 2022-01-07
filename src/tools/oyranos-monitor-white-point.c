/** @file oyranos-monitor-white-point.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2017-2022  Kai-Uwe Behrmann
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

#include <stdio.h>                /* popen() */
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* usleep() */

#include "oyranos_color.h"
#include "oyranos_color_internal.h"
#include "oyranos_conversion.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_json.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "oyranos_threads.h"
#include "oyProfiles_s.h"

#include <X11/Xcm/XcmVersion.h>

#define DBG_S_ if(oy_debug >= 1)DBG_S
#define DBG1_S_ if(oy_debug >= 1)DBG1_S
#define DBG2_S_ if(oy_debug >= 1)DBG2_S
int __sunriset__( int year, int month, int day, double lon, double lat,
                  double altit, int upper_limb, double *trise, double *tset );
int findLocation(oySCOPE_e scope, int dry);
int getLocation( double * lon, double * lat);
double getSunHeight( double year, double month, double day, double gmt_hours, double lat, double lon, int verbose );
int getSunriseSunset( double * rise, double * set, int dry, char ** text, int verbose );
int isNight(int dry);
int runDaemon(int dmode);
int setWtptMode( oySCOPE_e scope, int wtpt_mode, int dry );
void pingNativeDisplay();
int checkWtptState();
void updateVCGT();
// Dark/Bright Desktop backgrounds
char * getCurrentColorScheme();
void setColorScheme( const char * scheme );
char ** listColorSchemes( int * count );

#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)

static uint32_t icc_profile_flags = 0;
static oyjlOptionChoice_s * linear_effect_choices_ = NULL;
static oyjlOptionChoice_s * getLinearEffectProfileChoices (
                                                  oyjlOption_s   * o,
                                                  int               * selected,
                                                  oyjlOptions_s  * opts OY_UNUSED )
{
    int choices = 0;
    oyProfiles_s * patterns = oyProfiles_New( NULL ),
                 * profiles = 0;
    const char * man_page = getenv("DISPLAY");

    if(!selected && linear_effect_choices_)
      return linear_effect_choices_;

    if(man_page && strcmp(man_page,"man_page") == 0)
    {
      oyjlOptionChoice_s * c = calloc(1+2+1, sizeof(oyjlOptionChoice_s));
      if(c)
      {
        /* first entry is for unsetting */
        int i = 0;
        c[i].nick = strdup("-");
        c[i].name = strdup(_("[none]"));
        c[i].description = strdup("");
        c[i].help = strdup("");

        /* example profile choices */
        c[++i].nick = strdup("Effect-1");
        c[i].name = strdup("Example Effect 1");
        c[i].description = strdup("ICC profile of class abstract, wich affects gamma");
        c[i].help = strdup("");

        c[++i].nick = strdup("Effect-2");
        c[i].name = strdup("Example Effect 2");
        c[i].description = strdup("ICC profile of class abstract, wich affects gamma");
        c[i].help = strdup("");

        /* empty choice for end of list */
        c[++i].nick = malloc(4);
        c[i].nick[0] = '\000';
      }
      return c;
    }
    else
    {
      int i;
      char * value = NULL;
      oyjlOptionChoice_s * c;

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
      if(!choices)
      {
        oyProfiles_Release( &profiles );
        return NULL;
      }

      c = calloc(1+choices+1, sizeof(oyjlOptionChoice_s));

      if(OYJL_IS_O("g") && selected)
        value = oyGetPersistentString( OY_DISPLAY_STD "/night_effect", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      else if(OYJL_IS_O("e") && selected)
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
          const char * nick = oyProfile_GetFileName( p, -1 );

          c[i+1].nick = strdup(nick);
          c[i+1].name = strdup(oyProfile_GetText( p, oyNAME_DESCRIPTION ));
          c[i+1].description = strdup("");
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
    }
    return NULL;
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
#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)
#define OYJL_IS_OPTION( x ) (o->option && strcmp(o->option,x) == 0)
static oyjlOptionChoice_s * white_point_choices_ = NULL;
static int white_point_choices_selected_ = -1;
oyjlOptionChoice_s * getWhitePointChoices       ( oyjlOption_s      * o,
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
      const char * man_page = getenv("DISPLAY");
      int skip_temperature_info = man_page && strcmp(man_page,"man_page") == 0;
      oyjlOptionChoice_s * c = calloc(choices+1, sizeof(oyjlOptionChoice_s));
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

          if(i == 1 && !skip_temperature_info) /* automatic */
          {
            double temperature = oyGetTemperature(0);
            if(temperature)
              oyStringAddPrintf( &c[i].name, 0,0, " %g %s ", temperature, _("Kelvin") );
          }
          if((int)i == current && OYJL_IS_O("w") && 0 /* not possible, as the result is used over different options */)
            oyStringAddPrintf( &c[i].name, 0,0, " *" );
        }
        c[i].nick = malloc(4);
        c[i].nick[0] = '\000';
      }
      oyOptionChoicesFree( oyWIDGET_DISPLAY_WHITE_POINT, &choices_string_list, choices );
      white_point_choices_selected_ = current;
      if(selected)
        *selected = current;
      if(OYJL_IS_O("n") && selected)
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_night", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value && oyjlStringToLong( value, &l ) <= 0)
          *selected = l;
      } else if(OYJL_IS_O("s") && selected)
      {
        value = oyGetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_sunlight", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
        if(value && oyjlStringToLong( value, &l ) <= 0)
          *selected = l;
      }

      return c;
    } else
      return NULL;
}

char ** listColorSchemes( int * count )
{
  int n = 0, size = 0, i;
  char ** list, ** result_list = NULL;
  char * result = oyjlReadCommandF( &size, "r", malloc, "LANG=C plasma-apply-colorscheme --list-schemes" );
  list = oyjlStringSplit( result, '\n', &n, 0 );

  *count = 0;
  if(list)
  {
    for(i = 0; i < n; ++i)
    {
      char * t = list[i];
      if(strstr(t, "You have the"))
        continue;
      if(strchr(t, '*') != NULL)
        t = strchr(t, '*') + 1;
      if(t[0] == ' ')
        ++t;
      if(strstr(t, " (current color scheme)"))
      {
        char * txt = strstr(t, " (current color scheme)");
        txt[0] = '\000';
      }

      oyjlStringListAddString( &result_list, count, t, oyAllocateFunc_, oyDeAllocateFunc_ );
    }
    oyjlStringListRelease( &list, n, free );
  }

  return result_list;
}
static oyjlOptionChoice_s * getColorSchemeChoices (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts OY_UNUSED )
{
  const char * man_page = getenv("DISPLAY");
  int i = 0, n = 0;
  char ** list;
  char * value;
  int night;
  oyjlOptionChoice_s * c;

  if(man_page && strcmp(man_page,"man_page") == 0)
  {
    c = calloc(1+2+1, sizeof(oyjlOptionChoice_s));
    if(c)
    {
      /* first entry is for unsetting */
      c[i].nick = strdup("-");
      c[i].name = strdup(_("[none]"));
      c[i].description = strdup("");
      c[i].help = strdup("");

      /* example choice */
      c[++i].nick = strdup("Breeze");
      c[i].name = strdup("Breeze");
      c[i].description = strdup("Detected Color Scheme");
      c[i].help = strdup("");

      c[++i].nick = strdup("BreezeDark");
      c[i].name = strdup("Breeze Dark");
      c[i].description = strdup("2. detected Color Scheme");
      c[i].help = strdup("");

      /* empty choice for end of list */
      c[++i].nick = malloc(4);
      c[i].nick[0] = '\000';
    }
    return c;
  }

  night = OYJL_IS_OPTION("night-color-scheme"); 
  if(night)
    value = oyGetPersistentString( OY_DISPLAY_STD "/night_color_scheme", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  else
    value = oyGetPersistentString( OY_DISPLAY_STD "/sunlight_color_scheme", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );

  list = listColorSchemes( &n );
  if(list)
  {
    c = calloc(n+2, sizeof(oyjlOptionChoice_s));
    if(c)
    {
      int pos = 1;
      /* first entry is for unsetting */
      c[i].nick = strdup("-");
      c[i].name = strdup(_("[none]"));
      c[i].description = strdup("");
      c[i].help = strdup("");

      for(i = 0; i < n; ++i)
      {
        char * t = list[i];
        c[pos].nick = strdup( t );
        c[pos].name = strdup("");
        c[pos].description = strdup("");
        c[pos].help = strdup("");

        if(selected && value && strcmp(t, value) == 0)
          *selected = pos;

        ++pos;
      }
    }
    oyjlStringListRelease( &list, n, oyDeAllocateFunc_ );
  }
  if(selected && *selected > 0)
    fprintf( stderr, "%s %s selected: %d %s\n", o->option, value, *selected, c[*selected].nick );

  return c;
}
char * getCurrentColorScheme()
{
  int n = 0,i, size = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, "LANG=C plasma-apply-colorscheme --list-schemes" );
  char ** list = oyjlStringSplit( result, '\n', &n, 0 );
  char * current = NULL;

  if(list)
  {
    for(i = 0; i < n; ++i)
    {
      char * t = list[i];
      if(strstr(t, "You have the"))
        continue;
      if(strchr(t, '*') != NULL)
        t = strchr(t, '*') + 1;
      if(t[0] == ' ')
        ++t;
      if(strstr(t, " (current color scheme)"))
      {
        char * txt = strstr(t, " (current color scheme)");
        txt[0] = '\000';
        current = strdup( t );
      }
    }
    oyjlStringListRelease( &list, n, free );
  }

  return current;
}
void setColorScheme( const char * scheme )
{
  int size = 0;
  char * command = NULL;
  char * result;
  const char * home = oyGetHomeDir_();
  const char * ThemeName = strstr(scheme, "dark") == NULL && strstr(scheme, "Dark") == NULL?"Adwaita":"Adwaita-dark";

  oyjlStringAdd( &command, 0,0, "plasma-apply-colorscheme %s; export THEME=%s; sed -i \"/Net\\/ThemeName/cNet\\/ThemeName \\\"$THEME\\\"\" %s/.config/xsettingsd/xsettingsd.conf; xsettingsd & (sleep 1; killall xsettingsd)", scheme, ThemeName, home );
  result = oyjlReadCommandF( &size, "r", malloc, command );

  free(result);
  free(command);
}
const char * jcommands = "{\n\
  \"command_set\": \"oyranos-monitor-white-point\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"oyranos-monitor-white-point\",\n\
  \"command_get_args\": [\"-X\",\"json+command\"]\n\
}";
void myOptionsRelease                ( oyjlOptions_s    ** opts )
{
  if(*opts) free(*opts);
  *opts = NULL;
}

oySCOPE_e scope = oySCOPE_USER;
double hour_ = -1.0; /* ignore this default value */

int myMain( int argc , const char** argv )
{
  unsigned i;
  int error = 0;
  /* the functional switches */
  int wtpt_mode = -1;
  int wtpt_mode_night = -1;
  int wtpt_mode_sunlight = -1;
  const char * sunlight_effect = NULL;
  const char * sunlight_color_scheme = NULL;
  const char * night_effect = NULL;
  const char * night_color_scheme = NULL;
  double night_backlight = -1;
  double temperature = 0.0, temperature_man = 2800.0; // 2800 kelvin is near candel light
  int show = 0;
  const char * export = NULL;
  int dry = 0;
  int system_wide = 0;
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
  int version = 0;
  int state = 0;
  int worked = 0;
  oyjlOptions_s * opts;
  const char * render = NULL;
  oyjlUi_s * ui;
  oyjlUiHeaderSection_s * info;
  const char * man_page = getenv("DISPLAY");
  int man = 0;

  if( man_page && strcmp(man_page,"man_page") == 0 )
      man = 1;

  if(oy_debug)
    fprintf(stderr, " %.06g %s\n", DBG_UHR_, oyPrintTime() );

  if(!man)
    temperature_man = oyGetTemperature(5000);

  DBG_S_( oyPrintTime() );
  opts = oyjlOptions_New( argc, (const char **)argv );
  /* nick, name, description, help */
  oyjlOptionChoice_s d_choices[] = {{"0", _("Deactivate"), _("Deactivate"), ""},
                                    {"1", _("Autostart"), _("Autostart"), ""},
                                    {"2", _("Activate"), _("Activate"), ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s env_vars[]={{"OY_DEBUG", _("set the Oyranos debug level."), _("Alternatively the -v option can be used."), _("Valid integer range is from 1-20.")},
                                    {"OY_MODULE_PATH", _("route Oyranos to additional directories containing modules."), "", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s examples[]={{_("Enable the daemon, set night white point to 3000 Kelvin and use that in night mode"), "oyranos-monitor-white-point -d 2 -a 3000 -n 1", "", ""},
                                    {_("Switch all day light intereference off such as white point and effect"), "oyranos-monitor-white-point -s 0 -e 0", "", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s see_as_well[]={{"oyranos-monitor(1) oyranos-config(1) oyranos(3)", "", "", ""},
                                    {"http://www.oyranos.org","","",""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOption_s oarray[] = {
  /* type,   flags, o, option, key, name, description, help, value_name, value_type, values, var_type, variable */
    {"oiwi", 0, "d", "daemon", NULL, _("daemon"), _("Control user daemon"), NULL, "0|1|2", oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)d_choices, sizeof(d_choices), 0 )}, oyjlINT, {.i=&daemon} },
    {"oiwi", 0, "m", "modes", NULL, _("Modes"), _("Show white point modes"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&show} },
    {"oiwi", 0, "w", "white-point", NULL, _("Mode"), _("Set white point mode"), NULL, "0|1|2|3|4|5|6|7", oyjlOPTIONTYPE_FUNCTION, {.getChoices = getWhitePointChoices}, oyjlINT,{.i=&wtpt_mode} },
    {"oiwi", 0, "n", "night-white-point", NULL, _("Night Mode"), _("Set night time mode"), _("A white point temperature of around 4000K and lower allows to get easier into sleep. Enable by setting this option to Automatic (-n=1) and Temperature to 3000 (-a=3000)."), "0|1|2|3|4|5|6|7", oyjlOPTIONTYPE_FUNCTION, {.getChoices = getWhitePointChoices}, oyjlINT, {.i=&wtpt_mode_night} },
    {"oiwi", 0, "s", "sun-white-point", NULL, _("Day Mode"), _("Set day time mode"), NULL, "0|1|2|3|4|5|6|7", oyjlOPTIONTYPE_FUNCTION, {.getChoices = getWhitePointChoices}, oyjlINT, {.i=&wtpt_mode_sunlight} },
    {"oiwi", 0, "a", "automatic", NULL, _("Temperature"), _("A value from 2700 till 8000 Kelvin is expected to show no artefacts"), NULL,
      /*  The white point profiles will be generated in many different shades, which will explode
       *  conversion cache. Thus we limit the possible shades to 100 kelvin steps, which in turn
       *  limits to around 100 profiles per monitor white point. */
      _("KELVIN"), oyjlOPTIONTYPE_DOUBLE, {.dbl.start = 1100, .dbl.end = 10100, .dbl.tick = 100, .dbl.d = temperature_man}, oyjlDOUBLE, {.d=&temperature} },
    {"oiwi", 0, "g", "night-effect", NULL, _("Night effect"), _("Set night time effect"), _("A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes ."), _("ICC_PROFILE"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = getLinearEffectProfileChoices}, oyjlSTRING, {.s=&night_effect} },
    {"oiwi", 0, "e", "sunlight-effect", NULL, _("Sun light effect"), _("Set day time effect"), _("A ICC profile of class abstract. Ideally the effect profile works on 1D RGB curves only and is marked meta:EFFECT_linear=yes ."), _("ICC_PROFILE"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = getLinearEffectProfileChoices}, oyjlSTRING, {.s=&sunlight_effect} },
    {"oiwi", 0, "b", "night-backlight", NULL, _("Night Backlight"), _("Set Nightly Backlight"), _("The option needs xbacklight installed and supporting your device for dimming the monitor lamp."), _("PERCENT"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0, .dbl.end = 100, .dbl.tick = 1, .dbl.d = man?4:getDoubleFromDB( OY_DISPLAY_STD "/display_backlight_night", 0 )}, oyjlDOUBLE, {.d=&night_backlight} },
    {"oiwi", 0, NULL, "sunlight-color-scheme", NULL, _("Sun light color scheme"), _("Set day time typical brighter color scheme"), _("Use this to switch color scheme day/night dependent."), _("STRING"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = getColorSchemeChoices}, oyjlSTRING, {.s=&sunlight_color_scheme} },
    {"oiwi", 0, NULL, "night-color-scheme", NULL, _("Night color scheme"), _("Set nightly typical darker color scheme"), _("Use this to switch color scheme day/night dependent."), _("STRING"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = getColorSchemeChoices}, oyjlSTRING, {.s=&night_color_scheme} },
    {"oiwi", 0, "l", "location", NULL, _("location"), _("Detect location by IP adress"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&location} },
    {"oiwi", 0, "i", "latitude", NULL, _("Latitude"), _("Set Latitude"), NULL, _("ANGLE_IN_DEGREE"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -90, .dbl.end = 90, .dbl.tick = 1, .dbl.d = man?0.0:getDoubleFromDB( OY_DISPLAY_STD "/latitude", 0 )}, oyjlDOUBLE, {.d=&latitude} },
    {"oiwi", 0, "o", "longitude", NULL, _("Longitude"), _("Set Longitude"), NULL, _("ANGLE_IN_DEGREE"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -180, .dbl.end = 180, .dbl.tick = 1, .dbl.d = man?0.0:getDoubleFromDB( OY_DISPLAY_STD "/longitude", 0 )}, oyjlDOUBLE, {.d=&longitude} },
    {"oiwi", 0, "r", "sunrise", NULL, _("Sunrise"), _("Show local time, used geographical location, twilight height angles, sun rise and sun set times"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&sunrise} },
    {"oiwi", 0, "t", "twilight", NULL, _("Twilight"), _("Set Twilight angle"), _("0:sunrise/sunset|-6:civil|-12:nautical|-18:astronomical"), _("ANGLE_IN_DEGREE"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 18, .dbl.end = -18, .dbl.tick = 1, .dbl.d = man?0.0:getDoubleFromDB( OY_DISPLAY_STD "/twilight", 0 )}, oyjlDOUBLE, {.d=&twilight} },
    {"oiwi", 0, "z", "system-wide", NULL, _("system wide"), _("System wide DB setting"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&system_wide} },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help",NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&help} },
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE|OYJL_OPTION_FLAG_IMMEDIATE, "y", "test", NULL, _("No Action"), NULL, NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&dry} },
    {"oiwi", 0, "u", "hour", NULL, "hour", "hour", NULL, NULL, oyjlOPTIONTYPE_DOUBLE, {.dbl.start = 0, .dbl.end = 48, .dbl.tick = 1, .dbl.d = 0}, oyjlDOUBLE, {.d=&hour_} },
    {"oiwi", OYJL_OPTION_FLAG_MAINTENANCE, "c", "check", NULL, "check", "check", NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&check} },
    /* blind options, useful only for man page generation */
    {"oiwi", 0, "E", "man-environment_variables", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)env_vars, sizeof(env_vars), 0 )}, oyjlNONE, {.i=NULL} },
    {"oiwi", 0, "A", "man-examples", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)examples, sizeof(examples), 0 )}, oyjlNONE, {.i=NULL} },
    {"oiwi", 0, "S", "man-see_as_well", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)see_as_well, sizeof(see_as_well), 0 )}, oyjlNONE, {.i=NULL} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };
  opts->array = (oyjlOption_s*)oyjlStringAppendN( NULL, (const char*)oarray, sizeof(oarray), 0 );

  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name, description, help, mandatory, optional, detail */
    {"oiwg", 0, _("Mode"), _("Actual mode"), NULL, "w,a", "z,v", "w,a,y" },
#if defined( XCM_HAVE_X11 )
    {"oiwg", 0, _("Night Mode"), _("Nightly appearance"), _("The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times."), "n,g,b,night-color-scheme", "z,v,y", "n,g,b,night-color-scheme" },
#else
    {"oiwg", 0, _("Night Mode"), _("Nightly appearance"), _("The Night white point mode shall allow to reduce influence of blue light during night time. A white point temperature of around 4000K and lower allows to get easier into sleep and is recommended along with warm room illumination in evening and night times."), "n,g,night-color-scheme", "z,v,y", "n,g,night-color-scheme" },
#endif
    {"oiwg", 0, _("Day Mode"), _("Sun light appearance"), NULL, "s,e,sunlight-color-scheme", "z,v,y", "s,e,sunlight-color-scheme" },
    {"oiwg", OYJL_GROUP_FLAG_EXPLICITE, _("Location"), _("Location and Twilight"), NULL, "l|i,o", "t,z,v,y", "l,i,o,t"},
    {"oiwg", 0, _("Daemon Service"), _("Run sunset daemon"), NULL, "d", "v", "d" },
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, _("Misc"), _("General options"), NULL, "m|r|X|h|V|R", "v", "h,m,r,X,R,V,z,y,v" },
    {"",0,0,0,0,0,0,0}
  };
  double night = isNight(0);
  oyjlOptionGroup_s ng;
  if(man) night = 1;
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
  opts->groups = (oyjlOptionGroup_s*)oyjlStringAppendN( NULL, (const char*)groups, sizeof(groups), 0 );

  info = oyUiInfo(_("The tool can set the actual white point or set it by local day and night time. A additional effect profile can be selected."),
                  "2018-10-11T12:00:00", "October 11, 2018");
  ui = oyjlUi_Create( argc, (const char **)argv,
      "oyranos-monitor-white-point", _("Night Manager"), _("Oyranos Night Manager handles the monitor white point"),
      "oyNM-logo",
      info, opts->array, opts->groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    return 0;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-monitor-white-point\n\n", _("For more information read the man page:"));
    return 0;
  }
  if(!ui) return 1;

  if((export && strcmp(export,"json+command") == 0))
  {
    char * json = NULL,
         * json_commands = strdup(jcommands),
         * text = NULL;
    error = getSunriseSunset( &rise, &set, dry, &text, verbose );
    ui->opts->groups[3].help = text;
    json = oyjlUi_ToJson( ui, 0 ),
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    puts( json_commands );
    return 0;
  }

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

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));


#if !defined(NO_OYJL_ARGS_RENDER)
  /* Render boilerplate */
  if(render)
  { 
    int debug = verbose;
    oyjlArgsRender( argc, argv, NULL, jcommands, NULL, debug, ui, myMain );
    oyjlUi_Release( &ui);
    return 0;
  }
#endif

  DBG_S_( oyPrintTime() );
  if(verbose >= 1)
    oy_debug += verbose;

  if(system_wide)
    scope = oySCOPE_SYSTEM;

  if(temperature != 0.0)
  {
    int i = (temperature - 1000) / 100;
    double xyz[3] = { bb_100K[i][0], bb_100K[i][1], bb_100K[i][2] };
    double XYZ[3] = { xyz[0]/xyz[1], 1.0, xyz[2]/xyz[1] }, oldXYZ[3];
    double Lab[3], oldLab[3];
    double cie_a = 0.0, cie_b = 0.0;
    char * comment = NULL;
    double old_temperature = 0;
    double dist = 1.0;
    oyXYZ2Lab( XYZ, Lab);

    oyGetDisplayWhitePoint( 1 /* automatic */, oldXYZ );
    oyXYZ2Lab( oldXYZ, oldLab);
    cie_a = oldLab[1]/256.0 + 0.5;
    cie_b = oldLab[2]/256.0 + 0.5;

    old_temperature = oyEstimateTemperature( cie_a, cie_b, &dist );
    if(old_temperature && dist < 0.0001)
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
    ++worked;
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
        int flag = (int)i == current ? oyjlBOLD:0;
        if(i == 1) /* automatic */
        {
          double temperature = oyGetTemperature(0);
          const char * man_page = getenv("DISPLAY");
          int man = 0;
          if(man_page && strcmp(man_page,"man_page") == 0)
            man = 1;
          if(temperature)
          {
            oyStringAddPrintf( &tmp, 0,0, "%g %s ", man?temperature_man:temperature, _("Kelvin") );
            t = tmp;
          }
        }
        printf("-w %u # %s %s%s\n", i, oyjlTermColor(flag,choices_string_list[i]), t, (int)i == current ? "*":" ");
        if(tmp) oyFree_m_(tmp);
      }
    }
    ++worked;
  }


  if(location)
  {
    error = findLocation(scope, dry);
    ++worked;
  }

  if(twilight != -1000)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", twilight);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD OY_SLASH "/twilight", scope, value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(longitude != 360)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", longitude);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/longitude", scope, value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(latitude != 360)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", latitude);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/latitude", scope, value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(wtpt_mode_night != -1)
  {
    oyStringAddPrintf(&value, 0,0, "%d", wtpt_mode_night);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_night", scope, value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(wtpt_mode_sunlight != -1)
  {
    oyStringAddPrintf(&value, 0,0, "%d", wtpt_mode_sunlight);
    if(dry == 0)
      oySetPersistentString( OY_DISPLAY_STD "/display_white_point_mode_sunlight", scope, value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(night_effect != NULL && dry == 0)
  {
    oySetPersistentString( OY_DISPLAY_STD "/night_effect", scope,
                           (night_effect[0] && night_effect[0] != '-') ?
                            night_effect : NULL, NULL );
    ++worked;
  }

  if(night_color_scheme != NULL && dry == 0)
  {
    oySetPersistentString( OY_DISPLAY_STD "/night_color_scheme", scope,
                           (night_color_scheme[0] && night_color_scheme[0] != '-') ?
                            night_color_scheme : NULL, NULL );
    ++worked;
  }

  if(night_backlight != -1 && dry == 0)
  {
    oyStringAddPrintfC(&value, 0,0, "%g", night_backlight);
    oySetPersistentString( OY_DISPLAY_STD "/display_backlight_night", scope,
                           night_backlight == 0 ? NULL : value, NULL );
    oyFree_m_(value);
    ++worked;
  }

  if(sunlight_effect != NULL && dry == 0)
  {
    oySetPersistentString( OY_DISPLAY_STD "/sunlight_effect", scope,
                           (sunlight_effect[0] && sunlight_effect[0]!='-') ?
                            sunlight_effect : NULL, NULL );
    ++worked;
  }

  if(sunlight_color_scheme != NULL && dry == 0)
  {
    oySetPersistentString( OY_DISPLAY_STD "/sunlight_color_scheme", scope,
                           (sunlight_color_scheme[0] && sunlight_color_scheme[0]!='-') ?
                            sunlight_color_scheme : NULL, NULL );
    ++worked;
  }

  if(sunrise)
  {
    char * text = NULL;
    error = getSunriseSunset( &rise, &set, dry, &text, verbose );
    puts(text);
    ++worked;
  }

  if(daemon != -1)
  {
    error = runDaemon(daemon);
    ++worked;
  }

  if(check)
  {
    checkWtptState( dry );
    ++worked;
  }

  if(worked == 0)
    oyjlOptions_PrintHelp( opts, ui, verbose, NULL );

  if(oy_debug)
    fprintf(stderr, " %.06g %s\n", DBG_UHR_, oyPrintTime() );
  free(info);
  return error;
}

int main( int argc_, char ** argv_)
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start QML */
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef OYJL_USE_GETTEXT
#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_ALL,"");
#endif
#endif

  oyExportStart_(EXPORT_CHECK_NO);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif
  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
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
  int r = 0;
  DBG_S_( oyPrintTime() );
  if((r=oyDisplayColorServerIsActive()) > 0)
  {
    DBG2_S_( "color server is active(%d) - stop  %s", r, oyPrintTime() );
    return;
  } else
    DBG1_S_( "color server is inactive - continue  %s", oyPrintTime() );
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
    /* write VCGT to temporary profile */
    oyStringAddPrintf(&cmd, 0,0, "oyranos-monitor -c -f vcgt -d %d -o %s", i, tmpname);
    fputs(cmd, stderr); fputs("\n", stderr );
    r = system( cmd );
    oyFree_m_(cmd);
    /* upload VCGT tag from temporary profile */
    oyStringAddPrintf(&cmd, 0,0, "oyranos-monitor -g -d %d %s", i, tmpname);
    fputs(cmd, stderr); fputs("\n", stderr );
    r = system( cmd );
    oyFree_m_(cmd);
    if(getenv("OY_DEBUG_WRITE") == NULL)
      remove( tmpname );
  }
  DBG_S_( oyPrintTime() );
}

int findLocation(oySCOPE_e scope, int dry)
{
  int error = 0;

  {
    size_t size = 0;
    char * geo_json = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                                        "https://location.services.mozilla.com/v1/geolocate?key=test", NULL ),
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
        fprintf(stdout, "%s\n", geo_json);
    } else
      error = 1;

    if(root)
    {
      char * json = NULL;
      int level = 0;
      v = oyjlTreeGetValueF( root, 0, "location/lat" );
      value = oyjlValueText( v, oyAllocateFunc_ );
      if(oyjlStringToDouble( value, &lat ) > 0)
        error = 1;
      v = oyjlTreeGetValueF( root, 0, "location/lng" );
      value = oyjlValueText( v, oyAllocateFunc_ );
      if(oyjlStringToDouble( value, &lon ) > 0)
        error = 1;
      oyFree_m_(value);
      oyjlTreeToJson( root, &level, &json );

      if(lat != 0.0 && lon != 0.0)
      {
        #define PRINT_VAL( key, name )\
        v = oyjlTreeGetValueF( root, 0, key, NULL ); \
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
  if(value && oyjlStringToDouble( value, lat ) > 0)
    fprintf(stderr, "lat = %g / %s\n", *lat, value);
  if(value)
  {
    oyFree_m_(value);
  } else
    need_location = 1;
  value = oyGetPersistentString( OY_DISPLAY_STD "/longitude", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
  if(value && oyjlStringToDouble( value, lon ) > 0)
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
int getSunriseSunset( double * rise, double * set, int dry, char ** text, int verbose )
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
  if(value && oyjlStringToDouble( value, &twilight ) > 0)
  {
    if(verbose)
      fprintf(stderr, "twilight = %g / %s\n", isnan(twilight) ? 0 : twilight, value && value[0] ? value : "----");
    twilight = 0.0;
  }
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
    elevation = getSunHeight( year, month, day, oyGetCurrentGMTHour_(0), lat, lon, verbose );
    if(text)
      oyjlStringAdd( text, malloc, free, "%s%s%s%s",
             verbose?_("Local Time"):"", verbose?":":"", verbose?"\t":"", oyjlPrintTime(verbose?OYJL_TIME_ZONE:OYJL_BRACKETS, oyjlGREEN) );
    oySplitHour( oyGetCurrentLocalHour( *rise, gmt_diff_second ), &hour, &minute, &second );
    if(text)
    {
      if(verbose)
      {
        oyjlStringAdd( text, malloc, free, "\n%s:\t%g°\n%s:\t%g°\n%s:\t%g°\n%s:\t%g°\n%s:\t%d:%.2d:%.2d",
             _("Latitude"), lat, _("Longitude"), lon, _("Twilight"), twilight, _("Sun Elevation"), elevation, _("Sunrise"), hour, minute, second );
      }
      else
        oyjlStringAdd( text, malloc, free, " %s: %g° %g° %s: %g° (%s: %g°) %s: %d:%.2d:%.2d",
             _("Geographical Position"), lat, lon, _("Twilight"), twilight, _("Sun Elevation"), elevation, _("Sunrise"), hour, minute, second );
    }
    oySplitHour( oyGetCurrentLocalHour( *set,  gmt_diff_second ), &hour, &minute, &second );
    if(text)
      oyjlStringAdd( text, malloc, free, "%s%s:%s%d:%.2d:%.2d",
             verbose?"\n":" ",_("Sunset"), verbose?"\t":" ", hour, minute, second );
  }

  return r;
}

int isNight(int dry)
{
  int    diff;
  double dtime, rise, set;

  dtime = oyGetCurrentGMTHour_(&diff);

  if( getSunriseSunset( &rise, &set, dry, NULL, 0 ) == 0 )
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
  if( choices_string_list && getSunriseSunset( &rise, &set, dry, NULL, 0 ) == 0 )
  {
    int new_mode = -1;
    char * new_effect = NULL;
    int use_effect = 0;
    int night_set_to = -1;

    cmode = oyGetBehaviour( oyBEHAVIOUR_DISPLAY_WHITE_POINT );
    effect = oyGetPersistentString( OY_DEFAULT_DISPLAY_EFFECT_PROFILE, 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
    fprintf (stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    fprintf (stderr, "%s: %s %s: %s\n", _("Actual white point mode"), oyjlTermColor(oyjlBOLD,cmode<choices?choices_string_list[cmode]:"----"),
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
        {
          oySetPersistentString( OY_DISPLAY_STD "/night", scope, "0", NULL );
          night_set_to = 0;
        }

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
      {
        if(!oyExistPersistentString( OY_DISPLAY_STD "/night", "1", 0, oySCOPE_USER_SYS ))
        {
          oySetPersistentString( OY_DISPLAY_STD "/night", scope, "1", NULL );
          night_set_to = 1;
#if defined( XCM_HAVE_X11 )
          size_t size = 0;
          char * backlight = oyReadCmdToMem_( "xbacklight", &size, "r", oyAllocateFunc_);
          if(backlight && oyExistPersistentString( OY_DISPLAY_STD "/display_backlight_night", NULL, 0, oySCOPE_USER_SYS ))
          {
            int bl = atoi(backlight);
            int nbl = -1;
            if(backlight[strlen(backlight)-1] == '\n')
              backlight[strlen(backlight)-1] = '\000';
            value = oyGetPersistentString( OY_DISPLAY_STD "/display_backlight_night", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
            nbl = atoi(value);
            fprintf( stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
            fprintf( stderr, "xbacklight old | new: %d(%s) | %d\n", bl, backlight, nbl );
            if(bl > nbl)
            {
              value = oyReadCmdToMemf_( &size, "r", oyAllocateFunc_, "xbacklight -set %d", nbl );
            }
            oyFree_m_(value);
            oyFree_m_(backlight);
          }
#endif
        }
      }
    }

    if( (new_mode != cmode) ||
        ((effect?1:0) != (new_effect?1:0) ||
         (effect && new_effect && strcmp(effect, new_effect) != 0)))
    {
      fprintf(  stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
      fprintf(  stderr, "%s: %s %s: %s\n", _("New white point mode"), oyjlTermColor(oyjlBOLD,new_mode<choices?choices_string_list[new_mode]:"----"),
                _("Effect"), oyNoEmptyString_m_(new_effect) );

      if(dry == 0 && (use_effect ||
                      (effect?1:0) != (new_effect?1:0)))
      {
        if( !new_effect || (strcmp(new_effect,"-") == 0 ||
                            strlen(new_effect) == 0 ))
          oySetPersistentString( OY_DEFAULT_DISPLAY_EFFECT_PROFILE, scope, NULL, NULL );
        else
          oySetPersistentString( OY_DEFAULT_DISPLAY_EFFECT_PROFILE, scope, new_effect, NULL );
        pingNativeDisplay();
      }

      if(cmode != new_mode)
        error = setWtptMode( scope, new_mode, dry );
    }

    if(dry == 0 && night_set_to >= 0)
    {
      char * ccs = getCurrentColorScheme();
      if(night_set_to == 1)
        value = oyGetPersistentString( OY_DISPLAY_STD "/night_color_scheme", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      else
        value = oyGetPersistentString( OY_DISPLAY_STD "/sunlight_color_scheme", 0, oySCOPE_USER_SYS, oyAllocateFunc_ );
      if(value && (!ccs || strcmp(value, ccs) != 0))
      {
        setColorScheme( value);
        fprintf(  stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
        fprintf(  stderr, "%s: %s %s/%s\n", _("New color scheme"),
                _("old/new"), oyNoEmptyString_m_(ccs), oyjlTermColor(oyjlBOLD,value) );
      }
      oyFree_m_(value);
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
                                       oyStruct_s        * cb_progress_context OY_UNUSED )
{
  const char * key;
  if(!status_text) return;
  DBG_S_( oyPrintTime() );

  oyGetPersistentStrings(NULL);

  key = strchr( status_text, '/' );
  if(key)
    ++key;
  else
    return;

  if( strstr(key, OY_STD "/ping") == NULL && /* let us ping */
      strstr(key, OY_DISPLAY_STD) == NULL && /* all display variables */
      strstr(key, OY_DEFAULT_DISPLAY_EFFECT_PROFILE) == NULL && /* new display effect profile */
      strstr(key, OY_DEFAULT_DISPLAY_WHITE_POINT) == NULL && /* oySetDisplayWhitePoint() */
      strstr(key, OY_DEFAULT_EFFECT) == NULL && /* effect switch changes */
      strstr(key, OY_DEFAULT_EFFECT_PROFILE) == NULL /* new effect profile */
    )
  {
    fprintf(stderr, "ignoring key: %s\n", key );
    return;
  }
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
  oyStartDBusObserver( oyWatchDBus, oyFinishDBus, oyMonitorCallbackDBus, OY_STD, NULL )
  if(id)
  {
    fprintf(stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    fprintf(stderr, "oyStartDBusObserver ID: %d\n", id);
  }

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
                     double lat, double lon,
                     int verbose )
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
  if(verbose)
  fprintf( stdout, "GMT:\t%02d:%02d:%02d\n", hour,minute,second );
  if(verbose)
  fprintf( stdout, "JD (GMT 12:00):\t%fd\n", d + 2451545.0 + lon/360.0 );
  if(verbose)
  fprintf( stdout, "JD0 (GMT 12:00):\t%fd\n", d + lon/360.0 );
  if(verbose)
  fprintf( stdout, "LMST:\t%fd\n", d + gmt_hours/24.0*365.25/366.25 );

  /* Compute local sideral time of this moment */
  sidtime = revolution( GMST0(d) + 360.*gmt_hours/24.*365.25/366.25 + lon );
  oySplitHour( oyGetCurrentLocalHour( sidtime/15., 0 ), &hour, &minute, &second );
  if(verbose)
  fprintf( stdout, "Local Mean Sidereal Time:\t%02d:%02d:%02d\n", hour,minute,second );

  sun_RA_dec( d, &sRA, &sdec, &sr );
  if(verbose)
  fprintf( stdout, "Rectaszension:\t%g°\n", sRA);
  if(verbose)
  fprintf( stdout, "Declination:\t%g°\n", sdec);
  t = sidtime - sRA;
  if(verbose)
  fprintf( stdout, "Sun's Hourly Angle:\t%g° (%gh)\n", t, t/15.);
  A = atand( sind( t ) /
             ( cosd( t )*sind( lon ) - tand( sdec )*cosd( lon ) )
           );
  if(verbose)
  fprintf( stdout, "Sun's Azimut:\t%g°\n", revolution(A-180.0) );
  hs = cosd( sdec )*cosd( t )*cosd( lat ) + sind( sdec )*sind( lat );
  h = asind( hs );
  if(verbose)
  fprintf( stdout, "Sun's Height:\t%g° sin(%g)\n", h, hs );

  return h;
}
