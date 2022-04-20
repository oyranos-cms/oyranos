/** @file oyranos_cmm_lcm2.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2007-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    littleCMS CMM module for Oyranos
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2007/11/12
 */


#include <lcms2.h>
#include <stdarg.h>
#include <stdlib.h>

#include "oyArray2d_s_.h"
#include "oyCMM_s.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMui_s_.h"
#include "oyConnectorImaging_s_.h"
#include "oyImage_s.h"
#include "oyProfiles_s.h"
#include "oyStructList_s.h"

#include "oyranos_cmm.h"         /* the API's this CMM implements */
#include "oyranos_config_internal.h"
#include "oyranos_generic.h"         /* oy_connector_imaging_static_object */
#include "oyranos_helper.h"      /* oySprintf_ and other local helpers */
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_image.h"
#include "oyranos_object_internal.h"
#include "oyranos_string.h"

#include "oyranos_cmm_lcm2.i18n.c" /* oyranos_json define */

#ifdef _OPENMP
#define USE_OPENMP 1
#include <omp.h>
#endif

extern oyCMMapi4_s_   l2cms_api4_cmm;
/*
oyCMM_s               lcm2_cmm_module;
oyCMMapi4_s           l2cms_api4_cmm;
oyCMMui_s             l2cms_api4_ui;
oyCMMapi7_s           l2cms_api7_cmm;
oyConnectorImaging_s* l2cms_cmmIccSocket_connectors[2];
oyConnectorImaging_s  l2cms_cmmIccSocket_connector;
oyConnectorImaging_s* l2cms_cmmIccPlug_connectors[2];
oyConnectorImaging_s  l2cms_cmmIccPlug_connector;
oyCMMapi6_s           l2cms_api6_cmm;                    OY_LCM2_DATA_CONVERT_REGISTRATION
oyCMMapi10_s          l2cms_api10_cmm;                   OY_LCM2_CREATE_ABSTRACT_PROOFING_REGISTRATION
oyCMMapi10_s          l2cms_api10_cmm2;                  OY_LCM2_CREATE_MATRIX_REGISTRATION
oyCMMapi10_s          l2cms_api10_cmm3;                  OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION
oyCMMapi10_s          l2cms_api10_cmm4;                  OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION
oyCMMapi10_s          l2cms_api10_cmm5;                  OY_LCM2_PARSE_CGATS
*/

void* oyAllocateFunc_           (size_t        size);
void* oyAllocateWrapFunc_       (size_t        size,
                                 oyAlloc_f     allocate_func);
void  oyDeAllocateFunc_         (void *        data);

#include <math.h>
#include <string.h>                    /* memcpy */


/* --- internal definitions --- */

#define CMM_NICK "lcm2"
#define CMMProfileOpen_M    l2cmsOpenProfileFromMemTHR
#define CMMProfileRelease_M l2cmsCloseProfile
#define CMMToString_M(text) #text
#define CMMMaxChannels_M 16
#define l2cmsPROFILE   "lcP2"
#define l2cmsTRANSFORM "lcC2"
/** The proofing LUTs grid size may improove the sharpness of out of color 
 *  marking, but at the prise of lost speed and increased memory consumption.
 *  53 is the grid size used internally in l2cms' gamut marking code. */
#define l2cmsPROOF_LUT_GRID_RASTER 53
/*#define ENABLE_MPE 1*/

#define CMM_VERSION {0,1,1}

typedef enum {
  l2cmsOBJECT_NONE,
  l2cmsOBJECT_PROFILE = 220,
  l2cmsOBJECT_TRANSFORM
} l2cmOBJECT_e;

oyMessage_f l2cms_msg = oyMessageFunc;

void l2cmsErrorHandlerFunction       ( cmsContext          ContextID,
                                       cmsUInt32Number     ErrorCode,
                                       const char        * ErrorText );
int            l2cmsCMMMessageFuncSet( oyMessage_f         l2cms_msg_func );
int                l2cmsCMMinit      ( );
int                l2cmsCMMreset     ( );
int                l2cmsCMMapiInit   ( ) { return 0; }
int                l2cmsCMMapiReset  ( ) { return 0; }


/** @struct l2cmsProfileWrap_s
 *  @brief l2cms wrapper for profile data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
typedef struct l2cmsProfileWrap_s_ {
  l2cmOBJECT_e type;                   /**< shall be l2cmsOBJECT_PROFILE */
  oyStruct_Copy_f      copy;           /**< @brief Copy function */
  oyStruct_Release_f   release;        /**< @brief Release function */
  oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

  size_t       size;                   /**< size of block */
  oyPointer    block;                  /**< profile data */
  oyPointer    l2cms;                  /**< cmsHPROFILE struct */
  icColorSpaceSignature sig;           /**< ICC profile signature */
  oyProfile_s *dbg_profile;            /**< only for debugging */
  int          id;                     /**< oy ID */
} l2cmsProfileWrap_s;

/** @struct  l2cmsTransformWrap_s
 *  @brief   l2cms wrapper for transform data struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
typedef struct l2cmsTransformWrap_s_ {
  l2cmOBJECT_e type;                   /**< shall be l2cmsOBJECT_TRANSFORM */
  oyPointer    l2cms;                  /**< cmsHPROFILE struct */
  icColorSpaceSignature sig_in;        /**< ICC profile signature */
  icColorSpaceSignature sig_out;       /**< ICC profile signature */
  oyPixel_t    oy_pixel_layout_in;
  oyPixel_t    oy_pixel_layout_out;
} l2cmsTransformWrap_s;


l2cmsTransformWrap_s * l2cmsTransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature color_in,
                                       icColorSpaceSignature color_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyPointer_s       * oy );
int      l2cmsCMMTransform_GetWrap_  ( oyPointer_s       * cmm_ptr,
                                       l2cmsTransformWrap_s ** s );
int l2cmsCMMDeleteTransformWrap      ( l2cmsTransformWrap_s ** wrap);

l2cmsProfileWrap_s * l2cmsCMMProfile_GetWrap_(
                                       oyPointer_s       * cmm_ptr );
int l2cmsCMMProfileWrap_Release      ( l2cmsProfileWrap_s**p );

int        l2cmsCMMCheckPointer      ( oyPointer_s       * cmm_ptr,
                                       const char        * resource );
int        oyPixelToLcm2PixelLayout_ ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature color_space );
char * l2cmsImage_GetText            ( oyImage_s         * image,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );


char * l2cmsFilterNode_GetText       ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );
extern char l2cms_extra_options[];
char * l2cmsFlagsToText              ( int                 flags );
cmsHPROFILE  l2cmsGamutCheckAbstract ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof,
                                       uint32_t            icc_profile_flags );
oyPointer  l2cmsCMMColorConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       oyOptions_s       * opts,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
oyOptions_s* l2cmsFilter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
cmsHPROFILE  l2cmsAddProfile         ( oyProfile_s       * p );
l2cmsProfileWrap_s * l2cmsAddProofProfile
                                     ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof,
                                       uint32_t            icc_profile_flags );
oyPointer l2cmsFilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int  l2cmsModuleData_Convert         ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node );
int      l2cmsFilterPlug_CmmIccRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
const char * l2cmsInfoGetText        ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );




/* --- implementations --- */
/* explicitely load liblcms functions, to avoid conflicts */
static int l2cms_initialised = 0; /* 0 - need init; 1 - successful init; -1 - error on init */
static void * l2cms_handle = NULL;

static void (*l2cmsSetLogErrorHandler)(cmsLogErrorHandlerFunction Fn) = NULL;
static void (*l2cmsSetLogErrorHandlerTHR)(     cmsContext ContextID,
                                               cmsLogErrorHandlerFunction Fn) = NULL;
static cmsColorSpaceSignature (*l2cmsGetColorSpace)(cmsHPROFILE hProfile) = NULL;
static cmsColorSpaceSignature (*l2cmsGetPCS)(cmsHPROFILE hProfile) = NULL;
static cmsProfileClassSignature (*l2cmsGetDeviceClass)(cmsHPROFILE hProfile) = NULL;
static cmsUInt32Number (*l2cmsGetProfileInfoASCII)(cmsHPROFILE hProfile, cmsInfoType Info,
                                                            const char LanguageCode[3], const char CountryCode[3],
                                                            char* Buffer, cmsUInt32Number BufferSize) = NULL;
static int (*l2_cmsLCMScolorSpace)(cmsColorSpaceSignature ProfileSpace) = NULL;
static cmsUInt32Number (*l2cmsChannelsOf)(cmsColorSpaceSignature ColorSpace) = NULL;
static cmsBool         (*l2cmsIsTag)(cmsHPROFILE hProfile, cmsTagSignature sig) = NULL;
static cmsHTRANSFORM (*l2cmsCreateTransform)(cmsHPROFILE Input,
                                               cmsUInt32Number InputFormat,
                                               cmsHPROFILE Output,
                                               cmsUInt32Number OutputFormat,
                                               cmsUInt32Number Intent,
                                               cmsUInt32Number dwFlags) = NULL;
static cmsHTRANSFORM   (*l2cmsCreateTransformTHR)(cmsContext ContextID,
                                                  cmsHPROFILE Input,
                                                  cmsUInt32Number InputFormat,
                                                  cmsHPROFILE Output,
                                                  cmsUInt32Number OutputFormat,
                                                  cmsUInt32Number Intent,
                                                  cmsUInt32Number dwFlags) = NULL;
static cmsHTRANSFORM (*l2cmsCreateProofingTransform)(cmsHPROFILE Input,
                                               cmsUInt32Number InputFormat,
                                               cmsHPROFILE Output,
                                               cmsUInt32Number OutputFormat,
                                               cmsHPROFILE Proofing,
                                               cmsUInt32Number Intent,
                                               cmsUInt32Number ProofingIntent,
                                               cmsUInt32Number dwFlags) = NULL;
static cmsHTRANSFORM (*l2cmsCreateProofingTransformTHR)(cmsContext ContextID,
                                                  cmsHPROFILE Input,
                                                  cmsUInt32Number InputFormat,
                                                  cmsHPROFILE Output,
                                                  cmsUInt32Number OutputFormat,
                                                  cmsHPROFILE Proofing,
                                                  cmsUInt32Number Intent,
                                                  cmsUInt32Number ProofingIntent,
                                                  cmsUInt32Number dwFlags) = NULL;
static cmsHTRANSFORM (*l2cmsCreateMultiprofileTransform)(cmsHPROFILE hProfiles[],
                                                                cmsUInt32Number nProfiles,
                                                                cmsUInt32Number InputFormat,
                                                                cmsUInt32Number OutputFormat,
                                                                cmsUInt32Number Intent,
                                                                cmsUInt32Number dwFlags) = NULL;
static cmsHTRANSFORM (*l2cmsCreateExtendedTransform)(cmsContext ContextID,
                                                   cmsUInt32Number nProfiles, cmsHPROFILE hProfiles[],
                                                   cmsBool  BPC[],
                                                   cmsUInt32Number Intents[],
                                                   cmsFloat64Number AdaptationStates[],
                                                   cmsHPROFILE hGamutProfile,
                                                   cmsUInt32Number nGamutPCSposition,
                                                   cmsUInt32Number InputFormat,
                                                   cmsUInt32Number OutputFormat,
                                                   cmsUInt32Number dwFlags) = NULL;
static void (*l2cmsDeleteTransform)(cmsHTRANSFORM hTransform) = NULL;
static void (*l2cmsDoTransform)(cmsHTRANSFORM Transform,
                                                 const void * InputBuffer,
                                                 void * OutputBuffer,
                                                 cmsUInt32Number Size) = NULL;
static cmsHPROFILE (*l2cmsTransform2DeviceLink)(cmsHTRANSFORM hTransform, cmsFloat64Number Version, cmsUInt32Number dwFlags) = NULL;
static cmsBool (*l2cmsSaveProfileToMem)(cmsHPROFILE hProfile, void *MemPtr, 
                                                                cmsUInt32Number* BytesNeeded) = NULL;
static cmsHPROFILE (*l2cmsOpenProfileFromFile)(const char *ICCProfile, const char *sAccess) = NULL;
static cmsHPROFILE (*l2cmsOpenProfileFromMemTHR)(cmsContext ContextID, const void * MemPtr, cmsUInt32Number dwSize) = NULL;
static cmsHPROFILE (*l2cmsOpenProfileFromFileTHR)(cmsContext ContextID, const char *ICCProfile, const char *sAccess) = NULL;
static cmsBool     (*l2cmsSaveProfileToFile)(cmsHPROFILE hProfile, const char* FileName) = NULL;
static cmsBool (*l2cmsCloseProfile)(cmsHPROFILE hProfile) = NULL;
static cmsHPROFILE (*l2cmsCreateProfilePlaceholder)(cmsContext ContextID) = NULL;
static cmsHPROFILE (*l2cmsCreateLab4ProfileTHR)(cmsContext ContextID, const cmsCIExyY* WhitePoint) = NULL;
static cmsHPROFILE (*l2cmsCreateLab4Profile)(const cmsCIExyY* WhitePoint) = NULL;
static cmsHPROFILE (*l2cmsCreateXYZProfile)() = NULL;
static cmsHPROFILE (*l2cmsCreate_sRGBProfile)() = NULL;
static void (*l2cmsSetProfileVersion)(cmsHPROFILE hProfile, cmsFloat64Number Version) = NULL;
static void (*l2cmsSetDeviceClass)(cmsHPROFILE hProfile, cmsProfileClassSignature sig) = NULL;
static void (*l2cmsSetColorSpace)(cmsHPROFILE hProfile, cmsColorSpaceSignature sig) = NULL;
static void (*l2cmsSetPCS)(cmsHPROFILE hProfile, cmsColorSpaceSignature pcs) = NULL;
static cmsToneCurve* (*l2cmsBuildGamma)(cmsContext ContextID, cmsFloat64Number Gamma) = NULL;
static cmsToneCurve*(*l2cmsBuildSegmentedToneCurve)(cmsContext ContextID, cmsUInt32Number nSegments, const cmsCurveSegment Segments[]) = NULL;
static cmsToneCurve*(*l2cmsBuildParametricToneCurve)(cmsContext ContextID, cmsInt32Number Type, const cmsFloat64Number Parameters[]) = NULL;
static void (*l2cmsFreeToneCurve)(cmsToneCurve* Curve) = NULL;
static cmsHANDLE         (*l2cmsCIECAM02Init)               (cmsContext ContextID, const cmsViewingConditions* pVC) = NULL;
static void              (*l2cmsCIECAM02Done)               (cmsHANDLE hModel) = NULL;
static void              (*l2cmsCIECAM02Forward)            (cmsHANDLE hModel, const cmsCIEXYZ* pIn, cmsJCh* pOut) = NULL;
static void              (*l2cmsCIECAM02Reverse)            (cmsHANDLE hModel, const cmsJCh* pIn,    cmsCIEXYZ* pOut) = NULL;
static cmsPipeline*      (*l2cmsPipelineAlloc)              (cmsContext ContextID, cmsUInt32Number InputChannels, cmsUInt32Number OutputChannels) = NULL;
static int               (*l2cmsPipelineInsertStage)        (cmsPipeline* lut, cmsStageLoc loc, cmsStage* mpe) = NULL;
static void              (*l2cmsPipelineFree)               (cmsPipeline* lut) = NULL;
static cmsStage*         (*l2cmsPipelineGetPtrToFirstStage) (const cmsPipeline* lut ) = NULL;
static cmsStageSignature (*l2cmsStageType) (const cmsStage* stage) = NULL;
static cmsStage*         (*l2cmsStageNext) (const cmsStage* next ) = NULL;
static cmsUInt32Number   (*l2cmsStageInputChannels)         (const cmsStage* stage) = NULL;
static cmsUInt32Number   (*l2cmsStageOutputChannels)        (const cmsStage* stage) = NULL;
static cmsStage*(*l2cmsStageAllocCLut16bit)(cmsContext ContextID, cmsUInt32Number nGridPoints, cmsUInt32Number inputChan, cmsUInt32Number outputChan, const cmsUInt16Number* Table) = NULL;
static cmsStage*(*l2cmsStageAllocCLutFloat)(cmsContext ContextID, cmsUInt32Number nGridPoints, cmsUInt32Number inputChan, cmsUInt32Number outputChan, const cmsFloat32Number* Table) = NULL;
static cmsStage*         (*l2cmsStageAllocMatrix)(cmsContext ContextID, cmsUInt32Number Rows, cmsUInt32Number Cols, const cmsFloat64Number* Matrix, const cmsFloat64Number* Offset) = NULL;
static cmsBool (*l2cmsStageSampleCLut16bit)(cmsStage* mpe,    cmsSAMPLER16 Sampler, void* Cargo, cmsUInt32Number dwFlags) = NULL;
static cmsBool (*l2cmsStageSampleCLutFloat)(cmsStage* mpe, cmsSAMPLERFLOAT Sampler, void* Cargo, cmsUInt32Number dwFlags) = NULL;
static cmsStage*(*l2cmsStageAllocToneCurves)(cmsContext ContextID, cmsUInt32Number nChannels, cmsToneCurve* const Curves[]) = NULL;
static void*   (*l2cmsReadTag)(cmsHPROFILE hProfile, cmsTagSignature sig) = NULL;
static cmsBool (*l2cmsWriteTag)(cmsHPROFILE hProfile, cmsTagSignature sig, const void* data) = NULL;
static cmsMLU*(*l2cmsMLUalloc)(cmsContext ContextID, cmsUInt32Number nItems) = NULL;
static cmsBool (*l2cmsMLUsetASCII)(cmsMLU* mlu,
                                                  const char LanguageCode[3], const char CountryCode[3],
                                                  const char* ASCIIString) = NULL;
static cmsBool                 (*l2cmsMLUsetWide)(cmsMLU* mlu,
                                                  const char LanguageCode[3], const char CountryCode[3],
                                                  const wchar_t* WideString) = NULL;
static void (*l2cmsMLUfree)(cmsMLU* mlu) = NULL;
static cmsHANDLE      (*l2cmsDictAlloc)(cmsContext ContextID);
static void           (*l2cmsDictFree)(cmsHANDLE hDict);
static cmsHANDLE      (*l2cmsDictDup)(cmsHANDLE hDict);

static cmsBool        (*l2cmsDictAddEntry)(cmsHANDLE hDict, const wchar_t* Name, const wchar_t* Value, const cmsMLU *DisplayName, const cmsMLU *DisplayValue);
static const cmsDICTentry* (*l2cmsDictGetEntryList)(cmsHANDLE hDict);
static const cmsDICTentry* (*l2cmsDictNextEntry)(const cmsDICTentry* e);
static cmsHPROFILE (*l2cmsCreateRGBProfile)(const cmsCIExyY* WhitePoint,
                                        const cmsCIExyYTRIPLE* Primaries,
                                        cmsToneCurve* const TransferFunction[3]) = NULL;
static cmsHPROFILE (*l2cmsCreateLinearizationDeviceLink)(cmsColorSpaceSignature ColorSpace,
                                                                cmsToneCurve* const TransferFunctions[]) = NULL;
static void (*l2cmsLabEncoded2Float)(cmsCIELab* Lab, const cmsUInt16Number wLab[3]) = NULL;
static void (*l2cmsFloat2LabEncoded)(cmsUInt16Number wLab[3], const cmsCIELab* Lab) = NULL;
static const cmsCIEXYZ*  (*l2cmsD50_XYZ)(void);
static const cmsCIExyY*  (*l2cmsD50_xyY)(void);
static cmsBool           (*l2cmsWhitePointFromTemp)(cmsCIExyY* WhitePoint, cmsFloat64Number  TempK) = NULL;
static cmsBool           (*l2cmsAdaptToIlluminant)(cmsCIEXYZ* Result, const cmsCIEXYZ* SourceWhitePt,
                                                                           const cmsCIEXYZ* Illuminant,
                                                                           const cmsCIEXYZ* Value) = NULL;
static void              (*l2cmsxyY2XYZ)(cmsCIEXYZ* Dest, const cmsCIExyY* Source) = NULL;
static void              (*l2cmsXYZ2Lab)(const cmsCIEXYZ* WhitePoint, cmsCIELab* Lab, const cmsCIEXYZ* xyz) = NULL;
static void              (*l2cmsLab2XYZ)(const cmsCIEXYZ* WhitePoint, cmsCIEXYZ* xyz, const cmsCIELab* Lab) = NULL;
static cmsFloat64Number (*l2cmsDeltaE)(const cmsCIELab* Lab1, const cmsCIELab* Lab2) = NULL;
static void (*l2cmsGetAlarmCodes)(cmsUInt16Number NewAlarm[cmsMAXCHANNELS]) = NULL;
static cmsContext (*l2cmsCreateContext)(void* Plugin, void* UserData) = NULL;
static void              (*l2cmsDeleteContext)(cmsContext ContexID) = NULL;
static cmsContext dummyCreateContext(void* Plugin OY_UNUSED, void* UserData OY_UNUSED) {return NULL;}
static void       dummyDeleteContext(cmsContext ContexID OY_UNUSED) {};
static void* (*l2cmsGetContextUserData)(cmsContext ContextID) = NULL;
static void* dummyGetContextUserData(cmsContext ContextID OY_UNUSED) {return NULL;}
static cmsContext (*l2cmsGetProfileContextID)(cmsHPROFILE hProfile) = NULL;
static cmsContext (*l2cmsGetTransformContextID)(cmsHPROFILE hProfile) = NULL;
static int dummyGetEncodedCMMversion() {return LCMS_VERSION;}
static int (*l2cmsGetEncodedCMMversion)(void) = dummyGetEncodedCMMversion;
static cmsHANDLE         (*l2cmsIT8LoadFromMem)(cmsContext ContextID, const void *Ptr, cmsUInt32Number len);
static cmsUInt32Number   (*l2cmsIT8EnumProperties)(cmsHANDLE hIT8, char ***PropertyNames);
static cmsUInt32Number   (*l2cmsIT8EnumPropertyMulti)(cmsHANDLE hIT8, const char* cProp, const char ***SubpropertyNames);

