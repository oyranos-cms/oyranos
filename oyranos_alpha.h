/** @file oyranos_alpha.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    misc alpha APIs
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

/** @file @internal
 *  @brief misc alpha API's
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_ALPHA_H
#define OYRANOS_ALPHA_H

#include "oyranos.h"
#include "oyranos_icc.h"

#include <inttypes.h>

#ifdef __cplusplus
namespace oyranos {
extern "C" {
#endif /* __cplusplus */


#define OY_IN_PLANING
#ifdef OY_IN_PLANING


void                    oyLab2XYZ       (const double * CIElab, double * XYZ);
void                    oyXYZ2Lab       (const double * XYZ, double * CIElab);
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );

typedef struct oyImage_s_ oyImage_s;


typedef oyStruct_s * (*oyStruct_Copy_f ) ( oyStruct_s *, oyPointer );
typedef int       (*oyStruct_Release_f ) ( oyStruct_s ** );
typedef oyPointer (*oyStruct_copy_f )  ( oyPointer, oyPointer );
typedef int       (*oyStruct_release_f ) ( oyPointer * );
typedef oyPointer (*oyStruct_LockCreate_f ) ( oyStruct_s * obj );
typedef void      (*oyLockRelease_f )( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyLock_f )       ( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyUnLock_f )     ( oyPointer           look,
                                       const char        * marker,
                                       int                 line );

typedef oyPointer (*oyImage_GetPoint_t)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel );
typedef oyPointer (*oyImage_GetLine_t) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel );
typedef oyPointer (*oyImage_GetTile_t) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel );


void         oyThreadLockingSet      ( oyStruct_LockCreate_f  createLockFunc,
                                       oyLockRelease_f     releaseLockFunc,
                                       oyLock_f            lockFunc,
                                       oyUnLock_f          unlockFunc );

/** @brief codeset for Oyranos
 *
 *  set here the codeset part, e.g. "UTF-8", which shall be delivered from
 *  Oyranos string translations.
 *  Set this variable before any call to Oyranos.
 */
extern const char *oy_domain_codeset;

/** @internal
 *  @brief Oyranos name structure
 *
 *  @since Oyranos: version 0.1.x
 *  @date  22 november 2007 (API 0.1.8)
 */
typedef enum {
  oyOBJECT_TYPE_NONE,
  oyOBJECT_TYPE_OBJECT_S,             /**< oyObject_s */
  oyOBJECT_TYPE_DISPLAY_S,            /**< oyDisplay_s */
  oyOBJECT_TYPE_NAMED_COLOUR_S,       /*!< oyNamedColour_s */
  oyOBJECT_TYPE_NAMED_COLOURS_S,      /*!< oyNamedColours_s */
  oyOBJECT_TYPE_PROFILE_S,            /*!< oyProfile_s */
  oyOBJECT_TYPE_PROFILE_TAG_S,        /*!< oyProfileTag_s */
  oyOBJECT_TYPE_PROFILES_S,           /*!< oyProfiles_s */
  oyOBJECT_TYPE_OPTION_S,             /*!< oyOption_s */
  oyOBJECT_TYPE_OPTIONS_S,            /*!< oyOptions_s */
  oyOBJECT_TYPE_WIDGET_S,             /**< oyWidget_s */
  oyOBJECT_TYPE_WIDGET_BUTTON_S,      /**< oyWidgetButton_s */
  oyOBJECT_TYPE_WIDGET_CHOICE_S,      /**< oyWidgetChoice_s */
  oyOBJECT_TYPE_WIDGET_GROUP_S,       /**< oyWidgetGroup_s */
  oyOBJECT_TYPE_WIDGET_SLIDER_S,      /**< oyWidgetSlider_s */
  oyOBJECT_TYPE_WIDGET_TEXT_S,        /**< oyWidgetText_s */
  oyOBJECT_TYPE_REGION_S,             /*!< oyRegion_s */
  oyOBJECT_TYPE_IMAGE_S,              /*!< oyImage_s */
  oyOBJECT_TYPE_COLOUR_CONVERSION_S,  /*!< oyColourConversion_s */
  oyOBJECT_TYPE_FILTER_S,             /**< oyFilter_s */
  oyOBJECT_TYPE_FILTERS_S,            /**< oyFilters_s */
  oyOBJECT_TYPE_FILTER_NODE_S,        /**< oyFilterNode_s */
  oyOBJECT_TYPE_PIXEL_ACCESS_S,       /**< oyPixelAccess_s */
  oyOBJECT_TYPE_CONVERSIONS_S,        /**< oyConversions_s */
  oyOBJECT_TYPE_CMM_HANDLE_S = 50,    /**< oyCMMhandle_s */
  oyOBJECT_TYPE_CMM_POINTER_S,        /*!< oyCMMptr_s */
  oyOBJECT_TYPE_CMM_INFO_S,           /*!< oyCMMInfo_s */
  oyOBJECT_TYPE_CMM_API_S,            /**< oyCMMapi_s */
  oyOBJECT_TYPE_CMM_API1_S,           /**< oyCMMapi1_s */
  oyOBJECT_TYPE_CMM_API2_S,           /**< oyCMMapi2_s */
  oyOBJECT_TYPE_CMM_API3_S,           /**< oyCMMapi3_s */
  oyOBJECT_TYPE_CMM_API4_S,           /**< oyCMMapi4_s */
  oyOBJECT_TYPE_CMM_API_MAX,          /**< not defined */
  oyOBJECT_TYPE_ICON_S = 80,          /*!< oyIcon_s */
  oyOBJECT_TYPE_MODULE_S,             /*!< oyModule_s */
  oyOBJECT_TYPE_EXTERNFUNC_S,         /*!< oyExternFunc_s */
  oyOBJECT_TYPE_NAME_S,               /*!< oyName_s */
  oyOBJECT_TYPE_COMP_S_,              /*!< oyComp_s_ */
  oyOBJECT_TYPE_FILE_LIST_S_,         /*!< oyFileList_s_ */
  oyOBJECT_TYPE_HASH_S,               /**< oyHash_s */
  oyOBJECT_TYPE_HANDLE_S,             /**< oyHandle_s */
  oyOBJECT_TYPE_STRUCT_LIST_S,        /**< oyStructList_s */
  oyOBJECT_TYPE_NODE_S,               /**< oyNode_s */
  oyOBJECT_TYPE_MAX
} oyOBJECT_TYPE_e;


typedef struct oyObject_s_* oyObject_s;

/** @brief Oyranos base structure
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
struct oyStruct_s {
  oyOBJECT_TYPE_e      type_;          /**< struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
};

oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size );
const char * oyStruct_TypeToText     ( const oyStruct_s  * oy_struct );

typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

#define oyNAME_ID oyNAME_NICK

/** @brief Oyranos name structure
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_NAME_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char               * nick;    /*!< few letters for mass representation, eg. "A1" */
  char               * name;           /*!< normal visible name, eg. "A1-MySys"*/
  char               * description;    /*!< full user description, eg. "A1-MySys from Oyranos" */
  char                 lang[8];        /**< i18n language, eg. "en_GB" */
} oyName_s;

oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
int          oyName_release_         ( oyName_s         ** name,
                                       oyDeAlloc_f         deallocateFunc );
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
const char * oyName_get_             ( const oyName_s    * obj,
                                       oyNAME_e            type );

typedef enum {
  oyBOOLEAN_INTERSECTION,              /** and, the part covered by A and B */
  oyBOOLEAN_SUBSTRACTION,              /** minus, the part covered by A but not by B */
  oyBOOLEAN_DIFFERENZ,                 /** xor, exclusive parts of A and B */
  oyBOOLEAN_UNION                      /** or, the parts covered by A or B or both */
} oyBOOLEAN_e;

int          oyName_boolean          ( oyName_s          * name_a,
                                       oyName_s          * name_b,
                                       oyNAME_e            name_type,
                                       oyBOOLEAN_e         type );

#define OY_HASH_SIZE 16

#if 0
/** @internal
 *  @brief a handle
 *
 *  allow for polymorphing\n
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  27 november 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_HANDLE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  oyPointer            ptr;            /**< can be any type */
  oyOBJECT_TYPE_e      ptr_type;       /**< the type of the entry */
  oyStruct_release_f   ptrRelease;     /**< deallocation for ptr_ of list_type*/
  oyStruct_copy_f      ptrCopy;        /**< copy for ptr_ of list_type */
} oyHandle_s;

oyHandle_s *       oyHandle_new_     ( oyAlloc_f           allocateFunc );
oyHandle_s *       oyHandle_copy_    ( oyHandle_s        * orig,
                                       oyAlloc_f           allocateFunc );
int                oyHandle_release_ ( oyHandle_s       ** handle );

int                oyHandle_set_     ( oyHandle_s        * handle,
                                       oyPointer           ptr,
                                       oyOBJECT_TYPE_e     ptr_type,
                                       oyStruct_release_f  ptrRelease,
                                       oyStruct_copy_f     ptrCopy );
#endif

typedef struct oyStructList_s oyStructList_s;

/** @internal
 *  @brief Oyranos structure base
 *
 *  The base object of Oyranos object system is self contained. It can be
 *  handled by the belonging function set. Complex objects for user interaction
 *  should almost all be derived from this struct.\n
 *  The memory management can be controlled by the user and will affect internal
 *  and derived data.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
struct oyObject_s_ {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_OBJECT_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  int                  id_;            /**< identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< data release function */
  oyPointer            parent_;        /*!< parent struct of parent_type */
  oyOBJECT_TYPE_e      parent_type_;   /*!< parents struct type */
  oyPointer            backdoor_;      /*!< allow non breaking extensions */
  oyStructList_s     * handles_;       /*!< useful as list of oyStruct_s */
  oyName_s           * name_;          /*!< naming feature */
  int                  ref_;           /*!< reference counter */
  int                  version_;       /*!< OYRANOS_VERSION */
  unsigned char        hash_[2*OY_HASH_SIZE];
  oyPointer            lock_;          /**< the user provided lock */
};

oyObject_s   oyObject_New             ( void );
oyObject_s   oyObject_NewWithAllocators(oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
oyObject_s   oyObject_NewFrom         ( oyObject_s        object );
oyObject_s   oyObject_Copy            ( oyObject_s        object );
int          oyObject_Release         ( oyObject_s      * oy );

oyObject_s   oyObject_SetParent       ( oyObject_s        object,
                                        oyOBJECT_TYPE_e   type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s        oy,
                                        size_t          * size,
                                        oyAlloc_f         allocateFunc );*/

int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
int          oyObject_SetName         ( oyObject_s        object,
                                        const char      * name,
                                        oyNAME_e          type );
const char  *oyObject_GetName         ( const oyObject_s  object,
                                        oyNAME_e          type );
/*oyCMMptr_s * oyObject_GetCMMPtr       ( oyObject_s        object,
                                        const char      * cmm );
oyObject_s   oyObject_SetCMMPtr       ( oyObject_s        object,
                                        oyCMMptr_s      * cmm_ptr );*/
int          oyObject_Lock             ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnLock           ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnSetLocking     ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_GetId          ( oyObject_s        object );


/** @internal
 *  @brief a cache entry
 *
 *  Combine hash, description and oyPointer to one searchable struct. The struct
 *  can be used in a oyStructList_s for a hash map or searchable cache.
 *  @see oyCacheListNew_ oyHashGet_
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_HASH_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
  oyStruct_s         * entry;          /**< holds a pointer to something */
} oyHash_s;

oyHash_s *         oyHash_New_       ( oyObject_s          object );
oyHash_s *         oyHash_Get_       ( const char        * hash_text,
                                       oyObject_s          object );
oyHash_s *         oyHash_Copy_      ( oyHash_s          * entry,
                                       oyObject_s          object );
int                oyHash_Release_   ( oyHash_s         ** entry );

int                oyHash_IsOf_      ( oyHash_s          * hash,
                                       oyOBJECT_TYPE_e     type );
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_TYPE_e     type );
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj );

/** @internal
 *  @brief a pointer list
 *
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
struct oyStructList_s {
  oyOBJECT_TYPE_e      type_;          /*!< internal struct type oyOBJECT_TYPE_STRUCT_LIST_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
  oyStruct_s        ** ptr_;           /**< the list data */
  int                  n_;             /**< the number of visible pointers */
  int                  n_reserved_;    /**< the number of allocated pointers */
  char               * list_name;      /**< name of list */
};

oyStructList_s * oyStructList_New    ( oyObject_s          object );
oyStructList_s * oyStructList_Copy   ( oyStructList_s    * list,
                                       oyObject_s          obj );
int              oyStructList_Release( oyStructList_s   ** list );

int              oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos );
/*oyStruct_s **    oyStructList_GetRaw_( oyStructList_s    * list );*/
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos );
oyStruct_s *     oyStructList_GetType_(oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_TYPE_e     type );
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos );
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                       int                 pos,
                                       oyOBJECT_TYPE_e     type );
int              oyStructList_ReferenceAt_( oyStructList_s * list,
                                       int                 pos );
int              oyStructList_ReleaseAt( oyStructList_s  * list,
                                       int                 pos );
int              oyStructList_Count  ( oyStructList_s    * list );



oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       const char        * hash_text );
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text );
oyStructList_s** oyCMMCacheList_     ( void );
char   *     oyCMMCacheListPrint_    ( void );


/* --- colour conversions --- */

/** @enum    oyVALUETYPE_e
 *  @brief   a value type
 *
    @see     oyValue_u
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef enum {
  oyVAL_INT,
  oyVAL_INT_LIST,
  oyVAL_DOUBLE,    /*!< IEEE double precission floating point number */
  oyVAL_DOUBLE_LIST,
  oyVAL_STRING,
  oyVAL_STRING_LIST
} oyVALUETYPE_e;

/** @union   oyValue_u
 *  @brief   a value
 *
    @see     oyVALUETYPE_e
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef union {
  int32_t          int32;
  int32_t        * int32_list;         /**< first is number of int32 in list */
  double           dbl;
  double         * dbl_list;           /**< first is number of dbl in list */
  char           * string;             /**< null terminated */
  char          ** string_list;        /**< null terminated */
} oyValue_u;

