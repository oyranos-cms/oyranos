/** @file oyranos-monitor.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor configurator, gamma loader, daemon
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/03
 *
 * It reads the default profile(s) from the Oyranos CMS and recalls this
 * profile(s)
 * as new default profile for a screen, including a possible curves upload to
 * the video card.
 * One needs xcalib installed to do the curves upload.
 * 
 */
/* 
cc `pkg-config --libs --cflags oyranos oyjl xcm` -I src/include_private/ -I build/src/include_private/ -I src/API_generated/ -I src/include/ oyranos-monitor.c -o oyranos-monitor2 && ./oyranos-monitor2 -hv
*/

#include "oyranos_config_internal.h"
#ifdef HAVE_X11
#include <X11/Xcm/Xcm.h>
#include <X11/Xcm/XcmEvents.h>
#ifdef HAVE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif
#endif

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_icc.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_conversion.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_version.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "oyConversion_s.h"
#include "oyProfile_s_.h"
#include "oyProfiles_s.h"
#include "oyRectangle_s.h"

#include "oyjl.h"
#include "oyjl_version.h"
extern char **environ;


#ifdef XCM_HAVE_X11
int   updateOutputConfiguration      ( Display           * display );
int            getDeviceProfile      ( Display           * display,
                                       oyConfig_s        * device,
                                       int                 screen );
void  cleanDisplay                   ( Display           * display,
                                       int                 n );
