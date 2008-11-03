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
 *  @par License:
 *  new BSD - see: http://www.opensource.org/licenses/bsd-license.php
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

typedef struct oyImage_s oyImage_s;


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

/** param[out]     is_allocated          are the points always newly allocated*/
typedef oyPointer (*oyImage_GetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the lines always newly allocated */
typedef oyPointer (*oyImage_GetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the tiles always newly allocated */
typedef oyPointer*(*oyImage_GetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         int             * is_allocated );
typedef int       (*oyImage_SetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         oyPointer         data );


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

/**
 *  @brief   Oyranos structure type
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/22 (Oyranos: 0.1.x)
 *  @date    2008/07/31
 */
typedef enum {
  oyOBJECT_NONE,
  oyOBJECT_OBJECT_S,                  /**< oyObject_s */
  oyOBJECT_MONITOR_S,                 /**< oyMonitor_s */
  oyOBJECT_NAMED_COLOUR_S,            /*!< oyNamedColour_s */
  oyOBJECT_NAMED_COLOURS_S,           /*!< oyNamedColours_s */
  oyOBJECT_PROFILE_S,                 /*!< oyProfile_s */
  oyOBJECT_PROFILE_TAG_S,             /*!< oyProfileTag_s */
  oyOBJECT_PROFILES_S,                /*!< oyProfiles_s */
  oyOBJECT_OPTION_S,                  /*!< oyOption_s */
  oyOBJECT_OPTIONS_S,                 /*!< oyOptions_s */
  oyOBJECT_REGION_S,                  /*!< oyRegion_s */
  oyOBJECT_IMAGE_S,                   /*!< oyImage_s */
  oyOBJECT_ARRAY2D_S,                 /**< oyArray2d_s */
  oyOBJECT_COLOUR_CONVERSION_S,       /*!< oyColourConversion_s */
  oyOBJECT_CONNECTOR_S,               /**< oyConnector_s */
  oyOBJECT_FILTER_PLUG_S,             /**< oyFilterPlug_s */
  oyOBJECT_FILTER_PLUGS_S,            /**< oyFilterPlugs_s */
  oyOBJECT_FILTER_SOCKET_S,           /**< oyFilterSocket_s */
  oyOBJECT_FILTER_S,                  /**< oyFilter_s */
  oyOBJECT_FILTERS_S,                 /**< oyFilters_s */
  oyOBJECT_FILTER_NODE_S,             /**< oyFilterNode_s */
  oyOBJECT_PIXEL_ACCESS_S,            /**< oyPixelAccess_s */
  oyOBJECT_CONVERSION_S,              /**< oyConversion_s */
  oyOBJECT_CMM_HANDLE_S      = 50,    /**< oyCMMhandle_s */
  oyOBJECT_CMM_POINTER_S,             /*!< oyCMMptr_s */
  oyOBJECT_CMM_INFO_S,                /*!< oyCMMInfo_s */
  oyOBJECT_CMM_API_S,                 /**< oyCMMapi_s */
  oyOBJECT_CMM_API1_S,                /**< oyCMMapi1_s */
  oyOBJECT_CMM_API2_S,                /**< oyCMMapi2_s */
  oyOBJECT_CMM_API3_S,                /**< oyCMMapi3_s */
  oyOBJECT_CMM_API4_S,                /**< oyCMMapi4_s */
  oyOBJECT_CMM_API_MAX,               /**< not defined */
  oyOBJECT_ICON_S      = 80,          /*!< oyIcon_s */
  oyOBJECT_MODULE_S,                  /*!< oyModule_s */
  oyOBJECT_EXTERNFUNC_S,              /*!< oyExternFunc_s */
  oyOBJECT_NAME_S,                    /*!< oyName_s */
  oyOBJECT_COMP_S_,                   /*!< oyComp_s_ */
  oyOBJECT_FILE_LIST_S_,              /*!< oyFileList_s_ */
  oyOBJECT_HASH_S,                    /**< oyHash_s */
  oyOBJECT_HANDLE_S,                  /**< oyHandle_s */
  oyOBJECT_STRUCT_LIST_S,             /**< oyStructList_s */
  oyOBJECT_MAX
} oyOBJECT_e;


typedef struct oyObject_s_* oyObject_s;

/** @brief Oyranos base structure
 *  @ingroup objects_generic
 *
 *  All Oyranos structures should be castable to this basic one.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
struct oyStruct_s {
  oyOBJECT_e           type_;          /**< struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
};

oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size );
const char * oyStruct_TypeToText     ( const oyStruct_s  * oy_struct );

/** @enum    oyNAME_e
 *  @brief   describe the base types of a oyObject_s name
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 */
typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

#define oyNAME_ID oyNAME_NICK

/** @brief Oyranos name structure
 *  @ingroup objects_generic
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_e           type;           /*!< internal struct type oyOBJECT_NAME_S */
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

/** @enum    oyBOOLEAN_e
 *  @brief   boolean operations
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
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
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_HANDLE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  oyPointer            ptr;            /**< can be any type */
  oyOBJECT_e           ptr_type;       /**< the type of the entry */
  oyStruct_release_f   ptrRelease;     /**< deallocation for ptr_ of list_type*/
  oyStruct_copy_f      ptrCopy;        /**< copy for ptr_ of list_type */
} oyHandle_s;

oyHandle_s *       oyHandle_new_     ( oyAlloc_f           allocateFunc );
oyHandle_s *       oyHandle_copy_    ( oyHandle_s        * orig,
                                       oyAlloc_f           allocateFunc );
int                oyHandle_release_ ( oyHandle_s       ** handle );

int                oyHandle_set_     ( oyHandle_s        * handle,
                                       oyPointer           ptr,
                                       oyOBJECT_e          ptr_type,
                                       oyStruct_release_f  ptrRelease,
                                       oyStruct_copy_f     ptrCopy );
#endif

typedef struct oyStructList_s oyStructList_s;

/** @struct  oyObject_s
 *  @brief   Oyranos structure base
 *  @ingroup objects_generic
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
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_OBJECT_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  int                  id_;            /**< identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< data release function */
  oyPointer            parent_;        /*!< parent struct of parent_type */
  oyOBJECT_e           parent_type_;   /*!< parents struct type */
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
                                        oyOBJECT_e        type,
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
 *  @ingroup objects_generic
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
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_HASH_S */
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
                                       oyOBJECT_e          type );
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_e          type );
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj );

/** @internal
 *  @brief a pointer list
 *  @ingroup objects_generic
 *
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
struct oyStructList_s {
  oyOBJECT_e           type_;          /*!< internal struct type oyOBJECT_STRUCT_LIST_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
  oyStruct_s        ** ptr_;           /**< the list data */
  int                  n_;             /**< the number of visible pointers */
  int                  n_reserved_;    /**< the number of allocated pointers */
  char               * list_name;      /**< name of list */
  oyOBJECT_e           parent_type_;   /**< parents struct type */
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
                                       oyOBJECT_e          type );
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos );
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                       int                 pos,
                                       oyOBJECT_e          type );
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


/* --- colour conversion --- */

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
  oyVAL_STRING_LIST,
  oyVAL_STRUCT
} oyVALUETYPE_e;

/** @union   oyValue_u
 *  @brief   a value
 *  @ingroup objects_value
 *
 *  @see     oyVALUETYPE_e
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef union {
  int32_t          int32;
  int32_t        * int32_list;         /**< first is element number of int32 list */
  double           dbl;
  double         * dbl_list;           /**< first is element number of dbl list */
  char           * string;             /**< null terminated */
  char          ** string_list;        /**< null terminated */

  oyStruct_s     * oy_struct;          /**< a struct, e.g. a profile */
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
 *  @ingroup objects_value

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
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_OPTION_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  uint32_t             id;             /**< id to map to events and widgets */
  oyName_s             name;           /**< nick, name, description/help, e.g. "radius" "Radius" "..." */
  const char         * registration;    /**< full key name to store configuration, e.g. "sw/oyranos.org/generic/scale", config key name will be name.nick; @todo Whats the difference here? registration <-> XML? */
  int                  version[3];     /**< as for oyCMMapi4_s::version */
  oyVALUETYPE_e        value_type;     /**< the type in value */
  oyValue_u          * value;          /**< the actual value */
  oyValue_u          * standard;       /**< the standard value */
  oyValue_u          * start;          /**< value range start */
  oyValue_u          * end;            /**< value range end */
  uint32_t             flags;          /**< */
} oyOption_s;

oyOption_s *   oyOption_New          ( oyObject_s          object,
                                       const char        * name );
oyOption_s *   oyOption_Copy         ( oyOption_s        * option,
                                       oyObject_s          object );
int            oyOption_Release      ( oyOption_s       ** option );


int            oyOption_GetId        ( oyOption_s        * option );
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type );


/** @brief generic Options
 *  @ingroup objects_value
 *
 *  Options can be any flag or rendering intent and other informations needed to
 *  configure a process. The object contains a list of oyOption_s objects.
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_OPTIONS_S */
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
/** @enum    oyOPTIONDEFAULTS_e
 *  @brief   usage type
 *  @ingroup objects_value
 *
 *  The types of Oyranos default settings to include into the options set.
 *  So proofing makes no sense for toolkit widgets, but for advanced
 *  graphics displaying. All other Oyranos behaviour settings should be taken
 *  over untouched.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2008/10/08
 */
typedef enum {
  oyOPTIONDEFAULTS_BASIC,              /** basic settings, as typical for toolkits and office/web applciations, disable proofing */
  oyOPTIONDEFAULTS_ADVANCED            /** advanced settings, as typical for editing images, include proofing */
} oyOPTIONDEFAULTS_e;
oyOptions_s *  oyOptions_FromDefaults( oyOPTIONDEFAULTS_e  type,
                                       uint32_t            flags,
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
char *         oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value );


/** @brief general profile infos
 *  @ingroup objects_profile
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
 *  @ingroup objects_profile
 */
typedef struct {
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_PROFILE_S */
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
#define OY_NO_CACHE_READ            0x01        /** read not from cache */
#define OY_NO_CACHE_WRITE           0x02        /** write not from cache */
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
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB );
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
 *  @ingroup objects_profile
 */