void           oyValueCopy           ( oyValue_u         * from,
                                       oyValue_u         * to,
                                       oyVALUETYPE_e       type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueRelease        ( oyValue_u        ** value,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );

/** @brief Option for rendering

    @todo include the oyOptions_t_ type for gui elements
    should be used in a list oyColourTransformOptions_s to form a options set

 *  The id field maps to a oyWidget_s object.
 *  Options and widgets are to be queried by the according function / CMM
 *  combination.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.x)
 *  @date    2008/04/14
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_OPTION_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  uint32_t             id;             /**< id to map for instance to events and widgets */
  oyName_s             name;           /**< nick, name, description/help, e.g. "radius" "Radius" "..." */
  const char         * registration;    /**< full key name to store configuration, e.g. "org.oyranos.generic.scale.none,linear,cubic", config key name will be name.nick */
  int                  version[3];     /**< as for oyCMMapi4_s::version */
  oyVALUETYPE_e        value_type;     /**< the type in value */
  oyValue_u          * value;          /**< the actual value */
  oyValue_u          * standard;       /**< the standard value */
  oyValue_u          * start;          /**< value range start */
  oyValue_u          * end;            /**< value range end */
  uint32_t             flags;          /**<  */
} oyOption_s;

oyOption_s *   oyOption_New          ( oyObject_s          object,
                                       const char        * name );
oyOption_s *   oyOption_Copy         ( oyOption_s        * option,
                                       oyObject_s          object );
int            oyOption_Release      ( oyOption_s       ** option );


int            oyOption_GetId        ( oyOption_s        * option );


/** @brief Options for rendering
    Options can be any flag or rendering intent and other informations needed to
    configure a process. The object contains variables for colour transforms.
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_OPTIONS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list;
} oyOptions_s;

oyOptions_s *  oyOptions_FromMem     ( size_t            * size,
                                       const char        * opts_text,
                                       oyObject_s          object );
oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * pattern,
                                       oyOptions_s       * options,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object );
oyOptions_s *  oyOptions_Copy        ( oyOptions_s       * options,
                                       oyObject_s          object );
int            oyOptions_Release     ( oyOptions_s      ** options );


int            oyOptions_ReleaseAt   ( oyOptions_s       * list,
                                       int                 pos );
oyOption_s *   oyOptions_Get         ( oyOptions_s       * list,
                                       int                 pos );
int            oyOptions_Count       ( oyOptions_s       * list );
int            oyOptions_MoveIn      ( oyOptions_s       * options,
                                       oyOption_s       ** option );
int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option );
char *         oyOptions_GetMem      ( oyOptions_s       * options,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );


/** @brief general profile infos
 *
 *  use for oyProfile_GetSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
 */
typedef enum {
  oySIGNATURE_COLOUR_SPACE,            /**< colour space */
  oySIGNATURE_PCS,                     /**< profile connection space */
  oySIGNATURE_SIZE,                    /**< internal stored size */
  oySIGNATURE_CMM,                     /**< prefered CMM */
  oySIGNATURE_VERSION,                 /**< version */
  oySIGNATURE_CLASS,                   /**< usage class, e.g. 'mntr' ... */
  oySIGNATURE_MAGIC,                   /**< magic; ICC: 'acsp' */
  oySIGNATURE_PLATFORM,                /**< operating system */
  oySIGNATURE_OPTIONS,                 /**< various ICC header flags */
  oySIGNATURE_MANUFACTURER,            /**< device manufacturer */
  oySIGNATURE_MODEL,                   /**< device modell */
  oySIGNATURE_INTENT,                  /**< seldom used profile claimed intent*/
  oySIGNATURE_CREATOR,                 /**< profile creator ID */
  oySIGNATURE_DATETIME_YEAR,           /**< creation time in UTC */
  oySIGNATURE_DATETIME_MONTH,          /**< creation time in UTC */
  oySIGNATURE_DATETIME_DAY,            /**< creation time in UTC */
  oySIGNATURE_DATETIME_HOURS,          /**< creation time in UTC */
  oySIGNATURE_DATETIME_MINUTES,        /**< creation time in UTC */
  oySIGNATURE_DATETIME_SECONDS,        /**< creation time in UTC */
  oySIGNATURE_MAX
} oySIGNATURE_TYPE_e;

typedef struct oyProfileTag_s oyProfileTag_s;

/** @brief a profile and its attributes
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_PROFILE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  char               * file_name_;     /*!< file name for loading on request */
  size_t               size_;          /*!< ICC profile size */
  void               * block_;         /*!< ICC profile data */
  icColorSpaceSignature sig_;          /*!< ICC profile signature */
  oyPROFILE_e          use_default_;   /*!< if > 0 : take from settings */
  oyObject_s         * names_chan_;    /*!< user visible channel description */
  int                  channels_n_;    /*!< number of channels */
  oyStructList_s     * tags_;          /**< list of header + tags */
} oyProfile_s;

OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromStd ( oyPROFILE_e         colour_space,
                                       oyObject_s          object);
/** @type oyIO_t 
    parametric type as shorthand for IO flags \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    ------ W R \n

    R  no cache read \n
    W  no cache write \n

 */
#define oyNO_CACHE_READ             0x01        /** read not from cache */
#define oyNO_CACHE_WRITE            0x02        /** write not from cache */
/* decode */
#define oyToNoCacheRead_m(r)        ((r)&1)
#define oyToNoCacheWrite_m(w)       (((w) >> 1)&1)
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromFile( const char*         file_name,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMem ( size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromSignature(
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_Copy    ( oyProfile_s       * profile,
                                       oyObject_s          object);
OYAPI int  OYEXPORT oyProfile_Release( oyProfile_s      ** profile );
#if 0
OYAPI oyPointer OYEXPORT
                   oyProfile_Align   ( oyProfile_s     * profile,
                                       size_t          * size,
                                       oyAlloc_f         allocateFunc );
#endif
OYAPI int OYEXPORT oyProfile_GetChannelsCount ( oyProfile_s * colour );
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name );

OYAPI icSignature OYEXPORT
             oyProfile_GetSignature (  oyProfile_s       * profile,
                                       oySIGNATURE_TYPE_e  type );
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type );
OYAPI void OYEXPORT oyProfile_SetChannelNames( oyProfile_s * colour,
                                       oyObject_s        * names_chan );
OYAPI const oyObject_s * OYEXPORT
                   oyProfile_GetChannelNames( oyProfile_s * colour);
OYAPI const char   * OYEXPORT
                   oyProfile_GetChannelName ( oyProfile_s * profile,
                                         int               channel_pos,
                                         oyNAME_e          type );
OYAPI const char  * OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile );
OYAPI const char  * OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * profile,
                                       oyNAME_e            type );
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc );
oyProfileTag_s *   oyProfile_GetTagByPos ( oyProfile_s   * profile,
                                       int                 pos );
oyProfileTag_s *   oyProfile_GetTagById ( oyProfile_s    * profile,
                                       icTagSignature      id );
int                oyProfile_GetTagCount( oyProfile_s    * profile );
int                oyProfile_AddTag  ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** tag,
                                       int                 pos );
int                oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos );
const char   *     oyProfile_GetFileName ( oyProfile_s   * profile,
                                       int                 dl_pos );