int  runDaemon                       ( const char        * display_name );
#endif
int oyXCMDisplayColorServerIsActive  ( const char        * display_name );
void  oyProfile_SetTimeNow           ( oyProfile_s       * profile );

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }
int   compareRanks                   ( const void       * rank1,
                                       const void       * rank2 )
{const int32_t *r1=(int32_t*)rank1, *r2=(int32_t*)rank2; if(r1[1] < r2[1]) return 1; else return 0;}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int error = 0;
  int state = 0;
  int setup = 0;
  int gamma = 0;
  int erase = 0;
  int unset = 0;
  const char * display = NULL;
  int x = -1;
  int y = -1;
  int device_pos = -1;
  int x_color_region_target = 0;
  const char * monitor_profile = 0;
  int system_wide = 0;
  int list = 0;
  int short_var = 0;
  int path = 0;
  int list_taxi = 0;
  int icc_version_2 = 0;
  int icc_version_4 = 0;
  const char * module = 0;
  int modules = 0;
  const char * format = 0;
  const char * output = 0;
  int daemon = 0;
  int device_meta_tag = 0;
  const char * add_edid = 0;
  const char * add_vcgt = 0;
  const char * profile = 0;
  const char * name = 0;
  int xcm_active = 0;
  const char * help = NULL;
  int verbose = 0;
  int version = 0;
  const char * render = NULL;
  const char * export = 0;

  /* handle options */
  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,
   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation*. Choose what you see fit. Add new ones as needed. */
  oyjlUiHeaderSection_s * sections = oyUiInfo(_("The oyranos-monitor programm let you query and set the monitor profile(s) within the Oyranos color management system (CMS). Profiles are stored in a data base, the Oyranos DB."),
                  "2018-02-22T12:00:00", _("February 22, 2018"));

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s E_choices[] = {{_("DISPLAY"),  _("On X11 systems the display is selected by this variable."),NULL,                         NULL},
                                    {_("OY_DEBUG"), _("set the Oyranos debug level. Alternatively the -v option can be used."),NULL,                         NULL},
                                    {_("XDG_DATA_HOME XDG_DATA_DIRS"),_("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended."),_("http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal"),NULL},
                                    {_("OY_MODULE_PATH"),_("route Oyranos to additional directories containing modules."),NULL,                         NULL},
                                    {"","","",""}};

  oyjlOptionChoice_s A_choices[] = {{_("Put the following in a setup script like .xinitrc"),"oyranos-monitor", _("Select a monitor profile, load the binary blob into X and fill the VideoCardGammaTable, if appropriate"),NULL},
                                    {_("Assign a ICC profile to a screen"),_("oyranos-monitor -x pos -y pos profilename"),NULL,                         NULL},
                                    {_("Reset a screens hardware LUT in order to do a calibration"),_("oyranos-monitor -e -x pos -y pos profilename"),NULL,                         NULL},
                                    {_("Query the server side, network transparent profile"),_("oyranos-monitor -x pos -y pos"),NULL,                         NULL},
                                    {_("List all Oyranos monitor devices"),_("oyranos-monitor -l"),NULL,                         NULL},
                                    {_("Show the first Oyranos monitor device verbosely"),_("oyranos-monitor -l -v -d 0"),NULL,                         NULL},
                                    {_("Pass the monitor profile to a external tool"),_("iccDumpProfile -v \"`oyranos-monitor -l -d 0 -c --path`\""),NULL,                         NULL},
                                    {_("List all monitor devices through the oyX1 module"),_("oyranos-monitor -l --module oyX1"),NULL,                         NULL},
                                    {_("Dump data in the format following the -f option"),_("oyranos-monitor -f=[edid|icc|edid_icc] -o=edid.bin -x=pos -y=pos -m"),NULL,                         NULL},
                                    {_("Embedd device informations into ICC meta tag"),_("oyranos-monitor --add-edid=edid_filename --profile=profilename.icc --name=profilename"),NULL,                         NULL},
                                    {"","","",""}};

  oyjlOptionChoice_s S_choices[] = {{_("oyranos-config-fltk(1) oyranos-policy(1) oyranos-profiles(1) oyranos(3)"),NULL,               NULL,                         NULL},
                                    {_("https://gitlab.com/oyranos/oyranos"),NULL,               NULL,                         NULL},
                                    {"","","",""}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", 0,                          "#",NULL,            NULL,     "",         _("No args"),                 _("Run command without arguments"),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "s","setup",         NULL,     _("Setup"),    _("Setup a ICC profile for all or a selected monitor"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&setup}, NULL},
    {"oiwi", 0,                          "g","gamma",         NULL,     _("Gamma"),    _("Setup only Gamma VCGT part"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&gamma}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "x",NULL,            NULL,     NULL,          _("select the screen at the x position. The Oyranos screens are if possible real devices. A screen position must be unique and not shared by different screens, otherwise it will be ambiguous."),NULL, _("PIXEL"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlINT,       {.i=&x}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "y",NULL,            NULL,     NULL,          _("select the screen at the y position."),NULL, _("PIXEL"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlINT,       {.i=&y}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"display",      NULL,     NULL,          _("Use DISPLAY"),NULL, _("NAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&display}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "d","device-pos",    NULL,     NULL,          _("Position in device list."),  _("The numbering of monitors starts with zero for the first device. To get the number of all available devices use \"oyranos-monitor -l | wc -l\"."), _("NUMBER"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlINT,       {.i=&device_pos}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "@",NULL,            NULL,     NULL,          _("ICC device profile for a monitor"),NULL,_("ICC_FILE_NAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&monitor_profile}, NULL},
    {"oiwi", 0,                          "2","icc-version-2", NULL,     _("Icc Version 2"),_("Select ICC v2 Profiles"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_2},NULL},
    {"oiwi", 0,                          "4","icc-version-4", NULL,     _("Icc Version 4"),_("Select ICC v4 Profiles"),NULL,NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_4},NULL},
    {"oiwi", 0,                          NULL,"system-wide",   NULL,     _("System Wide"),_("do system wide, might need admin or root privileges"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&system_wide}, NULL},
    {"oiwi", 0,                          "u","unset",         NULL,     _("Unset"),    _("Release a ICC profile from a monitor device"),_("Reset the hardware gamma table to the defaults."),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&unset}, NULL},
    {"oiwi", 0,                          "e","erase",         NULL,     _("Erase"),    _("Release a ICC profile from a monitor device"),_("Reset the hardware gamma table to the defaults and erase from the Oyranos DB"),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&erase}, NULL},
    {"oiwi", 0,                          "l","list",          NULL,     _("List"),     _("list devices"),            NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list}, NULL},
    {"oiwi", 0,                          "L","list-taxi",     NULL,     _("Taxi"),     _("List Taxi Profiles"),      NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_taxi}, NULL},
    {"oiwi", 0,                          NULL,"short",         NULL,     _("Short"),    _("show only the ICC profiles file name"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&short_var}, NULL},
    {"oiwi", 0,                          NULL,"path",          NULL,     _("Path"),     _("show the full ICC profile path and file name"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&path}, NULL},
    {"oiwi", 0,                          "c","x-color-region-target",NULL,     _("X Color Region Target"),_("show the X Color Management (XCM) device profile"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&x_color_region_target}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"module",        NULL,     _("Module"),   _("Select module"),          _("See as well the --modules option."), _("MODULENAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&module}, NULL},
    {"oiwi", 0,                          NULL,"modules",       NULL,     _("Modules"),  NULL,                        _("List monitor modules"),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&modules}, NULL},
    {"oiwi", 0,                          "a","xcm-active",    NULL,     _("XCM active"),  NULL,                      _("Show if X Color Management is active"),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&xcm_active}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("Select data format"),_("Select a data format. Possible are edid for server side EDID data, icc for a server side ICC profile, edid_icc for a ICC profile created  from  server side  EDID and vcgt for effect and monitor white point merged into a possibly altered VCGT tag. Without the -o/--ouput option the output is written to stdout."),_("edid|icc|edid_icc|vcgt"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&format}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("Write data selected by -f/--format to the given filename."),NULL, _("OUT_FILENAME|default:stdout"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}, NULL},
    {"oiwi", 0,                          "m","device-meta-tag",NULL,    NULL,          _("(--device-meta-tag) switch is accepted by the -f=icc and -f=edid_icc option."), _("It embeddes device and driver informations about the actual device for later easier ICC profile to device assignment.  This becomes useful for sharing ICC profiles."),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&device_meta_tag}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"add-edid",      NULL,     _("Add EDID"), _("EDID Filename"),          _("Embedd EDID keys to a ICC profile as meta tag. Requires --profile."),_("FILENAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&add_edid}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"add-vcgt",      NULL,     _("Add VCGT"), _("VCGT Filename"),          _("Embedd VCGT calibration to a ICC profile as vcgt tag. FILENAME can be output from 'xcalib -p'. Requires --profile."),_("FILENAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&add_vcgt}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"profile",       NULL,     _("Profile"),  _("ICC profile."),NULL, _("ICC_FILE_NAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&profile}, NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"name",          NULL,     _("Name"),     _("String for ICC profile internal name."),NULL, _("NAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&name}, NULL},
    {"oiwi", 0,                          NULL,"daemon",        NULL,     NULL,          NULL,                        _("Run as daemon to observe monitor hotplug events and update the setup."),NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&daemon}, NULL},
    {"oiwi", 0,                          "E","man-environment",NULL,    _("Environment Variables"),NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "A","man-examples",  NULL,     _("Examples"),NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     _("See Also"),NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}, NULL},
    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help}, NULL },
    {"oiwi", 0,                          "v","verbose",       NULL,     _("Verbose"),  _("increase verbosity"),      NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&verbose}, NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R", "render",  NULL, _("render"),  _("Render"),  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render}, NULL },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version}, NULL },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export}, NULL },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}, NULL}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail,        properties */
    {"oiwg", 0,     NULL,               _("Set basic parameters"),    NULL,               NULL,          NULL,          "x,y,d,display,module,c,system-wide,2,4", NULL},
    {"oiwg", 0,     _("Activate"),      _("Activate Profiles"),       _("Set up the X11 server with the Oyranos DB stored monitor ICC profile(s)."),"#,s","x,y|d,v","#,s", NULL},
    {"oiwg", 0,     _("Assign"),        _("Set New Profile"),         _("Assign a monitor ICC profile to the selected screen and store in the Oyranos DB."), "@", "x,y|d,system-wide,v","@", NULL},
    {"oiwg", 0,     _("Erase"),         _("Erase Profile"),           NULL,               "e,u",         "x,y|d,system-wide,v","e,u",  NULL},
    {"oiwg", 0,     _("List"),          _("List Devices"),            _("List names and show details."), "l","x,y|d,short,path,c,v,module,2,4","l,short,path", NULL},
    {"oiwg", 0,     _("Informations"),  _("Give Informations"),       NULL,               "f,a,modules,L","o,x,y|d,display,m,v,2,4","f,o,m,a,modules,L",NULL},
    {"oiwg", 0,     _("Daemon"),        _("Run Daemon"),              _("Keep a session up to date."),"daemon","v",     "daemon",      NULL},
    {"oiwg", 0,     _("Add EDID"),      _("Add Device Meta Data"),    _("Add device information to a profile for automated selection."), "add-edid,add-vcgt,profile","name,v",  "add-edid,add-vcgt,profile,name", NULL},
    {"oiwg", 0,     _("Misc"),          _("General Options"),         NULL,               "h,X,R,V",     "v",           "h,X,V,R,v",   NULL},
    {"",0,0,0,0,0,0,0, NULL}
  };

  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-monitor", _("Monitor configuration with Oyranos CMS"), NULL,
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       NULL,
#endif
                                       sections, oarray, groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-monitor\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(ui && verbose)
  {
    char * json = oyjlOptions_ResultsToJson( ui->opts, 0 );
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = oyjlOptions_ResultsToText( ui->opts );
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );
  }

  if(ui && (export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = NULL;
    oyjlStringAdd( &json_commands, malloc, free, "{\n  \"command_set\": \"%s\"", argv[0] );
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] ); /* skip opening '{' */
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
    int debug = verbose;
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-render viewer." );
#endif
  } else if(ui)
  {
    /* ... working code goes here ... */
    char *ptr = NULL;
    char *oy_display_name = NULL;
    oyProfile_s * prof = 0;
    oyConfig_s * device = 0;
    oyConfigs_s * devices = 0;
    oyOptions_s * options = 0;
    oyConfig_s * c = 0;
    oyOption_s * o = 0;
    size_t size = 0;
    const char * filename = 0;
    char * data = 0;
    uint32_t n = 0;
    unsigned int i;

    char * device_class = oyjlStringCopy("monitor._native", 0);

    int daemon_var = daemon;
    const char * add_meta = add_edid;
    const char * new_profile_name = name,
               * prof_name = profile,
               * module_name = module;
    int simple = short_var,
        server = 0,
        list_modules = modules;
    uint32_t icc_profile_flags = 0;
    oySCOPE_e scope = system_wide ? oySCOPE_SYSTEM : oySCOPE_USER;
    char *display_name = display ? strdup(display):NULL;
    if(!display_name && getenv("DISPLAY"))
      display_name = strdup(getenv("DISPLAY"));

    if(icc_version_2) icc_profile_flags |= OY_ICC_VERSION_2;
    if(icc_version_4) icc_profile_flags |= OY_ICC_VERSION_4;

    if(path) simple = 2;
    if(device_pos || x || y)
      server = 1;
    if(unset || erase || list || x_color_region_target || (device_pos && !gamma) || format || output)
    {
      if(verbose && monitor_profile)
      {
        fprintf( stderr, "%s ", oyjlFunctionPrint( __func__, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__ ) );
        fprintf( stderr, "%s: %s\n", "rm monitor_profile", oyjlTermColor(oyjlBOLD, monitor_profile) );
      }
      monitor_profile = NULL;
    }

    if(verbose && device_pos != -1)
    {
      int r OY_UNUSED;
      char*cmd=NULL; oyjlStringAdd( &cmd, 0,0, "oyranos-monitor -l -d %d\n", device_pos);
      fprintf( stderr, "%s ", oyjlFunctionPrint( __func__, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__ ) );
      r = system(cmd);
    }

    XcmDebugVariableSet( &oy_debug );
    if(xcm_active)
    {
      oyjlTermColor(oyjlBOLD,"");
      int active = oyXCMDisplayColorServerIsActive( display_name );
      fprintf( stdout, "XCMDisplayColorServerIsActive: %s\n", active?oyjlTermColor(oyjlBOLD, "|ICM|"):"---" );
      return error;
    }


#ifdef HAVE_X11
    if(module_name && strstr(module_name, "oyX1"))
    {
#endif

#ifndef __APPLE__
      if(!display_name)
      {
        WARNc_S( _("DISPLAY variable not set: giving up.") );
        error = 1;
        return error;
      }
#endif

      /* cut off the screen information */
      if(display_name &&
         (ptr = strchr(display_name,':')) != 0)
        if( (ptr = strchr(ptr, '.')) != 0 )
          ptr[0] = '\000';
#ifdef HAVE_X11
    }
#endif

    /* implicite selection for the most common default case */
    if(!icc_profile_flags)
    {
      icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                                                OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                NULL, 0 );
    }

    /* start independent white point daemon */
    if(daemon_var)
    {
      int r OY_UNUSED;
      char*cmd=NULL; oyjlStringAdd( &cmd, 0,0, "oyranos-monitor-white-point --daemon=1%s &\n", verbose?" -v":"");
      fprintf( stderr, "%s", oyjlTermColor(oyjlITALIC, cmd) );
      r = system(cmd);
    }

