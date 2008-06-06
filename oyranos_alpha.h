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

typedef struct oyStruct_s oyStruct_s;

typedef oyStruct_s * (*oyStruct_CopyF_t) ( oyStruct_s *, oyPointer );
typedef int       (*oyStruct_ReleaseF_t) ( oyStruct_s ** );
typedef oyPointer (*oyStruct_copyF_t)  ( oyPointer, oyPointer );
typedef int       (*oyStruct_releaseF_t) ( oyPointer * );
typedef oyPointer (*oyStruct_LockCreateF_t) ( oyStruct_s * obj );
typedef void      (*oyLockReleaseF_t)( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyLockF_t)       ( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyUnLockF_t)     ( oyPointer           look,
                                       const char        * marker,
                                       int                 line );

typedef oyPointer (*oyImage_GetPoint_t)( int               point_x,
                                         int               point_y );
/*typedef oyPointer (*oyImage_GetLine_t) ( int               line_y,
                                         int             * height );
typedef oyPointer (*oyImage_GetTile_t) ( int               tile_x,
                                         int               tile_y );*/


void         oyThreadLockingSet      ( oyStruct_LockCreateF_t createLockFunc,
                                       oyLockReleaseF_t    releaseLockFunc,
                                       oyLockF_t           lockFunc,
                                       oyUnLockF_t         unlockFunc );

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
  oyOBJECT_TYPE_OBJECT_S,
  oyOBJECT_TYPE_DISPLAY_S,
  oyOBJECT_TYPE_NAMED_COLOUR_S,       /*!< oyNamedColour_s */
  oyOBJECT_TYPE_NAMED_COLOURS_S,      /*!< oyNamedColours_s */
  oyOBJECT_TYPE_PROFILE_S,            /*!< oyProfile_s */
  oyOBJECT_TYPE_PROFILE_TAG_S,        /*!< oyProfileTag_s */
  oyOBJECT_TYPE_PROFILE_LIST_S,       /*!< oyProfileList_s */
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
  oyOBJECT_TYPE_IMAGE_HANDLER_S,      /*!< oyImageHandler_s */
  oyOBJECT_TYPE_COLOUR_CONVERSION_S,  /*!< oyColourConversion_s */
  oyOBJECT_TYPE_CMM_HANDLE_S = 50,    /**< oyCMMhandle_s */
  oyOBJECT_TYPE_CMM_POINTER_S,        /*!< oyCMMptr_s */
  oyOBJECT_TYPE_CMM_INFO_S,           /*!< oyCMMInfo_s */
  oyOBJECT_TYPE_CMM_API_S,            /**< oyCMMapi_s */
  oyOBJECT_TYPE_CMM_API1_S,           /**< oyCMMapi1_s */
  oyOBJECT_TYPE_CMM_API2_S,           /**< oyCMMapi2_s */
  oyOBJECT_TYPE_CMM_API3_S,           /**< oyCMMapi3_s */
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
};

oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size );

typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

typedef enum {
  oySTRING,                            /**< not specified */
  oySTRING_PROD_NAME,                  /**< ??? */
  oySTRING_PROD_DESC,                  /**< icSigProfileDescriptionTag */
  oySTRING_INFO,                       /**< */
  oySTRING_COPYRIGHT,                  /**< icSigCopyrightTag */
  oySTRING_MODELL,                     /**< icSigDeviceModelDescTag */
  oySTRING_MANUFACTURER                /**< icSigDeviceMfgDescTag */
} oySTRING_e;

#define oyNAME_ID oyNAME_NICK

/** @brief Oyranos name structure
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_NAME_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char               * nick;    /*!< few letters for mass representation, eg. "A1" */
  char               * name;           /*!< normal visible name, eg. "A1-MySys"*/
  char               * description;    /*!< full user description, eg. "A1-MySys from Oyranos" */
  oySTRING_e           string_type;    /**< type of string */
  char                 lang[8];        /**< i18n language, eg. "en_GB" */
} oyName_s;

oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
int          oyName_release_         ( oyName_s         ** name,
                                       oyDeAllocFunc_t     deallocateFunc );
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAllocFunc_t       allocateFunc,
                                       oyDeAllocFunc_t     deallocateFunc );

#define OY_HASH_SIZE 16