#if !defined(COMPILE_STATIC)
#define REGISTER_FUNC( func, fallback_func ) \
  if(init) \
  { \
    l2##func = dlsym(l2cms_handle, #func ); \
               if(!l2##func) \
               { \
                 oyMSG_e type = oyMSG_ERROR; \
                 if(#fallback_func != NULL) \
                 { \
                   l2##func = fallback_func; \
                   type = oyMSG_WARN; \
                 } else \
                 { \
                   error = 1; \
                 } \
                 if(!error) \
                   error = -1; \
                 l2cms_msg( type,0, OY_DBG_FORMAT_" " \
                                      "dlsym failed: %s", \
                                      OY_DBG_ARGS_, dlerror() ); \
               } \
  } \
  else \
  { \
    l2##func = NULL; \
  }
#else
#define REGISTER_FUNC( func, fallback_func ) \
  if(init) \
  { \
    l2##func = func; \
               if(!l2##func) \
               { \
                 oyMSG_e type = oyMSG_ERROR; \
                 if(#fallback_func != NULL) \
                 { \
                   l2##func = fallback_func; \
                   type = oyMSG_WARN; \
                 } else \
                 { \
                   error = 1; \
                 } \
                 if(!error) \
                   error = -1; \
                 l2cms_msg( type,0, OY_DBG_FORMAT_" " \
                                      "dlsym failed: %s", \
                                      OY_DBG_ARGS_, dlerror() ); \
               } \
               l2cms_handle = 0; \
  } \
  else \
  { \
    l2##func = NULL; \
  }
#define dlerror() l2cms_handle = 0
#endif

int lcm2registerFuncs( int init, const char * fn OY_UNUSED )
{
  int error = 0;
      REGISTER_FUNC( cmsSetLogErrorHandler, NULL );
#if LCMS_VERSION >= 2060
      REGISTER_FUNC( cmsSetLogErrorHandlerTHR, NULL );
#endif
      REGISTER_FUNC( cmsGetColorSpace, NULL );
      REGISTER_FUNC( cmsGetPCS, NULL );
      REGISTER_FUNC( cmsGetDeviceClass, NULL );
      REGISTER_FUNC( cmsGetProfileInfoASCII, NULL );
      REGISTER_FUNC( _cmsLCMScolorSpace, NULL );
      REGISTER_FUNC( cmsChannelsOf, NULL );
      REGISTER_FUNC( cmsIsTag, NULL );
      REGISTER_FUNC( cmsCreateTransform, NULL );
      REGISTER_FUNC( cmsCreateTransformTHR, NULL );
      REGISTER_FUNC( cmsCreateProofingTransform, NULL );
      REGISTER_FUNC( cmsCreateProofingTransformTHR, NULL );
      REGISTER_FUNC( cmsCreateMultiprofileTransform, NULL );
      REGISTER_FUNC( cmsCreateExtendedTransform, NULL );
      REGISTER_FUNC( cmsDeleteTransform, NULL );
      REGISTER_FUNC( cmsDoTransform, NULL );
      REGISTER_FUNC( cmsOpenProfileFromFile, NULL );
      REGISTER_FUNC( cmsSaveProfileToFile, NULL );
      REGISTER_FUNC( cmsTransform2DeviceLink, NULL );
      REGISTER_FUNC( cmsSaveProfileToMem, NULL );
      REGISTER_FUNC( cmsOpenProfileFromMemTHR, NULL );
#if LCMS_VERSION >= 2060
      REGISTER_FUNC( cmsOpenProfileFromFileTHR, NULL );
#endif
      REGISTER_FUNC( cmsCloseProfile, NULL );
      REGISTER_FUNC( cmsCreateProfilePlaceholder, NULL );
      REGISTER_FUNC( cmsSetProfileVersion, NULL );
      REGISTER_FUNC( cmsCreateLab4ProfileTHR, NULL );
      REGISTER_FUNC( cmsCreateLab4Profile, NULL );
      REGISTER_FUNC( cmsCreateXYZProfile, NULL );
      REGISTER_FUNC( cmsCreate_sRGBProfile, NULL );
      REGISTER_FUNC( cmsCreateRGBProfile, NULL );
      REGISTER_FUNC( cmsCreateLinearizationDeviceLink, NULL );
      REGISTER_FUNC( cmsSetDeviceClass, NULL );
      REGISTER_FUNC( cmsSetColorSpace, NULL );
      REGISTER_FUNC( cmsSetPCS, NULL );
      REGISTER_FUNC( cmsBuildGamma, NULL );
      REGISTER_FUNC( cmsBuildSegmentedToneCurve, NULL );
      REGISTER_FUNC( cmsBuildParametricToneCurve, NULL );
      REGISTER_FUNC( cmsFreeToneCurve, NULL );
      REGISTER_FUNC( cmsCIECAM02Init, NULL );
      REGISTER_FUNC( cmsCIECAM02Done, NULL );
      REGISTER_FUNC( cmsCIECAM02Forward, NULL );
      REGISTER_FUNC( cmsCIECAM02Reverse, NULL );
      REGISTER_FUNC( cmsPipelineAlloc, NULL );
      REGISTER_FUNC( cmsPipelineFree, NULL );
      REGISTER_FUNC( cmsPipelineInsertStage, NULL );
      REGISTER_FUNC( cmsPipelineGetPtrToFirstStage, NULL );
      REGISTER_FUNC( cmsStageType, NULL );
      REGISTER_FUNC( cmsStageNext, NULL );
      REGISTER_FUNC( cmsStageInputChannels, NULL );
      REGISTER_FUNC( cmsStageOutputChannels, NULL );
      REGISTER_FUNC( cmsStageAllocCLut16bit, NULL );
      REGISTER_FUNC( cmsStageAllocCLutFloat, NULL );
      REGISTER_FUNC( cmsStageAllocMatrix, NULL );
      REGISTER_FUNC( cmsStageSampleCLut16bit, NULL );
      REGISTER_FUNC( cmsStageSampleCLutFloat, NULL );
      REGISTER_FUNC( cmsStageAllocToneCurves, NULL );
      REGISTER_FUNC( cmsReadTag, NULL );
      REGISTER_FUNC( cmsWriteTag, NULL );
      REGISTER_FUNC( cmsMLUalloc, NULL );
      REGISTER_FUNC( cmsMLUsetASCII, NULL );
      REGISTER_FUNC( cmsMLUsetWide, NULL );
      REGISTER_FUNC( cmsMLUfree, NULL );
      REGISTER_FUNC( cmsDictAlloc, NULL );
      REGISTER_FUNC( cmsDictFree, NULL );
      REGISTER_FUNC( cmsDictDup, NULL );
      REGISTER_FUNC( cmsDictAddEntry, NULL );
      REGISTER_FUNC( cmsDictGetEntryList, NULL );
      REGISTER_FUNC( cmsDictNextEntry, NULL );
      REGISTER_FUNC( cmsLabEncoded2Float, NULL );
      REGISTER_FUNC( cmsFloat2LabEncoded, NULL );
      REGISTER_FUNC( cmsD50_XYZ, NULL );
      REGISTER_FUNC( cmsD50_xyY, NULL );
      REGISTER_FUNC( cmsWhitePointFromTemp, NULL );
      REGISTER_FUNC( cmsAdaptToIlluminant, NULL );
      REGISTER_FUNC( cmsxyY2XYZ, NULL );
      REGISTER_FUNC( cmsXYZ2Lab, NULL );
      REGISTER_FUNC( cmsLab2XYZ, NULL );
      REGISTER_FUNC( cmsDeltaE, NULL );
      REGISTER_FUNC( cmsGetAlarmCodes, NULL );
#if LCMS_VERSION >= 2060
      REGISTER_FUNC( cmsCreateContext, dummyCreateContext ); /* available since lcms 2.6 */
      REGISTER_FUNC( cmsDeleteContext, dummyDeleteContext ); /* available since lcms 2.6 */
      REGISTER_FUNC( cmsGetContextUserData, dummyGetContextUserData ); /* available since lcms 2.6 */
#else
      l2cmsCreateContext = dummyCreateContext;
      l2cmsDeleteContext = dummyDeleteContext;
      l2cmsGetContextUserData = dummyGetContextUserData;
#endif
      REGISTER_FUNC( cmsGetProfileContextID, NULL );
      REGISTER_FUNC( cmsGetTransformContextID, NULL );
      if(init)
      {
        if(l2cmsSetLogErrorHandler)
          l2cmsSetLogErrorHandler( l2cmsErrorHandlerFunction );
        else
          l2cms_msg( oyMSG_WARN, (oyStruct_s*)NULL,
                    OY_DBG_FORMAT_"can not set error handler %d %d",
                    OY_DBG_ARGS_, l2cmsGetEncodedCMMversion, LCMS_VERSION );
        if(l2cmsGetEncodedCMMversion && l2cmsGetEncodedCMMversion() != LCMS_VERSION)
          l2cms_msg( oyMSG_WARN, (oyStruct_s*)NULL,
                    OY_DBG_FORMAT_" compile and run time version differ %d %d",
                    OY_DBG_ARGS_, l2cmsGetEncodedCMMversion, LCMS_VERSION );
      }
#if LCMS_VERSION >= 2080
      REGISTER_FUNC( cmsGetEncodedCMMversion, dummyGetEncodedCMMversion );
#endif
      REGISTER_FUNC( cmsIT8LoadFromMem, NULL );
      REGISTER_FUNC( cmsIT8EnumProperties, NULL );
      REGISTER_FUNC( cmsIT8EnumPropertyMulti, NULL );
          
  return error;
}

static char * lcm2_message_text_ = NULL;
static const char * lcm2StaticMessageFunc (
                                       oyPointer           obj,
                                       oyNAME_e            type OY_UNUSED,
                                       int                 flags OY_UNUSED )
{
  typedef struct {
    unsigned type;
  } data_struct;
  data_struct * o = (data_struct *) obj;

  if(obj)
  {
    if(o->type == l2cmsOBJECT_PROFILE)
    {
      l2cmsProfileWrap_s * s = (l2cmsProfileWrap_s*) obj;
      if(!lcm2_message_text_)
        lcm2_message_text_ = calloc(sizeof(char), 512 );

      if(type == oyNAME_NICK)
        sprintf( lcm2_message_text_, "l2cmsProfileWrap_s" );
      else if(type == oyNAME_JSON+2)
        sprintf( lcm2_message_text_, "%d", s->id );
      else
        sprintf( lcm2_message_text_, "l2cms oyProfile_s[%d]", s->id );
    }
    else
    if(o->type == l2cmsOBJECT_TRANSFORM)
      return "l2cmsTransformWrap_s";
  }
  else
    sprintf( lcm2_message_text_, "unknown" );
  return lcm2_message_text_;
}
static void l2cmsProfileWrap_StaticFree_           ( void )
{
  if(lcm2_message_text_)
  {
    free(lcm2_message_text_);
    lcm2_message_text_ = NULL;
  }
  if(oy_debug)
    fprintf(stderr, "%s() freeing static \"%s\" memory\n", "l2cmsProfileWrap_StaticFree_", "l2cmsProfileWrap_s" );
}

/** Function l2cmsCMMinit
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/02/27
 *  @since   2007/12/11 (Oyranos: 0.1.8)
 */
int                l2cmsCMMinit       ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  int init = 1;
  if(!l2cms_initialised)
  {
    char * fn = oyLibNameCreate_( "lcms2", 2 );
#if !defined(COMPILE_STATIC)
    l2cms_handle = dlopen(fn, RTLD_LAZY);

    if(!l2cms_handle)
    {
      l2cms_msg( oyMSG_ERROR,0, OY_DBG_FORMAT_" "
               "init \"%s\" failed: %s",
                OY_DBG_ARGS_, fn, dlerror() );
      error = 1;
      l2cms_initialised = -1;
    } else
#endif
    {
      error = lcm2registerFuncs( init, fn );
          
#if !defined(COMPILE_STATIC)
      if(error > 0)
        l2cms_initialised = -1;
      else
#endif
        l2cms_initialised = 1;

      if(error != 0)
        l2cms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_" "
               "init \"%s\" issue(s): v%d",
                OY_DBG_ARGS_, fn, l2cmsGetEncodedCMMversion() );

      {
        oyStruct_RegisterStaticMessageFunc( l2cmsOBJECT_PROFILE,
                                      lcm2StaticMessageFunc, l2cmsProfileWrap_StaticFree_ );
        oyStruct_RegisterStaticMessageFunc( l2cmsOBJECT_TRANSFORM,
                                      lcm2StaticMessageFunc, NULL );
      }
    }
    oyFree_m_( fn );

  } else if(l2cms_initialised == -1)
    error = 1;
  return error;
}
/** Function l2cmsCMMreset
 *  @brief   API requirement
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/02
 *  @since   2019/09/02 (Oyranos: 0.9.7)
 */
int                l2cmsCMMreset      ( oyStruct_s        * filter OY_UNUSED )
{
  int error = 0;
  int init = 0;

  if(l2cms_initialised)
  {
    error = lcm2registerFuncs( init, NULL );

    if(l2cms_handle) \
      dlclose(l2cms_handle); \
    l2cms_handle = NULL; \

    l2cms_initialised = 0;
  }
  return error;
}


#define cmsSetLogErrorHandler l2cmsSetLogErrorHandler
#define cmsSetLogErrorHandlerTHR l2cmsSetLogErrorHandlerTHR
#define cmsGetColorSpace l2cmsGetColorSpace
#define cmsGetPCS l2cmsGetPCS
#define cmsGetDeviceClass l2cmsGetDeviceClass
#define cmsGetProfileInfoASCII l2cmsGetProfileInfoASCII
#define _cmsLCMScolorSpace l2_cmsLCMScolorSpace
#define cmsChannelsOf l2cmsChannelsOf
#define cmsIsTag l2cmsIsTag
#define cmsCreateTransform l2cmsCreateTransform
#define cmsCreateTransformTHR l2cmsCreateTransformTHR
#define cmsCreateProofingTransform l2cmsCreateProofingTransform
#define cmsCreateProofingTransformTHR l2cmsCreateProofingTransformTHR
#define cmsCreateMultiprofileTransform l2cmsCreateMultiprofileTransform
#define cmsCreateExtendedTransform l2cmsCreateExtendedTransform
#define cmsDeleteTransform l2cmsDeleteTransform
#define cmsDoTransform l2cmsDoTransform
#define cmsOpenProfileFromFile l2cmsOpenProfileFromFile
#define cmsSaveProfileToFile l2cmsSaveProfileToFile
#define cmsTransform2DeviceLink l2cmsTransform2DeviceLink
#define cmsSaveProfileToMem l2cmsSaveProfileToMem
#define cmsOpenProfileFromMemTHR l2cmsOpenProfileFromMemTHR
#define cmsOpenProfileFromFileTHR l2cmsOpenProfileFromFileTHR
#define cmsCloseProfile l2cmsCloseProfile
#define cmsCreateProfilePlaceholder l2cmsCreateProfilePlaceholder
#define cmsSetProfileVersion l2cmsSetProfileVersion
#define cmsCreateLab4ProfileTHR l2cmsCreateLab4ProfileTHR
#define cmsCreateLab4Profile l2cmsCreateLab4Profile
#define cmsCreateXYZProfile l2cmsCreateXYZProfile
#define cmsCreate_sRGBProfile l2cmsCreate_sRGBProfile
#define cmsCreateRGBProfile l2cmsCreateRGBProfile
#define cmsCreateLinearizationDeviceLink l2cmsCreateLinearizationDeviceLink
#define cmsSetDeviceClass l2cmsSetDeviceClass
#define cmsSetColorSpace l2cmsSetColorSpace
#define cmsSetPCS l2cmsSetPCS
#define cmsBuildGamma l2cmsBuildGamma
#define cmsBuildSegmentedToneCurve l2cmsBuildSegmentedToneCurve
#define cmsBuildParametricToneCurve l2cmsBuildParametricToneCurve
#define cmsFreeToneCurve l2cmsFreeToneCurve
#define cmsCIECAM02Init l2cmsCIECAM02Init
#define cmsCIECAM02Done l2cmsCIECAM02Done
#define cmsCIECAM02Forward l2cmsCIECAM02Forward
#define cmsCIECAM02Reverse l2cmsCIECAM02Reverse
#define cmsPipelineAlloc l2cmsPipelineAlloc
#define cmsPipelineFree l2cmsPipelineFree
#define cmsPipelineInsertStage l2cmsPipelineInsertStage
#define cmsPipelineGetPtrToFirstStage l2cmsPipelineGetPtrToFirstStage
#define cmsStageType l2cmsStageType
#define cmsStageNext l2cmsStageNext
#define cmsStageInputChannels l2cmsStageInputChannels
#define cmsStageOutputChannels l2cmsStageOutputChannels
#define cmsStageAllocCLut16bit l2cmsStageAllocCLut16bit
#define cmsStageAllocCLutFloat l2cmsStageAllocCLutFloat
#define cmsStageAllocMatrix    l2cmsStageAllocMatrix
#define cmsStageSampleCLut16bit l2cmsStageSampleCLut16bit
#define cmsStageSampleCLutFloat l2cmsStageSampleCLutFloat
#define cmsStageAllocToneCurves l2cmsStageAllocToneCurves
#define cmsReadTag l2cmsReadTag
#define cmsWriteTag l2cmsWriteTag
#define cmsMLUalloc l2cmsMLUalloc
#define cmsMLUsetASCII l2cmsMLUsetASCII
#define cmsMLUsetWide l2cmsMLUsetWide
#define cmsMLUfree l2cmsMLUfree
#define cmsDictAlloc l2cmsDictAlloc
#define cmsDictFree l2cmsDictFree
#define cmsDictDup l2cmsDictDup
#define cmsDictAddEntry l2cmsDictAddEntry
#define cmsDictGetEntryList l2cmsDictGetEntryList
#define cmsDictNextEntry l2cmsDictNextEntry
#define cmsLabEncoded2Float l2cmsLabEncoded2Float
#define cmsFloat2LabEncoded l2cmsFloat2LabEncoded
#define cmsD50_XYZ l2cmsD50_XYZ
#define cmsD50_xyY l2cmsD50_xyY
#define cmsWhitePointFromTemp l2cmsWhitePointFromTemp
#define cmsAdaptToIlluminant l2cmsAdaptToIlluminant
#define cmsxyY2XYZ l2cmsxyY2XYZ
#define cmsXYZ2Lab l2cmsXYZ2Lab
#define cmsLab2XYZ l2cmsLab2XYZ
#define cmsDeltaE l2cmsDeltaE
#define cmsGetAlarmCodes l2cmsGetAlarmCodes
#define cmsCreateContext l2cmsCreateContext
#define cmsDeleteContext l2cmsDeleteContext
#define cmsGetContextUserData l2cmsGetContextUserData
#define cmsGetProfileContextID l2cmsGetProfileContextID
#define cmsGetTransformContextID l2cmsGetTransformContextID
#define cmsGetEncodedCMMversion l2cmsGetEncodedCMMversion
#define cmsIT8LoadFromMem l2cmsIT8LoadFromMem
#define cmsIT8EnumProperties l2cmsIT8EnumProperties
#define cmsIT8EnumPropertyMulti l2cmsIT8EnumPropertyMulti

#include "lcm2_profiler.c"

/** Function l2cmsCMMProfile_GetWrap_
 *  @brief   convert to l2cms profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/10
 *  @since   2007/12/10 (Oyranos: 0.1.8)
 */
l2cmsProfileWrap_s * l2cmsCMMProfile_GetWrap_( oyPointer_s* cmm_ptr )
{
  l2cmsProfileWrap_s * s = NULL;

  if(cmm_ptr && !l2cmsCMMCheckPointer( cmm_ptr, l2cmsPROFILE ) &&
     oyPointer_GetPointer(cmm_ptr))
    s = (l2cmsProfileWrap_s*) oyPointer_GetPointer(cmm_ptr);

  if(s && s->type != l2cmsOBJECT_PROFILE)
    s = NULL;

  if(s && oy_debug >= 2)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)cmm_ptr,
              OY_DBG_FORMAT_" profile size: %d %s cmm_ptr: %d",
              OY_DBG_ARGS_, s->size, s->dbg_profile?oyNoEmptyString_m_(oyProfile_GetFileName( s->dbg_profile,-1 )):"????",
              oyStruct_GetId((oyStruct_s*)cmm_ptr) );
  }

  if(s)
    oyObject_Ref(s->oy_);

  return s;
}

/** Function l2cmsCMMTransform_GetWrap_
 *  @brief   convert to l2cms transform wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2009/05/28
 */
int      l2cmsCMMTransform_GetWrap_   ( oyPointer_s       * cmm_ptr,
                                       l2cmsTransformWrap_s ** s )
{
  if(cmm_ptr && !l2cmsCMMCheckPointer( cmm_ptr, l2cmsTRANSFORM ) &&
     oyPointer_GetPointer(cmm_ptr))
    *s = (l2cmsTransformWrap_s*) oyPointer_GetPointer(cmm_ptr);

  if(*s && ((*s)->type != l2cmsOBJECT_TRANSFORM || !(*s)->l2cms))
  {
    *s = 0;
    return 1;
  }

  return 0;
}

/** Function l2cmsCMMProfileWrap_Release
 *  @brief   release a l2cms profile wrapper struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/12/20
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
int l2cmsCMMProfileWrap_Release          ( l2cmsProfileWrap_s**p )
{
  int error = !p;
  l2cmsProfileWrap_s * s = 0;
  
  if(!error && *p)
    s = *p;

  if(!error)
    error = !s;

  if(!error && s->type != l2cmsOBJECT_PROFILE)
    error = 1;

  if(!error)
  {
#if LCMS_VERSION >= 2060
    cmsContext tc = l2cmsGetProfileContextID( s->l2cms );
    oyProfile_s * p = l2cmsGetContextUserData( tc );
    oyProfile_Release ( &p );
    l2cmsDeleteContext( tc );
#endif
    CMMProfileRelease_M (s->l2cms);
    oyProfile_Release( &s->dbg_profile );

    s->l2cms = 0;
    oyObject_Release( &s->oy_ );
    s->type = 0;
    if(s->block && s->size)
      free(s->block);
    s->size = 0;
    s->block = 0;
    free(s);
  }

  if(!error)
    *p = 0;

  return error;
}


/** l2cmsCMMDataOpen()
 *  @brief   oyCMMProfileOpen_t implementation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 *  @date    2007/12/27
 */
int          l2cmsCMMProfileWrap_Create (
                                       oyStruct_s        * data,
                                       oyPointer_s       * oy )
{
  oyPointer_s * s = 0;
  int error = 0;

  if(!error)
  {
    size_t size = 0;
    oyPointer block = 0;
    oyProfile_s * p = NULL;
    l2cmsProfileWrap_s * s = calloc(sizeof(l2cmsProfileWrap_s), 1);

    if(data->type_ == oyOBJECT_PROFILE_S)
    {
      p = (oyProfile_s*)data;
      block = oyProfile_GetMem( p, &size, 0, malloc );
    }

    s->type = l2cmsOBJECT_PROFILE;
    s->oy_ = oyObject_NewFrom( NULL, "l2cmsProfileWrap_s" );
    oyObject_SetParent( s->oy_, l2cmsOBJECT_PROFILE, (oyPointer)s );
    s->size = size;
    s->block = block;
    if(oy_debug >= 2)
    {
      s->dbg_profile = oyProfile_Copy( p, 0 );
      l2cms_msg( oyMSG_DBG, data,
                OY_DBG_FORMAT_" going to open %s", OY_DBG_ARGS_,
                oyProfile_GetText(p, oyNAME_JSON) );
    }

#if LCMS_VERSION < 2060
    s->l2cms = CMMProfileOpen_M( data, block, size );
#else
    {
      oyProfile_s * p2 = oyProfile_Copy( p, NULL );
      cmsContext tc = l2cmsCreateContext( NULL, p2 ); /* threading context */
      l2cmsSetLogErrorHandlerTHR( tc, l2cmsErrorHandlerFunction );
      s->l2cms = CMMProfileOpen_M( tc, block, size );
    }
#endif
    s->id = oyStruct_GetId( (oyStruct_s*)p );
    if(!s->l2cms)
      l2cms_msg( oyMSG_WARN, (oyStruct_s*)data,
             OY_DBG_FORMAT_" %s() failed", OY_DBG_ARGS_, "CMMProfileOpen_M" );
    error = oyPointer_Set( oy, 0,
                           l2cmsPROFILE, s, CMMToString_M(CMMProfileOpen_M),
                           (int (*)(oyPointer *))l2cmsCMMProfileWrap_Release );
    if(error)
      l2cms_msg( oyMSG_WARN, (oyStruct_s*)data,
             OY_DBG_FORMAT_" oyPointer_Set() failed", OY_DBG_ARGS_ );
  }

  if(!error)
    s = oy;

  if(!error)
    error = !s;

  return error;
}