#ifdef XCM_HAVE_X11
#include <unistd.h> /* sleep() */
    if(daemon_var)
    {
      int r = 0;
      int seconds = 0;
      Display * display = XOpenDisplay( display_name );
      while(!display && seconds < 5)
      {
        fprintf( stderr, "%s: Can not open display.\n", oyjlTermColor(oyjlBOLD, _("WARN")));
        sleep( 1 );
        ++seconds;
      }
      if(!display)
      {
        fprintf( stderr, "%s: Can not open display. Give up after 5 seconds.\n", oyjlTermColor(oyjlRED, _("ERROR")));
        error = 1;
        return error;
      }
      if((r=XcmColorServerCapabilities( display )) > 0 && r & XCM_COLOR_SERVER_MANAGEMENT)
        daemon_var = 2;
      if(oy_debug || verbose) fprintf( stderr, "active: %d (daemon_var=%d)\n", r, daemon_var);
      XCloseDisplay( display );
      r = system(argv[0]);
    }

    /* we rely on any color server doing X11 setup by its own and do not want to interfere */
    if(daemon_var != 2)
#endif
    {
      if(!erase && !unset && !list && !setup && !format && !gamma &&
         !add_meta && !add_vcgt && !list_modules && !list_taxi)
        setup = 1;

      if(module_name)
      {
        STRING_ADD( device_class, ".");
        STRING_ADD( device_class, module_name);
      }

      /* by default a given monitor profile is used to setup the major monitor */
      if(monitor_profile && !server && device_pos == -1)
        device_pos = 0;

      if(server && x != -1 && y != -1)
        oy_display_name = oyGetDisplayNameFromPosition2 ( OY_TYPE_STD,
                                                      device_class,
                                                      display_name, x,y,
                                                      oyAllocFunc);
      else
      {
        error = oyOptions_SetFromString( &options,
                                       "//" OY_TYPE_STD "/config/command",
                                       "properties", OY_CREATE_NEW );
        error = oyOptions_SetFromString( &options, "//"OY_TYPE_STD"/config/edid",
                                         "1", OY_CREATE_NEW );
#ifdef HAVE_X11
        if(module_name && strstr(module_name, "oyX1"))
        {
#endif
        if(server)
          error = oyOptions_SetFromString( &options,
                                         "//"OY_TYPE_STD"/config/device_name",
                                         oy_display_name, OY_CREATE_NEW );
        else
          error = oyOptions_SetFromString( &options,
                                         "//"OY_TYPE_STD"/config/display_name",
                                         display_name, OY_CREATE_NEW );
#ifdef HAVE_X11
        }
#endif
        error = oyOptions_SetFromInt( &options,
                                      "//" OY_TYPE_STD "/icc_profile_flags",
                                      icc_profile_flags, 0, OY_CREATE_NEW );

        error = oyDevicesGet( 0, device_class, options, &devices );

        n = oyConfigs_Count( devices );
        if(error <= 0 && 0 <= device_pos && device_pos < (int)n )
        {
          device = oyConfigs_Get( devices, device_pos );
          oy_display_name = strdup( oyConfig_FindString( device, "device_name", 0 ));
        } else if(device_pos != -1)
          fprintf( stderr, "%s %d. %s: %d\n", _("Could not resolve device"),
                   device_pos, _("Available devices"), n);
        oyOptions_Release( &options );
      }

      if(list_modules)
      {
        uint32_t count = 0,
               * rank_list = 0;
        char ** texts = 0;

        error = oyConfigDomainList( device_class, &texts, &count,
                                    &rank_list, 0 );

        for(i = 0; i < count; ++i)
          printf("%s\n", strstr(texts[i],"monitor.") + 8 );
        return error;
      }

      if(list_taxi)
      {
        oyConfig_s * taxi_dev;
        int32_t * ranks;
        int head = 0;
        devices = 0;

        if(!oy_display_name)
        {
          fprintf(stderr,
                  "%s\n", _("Please specify a monitor with the -d option.") );
          return error;
        }

        oyDevicesFromTaxiDB( device, options, &devices, NULL );
        n = oyConfigs_Count( devices );
        if(n == 0)
        fprintf(stderr,
                "%s\n", _("Zero profiles found in Taxi ICC DB") );
        ranks = calloc(sizeof(int32_t), n*2+1);
        for(i = 0; i < n; ++i)
        {
          taxi_dev = oyConfigs_Get( devices, i );
          ranks[2*i+0] = i;
          error = oyDeviceCompare( device, taxi_dev, &ranks[2*i+1] );

          oyConfig_Release( &taxi_dev );
        }
        qsort( ranks, n, sizeof(int32_t)*2, compareRanks );
        for(i = 0; i < n; ++i)
        {
          taxi_dev = oyConfigs_Get( devices, ranks[2*i+0] );
          if(ranks[2*i+1] <= 0 && !verbose)
          {
            oyConfig_Release( &taxi_dev );
            continue;
          }

          if(!head)
          {
            oyOptions_s * cs_options = 0;
            char * text = NULL,
                 * report = NULL;

            head = 1;

            if(verbose)
            {
              if(x_color_region_target)
              {
                /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
                error = oyOptions_SetFromString( &cs_options,
                "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
              }
              error = oyDeviceGetInfo( device, oyNAME_NICK, cs_options, &text,
                                       oyAllocFunc );
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "\"%s\" ", text ? text : "???" );
              error = oyDeviceGetInfo( device, oyNAME_NAME, cs_options, &text,
                                       oyAllocFunc );
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "%s", text ? text : "???" );
              fprintf( stderr, "%s: %s\n", _("Taxi DB entries for device"),
                       report );
            }
            fprintf( stderr, "%s [%s] \"%s\"\n", _("Taxi ID"),
                     _("match value"), _("description") );
            oyOptions_Release( &cs_options );
          }

          printf("%s/0 [%d] ", oyNoEmptyString_m_(
                   oyConfig_FindString(taxi_dev, "TAXI_id", 0)), ranks[2*i+1]);
          printf("\"%s\"\n", oyNoEmptyString_m_(
                   oyConfig_FindString(taxi_dev, "TAXI_profile_description", 0)));

          if(oy_debug)
          {
            char * json_text = 0;
            oyDeviceToJSON( taxi_dev, 0, &json_text, oyAllocateFunc_ );
            fprintf(stderr, "%s\n", json_text );
            oyFree_m_(json_text);
          }

          oyConfig_Release( &taxi_dev );
        }
        oyConfig_Release( &device );
        oyConfigs_Release( &devices );
        oyOptions_Release( &options );

        return error;
      }

      if(format &&
         (strcmp(format,"edid") == 0 ||
          strcmp(format,"icc") == 0 ||
          strcmp(format,"edid_icc") == 0 ||
          strcmp(format,"vcgt") == 0))
      {
        icHeader * header = 0;
        char * out_name = 0;

        error = oyOptions_SetFromString( &options,
                                       "//" OY_TYPE_STD "/config/command",
                                       "properties", OY_CREATE_NEW );
        error = oyOptions_SetFromString( &options, "//"OY_TYPE_STD"/config/edid",
                                         "1", OY_CREATE_NEW );
        if(oy_display_name)
          error = oyOptions_SetFromString( &options,
                                         "//"OY_TYPE_STD"/config/device_name",
                                         oy_display_name, OY_CREATE_NEW );
#ifdef HAVE_X11
        else
        if(module_name && strstr(module_name, "oyX1"))
#endif
          error = oyOptions_SetFromString( &options,
                                         "//"OY_TYPE_STD"/config/display_name",
                                         display_name, OY_CREATE_NEW );

        error = oyOptions_SetFromInt( &options,
                                      "//" OY_TYPE_STD "/icc_profile_flags",
                                      icc_profile_flags, 0, OY_CREATE_NEW );

        n = oyConfigs_Count( devices );
        if(!error)
        {
          for(i = 0; i < n; ++i)
          {
            c = oyConfigs_Get( devices, i );

            if(strcmp(format,"edid_icc") == 0)
            {
              o = oyConfig_Find( c, "color_matrix.from_edid."
                       "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma");

              if(o)
              {
                oyOptions_s * opts = oyOptions_New(0),
                            * result = 0;

                error = oyOptions_MoveIn( opts, &o, -1 );
                error = oyOptions_SetFromInt( &opts,
                                      "//" OY_TYPE_STD "/icc_profile_flags",
                                      icc_profile_flags, 0, OY_CREATE_NEW );
                oyOptions_Handle( "///create_profile.icc",
                                  opts,"create_profile.icc_profile.color_matrix",
                                  &result );
                prof = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                          oyOBJECT_PROFILE_S );
                oyOptions_Release( &result );
              }

              if(prof)
              {
                uint32_t model_id = 0;
                const char * t = 0;
                error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag,
                                              (char*) output ? output : format );
                t = oyConfig_FindString( c, "EDID_manufacturer", 0 );
                if(t)
                  error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t );
                t =  oyConfig_FindString( c, "EDID_model", 0 );
                if(t)
                  error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);
                if(device_meta_tag)
                {
                  oyOptions_s * opts = 0;
                  error = oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                        "EDID_.OPENICC_" , OY_CREATE_NEW );
                  error = oyOptions_SetFromString( oyConfig_GetOptions( c,"backend_core" ),
                                          OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.monitor" OY_SLASH
                                         "OPENICC_automatic_generated",
                                         "1", OY_CREATE_NEW );
                  error = oyOptions_SetFromString( oyConfig_GetOptions( c,"backend_core" ),
                                          OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD OY_SLASH OY_TYPE_STD OY_SLASH "device" OY_SLASH "config.icc_profile.monitor" OY_SLASH
                                         "prefix",
                                         "EDID_.OPENICC_", OY_CREATE_NEW );
                  oyProfile_AddDevice( prof, c, opts );
                  oyOptions_Release( &opts );
                }
                data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
                header = (icHeader*) data;
                t = oyConfig_FindString( c, "EDID_mnft", 0 );
                if(t)
                  sprintf( (char*)&header->manufacturer, "%s", t );
                t = oyConfig_FindString( c, "EDID_model_id", 0 );
                if(t)
                  model_id = atoi( t );
                model_id = oyValueUInt32( model_id );
                memcpy( &header->model, &model_id, 4 );
                oyOption_Release( &o );
              }
            } else
            if(strcmp(format,"edid") == 0 && device_pos == (int)i)
            {
              o = oyConfig_Find( c, "edid" );
              data = oyOption_GetData( o, &size, oyAllocFunc );
            } else
            if( device_pos == (int)i &&
                ( strcmp(format,"icc") == 0 ||
                  strcmp(format, "vcgt") == 0 ) )
            {
              oyOptions_s * cs_options = 0;
              if(x_color_region_target)
              {
                /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
                error = oyOptions_SetFromString( &cs_options,
                "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
              }
              oyDeviceAskProfile2( c, cs_options, &prof );
              if(!prof)
              {
                fprintf(stderr, "Try harder to get monitor profile ...\n");
                oyDeviceGetProfile( c, cs_options, &prof );
                if(!prof)
                  fprintf(stderr, "Harder try to get monitor profile failed.\n");
              }
              oyOptions_Release( &cs_options );
              if(device_meta_tag)
              {
                oyOptions_s * opts = 0;
                error = oyOptions_SetFromString( &opts, "///set_device_attributes",
                                                      "true", OY_CREATE_NEW );
                error = oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                      "EDID_" , OY_CREATE_NEW );
                oyProfile_AddDevice( prof, c, opts );
                oyOptions_Release( &opts );
              }
              if(strcmp(format,"icc") == 0)
                data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
              else if(strcmp(format, "vcgt") == 0)
              {
                /* 1. detect if a XCM color server is active */
                int active = oyXCMDisplayColorServerIsActive( display_name );

                /* 1.1. stop if XCM is active*/
                if(active)
                  data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
                else
                {
                  if(oyProfile_CreateEffectVCGT( prof ))
                    fprintf(stderr, "Create Effect VCGT failed\n");
                  /* 7. write the profile */
                  data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
                }
                uint32_t flags = 0;
                int choices = 0, current = -1;
                const char ** choices_string_list = NULL;
                oyOptionChoicesGet2( oyWIDGET_DISPLAY_WHITE_POINT, flags,
                                   oyNAME_NAME, &choices,
                                   &choices_string_list, &current );
                fprintf(stderr, "%s [oy-moni] ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
                fprintf(stderr, "Color Server active: %d white point mode: %s\n", active, 0 <= current && current < choices ? choices_string_list[current]:"" );
              }
            }

            if(data && size)
            {
              if(output)
                error = oyWriteMemToFile2_( output,
                                            data, size, 0x01,
                                            &out_name, oyAllocFunc );
              else
                fwrite( data, sizeof(char), size, stdout );
              oyDeAllocFunc( data ); size = 0;
            } else
              error = 1;

            if(!error)
            { if(verbose) fprintf( stderr, "  written to %s\n", out_name ); }
            else
              printf( "Could not write %d bytes to %s\n",
                      (int)size, out_name?out_name:format);
            if(out_name){ oyDeAllocFunc(out_name); out_name = 0; }

            oyProfile_Release( &prof );
            oyOption_Release( &o );
            oyConfig_Release( &c );
          }
        } else
          WARNc2_S("oyDevicesGet %s %d", _("found issues"),error);

        oyConfigs_Release( &devices );
        oyOptions_Release( &options );

      } else
      if(prof_name && add_meta)
      {
        uint32_t id[4];
        oyBlob_s * edid = oyBlob_New(0);
        char * edid_fn = oyResolveDirFileName_(add_meta);

        data = oyReadFileToMem_( edid_fn, &size, oyAllocateFunc_ );
        oyBlob_SetFromData( edid, data, size, "edid" );
        oyFree_m_(data);
        prof = oyProfile_FromName( prof_name, OY_NO_CACHE_READ | icc_profile_flags, 0 );
        device = 0;
        oyOptions_Release( &options );
        error = oyOptions_SetFromString( &options,
                                       "//" OY_TYPE_STD "/config/command",
                                       "add_meta", OY_CREATE_NEW );
        error = oyOptions_MoveInStruct( &options,
                                       "//" OY_TYPE_STD "/config/icc_profile",
                                        (oyStruct_s**)&prof, OY_CREATE_NEW );
        error = oyOptions_MoveInStruct( &options,
                                       "//" OY_TYPE_STD "/config/edid",
                                        (oyStruct_s**)&edid, OY_CREATE_NEW );
        error = oyDeviceGet( OY_TYPE_STD, device_class, ":0.0", options, &device);
        if(verbose && device)
        {
          /* We need a newly opened profile, otherwise we obtaine cached
             modifications. */
          oyProfile_s * p = oyProfile_FromName( prof_name, OY_NO_CACHE_READ | icc_profile_flags, 0 );
          oyConfig_s * p_device = oyConfig_FromRegistration( 
                                         oyConfig_GetRegistration( device ), 0 );
          int32_t rank = 0;
          int old_oy_debug = oy_debug;
          char * json = 0;
          oyProfile_GetDevice( p, p_device );

          if(oy_debug > 1)
          {
            error = oyDeviceToJSON( p_device, 0, &json, oyAllocateFunc_ );
            fprintf(stderr, "device from profile %s:\n%s\n", prof_name, json );
            oyFree_m_( json );
          }
          if(oy_debug > 1)
          {
            error = oyDeviceToJSON( device, 0, &json, oyAllocateFunc_ );
            fprintf(stderr, "device from edid %s:\n%s\n", edid_fn, json );
            oyFree_m_( json );
          }

          /*p_device->db = oyOptions_Copy( p_device->backend_core, 0 );
          device->db = oyOptions_Copy( device->backend_core, 0 );*/
          if(oy_debug < 2) oy_debug = 2;
          error = oyDeviceCompare( p_device, device, &rank );
          oy_debug = old_oy_debug;
          fprintf(stderr, "rank of edid to previous profile %d\n", rank);
        }
        oyConfig_Release( &device );
        oyFree_m_(edid_fn);
        prof = (oyProfile_s*)oyOptions_GetType( options, -1, "icc_profile",
                                                oyOBJECT_PROFILE_S );
        oyOptions_Release( &options );
        if(new_profile_name)
          error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag, new_profile_name );
        /* serialise before requesting a ICC md5 */
        data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
        oyFree_m_(data);
        oyProfile_GetMD5( prof, OY_COMPUTE, id );
        oyProfile_ToFile_( (oyProfile_s_*)prof, prof_name );
        oyProfile_Release( &prof );
      } else
      if(prof_name && add_vcgt)
      {
        uint32_t id[4];
        char * vcgt_fn = oyResolveDirFileName_(add_vcgt);
        char * error_buffer = calloc(sizeof(char), 1228);
        uint16_t * vcgt;
        int width = 0,i,j;
        oyjl_val root, v;
        data = oyReadFileToMem_( vcgt_fn, &size, oyAllocateFunc_ );
        prof = oyProfile_FromName( prof_name, OY_NO_CACHE_READ | icc_profile_flags, 0 );
        root = oyjlTreeParseCsv( data, NULL, OYJL_NUMBER_DETECTION, error_buffer, 128 );
        width = oyjlValueCount(root);
        vcgt = calloc(3*width, sizeof(uint16_t));
        for(i = 0 ; i < width; ++i)
        {
          for(j = 0; j < 3; ++j)
          {
            v = oyjlTreeGetValueF( root, 0, "[%d]/[%d]", i, j );
            if(!v)
            {
              width = i;
              break;
            }
            vcgt[i*3+j] = v->u.number.i;
          }
        }
        oyProfile_SetVCGT(prof, vcgt, width);
        oyFree_m_(vcgt_fn);
        oyFree_m_(data);
        if(new_profile_name)
          error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag, new_profile_name );
        /* serialise before requesting a ICC md5 */
        oyProfile_SetTimeNow( prof );
        data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
        oyFree_m_(data);
        oyProfile_GetMD5( prof, OY_COMPUTE, id );
        {
          const char * dest = new_profile_name?new_profile_name:prof_name;
          char * t = oyjlStringCopy( dest, 0 ),
               * tmp = strrchr( t, '.' );
          if(!(tmp && strlen(tmp) == 4))
            oyjlStringAdd( &t, 0,0, ".icc" );
          oyProfile_ToFile_( (oyProfile_s_*)prof, t );
          if(verbose)
            fprintf(stderr, "Wrote: \"%s\" to %s\n", dest, t );
          free(t);
        }
        oyProfile_Release( &prof );
      }

      if(list)
      {
        char * text = 0,
             * report = 0;
        uint32_t n = 0, i;
        oyOptions_s * cs_options = 0;

        if(x_color_region_target)
        {
          /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
          error = oyOptions_SetFromString( &cs_options,
                "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
        }

        n = oyConfigs_Count( devices );
        if(error <= 0)
        {
          const char * device_name = 0;
          for(i = 0; i < n; ++i)
          {
            c = oyConfigs_Get( devices, i );
            device_name = oyConfig_FindString( c, "device_name", 0 );

            if( oy_display_name && device_name &&
                strcmp( oy_display_name, device_name ) != 0 )
            {
              oyConfig_Release( &c );
              device_name = 0;
              continue;
            }

            if(verbose)
            fprintf(stdout,"------------------------ %d ---------------------------\n",i);

            error = oyDeviceGetInfo( c, oyNAME_NICK, cs_options, &text,
                                     oyAllocFunc );
            if(!simple)
            {
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "%d: ", i );
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "\"%s\" ", text ? text : "???" );
              error = oyDeviceGetInfo( c, oyNAME_NAME, cs_options, &text,
                                       oyAllocFunc );
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "%s%s", text ? text : "???",
                                  (i+1 == n) || device_pos != -1 ? "" : "\n" );
            } else
            {
              oyDeviceAskProfile2( c, cs_options, &prof );
              data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
              if(size && data)
                oyDeAllocFunc( data );
              filename = oyProfile_GetFileName( prof, -1 );
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "%s%s", filename ? (simple == 1)?(strrchr(filename,OY_SLASH_C) ? strrchr(filename,OY_SLASH_C)+1:filename):filename : OY_PROFILE_NONE,
                                  (i+1 == n) || device_pos != -1 ? "" : "\n" );
            }
            if(verbose)
            {
              error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, cs_options, &text,
                                       oyAllocFunc );
              fprintf( stdout,"%s\n", text ? text : "???" );
            }

            if(text)
              free( text );

            /* verbose adds */
            if(verbose)
            {
              oyDeviceAskProfile2( c, cs_options, &prof );
              data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
              if(size && data)
                oyDeAllocFunc( data );
              filename = oyProfile_GetFileName( prof, -1 );
              fprintf( stdout, " server profile \"%s\" size: %d\n",
                      filename?filename:OY_PROFILE_NONE, (int)size );

              text = 0;
              oyDeviceProfileFromDB( c, &text, oyAllocFunc );
              fprintf( stdout, " DB profile \"%s\"\n  keys: %s\n",
                      text?text:OY_PROFILE_NONE,
                      oyConfig_FindString( c, "key_set_name", 0 ) ?
                        oyConfig_FindString( c, "key_set_name", 0 ) :
                        OY_PROFILE_NONE );

              oyProfile_Release( &prof );
              oyDeAllocFunc( text );
            }

            oyConfig_Release( &c );
          }

          if(report)
            fprintf( stdout, "%s\n", report );
          oyDeAllocFunc( report ); report = 0;
        } else
          WARNc2_S("oyDevicesGet %s %d", _("found issues"),error);
          
        oyConfigs_Release( &devices );
        oyOptions_Release( &cs_options );
      }

      if(oy_display_name)
      /* make shure the display name is correctly including the screen number */
      {
        error = oyOptions_SetFromInt( &options,
                                      "//" OY_TYPE_STD "/icc_profile_flags",
                                      icc_profile_flags, 0, OY_CREATE_NEW );

        if(monitor_profile && !gamma)
        {
          if(verbose)
            fprintf( stdout, "oyDeviceSetProfile()\n" );
          oyDeviceSetProfile( device, scope, monitor_profile );
          if(verbose)
            fprintf( stdout, "oyDeviceUnset()\n" );
          oyDeviceUnset( device );
        } else
        if(erase || unset)
        {
          if(verbose)
            fprintf( stdout, "oyDeviceUnset()\n" );
          oyDeviceUnset( device );
        }
        if(erase)
        {
          if(verbose)
            fprintf( stdout, "oyConfig_EraseFromDB()\n" );
          oyConfig_EraseFromDB( device, scope );
        }

        if(setup)
        {
          if(monitor_profile)
            error = oyOptions_SetFromString( &options,
                                        "//"OY_TYPE_STD"/config/skip_ask_for_profile", "yes", OY_CREATE_NEW );
          if(verbose)
            fprintf( stdout, "oyDeviceSetup()\n" );
          oyDeviceSetup2( device, options );
        }
        if(gamma)
        {
          if(monitor_profile)
          {
            oyDeviceSetupVCGT( device, options, monitor_profile );
          }
          else
          {
            fprintf( stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
            fprintf( stderr, "%s ", oyjlFunctionPrint( __func__, strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__ ) );
            fprintf( stderr, "%s: %s\n", oyjlTermColor(oyjlRED, "no monitor profile specified"), monitor_profile?monitor_profile:"----" );
          }
        }

        oyConfig_Release( &device );
      }
      else if(erase || unset || setup)
      {
        n = oyConfigs_Count( devices );
        if(!error)
        {
          for(i = 0; i < n; ++i)
          {
            device = oyConfigs_Get( devices, i );

            if(erase || unset)
              oyDeviceUnset( device );
            if(erase)
              oyConfig_EraseFromDB( device, scope );
            if(setup)
              oyDeviceSetup2( device, options );

            oyConfig_Release( &device );
          }
        }
        oyConfigs_Release( &devices );
        oyOptions_Release( &options );
      }
    }

    if(oy_display_name)
      oyDeAllocFunc(oy_display_name);