/** @brief CMM pointer
 *
 *  The oyCMMptr_s is used internally and for CMM's.
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_CMM_POINTER_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char                 cmm[5];         /*!< the CMM */
  char                 func_name[32];  /*!< optional the CMM's function name */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  char                 resource[5];    /**< the resource type */
  oyStruct_releaseF_t  ptrRelease;     /*!< CMM's deallocation function */
  int                  ref;            /**< Oyranos reference counter */
} oyCMMptr_s;

oyCMMptr_s *       oyCMMptr_New_     ( oyAllocFunc_t       allocateFunc );
oyCMMptr_s *       oyCMMptr_Copy_    ( oyCMMptr_s        * cmm_ptr,
                                       oyAllocFunc_t       allocateFunc );
int                oyCMMptr_Release_ ( oyCMMptr_s       ** cmm_ptr );

int                oyCMMptr_Set_     ( oyCMMptr_s        * cmm_ptr,
                                       const char        * cmm,
                                       const char        * func_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       oyStruct_releaseF_t ptrRelease );

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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  oyPointer            ptr;            /**< can be any type */
  oyOBJECT_TYPE_e      ptr_type;       /**< the type of the entry */
  oyStruct_releaseF_t  ptrRelease;     /**< deallocation for ptr_ of list_type*/
  oyStruct_copyF_t     ptrCopy;        /**< copy for ptr_ of list_type */
} oyHandle_s;

oyHandle_s *       oyHandle_new_     ( oyAllocFunc_t       allocateFunc );
oyHandle_s *       oyHandle_copy_    ( oyHandle_s        * orig,
                                       oyAllocFunc_t       allocateFunc );
int                oyHandle_release_ ( oyHandle_s       ** handle );

int                oyHandle_set_     ( oyHandle_s        * handle,
                                       oyPointer           ptr,
                                       oyOBJECT_TYPE_e     ptr_type,
                                       oyStruct_releaseF_t ptrRelease,
                                       oyStruct_copyF_t    ptrCopy );


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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyAllocFunc_t        allocateFunc_;  /**< data  allocator */
  oyDeAllocFunc_t      deallocateFunc_;/**< data release function */
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
oyObject_s   oyObject_NewWithAllocators(oyAllocFunc_t     allocateFunc,
                                        oyDeAllocFunc_t   deallocateFunc );
oyObject_s   oyObject_NewFrom         ( oyObject_s        object );
oyObject_s   oyObject_Copy            ( oyObject_s        object );
int          oyObject_Release         ( oyObject_s      * oy );

oyObject_s   oyObject_SetParent       ( oyObject_s        object,
                                        oyOBJECT_TYPE_e   type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s        oy,
                                        size_t          * size,
                                        oyAllocFunc_t     allocateFunc );*/

int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
int          oyObject_SetName         ( oyObject_s        object,
                                        const oyChar    * name,
                                        oyNAME_e          type );
const oyChar*oyObject_GetName         ( const oyObject_s  object,
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< features name and hash */
  oyStruct_s        ** ptr_;           /**< the list data */
  int                  n_;             /**< the number of visible pointers */
  int                  n_reserved_;    /**< the number of allocated pointers */
  oyChar             * list_name;      /**< name of list */
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
oyChar *     oyCMMCacheListPrint_    ( void );


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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  uint32_t             id;             /**< id to map for instance to events and widgets */
  oyName_s             name;           /**< nick, name, description/help */
  const char         * config_path;    /**< full key name to store configuration, use three point separated string, eg "org.oyranos.lcms" */
  const char         * config_key;     /**< key name to store configuration, eg "transform_precalculation" */
  oyVALUETYPE_e        value_type;     /**< the type in value */
  oyValue_u            value;          /**< the actual value */
  oyValue_u            standard;       /**< the standard value */
  oyValue_u            start;          /**< value range start */
  oyValue_u            end;            /**< value range end */
  uint32_t             flags;          /**<  */
} oyOption_s;

/** @brief Options for rendering
    Options can be any flag or rendering intent and other informations needed to
    configure a process. The object contains variables for colour transforms.
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_OPTIONS_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  uint32_t             n;              /*!< number of options */
  oyOption_s         * opts;
} oyOptions_s;
#if 0
/** allocate n oyOption_s */
oyOptions_s*   oyOptions_Create       ( int n,
                                        oyObject_s        object);
/** allocate oyOption_s for a 4 char CMM identifier obtained by oyModulsGetNames
 */
