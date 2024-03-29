/** @file oyjl_args_base.h
 *
 *  oyjl - args C declarations and basic API's
 *
 *  @par Copyright:
 *            2010-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    OyjlArgs API provides a platformindependent C interface for argument handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_ARGS_BASE_H
#define OYJL_ARGS_BASE_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** \addtogroup oyjl_args

 *  @{ *//* oyjl_args */

/** @brief Type of option */
typedef enum oyjlOPTIONTYPE_e {
    oyjlOPTIONTYPE_START,              /**< */
    oyjlOPTIONTYPE_CHOICE,             /**< list of choices */
    oyjlOPTIONTYPE_FUNCTION,           /**< computed list of choices */
    oyjlOPTIONTYPE_DOUBLE,             /**< IEEE floating point number with double precission */
    oyjlOPTIONTYPE_NONE,               /**< no value possible - the option is a flag like -v/--verbose */
    oyjlOPTIONTYPE_END                 /**< */
} oyjlOPTIONTYPE_e;

/** @brief Types for oyjlVariable_u */
typedef enum oyjlVARIABLETYPE_e {
    oyjlNONE,                          /**< no variable given, will be asked later with oyjlOptions_GetResult() */
    oyjlSTRING,                        /**< pointer to a array of char */
    oyjlDOUBLE,                        /**< IEEE floating point number with double precission */
    oyjlINT                            /**< integer number declared as int */
} oyjlVARIABLE_e;                      /**< @brief type of result */

/** @brief Choice item */
typedef struct oyjlOptionChoice_s {
  char * nick;                         /**< @brief nick / ID as argument for a option */
  char * name;                         /**< @brief i18n short name for labels; In a MAN section and when starting with "http" this string is marked as link. */
  char * description;                  /**< @brief i18n description sentence; can be "" */
  char * help;                         /**< @brief i18n longer help text; can be "" */
} oyjlOptionChoice_s;

typedef struct oyjlOptions_s oyjlOptions_s;
typedef struct oyjlOption_s oyjlOption_s;

/** @brief abstract value
 *
 *  The type is declared inside the ::oyjlVARIABLE_e enum range. */
typedef union oyjlVariable_u {
  const char ** s;                     /**< @brief oyjlVARIABLE_STRING */
  double * d;                          /**< @brief oyjlVARIABLE_DOUBLE */
  int * i;                             /**< @brief oyjlVARIABLE_INT */
} oyjlVariable_u;

/** @brief abstract option
 *
 *  The type is declared inside the ::oyjlOPTIONTYPE_e enum range. */
typedef union oyjlOption_u {
  /** @brief oyjlOPTIONTYPE_FUNCTION
   *  The function is well called inside -h for generating help texts. Use OYJL_OPTION_FLAG_EDITABLE to supress this callback with -h.
   *  @param[in]   opt                 the option context
   *  @param[out]  selected            show the default; optional
   *  @param[in]   context             for more information
   *  @result                          the same as for the *choices::list* member; caller owns the memory
   */
  oyjlOptionChoice_s * (*getChoices)( oyjlOption_s * opt, int * selected, oyjlOptions_s * context );
  struct {
    oyjlOptionChoice_s * list;         /**< used for oyjlOPTIONTYPE_CHOICE | oyjlOPTIONTYPE_EDIT */
    int selected;                      /**< the currently selected choice */
  } choices;                           /**< @brief oyjlOPTIONTYPE_CHOICE | oyjlOPTIONTYPE_EDIT */
  struct {
    double d;                          /**< default / recommendation */
    double start;
    double end;
    double tick;
  } dbl;                               /**< @brief oyjlOPTIONTYPE_DOUBLE */
} oyjlOption_u;

#define OYJL_OPTION_FLAG_EDITABLE      0x001 /**< @brief The oyjlOption_s choices are merely a hint. Let users fill other strings too: "prog --opt=arg|..." */
#define OYJL_OPTION_FLAG_ACCEPT_NO_ARG 0x002 /**< @brief Accept as well no arg: "prog --opt[=arg]" */
#define OYJL_OPTION_FLAG_NO_DASH       0x004 /**< @brief No double dash '--' acceptance; single dash can be omitted by not specifying oyjlOption_s::o : "prog opt" */
#define OYJL_OPTION_FLAG_REPETITION    0x008 /**< @brief Accept more than one occurence: "prog --opt=arg ..." */
#define OYJL_OPTION_FLAG_MAINTENANCE   0x100 /**< @brief Maintenance option; can be invisible */
#define OYJL_OPTION_FLAG_IMMEDIATE     0x200 /**< @brief Apply instantly in UI; opposite to ::OYJL_GROUP_FLAG_EXPLICITE */
/** @brief abstract UI option
 *
 *  A oyjlOption_s::o is inside of oyjlOptionGroup_s::detail to be displayed and oyjlOptionGroup_s::mandatory/optional for syntax checking.
 */