#if defined(XCM_HAVE_X11)
    if(daemon_var)
      error = runDaemon( display_name );
#else
    if(daemon_var)
      fprintf( stderr, "daemon mode not supported on your OS\n" );
#endif

  }
  else error = 1;

  clean_main:
  free(sections);
  {
    int i = 0;
    while(oarray[i].type[0])
    {
      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)
        free(oarray[i].values.choices.list);
      ++i;
    }
  }
  oyjlLibRelease();

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start Renderer (e.g. QML) */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif

  return 0;
}


#ifdef XCM_HAVE_X11
void cleanDisplay                    ( Display           * display,
                                       int                 n OY_UNUSED )
{
  oyOptions_s * opts = 0,
              * result = 0;

  const char * display_name = strdup(XDisplayString(display));

  oyOptions_SetFromString( &opts, "////display_name",
                         display_name, OY_CREATE_NEW );
  oyOptions_Handle( "//" OY_TYPE_STD "/clean_profiles",
                                 opts,"clean_profiles",
                                 &result );
}

int            getDeviceProfile      ( Display           * display OY_UNUSED,
                                       oyConfig_s        * device,
                                       int                 screen OY_UNUSED )
{
  oyProfile_s * dst_profile = 0;
  int error = 0;
  uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                  OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                 NULL, 0 );
  oyOptions_s * options = 0;

  oyDeviceUnset( device );

      /*oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/command",
                                       "list", OY_CREATE_NEW );
      oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                                       "yes", OY_CREATE_NEW );*/
  error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    icc_profile_flags, 0, OY_CREATE_NEW );
  error = oyDeviceGetProfile( device, options, &dst_profile );
  oyOptions_Release( &options );

  return error;
}