oyOptions_s*   oyOptions_CreateFor    ( const char      * cmm,
                                        oyObject_s      * object);
void           oyOptions_Release      ( oyOptions_s     * options );
oyPointer      oyOptions_Align        ( oyOptions_s     * options,
                                        size_t          * size,
                                        oyAllocFunc_t     allocateFunc );

/** confirm if all is ok

  @param[in]   opts      the options to verify
  @param[in]   cmm       the CMM to check for
  @return                NULL for no error, or non conforming options
 */
oyOptions_s*   oyOptions_VerifyForCMM ( oyOptions_s     * opts,
                                        const char      * cmm,
                                        oyObject_s        object);
#endif

typedef struct oyProfileTag_s oyProfileTag_s;

/** @brief a profile and its attributes
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_PROFILE_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  oyChar             * file_name_;     /*!< file name for loading on request */
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
                   oyProfile_Copy    ( oyProfile_s       * profile,
                                       oyObject_s          object);
OYAPI int  OYEXPORT oyProfile_Release( oyProfile_s      ** profile );
#if 0
OYAPI oyPointer OYEXPORT
                   oyProfile_Align   ( oyProfile_s     * profile,
                                       size_t          * size,
                                       oyAllocFunc_t     allocateFunc );
#endif
OYAPI int OYEXPORT oyProfile_GetChannelsCount ( oyProfile_s * colour );
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name );

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
  oySIGNATURE_CLASS,                   /**< usage class 'mntr' ... */
  oySIGNATURE_MAGIC,                   /**< magic; ICC: 'acsp' */
  oySIGNATURE_PLATFORM,                /**< operating system */
  oySIGNATURE_OPTIONS,                 /**< various ICC header flags */
  oySIGNATURE_MANUFACTURER,            /**< device manufacturer */
  oySIGNATURE_MODEL,                   /**< device modell */
  oySIGNATURE_INTENT,                  /**< seldom used profile claimed intent*/
  oySIGNATURE_CREATOR                  /**< profile creator ID */
} oySIGNATURE_TYPE_e;

OYAPI icSignature OYEXPORT
                   oyProfile_GetSignature ( oyProfile_s * profile,
                                            oySIGNATURE_TYPE_e type );
OYAPI void OYEXPORT oyProfile_SetChannelNames( oyProfile_s * colour,
                                         oyObject_s      * names_chan );
OYAPI const oyObject_s * OYEXPORT
                   oyProfile_GetChannelNames( oyProfile_s * colour);
OYAPI const oyChar * OYEXPORT
                   oyProfile_GetChannelName ( oyProfile_s * profile,
                                         int               channel_pos,
                                         oyNAME_e          type );
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile );
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * profile,
                                       oyNAME_e            type );
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAllocFunc_t       allocateFunc );
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
const oyChar *     oyProfile_GetFileName ( oyProfile_s   * profile,
                                       int                 dl_pos );


/** @brief tell about the conversion profiles
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_PROFILE_LIST_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  oyStructList_s     * list_;          /**< list of profiles */
} oyProfileList_s;

OYAPI oyProfileList_s * OYEXPORT
                   oyProfileList_New   ( oyObject_s        object );
OYAPI oyProfileList_s * OYEXPORT
                   oyProfileList_Copy  ( oyProfileList_s * profile_list,
                                         oyObject_s        object);
OYAPI int  OYEXPORT
                 oyProfileList_Release(oyProfileList_s  ** profile_list );

oyProfileList_s* oyProfileList_MoveIn( oyProfileList_s   * list,
                                       oyProfile_s      ** ptr,
                                       int                 pos );
int              oyProfileList_ReleaseAt(oyProfileList_s * list,
                                       int                 pos );
oyProfile_s *    oyProfileList_Get   ( oyProfileList_s   * list,
                                       int                 pos );
int              oyProfileList_Count ( oyProfileList_s   * list );
 

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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
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
oyChar **      oyProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAllocFunc_t       allocateFunc );

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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;
  float x;
  float y;
  float width;
  float height;
} oyRegion_s;

oyRegion_s *   oyRegion_New_         ( oyObject_s          object );
oyRegion_s *   oyRegion_NewWith_     ( oyObject_s          object,
                                       float               x,
                                       float               y,
                                       float               width,
                                       float               height );