struct oyjlOption_s {
  char type[8];                        /**< @brief must be 'oiwi' */
  /** - ::OYJL_OPTION_FLAG_EDITABLE : flag for oyjlOPTIONTYPE_CHOICE and oyjlOPTIONTYPE_FUNCTION. Hints a not closely specified intput. The content is typically not useful for a overview in a help or man page. These can print a overview with oyjlOption_s::value_type. This flag is intented for convinience suggestions or very verbose dictionaries used in scrollable pull down GUI elements.
   *  - ::OYJL_OPTION_FLAG_ACCEPT_NO_ARG : the flagged option can accept as well no argument without error.
   *    - oyjlSTRING will be set to empty string ("")
   *    - oyjlDOUBLE and oyjlINT will be set to 1
   *  - ::OYJL_OPTION_FLAG_NO_DASH can be used for subcommand options in ::OYJL_GROUP_FLAG_SUBCOMMAND flagged groups
   *  - ::OYJL_OPTION_FLAG_REPETITION multi occurence; print trailing ...
   *  - ::OYJL_OPTION_FLAG_IMMEDIATE instant applying; e.g. for cheap status info inside a ::OYJL_GROUP_FLAG_EXPLICITE flagged group
   *  - ::OYJL_OPTION_FLAG_MAINTENANCE accept even without printed visibility
   *  - ::OYJL_NO_OPTIMISE pass values through as is, e.g. without removing \"(double quote)
   */
  unsigned int flags;                  /**< @brief parsing and rendering hints */
  /** '#' is used as default option like a command without any arguments.
   *  '@' together with value_name expects arbitrary arguments as described in oyjlOption_s::value_name.
   *  Reserved letters are ,(comma), \'(quote), \"(double quote), .(dot), :(double point), ;(semikolon), /(slash), \\(backslash)
   *  The letter shall return strlen(o) <= 1.
   *  If zero '\000' terminated, this short :o: option name is not enabled and a long :option: name shall be provided.
   */
  const char * o;                      /**< @brief One letter UTF-8 option name; optional if *option* is present */
  /** The same reserved letters apply as for the oyjlOption_s::o member letter. */
  const char * option;                 /**< @brief String without white space, "my-option"; optional if *o* is present */
  const char * key;                    /**< @brief DB key; optional */
  const char * name;                   /**< @brief i18n label string */
  const char * description;            /**< @brief i18n short sentence about the option */
  const char * help;                   /**< @brief i18n longer text to explain what the option does; optional */
  const char * value_name;             /**< @brief i18n value string; used only for option args; consider using upper case, e.g. FILENAME, NUMBER ... */
  oyjlOPTIONTYPE_e value_type;         /**< @brief type for *values* */
  oyjlOption_u values;                 /**< @brief the selectable values for the option; not used for oyjlOPTIONTYPE_NONE */
  oyjlVARIABLE_e variable_type;        /**< @brief type for *variable* */
  oyjlVariable_u variable;             /**< @brief automatically filled variable depending on *value_type* */
  const char * properties;             /**< @brief newline separated extension key=value pairs; known key is file_names, e.g.: "file_names=*.[J,j][S,s][O,o][N,n];*.[X,x][M,m][L,l];*.[Y,y][A,a][M,m][L,l]\ncomment=select JSON and friends" */
};
char * oyjlOption_PropertiesGetValue ( oyjlOption_s      * o,
                                       const char        * key );

/** For a related flag see ::OYJL_OPTION_FLAG_NO_DASH */
#define OYJL_GROUP_FLAG_SUBCOMMAND     0x080 /**< @brief This oyjlOptionGroup_s flag requires one single mandatory option with oyjlOPTIONTYPE_NONE. */
/** For per option exception see ::OYJL_OPTION_FLAG_IMMEDIATE . The flag is intended for e.g. costly processing. */
#define OYJL_GROUP_FLAG_EXPLICITE      0x100 /**< @brief Apply explicitely in UI */
/** Recommend to set a special section name "GENERAL OPTIONS". Usualy it follows the "OPTIONS" section. */
#define OYJL_GROUP_FLAG_GENERAL_OPTS   0x200 /**< @brief Hint for MAN page section */
/**
 *  @brief Info to compile a Syntax line and check missing arguments
 *
 *  Options listed in mandatory, optional and detail are comma(,) separated.
 */
typedef struct oyjlOptionGroup_s {
  char type [8];                       /**< @brief must be 'oiwg' */
  /** - ::OYJL_GROUP_FLAG_SUBCOMMAND : flag inhibits --style mandatory option documentation and uses subcommand style without double dash "--" prefix. */
  unsigned int flags;                  /**< @brief parsing and rendering hints */
  const char * name;                   /**< @brief i18n label string */
  const char * description;            /**< @brief i18n short sentence about the option */
  const char * help;                   /**< @brief i18n longer text to explain what the option does; optional */
  const char * mandatory;              /**< @brief list of mandatory options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options; one single option here makes a subcommand and is usualy easier to understand */
  const char * optional;               /**< @brief list of non mandatory options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options */
  const char * detail;                 /**< @brief list of options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options to display */
  const char * properties;             /**< @brief newline separated extension key=value pairs */
} oyjlOptionGroup_s;