/** Function l2cmsCMMCheckPointer
 *  @brief   
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/02/21
 *  @since   2007/11/12 (Oyranos: 0.1.8)
 */
int        l2cmsCMMCheckPointer      ( oyPointer_s       * cmm_ptr,
                                       const char        * resource )
{
  int error = !cmm_ptr;

  if(cmm_ptr &&
     oyPointer_GetPointer(cmm_ptr) && oyPointer_GetResourceName(cmm_ptr))
  {
    int * res_id = (int*)oyPointer_GetResourceName(cmm_ptr);

    if(!oyCMMlibMatchesCMM(oyPointer_GetLibName(cmm_ptr), CMM_NICK) ||
       *res_id != *((int*)(resource)) )
      error = 1;
  } else {
    error = 1;
  }

  return error;
}



/** Function oyPixelToLcm2PixelLayout_
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int        oyPixelToLcm2PixelLayout_ ( oyPixel_t           pixel_layout,
                                       icColorSpaceSignature color_space )
{
  int cmm_pixel = 0;
  int chan_n = oyToChannels_m (pixel_layout);
  int c_off = oyToColorOffset_m (pixel_layout);
  oyDATATYPE_e data_type = oyToDataType_m (pixel_layout);
  int planar = oyToPlanar_m (pixel_layout);
  int flavour = oyToFlavor_m (pixel_layout);
  unsigned int cchans = l2cmsChannelsOf( (cmsColorSpaceSignature)color_space );
  unsigned int l2cms_color_space = l2_cmsLCMScolorSpace( (cmsColorSpaceSignature)color_space );
  int extra = chan_n - cchans;

  if(chan_n > CMMMaxChannels_M)
    l2cms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_" "
             "can not handle more than %d channels; found: %d",
             OY_DBG_ARGS_, CMMMaxChannels_M, chan_n);

  cmm_pixel |= CHANNELS_SH(cchans);
  if(extra)
    cmm_pixel |= EXTRA_SH(extra);
  if(c_off == 1)
    cmm_pixel |= SWAPFIRST_SH(1);
  if(data_type == oyUINT8)
    cmm_pixel |= BYTES_SH(1);
  else if(data_type == oyUINT16 || data_type == oyHALF)
    cmm_pixel |= BYTES_SH(2);
  else if(data_type == oyFLOAT)
    cmm_pixel |= BYTES_SH(4);
  else if(data_type == oyDOUBLE)
    cmm_pixel |= BYTES_SH(0);
  if(data_type == oyDOUBLE || data_type == oyFLOAT || data_type == oyHALF)
    cmm_pixel |= FLOAT_SH(1);
  if(oyToSwapColorChannels_m (pixel_layout))
    cmm_pixel |= DOSWAP_SH(1);
  if(oyToByteswap_m(pixel_layout))
    cmm_pixel |= ENDIAN16_SH(1);
  if(planar)
    cmm_pixel |= PLANAR_SH(1);
  if(flavour)
    cmm_pixel |= FLAVOR_SH(1);

  /* lcms2 uses V4 style value ranges */
  cmm_pixel |= COLORSPACE_SH( l2cms_color_space );


  return cmm_pixel;
}

/** Function l2cmsCMMDeleteTransformWrap
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/00 (Oyranos: 0.1.8)
 *  @date    2007/12/00
 */
int l2cmsCMMDeleteTransformWrap(l2cmsTransformWrap_s ** wrap)
{
  
  if(wrap && *wrap)
  {
    l2cmsTransformWrap_s * s = *wrap;

    l2cmsDeleteTransform (s->l2cms);
    s->l2cms = 0;

    free(s);

    *wrap = 0;

    return 0;
  }

  return 1;
}

/** Function l2cmsTransformWrap_Set_
 *  @brief   fill a l2cmsTransformWrap_s struct
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/12/21
 */
l2cmsTransformWrap_s * l2cmsTransformWrap_Set_ (
                                       cmsHTRANSFORM       xform,
                                       icColorSpaceSignature color_in,
                                       icColorSpaceSignature color_out,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyPointer_s       * oy )
{
  int error = !xform;
  l2cmsTransformWrap_s * s = 0;
  
  if(!error)
  {
    l2cmsTransformWrap_s * ltw = calloc(sizeof(l2cmsTransformWrap_s), 1);

    ltw->type = l2cmsOBJECT_TRANSFORM;

    ltw->l2cms = xform; xform = 0;

    ltw->sig_in  = color_in;
    ltw->sig_out = color_out;
    ltw->oy_pixel_layout_in  = oy_pixel_layout_in;
    ltw->oy_pixel_layout_out = oy_pixel_layout_out;
    s = ltw;
    if(oy_debug >= 2)
      l2cms_msg( oyMSG_DBG, NULL, OY_DBG_FORMAT_
             " xform: "OY_PRINT_POINTER
             " ltw: "OY_PRINT_POINTER,
             OY_DBG_ARGS_, ltw->l2cms, ltw );
  }

  if(!error)
    oyPointer_Set( oy, 0, 0, s,
                   "l2cmsCMMDeleteTransformWrap", (int (*)(oyPointer *)) l2cmsCMMDeleteTransformWrap );

  return s;
}

int            l2cmsIntentFromOptions ( oyOptions_s       * opts,
                                       int                 proof )
{
  int intent = 0,
      intent_proof = 0;
  const char * o_txt = 0;

#ifndef oyStrlen_
#define oyStrlen_ strlen
#endif
      o_txt = oyOptions_FindString  ( opts, "rendering_intent", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_intent_proof", 0);
      if(o_txt && oyStrlen_(o_txt))
        intent_proof = atoi( o_txt );

      intent_proof = intent_proof == 0 ? INTENT_RELATIVE_COLORIMETRIC :
                                         INTENT_ABSOLUTE_COLORIMETRIC;

  if(oy_debug > 2)
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)opts, OY_DBG_FORMAT_"\n"
             "  proof: %d  intent: %d  intent_proof: %d\n",
             OY_DBG_ARGS_,
                proof,     intent,     intent_proof );

  if(proof)
    return intent_proof;
  else
    return intent;
}

uint32_t       l2cmsFlagsFromOptions  ( oyOptions_s       * opts )
{
  int bpc = 0,
      gamut_warning = 0,
      precalculation = 0,
      precalculation_curves = 1,
      no_white_on_white_fixup = 1,
      flags = 0;
  const char * o_txt = 0;
  static int precalculation_curves_warn = 0;

      o_txt = oyOptions_FindString  ( opts, "rendering_bpc", 0 );
      if(o_txt && oyStrlen_(o_txt))
        bpc = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "rendering_gamut_warning", 0 );
      if(o_txt && oyStrlen_(o_txt))
        gamut_warning = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "precalculation", 0 );
      if(o_txt && oyStrlen_(o_txt))
        precalculation = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "precalculation_curves", 0 );
      if(o_txt && oyStrlen_(o_txt))
        precalculation_curves = atoi( o_txt );

      o_txt = oyOptions_FindString  ( opts, "no_white_on_white_fixup", 0 );
      if(o_txt && oyStrlen_(o_txt))
        no_white_on_white_fixup = atoi( o_txt );

      /* this should be moved to the CMM and not be handled here in Oyranos */
      flags = bpc ?           flags | cmsFLAGS_BLACKPOINTCOMPENSATION :
                              flags & (~cmsFLAGS_BLACKPOINTCOMPENSATION);
      flags = gamut_warning ? flags | cmsFLAGS_GAMUTCHECK :
                              flags & (~cmsFLAGS_GAMUTCHECK);
      flags = no_white_on_white_fixup ?
                              flags | cmsFLAGS_NOWHITEONWHITEFIXUP :
                              flags & (~cmsFLAGS_NOWHITEONWHITEFIXUP);
      switch(precalculation)
      {
      case 0: flags |= 0; break;
      case 1: flags |= cmsFLAGS_NOOPTIMIZE; break;
      case 2: flags |= cmsFLAGS_HIGHRESPRECALC; break;
      case 3: flags |= cmsFLAGS_LOWRESPRECALC; break;
      case 4: flags |= cmsFLAGS_NULLTRANSFORM; break;
      }

      if(l2cmsGetEncodedCMMversion() >= 2070)
      {
        switch(precalculation_curves)
        {
        case 0: flags |= 0; break;
        case 1: flags |= cmsFLAGS_CLUT_POST_LINEARIZATION | cmsFLAGS_CLUT_PRE_LINEARIZATION; break;
        }
      } else
        if(precalculation_curves_warn++ == 0)
          l2cms_msg( oyMSG_WARN, (oyStruct_s*)opts, OY_DBG_FORMAT_
              "Skipping cmsFLAGS_CLUT_POST_LINEARIZATION! Can not handle flag for DL creation. v%d\n",
              OY_DBG_ARGS_, l2cmsGetEncodedCMMversion() );

  if(oy_debug > 2)
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)opts, OY_DBG_FORMAT_"\n"
              "%s\n", OY_DBG_ARGS_, l2cmsFlagsToText(flags) );

  return flags;
}

uint16_t in[4] = {32000,32000,32000,0}, out[4] = {65535,65535,65535,65535};

/** Function l2cmsCMMConversionContextCreate_
 *  @brief   create a CMM transform
 *
 *  @version Oyranos: 0.3.3
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2011/11/18
 */
cmsHTRANSFORM  l2cmsCMMConversionContextCreate_ (
                                       oyFilterNode_s    * node,
                                       cmsHPROFILE       * lps,
                                       int                 profiles_n,
                                       oyProfiles_s      * simulation,
                                       int                 proof_n,
                                       int                 proof,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       oyOptions_s       * opts,
                                       l2cmsTransformWrap_s ** ltw,
                                       oyPointer_s       * oy,
                                       int                 verbose )
{
  oyPixel_t l2cms_pixel_layout_in = 0;
  oyPixel_t l2cms_pixel_layout_out = 0;
  int error = !lps;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE * merge = 0;
  icColorSpaceSignature color_in = 0;
  icColorSpaceSignature color_out = 0;
  icProfileClassSignature profile_class_in = 0;
  int intent = l2cmsIntentFromOptions( opts,0 ),
      intent_proof = l2cmsIntentFromOptions( opts,1 ),
      cmyk_cmyk_black_preservation = 0,
      flags = l2cmsFlagsFromOptions( opts ),
      gamut_warning = flags & cmsFLAGS_GAMUTCHECK;
  const char * o_txt = 0;
  double adaption_state = 0.0;
  int multi_profiles_n = profiles_n;

  if(!lps || !profiles_n || !oy_pixel_layout_in || !oy_pixel_layout_out)
    return 0;

  flags = proof ?             flags | cmsFLAGS_SOFTPROOFING :
                              flags & (~cmsFLAGS_SOFTPROOFING);

  if(!error && lps[0] && lps[profiles_n-1])
  {
    color_in = (icColorSpaceSignature) l2cmsGetColorSpace( lps[0] );
    if(profiles_n > 1)
      color_out = (icColorSpaceSignature) l2cmsGetColorSpace( lps[profiles_n-1] );
    else
      color_out = (icColorSpaceSignature) l2cmsGetPCS( lps[profiles_n-1] );
    profile_class_in = (icProfileClassSignature) l2cmsGetDeviceClass( lps[0] );
  }

  if(oy_debug || verbose)
    l2cms_msg( oy_debug?oyMSG_DBG:oyMSG_WARN,(oyStruct_s*)node, OY_DBG_FORMAT_"\n"
             "  profiles_n: %d  proof_n: %d",
             OY_DBG_ARGS_, profiles_n, proof_n );

  l2cms_pixel_layout_in  = oyPixelToLcm2PixelLayout_(oy_pixel_layout_in,
                                                   color_in);
  l2cms_pixel_layout_out = oyPixelToLcm2PixelLayout_(oy_pixel_layout_out,
                                                   color_out);

  o_txt = oyOptions_FindString  ( opts, "cmyk_cmyk_black_preservation", 0 );
  if(o_txt && oyStrlen_(o_txt))
    cmyk_cmyk_black_preservation = atoi( o_txt );

  intent = cmyk_cmyk_black_preservation ? intent + 10 : intent;
  if(cmyk_cmyk_black_preservation == 2)
    intent += 13;

  o_txt = oyOptions_FindString  ( opts, "adaption_state", 0 );
  if(o_txt && oyStrlen_(o_txt))
    oyjlStringToDouble( o_txt, &adaption_state, 0 );

  if(!error)
  {
    cmsUInt32Number * intents=0;
    cmsBool * bpc=0;
    cmsFloat64Number * adaption_states=0;

         if(profiles_n == 1 || profile_class_in == icSigLinkClass)
    {
        /* we have to erase the color space */
#if 1
        int csp = T_COLORSPACE(l2cms_pixel_layout_in);
        l2cms_pixel_layout_in &= (~COLORSPACE_SH( csp ));
        csp = T_COLORSPACE(l2cms_pixel_layout_out);
        l2cms_pixel_layout_out &= (~COLORSPACE_SH( csp ));
#endif
        xform = l2cmsCreateTransform( lps[0], l2cms_pixel_layout_in,
                                     0, l2cms_pixel_layout_out,
                                     (intent > 3)?0:intent,
                                     flags | cmsFLAGS_KEEP_SEQUENCE );
        if(oy_debug || verbose)
        {
          int level = oy_debug?oyMSG_DBG:oyMSG_WARN, f;
          l2cms_msg( level,(oyStruct_s*)node, OY_DBG_FORMAT_"\n"
               "  created xform: " OY_PRINT_POINTER " %s", OY_DBG_ARGS_, xform, l2cmsFlagsToText(flags) );
          f = l2cms_pixel_layout_in;
          l2cms_msg( level, (oyStruct_s*)opts, OY_DBG_FORMAT_
              "float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d",
              OY_DBG_ARGS_,
              T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
          f = l2cms_pixel_layout_out;
          l2cms_msg( level, (oyStruct_s*)opts, OY_DBG_FORMAT_
              "float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d",
              OY_DBG_ARGS_,
              T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
        }
    }
    else if(profiles_n == 2 && (!proof_n || (!proof && !gamut_warning)))
    {
      oyAllocHelper_m_( intents, cmsUInt32Number, 2,0, goto end);
      oyAllocHelper_m_( bpc, cmsBool, 2,0, goto end);
      oyAllocHelper_m_( adaption_states, cmsFloat64Number, 2,0, goto end);
      intents[0] = intent; intents[1] = intent;
      bpc[0] = flags & cmsFLAGS_BLACKPOINTCOMPENSATION;
      bpc[1] = flags & cmsFLAGS_BLACKPOINTCOMPENSATION;
      adaption_states[0] = adaption_state; adaption_states[1] = adaption_state;
      xform = l2cmsCreateExtendedTransform( 0, profiles_n, lps, bpc,
                                          intents, adaption_states, NULL, 0,
                                          l2cms_pixel_layout_in,
                                          l2cms_pixel_layout_out, flags | cmsFLAGS_KEEP_SEQUENCE );
      oyFree_m_(intents);
      oyFree_m_(bpc);
      oyFree_m_(adaption_states);
    }
    else
    {
      int i;

      if(proof_n && (proof || gamut_warning))
      {
        int len = sizeof(cmsHPROFILE) * (profiles_n + proof_n);

        oyAllocHelper_m_( merge, cmsHPROFILE, profiles_n + proof_n,0, goto end);
        memset( merge, 0, len );
        memcpy( merge, lps, sizeof(cmsHPROFILE) * (profiles_n - 1) );

        for(i = 0; i < proof_n; ++i)
        {
          oyProfile_s * p = oyProfiles_Get(simulation,i);
          l2cmsProfileWrap_s * wrap = l2cmsAddProofProfile( 
                                             p, flags,
                                             intent, intent_proof, 0);
          oyProfile_Release( &p );
          if(!wrap || !wrap->l2cms )
          {
            error = 1;
            l2cms_msg( oyMSG_ERROR, (oyStruct_s*)simulation, OY_DBG_FORMAT_
                "simulation profiles: %d",
                OY_DBG_ARGS_,
                oyProfiles_Count(simulation) );
            break;
          }
          else
            merge[profiles_n-1 + i] = wrap->l2cms;

          oyObject_UnRef( wrap->oy_ ); wrap = NULL;
        }

        merge[profiles_n + proof_n -1] = lps[profiles_n - 1];

        /* merge effect and simulation profiles */
        multi_profiles_n += proof_n;
        lps = merge;
      }

      if(flags & cmsFLAGS_GAMUTCHECK)
        flags |= cmsFLAGS_GRIDPOINTS(l2cmsPROOF_LUT_GRID_RASTER);

      if(oy_debug > 2)
      {
        uint32_t f = l2cms_pixel_layout_in;
        printf ("%s:%d %s() float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
        f = l2cms_pixel_layout_out;
        printf ("%s:%d %s() float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d \n", __FILE__,__LINE__,__func__, T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
        printf("multi_profiles_n: %d intent: %d adaption: %g flags: %d \"%s\" l1 %d, l2 %d\n",
              multi_profiles_n, intent, adaption_state, flags,
              l2cmsFlagsToText(flags),
              l2cms_pixel_layout_in, l2cms_pixel_layout_out);
      }

#define SET_ARR(arr,val,n) for(i = 0; i < n; ++i) arr[i] = val;
      oyAllocHelper_m_( intents, cmsUInt32Number, multi_profiles_n,0, goto end);
      oyAllocHelper_m_( bpc, cmsBool, multi_profiles_n,0, goto end);
      oyAllocHelper_m_( adaption_states, cmsFloat64Number, multi_profiles_n,0, goto end);
      SET_ARR(intents,intent,multi_profiles_n);
      SET_ARR(bpc,flags & cmsFLAGS_BLACKPOINTCOMPENSATION,multi_profiles_n);
      SET_ARR(adaption_states,adaption_state,multi_profiles_n);
      xform = l2cmsCreateExtendedTransform( 0, multi_profiles_n, lps, bpc,
                                          intents, adaption_states, NULL, 0,
                                          l2cms_pixel_layout_in,
                                          l2cms_pixel_layout_out, flags | cmsFLAGS_KEEP_SEQUENCE );

      if(oy_debug >= 2)
      {
        int i;
        l2cms_msg( oyMSG_DBG, (oyStruct_s*)opts,
                OY_DBG_FORMAT_"l2cmsCreateExtendedTransform(multi_profiles_n %d)"
                " xform: " OY_PRINT_POINTER,
                OY_DBG_ARGS_, multi_profiles_n, xform, ltw );
#if LCMS_VERSION >= 2060
        for(i = 0; i < multi_profiles_n; ++i)
        {
          cmsContext tc = l2cmsGetProfileContextID( lps[i] );
          oyProfile_s * p = l2cmsGetContextUserData( tc );
          const char * fn = oyProfile_GetFileName( p, -1 );
          size_t size = 0;
          char * block = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
          oyFree_m_(block);
          fprintf( stdout, " -> \"%s\"[%lu]", fn?fn:"----", (long unsigned int)size );
        }
        fprintf(stdout, "\n");
#endif
      }

#ifdef ENABLE_MPE
      unsigned char in[3] = {128,128,128};
      unsigned short o[3];
      l2cmsDoTransform( xform, in, o, 1 );
      printf("%d %d %d\n", o[0],o[1],o[2]);
#endif /* ENABLE_MPE */

      oyFree_m_( intents );
      oyFree_m_( bpc );
      oyFree_m_( adaption_states );
    }
  }

  if(!xform || oy_debug > 2)
  {
    int level = oyMSG_DBG;
    uint32_t f = l2cms_pixel_layout_in, i;

    if(!xform || !oy_debug)
    {
      level = oyMSG_WARN;
      error = 1;
    }

    l2cms_msg( level, (oyStruct_s*)opts, OY_DBG_FORMAT_
              " float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d",
              OY_DBG_ARGS_,
              T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
    f = l2cms_pixel_layout_out;
    l2cms_msg( level, (oyStruct_s*)opts, OY_DBG_FORMAT_
              "float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes %d",
              OY_DBG_ARGS_,
              T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
    l2cms_msg( level, (oyStruct_s*)opts, OY_DBG_FORMAT_
              "multi_profiles_n: %d intent: %d adaption: %g \"%s\"",
              OY_DBG_ARGS_,
              multi_profiles_n, intent, adaption_state,
              l2cmsFlagsToText(flags));
    for(i=0; i < (unsigned)profiles_n; ++i)
      l2cms_msg( level,(oyStruct_s*)node, OY_DBG_FORMAT_"\n"
             "  ColorSpace:%s->PCS:%s DeviceClass:%s",
             OY_DBG_ARGS_,
             lps[0]?oyICCColorSpaceGetName((icColorSpaceSignature) l2cmsGetColorSpace( lps[0])):"----",
             lps[i]?oyICCColorSpaceGetName((icColorSpaceSignature) l2cmsGetPCS( lps[i] )):"----",
             lps[i]?oyICCDeviceClassDescription((icProfileClassSignature) l2cmsGetDeviceClass(lps[i])):"----" );
  } else
  if(oy_debug || verbose)
    l2cms_msg( oy_debug?oyMSG_DBG:oyMSG_WARN,(oyStruct_s*)node, OY_DBG_FORMAT_"\n"
               "  opened xform: " OY_PRINT_POINTER, OY_DBG_ARGS_, xform );

  if(!error && ltw && oy)
    *ltw= l2cmsTransformWrap_Set_( xform, color_in, color_out,
                                  oy_pixel_layout_in, oy_pixel_layout_out, oy );

  end:
  return xform;
}

/** Function l2cmsCMMColorConversion_ToMem_
 *
 *  convert a l2cms color conversion context to a device link
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyPointer  l2cmsCMMColorConversion_ToMem_ (
                                       cmsHTRANSFORM     * xform,
                                       oyOptions_s       * opts,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int error = !xform;
  oyPointer data = 0;
  int flags = l2cmsFlagsFromOptions( opts );

  if(flags & cmsFLAGS_GAMUTCHECK)
    flags |= cmsFLAGS_GRIDPOINTS(l2cmsPROOF_LUT_GRID_RASTER);

  if(!error)
  {
    cmsHPROFILE dl= l2cmsTransform2DeviceLink( xform, 4.3,
                                              flags | cmsFLAGS_KEEP_SEQUENCE );

    *size = 0;

#if 0
    {
        int nargs = 1, i;
        size_t size = sizeof(int) + nargs * sizeof(cmsPSEQDESC);
        LPcmsSEQ pseq = (LPcmsSEQ) oyAllocateFunc_(size);
        
        ZeroMemory(pseq, size);
        pseq ->n = nargs;

        for (i=0; i < nargs; i++) {

            strcpy(pseq ->seq[i].Manufacturer, CMM_NICK);
            strcpy(pseq ->seq[i].Model, "CMM ");
        }

        cmsAddTag(dl, icSigProfileSequenceDescTag, pseq);
        free(pseq);
    }
#endif
    if(dl)
    {
      data = lcm2WriteProfileToMem( dl, size, allocateFunc );
      l2cmsCloseProfile( dl );
    }
  }

  return data;
}

oyOptions_s* l2cmsFilter_CmmIccValidateOptions
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate OY_UNUSED,
                                       int                 statical OY_UNUSED,
                                       uint32_t          * result )
{
  uint32_t error = !filter;

  if(!error)
    error = oyFilterRegistrationMatch(oyFilterCore_GetRegistration(filter),
                                      "//"OY_TYPE_STD"/icc_color",
                                      oyOBJECT_CMM_API4_S);

  *result = error;

  return 0;
}

oyWIDGET_EVENT_e   l2cmsWidgetEvent  ( oyOptions_s       * options OY_UNUSED,
                                       oyWIDGET_EVENT_e    type OY_UNUSED,
                                       oyStruct_s        * event OY_UNUSED )
{return 0;}


oyDATATYPE_e l2cms_cmmIcc_data_types[7] = {oyUINT8, oyUINT16, oyHALF, oyFLOAT, oyDOUBLE, 0};

oyConnectorImaging_s_ l2cms_cmmIccSocket_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorSocketText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  0, /* is_plug == oyFilterPlug_s */
  l2cms_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_color_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_color_count; */
  16, /* max_color_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* channel_types_n */
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s_* l2cms_cmmIccSocket_connectors[2]={&l2cms_cmmIccSocket_connector,0};

oyConnectorImaging_s_ l2cms_cmmIccPlug_connector = {
  oyOBJECT_CONNECTOR_IMAGING_S,0,0,
                               (oyObject_s)&oy_connector_imaging_static_object,
  oyCMMgetImageConnectorPlugText, /* getText */
  oy_image_connector_texts, /* texts */
  "//" OY_TYPE_STD "/manipulator.data", /* connector_type */
  oyFilterSocket_MatchImagingPlug, /* filterSocket_MatchPlug */
  1, /* is_plug == oyFilterPlug_s */
  l2cms_cmmIcc_data_types, /* data_types */
  3, /* data_types_n; elements in data_types array */
  1, /* max_color_offset */
  1, /* min_channels_count; */
  16, /* max_channels_count; */
  1, /* min_color_count; */
  16, /* max_color_count; */
  1, /* can_planar; can read separated channels */
  1, /* can_interwoven; can read continuous channels */
  1, /* can_swap; can swap color channels (BGR)*/
  1, /* can_swap_bytes; non host byte order */
  1, /* can_revert; revert 1 -> 0 and 0 -> 1 */
  1, /* can_premultiplied_alpha; */
  1, /* can_nonpremultiplied_alpha; */
  0, /* can_subpixel; understand subpixel order */
  0, /* oyCHANNELTYPE_e    * channel_types; */
  0, /* channel_types_n */
  1, /* id; relative to oyFilterCore_s, e.g. 1 */
  0  /* is_mandatory; mandatory flag */
};
oyConnectorImaging_s_* l2cms_cmmIccPlug_connectors[2]={&l2cms_cmmIccPlug_connector,0};

/** Function l2cmsAddProofProfile
 *  @brief   add a abstract proofing profile to the l2cms profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation or generate a
 *  new abstract profile containing the proofing profiles changes. This can be
 *  a proofing color space simulation or out of gamut marking.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/02
 *  @since   2009/11/05 (Oyranos: 0.1.10)
 */
l2cmsProfileWrap_s*l2cmsAddProofProfile( oyProfile_s     * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof,
                                       uint32_t            icc_profile_flags )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyPointer_s * cmm_ptr = 0;
  l2cmsProfileWrap_s * s = 0;
  char * hash_text = 0,
       num[12];

  if(!proof || proof->type_ != oyOBJECT_PROFILE_S)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)proof, OY_DBG_FORMAT_
              "no profile provided %s", OY_DBG_ARGS_,
              (proof != NULL) ? oyStruct_GetText( (oyStruct_s*) proof->type_, oyNAME_NAME, 0 ) : "" );
    return 0;
  }

  /* build hash text */
  STRING_ADD( hash_text, "abstract proofing profile " );
  STRING_ADD( hash_text, oyObject_GetName( proof->oy_, oyNAME_NICK ) );
  STRING_ADD( hash_text, " intent:" );
  sprintf( num, "%d", intent );
  STRING_ADD( hash_text, num );
  STRING_ADD( hash_text, " intent_proof:" );
  sprintf( num, "%d", intent_proof );
  STRING_ADD( hash_text, num );
  STRING_ADD( hash_text, " flags|gmtCheck|softPrf:" );
  sprintf( num, "%d|%d|%d", (int)flags, (flags & cmsFLAGS_GAMUTCHECK)?1:0,
                                        (flags & cmsFLAGS_SOFTPROOFING)?1:0 );
  STRING_ADD( hash_text, num );

  /* cache look up */
  cmm_ptr = oyPointer_LookUpFromText( hash_text, l2cmsPROFILE );

  oyPointer_Set( cmm_ptr, CMM_NICK, 0,0,0,0 );

  /* for empty profile create a new abstract one */
  if(!oyPointer_GetPointer(cmm_ptr))
  {
    oyPointer_s * oy = cmm_ptr;

    size_t size = 0;
    oyPointer block = 0;
    l2cmsProfileWrap_s * s = calloc(sizeof(l2cmsProfileWrap_s), 1);

    if(oy_debug > 3)
      fprintf( stderr, OY_DBG_FORMAT_" created: \"%s\"",
               OY_DBG_ARGS_, hash_text );
    else
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)proof,
             OY_DBG_FORMAT_" created abstract proofing profile: \"%s\"",
             OY_DBG_ARGS_, hash_text );
 
    /* create */
    hp = l2cmsGamutCheckAbstract( proof, flags, intent, intent_proof, icc_profile_flags );
    if(hp)
    {
      /* save to memory */
      block = lcm2WriteProfileToMem( hp, &size, malloc );
      if(oy_debug >= 2)
        l2cms_msg( oyMSG_DBG, (oyStruct_s*)proof, OY_DBG_FORMAT_
                   " wrote to mem and close: " OY_PRINT_POINTER, OY_DBG_ARGS_, hp );
      l2cmsCloseProfile( hp ); hp = 0;
    } else
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)proof, OY_DBG_FORMAT_
                 "no abstract profile created", OY_DBG_ARGS_ );

    s->type = l2cmsOBJECT_PROFILE;
    s->oy_ = oyObject_NewFrom( NULL, "l2cmsProfileWrap_s" );
    oyObject_SetParent( s->oy_, l2cmsOBJECT_PROFILE, (oyPointer)s );
    s->size = size;
    s->block = block;
    s->id = oyStruct_GetId( (oyStruct_s*)proof );

    /* reopen */