oyRegion_s *   oyRegion_NewFrom_     ( oyObject_s          object,
                                       oyRegion_s        * ref );
oyRegion_s *   oyRegion_Copy_        ( oyRegion_s        * region,
                                       oyObject_s          object );
int            oyRegion_Release_     ( oyRegion_s       ** region );

void           oyRegion_SetGeo_      ( oyRegion_s        * edit_region,
                                       float               x,
                                       float               y,
                                       float               width,
                                       float               height );
void           oyRegion_SetByRegion_ ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_Trim_        ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_MoveInside_  ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref );
void           oyRegion_Scale_       ( oyRegion_s        * edit_region,
                                       float               factor );
void           oyRegion_Normalise_   ( oyRegion_s        * edit_region );
void           oyRegion_Round_       ( oyRegion_s        * edit_region );
int            oyRegion_IsEqual_     ( oyRegion_s        * region1,
                                       oyRegion_s        * region2 );
int            oyRegion_IsInside_    ( oyRegion_s        * region,
                                       float               x,
                                       float               y );
int            oyRegion_CountPoints_ ( oyRegion_s        * region );
int            oyRegion_Index_       ( oyRegion_s        * region,
                                       float               x,
                                       float               y );
oyChar*        oyRegion_Show_        ( oyRegion_s        * region );


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

oyChar *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAllocFunc_t       allocateFunc );


typedef struct oyImage_s_ oyImage_s;

/** @struct oyImageHandler_s
 *  @brief a advanced image processing struct
 *
 *  Eighter specify the line or tile interface.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< struct type oyOBJECT_TYPE_IMAGE_HANDLER_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */

  oyPointer        dummy;              /**< keep to zero */
  oyImage_GetPoint_t   getPoint;       /**< the point interface */
/*  oyImage_GetLine_t    getLine;*/        /**< the line interface */
/*  oyImage_GetTile_t    getTile;*/        /**< the tile interface */
/*  int                  tile_width;*/     /**< needed by the tile interface */
/*  int                  tile_height;*/    /**< needed by the tile interface */
} oyImageHandler_s;

oyImageHandler_s * oyImageHandler_Create ( oyImage_s     * image );

/** @brief a reference struct to gather information for image transformation

    as we dont target a complete imaging solution, only raster is supported

    oyImage_s should hold image dimensions,
    oyDisplayRegion_s information and
    a reference to the data for conversion

    As well referencing of itself would be nice, to allow light copies.

    Should oyImage_s become internal and we provide a user interface?
 */
struct oyImage_s_ {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_IMAGE_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyPointer            data;           /*!< image data */
  oyOptions_s        * options_;       /*!< for instance channel layout */
  oyProfile_s        * profile_;       /*!< image profile */
  oyRegion_s         * region;      /*!< region to render, if zero render all */
  int                  display_pos_x;  /*!< upper position on display of image*/
  int                  display_pos_y;  /*!< left position on display of image */
  oyPixel_t          * layout_;  /*!< internal samples mask (3,384,2,1,0 BGR) */
  oyImageHandler_s   * handler;        /**< handler; alternative to full data */
};


oyImage_s *    oyImage_Create         ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s        object);
oyImage_s *    oyImage_CreateForDisplay( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        const char      * display_name,
                                        int               display_pos_x,
                                        int               display_pos_y,
                                        oyObject_s        object);
oyImage_s *    oyImage_Copy           ( oyImage_s       * image,
                                        oyObject_s        object );
int            oyImage_Release        ( oyImage_s      ** image );


int            oyImage_SetCritical    ( oyImage_s       * image,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyOptions_s     * options );

/** @struct oyColourConversion_s
    In case where
      a option indicates monitor output, or
      the out image struct has no profile set, 
    the conversion will route to monitor colours, honouring the oyImage_s screen
    position.
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /*!< struct type oyOBJECT_TYPE_COLOUR_CONVERSION_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< bse object */
  oyProfileList_s    * profiles_;      /*!< effect / simulation profiles */ 
  oyOptions_s        * options_;       /*!< conversion opts */
  oyImage_s          * image_in_;      /*!< input */
  oyImage_s          * image_out_;     /*!< output */
  oyStructList_s     * cmms_;          /**< list of CMM entries to call */
} oyColourConversion_s;