typedef struct {
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_PROFILES_S */
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
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         colour_space,
                                       int               * current,
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
 *  @ingroup objects_profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
struct oyProfileTag_s {
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_PROFILE_TAG_S */
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


/** @enum    oyDATALAYOUT_e
 *  @brief   ideas for basic data arrangements
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
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


/** @brief start with a simple rectangle
 *  @ingroup objects_region
 */
typedef struct {
  oyOBJECT_e           type_;          /**< internal struct type oyOBJECT_REGION_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;

  double x;
  double y;
  double width;
  double height;

} oyRegion_s;

oyRegion_s *   oyRegion_New_         ( oyObject_s          object );
oyRegion_s *   oyRegion_NewWith      ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object );
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


/** @enum    oyDATATYPE_e
 *  @brief   basic data types for anonymous pointers
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.8)
 *  @date    2007/00/00
 */
typedef enum {
  oyUINT8,     /*!<  8-bit integer */
  oyUINT16,    /*!< 16-bit integer */
  oyUINT32,    /*!< 32-bit integer */
  oyHALF,      /*!< 16-bit floating point number */
  oyFLOAT,     /*!< IEEE floating point number */
  oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE_e;


/** @type oyPixel_t 
 *  @ingroup objects_image
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
#define OY_TYPE_123_8       (oyChannels_m(3)|oyDataType_m(oyUINT8))
#define OY_TYPE_123_16      (oyChannels_m(3)|oyDataType_m(oyUINT16))
#define OY_TYPE_123_HALF    (oyChannels_m(3)|oyDataType_m(oyHALF))
#define OY_TYPE_123_FLOAT   (oyChannels_m(3)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123_DBL     (oyChannels_m(3)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_123A_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define OY_TYPE_123A_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define OY_TYPE_123A_HALF   (oyChannels_m(4)|oyDataType_m(oyHALF))
#define OY_TYPE_123A_FLOAT  (oyChannels_m(4)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123A_DBL    (oyChannels_m(4)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_123AZ_HALF  (oyChannels_m(5)|oyDataType_m(oyHALF))
#define OY_TYPE_123AZ_FLOAT (oyChannels_m(5)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123AZ_DBL   (oyChannels_m(5)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_A123_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8))
#define OY_TYPE_A123_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16))

#define OY_TYPE_A321_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8)|oySwapColourChannels_m(oyYES))
#define OY_TYPE_A321_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16)|oySwapColourChannels_m(oyYES))

/* some intermixed types will work as well */
#define OY_TYPE_123A_HALF_Z_FLOAT (oyChannels_m(4+1*2)|oyDataType_m(oyHALF))

#define OY_TYPE_1234_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define OY_TYPE_1234A_8     (oyChannels_m(5)|oyDataType_m(oyUINT8))
#define OY_TYPE_1234_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define OY_TYPE_1234A_16    (oyChannels_m(5)|oyDataType_m(oyUINT16))

#define OY_TYPE_1234_8_REV  (oyChannels_m(4)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_8_REV (oyChannels_m(5)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234_16_REV (oyChannels_m(4)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_16_REV (oyChannels_m(5)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))


#define OY_TYPE_1_8         (oyChannels_m(1)|oyDataType_m(oyUINT8))
#define OY_TYPE_1_16        (oyChannels_m(1)|oyDataType_m(oyUINT16))
#define OY_TYPE_1_HALF      (oyChannels_m(1)|oyDataType_m(oyHALF))
#define OY_TYPE_1_FLOAT     (oyChannels_m(1)|oyDataType_m(oyFLOAT))
#define OY_TYPE_1A_8        (oyChannels_m(2)|oyDataType_m(oyUINT8))
#define OY_TYPE_1A_16       (oyChannels_m(2)|oyDataType_m(oyUINT16))
#define OY_TYPE_1A_HALF     (oyChannels_m(2)|oyDataType_m(oyHALF))
#define OY_TYPE_1A_FLOAT    (oyChannels_m(2)|oyDataType_m(oyFLOAT))

#define OY_TYPE_1_8_REV     (oyChannels_m(1)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1_16_REV    (oyChannels_m(1)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1_HALF_REV  (oyChannels_m(1)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define OY_TYPE_1_FLOAT_REV (oyChannels_m(1)|oyDataType_m(oyFLOAT))|oyFlavor_m(oyYes)
#define OY_TYPE_1A_8_REV    (oyChannels_m(2)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_16_REV   (oyChannels_m(2)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_HALF_REV (oyChannels_m(2)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_FLOAT_REV (oyChannels_m(2)|oyDataType_m(oyFLOAT)|oyFlavor_m(oyYes))

/* decode */
#define oyToChannels_m(c)           ((c)&255)
#define oyToColourOffset_m(o)       (((o) >> 8)&255)
#define oyToDataType_m(t)           (((t) >> 16)&15)
#define oyToSwapColourChannels_m(s) (((s) >> 20)&1)
#define oyToPlanar_m(p)             (((p) >> 21)&1)
#define oyToFlavor_m(f)             (((f) >> 22)&1)
#define oyToByteswap_m(x)           (((x) >> 23)&1)

/** @enum    oyCHANNELTYPE_e
 *  @brief   channels types
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyCHANNELTYPE_UNDEFINED,            /**< as well for the list end */
  oyCHANNELTYPE_OTHER,
  oyCHANNELTYPE_LIGHTNESS,
  oyCHANNELTYPE_COLOUR,
  oyCHANNELTYPE_COLOUR_LIGHTNESS,
  oyCHANNELTYPE_COLOUR_LIGHTNESS_PREMULTIPLIED,
  oyCHANNELTYPE_DEPTH,
  oyCHANNELTYPE_ALPHA1,
  oyCHANNELTYPE_ALPHA2,
  oyCHANNELTYPE_ALPHA3
} oyCHANNELTYPE_e;

oyCHANNELTYPE_e oyICCColourSpaceToChannelLayout (
                                       icColorSpaceSignature sig,
                                       int                 pos );
#if 0
char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc );
#endif

/** @struct  oyArray2d_s
 *  @brief   2d data array
 *  @ingroup objects_image
 *
 *  oyArray2d_s is a in memory data view. The array2d holds pointers to lines in
 *  the original memory blob. The arrays contained in array2d represent the 
 *  samples. There is no information in which order the samples appear. No pixel
 *  layout or meaning is provided. Given the coordinates x and y, a samples 
 *  memory adress can be accessed by &array2d[y][x] . This adress must be
 *  converted to the data type provided in oyArray2d_s::t.
 *
 *  The oyArray2d_s::data pointer should be observed in order to be signaled
 *  about its invalidation.
 *
  \dot
  digraph a {
  nodesep=.05;
  rankdir=LR
      node [shape=record,fontname=Helvetica, fontsize=10, width=.1,height=.1];

      e [ label="oyArray2d_s with 8 samples x 10 lines", shape=plaintext];

      y [ label="<0>0|<1>1|<2>2|<3>3|<4>4|<5>5|<6>6|<7>7|<8>8|<9>9", height=2.0 , style=filled ];
      node [width = 1.5];
      0 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      1 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      2 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      3 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      4 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      5 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      6 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      7 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      8 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      9 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      e
      y:0 -> 0:0
      y:1 -> 1:0
      y:2 -> 2:0
      y:3 -> 3:0
      y:4 -> 4:0
      y:5 -> 5:0
      y:6 -> 6:0
      y:7 -> 7:0
      y:8 -> 8:0
      y:9 -> 9:0
      0:7 -> 1:0 [arrowhead="open", style="dashed"];
      1:7 -> 2:0 [arrowhead="open", style="dashed"];
  }
  \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
typedef struct {
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_ARRAY2D_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyDATATYPE_e         t;              /**< data type */
  int                  width;          /**< width of actual data view */
  int                  height;         /**< height of actual data view */
  oyRegion_s         * data_area;      /**< size of reserve pixels, x,y <= 0, width,height >= data view width,height */

  unsigned char     ** array2d;        /**< sorted data */
  int                  own_lines;      /**< is *array2d owned by this object? */
} oyArray2d_s;

OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object );
OYAPI oyArray2d_s * OYEXPORT
                 oyArray2d_New       ( oyObject_s          object );