#if LCMS_VERSION < 2060
    s->l2cms = CMMProfileOpen_M( proof, block, size );
#else
    {
      oyProfile_s * proof2 = oyProfile_Copy( proof, NULL );
      cmsContext tc = l2cmsCreateContext( NULL, proof2 ); /* threading context */
      l2cmsSetLogErrorHandlerTHR( tc, l2cmsErrorHandlerFunction );
      s->l2cms = CMMProfileOpen_M( tc, block, size );
    }
#endif
    error = oyPointer_Set( oy, 0,
                           l2cmsPROFILE, s, CMMToString_M(CMMProfileOpen_M),
                           (int (*)(oyPointer *))l2cmsCMMProfileWrap_Release );
  }

  if(!error)
  {
    s = l2cmsCMMProfile_GetWrap_( cmm_ptr );
    error = !s;
  }

  oyPointer_Release( &cmm_ptr );
  if(hash_text)
    oyFree_m_(hash_text);

  if(!error)
    return s;
  else
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)proof,
             OY_DBG_FORMAT_" adding %s failed", OY_DBG_ARGS_, oyProfile_GetText( proof, oyNAME_DESCRIPTION ) );
    return 0;
  }
}


/** Function l2cmsAddProfile
 *  @brief   add a profile from Oyranos to the l2cms profile stack 
 *
 *  Look in the Oyranos cache for a CMM internal representation
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
cmsHPROFILE  l2cmsAddProfile          ( oyProfile_s       * p )
{
  int error = 0;
  cmsHPROFILE * hp = 0;
  oyPointer_s * cmm_ptr = 0;
  l2cmsProfileWrap_s * s = 0;

  if(!p || p->type_ != oyOBJECT_PROFILE_S)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)p, OY_DBG_FORMAT_" "
             "no profile provided", OY_DBG_ARGS_ );
    return 0;
  }

  cmm_ptr = oyPointer_LookUpFromObject( (oyStruct_s*)p, l2cmsPROFILE );
  if(oy_debug >= 2)
  {
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)p,
              OY_DBG_FORMAT_" going to open %s cmm_ptr: %d", OY_DBG_ARGS_,
              oyProfile_GetText(p, oyNAME_JSON), oyStruct_GetId((oyStruct_s*)cmm_ptr) );
  }

  if(!cmm_ptr)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)p,
             OY_DBG_FORMAT_" oyPointer_LookUpFromObject() failed", OY_DBG_ARGS_ );
    return 0;
  }

  oyPointer_Set( cmm_ptr, CMM_NICK, 0,0,0,0 );

  if(!oyPointer_GetPointer(cmm_ptr))
    error = l2cmsCMMProfileWrap_Create( (oyStruct_s*)p, cmm_ptr );

  if(error)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)p,
             OY_DBG_FORMAT_" l2cmsCMMProfileWrap_Create() failed", OY_DBG_ARGS_ );
  } else
  {
    s = l2cmsCMMProfile_GetWrap_( cmm_ptr );
    error = !s;
    if(error)
      l2cms_msg( oyMSG_WARN, (oyStruct_s*)p,
             OY_DBG_FORMAT_" l2cmsCMMProfile_GetWrap_() failed", OY_DBG_ARGS_ );
  }

  if(!error)
  {
    hp = s->l2cms;
    if(oy_debug >= 2)
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)p, OY_DBG_FORMAT_
              " opened profile: " OY_PRINT_POINTER, OY_DBG_ARGS_, hp );
    oyObject_UnRef( s->oy_ ); s = NULL;
  }

  oyPointer_Release( &cmm_ptr );

  if(!error)
    return hp;
  else
    return 0;
}


int gamutCheckSampler16(const cmsUInt16Number In[],
                              cmsUInt16Number Out[],
                        void                * Cargo)
{
  cmsCIELab Lab1, Lab2;
  oyPointer * ptr = (oyPointer*)Cargo;

  l2cmsLabEncoded2Float(&Lab1, In);
  l2cmsDoTransform( ptr[0], In, Out, 1 );
  l2cmsLabEncoded2Float(&Lab2, Out);
  /*double d = cmsDeltaE( &Lab1, &Lab2 );
  if(abs(d) > 10 && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }*/
  l2cmsFloat2LabEncoded(Out, &Lab2); 

  return TRUE;
}

int  gamutCheckSamplerFloat          ( const cmsFloat32Number In[],
                                             cmsFloat32Number Out[],
                                       void                 * Cargo )
{
  cmsCIELab Lab1, Lab2;
  double d;
  cmsFloat32Number i[3], o[3];
  oyPointer * ptr = (oyPointer*)Cargo;

  i[0] = Lab1.L = In[0] * 100.0;
  i[1] = Lab1.a = In[1] * 257.0 - 128.0;
  i[2] = Lab1.b = In[2] * 257.0 - 128.0;

  l2cmsDoTransform( ptr[0], i, o, 1 );

  Lab2.L = o[0]; Lab2.a = o[1]; Lab2.b = o[2];

  d = l2cmsDeltaE( &Lab1, &Lab2 );
  if((fabs(d) > 10) && ptr[1] != NULL)
  {
    Lab2.L = 50.0;
    Lab2.a = Lab2.b = 0.0;
  }

  Out[0] = Lab2.L/100.0; 
  Out[1] = (Lab2.a + 128.0) / 257.0;
  Out[2] = (Lab2.b + 128.0) / 257.0;

  return TRUE;
}

const char *       oyICCMpeDescription(cmsStageSignature sig, int type )
{
#if LCMS_VERSION >= 2060
  switch ((unsigned int)sig)
  {
    // Multi process elements types
    case cmsSigCurveSetElemType: return type ? "cvst" : _("Curve Set");
    case cmsSigMatrixElemType:   return type ? "matf" : _("Matrix");
    case cmsSigCLutElemType:     return type ? "clut" : _("Look Up Table");

    case cmsSigBAcsElemType:     return type ? "bACS" : _("BAcs");
    case cmsSigEAcsElemType:     return type ? "eACS" : _("EAcs");

    // Custom from here, not in the ICC Spec
    case cmsSigXYZ2LabElemType:  return type ? "l2x " : _("XYZ2Lab");
    case cmsSigLab2XYZElemType:  return type ? "x2l " : _("Lab2XYZ");
    case cmsSigNamedColorElemType: return type ? "ncl " : _("Named Color");
    case cmsSigLabV2toV4:          return type ? "2 4 " : _("V2toV4");
    case cmsSigLabV4toV2:          return type ? "4 2 " : _("V4toV2");
  
    // Identities
    case cmsSigIdentityElemType:   return type ? "idn " : _("Identity");

    // Float to floatPCS
    case cmsSigLab2FloatPCS:       return type ? "d2l '" : _("Lab2FloatPCS");
    case cmsSigFloatPCS2Lab:       return type ? "l2d '" : _("FloatPCS2Lab");
    case cmsSigXYZ2FloatPCS:       return type ? "d2x '" : _("XYZ2FloatPCS");
#if LCMS_VERSION >= 2070
    case cmsSigFloatPCS2XYZ:       return type ? "x2d '" : _("FloatPCS2XYZ");
    case cmsSigClipNegativesElemType: return type ? "clp '" : _("Clip Negatives");
#endif /* >= 2070 */
    case 0: return _("----");
    default:
#endif /* >= 2060 */
             { static union { char c[8]; cmsStageSignature sig; } stage_sig = { .c[4] = 0 };
               stage_sig.sig = (cmsStageSignature)oyValueUInt32( sig );
               return stage_sig.c;
             }
#if LCMS_VERSION >= 2060
  }
#endif
}

// A single stage
struct _cmsStage_struct {

    cmsContext          ContextID;

    cmsStageSignature   Type;           // Identifies the stage
    cmsStageSignature   Implements;     // Identifies the *function* of the stage (for optimizations)
};

void printPipeline( cmsPipeline * lut )
{
  cmsStage * first = l2cmsPipelineGetPtrToFirstStage(lut),
           * next = first;
  int i = 0;
  do {
    fprintf(stderr, "stage[%d] %s:%s-%s %d -> %d\n", i, oyICCMpeDescription(l2cmsStageType(next),oyNAME_NICK),
            oyICCMpeDescription(next->Implements,oyNAME_NAME),
            oyICCMpeDescription(l2cmsStageType(next),oyNAME_NAME), l2cmsStageInputChannels(next), l2cmsStageOutputChannels(next) );
    ++i;
  } while ((next = l2cmsStageNext( next )) != NULL);
   
}


oyProfiles_s * l2cmsProfilesFromOptions( oyFilterNode_s * node, oyFilterPlug_s * plug,
                                        oyOptions_s * node_options,
                                        const char * key, int profiles_switch, int verbose )
{
  oyProfiles_s * profiles = NULL;
  oyOption_s * o = NULL;
  
  if(profiles_switch || oy_debug || verbose)
    o = oyOptions_Find( node_options, key, oyNAME_PATTERN );
  if(o)
  {
    profiles = (oyProfiles_s*) oyOption_GetStruct( o, oyOBJECT_PROFILES_S );
    if((oy_debug || verbose))
    {
      l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " found \"%s\" %d  switch %d",
               OY_DBG_ARGS_, key, oyProfiles_Count( profiles ), profiles_switch );
    } else
    if( !profiles )
    {
      oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION );
      l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_
               " incompatible \"%s\"", OY_DBG_ARGS_, key );
      
    }
    oyOption_Release( &o );
  }

  if(!profiles_switch)
    oyProfiles_Release( &profiles );

  return profiles;
}

/* create a linear device link for null color transform */
oyPointer l2cmsPassThroughDL         ( icColorSpaceSignature csp_in,
                                       icColorSpaceSignature csp_out,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  int cchann_in, cchann_out;
  oyPointer block = NULL;

  cchann_in = cmsChannelsOf(csp_in);
  cchann_out = cmsChannelsOf(csp_out);
  if(cchann_in == cchann_out)
  {
    cmsToneCurve * carr[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0},
                 * null_curve = cmsBuildGamma(0, 1.0);
    if(null_curve)
    {
      cmsHPROFILE pass_through = NULL;
      int i;
      for(i = 0; i < 16; ++i) carr[i] = null_curve;
      pass_through = cmsCreateLinearizationDeviceLink( csp_in, carr );
      if(pass_through)
      {
        cmsMLU * mlu = cmsMLUalloc(0,1);
        if(mlu)
        {
          char desc[32];
          sprintf(desc, "pass through %d", abs(cchann_in)<=1024?cchann_in:-1 );
          cmsMLUsetASCII(mlu, "EN", "us", desc);
          cmsWriteTag( pass_through, cmsSigProfileDescriptionTag, mlu );
          cmsMLUfree(mlu); mlu = NULL;
        }
        *size = 0;
        block = lcm2WriteProfileToMem( pass_through, size, oy_debug?oyAllocateFunc_:allocateFunc );
        cmsCloseProfile(pass_through); pass_through = NULL;
      }
      else
      {
        l2cms_msg( oyMSG_ERROR, NULL, OY_DBG_FORMAT_
                 "cmsCreateLinearizationDeviceLink() failed",
                 OY_DBG_ARGS_ );
      }
      cmsFreeToneCurve( null_curve );
      null_curve = NULL;
    } else
    {
      l2cms_msg( oyMSG_ERROR, NULL, OY_DBG_FORMAT_
                 "cmsBuildGamma(0, 1.0) failed",
                 OY_DBG_ARGS_ );
    }
  }
  else
  {
    l2cms_msg( oyMSG_ERROR, NULL, OY_DBG_FORMAT_
                 "channels_in %d == channels_out %d for linear device link failed",
                 OY_DBG_ARGS_, cchann_in, cchann_out );
  }

  return block;
}