oyColourConversion_s* oyColourConversion_Create (
                                       oyProfileList_s   * list,
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
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
const oyChar *    oyNamedColour_GetName( oyNamedColour_s * s,
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /*!< base object */
  oyStructList_s     * list_;          /**< colour list */
} oyNamedColours_s;

oyNamedColours_s* oyNamedColours_New ( oyObject_s       object );
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          object );
int               oyNamedColours_Release ( oyNamedColours_s** colours );

int               oyNamedColours_Count( oyNamedColours_s * swatch );
oyNamedColour_s*  oyNamedColours_GetRef ( oyNamedColours_s  * swatch,
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;
  int              height;
  float          * data;               /*!< should be sRGB matched */
  oyChar         * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
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
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char             cmm[4];             /*!< ICC signature, eg 'lcms' */
  oyChar         * backend_version;    /*!< non translatable, eg "v1.17" */
  oyName_s         name;               /*!< translatable, eg "lcms" "little cms" "..." */
  oyName_s         manufacturer;       /*!< translatable, eg "Marti" "Marti Maria" "support email: @; internet: www.littlecms.com; sources: ..." */
  oyName_s         copyright;          /*!< translatable, eg "MIT","MIT License".. */
  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */

  oyCMMapi_s     * api;                /**< must be casted to a according API */
  int              apis_n;             /**< count of implemented apis */

  oyIcon_s         icon;               /*!< zero terminated list of a icon pyramid */
} oyCMMInfo_s;

/** @internal
 *  @brief a CMM handle to collect resources
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_CMM_HANDLE_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  char                 cmm[5];         /**< the CMM */
  oyCMMInfo_s        * info;           /**< the modules info struct */
  oyPointer            dso_handle;     /**< the ldopen library handle */
} oyCMMhandle_s;

oyCMMhandle_s *  oyCMMhandle_New_    ( oyObject_s          object );
oyCMMhandle_s *  oyCMMhandle_Copy_   ( oyCMMhandle_s     * handle,
                                       oyObject_s          object );
int              oyCMMhandle_Release_( oyCMMhandle_s    ** handle );

int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMInfo_s       * info,
                                       oyPointer           dso_handle );

/*#if 10*/

typedef struct oyWidget_s oyWidget_s;

typedef enum {
  oyWIDGET_HORIZONTAL,
  oyWIDGET_VERTICAL
} oyWIDGET_ORIENTATION_e;


/** @enum   oyWIDGET_GROUP_INITIAL_VISIBILITY_e
 *  @brief  group widget styles
 *
 *  Initially a group appears as a tab or similiarily as an unexpanded endity.
 *  Tabs show normally alternatives as a row of buttons and aside the content.
 *  You migth use a cloud of buttons or a choice instead.
 *
 *  Of course there can be lists of widget be separated on a page by frames.
 *  The frame should show 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/27 (API 0.1.8)
 */
typedef enum {
  oyWIDGET_GROUP_INITIALLY_INVISIBLE,  /**< a unexpanded group */
  oyWIDGET_GROUP_INITIALLY_VISIBLE     /**< a expanded group */
} oyWIDGET_GROUP_INITIAL_VISIBILITY_e;

/** @enum   oyWIDGET_GROUP_FRAME_e
 *  @brief  group widget styles
 *
 *  A group can be placed side by side with other groups. This is very common
 *  among horizontal and vertical placement, where all entries belong to one
 *  logical endity. 
 *  For instance if you like a slider with a number beside been gouped
 *  in a colum without a frame around each slider then use 
 *  oyWIDGET_GROUP_FRAME_NO.
 *  In case the group will get a name and or shall be separated from other
 *  entries use oyWIDGET_GROUP_FRAME_SHOW.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/28 (API 0.1.8)
 */
typedef enum {
  oyWIDGET_GROUP_FRAME_NO,             /**< only for layout purpose */
  oyWIDGET_GROUP_FRAME_SHOW            /**< show the group */
} oyWIDGET_GROUP_FRAME_e;

#if 0
typedef enum {
  oyWIDGET_USERGROUP_BEGINNER,         /**< common options */
  oyWIDGET_USERGROUP_ADVANCED,         /**< additional options for users with some knowledge about */
  oyWIDGET_USERGROUP_EXPERTS,          /**< expert with some knowledge */
  oyWIDGET_USERGROUP_TECHNIC,          /**< expert with some knowledge */
} oyWIDGET_USERGROUP_e;
#endif

