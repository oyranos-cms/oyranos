/**
 *  @file openicc_core.h
 *
 *  libOpenICC - OpenICC Colour Management Tools
 *
 *  @par Copyright:
 *            2011-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management core types
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/10/21
 */

#ifndef __OPENICC_CORE_H__
#define __OPENICC_CORE_H__

#include <stdio.h>


#if   defined(__clang__)
#define OI_FALLTHROUGH
#elif __GNUC__ >= 7 
#define OI_FALLTHROUGH                 __attribute__ ((fallthrough));
#else
#define OI_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define OI_DEPRECATED                  __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OI_DEPRECATED                  __declspec(deprecated)
#else
#define OI_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OI_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OI_UNUSED                      __declspec(unused)
#else
#define OI_UNUSED
#endif

/** \addtogroup misc

 *  @{ */

typedef void * (*openiccAlloc_f)     ( size_t              size );
typedef void   (*openiccDeAlloc_f)   ( void              * data );

#define OPENICC_SLASH "/"

/** @brief customisable messages */
#define OI_DEBUG                       "OI_DEBUG"
extern int * openicc_debug;
extern int openicc_backtrace;

typedef enum {
  openiccMSG_ERROR = 300,              /**< @brief fatal user messages */
  openiccMSG_WARN,                     /**< @brief log messages */
  openiccMSG_DBG,                      /**< @brief developer messages */
} openiccMSG_e;

typedef int  (*openiccMessage_f)     ( int/*openiccMSG_e*/ error_code,
                                       const void        * context_object,
                                       const char        * format,
                                       ... );
int            openiccMessageFuncSet ( openiccMessage_f    message_func );
int            openiccVersion        ( void );
void           openiccSetDebugVariable(int               * cmm_debug );

int            openiccReadFileSToMem ( FILE              * fp,
                                       char             ** ptr,
                                       int               * size);
void *         openiccMemDup         ( const void        * src,
                                       size_t              size );
/** 
 *  @} *//* misc
 */

/** \addtogroup args

 *  @{ */

/** @brief Type of option */
typedef enum openiccOPTIONTYPE_e {
    openiccOPTIONTYPE_START,           /**< */
    openiccOPTIONTYPE_CHOICE,          /**< list of choices */
    openiccOPTIONTYPE_FUNCTION,        /**< computed list of choices */
    openiccOPTIONTYPE_DOUBLE,          /**< IEEE floating point number with double precission */
    openiccOPTIONTYPE_NONE,            /**< no value possible - the option is a flag like -v/--verbose */
    openiccOPTIONTYPE_END              /**< */
} openiccOPTIONTYPE_e;

/** @brief Choice item */
typedef struct openiccOptionChoice_s {
  char * nick;                         /**< nick / ID as argument for a option */
  char * name;                         /**< i18n short name for labels */
  char * description;                  /**< i18n description sentence; can be "" */
  char * help;                         /**< i18n longer help text; can be "" */
} openiccOptionChoice_s;
void openiccOptionChoice_Release        ( openiccOptionChoice_s**choices );
typedef struct openiccOptions_s openiccOptions_s;
typedef struct openiccOption_s openiccOption_s;
typedef union openiccOption_u {
  struct choices {
    openiccOptionChoice_s * list;      /**< used for openiccOPTIONTYPE_CHOICES; not needed when *getChoices* is set */
    int selected;                      /**< the currently selected choice */
  } choices;
  /** openiccOPTIONTYPE_FUNCTION used for openiccOPTIONTYPE_CHOICES; not needed when *choices* is set
   *  @param[in]   opt                 the option context
   *  @param[out]  selected            show the default; optional
   *  @param[in]   context             for more information
   *  @result                          the same as for the *choices::list* member; caller owns the memory
   */
  openiccOptionChoice_s * (*getChoices)( openiccOption_s * opt, int * selected, openiccOptions_s * context );
  struct dbl {
    double d;                          /**< default / recommendation */
    double start;
    double end;
    double tick;
  } dbl;                               /**< openiccOPTIONTYPE_DOUBLE */
} openiccOption_u;

/** @brief abstract UI option */
struct openiccOption_s {
  char type[4];                        /**< must be 'oiwi' */
  unsigned int flags;                  /**< unused */
  char o;                              /**< one letter option name; '-' and ' ' are reserved */
  const char * option;                 /**< string without white space, "my-option"; optional if *o* is present */
  const char * key;                    /**< DB key; optional */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * value_name;             /**< i18n value string; used only for option args; consider using upper case, e.g. FILENAME, NUMBER ... */
  openiccOPTIONTYPE_e value_type;      /**< type for *choices* */
  openiccOption_u values;              /**< the selectable values for the option; not used for openiccOPTIONTYPE_NONE */
};