/**
 *   @brief Main command line, options and groups
 *
 *  Man sections can be added by using the "man-"man_name identifier/name scheme, e.g. "man-see_also" in oyjlOption_s::option.
 *
 *  @see oyjlUi_ToMan()
 */
struct oyjlOptions_s {
  char type [8];                       /**< @brief must be 'oiws' */
  oyjlOption_s * array;                /**< @brief the options; make shure to add -h|--help and -v|--verbose options */
  oyjlOptionGroup_s * groups;          /**< @brief groups of options, which form a command */
  void * user_data;                    /**< @brief will be passed to functions; optional */
  int argc;                            /**< @brief plain reference from main(argc,argv) */
  const char ** argv;                  /**< @brief plain reference from main(argc,argv) */
  void * private_data;                 /**< internal state; private to OyjlArgs API, do not use */
};
/** @brief option state */
typedef enum {
  oyjlOPTION_NONE,                     /**< untouched */
  oyjlOPTION_USER_CHANGED,             /**< passed in by user */
  oyjlOPTION_MISSING_VALUE,            /**< user error */
  oyjlOPTION_UNEXPECTED_VALUE,         /**< user error */
  oyjlOPTION_NOT_SUPPORTED,            /**< user error */
  oyjlOPTION_DOUBLE_OCCURENCE,         /**< user error; except '@' is specified */
  oyjlOPTIONS_MISSING,                 /**< user error; except '#' is specified */
  oyjlOPTION_NO_GROUP_FOUND,           /**< user error */
  oyjlOPTION_SUBCOMMAND,               /**< category */
  oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND /**< user error */
} oyjlOPTIONSTATE_e;
char **  oyjlOptions_ResultsToList   ( oyjlOptions_s     * opts,
                                       const char        * option,
                                       int               * count );
/** @brief Header section */
typedef struct oyjlUiHeaderSection_s {
  char type [8];                       /**< @brief must be 'oihs' */
  const char * nick;                   /**< @brief single word well known identifier; *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *oyjl_modules_author*, *documentation* */
  const char * label;                  /**< @brief i18n short string, in case this section nick is not well known; optional */
  const char * name;                   /**< @brief i18n short content */
  const char * description;            /**< @brief i18n optional second string; might contain a browsable url for further information, e.g. a link to the full text license, home page; optional */
} oyjlUiHeaderSection_s;

/** @brief Info for graphic UI's containing options, additional info sections and other bells and whistles */
typedef struct oyjlUi_s {
  char type [8];                       /**< @brief must be 'oiui' */
  const char * app_type;               /**< @brief "tool" or "module" */
  const char * nick;                   /**< @brief four byte ID for module or plain comand line tool name, e.g. "oyjl-tool" */
  const char * name;                   /**< @brief i18n short name for tool bars, app lists */
  const char * description;            /**< @brief i18n name, version, maybe more for a prominent one liner */
  const char * logo;                   /**< @brief file name body without path, for relocation, nor file type ending; typical a PNG or SVG icon; e.g. "lcms_icon" for lcms_icon.png or lcms_icon.svg; optional */
  /** We describe here a particular tool/module. Each property object contains at least one 'nick' and one 'name' key. All values shall be strings. *label* or *description* keys are optional. If they are not contained, fall back to *name*. Well known *nick* values are *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *oyjl_modules_author*, *documentation*, *date* and *logo*. */
  oyjlUiHeaderSection_s * sections;
  oyjlOptions_s * opts;                /**< @brief info for UI logic */
} oyjlUi_s;
enum {
  oyjlUI_STATE_NONE,                   /**< @brief nothing to report */
  oyjlUI_STATE_HELP,                   /**< @brief --help printed */
  oyjlUI_STATE_VERBOSE = 2,            /**< @brief --verbose option detected */
  oyjlUI_STATE_EXPORT = 4,             /**< @brief --export=XXX or other stuff printed */
  oyjlUI_STATE_OPTION = 24,            /**< @brief bit shift for detected error from option parser */
  oyjlUI_STATE_NO_CHECKS = 0x1000,     /**< @brief skip any checks */
  oyjlUI_STATE_NO_RELEASE = 0x2000     /**< @brief skip any data release, e.g. for --help + freeing oyjlUi_s */
};
oyjlUi_s *         oyjlUi_Create     ( int                 argc,
                                       const char       ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s      * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status );

/** 
 *  @} *//* oyjl_args
 */


#ifdef __cplusplus
}
#endif


#endif /* OYJL_ARGS_BASE_H */