/** @brief tell about the conversion profiles
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_PROFILE_LIST_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  oyStructList_s     * list_;          /**< list of profiles */
} oyProfiles_s;

OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_New      ( oyObject_s          object );
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Copy     ( oyProfiles_s      * profile_list,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyProfiles_Release  ( oyProfiles_s     ** profile_list );


oyProfiles_s   * oyProfiles_MoveIn   ( oyProfiles_s      * list,
                                       oyProfile_s      ** ptr,
                                       int                 pos );
int              oyProfiles_ReleaseAt( oyProfiles_s      * list,
                                       int                 pos );
oyProfile_s *    oyProfiles_Get      ( oyProfiles_s      * list,
                                       int                 pos );
int              oyProfiles_Count    ( oyProfiles_s      * list );
 

typedef enum {
  oyOK,
  oyCORRUPTED,
  oyUNDEFINED
} oySTATUS_e;

/** @struct oyProfileTag_s
 *  @brief  a profile constituting element
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
struct oyProfileTag_s {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_PROFILE_TAG_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  icTagSignature       use;            /**< tag functionality inside profile */
  icTagTypeSignature   tag_type_;      /**< tag type to decode the data block */

  oySTATUS_e           status_;        /**< status at load time */

  size_t               offset_orig;    /**< information from profile read */
  size_t               size_;          /**< data block size */
  oyPointer            block_;         /**< the data to interprete */

  char                 profile_cmm_[5];/**< the profile prefered CMM */
  char                 required_cmm[5];/**< selected a certain CMM */
  char                 last_cmm_[5];   /**< info: last processing CMM */
};

OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_New      ( oyObject_s          object );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       const char        * required_cmm,
                                       oyObject_s          object);
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Copy     ( oyProfileTag_s    * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
               oyProfileTag_Release  ( oyProfileTag_s   ** obj );

OYAPI int  OYEXPORT
               oyProfileTag_Set      ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block );
char   **      oyProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAlloc_f           allocateFunc );


typedef enum {
  oyDATALAYOUT_NONE,
  oyDATALAYOUT_CURVE,                 /**< equally spaced curve, oyDATALAYOUT_e[0], size[1], min[2], max[3], elements[4]... */
  oyDATALAYOUT_MATRIX,                /**< 3x3 matrix, oyDATALAYOUT_e[0], a1[1],a2[2],a3,b1,b2,b3,c1,c2,c3 */
  oyDATALAYOUT_TABLE,                 /**< CLUT, oyDATALAYOUT_e[0], table dimensions in[1], array out[2], size of first dimension[3], size of second [4], ... size of last[n], elements[n+1]... */
  oyDATALAYOUT_ARRAY,                 /**< value array, oyDATALAYOUT_e[0], size[1], elements[2]... */
  /*oyDATALAYOUT_PICEWISE_CURVE,*/       /**< paired curve, layout as in oyDATA_LAYOUT_CURVE but with elements grouped to two */
  /*oyDATALAYOUT_HULL,*/              /**< oyDATALAYOUT_e[0], triangle count[1], indixes[2], XYZ triples[3..5]..., followed by interwoven index + normale + midpoint triples[n]... */
  /*oyDATALAYOUT_,*/                /**< */
  oyDATALAYOUT_MAX                    /**< */
} oyDATALAYOUT_e;


/** @brief start with a simple rectangle */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< internal struct type oyOBJECT_TYPE_REGION_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;
  double x;
  double y;
  double width;
  double height;
} oyRegion_s;