oyConfigs_s * old_devices = NULL;

int updateOutputConfiguration( Display * display )
{
  int error = 0,
      i, n, update = 0;
  oyOptions_s * options = 0;
  oyConfigs_s * devices = 0;
  oyConfig_s * device = 0, * old_device = 0;

  /* allow Oyranos to see modifications made to the compiz Xlib context */
  XFlush( display );

  /* obtain device informations, including geometry and ICC profiles
     from the according Oyranos module */
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/edid",
                                 "refresh", OY_CREATE_NEW );
  error = oyDevicesGet( OY_TYPE_STD, "monitor", options, &devices );
  if(error) WARNc2_S("%s %d", _("found issues"),error);
  n = oyOptions_Count( options );
  oyOptions_Release( &options );

  n = oyConfigs_Count( devices );
  /* find out if monitors have changed at all
   * care only about EDID's and enumeration, no dimension */
  if(n != oyConfigs_Count( old_devices ))
    update = 1;
  else
  for(i = 0; i < n; ++i)
  {
    const char * edid, * old_edid;
    device = oyConfigs_Get( devices, i );
    old_device = oyConfigs_Get( old_devices, i );
    edid = oyOptions_FindString( *oyConfig_GetOptions(device,"backend_core"),"EDID",0 );
    old_edid = oyOptions_FindString( *oyConfig_GetOptions(old_device,"backend_core"),"EDID",0 );

    if(edid && old_edid && strcmp(edid,old_edid)==0)
      update = 0;
    else
      update = 1;

    oyConfig_Release( &device );
    oyConfig_Release( &old_device );
    if(update) break;
  }

  if(!update)
    goto clean_update;

  cleanDisplay( display, n );

  for(i = 0; i < n; ++i)
  {
    device = oyConfigs_Get( devices, i );

    error = getDeviceProfile( display, device, i );
    if(error) WARNc2_S("%s %d", _("found issues"),error);

    oyConfig_Release( &device );
  }

  clean_update:
  oyConfigs_Release( &old_devices );
  old_devices = devices;

  return 0;
}