/**
    @brief info to compile a Syntax line and check missing arguments
 */
typedef struct openiccOptionGroup_s {
  char type [4];                       /**< must be 'oiwg' */
  unsigned int flags;                  /**< unused */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * mandatory;              /**< list of mandatory one letter options for this group of associated options */
  const char * optional;               /**< list of non mandatory one letter options for this group of associated options */
  const char * detail;                 /**< list of one letter options for this group of associated options to display */
} openiccOptionGroup_s;

/**
 *   @brief main command line, options and groups
 */
struct openiccOptions_s {
  char type [4];                       /**< must be 'oiws' */
  openiccOption_s * array;             /**< the options */
  openiccOptionGroup_s * groups;       /**< groups of options, which form a command */
  void * user_data;                    /**< will be passed to functions; optional */
  int argc;                            /**< plain reference from main(argc,argv) */
  char ** argv;                        /**< plain reference from main(argc,argv) */
  void * private_data;                 /**< internal state; private, do not use */
};
int    openiccOptions_Count          ( openiccOptions_s  * opts );
int    openiccOptions_CountGroups    ( openiccOptions_s  * opts );
openiccOption_s * openiccOptions_GetOption (
                                       openiccOptions_s  * opts,
                                       char                oc );
openiccOption_s * openiccOptions_GetOptionL(
                                       openiccOptions_s  * opts,
                                       char              * ostring );
/** @brief option state */
typedef enum {
  openiccOPTION_NONE,                  /**< untouched */
  openiccOPTION_USER_CHANGED,          /**< passed in by user */
  openiccOPTION_MISSING_VALUE,         /**< user error */
  openiccOPTION_UNEXPECTED_VALUE,      /**< user error */
  openiccOPTION_NOT_SUPPORTED,         /**< user error */
  openiccOPTION_DOUBLE_OCCURENCE       /**< user error */
} openiccOPTIONSTATE_e;
openiccOptions_s * openiccOptions_New( int                 argc,
                                       char             ** argv );
openiccOPTIONSTATE_e openiccOptions_Parse  (
                                       openiccOptions_s  * opts );
openiccOPTIONSTATE_e openiccOptions_GetResult (
                                       openiccOptions_s  * opts,
                                       char                oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int );
char * openiccOptions_ResultsToJson  ( openiccOptions_s  * opts );
char * openiccOptions_ResultsToText  ( openiccOptions_s  * opts );
typedef struct openiccUi_s openiccUi_s;
void   openiccOptions_PrintHelp      ( openiccOptions_s  * opts,
                                       openiccUi_s       * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... );
/** @brief Header section */
typedef struct openiccUiHeaderSection_s {
  char type [4];                       /**< must be 'oihs' */
  const char * nick;                   /**< single word well known identifier; *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *openicc_modules_author*, *documentation* */
  const char * label;                  /**< i18n short string, in case this section nick is not well known; optional */
  const char * name;                   /**< i18n short content */
  const char * description;            /**< i18n optional second string; might contain a browsable url for further information, e.g. a link to the full text license, home page; optional */
} openiccUiHeaderSection_s;

/** @brief Info for graphic UI's containing options, additional info sections and other bells and whistles */
struct openiccUi_s {
  char type [4];                       /**< must be 'oiui' */
  const char * app_type;               /**< "tool" or "module" */
  char         nick[8];                /**< four byte ID, e.g. "openicc" */
  const char * name;                   /**< i18n short name for tool bars, app lists */
  const char * description;            /**< i18n name, version, maybe more for a prominent one liner */
  const char * logo;                   /**< file name body without path, for relocation, nor file type ending; typical a PNG or SVG icon; e.g. "lcms_icon" for lcms_icon.png or lcms_icon.svg; optional */
  /** We describe here a particular tool/module. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *openicc_modules_author*, *documentation* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier. */
  openiccUiHeaderSection_s * sections;
  openiccOptions_s * opts;             /**< info for UI logic */
};
openiccUi_s *  openiccUi_New         ( int                 argc,
                                       char             ** argv );
int    openiccUi_CountHeaderSections ( openiccUi_s       * ui );
openiccUiHeaderSection_s * openiccUi_GetHeaderSection (
                                       openiccUi_s       * ui,
                                       const char        * nick );
char *       openiccUi_ToJson        ( openiccUi_s       * ui,
                                       int                 flags );

/** 
 *  @} *//* args
 */

#endif /* __OPENICC_CORE_H__ */