oyRegion_s *   oyRegion_New_         ( oyObject_s          object );
oyRegion_s *   oyRegion_NewWith      ( oyObject_s          object,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
oyRegion_s *   oyRegion_NewFrom      ( oyRegion_s        * ref,
                                       oyObject_s          object );
oyRegion_s *   oyRegion_Copy         ( oyRegion_s        * region,
                                       oyObject_s          object );
int            oyRegion_Release      ( oyRegion_s       ** region );

void           oyRegion_SetGeo       ( oyRegion_s        * edit_region,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
void           oyRegion_SetByRegion  ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_Trim         ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_MoveInside   ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_Scale        ( oyRegion_s        * edit_region,
                                       double              factor );
void           oyRegion_Normalise    ( oyRegion_s        * edit_region );
void           oyRegion_Round        ( oyRegion_s        * edit_region );
int            oyRegion_IsEqual      ( oyRegion_s        * region1,
                                       oyRegion_s        * region2 );
int            oyRegion_IsInside     ( oyRegion_s        * region,
                                       double              x,
                                       double              y );
int            oyRegion_CountPoints ( oyRegion_s        * region );
int            oyRegion_Index        ( oyRegion_s        * region,
                                       double              x,
                                       double              y );
char  *        oyRegion_Show         ( oyRegion_s        * region );


typedef enum {
  oyUINT8,     /*!<  8-bit integer */
  oyUINT16,    /*!< 16-bit integer */
  oyUINT32,    /*!< 32-bit integer */
  oyHALF,      /*!< 16-bit floating point number */
  oyFLOAT,     /*!< IEEE floating point number */
  oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE_e;


/** @type oyPixel_t 
    parametric type as shorthand for the channel layout in bitmaps \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    X F P S TTTT OOOOOOOO CCCCCCCC \n

    C  channels count per pixel (3 for RGB); max 255 \n
    O  colour channel offset (0 for RGB, 1 for ARGB) \n
    P  Planar bit: 0 - interwoven, 1 - one channel after the other \n
    S  Swap colour channels bit (BGR) \n
    T  Type oyDATATYPE_e \n
    X  non host byte order bit \n
    F  Revert bit: 0 - MinIsBlack(Chocolate) 1 - MinIsWhite(Vanilla); \n
       exchange min and max : (1-x)

 */
typedef uint32_t oyPixel_t;

#define oyChannels_m(c)             (c)
#define oyColourOffset_m(o)         ((o) << 8)
#define oyDataType_m(t)             ((t) << 16)
#define oySwapColourChannels_m(s)   ((s) << 20)
#define oyPlanar_m(p)               ((p) << 21)
#define oyFlavor_m(p)               ((f) << 22)
#define oyByteSwap_m(x)             ((x) << 23)

/** define some common types */
#define oyTYPE_123_8       (oyChannels_m(3)|oyDataType_m(oyUINT8))
#define oyTYPE_123_16      (oyChannels_m(3)|oyDataType_m(oyUINT16))
#define oyTYPE_123_HALF    (oyChannels_m(3)|oyDataType_m(oyHALF))
#define oyTYPE_123_FLOAT   (oyChannels_m(3)|oyDataType_m(oyFLOAT))
#define oyTYPE_123_DBL     (oyChannels_m(3)|oyDataType_m(oyDOUBLE))

#define oyTYPE_123A_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define oyTYPE_123A_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define oyTYPE_123A_HALF   (oyChannels_m(4)|oyDataType_m(oyHALF))
#define oyTYPE_123A_FLOAT  (oyChannels_m(4)|oyDataType_m(oyFLOAT))
#define oyTYPE_123A_DBL    (oyChannels_m(4)|oyDataType_m(oyDOUBLE))

#define oyTYPE_123AZ_HALF  (oyChannels_m(5)|oyDataType_m(oyHALF))
#define oyTYPE_123AZ_FLOAT (oyChannels_m(5)|oyDataType_m(oyFLOAT))
#define oyTYPE_123AZ_DBL   (oyChannels_m(5)|oyDataType_m(oyDOUBLE))

#define oyTYPE_A123_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8))
#define oyTYPE_A123_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16))

#define oyTYPE_A321_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8)|oySwapColourChannels_m(oyYES))
#define oyTYPE_A321_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16)|oySwapColourChannels_m(oyYES))

/* some intermixed types will work as well */
#define oyTYPE_123A_HALF_Z_FLOAT (oyChannels_m(4+1*2)|oyDataType_m(oyHALF))

#define oyTYPE_1234_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define oyTYPE_1234A_8     (oyChannels_m(5)|oyDataType_m(oyUINT8))
#define oyTYPE_1234_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define oyTYPE_1234A_16    (oyChannels_m(5)|oyDataType_m(oyUINT16))

#define oyTYPE_1234_8_REV  (oyChannels_m(4)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define oyTYPE_1234A_8_REV (oyChannels_m(5)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define oyTYPE_1234_16_REV (oyChannels_m(4)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define oyTYPE_1234A_16_REV (oyChannels_m(5)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))


#define oyTYPE_1_8         (oyChannels_m(1)|oyDataType_m(oyUINT8))
#define oyTYPE_1_16        (oyChannels_m(1)|oyDataType_m(oyUINT16))
#define oyTYPE_1_HALF      (oyChannels_m(1)|oyDataType_m(oyHALF))
#define oyTYPE_1_FLOAT     (oyChannels_m(1)|oyDataType_m(oyFLOAT))
#define oyTYPE_1A_8        (oyChannels_m(2)|oyDataType_m(oyUINT8))
#define oyTYPE_1A_16       (oyChannels_m(2)|oyDataType_m(oyUINT16))
#define oyTYPE_1A_HALF     (oyChannels_m(2)|oyDataType_m(oyHALF))
#define oyTYPE_1A_FLOAT    (oyChannels_m(2)|oyDataType_m(oyFLOAT))

#define oyTYPE_1_8_REV     (oyChannels_m(1)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define oyTYPE_1_16_REV    (oyChannels_m(1)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define oyTYPE_1_HALF_REV  (oyChannels_m(1)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define oyTYPE_1_FLOAT_REV (oyChannels_m(1)|oyDataType_m(oyFLOAT))|oyFlavor_m(oyYes)
#define oyTYPE_1A_8_REV    (oyChannels_m(2)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define oyTYPE_1A_16_REV   (oyChannels_m(2)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define oyTYPE_1A_HALF_REV (oyChannels_m(2)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define oyTYPE_1A_FLOAT_REV (oyChannels_m(2)|oyDataType_m(oyFLOAT)|oyFlavor_m(oyYes))

/* decode */
#define oyToChannels_m(c)           ((c)&255)
#define oyToColourOffset_m(o)       (((o) >> 8)&255)
#define oyToDataType_m(t)           (((t) >> 16)&15)
#define oyToSwapColourChannels_m(s) (((s) >> 20)&1)
#define oyToPlanar_m(p)             (((p) >> 21)&1)
#define oyToFlavor_m(f)             (((f) >> 22)&1)
#define oyToByteswap_m(x)           (((x) >> 23)&1)

#if 0
char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc );
#endif

/** @brief a reference struct to gather information for image transformation

    as we dont target a complete imaging solution, only raster is supported

    oyImage_s should hold image dimensions,
    oyDisplayRegion_s information and
    a reference to the data for conversion

    As well referencing of itself would be nice, to allow light copies.

    The resolution is always in pixel per centimeter.

    Should oyImage_s become internal and we provide a user interface?
 */
struct oyImage_s_ {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_IMAGE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyRegion_s         * image_dimension;/**< image dimensions */
  double               resolution_x;   /**< resolution in horizontal direction*/
  double               resolution_y;   /**< resolution in vertical direction */

  oyPixel_t          * layout_;        /*!< samples mask */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyOptions_s        * options_;       /*!< for instance channel layout (?) */
  oyProfile_s        * profile_;       /*!< image profile */
  oyRegion_s        ** regions;        /*!< region to render, if zero render all */
  int                  display_pos_x;  /**< Possibly this can be part of the output profile; upper position on display of image*/
  int                  display_pos_y;  /*!< left position on display of image */


  oyPointer            data;           /**< image data */
  oyImage_GetPoint_t   getPoint;       /**< the point interface */
  oyImage_GetLine_t    getLine;        /**< the line interface */
  oyImage_GetTile_t    getTile;        /**< the tile interface */
  int                  tile_width;     /**< needed by the tile interface */
  int                  tile_height;    /**< needed by the tile interface */
  oyStruct_s         * user_data;      /**< user provided pointer */
};


oyImage_s *    oyImage_Create        ( int                 width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyObject_s          object);
oyImage_s *    oyImage_CreateForDisplay( int               width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 display_pos_x,
                                       int                 display_pos_y,
                                       oyObject_s          object);
oyImage_s *    oyImage_Copy          ( oyImage_s         * image,
                                       oyObject_s          object );
int            oyImage_Release       ( oyImage_s        ** image );


int            oyImage_SetCritical   ( oyImage_s         * image,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyOptions_s       * options );


typedef enum {
  oyFILTER_TYPE_NONE,                  /**< nothing */
  oyFILTER_TYPE_COLOUR,                /**< colour */
  oyFILTER_TYPE_TONEMAP,               /**< contrast or tone mapping */
  oyFILTER_TYPE_IMAGE,                 /**< image */
  oyFILTER_TYPE_GENERIC,               /**< generic */
  oyFILTER_TYPE_MAX
} oyFILTER_TYPE_e;

const char *   oyFilterTypeToText    ( oyFILTER_TYPE_e     filter_type,
                                       oyNAME_e            type );

typedef enum {
  oyFILTER_REG_NONE,
  oyFILTER_REG_TOP,
  oyFILTER_REG_VENDOR,
  oyFILTER_REG_TYPE,                   /**< oyFilterTypeToText/oyFILTER_TYPE_e*/
  oyFILTER_REG_NAME,
  oyFILTER_REG_FEATURES,
  oyFILTER_REG_MAX
} oyFILTER_REG_e;

char * oyFilterRegistrationToText    ( const char        * registration,
                                       oyFILTER_REG_e      type,
                                       oyFILTER_TYPE_e   * filter_type,
                                       oyAlloc_f           allocateFunc );
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern );

typedef struct oyFilter_s oyFilter_s;
typedef struct oyCMMapi4_s oyCMMapi4_s;
typedef struct oyFilterNode_s oyFilterNode_s;

/** @struct oyFilter_s
 *  @brief  a filter to manipulate a image
 *
 *  This is the Oyranos filter object. There are basic classes of filters.
 *  Filters are a container concept. They can contain various data and options.
 *  Filters can be manipulated by changing their options or profiles.
 *
 *  Filters are chained into a oyConversions_s in order to get applied to data.
 *  This happens by the oyFilterNode_s struct.
 *
 *  The oyFILTER_TYPE_e describes different basic types of filters.
 *  - oyFILTER_TYPE_COLOUR filters contain only profiles and options. They can grab their surounding and concatenate the neighbour profiles to one profile transform for speed.
 *  - oyFILTER_TYPE_TONEMAP filters are similiar to oyFILTER_TYPE_COLOUR except they can work in a two dimensional domain to apply to HDR content. This distinction is driven by usage. A oyFILTER_TYPE_TONEMAP filter may contain profiles and options. But this is not required.
 *  - oyFILTER_TYPE_IMAGE is a container for one oyImage_s.
 *  - oyFILTER_TYPE_GENERIC can be used for lots of things. It is the most flexible one and can contain any kind of data except profiles and images.
 *
 *  @param   profile_in
 *  @param   profile_out               should not be zero for a CMM.
 *  For a non CMM filter the oyProfile_s' can be a icColorSpaceSignature only. 
 *  @param   type                      is the functional type of filter 
 *  @param   category                  is useful for building menues
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */
struct oyFilter_s {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_FILTER_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  const char         * registration_;  /**< a registration name, e.g. "org.oyranos.generic.scale.none,linear,cubic" */
  oyName_s           * name_;          /**< nick, name, description/help */
  char                 cmm_[8];        /**< cmm name to look up for infos */

  oyFILTER_TYPE_e      filter_type_;   /**< filter type */
  char               * category_;      /**< the ui menue category for this filter */

  oyOptions_s        * options_;       /**< local options */
  char               * opts_ui_;       /**< xml ui elements for filter options*/

  oyImage_s          * image_;         /**< image, used for oyFILTER_TYPE_IMAGE */
  oyProfiles_s       * profiles_;      /**< profiles */

  oyCMMapi4_s        * api_;           /**<  */
};

oyFilter_s * oyFilter_New            ( oyFILTER_TYPE_e     type,
                                       const char        * registration,
                                       const char        * cmm,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
oyFilter_s * oyFilter_Copy           ( oyFilter_s        * filter,
                                       oyObject_s          object );
int          oyFilter_Release        ( oyFilter_s       ** filter );


const char * oyFilter_GetText        ( oyFilter_s        * filter,
                                       oyNAME_e            name_type );
const char * oyFilter_GetName        ( oyFilter_s        * filter,
                                       oyNAME_e            name_type );
const char * oyFilter_CategoryGet    ( oyFilter_s        * filter,
                                       int                 nontranslated );
#define oyFILTER_SET_TEST              0x01        /** only test */
#define oyFILTER_GET_DEFAULT           0x01        /** defaults */
/* decode */
#define oyToFilterSetTest_m(r)         ((r)&1)
#define oyToFilterGetDefaults_m(r)     ((r)&1)
oyOptions_s* oyFilter_OptionsSet     ( oyFilter_s        * filter,
                                       oyOptions_s       * options,
                                       int                 flags );
oyOptions_s* oyFilter_OptionsGet     ( oyFilter_s        * filter,
                                       int                 flags );
const char * oyFilter_WidgetsSet     ( oyFilter_s        * filter,
                                       const char        * widgets,
                                       int                 flags );
const char * oyFilter_WidgetsGet     ( oyFilter_s        * filter,
                                       int                 flags );
oyProfiles_s*oyFilter_ProfilesSet    ( oyFilter_s        * filter,
                                       oyProfiles_s      * profiles,
                                       int                 flags );
oyProfiles_s*oyFilter_ProfilesGet    ( oyFilter_s        * filter,
                                       int                 flags );
int          oyFilter_ImageSet       ( oyFilter_s        * filter,
                                       oyImage_s         * image );
oyImage_s *  oyFilter_ImageGet       ( oyFilter_s        * filter );
oyPointer    oyFilter_TextToInfo     ( oyFilter_s        * filter,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );


/** @struct  oyFilters_s
 *  @brief   a Filters list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_FILTERS_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list_;          /**< the list data */
} oyFilters_s;

OYAPI oyFilters_s * OYEXPORT
                 oyFilters_New       ( oyObject_s          object );
OYAPI oyFilters_s * OYEXPORT
                 oyFilters_Copy      ( oyFilters_s       * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilters_Release   ( oyFilters_s      ** list );


OYAPI oyFilters_s * OYEXPORT
                 oyFilters_MoveIn    ( oyFilters_s       * list,
                                       oyFilter_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilters_ReleaseAt ( oyFilters_s       * list,
                                       int                 pos );
OYAPI oyFilter_s * OYEXPORT
                 oyFilters_Get       ( oyFilters_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilters_Count     ( oyFilters_s       * list );

/** @struct  oyNode_s
 *  @brief   a Node object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/11 (Oyranos: 0.1.8)
 *  @date    2008/07/11
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_NODE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * parents;        /**< parent nodes */
  oyStructList_s     * children;       /**< child nodes */
} oyNode_s;

OYAPI oyNode_s * OYEXPORT
                 oyNode_New          ( oyObject_s          object );
OYAPI oyNode_s * OYEXPORT
                 oyNode_Copy         ( oyNode_s          * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyNode_Release      ( oyNode_s         ** list );



/** @struct  oyFilterNode_s
 *  @brief   a FilterNode object
 *
 *  This object provides support for separation of options from chaining.
 *  So it will be possible to implement options changing, which can affect
 *  the same filter instance in different graphs.
 *
 *  This struct adheres additionally to the oyStruct_s to the oyNode_s layout.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
struct oyFilterNode_s {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_FILTER_NODE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * parents;        /**< parent nodes */
  oyStructList_s     * children;       /**< child nodes */

  oyFilter_s         * filter;         /**< the filter */

  oyFilterNode_s     * merged_to;      /**< the node, which does processing */

  oyStruct_s         * data;           /**< the filters private data */
};

oyFilterNode_s *   oyFilterNode_New  ( oyObject_s          object );
oyFilterNode_s *   oyFilterNode_Copy ( oyFilterNode_s    * node,
                                       oyObject_s          object );
int          oyFilterNode_Release    ( oyFilterNode_s   ** node );

int          oyFilterNode_FilterSet  ( oyFilterNode_s    * node,
                                       oyFilters_s       * parents,
                                       oyFilters_s       * children,
                                       int                 flags );
int          oyFilterNode_FilterGet  ( oyFilterNode_s    * node,
                                       oyFilters_s      ** parents,
                                       oyFilters_s      ** cildren );


typedef struct oyPixelAccess_s oyPixelAccess_s;

/** @struct  oyPixelAccess_s
 *  @brief   control pixel access order
 *
 *  A struct to control pixel access. It is a kind of flexible pixel 
 *  iterator. The order or pattern of access is defined by the array_xy and
 *  start_[x,y] variables.
 *
 *  The array_index specifies the iterator position in the array_xy index
 *  array or request region.
 *
 *  The cache is the reason why this struct is separate from a oyFilterNode_s.
 *
 *  array_cache_pixels says how many pixels are to be processed for the cache.
 *  array_cache_pixels is used to calculate the buffers located with getBuffer
 *  and freeBuffer.
 *  The amount of pixel specified in array_cache_pixels must be processed by
 *  each filter, because other filters are relying on a properly filled cache.
 *  This variable also determins the size of the next iteration.
 *
 *  The relation of array_cache_pixels to array_xy and start_[x,y] is that a
 *  minimum of array_cache_pixels must be processed by starting with start_[x,y]
 *  and processing array_cache_pixels through array_xy. array_xy specifies
 *  the offset pixel distance to a next pixel in x and y directions. In case
 *  array_cache_pixels is larger than array_n the array_xy has to be continued
 *  at array_xy[0,1] after reaching its end (array_n). \n
 *  \b Example: \n
 *  Thus a line iterator behaviour can be specified by simply setting 
 *  array_xy = {1,0}, array_n = 1 and array_cache_pixels = image_width.
 *
 *  The two cases for handling pixel access are array_xy or
 *  a set request variable. They are toupported by a filter in
 *  a function of type oyCMMFilter_GetNext_f() in
 *  oyCMMapi4_s::oyCMMFilter_GetNext.
 *
 *  Access to the buffers by concurrenting threads is handled by passing
 *  different oyPixelAccess_s objects pre thread.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/04 (Oyranos: 0.1.8)
 *  @date    2008/07/04
 */
struct oyPixelAccess_s {
  oyOBJECT_TYPE_e      type;           /**< internal struct type oyOBJECT_TYPE_PIXEL_ACCESS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  int32_t          start_xy[2];        /**< the start point */
  int32_t          start_xy_old[2];    /**< the previous start point */
  int32_t        * array_xy;           /**< array of shifts, e.g. 1,0,2,0,1,0 */
  int              array_n;            /**< the number of points in array_xy */
  oyRegion_s     * request;            /**< requested image region */
  int              index;              /**< to be advanced by the last caller */
  size_t           array_cache_pixels; /**< pixels to process/cache at once */

  int32_t          workspace_id;       /**< a ID to assign distinct resources to */
  oyPointer      (*getBuffers)(  size_t            size,  /**< in bytes */
                                 int               input, /**< 0 for data_out */
                                 oyPixelAccess_s * ref );
  int            (*freeBuffers)( oyPointer,  /**< free intermediate data */
                                 oyPixelAccess_s * ref );

  oyPointer        data_in;            /**< input data cache */
  oyPointer        data_out;           /**< output data cache */
};

typedef enum {
  oyPIXEL_ACCESS_IMAGE,                /**< this requires to omit array_xy and work on native pixel representations like tiles or scanlines */
  oyPIXEL_ACCESS_POINT,
  oyPIXEL_ACCESS_LINE
} oyPIXEL_ACCESS_TYPE_e;

oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilter_s        * filter,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object );
oyPixelAccess_s *  oyPixelAccess_Copy( oyPixelAccess_s   * obj,
                                       oyObject_s          object );
int                oyPixelAccess_Release(
                                       oyPixelAccess_s  ** obj );

/** @struct oyConversions_s
 *  @brief  a filter chain or graph to manipulate a image
 *
 *  Order of filters matters. \n
 *  The idea is a bit like raytracing:
 *  @verbatim
    output_image -ask filter C-> C -ask filter B and process-> B ->ask filter A and process-> A ->ask source_image-> input_image @endverbatim
 *
 *  The graph is allowed to be a directed graph without cycles.
 *  oyConversions_s shall provide access to the graph and help in processing
 *  and managing nodes.\n
 *
 *  \b Creating \b graphs: \n
 *  Most simple is to use the oyConversions_CreateBasic() function to create
 *  a profile to profile and possible image buffer to image buffer linear
 *  graph.\n
 *  The next possibility is to create a linear graph by chaining linear nodes
 *  together with oyConversions_CreateInput(), oyConversions_FilterAdd() and
 *  oyConversions_OutputAdd() in that order. A linear node is one that can have
 *  exactly one parent and one child node. The above scheme illustrates a linear
 *  graph.\n
 *  The last possibility is to create a non linear graph. The input member
 *  be accessed for this directly.
 *
 *  While it would be possible to have several open ends in a graph, there
 *  are two endpoints considered as special. Th input member prepresents the
 *  top most required node to be provided in a oyConversions_s graph. The
 *  input node is accessible for use manipulation. The other one is the out_
 *  member. It is the closing node in the graph. It will be set by Oyranos
 *  during processing the graph, e.g. in oyConversions_OutputAdd().
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_CONVERSIONS_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyFilterNode_s     * input;          /**< the input image filter; Most users will start logically with this pice and chain their filters to get the final result. */
  oyFilterNode_s     * out_;           /**< the Oyranos output image. Oyranos will stream the filters starting from the end. */
  oyPixelAccess_s    * one_pixel_cfg;  /**< one pixel accessor */
} oyConversions_s;

oyConversions_s  * oyConversions_CreateBasic (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
oyConversions_s  * oyConversions_CreateInput (
                                       oyImage_s         * input,
                                       oyObject_s          object );
oyConversions_s  * oyConversions_Copy( oyConversions_s   * conversions,
                                       oyObject_s          object );
int                oyConversions_Release (
                                       oyConversions_s  ** conversions );


int                oyConversions_FilterAdd (
                                       oyConversions_s   * conversions,
                                       oyFilter_s        * filter );
int                oyConversions_OutputAdd (
                                       oyConversions_s   * conversions,
                                       oyImage_s         * input );
int                oyConversions_Init( oyConversions_s   * conversions );
oyPointer        * oyConversions_GetNextPixel (
                                       oyConversions_s   * conversions,
                                       oyPixelAccess_s   * pixel_access,
                                       int32_t           * feedback );
oyPointer        * oyConversions_GetOnePixel (
                                       oyConversions_s   * conversions,
                                       int32_t             x,
                                       int32_t             y,
                                       int32_t           * feedback );
oyProfile_s      * oyConversions_ToProfile (
                                       oyConversions_s   * conversions );
int             ** oyConversions_GetAdjazenzlist (
                                       oyConversions_s   * conversions,
                                       oyAlloc_f           allocateFunc );

/** @struct oyColourConversion_s
    In case where
      a option indicates monitor output, or
      the out image struct has no profile set, 
    the conversion will route to monitor colours, honouring the oyImage_s screen
    position.

    deprecate with the new filter architecture
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_COLOUR_CONVERSION_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  oyProfiles_s       * profiles_;      /*!< effect / simulation profiles */ 
  oyOptions_s        * options_;       /*!< conversion opts */
  oyImage_s          * image_in_;      /*!< input */
  oyImage_s          * image_out_;     /*!< output */
  oyStructList_s     * cmms_;          /**< list of CMM entries to call */
} oyColourConversion_s;

oyColourConversion_s* oyColourConversion_Create (
                                       oyProfiles_s      * list,
                                       oyOptions_s       * opts,
                                       oyImage_s         * in,
                                       oyImage_s         * out,
                                       oyObject_s          object );
oyColourConversion_s* oyColourConversion_Copy (
                                       oyColourConversion_s * cc,
                                       oyObject_s          object );
int          oyColourConversion_Release ( oyColourConversion_s ** cc );

int          oyColourConversion_Run  ( oyColourConversion_s * colour /*!< object*/
                                     );                  /*!< return: error */
oyProfile_s* oyColourConversion_ToProfile ( oyColourConversion_s * s );

/** @struct oyNamedColour_s
 *  @brief colour patch with meta informations
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  TODO: needs to be Xatom compatible
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_NAMED_COLOUR_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  double             * channels_;      /**< eigther parsed or calculated otherwise */
  double               XYZ_[3];        /**< CIE*XYZ representation */
  char               * blob_;          /**< advanced : CGATS / ICC ? */
  size_t               blob_len_;      /**< advanced : CGATS / ICC ? */
  oyProfile_s        * profile_;       /**< ICC */
} oyNamedColour_s;