/** l2cmsFilterNode_CmmIccContextToMem()
 *  @brief   implement oyCMMFilterNode_CreateContext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyPointer l2cmsFilterNode_CmmIccContextToMem (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  int error = 0;
  int n,i,len;
  size_t size_ = 0;
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * src_socket = oyFilterPlug_GetSocket( plug );
  oyImage_s * image_input = 0,
            * image_output = 0;
  cmsHPROFILE * lps = 0;
  cmsHTRANSFORM xform = 0;
  oyOptions_s * node_tags = oyFilterNode_GetTags( node ),
              * node_options = oyFilterNode_GetOptions( node, 0 );
  oyProfile_s * p = 0,
              * prof = 0,
              * image_input_profile,
              * image_output_profile;
  oyProfiles_s * profiles = 0,
               * profs = 0;
  oyProfileTag_s * psid = 0,
                 * info = 0,
                 * cprt = 0;
  int profiles_n = 0,
      profiles_simulation_n = 0,
      profiles_display_n = 0,
      proof = 0,
      effect_switch = 0;
  int verbose = oyOptions_FindString( node_tags, "verbose", "true" ) ? 1 : 0;

  image_input = (oyImage_s*)oyFilterSocket_GetData( src_socket );
  image_output = (oyImage_s*)oyFilterSocket_GetData( socket );
  image_input_profile = oyImage_GetProfile( image_input );
  image_output_profile = oyImage_GetProfile( image_output );

  if(!image_input)
    goto l2cmsFilterNode_CmmIccContextToMemClean;

  if(image_input->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" missed input image %d", OY_DBG_ARGS_,
             image_input->type_ );
  }
  if(!image_output || image_output->type_ != oyOBJECT_IMAGE_S)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" missed output image %d", OY_DBG_ARGS_, image_output?image_output->type_:0 );
  }

  /*oyDATATYPE_e data_type = 0;
  data_type = oyToDataType_m( oyImage_GetPixelLayout( image_input, oyLAYOUT ) );

  if(data_type == oyHALF)
  {
    oyFilterSocket_Callback( plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node,
             OY_DBG_FORMAT_" can not handle oyHALF", OY_DBG_ARGS_ );
  }*/

  len = sizeof(cmsHPROFILE) * (15 + 2 + 1);
  lps = oyAllocateFunc_( len );
  if(!lps)
    goto l2cmsFilterNode_CmmIccContextToMemClean;
  memset( lps, 0, len );

  /* input profile */
  lps[ profiles_n++ ] = l2cmsAddProfile( image_input_profile );
  if(!image_input_profile)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_input->profile_", OY_DBG_ARGS_ );
    goto l2cmsFilterNode_CmmIccContextToMemClean;
  }
  p = oyProfile_Copy( image_input_profile, 0 );
  profs = oyProfiles_New( 0 );
  error = oyProfiles_MoveIn( profs, &p, -1 );

  /* effect profiles */
  effect_switch = oyOptions_FindString  ( node_options, "effect_switch", "1" ) ? 1 : 0;
  profiles = l2cmsProfilesFromOptions( node, plug, node_options,
                                      "profiles_effect", effect_switch, verbose );
  n = oyProfiles_Count( profiles );
  if(n)
    for(i = 0; i < n; ++i)
    {
        p = oyProfiles_Get( profiles, i );

        /* Look in the Oyranos cache for a CMM internal representation */
        lps[ profiles_n++ ] = l2cmsAddProfile( p );
        error = oyProfiles_MoveIn( profs, &p, -1 );
    }
  oyProfiles_Release( &profiles );


  /* simulation profile */
  proof = oyOptions_FindString  ( node_options, "proof_soft", "1" ) ? 1 : 0;
  proof += oyOptions_FindString  ( node_options, "proof_hard", "1" ) ? 1 : 0;

  if(oy_debug  > 2 && proof)
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_
               " proof requested",OY_DBG_ARGS_);

  profiles = l2cmsProfilesFromOptions( node, plug, node_options,
                                       "profiles_simulation", proof, verbose );
  n = oyProfiles_Count( profiles );
  if(n)
    for(i = 0; i < n; ++i)
    {
      p = oyProfiles_Get( profiles, i );

      if(oy_debug)
        l2cms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                  " found profile: %s",
                  OY_DBG_ARGS_, p?oyProfile_GetFileName( p,-1 ):"????");

      error = oyProfiles_MoveIn( profs, &p, -1 );
      ++profiles_simulation_n;

      oyProfile_Release( &p );
    }
  else
    if(verbose || oy_debug > 2)
      l2cms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                " no simulation profile found", OY_DBG_ARGS_);


  /* display profile */
  profiles_display_n = oyOptions_CountType( node_options, "display.abstract.icc_profile", oyOBJECT_PROFILE_S );
  if(verbose || oy_debug)
  l2cms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_ "display.abstract.icc_profile[] = %d",
             OY_DBG_ARGS_, profiles_display_n );
  for(i = 0; i < profiles_display_n; ++i)
  {
    oyOption_s * o = NULL;
    error = oyOptions_GetType2( node_options, i, "display.abstract.icc_profile", oyNAME_PATTERN,
                                oyOBJECT_PROFILE_S, NULL, &o );
    const char * reg = oyOption_GetRegistration( o );
    p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
    if(verbose || oy_debug > 2)
      l2cms_msg( verbose?oyMSG_WARN:oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_ "display.abstract.icc_profile[%d]: %s:%s",
                 OY_DBG_ARGS_, i, reg, oyProfile_GetText(p,oyNAME_DESCRIPTION) );
    oyOption_Release( &o );
    lps[ profiles_n++ ] = l2cmsAddProfile( p );
    error = oyProfiles_MoveIn( profs, &p, -1 );
  }


  /* output profile */
  if(!image_output_profile)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_" "
             "missed image_output->profile_", OY_DBG_ARGS_ );
    goto l2cmsFilterNode_CmmIccContextToMemClean;
  }
  lps[ profiles_n++ ] = l2cmsAddProfile( image_output_profile );
  p = oyProfile_Copy( image_output_profile, 0 );
  error = oyProfiles_MoveIn( profs, &p, -1 );

  *size = 0;
  int flags = l2cmsFlagsFromOptions( node_options );

  /* create the context */
  if(flags & cmsFLAGS_NULLTRANSFORM)
  {
    icColorSpaceSignature csp_in = 0, csp_out = 0;

    csp_in = (icColorSpaceSignature) l2cmsGetColorSpace( lps[0] );

    if(profiles_n > 1)
      csp_out = (icColorSpaceSignature) l2cmsGetColorSpace( lps[profiles_n-1] );
    else
      csp_out = (icColorSpaceSignature) l2cmsGetPCS( lps[profiles_n-1] );

    block = l2cmsPassThroughDL( csp_in, csp_out, size, allocateFunc );
    error = !block;

    if(oy_debug || verbose)
      l2cms_msg( block?oy_debug?oyMSG_DBG:oyMSG_WARN:oyMSG_ERROR,NULL, OY_DBG_FORMAT_
                 "skipping on cmsFLAGS_NULLTRANSFORM (precalculation=4 NOTRANSFORM) %s",
                 OY_DBG_ARGS_, block?"use pass_through device link":"failed" );
  }
  else
  {
    xform = l2cmsCMMConversionContextCreate_( node, lps, profiles_n,
                                           profiles, profiles_simulation_n, proof,
                                oyImage_GetPixelLayout( image_input, oyLAYOUT ),
                                oyImage_GetPixelLayout( image_output, oyLAYOUT ),
                                           node_options, 0, 0, verbose );

    error = !xform;
  }

  if(oy_debug > 3)
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_"\n%s",
              OY_DBG_ARGS_,
              oyFilterNode_GetText( node, oyNAME_NAME ) );
  else
    if(oy_debug || verbose)
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_"start oyDL %d",
              OY_DBG_ARGS_, error );

  if(!block && xform)
  {
    if(oy_debug)
      block = l2cmsCMMColorConversion_ToMem_( xform, node_options,
                                              size, oyAllocateFunc_ );
    else
      block = l2cmsCMMColorConversion_ToMem_( xform, node_options,
                                              size, allocateFunc );
    error = !block || !*size;
    l2cmsDeleteTransform( xform ); xform = 0;
    if(oy_debug || verbose)
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)node, OY_DBG_FORMAT_"created oyDL %d",
              OY_DBG_ARGS_, error );
  } else if(!block)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_"\n"
              "loading failed profiles_n:%d profiles_simulation_n:%d profiles:%d",
              OY_DBG_ARGS_,
              profiles_n, profiles_simulation_n, oyProfiles_Count(profiles) );
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_"\n"
              "  input profile: \"%s\" %s %s->%s %s\n  %s",
              OY_DBG_ARGS_,
              oyProfile_GetText( image_input_profile, oyNAME_DESCRIPTION ),
              oyProfile_GetText( image_input_profile, oyNAME_NAME ),
              oyICCColorSpaceGetName( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_COLOR_SPACE ) ),
              oyICCColorSpaceGetName( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_PCS ) ),
              oyICCDeviceClassDescription( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_CLASS ) ),
              oyPixelPrint(oyImage_GetPixelLayout( image_input, oyLAYOUT ), malloc));
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)node, OY_DBG_FORMAT_"\n"
              "  output profile: \"%s\" %s %s->%s %s\n  %s",
              OY_DBG_ARGS_,
              oyProfile_GetText( image_input_profile, oyNAME_DESCRIPTION ),
              oyProfile_GetText( image_output_profile, oyNAME_NAME ),
              oyICCColorSpaceGetName( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_COLOR_SPACE ) ),
              oyICCColorSpaceGetName( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_PCS ) ),
              oyICCDeviceClassDescription( oyProfile_GetSignature( 
                            image_input_profile, oySIGNATURE_CLASS ) ),
              oyPixelPrint(oyImage_GetPixelLayout( image_output, oyLAYOUT ), malloc));
  }

  /* additional tags for debugging */
  if(!error && (oy_debug || verbose || getenv("OY_DEBUG_WRITE")))
  {
    if(!error)
    {
      size_ = *size;

      if(!size_)
      {
        block = NULL;
        goto l2cmsFilterNode_CmmIccContextToMemClean;
      }

      prof = oyProfile_FromMem( size_, block, 0, 0 );

      /* icSigProfileSequenceIdentifierType */
      {
        oyStructList_s * list = oyStructList_New(0);
        int i, n = oyProfiles_Count( profs );
        for( i = 0; i < n ; ++i )
        {
          oyProfile_s * p = oyProfiles_Get( profs, i );
          oyStructList_MoveIn( list, (oyStruct_s**) &p, -1, 0 );
        }
        psid = oyProfileTag_Create( list, icSigProfileSequenceIdentifierTag,
                                 icSigProfileSequenceIdentifierType, 0, 0 );
        if(psid)
          error = oyProfile_TagMoveIn ( prof, &psid, -1 );

        oyStructList_Release( &list );
      }

      /* Info tag */
      if(!error)
      {
        oyStructList_s * list = oyStructList_Create( oyOBJECT_NONE, "l2cmsFilterNode_CmmIccContextToMem()", 0);
        char h[5] = {"Info"};
        uint32_t * hi = (uint32_t*)&h;
        char * cc_name = l2cmsFilterNode_GetText( node, oyNAME_NICK,
                                                  oyAllocateFunc_ );
        const char * lib_name = oyFilterNode_GetModuleName( node );

        oyStructList_MoveInName( list, &cc_name, 0, oyNAME_NAME );
        oyStructList_AddName( list, lib_name, 0, oyNAME_NICK );

        info = oyProfileTag_Create( list, (icTagSignature)oyValueUInt32(*hi),
                                    icSigTextType, 0, 0);

        oyStructList_Release( &list );

        if(info)
          error = oyProfile_TagMoveIn ( prof, &info, -1 );
      }

      if(!error)
        cprt = oyProfile_GetTagById( prof, icSigCopyrightTag );

      /* icSigCopyrightTag */
      if(!error && !cprt)
      {
        oyStructList_s * list = oyStructList_New(0);
        error = oyStructList_AddName( list, "no copyright; use freely", -1, oyNAME_NAME );

        if(!error)
        {
          cprt = oyProfileTag_Create( list, icSigCopyrightTag,
                                      icSigTextType, 0, 0);
          error = !cprt;
        }

        oyStructList_Release( &list );

        if(!error)
          error = oyProfile_TagMoveIn ( prof, &cprt, -1 );
      }

      if(block)
      { oyDeAllocateFunc_( block ); block = 0; size_ = 0; }

      block = oyProfile_GetMem( prof, &size_, 0, allocateFunc );
      if(getenv("OY_DEBUG_WRITE"))
      {
        char * t = 0; oyjlStringAdd( &t, 0,0,
        "%04d-%s-dl[%d].icc", ++oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)node));
        l2cms_msg( oy_debug?oyMSG_DBG:oyMSG_WARN, (oyStruct_s*)prof, OY_DBG_FORMAT_
                  "wrote %d to %s", OY_DBG_ARGS_, oyjlWriteFile( t, block, size_ ), t );
        oyFree_m_(t);
      }

      *size = size_;
      oyProfile_Release( &prof );
    }
  }

l2cmsFilterNode_CmmIccContextToMemClean:
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( & src_socket );
  oyOptions_Release( &node_tags );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyProfile_Release( &image_input_profile );
  oyProfile_Release( &image_output_profile );
  oyOptions_Release( &node_options );
  oyProfiles_Release( &profs );
  oyProfiles_Release( &profiles );
  oyFree_m_( lps );

  if(verbose || oy_debug)
    l2cms_msg( oyMSG_DBG,(oyStruct_s*)node, OY_DBG_FORMAT_
                "finished", OY_DBG_ARGS_);
  return block;
}

char * l2cmsImage_GetText            ( oyImage_s         * image,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc )
{
  oyPixel_t pixel_layout = oyImage_GetPixelLayout(image,oyLAYOUT); 
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = oyImage_GetProfile( image );
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColorOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColorChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  int so = oyDataTypeGetSize( t );
  char * text = oyAllocateFunc_(512);
  char * hash_text = 0;
  oyImage_s * s = image;

  /* describe the image */
  if(type == oyNAME_DESCRIPTION)
    oySprintf_( text,   "  {\n  \"oyImage_s\": {\n    \"oyProfile_s\": %s", oyjlTermColorToPlain(oyProfile_GetText(profile, oyNAME_JSON)));
  else
    oySprintf_( text,   "%s", oyjlTermColorToPlain(oyProfile_GetText(profile, oyNAME_JSON)));
  hashTextAdd_m( text );
  if(type == oyNAME_DESCRIPTION)
  {
    oySprintf_( text,",\n    \"channels\": {\n      \"all\": \"%d\",\n      \"color\": \"%d\"\n    },\n", n,cchan_n);
    hashTextAdd_m( text );
    oySprintf_( text,
                        "    \"offsets first_color_sample\": \"%d\",\n    \"next_pixel\": \"%d\",\n"
              /*"  next line = %d\n"*/,
              coff_x, oyImage_GetPixelLayout( s,oyPOFF_X )/*, mask[oyPOFF_Y]*/ );
    hashTextAdd_m( text );

    if(swap || oyToByteswap_m( pixel_layout ))
    {
      hashTextAdd_m(    "    \"swap\": {" );
      if(swap)
        hashTextAdd_m(  "      \"colorswap\": \"yes\",\n" );
      if( oyToByteswap_m( pixel_layout ) )
        hashTextAdd_m(  "      \"byteswap\": \"yes\"\n" );
      hashTextAdd_m(    "    },\n" );
    }

    if( oyToFlavor_m( pixel_layout ) )
    {
      oySprintf_( text, ",\n    \"flawor\": {\n      \"value\": \"yes\"\n    },\n" );
      hashTextAdd_m( text );
    }
    oySprintf_( text,   "    \"sample\": {\n      \"type\": \"%s\",\n      \"bytes\": \"%d\"\n    }\n",
                    oyDataTypeToText(t), so );
    hashTextAdd_m( text );
    oySprintf_( text,     "  }\n}");
    hashTextAdd_m( text );
  }

  oyDeAllocateFunc_(text);

  if(allocateFunc != oyStruct_GetAllocator((oyStruct_s*)s))
  {
    text = hash_text;
    hash_text = oyjlStringCopy( text, allocateFunc );
    oySTRUCT_FREE_m( s, text );
  }
  text = 0;
  oyProfile_Release( &profile );

  return hash_text;
}

/** Function l2cmsFilterNode_GetText
 *  @brief   implement oyCMMFilterNode_GetText_f()
 *
 *  provides a description in JSON format
 *
 *  param type oyNAME_NAME
 *             - oyNAME_NICK suitable for api4 hash ID;
 *             - oyNAME_DESCRIPTION more details for api7 hash ID
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/21
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 */
char * l2cmsFilterNode_GetText       ( oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc )
{
#ifdef NO_OPT
  return oyjlStringCopy( oyFilterNode_GetText( node, type ), allocateFunc );
#else
  const char * model = NULL;
  char * hash_text = NULL,
       * temp = NULL;
  oyFilterNode_s * s = node;

  oyImage_s * in_image = 0,
            * out_image = 0;
  int verbose;
  oyOptions_s * node_opts = oyFilterNode_GetOptions( node, 0 );
  oyOptions_s * node_tags = oyFilterNode_GetTags( node ),
              * opts_tmp, * opts_tmp2, * options;
  oyFilterCore_s * node_core = oyFilterNode_GetCore( node );
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * src_socket = oyFilterPlug_GetSocket( plug );
  oyProfiles_s * profiles;
  oyProfile_s * p;
  int effect_switch, proof, profiles_display_n, i,n;

  /* pick all plug (input) data */
  in_image = (oyImage_s*)oyFilterSocket_GetData( src_socket );
  /* pick all socket (output) data */
  out_image = (oyImage_s*)oyFilterSocket_GetData( socket );

  if(!node)
    return 0;

  verbose = oyOptions_FindString( node_tags, "verbose", "true" ) ? 1 : 0;

  /* 1. create hash text */
  hashTextAdd_m( "{ \"node\": " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( node_core, oyNAME_JSON ) );
  hashTextAdd_m( ",\n" );

  /* make a description */
  {
    /* input data */
    if(type == oyNAME_DESCRIPTION)
      hashTextAdd_m(   " \"data_in\": " );
    else
      hashTextAdd_m(   " \"icc_profile.in\": " );
    if(in_image)
    {
      temp = l2cmsImage_GetText( in_image, type, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyDeAllocateFunc_(temp); temp = 0;
    }
    hashTextAdd_m( ",\n" );

    /* pick inbuild defaults */
    opts_tmp2 = oyOptions_FromText( l2cms_extra_options, 0, NULL );
    opts_tmp = oyOptions_ForFilter( "//" OY_TYPE_STD "/icc_color",
                                oyOPTIONSOURCE_FILTER | OY_SELECT_COMMON , 0 );
    options = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION,NULL);
    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );
    opts_tmp = options;
    /* add existing custom options */
    options = oyOptions_FromBoolean( opts_tmp, node_opts, oyBOOLEAN_UNION,NULL);
    oyOptions_Release( &opts_tmp );

    /* options -> xforms */
    model = oyOptions_GetText( options, oyNAME_JSON );
    if(model)
    {
      hashTextAdd_m(   " \"model\": " );
      hashTextAdd_m( model );
      hashTextAdd_m( ",\n" );
    }
    oyOptions_Release( &options );

    /* abstract profiles */
    proof = oyOptions_FindString  ( node_opts, "proof_soft", "1" ) ? 1 : 0;
    proof += oyOptions_FindString  ( node_opts, "proof_hard", "1" ) ? 1 : 0;
    effect_switch = oyOptions_FindString  ( node_opts, "effect_switch", "1" ) ? 1 : 0;
    /* display profile */
    profiles_display_n = oyOptions_CountType( node_opts, "display.abstract.icc_profile", oyOBJECT_PROFILE_S );
    profiles = l2cmsProfilesFromOptions( node, plug, node_opts, "profiles_effect", effect_switch, verbose );
    n = oyProfiles_Count( profiles );
    for(i = 0; i < n; ++i)
    {
      p = oyProfiles_Get( profiles, i );
      model = oyjlTermColorToPlain(oyProfile_GetText( p, oyNAME_JSON ));
      oyProfile_Release( &p );

      if(i==0)
        hashTextAdd_m(  " \"icc_profile.effect.abstract\": [\n" );
      else
        hashTextAdd_m(    ",\n  " );
      hashTextAdd_m( model );
      if(i+1 == n)
        hashTextAdd_m(  " ],\n" );
    }
    oyProfiles_Release( &profiles );

    for(i = 0; i < profiles_display_n; ++i)
    {
      oyOption_s * o = NULL;
      oyOptions_GetType2( node_opts, i, "display.abstract.icc_profile", oyNAME_PATTERN,
                                  oyOBJECT_PROFILE_S, NULL, &o );
      p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
      oyOption_Release( &o );
      model = oyjlTermColorToPlain(oyProfile_GetText( p, oyNAME_JSON ));
      oyProfile_Release( &p );

      if(i==0)
        hashTextAdd_m(  " \"icc_profile.display.abstract\": [\n" );
      else
        hashTextAdd_m(    ",\n  " );
      hashTextAdd_m( model );
      if(i+1 == profiles_display_n)
        hashTextAdd_m(  " ],\n" );
    }

    /* output data */
    if(type == oyNAME_DESCRIPTION)
      hashTextAdd_m(   " \"data_out\": " );
    else
      hashTextAdd_m(   " \"icc_profile.out\": " );
    if(out_image)
    {
      temp = l2cmsImage_GetText( out_image, type, oyAllocateFunc_ );
      hashTextAdd_m( temp );
      oyFree_m_(temp);
    }
    hashTextAdd_m( "\n}\n" );
  }

  /* add hash in the first line */
  if(hash_text)
  {
    oyjl_val root = oyJsonParse( hash_text );
    unsigned char hash[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint32_t * h = (uint32_t*)&hash;

    temp = oyJsonPrint( root );
    oyMiscBlobGetMD5_(temp, strlen(temp), hash);
    oySprintf_( hash_text, "%08x%08x%08x%08x\n", h[0], h[1], h[2], h[3] );
    hashTextAdd_m( temp );

    oyFree_m_(temp);
    oyjlTreeFree( root );
    if(allocateFunc != oyAllocateFunc_)
    {
      temp = hash_text;
      hash_text = oyjlStringCopy( temp, allocateFunc );
      oyFree_m_(temp);
    }
  }

  oyOptions_Release( &node_opts );
  oyOptions_Release( &node_tags );
  oyFilterCore_Release( &node_core );
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( &src_socket );
  oyImage_Release( &in_image );
  oyImage_Release( &out_image );

  return hash_text;
#endif
}

/** Function l2cmsFlagsToText
 *  @brief   debugging helper
 *
 *  @version Oyranos: 0.1.13
 *  @since   2010/11/28 (Oyranos: 0.1.13)
 *  @date    2010/11/28
 */
char * l2cmsFlagsToText               ( int                 flags )
{
  char * t = 0;
  char num[24];

  sprintf(num, "%d", flags);
  STRING_ADD( t, "flags[" );
  STRING_ADD( t, num );
  STRING_ADD( t, "]: " );
#define STRING_ADD_FLAG( flag_name ) \
  if(flags & flag_name) \
    STRING_ADD( t, " " #flag_name );
  STRING_ADD_FLAG( cmsFLAGS_NOCACHE );
  STRING_ADD_FLAG( cmsFLAGS_NOOPTIMIZE );
  STRING_ADD_FLAG( cmsFLAGS_NULLTRANSFORM );
  STRING_ADD_FLAG( cmsFLAGS_GAMUTCHECK );
  STRING_ADD_FLAG( cmsFLAGS_SOFTPROOFING );
  STRING_ADD_FLAG( cmsFLAGS_BLACKPOINTCOMPENSATION );
  STRING_ADD_FLAG( cmsFLAGS_NOWHITEONWHITEFIXUP );
  STRING_ADD_FLAG( cmsFLAGS_HIGHRESPRECALC );
  STRING_ADD_FLAG( cmsFLAGS_LOWRESPRECALC );
  STRING_ADD_FLAG( cmsFLAGS_8BITS_DEVICELINK );
  STRING_ADD_FLAG( cmsFLAGS_GUESSDEVICECLASS );
  STRING_ADD_FLAG( cmsFLAGS_KEEP_SEQUENCE );
  STRING_ADD_FLAG( cmsFLAGS_FORCE_CLUT );
  STRING_ADD_FLAG( cmsFLAGS_CLUT_POST_LINEARIZATION );
  STRING_ADD_FLAG( cmsFLAGS_CLUT_PRE_LINEARIZATION );
  return t;
}

/** Function l2cmsModuleData_Convert
 *  @brief   Convert a ICC device link to LittleCMS 2 color transform
 *  @ingroup cmm_handling
 *
 *  The function might be used to provide a module specific context.
 *  Implements oyModuleData_Convert_f
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int  l2cmsModuleData_Convert          ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node )
{
  int error = !data_in || !data_out;
  oyPointer_s * cmm_ptr_in = data_in,
             * cmm_ptr_out = data_out;
  l2cmsTransformWrap_s * ltw  = 0;
  cmsHTRANSFORM xform = 0;
  cmsHPROFILE lps[2] = {0,0};
  oyFilterPlug_s * plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, 0 ),
                   * src_socket = oyFilterPlug_GetSocket( plug );
  oyOptions_s * node_options = oyFilterNode_GetOptions( node, 0 ),
              * node_tags = oyFilterNode_GetTags( node );
  oyImage_s * image_input = (oyImage_s*)oyFilterSocket_GetData( src_socket ),
            * image_output = (oyImage_s*)oyFilterSocket_GetData( socket );
  int verbose = oyOptions_FindString( node_tags, "verbose", "true" ) ? 1 : 0;

  if(!error)
  {
    cmm_ptr_in = (oyPointer_s*) data_in;
    cmm_ptr_out = (oyPointer_s*) data_out;
  }

  if(!error &&
     ( (strcmp( oyPointer_GetResourceName(cmm_ptr_in), oyCOLOR_ICC_DEVICE_LINK ) != 0) ||
       (strcmp( oyPointer_GetResourceName(cmm_ptr_out), l2cmsTRANSFORM ) != 0) ) )
    error = 1;

  if(!error)
  {
#if LCMS_VERSION < 2060
    lps[0] = CMMProfileOpen_M( node, oyPointer_GetPointer(cmm_ptr_in),
                               oyPointer_GetSize( cmm_ptr_in) );
#else
    {
      oyFilterNode_s * node2 = oyFilterNode_Copy( node, NULL );
      cmsContext tc = l2cmsCreateContext( NULL, node2 ); /* threading context */
      l2cmsSetLogErrorHandlerTHR( tc, l2cmsErrorHandlerFunction );
      lps[0] = CMMProfileOpen_M( tc, oyPointer_GetPointer(cmm_ptr_in),
                                 oyPointer_GetSize( cmm_ptr_in) );
    }