OYAPI oyArray2d_s * OYEXPORT
                 oyArray2d_Copy      ( oyArray2d_s       * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyArray2d_Release   ( oyArray2d_s      ** obj );


OYAPI int  OYEXPORT
                 oyArray2d_DataSet   ( oyArray2d_s       * obj,
                                       oyPointer           data );

/** @brief a reference struct to gather information for image transformation
 *  @ingroup objects_image
 *
 *  as we dont target a complete imaging solution, only raster is supported
 *
 *  Resolution is in pixel per centimeter.
 *
 *  Requirements: \n
 *  - to provide a view on the image data we look at per line arrays
 *  - it should be possible to echange the to be processed data without altering
 *    the context
 *  - oyImage_s should hold image dimensions,
 *  - display region information and
 *  - a reference to the data for conversion
 *
 *  To set a image data backend use oyImage_DataSet().
 *  \dot
 digraph oyImage_s {
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record,fontname=Helvetica, fontsize=10, width=.1];

  subgraph cluster_3 {
    label="oyImage_s data backends";
    color=white;
    clusterrank=global;

      i [ label="... | <0>oyStruct_s * pixel | <1> oyImage_GetPoint_f getPoint | <2>oyImage_GetLine_f getLine | <3>oyImage_GetTile_f getTile | ..."];

      node [width = 2.5, style=filled];
      pixel_A [label="oyArray2d_s arrayA"];
      gp_p_A [label="Array2d_GetPointA"];
      gp_l_A [label="Array2d_GetLineA"];
      gp_t_A [label="Array2d_GetTileA"];

      pixel_B [label="mmap arrayB"];
      gp_p_B [label="mmap_GetPointB"];
      gp_l_B [label="mmap_GetLineB"];
      gp_t_B [label="mmap_GetTileB"];

      subgraph cluster_0 {
        rank=max;
        color=red;
        style=dashed;
        node [style="filled"];
        pixel_A; gp_p_A; gp_l_A; gp_t_A;
        //pixel_A -> gp_p_A -> gp_l_A -> gp_t_A [color=white, arrowhead=none, dirtype=none];
        label="backend A";
      }

      subgraph cluster_1 {
        color=blue;
        style=dashed;
        node [style="filled"];
        pixel_B; gp_p_B; gp_l_B; gp_t_B;
        label="backend B";
      }

      subgraph cluster_2 {
        color=gray;
        node [style="filled"];
        i;
        label="oyImage_s";
        URL="structoyImage__s.html";
      }

      i:0 -> pixel_A [arrowhead="open", color=red];
      i:1 -> gp_p_A [arrowhead="open", color=red];
      i:2 -> gp_l_A [arrowhead="open", color=red];
      i:3 -> gp_t_A [arrowhead="open", color=red];
      i:0 -> pixel_B [arrowhead="open", color=blue];
      i:1 -> gp_p_B [arrowhead="open", color=blue];
      i:2 -> gp_l_B [arrowhead="open", color=blue];
      i:3 -> gp_t_B [arrowhead="open", color=blue];
  }
 }
 \enddot
 *
 *  Should oyImage_s become internal and we provide a user interface?
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
struct oyImage_s {
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_IMAGE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyRegion_s         * viewport;       /**< intented viewing area, normalised to the pixel width == 1.0 */
  double               resolution_x;   /**< resolution in horizontal direction*/
  double               resolution_y;   /**< resolution in vertical direction */

  oyPixel_t          * layout_;        /**< samples mask; the oyPixel_t pixel_layout variable passed during oyImage_Create is stored in position 0 */
  oyCHANNELTYPE_e    * channel_layout; /**< non profile described channels */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyOptions_s        * options_;       /*!< channel layout (? undecided) */
  oyProfile_s        * profile_;       /*!< image profile */
  int                  display_pos_x;  /**< Possibly this can be part of the output profile; upper position on display of image*/
  int                  display_pos_y;  /*!< left position on display of image */

  oyStruct_s         * pixel_data;     /**< struct used by each subsequent call of g/set* pixel acessors */
  oyImage_GetPoint_f   getPoint;       /**< the point interface */
  oyImage_GetLine_f    getLine;        /**< the line interface */
  oyImage_GetTile_f    getTile;        /**< the tile interface */
  oyImage_SetPoint_f   setPoint;       /**< the point interface */
  oyImage_SetLine_f    setLine;        /**< the line interface */
  oyImage_SetTile_f    setTile;        /**< the tile interface */
  int                  tile_width;     /**< needed by the tile interface */
  int                  tile_height;    /**< needed by the tile interface */
  uint16_t             subsampling[2]; /**< 1, 2 or 4 */
  int                  sub_positioning;/**< 0 None, 1 Postscript, 2 CCIR 601-1*/
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
int            oyImage_DataSet       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile );
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRegion_s        * region,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyObject_s          obj );


/** @enum    oyFILTER_TYPE_e
 *  @brief   basic filter classes
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyFILTER_TYPE_NONE,                  /**< nothing */
  oyFILTER_TYPE_COLOUR,                /**< colour */
  oyFILTER_TYPE_COLOUR_ICC,            /**< colour icc style */
  oyFILTER_TYPE_TONEMAP,               /**< contrast or tone mapping */
  oyFILTER_TYPE_IMAGE,                 /**< image storage */
  oyFILTER_TYPE_GENERIC,               /**< generic */
  oyFILTER_TYPE_MAX
} oyFILTER_TYPE_e;

const char *   oyFilterTypeToText    ( oyFILTER_TYPE_e     filter_type,
                                       oyNAME_e            type );

/** see:http://lists.freedesktop.org/archives/openicc/2008q4/001724.html 
 *  @ingroup objects_conversion
 */
typedef enum {
  oyFILTER_REG_NONE,
  oyFILTER_REG_TOP,                    /**< e.g. "sw" for filters */
  oyFILTER_REG_DOMAIN,                 /**< e.g. "oyranos.org" */
  oyFILTER_REG_TYPE,                   /**< oyFilterTypeToText/oyFILTER_TYPE_e, e.g. "generic" filter group */
  oyFILTER_REG_APPLICATION,            /**< e.g. "scale" filter name */
  oyFILTER_REG_OPTION,                 /**< e.g. "x" filter option */
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
typedef struct oyConnector_s oyConnector_s;
typedef struct oyFilterPlug_s oyFilterPlug_s;
typedef struct oyFilterPlugs_s oyFilterPlugs_s;
typedef struct oyFilterSocket_s oyFilterSocket_s;


/** @enum    oyCONNECTOR_e
 *  @brief   basic connector classes
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  /** a data manipulator. e.g. a normal filter */
  oyCONNECTOR_MANIPULATOR,
  /** a data generator, e.g. checkerboard, gradient */
  oyCONNECTOR_GENERATOR,
  /** a pixel data provider, e.g. oyFILTER_TYPE_IMAGE */
  oyCONNECTOR_IMAGE,
  /** observer, a endpoint, only input, e.g. text log, thumbnail viewer */
  oyCONNECTOR_OBSERVER,
  /** a routing element, without data altering */
  oyCONNECTOR_SPLITTER,
  /** combines or splits image data, e.g. blending */
  oyCONNECTOR_COMPOSITOR,

  /** converts pixel layout to other formats */
  oyCONNECTOR_CONVERTOR_PIXELDATA,
  /** converts pixel layout to other formats, with precission loss, e.g. float -> uint8_t, only relevant for output connectors */
  oyCONNECTOR_CONVERTOR_PIXELDATA_LOSSY,
  /** combines gray channels, e.g. from colour */
  oyCONNECTOR_COMPOSITOR_CHANNEL,
  /** provides gray scale views of channels */
  oyCONNECTOR_SPLITTER_CHANNEL,

  /** provides values or text, only output */
  oyCONNECTOR_ANALYSIS
} oyCONNECTOR_e;