oyNamedColour_s*  oyNamedColour_Create( const double      * chan,
                                        const char        * blob,
                                        int                 blob_len,
                                        oyProfile_s       * profile_ref,
                                        oyObject_s          object );
oyNamedColour_s*  oyNamedColour_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );
oyNamedColour_s*  oyNamedColour_Copy ( oyNamedColour_s   * colour,
                                       oyObject_s          object );
int               oyNamedColour_Release ( oyNamedColour_s  ** colour );

oyProfile_s *     oyNamedColour_GetSpaceRef ( oyNamedColour_s  * colour );
void              oyNamedColour_SetChannels ( oyNamedColour_s * colour,
                                       const double      * channels,
                                       uint32_t            flags );
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags );
const double *    oyNamedColour_GetChannelsConst ( oyNamedColour_s * colour,
                                       uint32_t          * flags );
const double *    oyNamedColour_GetXYZConst      ( oyNamedColour_s * colour);
int               oyNamedColour_GetColourStd     ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags );
int               oyNamedColour_GetColour ( oyNamedColour_s * colour,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags );
const char   *    oyNamedColour_GetName( oyNamedColour_s * s,
                                       oyNAME_e            type,
                                       uint32_t            flags );

/** @brief list of colour patches
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  TODO: make the object non visible
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_NAMED_COLOURS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /*!< base object */
  oyStructList_s     * list_;          /**< colour list */
} oyNamedColours_s;