/** @struct oyWidgetGroup_s
 *  @brief  group widget parameters
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/27 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_GROUP_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  /*oyWIDGET_USERGROUP_e user_target;*/    /**< user / advanced / expert / technic*/
  oyWidget_s        ** widgets;        /**< list of all sub widgets, without widgets the group should be interpretet as a separating line only */
  uint32_t             widgets_n;      /**< number of widgets in this group */
  oyWIDGET_ORIENTATION_e layout;       /**< horizontal, vertical */
  oyWIDGET_GROUP_INITIAL_VISIBILITY_e initial_visibility; /**< non (tab/choice) or visible (like in a frame or radio buttons ) */
  oyWIDGET_GROUP_FRAME_e mark;         /**< mark visibly like with a frame */
  int32_t              visible;        /**< status */
} oyWidgetGroup_s;

/** @struct oyWidgetSlider_s
 *  @brief  slider widget parameters
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/27 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_SLIDER_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  double      range_start;      /**< start of range for a value selection */
  double      range_end;        /**< end of range for a value selection */
  double      range_step_major; /**< step size for a value selection */
  double      range_step_minor; /**< step size for a value selection */
} oyWidgetSlider_s;

typedef struct oyWidgetChoice_s oyWidgetChoice_s;

/** @struct oyWidgetChoice_s
 *  @brief  choice widget parameters
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/27 (API 0.1.8)
 */
struct oyWidgetChoice_s {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_CHOICE_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  int32_t              choices_n;      /**< number of options */
  char              ** choices;        /**< label for each choice */
  uint32_t             flags;          /**< */
};

/** @enum   oyWIDGET_BUTTON_TYPE_e
 *  @brief  button widget styles
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/28 (API 0.1.8)
 */
typedef enum {
  oyWIDGET_BUTTON_NORMAL,
  oyWIDGET_BUTTON_CHECK_BOX,
  oyWIDGET_BUTTON_RADIO
} oyWIDGET_BUTTON_TYPE_e;

/** @struct oyWidgetButton_s
 *  @brief  button widget parameters
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/28 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_BUTTON_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyWIDGET_BUTTON_TYPE_e button_type;  /**< button style */
  oyIcon_s           * icon_active;    /**< button icon */
  oyIcon_s           * icon_passive;   /**< button icon */
  oyIcon_s           * icon_highligthed; /**< button icon */
} oyWidgetButton_s;

/** @enum    oyORIENTATION_e
 *  @brief   orientation type
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.8)
 *  @date    2008/02/16
 */
typedef enum {
  oyORIENTATION_TOP,
  oyORIENTATION_TOP_RIGHT,
  oyORIENTATION_RIGHT,
  oyORIENTATION_BOTTOM_RIGHT,
  oyORIENTATION_BUTTOM,
  oyORIENTATION_BUTTOM_LEFT,
  oyORIENTATION_LEFT,
  oyORIENTATION_TOP_LEFT
} oyORIENTATION_e;

/** @enum    oyTEXT_EMPHASISE_e
 *  @brief   text styles
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.8)
 *  @date    2008/02/16
 */
typedef enum {
  oyTEXT_EMPHASISE_NORMAL,
  oyTEXT_EMPHASISE_H1,                 /**< title size relative to widgets hierarchy */
  oyTEXT_EMPHASISE_H2,
  oyTEXT_EMPHASISE_H3,
  oyTEXT_EMPHASISE_ANNOTATION          /**< rather small */
} oyTEXT_EMPHASISE_e;

/** @enum   oyWIDGET_TEXT_TYPE_e
 *  @brief  text widget styles
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.8)
 *  @date    2008/02/16
 */
typedef enum {
  oyWIDGET_TEXT_DISPLAY,               /**< normal, flat text */
  oyWIDGET_TEXT_OUTPUT,                /**< emphasised text */
  oyWIDGET_TEXT_INPUT                  /**< interactive text for input */
} oyWIDGET_TEXT_TYPE_e;

/** @struct oyWidgetText_s
 *  @brief  text widget parameters
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.8)
 *  @date    2008/02/16
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_TEXT_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyWIDGET_TEXT_TYPE_e text_type;      /**< text style */
  uint32_t             min_lines;      /**< minimal text lines */
  oyORIENTATION_e      align;          /**< direction the text should touch */
} oyWidgetText_s;