int  runDaemon                       ( const char        * display_name )
{
  Display * display;
  Window root;
  int rr_event_base = 0, rr_error_base = 0;
  XcmeContext_s * c = XcmeContext_New( );

  XcmeContext_Setup2( c, display_name, 0 );
  display = XcmeContext_DisplayGet( c );

  if(!display)
    return 1;

  root = RootWindow( display, DefaultScreen( display ) );

#ifdef HAVE_XRANDR
  XRRQueryExtension( display, &rr_event_base, &rr_error_base );
  XRRSelectInput( display, root, RRScreenChangeNotifyMask | RRCrtcChangeNotifyMask | RROutputChangeNotifyMask | RROutputPropertyNotifyMask);
#endif



  for(;;)
  {
    XEvent event;
    XNextEvent( display, &event);

    /* we rely on any color server doing X11 setup by its own */
    if(XcmColorServerCapabilities( display ) & XCM_COLOR_SERVER_MANAGEMENT)
      continue;

#ifdef HAVE_XRANDR
    if(event.type == rr_event_base + RRNotify)
    {
      XRRNotifyEvent *rrn = (XRRNotifyEvent *) &event;
      if(rrn->subtype == RRNotify_OutputChange ||
         rrn->subtype == RR_Rotate_0)
      {
        fprintf( stderr,"detected RRNotify_OutputChange event -> update\n");
        updateOutputConfiguration( display );
      }
    }
#else
    {
      Atom net_desktop_geometry = XInternAtom( display,
                                      "_NET_DESKTOP_GEOMETRY", False );
      if( event.type == PropertyNotify &&
          event.xproperty.atom == net_desktop_geometry)
      {
        fprintf( stderr,"detected _NET_DESKTOP_GEOMETRY event -> update\n");
        updateOutputConfiguration( display );
      }
    }
#endif
  }

  XcmeContext_Release( &c );

  return 0;
}
#endif