oyNamedColours_s* oyNamedColours_New ( oyObject_s       object );
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          object );
int               oyNamedColours_Release ( oyNamedColours_s** colours );

int               oyNamedColours_Count(oyNamedColours_s  * swatch );
oyNamedColour_s*  oyNamedColours_Get ( oyNamedColours_s  * swatch,
                                       int                 position);
oyNamedColours_s* oyNamedColours_MoveIn ( oyNamedColours_s  * list,
                                       oyNamedColour_s  ** obj,
                                       int                 pos );
int               oyNamedColours_ReleaseAt ( oyNamedColours_s * swatch,
                                       int                 position );




void              oyCopyColour       ( const double      * from,
                                       double            * to,
                                       int                 n,
                                       oyProfile_s       * profile_ref,
                                       int                 channels_n );




/* --- CMM API --- */

/** @brief icon data
 *
 *  Since: 0.1.8
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /*!< struct type oyOBJECT_TYPE_ICON_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;
  int              height;
  float          * data;               /*!< should be sRGB matched */
  char           * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
} oyIcon_s;

typedef struct oyCMMapi_s oyCMMapi_s;

/** @brief the CMM API resources struct to implement and set by a CMM
 *
 *  Given an example CMM with name "little cms", which wants to use the 
 *  four-char ID 'lcms', the CMM can register itself to Oyranos as follows:
 *  The CMM module file must be named
 *  something_lcms_cmm_module_something.something .
 *  On Linux this could be "liboyranos_lcms_cmm_module.so.0.1.8".
 *  The four-chars 'lcms' must be prepended with OY_MODULE_NAME alias
 *  "_cmm_module".
 *
 *  Oyranos will scan the $(libdir)/color/cmms/ path, opens the available 
 *  CMM's from this directory and extracts the four-chars before OY_MODULE_NAME
 *  from the library file names. Module paths can be added through the
 *  OY_MODULE_PATHS environment variable.
 *  Oyranos looks for a symbol to a oyCMMInfo_s struct of the four-byte ID plus
 *  OY_MODULE_NAME which results in our example in the name "lcms_cmm_module".
 *  On Posix system this should be loadable by dlsym.
 *  The lcms_cmm_module should be of type oyCMMInfo_s with the type field
 *  and all other fields set appropriately.
 *
 *  The api field is a placeholder to get a real api struct assigned. If the CMM
 *  wants to provide more than one API, they can be chained. The apis_n member
 *  is to be set to the number of APIs.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /*!< struct type oyOBJECT_TYPE_CMM_INFO_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char             cmm[8];             /*!< ICC signature, eg 'lcms' */
  char           * backend_version;    /*!< non translatable, eg "v1.17" */
  oyName_s         name;               /*!< translatable, eg "lcms" "little cms" "..." */
  oyName_s         manufacturer;       /*!< translatable, eg "Marti" "Marti Maria" "support email: @; internet: www.littlecms.com; sources: ..." */
  oyName_s         copyright;          /*!< translatable, eg "MIT","MIT License".. */
  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */

  oyCMMapi_s     * api;                /**< must be casted to a according API */
  int              apis_n;             /**< count of implemented apis */

  oyIcon_s         icon;               /*!< zero terminated list of a icon pyramid */
} oyCMMInfo_s;