/** @union  oyWidgetWidget_u
 *  @brief  widget parameters
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/14 (API 0.1.8)
 */
typedef union {
  oyWidgetSlider_s         vslider;
  oyWidgetChoice_s         vchoice;
  oyWidgetGroup_s          vgroup;
  oyWidgetButton_s         vbutton;
  oyWidgetText_s           vtext;
  oyStruct_s               vcommon;
} oyWidgetParameters_u;

/** @struct  oyWidgetLayout_s
 *  @brief   widget layout
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.8)
 *  @date    2008/02/16
 */
typedef struct {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_PARAM_TEXT_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  uint32_t             min_width;      /**< opt out with 0 */
  uint32_t             min_heigth;
  uint32_t             max_width;
  uint32_t             max_heigth;
  uint32_t             percentage;     /**< the filling size to align widgets inside a group, e.g. 80% for a slider and 20% for other widgets in the group */
  uint32_t             border;         /**< border in pixel */
} oyWidgetLayout_s;

#define oyWIDGET_HAS_FOCUS   0x01
#define oyWIDGET_IS_VISIBLE  0x02

/** @struct oyWidget_s
 *  @brief  a user settings widget
 *
 *  The struct contains a common description for all toolkits.
 *  One Widget coresponds to one oyOption_s object via the id field.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 january 2008 (API 0.1.8)
 */
struct oyWidget_s {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_WIDGET_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  
  oyOBJECT_TYPE_e      param_type;     /**< type, the same value as in parameter::type */
  oyWidgetParameters_u * parameter;    /**< widget parameters */
  uint32_t             id;         /**< identification to map a option */
  uint32_t             flags;          /**< flags to control widget behaviour, like oyWIDGET_HAS_FOCUS, oyWIDGET_IS_VISIBLE */
  oyRegion_s         * region;         /**< actual region on screen */
  oyOption_s         * option;         /**< the actual value */
  oyWidgetLayout_s   * layout;         /**< size hints for alignment */
};
/*#endif*/

/** @struct oyWidgets_s
 *  @brief  user settings widgets
 *
 *  The struct contains a common description for all toolkits.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/04/14 (Oyranos: 0.1.8)
 *  @date    2008/04/14
 */
struct oyWidgets_s {
  oyOBJECT_TYPE_e      type_;          /**< internal struct type oyOBJECT_TYPE_WIDGET_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyWidget_s         * widgets;        /**< the widgets */
  uint32_t             widgets_n;      /**< the number of widgets */
};



/** @deprecated */
int            oyModulRegisterXML    ( oyGROUP_e           group,
                                       const char        * xml );

/** obtain 4 char CMM identifiers and count of CMM's */
char **        oyModulsGetNames      ( int               * count,
                                       oyAllocFunc_t       allocateFunc );
/** Query for available options for a cmm

    @param[in] cmm      the 4 char CMM ID or zero for the current CMM
    @return             available options
 */
oyOptions_s *  oyModulGetOptions     ( const char        * cmm,
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

int            oyICCColourSpaceGetChannelCount ( icColorSpaceSignature color );
const char *   oyICCColourSpaceGetName ( icColorSpaceSignature sig );
const oyChar * oyICCColourSpaceGetChannelName ( icColorSpaceSignature sig,
                                       int                 pos,
                                       int                 type );
const oyChar * oyICCTagDescription   ( icTagSignature      sig );
const oyChar * oyICCTagName          ( icTagSignature      sig );
const oyChar * oyICCDeviceClassDescription ( icProfileClassSignature sig );
const oyChar * oyICCPlatformDescription ( icPlatformSignature platform );
const oyChar * oyICCTagTypeName      ( icTagTypeSignature  sig );
const oyChar * oyICCTechnologyDescription ( icTechnologySignature sig );
const oyChar * oyICCChromaticityColorantDescription ( icSignature sig );
const oyChar * oyICCIlluminantDescription ( icIlluminant sig );
const oyChar * oyICCStandardObserverDescription ( icStandardObserver sig );
const oyChar * oyICCMeasurementGeometryDescription ( icMeasurementGeometry sig );
const oyChar * oyICCMeasurementFlareDescription ( icMeasurementFlare sig );


oyChar *       oyDumpColourToCGATS   ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAllocFunc_t       allocateFunc,
                                       const oyChar      * DESCRIPTOR );



#endif /* OY_IN_PLANING */

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