/** @enum    oyCONNECTOR_EVENT_e
 *  @brief   connector events types
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyCONNECTOR_EVENT_OK,                /**< kind of ping */
  oyCONNECTOR_EVENT_CONNECTED,         /**< connection established */
  oyCONNECTOR_EVENT_RELEASED,          /**< released the connection */
  oyCONNECTOR_EVENT_IMAGE_DATA_CHANGED,/**< call to update image views */
  oyCONNECTOR_EVENT_IMAGE_STORAGE_CHANGED,/**< new data accessors */
  oyCONNECTOR_EVENT_INCOMPATIBLE_IMAGE,/**< can not process image */
  oyCONNECTOR_EVENT_INCOMPATIBLE_PROFILE,/**< can not handle profile */
  oyCONNECTOR_EVENT_INCOMPLETE_GRAPH   /**< can not completely process */ 
} oyCONNECTOR_EVENT_e;

/** @struct  oyConnector_s
 *  @brief   a filter connection description structure
 *  @ingroup objects_conversion
 *
 *  This structure holds informations about the connection capabilities.
 *  It holds common structure members of oyFilterPlug_s and oyFilterSocket_s.
 *
 *  To signal a value is not initialised or does not apply, set the according
 *  integer value to -1.
 *
 *  @todo generalise the connector properties
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/26 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyConnector_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONNECTOR_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyName_s             name;           /**< e.g."Img", "Image", "Image Socket"*/

  oyCONNECTOR_e        connector_type; /**< */
  /** make requests and receive data, by part of oyFilterPlug_s */
  int                  is_plug;
  oyDATATYPE_e       * data_types;     /**< supported float's and int's */
  int                  data_types_n;   /**< elements in data_types array */
  int                  max_colour_offset;
  int                  min_channels_count;
  int                  max_channels_count;
  int                  min_colour_count;
  int                  max_colour_count;
  int                  can_planar;     /**< can read separated channels */
  int                  can_interwoven; /**< can read continuous channels */
  int                  can_swap;       /**< can swap colour channels (BGR)*/
  int                  can_swap_bytes; /**< non host byte order */
  int                  can_revert;     /**< revert 1 -> 0 and 0 -> 1 */
  int                  can_premultiplied_alpha;
  int                  can_nonpremultiplied_alpha;
  int                  can_subpixel;   /**< understand subpixel order */
  /** describe which channel types the connector requires */
  oyCHANNELTYPE_e    * channel_types;
  int                  channel_types_n;/**< count in channel_types */
  int                  id;             /**< relative to oyFilter_s, e.g. 1 */
  /**< connector is mandatory or optional, important for backends */
  int                  is_mandatory;
};

OYAPI oyConnector_s * OYEXPORT
                 oyConnector_New     ( oyObject_s          object );
OYAPI oyConnector_s * OYEXPORT
                 oyConnector_Copy    ( oyConnector_s     * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyConnector_Release ( oyConnector_s    ** list );



/** @struct oyFilterSocket_s
 *  @brief  a filter connection structure
 *  @ingroup objects_conversion
 *
 *  The passive output version of a oyConnector_s.
 \dot
digraph G {
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [URL="structoyFilterPlug__s.html", label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowtail=crow, arrowhead=box];
} 
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterSocket_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_SOCKET_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  oyFilterNode_s     * node;           /**< filter node for this connector */
  char               * relatives_;     /**< hint about belonging to a filter */

  oyStruct_s         * data;           /**< unprocessed data model */

  oyFilterPlugs_s    * requesting_plugs_;/**< all remote inputs */
};

OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterSocket_New  ( oyObject_s          object );
OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterSocket_Copy ( oyFilterSocket_s  * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterSocket_Release (
                                       oyFilterSocket_s ** list );


OYAPI int  OYEXPORT
                 oyFilterSocket_Callback (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e );

/** @struct oyFilterPlug_s
 *  @brief  a filter connection structure
 *  @ingroup objects_conversion
 *
 *  The active input version of a oyConnector_s.
 *  Each plug can connect to exact one socket.
 \dot
digraph G {
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [URL="structoyFilterSocket__s.html", label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowtail=crow, arrowhead=box];
} 
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterPlug_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_PLUG_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  oyFilterNode_s     * node;           /**< filter node for this connector */
  char               * relatives_;     /**< hint about belonging to a filter */

  oyFilterSocket_s   * remote_socket_; /**< the remote output */
};

OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterPlug_New    ( oyObject_s          object );
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterPlug_Copy   ( oyFilterPlug_s    * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterPlug_Release( oyFilterPlug_s   ** list );


OYAPI int  OYEXPORT
                 oyFilterPlug_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterPlug_ConnectIntoSocket (
                                       oyFilterPlug_s   ** p,
                                       oyFilterSocket_s ** s );

/** @struct  oyFilterPlugs_s
 *  @brief   a FilterPlugs list
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterPlugs_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_PLUGS_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list_;          /**< the list data */
};

OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_New   ( oyObject_s          object );
OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_Copy  ( oyFilterPlugs_s   * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterPlugs_Release (
                                       oyFilterPlugs_s  ** list );


OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_MoveIn( oyFilterPlugs_s   * list,
                                       oyFilterPlug_s   ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterPlugs_ReleaseAt (
                                       oyFilterPlugs_s   * list,
                                       int                 pos );
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterPlugs_Get   ( oyFilterPlugs_s   * list,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterPlugs_Count ( oyFilterPlugs_s   * list );


/** @struct oyFilter_s
 *  @brief  a filter to manipulate a image
 *  @ingroup objects_conversion
 *
 *  This is the Oyranos filter object. Filters are categorised into basic
 *  classes of filters described in the filter_type_ member.
 *  Filters implement a container for data and options.
 *  Filters can be manipulated by changing their options or data set.
 *
 *  Filters are chained into a oyConversion_s in order to get applied to data.
 *  The relation of filters in a graph is defined through the oyFilterNode_s
 *  struct.
 *
 *  It is possible to chain filters in different ways together. The aimed way
 *  here is to use members and queries to find possible connections. For 
 *  instance a one in one out filter can not be connected to two sources at 
 *  once.
 *
 *  The oyFILTER_TYPE_e describes different basic types of filters.
 *  - oyFILTER_TYPE_COLOUR filters contain only profiles and options. They can grab their surounding and concatenate the neighbour profiles to one profile transform for speed.
 *  - oyFILTER_TYPE_TONEMAP filters are similiar to oyFILTER_TYPE_COLOUR except they can work in a two dimensional domain to apply to HDR content. This distinction is driven by usage. A oyFILTER_TYPE_TONEMAP filter may contain profiles and options. But this is not required.
 *  - oyFILTER_TYPE_IMAGE is a container for one oyImage_s. There is no 
 *    assumption on how the buffers are implemented.
 *  - oyFILTER_TYPE_GENERIC can be used for lots of things. It is the most flexible one and can contain any kind of data except profiles and images.
 *
 *  @todo generalise the filters data, e.g. oyImage_s -> oyData_s
 *                                          oyProfiles_s -> oyOptions_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */
struct oyFilter_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  const char         * registration_;  /**< a registration name, e.g. "sw/oyranos.org/generic/scale" */
  oyName_s           * name_;          /**< nick, name, description/help */
  char                 cmm_[8];        /**< cmm name to look up for infos */

  oyFILTER_TYPE_e      filter_type_;   /**< filter type */
  char               * category_;      /**< the ui menue category for this filter, to be specified */

  oyOptions_s        * options_;       /**< local options */
  char               * opts_ui_;       /**< xml ui elements for filter options*/

  /*oyImage_s          * image_;*/         /**< the referenced image */
  oyProfiles_s       * profiles_;      /**< profiles */

  oyCMMapi4_s        * api4_;          /**<  */
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
#define OY_FILTER_SET_TEST             0x01        /** only test */
#define OY_FILTER_GET_DEFAULT          0x01        /** defaults */
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
oyPointer    oyFilter_TextToInfo     ( oyFilter_s        * filter,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
OYAPI oyConnector_s * OYEXPORT
             oyFilter_ShowConnector  ( oyFilter_s        * filter,
                                       int                 as_pos,
                                       int                 plug );
OYAPI int  OYEXPORT
             oyFilter_ShowConnectorCount(
                                       oyFilter_s        * filter,
                                       int                 plug,
                                       uint32_t          * last_adds );


/** @struct  oyFilters_s
 *  @brief   a Filters list
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/08
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTERS_S */ 
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



/** @struct  oyFilterNode_s
 *  @brief   a FilterNode object
 *  @ingroup objects_conversion
 *
 *  Filter nodes chain filters into a oyConversion_s graph. The filter nodes
 *  use plugs and sockets for creating connections. Each plug can only connect
 *  to one socket.
 \dot
digraph G {
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>oyFilter_s A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>oyFilter_s B</td>
  </tr>
</table>>
  ]

  b:p->a:s [arrowtail=crow, arrowhead=box, constraint=false];

  subgraph cluster_0 {
    color=gray;
    label="FilterNode A";
    a;
  }
  subgraph cluster_1 {
    color=gray;
    label="FilterNode B";
    b;
  }
}
 \enddot
 *
 *  This object provides support for separation of options from chaining.
 *  So it will be possible to implement options changing, which can affect
 *  the same filter instance in different graphs.
 *
 *  A oyFilterNode_s can have various oyFilterPlug_s ' to obtain data from
 *  different sources. The required number is described in the oyCMMapi4_s 
 *  structure, which is part of oyFilter_s.
 \dot
digraph G {
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  b [ label="{<plug> | Filter Node 2 |<socket>}"];
  c [ label="{<plug> | Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 |<socket>}"];

  b:socket -> d:plug [arrowtail=normal, arrowhead=none];
  c:socket -> d:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 *  oyFilterSocket_s is designed to accept arbitrary numbers of connections 
 *  to allow for viewing on a filters data output or observe its state changes.
 \dot
digraph G {
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> | Filter Node 1 |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 1| Filter Node 4 |<socket>}"];
  e [ label="{<plug> 1| Filter Node 5 |<socket>}"];

  a:socket -> b:plug [arrowtail=normal, arrowhead=none];
  a:socket -> c:plug [arrowtail=normal, arrowhead=none];
  a:socket -> d:plug [arrowtail=normal, arrowhead=none];
  a:socket -> e:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/07/10
 */
struct oyFilterNode_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_NODE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyFilterPlug_s    ** plugs;          /**< active input connectors, list ends with a trailing zero */
  oyFilterSocket_s  ** sockets;        /**< active output connectors, list ends with a trailing zero */

  oyFilter_s         * filter;         /**< the filter */
  char               * relatives_;     /**< hint about belonging to a filter */

  oyStruct_s         * backend_data;   /**< the filters private data */
};

oyFilterNode_s *   oyFilterNode_New  ( oyObject_s          object );
oyFilterNode_s *   oyFilterNode_Create(oyFilter_s        * filter,
                                       oyObject_s          object );
oyFilterNode_s *   oyFilterNode_Copy ( oyFilterNode_s    * node,
                                       oyObject_s          object );
int          oyFilterNode_Release    ( oyFilterNode_s   ** node );

oyConnector_s* oyFilterNode_Get      ( oyFilterNode_s    * node,
                                       oyConnector_s     * pattern,
                                       int                 input );

#define OY_FILTERNODE_FREE             0x01        /** list free nodes */
#define OY_FILTERNODE_CONNECTED        0x02        /** list connected nodes */
/* decode */
#define oyToFilterNode_Free_m(r)       ((r)&1)
#define oyToFilterNode_Connected_m(r)  (((r) >> 1)&1)
int            oyFilterNode_Count    ( oyFilterNode_s    * node,
                                       oyConnector_s     * pattern,
                                       int                 input,
                                       int                 flags );
#define OY_FILTERNODE_RESOLVE_AUTOMATIC 0x01        /** as far as possible */
/* decode */
#define oyToFilterNode_ResoveAutomatic_m(r)      ((r)&1)
int            oyFilterNode_Connect  ( oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       int                 flags );
OYAPI int  OYEXPORT
                 oyFilterNode_ConnectorMatch (
                                       oyFilterNode_s    * node_first,
                                       int                 pos_first,
                                       oyConnector_s     * connector_second );
OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterNode_GetSocket (
                                       oyFilterNode_s    * node,
                                       int                 pos );
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterNode_GetPlug( oyFilterNode_s    * node,
                                       int                 pos );




typedef struct oyPixelAccess_s oyPixelAccess_s;

/** @struct  oyPixelAccess_s
 *  @brief   control pixel access order
 *  @ingroup objects_conversion
 *
 *  A struct to control pixel access. It is a kind of flexible pixel 
 *  iterator. The order or pattern of access is defined by the array_xy and
 *  start_[x,y] variables.
 *
 *  The index variable specifies the iterator position in the array_xy index
 *  array.
 *
 *  oyPixelAccess_s is like a job ticket. Goal is to maintain all intermediate
 *  and processing dependend memory references here in this structure.
 *
 *  pixels_n says how many pixels are to be processed for the cache.
 *  pixels_n is used to calculate the buffers located with getBuffer
 *  and freeBuffer.
 *  The amount of pixel specified in pixels_n must be processed by
 *  each filter, because other filters are relying on a properly filled cache.
 *  This variable also determins the size of the next iteration.
 *
 *  The relation of pixels_n to array_xy and start_[x,y] is that a
 *  minimum of pixels_n must be processed by starting with start_[x,y]
 *  and processing pixels_n through array_xy. array_xy specifies
 *  the offset pixel distance to a next pixel in x and y directions. In case
 *  pixels_n is larger than array_n the array_xy has to be continued
 *  at array_xy[0,1] after reaching its end (array_n). \n
 *  \b Example: \n
 *  Thus a line iterator behaviour can be specified by simply setting 
 *  array_xy = {1,0}, for a advancement in x direction of one, array_n = 1, 
 *  as we need just this type of advancement and pixels_n = image_width, 
 *  for saying how often the pattern descibed in array_xy has to be applied.
 *
 *  Handling of pixel access is to be supported by a filter in a function of
 *  type oyCMMFilter_GetNext_f() in oyCMMapi4_s::oyCMMConnector_GetNext().
 *
 *  Access to the buffers by concurrenting threads is handled by passing
 *  different oyPixelAccess_s objects per thread.
 *
 *  From the backend point of view it is a requirement to obtain the 
 *  intermediate buffers from somewhere. These are the ones to read from and
 *  to write computed results into. \n
 *
 *  Pixel in- and output buffers separation:
 *  - Copy the output area and request to manipulate it by each filter.
 *    There is no overwriting of results.
 *    Reads a bit fixed. How can filters decide upon the input size?
 *    However, if a filter works on more than one dimension, it can
 *    opt to get its area directly from a input mediator.
 *  - Provide a opaque output and input area and request to copy by each filter.
 *    Filters would overwrite previous manipulations or some mechanism of
 *    swapping the input with the output side is needed.
 *  - Some filters want different input and output areas. They see the mediator
 *    as the previous, or the input, element in the graph.
 *  - Will the mediators always be visible in order to get all informations
 *    about the image? During setting up the graph this should be handled.
 *
 *  Access to input and output buffers:
 *  - The output oyArray2d_s is to be provided for 1D colour transforms.
 *  - The input oyArray2d_s is to be provided for multi dimensional
 *    manipulators directly from the input mediator.
 *
 *  Thread synchronisation:
 *  - The oyArray2d_s is a opaque memory blob. So different filters can act upon
 *    this resource. It would be in the resposiblity of the graph to avoid
 *    conflicts like using the same output for different manipulations. Given
 *    that the output is acting actively, the potential is small anyway.
 *  - The input should be neutral and not directly manipulated. What can happen
 *    is that different threads request the same input area and the according
 *    data is to be rendered first. So this easily could end in rendering two
 *    times for the same result. Some scheduling in the mediators may help
 *    solving this and improove on performance.
 *
 *  Area dimensions:
 *  - One point is very simple to provide. It may easily require additional
 *    preparations for area manipulations like blur.
 *  - Line is the next hard. The advantage it is still simple and speed
 *    efficient. Programming is a bit more demanding.
 *  - Areas of pixel are easy to provide through oyArray2d_s. It can include the
 *    above two cases.
 *  - Pattern accessors are very flexible for manipulators. It's not clear how
 *    the resulting complexity of translating the pattern to a array with known
 *    pixel positions can be hidden from other filters, which need to know about
 *    positions. One strategy would be to use mediators. They can request the
 *    according pixels from previous filters. A function to convert the pattern
 *    to a list of positions should be provided. Very elegant, but probably
 *    better to do later after oyArray2d_s.
 *
 *  Possible strategies are (old text):
 *  - Use mediators to convert between different pixel layouts and areas.
 *    These could cache the record of a successful query. Mediators are Nodes in
 *    the graph. As the graph and thus mediators can be accessed over
 *    concurrenting entries a cache tends to be expensive.
 *  - The oyPixelAccess_s could hold caches instead of mediators. It is the
 *    structure, which is owned by a given thread anyway.
 *    oyPixelAccess_s needs two buffers one for input and one for output.
 *    As the graph is asked to provide the next pixel via a oyPixelAccess_s
 *    struct, this struct must be associated with source and destination 
 *    buffers. The mediator on output has to search through the chain for the 
 *    previous
 *    mediator and ask there for the input buffer. The ouput buffer is provided
 *    by this mediator itself. These two buffers are set as the actual ones for
 *    processing by the normal filters. It must be clear, what is a mediator,
 *    for this scheme to work. As a mediator is reached in the processing graph,
 *    its task is not only to convert between buffers but as well to update the
 *    oyPixelAccess_s struct with the next mediators and its own buffer. Thus
 *    the next inbetween filters can process on their part.
 *    One advantage is that the mediators can pass their buffers to 
 *    oyPixelAccess_s, which are independent to threads and can be shared.
 *  - Each filter obtains a buffer being asked to fill it with the pixels 
 *    positions described in oyPixelAccess_s. A filter is free to create a new
 *    oyPixelAccess_s description and obtain for instance the surounding of the
 *    requested pixels. There is no caching to be expected other than in 
 *    the oyPixelAccess_s own output buffer.
 *
 *  @todo clear about pixel buffer copying, how to reach the buffers, thread
 *        synchronisation, simple or complex pixel areas (point, line, area,
 *        pattern )
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/04 (Oyranos: 0.1.8)
 *  @date    2008/10/22
 */
struct oyPixelAccess_s {
  oyOBJECT_e           type;           /**< internal struct type oyOBJECT_PIXEL_ACCESS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  int32_t          start_xy[2];        /**< the start point */
  int32_t          start_xy_old[2];    /**< @deprecated the previous start point */
  int32_t        * array_xy;           /**< @deprecated array of shifts, e.g. 1,0,2,0,1,0 */
  int              array_n;            /**< @deprecated the number of points in array_xy */

  int              index;              /**< to be advanced by the last caller */
  size_t           pixels_n;           /**< pixels to process/cache at once; should be set to 0 or 1 */

  int32_t          workspace_id;       /**< a ID to assign distinct resources to */
  oyStruct_s     * user_data;          /**< user data, e.g. for error messages*/
};

/** @enum    oyPIXEL_ACCESS_TYPE_e
 *  @brief   pixel access types
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyPIXEL_ACCESS_IMAGE,
  oyPIXEL_ACCESS_POINT,                /**< dont use */
  oyPIXEL_ACCESS_LINE                  /**< dont use */
} oyPIXEL_ACCESS_TYPE_e;

oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterSocket_s  * sock,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object );
oyPixelAccess_s *  oyPixelAccess_Copy( oyPixelAccess_s   * obj,
                                       oyObject_s          object );
int                oyPixelAccess_Release(
                                       oyPixelAccess_s  ** obj );

int                oyPixelAccess_CalculateNextStartPixel (
                                       oyPixelAccess_s   * obj,
                                       oyFilterPlug_s    * requestor_plug );

/** @struct oyConversion_s
 *  @brief  a filter chain or graph to manipulate a image
 *  @ingroup objects_conversion
 *
 *  Order of filters matters.
 *  The processing direction is a bit like raytracing as nodes request their
 *  parent.
 *
 *  The graph is allowed to be a directed graph without cycles.
 \dot
digraph G {
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="filled,rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="Oyranos Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal, arrowhead=none, label=request];
    b:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
    a:socket -> c:plug [arrowtail=normal, arrowhead=none, label=request];
    c:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
  }
}
 \enddot
 *  oyConversion_s shall provide access to the graph and help in processing
 *  and managing nodes.\n
 \dot
digraph G {
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  conversion [shape=plaintext, label=<
<table border="0" cellborder="1" cellspacing="0" bgcolor="lightgray">
  <tr><td>oyConversion_s</td></tr>
  <tr><td>
     <table border="0" cellborder="0" align="left">
       <tr><td align="left">...</td></tr>
       <tr><td align="left" port="in">+input</td></tr>
       <tr><td align="left" port="out">+out_</td></tr>
       <tr><td align="left">...</td></tr>
     </table>
     </td></tr>
  <tr><td> </td></tr>
</table>>,
                    style=""];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="oyConversion_s with attached Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal];
    b:socket -> d:plug [arrowtail=normal];
    a:socket -> c:plug [arrowtail=normal];
    c:socket -> d:plug [arrowtail=normal];

    conversion:in -> a;
    conversion:out -> d;
  }

  conversion
}
 \enddot
 *  \b Creating \b Graphs: \n
 *  Most simple is to use the oyConversion_CreateBasic() function to create
 *  a profile to profile and possible image buffer to image buffer linear
 *  graph.\n
 *  The next possibility is to create a linear graph by chaining linear nodes
 *  together with oyConversion_CreateInput(), oyConversion_FilterAdd() and
 *  oyConversion_OutputAdd() in that order. A linear node is one that can have
 *  exactly one parent and one child node. The above scheme illustrates a linear
 *  graph.\n
 *  The last possibility is to create a non linear graph. The input member
 *  be accessed for this directly.
 *
 *  While it would be possible to have several open ends in a graph, there
 *  are two endpoints considered as special. The input member prepresents the
 *  top most required node to be provided in a oyConversion_s graph. The
 *  input node is accessible for user manipulation. The other one is the out_
 *  member. It is the closing node in the graph. It will be set by Oyranos
 *  during closing the graph, e.g. in oyConversion_OutputAdd().
 *
 *  \b Using \b Graphs: \n
 *  To obtain the data the oyConversion_GetNextPixel() and
 *  oyConversion_GetOnePixel() functions are available.
 \dot
digraph G {
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=12];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];
  app [ label="application", style=filled ]

  subgraph cluster_0 {
    label="Data Flow";
    color=gray;
    a:socket -> b:plug [label=data];
    b:socket -> d:plug [label=data];
    a:socket -> c:plug [label=data];
    c:socket -> d:plug [label=data];
    d:socket -> app [label=<<table  border="0" cellborder="0"><tr><td>return of<br/>oyConversion_GetNextPixel()</td></tr></table>>];
  }
}
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONVERSION_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyFilterNode_s     * input;          /**< the input image filter; Most users will start logically with this pice and chain their filters to get the final result. */
  oyFilterNode_s     * out_;           /**< the Oyranos output image. Oyranos will stream the filters starting from the end. This element will be asked on its first plug. */
} oyConversion_s;