/** @deprecated */
int            oyModulRegisterXML    ( oyGROUP_e           group,
                                       const char        * xml );

/** obtain 4 char CMM identifiers and count of CMM's */
char **        oyModulsGetNames      ( int               * count,
                                       oyAlloc_f           allocateFunc );
/** Query for available options for a cmm

    @param[in] cmm      the 4 char CMM ID or zero for the current CMM
    @return             available options
 */
const char *   oyModulGetOptions     ( const char        * cmm,
                                       oyObject_s          object);
const char *   oyModuleGetActual     ( unsigned int        flags );


/* --- Image Colour Profile API --- */
/* needs extra libraries liboyranos_png liboyranos_tiff ... */
#ifdef OY_HAVE_PNG_
#include <png.h>
#define OY_PNG_s    png_infop
oyProfile_s *  oyImagePNGgetICC      ( OY_PNG_s            info,
                                       int                 flags );
#endif
#ifdef OY_HAVE_TIFF_
#include <tiffio.h>
#define OY_TIFF_s   TIFF*
oyProfile_s *  oyImageTIFFgetICC     ( OY_TIFF_s           dir,
                                       int                 flags );
int            oyImageTIFFsetICC     ( OY_TIFF_s           dir,
                                      oyProfile_s        * profile,
                                      int                  flags);
#endif
#ifdef OY_HAVE_EXR_
#include <OpenEXR/OpenEXR.h>
#define OY_EXR_s    ImfHeader
oyProfile_s *  oyImageEXRgetICC      ( OY_EXR_s            header,
                                       int                 flags );
#ifdef __cplusplus
#define OY_EXRpp_s  Imf::Header*
oyProfile_s *  oyImageEXRgetICC      ( OY_EXRpp_s          header,
                                       int                 flags );
#endif
#endif
/*
with flags something like:
oyIMAGE_EMBED_ICC_MINIMAL  ...
oyIMAGE_EMBED_ICC_FULL (while for OpenEXR this would not make sense) */


char   *       oyDumpColourToCGATS   ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const char        * DESCRIPTOR );



#endif /* OY_IN_PLANING */

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