#endif
    xform = l2cmsCMMConversionContextCreate_( node, lps, 1, 0,0,0,
                                oyImage_GetPixelLayout( image_input, oyLAYOUT ),
                                oyImage_GetPixelLayout( image_output,oyLAYOUT ),
                                           node_options,
                                           &ltw, cmm_ptr_out, verbose );

    if(oy_debug > 4)
    {
      oyProfile_s *p = oyProfile_FromMem( oyPointer_GetSize( cmm_ptr_in),
                                          oyPointer_GetPointer(cmm_ptr_in),0,0);
      uint32_t id[8]={0,0,0,0,0,0,0,0};
      char * hash_text = oyjlStringCopy( l2cmsTRANSFORM ":", oyAllocateFunc_ );

      char * t = l2cmsFilterNode_GetText( node, oyNAME_DESCRIPTION, oyAllocateFunc_ );
      STRING_ADD( hash_text, t );
      oyFree_m_(t);

      oyMiscBlobGetHash_((void*)hash_text, oyStrlen_(hash_text), 0,
                         (unsigned char*)id);
      oyjlStringAdd( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                          "node: %d hash: %08x%08x%08x%08x",
                          oyStruct_GetId((oyStruct_s*)node),
                          id[0],id[1],id[2],id[3] );

      oyProfile_GetMD5( p, OY_COMPUTE, id );
      oyjlStringAdd( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                          " oyDL: %08x%08x%08x%08x", id[0],id[1],id[2],id[3] );
      
      if(oy_debug >= 1)
      l2cms_msg( oyMSG_DBG,(oyStruct_s*) node, OY_DBG_FORMAT_"oyDL: %08x%08x%08x%08x %s %s",
                OY_DBG_ARGS_, id[0],id[1],id[2],id[3], t, hash_text );

      oyPointer_SetId( cmm_ptr_out, t );

      oyProfile_Release( &p );
      oyFree_m_(t);
    }

    if(!xform)
    {
      uint32_t f = oyImage_GetPixelLayout( image_input, oyLAYOUT );
      l2cms_msg( oyMSG_WARN,(oyStruct_s*) node, OY_DBG_FORMAT_
      "float:%d optimised:%d colorspace:%d extra:%d channels:%d lcms_bytes%d",
      OY_DBG_ARGS_,
      T_FLOAT(f), T_OPTIMIZED(f), T_COLORSPACE(f), T_EXTRA(f), T_CHANNELS(f), T_BYTES(f) );
      error = 1;
    }
    {
#if LCMS_VERSION >= 2060
      cmsContext tc = l2cmsGetProfileContextID( lps[0] );
      oyFilterNode_s * node = l2cmsGetContextUserData( tc );
      oyFilterNode_Release( &node );
      l2cmsDeleteContext( tc );
#endif
      CMMProfileRelease_M (lps[0] );
    }
  }
  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterSocket_Release( & src_socket );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyOptions_Release( &node_options );
  oyOptions_Release( &node_tags );

  return error;
}

char * oyCMMCacheListPrint_();

/** Function l2cmsFilterPlug_CmmIccRun
 *  @brief   implement oyCMMFilterPlug_GetNext_f()
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2011/06/17
 */
int      l2cmsFilterPlug_CmmIccRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int j, k, n;
  int error = 0;
  oyDATATYPE_e data_type_in = oyUINT8,
               data_type_out = oyUINT8;
  int channels_out, channels_in;
  int bps_in;
  oyPixel_t pixel_layout_in,
            layout_out;

  oyFilterSocket_s * socket = oyFilterPlug_GetSocket( requestor_plug );
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * input_node,
                 * node = oyFilterSocket_GetNode( socket );
  oyImage_s * image_input = 0, * image_output = 0;
  oyArray2d_s * array_in = 0, * array_out = 0;
  l2cmsTransformWrap_s * ltw  = 0;
  oyPixelAccess_s * new_ticket = ticket;

  plug = oyFilterNode_GetPlug( node, 0 );
  input_node = oyFilterNode_GetPlugNode( node, 0 );

  image_input = oyFilterPlug_ResolveImage( plug, socket, ticket );
  pixel_layout_in = oyImage_GetPixelLayout( image_input, oyLAYOUT );
  channels_in = oyToChannels_m( pixel_layout_in );

  image_output = oyPixelAccess_GetOutputImage( ticket );
  layout_out = oyImage_GetPixelLayout( image_output, oyLAYOUT );
  channels_out = oyToChannels_m( layout_out );
  if(!channels_out)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)ticket, OY_DBG_FORMAT_"layout_out %s channels %d",
               OY_DBG_ARGS_, oyPixelPrint(layout_out,malloc), channels_out );
    channels_out = 1;
  }

  if(oyImage_GetPixelLayout( image_input, oyLAYOUT ) != 
     oyImage_GetPixelLayout( image_output, oyLAYOUT ))
  {
    /* create a new ticket to avoid pixel layout conflicts */
    /* keep old ticket array dimensions */
    oyArray2d_s * a,
                * old_a = oyPixelAccess_GetArray( new_ticket );
    new_ticket = oyPixelAccess_Copy( ticket, ticket->oy_ );
    oyPixelAccess_SetOutputImage( new_ticket, image_input );

    /* remove old array as it's layout does not fit */
    oyPixelAccess_SetArray( new_ticket, 0, 0 );
    /* should be empty */
    a = oyPixelAccess_GetArray( new_ticket );
    if(!a)
    {
      /* Use original pixel size for being save and do not fiddle with ROI's */
      int w = oyArray2d_GetDataGeo1( old_a, 2 ) / channels_out;
      int h = oyArray2d_GetDataGeo1( old_a, 3 );
      a = oyArray2d_Create( NULL, w * channels_in,h, oyToDataType_m( pixel_layout_in ), ticket->oy_ );
      if(oy_debug)
      {
        l2cms_msg( oy_debug?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_"layout_out(%d) != layout_in(%d) created %s",
                OY_DBG_ARGS_, layout_out, pixel_layout_in,
                oyArray2d_Show( a, channels_in ));
      }
    }
    oyArray2d_Release( &old_a );
    oyPixelAccess_SetArray( new_ticket, a, 0 );
    oyArray2d_Release( &a );

    oyPixelAccess_SynchroniseROI( new_ticket, ticket );

    if(oy_debug)
      l2cms_msg( oy_debug?oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_"new_ticket %s",
                OY_DBG_ARGS_,
                oyPixelAccess_Show( new_ticket ));
  }

  /* We let the input filter do its processing first. */
  error = oyFilterNode_Run( input_node, plug, new_ticket );
  if(error != 0)
  {
    l2cms_msg( oyMSG_ERROR, (oyStruct_s*)input_node, OY_DBG_FORMAT_"%s %d err:%d",
                OY_DBG_ARGS_, _("running new ticket failed"),
                oyStruct_GetId( (oyStruct_s*)new_ticket ), error );
    return error;
  }

  array_in = oyPixelAccess_GetArray( new_ticket );
  array_out = oyPixelAccess_GetArray( ticket );
  if(oy_debug > 2)
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)new_ticket, OY_DBG_FORMAT_"%s %cnew_ticket->array:%s %s[%d]",
              OY_DBG_ARGS_,_("Read from"), oyPixelAccess_ArrayIsFocussed( new_ticket )?' ':'~',
              oyArray2d_Show( array_in, channels_in ),
              _("Image"), oyStruct_GetId( (oyStruct_s*)image_input ) );
  if(oy_debug > 2)
    l2cms_msg( oyMSG_DBG, (oyStruct_s*)ticket, OY_DBG_FORMAT_"%s %cticket->array:%s %s[%d]",
              OY_DBG_ARGS_,_("Write to"), oyPixelAccess_ArrayIsFocussed( ticket )?' ':'~',
              oyArray2d_Show( array_out, channels_out ),
              _("Image"), oyStruct_GetId( (oyStruct_s*)image_output ) );

  data_type_in = oyToDataType_m( oyImage_GetPixelLayout( image_input, oyLAYOUT ) );
  bps_in = oyDataTypeGetSize( data_type_in );

  /*if(data_type_in == oyHALF)
  {
    oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
    l2cms_msg(oyMSG_WARN,(oyStruct_s*)ticket, OY_DBG_FORMAT_" can not handle oyHALF",OY_DBG_ARGS_);
    error = 1;
  }*/

  if(!image_output)
  {
    l2cms_msg( oyMSG_WARN,(oyStruct_s*)ticket, OY_DBG_FORMAT_ " no ticket->output_image",
             OY_DBG_ARGS_);
    error = 1;
  }

  if(!error)
  {
    oyPointer_s * backend_data = oyFilterNode_GetContext( node );
    data_type_out = oyToDataType_m( oyImage_GetPixelLayout( image_output, oyLAYOUT ) );

    /* get transform */
    error = l2cmsCMMTransform_GetWrap_( backend_data, &ltw );
    if(oy_debug >= 2 && ltw)
      l2cms_msg( oyMSG_DBG, NULL, OY_DBG_FORMAT_
             " xform: " OY_PRINT_POINTER
             " ltw: " OY_PRINT_POINTER
             " backend_data: %d",
             OY_DBG_ARGS_, ltw->l2cms, ltw, oyStruct_GetId((oyStruct_s*)backend_data) );

    if(oy_debug > 4)
    /* verify context */
    {
      int msg_type = oyMSG_DBG;
      uint32_t id[8]={0,0,0,0,0,0,0,0};
      char * hash_text = oyjlStringCopy( l2cmsTRANSFORM ":", oyAllocateFunc_ );

      char * t = 0;
      t = l2cmsFilterNode_GetText( node, oyNAME_DESCRIPTION, oyAllocateFunc_ );
      STRING_ADD( hash_text, t );
      oyFree_m_(t);

      oyMiscBlobGetHash_((void*)hash_text, oyStrlen_(hash_text), 0,
                         (unsigned char*)id);
      oyjlStringAdd( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                          "hash: %08x%08x%08x%08x",
                          id[0],id[1],id[2],id[3] );

      /* check if we obtained the context from our
       * l2cms_api4_cmm::l2cmsFilterNode_CmmIccContextToMem */
      if(oyPointer_GetFuncName( backend_data ) &&
         strstr(oyPointer_GetLibName( backend_data ),CMM_NICK) &&
         /* check if context and actual options match */
         oyPointer_GetId( backend_data ) &&
         !strstr(oyPointer_GetId( backend_data ),t))
      {
        /* send error message */
        error = 1;
        msg_type = oyMSG_ERROR;

        l2cms_msg( msg_type, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                  "requested and actual contexts differ by hash",OY_DBG_ARGS_ );
      }

      if(error || oy_debug > 4)
        l2cms_msg( msg_type, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                  "node: %d \"%s\" (context %s)\nwant: %s\n%s", OY_DBG_ARGS_,
                  oyStruct_GetId((oyStruct_s*)node), t,
                  oyNoEmptyString_m_(oyPointer_GetId( backend_data )),
                  oy_debug > 0 && error > 0 ? hash_text : "----",
                  oy_debug > 0 && error > 0 ? oyCMMCacheListPrint_() : "" );
      if(oy_debug > 4 && error < 1)
        l2cms_msg( msg_type, (oyStruct_s*)ticket, OY_DBG_FORMAT_
                  "%s", OY_DBG_ARGS_, hash_text );

      oyFree_m_(hash_text);
      oyFree_m_(t);
    }
    oyPointer_Release( &backend_data );
  }

  DBGs_PROG4_S( ticket, "channels in/out: %d[%d]->%d[%d]",
                channels_in, oyStruct_GetId((oyStruct_s*)image_input),
                channels_out, oyStruct_GetId((oyStruct_s*)image_output) );

  if(ltw && !array_out)
  {
    l2cms_msg( oyMSG_ERROR,(oyStruct_s*)ticket, OY_DBG_FORMAT_ " no ticket->array",
             OY_DBG_ARGS_);
    error = 1;
  }

  /* now do some position blind manipulations */
  if(ltw && error <= 0)
  {
    uint8_t * array_in_tmp = 0,
            * array_out_tmp = 0;
    float * array_in_tmp_flt = 0,
          * array_out_tmp_flt = 0;
    double * array_in_tmp_dbl = 0,
           * array_out_tmp_dbl = 0;
    uint8_t ** array_in_data = oyArray2d_GetData( array_in ),
            ** array_out_data = oyArray2d_GetData( array_out );
    int threads_n = 
#if defined(_OPENMP) && defined(USE_OPENMP)
                    omp_get_max_threads();
#else
                    1;
#endif
    int w_in =  (int)(oyArray2d_GetWidth(array_in)+0.5),
        w_out = (int)(oyArray2d_GetWidth(array_out)+0.5);
    int stride_in = w_in * bps_in;

    n = OY_MIN(w_in/channels_in, w_out/channels_out);

    if(oy_debug)
    {
      char * tmp = oyjlStringCopy( oyArray2d_Show(array_in,channels_in), NULL );
      l2cms_msg( oyMSG_DBG,(oyStruct_s*)ticket, OY_DBG_FORMAT_
             " %s[%d]=\"%s\" threads_n: %d %s: " OY_PRINT_POINTER
             " -> %s: " OY_PRINT_POINTER " convert pixel: %d xform: " OY_PRINT_POINTER,
             OY_DBG_ARGS_,
             _("Node"),oyStruct_GetId((oyStruct_s*)node),oyStruct_GetInfo((oyStruct_s*)node,0,0),
             threads_n,
             tmp,array_in_data,
             oyArray2d_Show(array_out,channels_out),array_out_data,n,
             ltw->l2cms );
      oyFree_m_(tmp);
    }

    if(!(data_type_in == oyUINT8 ||
         data_type_in == oyUINT16 ||
         data_type_in == oyHALF ||
         data_type_in == oyFLOAT ||
         data_type_in == oyDOUBLE))
    {
      oyFilterSocket_Callback( requestor_plug, oyCONNECTOR_EVENT_INCOMPATIBLE_DATA );
      error = 1;
    }
    
    if(ltw->sig_in  == icSigXYZData &&
       (data_type_in == oyFLOAT ||
        data_type_in == oyDOUBLE))
    {
      array_in_tmp = oyAllocateFunc_( stride_in * threads_n );
      if(data_type_in == oyFLOAT)
        array_in_tmp_flt = (float*) array_in_tmp;
      else if(data_type_in == oyDOUBLE)
        array_in_tmp_dbl = (double*) array_in_tmp;
    }
    if(ltw->sig_out  == icSigXYZData &&
       (data_type_out == oyFLOAT ||
        data_type_out == oyDOUBLE))
    {
      array_out_tmp = array_out_data[0];
    }
    

    /*  - - - - - conversion - - - - - */
    /*l2cms_msg(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d Start lines: %d",
            __FILE__,__LINE__, array_out->height);*/
    if(!error)
    {
      const double xyz_factor = 1.0 + 32767.0/32768.0;
      const int use_xyz_scale = 1;
      int index = 0;
      int array_in_height = oyArray2d_GetHeight(array_in),
          array_out_height = oyArray2d_GetHeight(array_out),
          lines = OY_MIN(array_in_height, array_out_height);
      if(lines > threads_n * 10)
      {
#if defined(USE_OPENMP)
#pragma omp parallel for private(index,j,array_in_tmp_flt,array_in_tmp_dbl,array_out_tmp_flt,array_out_tmp_dbl)
#endif
        for( k = 0; k < lines; ++k)
        {
          if(array_in_tmp && use_xyz_scale)
          {
#if defined(_OPENMP) && defined(USE_OPENMP)
            index = omp_get_thread_num();
#endif
            memcpy( &array_in_tmp[stride_in*index], array_in_data[k],
                    w_in * bps_in );
            if(data_type_in == oyFLOAT)
            {
              array_in_tmp_flt = (float*) &array_in_tmp[stride_in*index];
              for(j = 0; j < w_in; ++j)
              {
                array_in_tmp_flt[j] /= xyz_factor;
              }
            } else
            if(data_type_in == oyDOUBLE)
            {
              array_in_tmp_dbl = (double*) &array_in_tmp[stride_in*index];
              for(j = 0; j < w_in; ++j)
              {
                array_in_tmp_dbl[j] /= xyz_factor;
              }
            }
            l2cmsDoTransform( ltw->l2cms, &array_in_tmp[stride_in*index],
                                       array_out_data[k], n );
          } else
            l2cmsDoTransform( ltw->l2cms, array_in_data[k],
                                       array_out_data[k], n );
          if(array_out_tmp && use_xyz_scale)
          {
            if(data_type_out == oyFLOAT)
            {
              array_out_tmp_flt = (float*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_flt[j] *= xyz_factor;
            } else
            if(data_type_out == oyDOUBLE)
            {
              array_out_tmp_dbl = (double*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_dbl[j] *= xyz_factor;
            }
          }
        }
      } else
        for( k = 0; k < lines; ++k)
        {
          if(array_in_tmp && use_xyz_scale)
          {
            memcpy( array_in_tmp, array_in_data[k], w_in * bps_in );
            if(data_type_in == oyFLOAT)
            for(j = 0; j < w_in; ++j)
            {
              array_in_tmp_flt[j] /= xyz_factor;
            }
            if(data_type_in == oyDOUBLE)
            for(j = 0; j < w_in; ++j)
            {
              array_in_tmp_dbl[j] /= xyz_factor;
            }
            l2cmsDoTransform( ltw->l2cms, array_in_tmp,
                                       array_out_data[k], n );
          } else
            l2cmsDoTransform( ltw->l2cms, array_in_data[k],
                                       array_out_data[k], n );
          if(array_out_tmp && use_xyz_scale)
          {
            if(data_type_out == oyFLOAT)
            {
              array_out_tmp_flt = (float*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_flt[j] *= xyz_factor;
            } else
            if(data_type_out == oyDOUBLE)
            {
              array_out_tmp_dbl = (double*) array_out_data[k];
              for(j = 0; j < w_out; ++j)
                array_out_tmp_dbl[j] *= xyz_factor;
            }
          }
        }
    /*l2cms_msg(oyMSG_WARN,(oyStruct_s*)ticket, "%s: %d End width: %d",
            __FILE__,__LINE__, n);*/
    }

    if(array_in_tmp)
      oyDeAllocateFunc_( array_in_tmp );

    if(oy_debug && getenv("OY_DEBUG_WRITE"))
    {
      char * t = 0; oyjlStringAdd( &t, 0,0,
      "%04d-%s-array_in[%d].ppm", ++oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)array_in));
      oyArray2d_ToPPM_( (oyArray2d_s_*)array_in, t );
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_ "wrote debug image to: %s",
                 OY_DBG_ARGS_, t );
      t[0] = '\000'; oyjlStringAdd( &t, 0,0,
      "%04d-%s-array_out[%d].ppm", oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)array_out));
      oyArray2d_ToPPM_( (oyArray2d_s_*)array_out, t );
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_ "wrote debug image to: %s",
                 OY_DBG_ARGS_, t );
      t[0] = '\000'; oyjlStringAdd( &t, 0,0,
      "%04d-%s-node[%d]-array_out[%d]%dc.ppm", oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)node),oyStruct_GetId((oyStruct_s*)array_out),channels_out);
      {
        oyProfile_s * p = oyImage_GetProfile( image_output );
        oyImage_s * img = oyImage_Create( oyArray2d_GetWidth(array_out)/channels_out, oyArray2d_GetHeight(array_out),NULL,
                                          oyImage_GetPixelLayout( image_output, oyLAYOUT ), p, NULL );
        oyImage_ReadArray( img, NULL, array_out, NULL );
        oyImage_WritePPM( img, t, t );
        oyProfile_Release( &p );
        oyImage_Release( &img );
      }
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)ticket,
                 OY_DBG_FORMAT_ "wrote debug image to: %s",
                 OY_DBG_ARGS_, t );
      oyFree_m_(t);
    }
  } else
  {
    if(error)
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT );
    else
      oyFilterSocket_Callback( requestor_plug,
                               oyCONNECTOR_EVENT_OK );

    error = oyPixelAccess_SetFromString( ticket,
                     "//" OY_TYPE_STD "/profile/dirty", "true", OY_CREATE_NEW );
    error = 1;
  }

  if(oyImage_GetPixelLayout( image_input, oyLAYOUT ) != 
     oyImage_GetPixelLayout( image_output, oyLAYOUT ))
    oyPixelAccess_Release( &new_ticket );

  oyFilterPlug_Release( &plug );
  oyFilterSocket_Release( &socket );
  oyFilterNode_Release( &input_node );
  oyFilterNode_Release( &node );
  oyImage_Release( &image_input );
  oyImage_Release( &image_output );
  oyArray2d_Release( &array_in );
  oyArray2d_Release( &array_out );

  return error;
}




/*
oyPointer          oyCMMallocateFunc   ( size_t            size )
{
  oyPointer p = 0;
  if(size)
    p = malloc(size);
  return p;
}

void               oyCMMdeallocateFunc ( oyPointer         mem )
{
  if(mem)
    free(mem);
}*/

/** Function l2cmsErrorHandlerFunction
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
void l2cmsErrorHandlerFunction       ( cmsContext          ContextID,
                                       cmsUInt32Number     ErrorCode OY_UNUSED,
                                       const char        * ErrorText )
{
  int code = 0;
#if LCMS_VERSION < 2060
  oyStruct_s * s = ContextID;
#else
  oyStruct_s * s = ContextID ? l2cmsGetContextUserData( ContextID ) : NULL;
#endif
  code = oyMSG_ERROR;
  l2cms_msg( code, s, CMM_NICK ": %s", ErrorText );
}

/** Function l2cmsCMMMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/00
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 */
int            l2cmsCMMMessageFuncSet ( oyMessage_f         message_func )
{
  l2cms_msg = message_func;
  lcm2MessageFuncSet( message_func );
  return 0;
}

char l2cms_extra_options[] = {
"{\n\
  \"" OY_TOP_SHARED "\": {\n\
   \"" OY_DOMAIN_INTERNAL "\": {\n\
    \"" OY_TYPE_STD "\": {\n\
     \"" "icc_color" "\": {\n\
      \"cmyk_cmyk_black_preservation.advanced\": \"0\",\n\
      \"precalculation.advanced\": \"0\",\n\
      \"precalculation_curves.advanced\": \"1\",\n\
      \"adaption_state.advanced\": \"1.0\",\n\
      \"no_white_on_white_fixup.advanced\": \"1\"\n\
     }\n\
    }\n\
   }\n\
  }\n\
}\n"
};

#define A(long_text) STRING_ADD( tmp, long_text)