int oyXCMDisplayColorServerIsActive  ( const char        * display_name )
{
  int active = 0, r;
#if defined(XCM_HAVE_X11)
  Display * display = XOpenDisplay( display_name );
  int old_debug = oy_debug;
  oy_debug = 1;
  if((r = XcmColorServerCapabilities( display )) > 0 && r & XCM_COLOR_SERVER_MANAGEMENT)
    active = 1;
  if(oy_debug) fprintf( stderr, "active: %d\n", r);
  oy_debug = old_debug;
  XCloseDisplay( display );
#endif
  return active;
}

void  oyProfile_SetTimeNow           ( oyProfile_s       * profile )
{
  struct tm * gmt;
  time_t cutime = time(NULL); /* time right NOW */
  gmt = localtime( &cutime );
  oyProfile_SetSignature( profile, 1900+gmt->tm_year, oySIGNATURE_DATETIME_YEAR );
  oyProfile_SetSignature( profile, 1+gmt->tm_mon, oySIGNATURE_DATETIME_MONTH );
  oyProfile_SetSignature( profile, gmt->tm_mday, oySIGNATURE_DATETIME_DAY );
  oyProfile_SetSignature( profile, gmt->tm_hour, oySIGNATURE_DATETIME_HOURS );
  oyProfile_SetSignature( profile, gmt->tm_min, oySIGNATURE_DATETIME_MINUTES );
  oyProfile_SetSignature( profile, gmt->tm_sec, oySIGNATURE_DATETIME_SECONDS );
}