oyConversion_s  *  oyConversion_CreateBasic (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
oyConversion_s  *  oyConversion_CreateInput (
                                       oyImage_s         * input,
                                       oyObject_s          object );
oyConversion_s  *  oyConversion_Copy ( oyConversion_s    * conversion,
                                       oyObject_s          object );
int                oyConversion_Release (
                                       oyConversion_s   ** conversion );


int                oyConversion_FilterAdd (
                                       oyConversion_s    * conversion,
                                       oyFilter_s        * filter );
int                oyConversion_OutputAdd (
                                       oyConversion_s    * conversion,
                                       oyImage_s         * input );
int                oyConversion_Run  ( oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access,
                                       oyRegion_s        * region );
oyPointer        * oyConversion_GetOnePixel (
                                       oyConversion_s    * conversion,
                                       int32_t             x,
                                       int32_t             y,
                                       int32_t           * feedback );
oyProfile_s      * oyConversion_ToProfile (
                                       oyConversion_s    * conversion );
int             ** oyConversion_GetAdjazenzlist (
                                       oyConversion_s    * conversion,
                                       oyAlloc_f           allocateFunc );
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
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
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_COLOUR_CONVERSION_S */
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

int          oyColourConversion_Run  ( oyColourConversion_s * colour );
oyProfile_s* oyColourConversion_ToProfile ( oyColourConversion_s * s );

/** @struct oyNamedColour_s
 *  @brief colour patch with meta informations
 *  @ingroup objects_single_colour
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
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_NAMED_COLOUR_S */
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
 *  @ingroup objects_single_colour
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
  oyOBJECT_e           type_;          /*!< struct type oyOBJECT_NAMED_COLOURS_S */
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

/** @brief   icon data
 *  @ingroup cmm_handling
 *
 *  Since: 0.1.8
 */
typedef struct {
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_ICON_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;
  int              height;
  float          * data;               /*!< should be sRGB matched */
  char           * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
} oyIcon_s;

typedef struct oyCMMapi_s oyCMMapi_s;

/** @brief   the CMM API resources struct to implement and set by a CMM
 *  @ingroup cmm_handling
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
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_CMM_INFO_S */
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

    @param[in]     cmm                 the 4 char CMM ID or zero for the current CMM
    @param[in]     object              the optional base
    @return                            available options
 */
const char *   oyModulGetOptions     ( const char        * cmm,
                                       oyObject_s          object);
const char *   oyModuleGetActual     ( oyFILTER_TYPE_e     type );


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