/** Function l2cmsGetOptionsUI
 *  @brief   return XFORMS for matching options
 *
 *  Static options.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/06/14
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 */
int l2cmsGetOptionsUI                ( oyCMMapiFilter_s   * module OY_UNUSED,
                                       oyOptions_s        * options OY_UNUSED,
                                       int                  flags OY_UNUSED,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc )
{
  char * tmp = 0;

#if 0
  tmp = oyjlStringCopy( "\
  <xf:group type=\"frame\">\
    <xf:label>little CMS 2 ", oyAllocateFunc_ );

  A(       _("Extended Options"));
  A(                         ":</xf:label>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/cmyk_cmyk_black_preservation\">\n\
      <xf:label>" );
  A(          _("Black Preservation"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Decide how to preserve the black channel for Cmyk to Cmyk transforms"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Cmyk to Cmyk transforms can provide various strategies to preserve the black only channel. None means, black might change to Cmy and thus text prints not very well. LittleCMS 2 has added two different modes to deal with that: Black-ink-only preservation and black-plane preservation. The first is simple and effective: do all the colorimetric transforms but keep only K (preserving L*) where the source image is only black. The second mode is fair more complex and tries to preserve the WHOLE K plane."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>");
  A(            _("[none]"));
  A(                 "</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>LCMS_PRESERVE_PURE_K</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>2</xf:value>\n\
        <xf:label>LCMS_PRESERVE_K_PLANE</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/precalculation\">\n\
      <xf:label>" );
  A(          _("Optimization"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Color Transforms can be differently stored internally"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Little CMS tries to optimize profile chains whatever possible. There are some built-in optimization schemes, and you can add new schemas by using a plug-in. This generally improves the performance of the transform, but may introduce a small delay of 1-2 seconds when creating the transform. If you are going to transform just few colors, you don't need this precalculations. Then, the flag cmsFLAGS_NOOPTIMIZE in cmsCreateTransform() can be used to inhibit the optimization process. See the API reference for a more detailed discussion of the flags."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>normal</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>LCMS2_NOOPTIMIZE</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>2</xf:value>\n\
        <xf:label>LCMS2_HIGHRESPRECALC</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>3</xf:value>\n\
        <xf:label>LCMS2_LOWRESPRECALC</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/precalculation_curves\">\n\
      <xf:label>" );
  A(          _("Curves for Optimization"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Color Transform CLUT's can additionally use curves for special cases"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Little CMS can use curves before and after CLUT's for special cases like gamma encoded values to and from linear gamma values. Performance will suffer."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>");
  A(            _("[none]"));
  A(                 "</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>LCMS2_POST+PRE_CURVES</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/adaption_state\">\n\
      <xf:label>" );
  A(          _("Adaptation State"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Adaptation state for absolute colorimetric intent"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("The adaption state should be between 0 and 1.0 and will apply to the absolute colorimetric intent."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0.0</xf:value>\n\
        <xf:label>0.0</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1.0</xf:value>\n\
        <xf:label>1.0</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n");
  A("\
     <xf:select1 ref=\"/" OY_TOP_SHARED "/" OY_DOMAIN_INTERNAL "/" OY_TYPE_STD "/" "icc_color/no_white_on_white_fixup\">\n\
      <xf:label>" );
  A(          _("No White on White Fix"));
  A(                              "</xf:label>\n\
      <xf:hint>" );
  A(          _("Skip White Point on White point alignment"));
  A(                              "</xf:hint>\n\
      <xf:help>" );
  A(          _("Avoid force of White on White mapping. Default for absolute rendering intent."));
  A(                              "</xf:help>\n\
      <xf:choices>\n\
       <xf:item>\n\
        <xf:value>0</xf:value>\n\
        <xf:label>");
  A(            _("No"));
  A(                 "</xf:label>\n\
       </xf:item>\n\
       <xf:item>\n\
        <xf:value>1</xf:value>\n\
        <xf:label>");
  A(            _("Yes"));
  A(                  "</xf:label>\n\
       </xf:item>\n\
      </xf:choices>\n\
     </xf:select1>\n\
   </xf:group>\n"
   );

  if(allocateFunc && tmp)
  {
    char * t = oyjlStringCopy( tmp, allocateFunc );
    oyFree_m_( tmp );
    tmp = t; t = 0;
  } else
    return 1;
#else
  tmp = oyjlStringCopy( oyranos_json, allocateFunc );
#endif

  *ui_text = tmp;

  return 0;
}


/** \addtogroup misc_modules
 *  @{ */
/** \addtogroup lcm2_misc lcm2 Module
 *  @brief      Little CMS 2 ICC style color profiles
 *
 *  The modules provide ICC style color space creation.
 *
 *  @{ */

#if 0
/* the OY_LCM2_PARSE_CGATS filter can not be implemented as described in lcm2ParseCGATS */
/* OY_LCM2_PARSE_CGATS -------------------------- */

/** @brief   TODO: Parse a CGATS text
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/11/26 (Oyranos: 0.9.7)
 *  @date    2019/02/04
 */
oyPointer_s* lcm2ParseCGATS          ( const char        * cgats )
{
  int error = !cgats;
  oyPointer_s * ptr = NULL;
  oyjl_val root = NULL;
  char ** props = NULL;
  cmsHANDLE lcgats;

  if(error) return ptr;
  cmsContext tc = l2cmsCreateContext( NULL, NULL ); /* threading context */
  lcgats = l2cmsIT8LoadFromMem(tc, cgats, strlen(cgats));
  int n = l2cmsIT8EnumProperties( lcgats, &props ), i;
  for(i = 0; i < n; ++i)
    l2cms_msg( oyMSG_DBG, NULL, OY_DBG_FORMAT_
               "Properties: %s", OY_DBG_ARGS_, props[i] );
  /* lcms has no API to analyse BEGIN_DATA_FORMAT END_DATA_FORMAT section - very limiting in order to understand the meaning of the table values */
  n = l2cmsIT8EnumPropertyMulti( lcgats, "BEGIN_DATA_FORMAT", (const char***)&props );

  /** @todo implement CGATS parsing with cmsCGATS */
  ptr = oyPointer_New(0);
  oyPointer_Set( ptr, __FILE__,
                 "oyjl_val", root, 0, 0 );

  return ptr;
}

#define OY_LCM2_PARSE_CGATS OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "parse_cgats.cgats._" CMM_NICK "._CPU"

/** @brief  l2cmsMOptions_Handle5()
 *  This function implements oyMOptions_Handle_f.
 *
 *  @param[in]     options             expects at least one options
 *                                     - "cgats": The option shall be a string.
 *  @param[in]     command             "//" OY_TYPE_STD "/parse_cgats"
 *  @param[out]    result              will contain a oyPointer_s in "data"
 *
 *  The Handler uses internally lcm2ParseCGATS().
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/11/26 (Oyranos: 0.9.7)
 *  @date    2019/02/04
 */
int          l2cmsMOptions_Handle5   ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"parse_cgats", 0))
    {
      const char * cgats = oyOptions_FindString( options, "cgats", 0 );
      if(!cgats) error = 1;
      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"parse_cgats", 0))
  {
    oyPointer_s * spec = NULL;
    const char * cgats = NULL;

    cgats = oyOptions_FindString( options, "cgats", 0 );

    spec = lcm2ParseCGATS( cgats );

    if(spec)
    {
      oyOption_s * o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "cgats.data._" CMM_NICK, 0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &spec );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "parsing creation failed",
                   OY_DBG_ARGS_ );
  }

  return 0;
}
/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/06/06 (Oyranos: 0.9.7)
 *  @date    2019/02/04
 */
const char * l2cmsInfoGetTextProfileC5(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "parse_cgats")==0)
  {
         if(type == oyNAME_NICK)
      return "parse_cgats";
    else if(type == oyNAME_NAME)
      return _("Parse CGATS text.");
    else
      return _("The littleCMS \"parse_cgats\" command lets you parse CGATS files. The filter expects a oyOption_s object with name \"cgats\" containing a string value. The result will appear in \"data\" as a oyPointer_s containing a oyjl_val.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Parse CGATS file.");
    else
      return _("The littleCMS \"parse_cgats\" command lets you parse CGATS files. See the \"parse_cgats\" info item.");
  }
  return 0;
}
const char *l2cms_texts_parse_cgats[4] = {"can_handle","parse_cgats","help",0};

/** l2cms_api10_cmm5
 *  @brief   Node for Parsing a CGATS text
 *
 *  littleCMS 2 oyCMMapi10_s implementation
 *
 *  For the front end API see oyOptions_Handle(). The backend options
 *  are described in l2cmsMOptions_Handle5().
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 *  @date    2019/02/04
 */
oyCMMapi10_s_    l2cms_api10_cmm5 = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  0,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_PARSE_CGATS,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  l2cmsInfoGetTextProfileC5,            /**< getText */
  (char**)l2cms_texts_parse_cgats,      /**<texts; list of arguments to getText*/
 
  l2cmsMOptions_Handle5                 /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_LCM2_PARSE_CGATS -------------------------- */
#endif

const char *l2cms_texts_profile_create[4] = {"can_handle","create_profile","help",0};

/* OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION -------------------------- */

/** Function lcm2AbstractWhitePointBradford
 *  @brief   create a White point correction profile with Bradford
 *
 *  Abstract profiles can easily be merged into a multi profile transform.
 *
 *  @see lcm2CreateAbstractWhitePointProfileBradford()
 *
 *  @param         src_iccXYZ          the source white point
 *  @param         src_name            source whitepoint or profile name; optional, not usedd for file_name
 *  @param         illu_iccXYZ         the illumination white point
 *  @param         illu_name           target illuminant name; optional, not usedd for file_name
 *  @param         icc_profile_flags   profile flags
 *  @param         file_name           return the file name
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/03/17
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 */
oyProfile_s* lcm2AbstractWhitePointBradford (
                                       double            * src_iccXYZ,
                                       const char        * src_name,
                                       double            * illu_iccXYZ,
                                       const char        * illu_name,
                                       double              scale,
                                       uint32_t            icc_profile_flags,
                                       char             ** file_name )
{
  int error = 0;
  cmsHPROFILE abs = NULL;
  char * my_abstract_file_name = NULL;
  double profile_version = 2.4;
  oyProfile_s * prof = NULL;

  l2cms_msg( oyMSG_DBG, NULL, OY_DBG_FORMAT_
             "XYZ %g %g %g -> %g %g %g", OY_DBG_ARGS_, src_iccXYZ[0], src_iccXYZ[1], src_iccXYZ[2], illu_iccXYZ[0], illu_iccXYZ[1], illu_iccXYZ[2] );

  if(icc_profile_flags & OY_ICC_VERSION_2)
    profile_version = 4.3;

  error = lcm2CreateAbstractWhitePointProfileBradford( src_iccXYZ, src_name,
                                                illu_iccXYZ, illu_name, &scale,
                                                profile_version, file_name?0x01:0,
                                                & my_abstract_file_name,
                                                &abs );
  if(file_name)
  {
    *file_name = my_abstract_file_name;
    return prof;
  }

  if(error || !abs)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)abs, OY_DBG_FORMAT_ " "
               "failed to build white point effect: %s",
               OY_DBG_ARGS_, oyNoEmptyString_m_(my_abstract_file_name) );
  } else
  {
    void * data;
    size_t size = 0;
    data = lcm2WriteProfileToMem( abs, &size, oyAllocateFunc_ );
    prof = oyProfile_FromMem( size, data, 0,0 );
    if(data && size) oyFree_m_( data );
  }

  if(oy_debug && getenv("OY_DEBUG_WRITE"))
  {
      char * t = 0; oyjlStringAdd( &t, 0,0,
      "%04d-%s-abstract-wtptB[%d]", ++oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)prof));
      lcm2WriteProfileToFile( abs, t, NULL,NULL );
      oyFree_m_(t);
  }

  oyFree_m_(my_abstract_file_name);
  if(abs) l2cmsCloseProfile( abs );

  return prof;
}

#define OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "create_profile.white_point_adjust.bradford.icc._" CMM_NICK "._CPU"

/** @brief  lcm2AbstractWhitePoint()
 *  This function implements oyMOptions_Handle_f.
 *
 *  @param[in]     options             expects at least two options
 *                                     - "src_iccXYZ": The option shall be a double[3] array.
 *                                     - "illu_iccXYZ": The option shall be a double[3] array.
 *                                     - "icc_profile_flags"  ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *                                     This option shall be a integer.
 *  @param[in]     command             "//" OY_TYPE_STD "/create_profile.white_point_adjust.bradford"
 *  @param[out]    result              will contain a oyProfile_s in "icc_profile.create_profile.white_point_adjust.bradford"
 *
 *  This function uses internally lcm2AbstractWhitePoint().
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/02
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 */
int          l2cmsMOptions_Handle4   ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  double  src_iccXYZ[3] = {-1,-1,-1},
         illu_iccXYZ[3] = {-1,-1,-1};

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile.white_point_adjust.bradford", 0))
    {
      error = !(oyOptions_FindDouble( options,  "src_iccXYZ", 2,  &src_iccXYZ[2] ) == 0 &&
                oyOptions_FindDouble( options, "illu_iccXYZ", 2, &illu_iccXYZ[2] ) == 0 );

      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile.white_point_adjust.bradford.file_name", 0))
  {
    int32_t icc_profile_flags = 0;
    double scale = 0.0;
    const char * src_name = oyOptions_FindString( options, "src_name", 0 );
    const char * illu_name = oyOptions_FindString( options, "illu_name", 0 );
    oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags ); 
    oyOptions_FindDouble( options, "scale", 0, &scale ); 
    char * file_name = NULL;
    if(scale != 0.0)
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "scale: %g",
                   OY_DBG_ARGS_, scale );

    if( oyOptions_FindDouble( options,  "src_iccXYZ", 0, &src_iccXYZ[0] ) == 0 &&
        oyOptions_FindDouble( options,  "src_iccXYZ", 1, &src_iccXYZ[1] ) == 0 &&
        oyOptions_FindDouble( options,  "src_iccXYZ", 2, &src_iccXYZ[2] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 0, &illu_iccXYZ[0] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 1, &illu_iccXYZ[1] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 2, &illu_iccXYZ[2] ) == 0 )
      lcm2AbstractWhitePointBradford( src_iccXYZ, src_name, illu_iccXYZ, illu_name, scale, icc_profile_flags, &file_name );

    if(file_name)
    {
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_SetFromString( result, OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION ".file_name",
                               file_name, OY_CREATE_NEW );
      if(file_name) free(file_name);
    } else
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "effect name failed",
                   OY_DBG_ARGS_ );
  }
  else if(oyFilterRegistrationMatch(command,"create_profile.white_point_adjust.bradford", 0))
  {
    oyProfile_s * p = NULL;
    const char * src_name = oyOptions_FindString( options, "src_name", 0 );
    const char * illu_name = oyOptions_FindString( options, "illu_name", 0 );
    double scale = 0.0;
    int32_t icc_profile_flags = 0;
    oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags ); 
    oyOptions_FindDouble( options, "scale", 0, &scale ); 

    if(oy_debug)
      l2cms_msg( oyMSG_DBG, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "src_name: %s -> illu_name: %s",
                   OY_DBG_ARGS_, src_name, illu_name );
    if( oyOptions_FindDouble( options,  "src_iccXYZ", 0, &src_iccXYZ[0] ) == 0 &&
        oyOptions_FindDouble( options,  "src_iccXYZ", 1, &src_iccXYZ[1] ) == 0 &&
        oyOptions_FindDouble( options,  "src_iccXYZ", 2, &src_iccXYZ[2] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 0, &illu_iccXYZ[0] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 1, &illu_iccXYZ[1] ) == 0 &&
        oyOptions_FindDouble( options, "illu_iccXYZ", 2, &illu_iccXYZ[2] ) == 0 )
      p = lcm2AbstractWhitePointBradford( src_iccXYZ, src_name, illu_iccXYZ, illu_name, scale, icc_profile_flags, NULL );

    if(p)
    {
      oyOption_s * o = oyOption_FromRegistration( OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION ".icc_profile", 0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "effect creation failed",
                   OY_DBG_ARGS_ );
  }


  return 0;
}
/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/02
 *  @since   2017/06/06 (Oyranos: 0.9.7)
 */
const char * l2cmsInfoGetTextProfile4(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "create_profile")==0)
  {
         if(type == oyNAME_NICK)
      return "white_point_adjust.bradford";
    else if(type == oyNAME_NAME)
      return _("Create a ICC white point profile.");
    else
      return _("The littleCMS \"create_profile.white_point_adjust.bradford\" command lets you create ICC abstract profiles from CIE*XYZ coordinates for white point adjustment. The filter expects a oyOption_s object with name \"src_iccXYZ\" and \"illu_iccXYZ\" each containing a double triple value in range 0.0 - 2.0. The result will appear in \"icc_profile\" with the additional attributes \"create_profile.white_point_adjust.bradford\" as a oyProfile_s object.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC abstract white point effect profile.");
    else
      return _("The littleCMS \"create_profile.white_point_adjust.bradford\" command lets you create ICC abstract profiles from a pair of CIE*XYZ coordinates. See the \"create_profile\" info item.");
  }
  return 0;
}

/** l2cms_api10_cmm4
 *  @brief   Node for Creating White Point Effect Profiles
 *
 *  littleCMS 2 oyCMMapi10_s implementation
 *
 *  For the front end API see oyOptions_Handle(). The backend options
 *  are described in l2cmsMOptions_Handle4().
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/02
 *  @since   2018/03/02 (Oyranos: 0.9.7)
 */
oyCMMapi10_s_    l2cms_api10_cmm4 = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  0,//(oyCMMapi_s*) & l2cms_api10_cmm5,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  l2cmsInfoGetTextProfile4,            /**< getText */
  (char**)l2cms_texts_profile_create,  /**<texts; list of arguments to getText*/
 
  l2cmsMOptions_Handle4                 /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_BRADFORD_REGISTRATION -------------------------- */

/* OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION -------------------------- */

/** Function lcm2AbstractWhitePoint
 *  @brief   create a White point correction profile
 *
 *  Abstract profiles can easily be merged into a multi profile transform.
 *
 *  @see lcm2CreateAbstractWhitePointProfile()
 *
 *  @param         cie_a               the white point coordinate
 *  @param         cie_b               the white point coordinate
 *  @param         icc_profile_flags   profile flags
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 *  @date    2018/01/17
 */
oyProfile_s* lcm2AbstractWhitePoint  ( double              cie_a,
                                       double              cie_b,
                                       uint32_t            icc_profile_flags )
{
  int error = 0;
  cmsHPROFILE abs = NULL;
  char * my_abstract_file_name = NULL;
  double profile_version = 2.4;
  oyProfile_s * prof = NULL;

  l2cms_msg( oyMSG_DBG, NULL, OY_DBG_FORMAT_
             "cie_ab %g %g", OY_DBG_ARGS_, cie_a, cie_b );

  if(icc_profile_flags & OY_ICC_VERSION_2)
    profile_version = 4.3;

  error = lcm2CreateAbstractWhitePointProfileLab( cie_a, cie_b, 15,
                                                  profile_version,
                                                  &my_abstract_file_name,
                                                  &abs );

  if(error || !abs)
  {
    l2cms_msg( oyMSG_WARN, (oyStruct_s*)abs, OY_DBG_FORMAT_ " "
               "failed to build white point effect: %s",
               OY_DBG_ARGS_, oyNoEmptyString_m_(my_abstract_file_name) );
  } else
  {
    void * data;
    size_t size = 0;
    data = lcm2WriteProfileToMem( abs, &size, oyAllocateFunc_ );
    prof = oyProfile_FromMem( size, data, 0,0 );
    if(data && size) oyFree_m_( data );
  }

  if(oy_debug && getenv("OY_DEBUG_WRITE"))
  {
      char * t = 0; oyjlStringAdd( &t, 0,0,
      "%04d-%s-abstract-wtptL[%d]", ++oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)prof));
      lcm2WriteProfileToFile( abs, t, NULL,NULL );
      oyFree_m_(t);
  }

  oyFree_m_(my_abstract_file_name);
  if(abs) l2cmsCloseProfile( abs );

  return prof;
}

#define OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "create_profile.white_point_adjust.lab.icc._" CMM_NICK "._CPU"

/** @brief  lcm2AbstractWhitePoint()
 *  This function implements oyMOptions_Handle_f.
 *
 *  @param[in]     options             expects at least two options
 *                                     - "cie_a": The option shall be a double.
 *                                     - "cie_b": The option shall be a double.
 *                                     - "icc_profile_flags"  ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *                                     This option shall be a integer.
 *  @param[in]     command             "//" OY_TYPE_STD "/create_profile.white_point_adjust.lab"
 *  @param[out]    result              will contain a oyProfile_s in "icc_profile.create_profile.white_point_adjust.lab"
 *
 *  This function uses internally lcm2AbstractWhitePoint().
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 *  @date    2017/06/05
 */
int          l2cmsMOptions_Handle3   ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  double cie_a = -1,
         cie_b = -1;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile.white_point_adjust.lab", 0))
    {
      error = oyOptions_FindDouble( options, "cie_a", 0, &cie_a ); 

      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile.white_point_adjust.lab", 0))
  {
    int32_t icc_profile_flags = 0;
    oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags ); 
    oyProfile_s * p = NULL;

    if( oyOptions_FindDouble( options, "cie_a", 0, &cie_a ) == 0 &&
        oyOptions_FindDouble( options, "cie_b", 0, &cie_b ) == 0 )
      p = lcm2AbstractWhitePoint( cie_a, cie_b, icc_profile_flags );

    if(p)
    {
      oyOption_s * o = oyOption_FromRegistration( OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION ".icc_profile", 0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                   "effect creation failed",
                   OY_DBG_ARGS_ );
  }

  return 0;
}
/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/01
 *  @since   2017/06/06 (Oyranos: 0.9.7)
 */
const char * l2cmsInfoGetTextProfileC3(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "create_profile")==0)
  {
         if(type == oyNAME_NICK)
      return "white_point_adjust.lab";
    else if(type == oyNAME_NAME)
      return _("Create a ICC white point profile.");
    else
      return _("The littleCMS \"create_profile.white_point_adjust.lab\" command lets you create ICC abstract profiles from CIE*ab coordinates for white point adjustment. The filter expects a oyOption_s object with name \"cie_a\" and \"cie_b\" each containing a double value in range -0.5 - 0.5. The result will appear in \"icc_profile\" with the additional attributes \"create_profile.white_point_adjust.lab\" as a oyProfile_s object.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC abstract white point effect profile.");
    else
      return _("The littleCMS \"create_profile.white_point_adjust.lab\" command lets you create ICC abstract profiles from CIE*ab coordinates. See the \"create_profile\" info item.");
  }
  return 0;
}

/** l2cms_api10_cmm3
 *  @brief   Node for Creating White Point Effect Profiles
 *
 *  littleCMS 2 oyCMMapi10_s implementation
 *
 *  For the front end API see oyOptions_Handle(). The backend options
 *  are described in l2cmsMOptions_Handle3().
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/06/05 (Oyranos: 0.9.7)
 *  @date    2017/06/05
 */
oyCMMapi10_s_    l2cms_api10_cmm3 = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api10_cmm4,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  l2cmsInfoGetTextProfileC3,            /**< getText */
  (char**)l2cms_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  l2cmsMOptions_Handle3                 /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_LCM2_CREATE_ABSTRACT_WHITE_POINT_LAB_REGISTRATION -------------------------- */

/* OY_LCM2_CREATE_ABSTRACT_PROOFING_REGISTRATION -------------------------- */

/** Function l2cmsGamutCheckAbstract
 *  @brief   convert a proofing profile into a abstract one
 *
 *  Abstract profiles can easily be merged into a multi profile transform.
 *
 *  @param         proof               the proofing profile; owned by the
 *                                     function
 *  @param         flags               the gamut check and softproof flags
 *  @param         intent              rendering intent
 *  @param         intent_proof        proof rendering intent
 *  @param         icc_profile_flags   profile flags
 *
 *  @version Oyranos: 0.1.11
 *  @since   2009/11/04 (Oyranos: 0.1.10)
 *  @date    2010/08/14
 */
cmsHPROFILE  l2cmsGamutCheckAbstract  ( oyProfile_s       * proof,
                                       cmsUInt32Number     flags,
                                       int                 intent,
                                       int                 intent_proof,
                                       uint32_t            icc_profile_flags )
{
      int error = 0;
#if LCMS_VERSION >= 2060
      cmsContext tc = l2cmsCreateContext( NULL, NULL ); /* threading context */
      l2cmsSetLogErrorHandlerTHR( tc, l2cmsErrorHandlerFunction );
#else
      void * tc = NULL;
#endif
      cmsHPROFILE gmt = 0,
                  hLab = 0,
                  hproof = 0;
      cmsHTRANSFORM tr = 0;

      cmsHTRANSFORM ptr[2] = {0,0};

      l2cms_msg( oyMSG_DBG, (oyStruct_s*)proof, OY_DBG_FORMAT_
                "softproofing %d gamutcheck %d intent %d intent_proof %d", OY_DBG_ARGS_,
                flags & cmsFLAGS_SOFTPROOFING,
                flags & cmsFLAGS_GAMUTCHECK,
                intent, intent_proof );

      if(!(flags & cmsFLAGS_GAMUTCHECK || flags & cmsFLAGS_SOFTPROOFING))
        return gmt;

      hLab  = l2cmsCreateLab4ProfileTHR(tc, l2cmsD50_xyY());
#if LCMS_VERSION < 2060
      hproof = l2cmsAddProfile( proof );
#else
      {
        size_t size = 0;
        void * block = oyProfile_GetMem( proof, &size, 0, malloc );
        hproof = CMMProfileOpen_M( tc, block, size );
      }
#endif

      if(!hLab || !hproof)
      { l2cms_msg( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                 "hLab or hproof failed", OY_DBG_ARGS_);
                 goto clean;
      }

      tr = l2cmsCreateProofingTransformTHR (  tc,
                                               hLab, TYPE_Lab_FLT,
                                               hLab, TYPE_Lab_FLT,
                                               hproof,
                                               intent,
            /* TODO The INTENT_ABSOLUTE_COLORIMETRIC should lead to 
               paper simulation, but does take white point into account.
               Do we want this?
             */
                                               intent_proof,
                                               flags | cmsFLAGS_KEEP_SEQUENCE);
      if(!tr) { l2cms_msg( oyMSG_ERROR, (oyStruct_s*)proof, OY_DBG_FORMAT_
                          "cmsCreateProofingTransform() failed", OY_DBG_ARGS_);
                error = 1; }
      ptr[0] = tr;
      ptr[1] = flags & cmsFLAGS_GAMUTCHECK ? (oyPointer)1 : 0;
      if(!error)
      {
        const char * proof_meta[] = {
        "EFFECT_,CMF_",
        "EFFECT_class", "proof,saturation,contrast,atom",
        "EFFECT_saturation", "yes,reduce",
        "EFFECT_lightness", "no",
        "EFFECT_contrast", "yes,reduce",
        "CMF_binary", "lcm2profile",
        "CMF_version", "0.9.7",
        "CMF_product", "Oyranos",
        0,0
        };
        const char * desc = oyjlTermColorToPlain(oyProfile_GetText( proof, oyNAME_DESCRIPTION ));
        lcm2CreateAbstractProfile (
                             lcm2SamplerProof, ptr,
                             "*lab", // CIE*Lab
                             l2cmsPROOF_LUT_GRID_RASTER,
                             icc_profile_flags & OY_ICC_VERSION_2 ? 2.4 : 4.2,
                             "proofing",
                             NULL,
                             "proofing",
                             "",
                             "",
                             ICC_2011_LICENSE,
                             desc,
                             "http://www.oyranos.org",
                             proof_meta,
                             &gmt
                           );

      }

      if(!gmt)
      {
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)proof, OY_DBG_FORMAT_ " "
                 "failed to build proof",
                 OY_DBG_ARGS_ );
        goto clean;
      }

  if(oy_debug && getenv("OY_DEBUG_WRITE"))
  {
      char * t = 0; oyjlStringAdd( &t, 0,0,
      "%04d-%s-abstract-proof[%d]", ++oy_debug_write_id,CMM_NICK,oyStruct_GetId((oyStruct_s*)proof));
      lcm2WriteProfileToFile( gmt, t, NULL,NULL );
      oyFree_m_(t);
  }

  clean:
      if(hLab) { l2cmsCloseProfile( hLab ); hLab = 0; }
      if(hproof) { l2cmsCloseProfile( hproof ); hproof = 0; }
      if(tr) { l2cmsDeleteTransform( tr ); tr = 0; }
      l2cmsDeleteContext( tc );

  return gmt;
}

/** @brief  l2cmsAddProofProfile()
 *  This function implements oyMOptions_Handle_f.
 *
 *  @param[in]     options             expects at least one option
 *                                     - "proofing_profile": The option shall be a oyProfile_s.
 *                                     - "icc_profile_flags"  ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *                                     This option shall be a integer.
 *  @param[in]     command             "//" OY_TYPE_STD "/create_profile.proofing_profile"
 *  @param[out]    result              will contain a oyProfile_s in "icc_profile.create_profile.proofing_profile"
 *
 *  This function uses internally l2cmsAddProofProfile().
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
int          l2cmsMOptions_Handle2    ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  int error = 0;
  oyProfile_s * prof = 0,
              * p = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile", 0))
    {
      p = (oyProfile_s*) oyOptions_GetType( options,-1, "proofing_profile",
                                            oyOBJECT_PROFILE_S );
      if(!p)
      {
        error = -1;
      }

      oyProfile_Release( &p );

      return error;
    }
    else
      return -1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile", 0))
  {
    int32_t icc_profile_flags = 0;
    oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags ); 

    p = (oyProfile_s*) oyOptions_GetType( options,-1, "proofing_profile",
                                          oyOBJECT_PROFILE_S );
    if(p)
    {
      int intent = l2cmsIntentFromOptions( options,0 ),
      intent_proof = l2cmsIntentFromOptions( options,1 ),
      flags = l2cmsFlagsFromOptions( options );
      oyOption_s * o;

      l2cmsProfileWrap_s * wrap = l2cmsAddProofProfile( p, flags | cmsFLAGS_SOFTPROOFING,
                                            intent, intent_proof, icc_profile_flags );
      oyProfile_Release( &p );
      if(!wrap)
        return 1;

      prof = oyProfile_FromMem( wrap->size, wrap->block, 0, 0 );
      oyObject_UnRef( wrap->oy_ ); wrap = NULL;

      o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.create_profile.proofing_effect._" CMM_NICK,
                        0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    } else
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ " "
                 "no option \"proofing_effect\" of type oyProfile_s found",
                 OY_DBG_ARGS_ );
  }

  return 0;
}
/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
const char * l2cmsInfoGetTextProfileC2(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "create_profile")==0)
  {
         if(type == oyNAME_NICK)
      return "proofing_effect";
    else if(type == oyNAME_NAME)
      return _("Create a ICC abstract proofing profile.");
    else
      return _("The littleCMS \"create_profile.proofing_effect\" command lets you create ICC abstract profiles from a given ICC profile for proofing. The filter expects a oyOption_s object with name \"proofing_profile\" containing a oyProfile_s as value. The options \"rendering_intent\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_gamut_warning\", \"precalculation\", \"precalculation_curves\", \"cmyk_cmyk_black_preservation\", \"adaption_state\"  and \"no_white_on_white_fixup\" are honoured. The result will appear in \"icc_profile\" with the additional attributes \"create_profile.proofing_effect\" as a oyProfile_s object.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC proofing profile.");
    else
      return _("The littleCMS \"create_profile.proofing_effect\" command lets you create ICC abstract profiles from some given ICC profile. See the \"create_profile\" info item.");
  }
  return 0;
}

#define OY_LCM2_CREATE_ABSTRACT_PROOFING_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "create_profile.proofing_effect.icc._" CMM_NICK "._CPU"

/** l2cms_api10_cmm2
 *  @brief   Node for Creating Proofing Effect Profiles
 *  
 *  littleCMS 2 oyCMMapi10_s implementation
 *
 *  For the front end API see oyOptions_Handle(). The backend options
 *  are described in l2cmsMOptions_Handle2().
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/21 (Oyranos: 0.3.0)
 *  @date    2011/02/21
 */
oyCMMapi10_s_    l2cms_api10_cmm2 = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api10_cmm3,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_CREATE_ABSTRACT_PROOFING_REGISTRATION,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  l2cmsInfoGetTextProfileC2,            /**< getText */
  (char**)l2cms_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  l2cmsMOptions_Handle2                 /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_LCM2_CREATE_ABSTRACT_PROOFING_REGISTRATION -------------------------- */

/* OY_LCM2_CREATE_MATRIX_REGISTRATION ------------------------------------- */

/** Function l2cmsCreateICCMatrixProfile
 *  @brief   create a profile from primaries, white point and one gamma value
 *
 *  Used for ICC from EDID, Camera RAW etc. Marti calls these matrix/shaper.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/04/07
 *  @since   2009/10/24 (Oyranos: 0.1.10)
 */
oyProfile_s *      l2cmsCreateICCMatrixProfile (
                                       float             gamma,
                                       float rx, float ry,
                                       float gx, float gy,
                                       float bx, float by,
                                       float wx, float wy,
                                       int icc_profile_flags )
{
  cmsHPROFILE lp = 0;
  size_t size = 0;
  char * data = 0;

  int error = 0;
  oyProfile_s * prof = 0;

  lp = lcm2CreateICCMatrixProfile2( gamma, rx,ry, gx,gy, bx,by, wx,wy );

  if(icc_profile_flags & OY_ICC_VERSION_2)
    l2cmsSetProfileVersion(lp, 2.4);

  data = lcm2WriteProfileToMem( lp, &size, oyAllocateFunc_ );
  if(!size)
    l2cms_msg( oyMSG_WARN,0, OY_DBG_FORMAT_
             "l2cmsSaveProfileToMem failed for: red: %g %g green: %g %g blue: %g %g white: %g %g gamma: %g",
             OY_DBG_ARGS_, rx,ry, gx,gy, bx,by, wx,wy, gamma );
  l2cmsCloseProfile( lp );

  prof = oyProfile_FromMem( size, data, 0,0 );


  error = oyProfile_AddTagText( prof, icSigCopyrightTag,
                                      "no copyright; use freely" );
  if(error) WARNc2_S("%s %d", _("found issues"),error);

  oyDeAllocateFunc_( data ); size = 0;
  return prof;
}

/** @brief  l2cmsCreateICCMatrixProfile()
 *  This function implements oyMOptions_Handle_f.
 *
 *  @param[in]     options             expects at least one option
 *                                     - "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma"
 *                                     The option shall contain 9 double values.
 *                                     - "icc_profile_flags"  ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *                                     This option shall be a integer.
 *  @param[in]     command             "//" OY_TYPE_STD "/create_profile.color_matrix.icc"
 *  @param[out]    result              will contain a oyProfile_s in "icc_profile.create_profile.color_matrix"
 *
 *  This function uses internally l2cmsCreateICCMatrixProfile().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
int          l2cmsMOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  oyOption_s * o = 0;
  oyProfile_s * prof = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"create_profile", 0))
    {
      double val = 0.0;
      o = oyOptions_Find( options,
        "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                          oyNAME_PATTERN );
      error = oyOptions_FindDouble( options,
        "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                            8, &val );
      if(!o)
      {
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ " "
                 "no option \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" found",
                 OY_DBG_ARGS_ );
        error = 1;
      } else if( error != 0 )
      {
        l2cms_msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_" "
                 "option \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" %s",
                 OY_DBG_ARGS_,
                 (error < 0) ? "contains less than 9 required values" :
                               "access returned with error" );
      }

      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"create_profile", 0))
  {
    o = oyOptions_Find( options,
         "color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma",
                        oyNAME_PATTERN );
    if(o)
    {
      int32_t icc_profile_flags = 0;
      oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags ); 

      prof = l2cmsCreateICCMatrixProfile (
                    oyOption_GetValueDouble(o,8),
                    oyOption_GetValueDouble(o,0), oyOption_GetValueDouble(o,1),
                    oyOption_GetValueDouble(o,2), oyOption_GetValueDouble(o,3),
                    oyOption_GetValueDouble(o,4), oyOption_GetValueDouble(o,5),
                    oyOption_GetValueDouble(o,6), oyOption_GetValueDouble(o,7),
                    icc_profile_flags );
      oyOption_Release( &o );

      o = oyOption_FromRegistration( OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.create_profile.color_matrix._" CMM_NICK,
                        0 );
      error = oyOption_MoveInStruct( o, (oyStruct_s**) &prof );
      if(!*result)
        *result = oyOptions_New(0);
      oyOptions_MoveIn( *result, &o, -1 );
    }
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
const char * l2cmsInfoGetTextProfileC ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "create_profile")==0)
  {
         if(type == oyNAME_NICK)
      return "create_profile";
    else if(type == oyNAME_NAME)
      return _("Create a ICC matrix profile.");
    else
      return _("The littleCMS \"create_profile.color_matrix\" command lets you create ICC profiles from some given colorimetric coordinates. The filter expects a oyOption_s object with name \"color_matrix.redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma\" containing 9 floats in the order of CIE*x for red, CIE*y for red, CIE*x for green, CIE*y for green, CIE*x for blue, CIE*y for blue, CIE*x for white, CIE*y for white and a gamma value.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("Create a ICC matrix profile.");
    else
      return _("The littleCMS \"create_profile.color_matrix\" command lets you create ICC profiles from some given colorimetric coordinates. See the \"create_profile\" info item.");
  }
  return 0;
}
#define OY_LCM2_CREATE_MATRIX_REGISTRATION OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "create_profile.color_matrix.icc._" CMM_NICK "._CPU"

/** l2cms_api10_cmm
 *  @brief   Node for Creating simple Color Matrix Profiles
 *
 *  littleCMS 2 oyCMMapi10_s implementation
 *
 *  For the front end API see oyOptions_Handle(). The backend options
 *  are described in l2cmsMOptions_Handle().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
oyCMMapi10_s_    l2cms_api10_cmm = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api10_cmm2,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_CREATE_MATRIX_REGISTRATION,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
 
  l2cmsInfoGetTextProfileC,             /**< getText */
  (char**)l2cms_texts_profile_create,   /**<texts; list of arguments to getText*/
 
  l2cmsMOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};

/* OY_LCM2_CREATE_MATRIX_REGISTRATION ------------------------------------- */
/**  @} *//* lcm2_misc */
/**  @} *//* misc_modules */


/** \addtogroup graph_modules
 *  @{ */
/** \addtogroup lcm2_graph lcm2 Module
 *  @brief      Little CMS 2 ICC style color conversion
 *
 *  The modules provide ICC style color space converison and data processing.
 *
 *  @{ */

#define OY_LCM2_DATA_CONVERT_REGISTRATION  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH \
  "icc_color._" CMM_NICK "._CPU." oyCOLOR_ICC_DEVICE_LINK "_" l2cmsTRANSFORM

/** l2cms_api6_cmm
 *  @brief   Node for Converting a Device Link into a lcms2 CMM Context
 *
 *  littleCMS oyCMMapi6_s implementation
 *
 *  a filter providing CMM API's
 *
 *  This Node type uses internally l2cmsModuleData_Convert().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
oyCMMapi6_s_ l2cms_api6_cmm = {

  oyOBJECT_CMM_API6_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api10_cmm,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_LCM2_DATA_CONVERT_REGISTRATION,

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */
  
  oyCOLOR_ICC_DEVICE_LINK,             /* data_type_in, "oyDL" */
  l2cmsTRANSFORM,                      /* data_type_out, l2cmsTRANSFORM */
  l2cmsModuleData_Convert              /* oyModuleData_Convert_f oyModuleData_Convert */
};


/** l2cms_api7_cmm
 *  @brief   lcms2 ICC CMM Pixel Processor Engine Node
 *
 *  littleCMS oyCMMapi7_s implementation
 *
 *  a filter providing CMM API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
oyCMMapi7_s_ l2cms_api7_cmm = {

  oyOBJECT_CMM_API7_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api6_cmm,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc_color._" CMM_NICK "._icc_version_2._icc_version_4._CPU._NOACCEL",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  l2cmsFilterPlug_CmmIccRun,  /* oyCMMFilterPlug_Run_f */
  l2cmsTRANSFORM,             /* data_type, l2cmsTRANSFORM */

  (oyConnector_s**) l2cms_cmmIccPlug_connectors,/* plugs */
  1,                         /* plugs_n */
  0,                         /* plugs_last_add */
  (oyConnector_s**) l2cms_cmmIccSocket_connectors,   /* sockets */
  1,                         /* sockets_n */
  0,                         /* sockets_last_add */
  NULL                       /* properties */
};

/**
 *  This function implements oyCMMGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/22 (Oyranos: 0.1.10)
 *  @date    2009/12/22
 */
const char * l2cmsApi4UiGetText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  static char * category = 0;
  if(strcmp(select,"name") == 0 ||
     strcmp(select,"help") == 0)
  {
    return l2cmsInfoGetText( select, type, context );
  }
  else if(strcmp(select,"category"))
  {
    if(!category)
    {
      STRING_ADD( category, _("Color") );
      STRING_ADD( category, _("/") );
      /* CMM: abbreviation for Color Matching Module */
      STRING_ADD( category, _("CMM") );
      STRING_ADD( category, _("/") );
      STRING_ADD( category, _("littleCMS") );
    }
         if(type == oyNAME_NICK)
      return "category";
    else if(type == oyNAME_NAME)
      return category;
    else
      return category;
  }
  return 0;
}
const char * l2cms_api4_ui_texts[] = {"name", "category", "help", 0};
/** l2cms_api4_ui
 *  @brief   lcms2 ICC CMM Node UI
 *
 *  l2cms oyCMMapi4_s::ui implementation
 *
 *  The UI for l2cms.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/09 (Oyranos: 0.1.10)
 *  @date    2009/09/09
 */
oyCMMui_s_ l2cms_api4_ui = {
  oyOBJECT_CMM_DATA_TYPES_S,           /**< oyOBJECT_e       type; */
  0,0,0,                            /* unused oyStruct_s fields; keep to zero */

  CMM_VERSION,                         /**< int32_t version[3] */
  CMM_API_VERSION,                            /**< int32_t module_api[3] */

  l2cmsFilter_CmmIccValidateOptions, /* oyCMMFilter_ValidateOptions_f */
  l2cmsWidgetEvent, /* oyWidgetEvent_f */

  "Color/CMM/littleCMS2", /* category */
  l2cms_extra_options,   /* const char * options */
  l2cmsGetOptionsUI,     /* oyCMMuiGet_f oyCMMuiGet */

  l2cmsApi4UiGetText, /* oyCMMGetText_f   getText */
  l2cms_api4_ui_texts,/* const char    ** texts */
  (oyCMMapiFilter_s*)&l2cms_api4_cmm /* oyCMMapiFilter_s * parent */
};

/** l2cms_api4_cmm
 *  @brief   lcms2 ICC CMM Context Setup and UI Node
 *
 *  littleCMS oyCMMapi4_s implementation for color context setup
 *
 *  A filter providing CMM API's. It creates specifically a ICC
 *  device link profile for exchange with data processing CMM engines.
 *
 *  This node type uses internally l2cmsFilterNode_CmmIccContextToMem().
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/18 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyCMMapi4_s_ l2cms_api4_cmm = {

  oyOBJECT_CMM_API4_S,
  0,0,0,
  (oyCMMapi_s*) & l2cms_api7_cmm,

  l2cmsCMMapiInit,
  l2cmsCMMapiReset,
  l2cmsCMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "icc_color._" CMM_NICK "._icc_version_2._icc_version_4._CPU._NOACCEL._effect",

  CMM_VERSION,
  CMM_API_VERSION,                  /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,   /* runtime_context */

  l2cmsFilterNode_CmmIccContextToMem, /* oyCMMFilterNode_ContextToMem_f */
  l2cmsFilterNode_GetText, /* oyCMMFilterNode_GetText_f */
  oyCOLOR_ICC_DEVICE_LINK, /* context data_type */

  &l2cms_api4_ui                        /**< oyCMMui_s *ui */
};

/**  @} *//* lcm2_graph */
/**  @} *//* graph_modules */


/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2008/12/30
 */
const char * l2cmsInfoGetText         ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return CMM_NICK;
    else if(type == oyNAME_NAME)
      return _("Little CMS 2");
    else
      return _("LittleCMS 2 is a CMM, a color management engine; it implements fast transforms between ICC profiles. \"Little\" stands for its small overhead. With a typical footprint of about 100K including C runtime, you can color-enable your application without the pain of ActiveX, OCX, redistributables or binaries of any kind. We are using little cms in several commercial projects, however, we are offering lcms library free for anybody under an extremely liberal open source license.");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return "Marti";
    else if(type == oyNAME_NAME)
      return "Marti Maria";
    else
      return _("littleCMS 2 project; www: http://www.littlecms.com; support/email: support@littlecms.com; sources: http://www.littlecms.com/downloads.htm; Oyranos wrapper: Kai-Uwe Behrmann for the Oyranos project");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return "MIT";
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 1998-2016 Marti Maria Saguer; MIT");
    else
      return _("MIT license: http://www.opensource.org/licenses/mit-license.php");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return "help";
    else if(type == oyNAME_NAME)
      return _("The lcms \"color_icc\" filter is a one dimensional color conversion filter. It can both create a color conversion context, some precalculated for processing speed up, and the color conversion with the help of that context. The adaption part of this filter transforms the Oyranos color context, which is ICC device link based, to the internal lcms format.");
    else
      return _("The following options are available to create color contexts:\n \"profiles_simulation\", a option of type oyProfiles_s, can contain device profiles for proofing.\n \"profiles_effect\", a option of type oyProfiles_s, can contain abstract color profiles.\n The following Oyranos options are supported: \"rendering_gamut_warning\", \"rendering_intent_proof\", \"rendering_bpc\", \"rendering_intent\", \"proof_soft\" and \"proof_hard\".\n The additional lcms option is supported \"cmyk_cmyk_black_preservation\" [0 - none; 1 - LCMS_PRESERVE_PURE_K; 2 - LCMS_PRESERVE_K_PLANE], \"precalculation\": [0 - normal; 1 - cmsFLAGS_NOOPTIMIZE; 2 - cmsFLAGS_HIGHRESPRECALC, 3 - cmsFLAGS_LOWRESPRECALC, 4 - cmsFLAGS_NULLTRANSFORM], \"precalculation_curves\": [0 - none; 1 - cmsFLAGS_CLUT_POST_LINEARIZATION + cmsFLAGS_CLUT_PRE_LINEARIZATION], \"adaption_state\": [0.0 - not adapted to screen, 1.0 - full adapted to screen] and \"no_white_on_white_fixup\": [0 - force white on white, 1 - keep as is]." );
  }
  return 0;
}
const char *l2cms_texts[5] = {"name","copyright","manufacturer","help",0};
oyIcon_s l2cms_icon = {oyOBJECT_ICON_S, 0,0,0, 0,0,0, "lcms_logo2.png"};

/** lcm2_cmm_module
 *  @brief    l2cms Module Infos
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/30
 */
oyCMM_s lcm2_cmm_module = {

  oyOBJECT_CMM_INFO_S,                 /**< type, struct type */
  0,0,0,                               /**< ,dynamic object functions */
  CMM_NICK,                            /**< cmm, ICC signature */
  "0.6",                               /**< backend_version */
  l2cmsInfoGetText,                    /**< getText */
  (char**)l2cms_texts,                 /**<texts; list of arguments to getText*/
  OYRANOS_VERSION,                     /**< oy_compatibility */

  (oyCMMapi_s*) & l2cms_api4_cmm,      /**< api */

  &l2cms_icon, /**< icon */
  l2cmsCMMinit,                        /**< oyCMMinfoInit_f */
  l2cmsCMMreset                        /**< oyCMMinfoReset_f */
};

