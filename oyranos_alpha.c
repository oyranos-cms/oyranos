/** @file oyranos_alpha.c
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


#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_cmms.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_monitor.h" /* TODO */
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#if !defined(WIN32)
#include <dlfcn.h>
#include <inttypes.h>
#endif
#include <math.h>

/* internal declarations */
oyObject_s   oyObject_SetAllocators_ ( oyObject_s          object,
                                       oyAllocFunc_t       allocateFunc,
                                       oyDeAllocFunc_t     deallocateFunc );
int          oyObject_UnRef          ( oyObject_s          obj );
int          oyObject_Ref            ( oyObject_s          obj );
/*oyObject_s   oyObject_SetCMMPtr_     ( oyObject_s          object,
                                       const char        * cmm,
                                       oyPointer           cmm_ptr,
                                       const char        * resource,
                                       oyStructReleaseF_t  ptrRelease,
                                       oyChar            * func_name );*/


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

oyProfile_s* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object);
oyCMMptr_s * oyProfile_GetCMMPtr_    ( oyProfile_s       * profile,
                                       const char        * cmm );
/*oyChar *     oyProfile_GetCMMText_   ( oyProfile_s       * profile,
                                       oyNAME_e            type,
                                       const char        * language,
                                       const char        * country );*/
oyCMMptr_s** oyProfileList_GetCMMptrs_(oyProfileList_s   * list,
                                       const char        * cmm );
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc );
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name );
int          oyProfile_Equal_        ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB );
int32_t      oyProfile_Match_        ( oyProfile_s       * pattern,
                                       oyProfile_s       * profile );
int32_t      oyProfile_Hashed_       ( oyProfile_s       * s );
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s     * profile,
                                       int                 pos );
oyColourConversion_s* oyColourConversion_Create_ (
                                       oyProfileList_s   * list,
                                       oyOptions_s       * opts,
                                       oyImage_s         * in,
                                       oyImage_s         * out,
                                       oyObject_s          object);
const oyChar *     oyColourConversion_GetID_ (
                                       oyColourConversion_s * s,
                                       oyProfileList_s * list,
                                       oyOptions_s     * opts,
                                       oyImage_s       * in,
                                       oyImage_s       * out);
oyPointer    oyColourConversion_ToMem_(
                                       oyColourConversion_s * oy,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc );

int          oyCMMdsoRelease_        ( const char        * cmm );
int          oyCMMdsoSearch_         ( const char        * cmm );
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name );

typedef enum {
  oyREQUEST_NONE,                      /**< deactives the request */
  oyREQUEST_SLIGTH,
  oyREQUEST_MID,
  oyREQUEST_MUCH,
  oyREQUEST_HARD                       /**< requirement, fail if not present */
} oyREQUEST_e;

/** @struct oyCMMapiQuery_s
 *  @internal
 *  @brief  check/prepare a module
 *
 *  @since  Oyranos: version 0.1.8
 *  @date   2008/01/03 (API 0.1.8)
 */
typedef struct {
  oyCMMQUERY_e         query;
  uint32_t             value;
  oyREQUEST_e          request;
} oyCMMapiQuery_s;

/** @struct oyCMMapiQuerie_s
 *  @internal
 *  @brief  check/prepare a module
 *
 *  @since  Oyranos: version 0.1.8
 *  @date   2008/01/03 (API 0.1.8)
 */
typedef struct {
  int                  n;
  oyCMMapiQuery_s   ** queries;
  char                 prefered_cmm[5];
} oyCMMapiQueries_s;


char **          oyCMMsGetNames_     ( int               * n,
                                       oyOBJECT_TYPE_e   * api_types,
                                       int                 types_n );
char **          oyCMMsGetLibNames_  ( int               * n,
                                       const char        * required_cmm );
char *           oyCMMnameFromLibName_(const char        * lib_name);
oyCMMInfo_s *    oyCMMInfoAtListFromLibName_(const char        * lib_name );
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name );
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_TYPE_e     type,
                                       const char        * cmm_required,
                                       oyCMMapiQueries_s * capabilities,
                                       char              * cmm_used,
                                       const char        * category,
                                       oyFILTER_TYPE_e     filter_type );

oyOBJECT_TYPE_e  oyCMMapi_Check_     ( oyCMMapi_s        * api );
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * cmm );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
int              oyCMMRelease_       ( const char        * cmm );
unsigned int     oyCMMapiIsReady_    ( oyOBJECT_TYPE_e     type );


int          oyPointerReleaseFunc_   ( oyPointer         * ptr );

int          oySizeofDatatype        ( oyDATATYPE_e        t );

uint32_t     oyCMMtoId               ( const char        * cmm );
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm );


#define hashTextAdd_m( text_ ) \
  oyStringAdd_( &hash_text, text_, s->oy_->allocateFunc_, \
                            s->oy_->deallocateFunc_ );

/** \addtogroup misc Miscellaneous

 *  @{
 */

/** @internal
 *  pixel mask description 
 */
enum {
  oyPOFF_X = 0, /* pixel count x offset */
  oyPOFF_Y,     /* pixel count y offset */
  oyCOFF,       /* channel offset */
  oyDATA_SIZE,  /* sample size in byte */
  oyLAYOUT,     /* remembering the layout */
  oyCHANS,      /* number of channels */
  oyCHAN0       /* first colour channel */
};

oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj )    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyLockDummy_                ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyUnLockDummy_              ( oyPointer         look,
                                         const char      * marker,
                                         int               line ) {;}


oyStruct_LockCreateF_t  oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockReleaseF_t        oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLockF_t               oyLockFunc_        = oyLockDummy_;
oyUnLockF_t             oyUnLockFunc_      = oyUnLockDummy_;


/** @func  oyICCColourSpaceGetChannelCount
 *  @brief number of channels in a colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oyICCColourSpaceGetChannelCount ( icColorSpaceSignature color )
{
  int n;

  switch (color) {
    case icSigXYZData: n = 3; break;
    case icSigLabData: n = 3; break;
    case icSigLuvData: n = 3; break;
    case icSigYCbCrData: n = 3; break;
    case icSigYxyData: n = 3; break;
    case icSigRgbData: n = 3; break;
    case icSigGrayData: n = 1; break;
    case icSigHsvData: n = 3; break;
    case icSigHlsData: n = 3; break;
    case icSigCmykData: n = 4; break;
    case icSigCmyData: n = 3; break;
    case icSig2colorData: n = 2; break;
    case icSig3colorData: n = 3; break;
    case icSig4colorData: n = 4; break;
    case icSig5colorData:
    case icSigMCH5Data:
          n = 5; break;
    case icSig6colorData:
    case icSigMCH6Data:
         n = 6; break;
    case icSig7colorData:
    case icSigMCH7Data:
         n = 7; break;
    case icSig8colorData:
    case icSigMCH8Data:
         n = 8; break;
    case icSig9colorData:
    case icSigMCH9Data:
         n = 9; break;
    case icSig10colorData:
    case icSigMCHAData:
         n = 10; break;
    case icSig11colorData:
    case icSigMCHBData:
         n = 11; break;
    case icSig12colorData:
    case icSigMCHCData:
         n = 12; break;
    case icSig13colorData:
    case icSigMCHDData:
         n = 13; break;
    case icSig14colorData:
    case icSigMCHEData:
         n = 14; break;
    case icSig15colorData:
    case icSigMCHFData:
         n = 15; break;
    default: n = 0; break;
  }
  return n;
}

/** @func  oyICCColourSpaceGetName
 *  @brief name of a colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
const char *
oyICCColourSpaceGetName ( icColorSpaceSignature sig )
{
  const char * text = 0;

  switch (sig) {
    case icSigXYZData: text =_("XYZ"); break;
    case icSigLabData: text =_("Lab"); break;
    case icSigLuvData: text =_("Luv"); break;
    case icSigYCbCrData: text =_("YCbCr"); break;
    case icSigYxyData: text =_("Yxy"); break;
    case icSigRgbData: text =_("Rgb"); break;
    case icSigGrayData: text =_("Gray"); break;
    case icSigHsvData: text =_("Hsv"); break;
    case icSigHlsData: text =_("Hls"); break;
    case icSigCmykData: text =_("Cmyk"); break;
    case icSigCmyData: text =_("Cmy"); break;
    case icSig2colorData: text =_("2color"); break;
    case icSig3colorData: text =_("3color"); break;
    case icSig4colorData: text =_("4color"); break;
    case icSigMCH5Data:
    case icSig5colorData: text =_("5color"); break;
    case icSigMCH6Data:
    case icSig6colorData: text =_("6color"); break;
    case icSigMCH7Data:
    case icSig7colorData: text =_("7color"); break;
    case icSigMCH8Data:
    case icSig8colorData: text =_("8color"); break;
    case icSigMCH9Data:
    case icSig9colorData: text =_("9color"); break;
    case icSigMCHAData:
    case icSig10colorData: text =_("10color"); break;
    case icSigMCHBData:
    case icSig11colorData: text =_("11color"); break;
    case icSigMCHCData:
    case icSig12colorData: text =_("12color"); break;
    case icSigMCHDData:
    case icSig13colorData: text =_("13color"); break;
    case icSigMCHEData:
    case icSig14colorData: text =_("14color"); break;
    case icSigMCHFData:
    case icSig15colorData: text =_("15color"); break;
    default: { icUInt32Number i = oyValueCSpaceSig(sig);
               static char t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               text = &t[0];
               break;
             }
  }
  return text;
}

/** @func oyICCColourSpaceGetChannelName
 *  @brief channel names of a colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
const oyChar*
oyICCColourSpaceGetChannelName ( icColorSpaceSignature sig,
                                    int                   pos,
                                    int                   type )
{
  int n = oyICCColourSpaceGetChannelCount( sig );

  if( 0 <= pos && pos < n )
    return "-";

  if( type < 0 )
    type = 0;
  if( type >= oyNAME_DESCRIPTION )
    type = oyNAME_NAME;

  switch(sig)
  {
    case icSigXYZData: switch(pos) {
         case 0: return type ? "X"  : _("CIE X");
         case 1: return type ? "Y"  : _("CIE Y (Luminance)");
         case 2: return type ? "Z"  : _("CIE Z");
         } break;
    case icSigLabData: switch(pos) {
         case 0: return type ? "*L" : _("CIE *L");
         case 1: return type ? "*a" : _("CIE *a");
         case 2: return type ? "*b" : _("CIE *b");
         } break;
    case icSigLuvData: switch(pos) {
         case 0: return type ? "*L" : _("CIE *L");
         case 1: return type ? "*u" : _("CIE *u");
         case 2: return type ? "*v" : _("CIE *v");
         } break;
    case icSigYCbCrData: switch(pos) {
         case 0: return type ? "Y"  : _("Luminance Y");
         case 1: return type ? "Cb" : _("Colour b");
         case 2: return type ? "Cr" : _("Colour r");
         } break;
    case icSigYxyData: switch(pos) {
         case 0: return type ? "Y"  : _("CIE Y (Luminance)");
         case 1: return type ? "x"  : _("CIE x");
         case 2: return type ? "y"  : _("CIE y");
         } break;
    case icSigRgbData: switch(pos) {
         case 0: return type ? "R"  : _("Red");
         case 1: return type ? "G"  : _("Green");
         case 2: return type ? "B"  : _("Blue");
         } break;
    case icSigGrayData: switch(pos) {
         case 0: return type ? "K"  : _("Black");
         } break;
    case icSigHsvData: switch(pos) {
         case 0: return type ? "H"  : _("Hue");
         case 1: return type ? "S"  : _("Saturation");
         case 2: return type ? "V"  : _("Value");
         } break;
    case icSigHlsData: switch(pos) {
         case 0: return type ? "H"  : _("Hue");
         case 1: return type ? "L"  : _("Lightness");
         case 2: return type ? "S"  : _("Saturation");
         } break;
    case icSigCmykData: switch(pos) {
         case 0: return type ? "C"  : _("Cyan");
         case 1: return type ? "M"  : _("Magenta");
         case 2: return type ? "Y"  : _("Yellow");
         case 3: return type ? "K"  : _("Black");
         } break;
    case icSigCmyData: switch(pos) {
         case 0: return type ? "C"  : _("Cyan");
         case 1: return type ? "M"  : _("Magenta");
         case 2: return type ? "Y"  : _("Yellow");
         } break;
    case icSig2colorData:
    case icSig3colorData:
    case icSig4colorData:
    case icSigMCH5Data:
    case icSig5colorData:
    case icSigMCH6Data:
    case icSig6colorData:
    case icSigMCH7Data:
    case icSig7colorData:
    case icSigMCH8Data:
    case icSig8colorData:
    case icSigMCH9Data:
    case icSig9colorData:
    case icSigMCHAData:
    case icSig10colorData:
    case icSigMCHBData:
    case icSig11colorData:
    case icSigMCHCData:
    case icSig12colorData:
    case icSigMCHDData:
    case icSig13colorData:
    case icSigMCHEData:
    case icSig14colorData:
    case icSigMCHFData:
    case icSig15colorData:
         switch(pos) {
         case 0: return type ? "1." : _("1. Channel");
         case 1: return type ? "2." : _("2. Channel");
         case 2: return type ? "3." : _("3. Channel");
         case 3: return type ? "4." : _("4. Channel");
         case 4: return type ? "5." : _("5. Channel");
         case 5: return type ? "6." : _("6. Channel");
         case 6: return type ? "7." : _("7. Channel");
         case 7: return type ? "8." : _("8. Channel");
         case 8: return type ? "9." : _("9. Channel");
         case 9: return type ? "10." : _("10. Channel");
         case 10: return type ? "11." : _("11. Channel");
         case 11: return type ? "12." : _("12. Channel");
         case 12: return type ? "13." : _("13. Channel");
         case 13: return type ? "14." : _("14. Channel");
         case 14: return type ? "15." : _("15. Channel");
         case 15: return type ? "16." : _("16. Channel");
         } break;
    default: return ("-"); break;
  }
  return "-";
}

/** @func  oyICCTagDescription
 *  @brief get tag description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagDescription ( icTagSignature      sig )
{

  switch (sig) {
    case icSigAToB0Tag: return _("Lockup table, device to PCS, intent perceptual"); break;
    case icSigAToB1Tag: return _("Lockup table, device to PCS, intent relative colorimetric"); break;
    case icSigAToB2Tag: return _("Lockup table, device to PCS, intent saturation"); break;
    case icSigBlueColorantTag: return _("Blue Colorant"); break;
    case icSigBlueTRCTag: return _("Blue tone reproduction curve"); break;
    case icSigBToA0Tag: return _("Lockup table, PCS to device, intent perceptual"); break;
    case icSigBToA1Tag: return _("Lockup table, PCS to device, intent relative colorimetric"); break;
    case icSigBToA2Tag: return _("Lockup table, PCS to device, intent saturation"); break;
    case icSigCalibrationDateTimeTag: return _("Calibration date"); break;
    case icSigCharTargetTag: return _("Colour measurement data"); break;
    case icSigCopyrightTag: return _("Copyright"); break;
    case icSigCrdInfoTag: return _("crdi"); break;
    case icSigDeviceMfgDescTag: return _("Device manufacturerer description"); break;
    case icSigDeviceModelDescTag: return _("Device model description"); break;
    case icSigGamutTag: return _("gamut"); break;
    case icSigGrayTRCTag: return _("Gray tone reproduction curve"); break;
    case icSigGreenColorantTag: return _("Green Colorant"); break;
    case icSigGreenTRCTag: return _("Green tone reproduction curve"); break;
    case icSigLuminanceTag: return _("Luminance"); break;
    case icSigMeasurementTag: return _("Measurement"); break;
    case icSigMediaBlackPointTag: return _("Media black point"); break;
    case icSigMediaWhitePointTag: return _("Media white point"); break;
    case icSigNamedColorTag: return _("Named Colour"); break;
    case icSigNamedColor2Tag: return _("Named Colour 2"); break;
    case icSigPreview0Tag: return _("Preview, perceptual"); break;
    case icSigPreview1Tag: return _("Preview, relative colorimetric"); break;
    case icSigPreview2Tag: return _("Preview, saturated"); break;
    case icSigProfileDescriptionTag: return _("Profile description"); break;
    case 1685283693: return _("Profile description, multilingual"); break;/*dscm*/
    case icSigProfileSequenceDescTag: return _("Profile sequence description"); break;
    case icSigProfileSequenceIdentifierTag: return _("Profile sequence identification"); break;
    case icSigPs2CRD0Tag: return _("psd0"); break;
    case icSigPs2CRD1Tag: return _("psd1"); break;
    case icSigPs2CRD2Tag: return _("psd2"); break;
    case icSigPs2CRD3Tag: return _("psd3"); break;
    case icSigPs2CSATag: return _("ps2s"); break;
    case icSigPs2RenderingIntentTag: return _("ps2i"); break;
    case icSigRedColorantTag: return _("Red Colorant"); break;
    case icSigRedTRCTag: return _("Red tone reproduction curve"); break;
    case icSigScreeningDescTag: return _("scrd"); break;
    case icSigScreeningTag: return _("scrn"); break;
    case icSigTechnologyTag: return _("Technologie"); break;
    case icSigUcrBgTag: return _("bfd"); break;
    case icSigViewingCondDescTag: return _("Viewing conditions description"); break;
    case icSigViewingConditionsTag: return _("Viewing Conditions"); break;
    case 1147500100: return _("Device colours"); break;/*DevD*/
    case 1128875332: return _("Measured colours"); break;/*CIED*/
    case 1349350514: return _("Profiling parameters"); break;/*Pmtr*/
    case 1986226036: return _("VideoCardGammaTable"); break;/*vcgt*/
    case 1667785060: return _("Colour adaption matrix"); break; /*chad*/
    case icSigChromaticityType: return _("Chromaticity"); break; /*chrm*/
    case 1668051567: return _("Named colour order"); break;/*clro*/
    case 1668051572: return _("Named colour names"); break;/*clrt*/
    case 0: return _("----"); break;
    default: { icUInt32Number i = oyValueUInt32(sig);
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }
  return "-";
}


/** @func  oyICCTagName
 *  @brief get tag ~4 char name
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagName        ( icTagSignature      sig )
{
  switch (sig) {
    case icSigAToB0Tag: return "A2B0"; break;
    case icSigAToB1Tag: return "A2B1"; break;
    case icSigAToB2Tag: return "A2B2"; break;
    case icSigBlueColorantTag: return "bXYZ"; break;
    case icSigBlueTRCTag: return "bTRC"; break;
    case icSigBToA0Tag: return "B2A0"; break;
    case icSigBToA1Tag: return "B2A1"; break;
    case icSigBToA2Tag: return "B2A2"; break;
    case icSigCalibrationDateTimeTag: return "calt"; break;
    case icSigCharTargetTag: return "targ"; break;
    case icSigCopyrightTag: return "cprt"; break;
    case icSigCrdInfoTag: return "crdi"; break;
    case icSigDeviceMfgDescTag: return "dmnd"; break;
    case icSigDeviceModelDescTag: return "dmdd"; break;
    case icSigGamutTag: return "gamt"; break;
    case icSigGrayTRCTag: return "kTRC"; break;
    case icSigGreenColorantTag: return "gXYZ"; break;
    case icSigGreenTRCTag: return "gTRC"; break;
    case icSigLuminanceTag: return "lumi"; break;
    case icSigMeasurementTag: return "meas"; break;
    case icSigMediaBlackPointTag: return "bkpt"; break;
    case icSigMediaWhitePointTag: return "wtpt"; break;
    case icSigNamedColorTag: return "'ncol"; break;
    case icSigNamedColor2Tag: return "ncl2"; break;
    case icSigPreview0Tag: return "pre0"; break;
    case icSigPreview1Tag: return "pre1"; break;
    case icSigPreview2Tag: return "pre2"; break;
    case icSigProfileDescriptionTag: return "desc"; break;
    case 1685283693: return "dscm"; break;
    case icSigProfileSequenceDescTag: return "pseq"; break;
    case icSigProfileSequenceIdentifierTag: return "psid"; break;
    case icSigPs2CRD0Tag: return "psd0"; break;
    case icSigPs2CRD1Tag: return "psd1"; break;
    case icSigPs2CRD2Tag: return "psd2"; break;
    case icSigPs2CRD3Tag: return "psd3"; break;
    case icSigPs2CSATag: return "ps2s"; break;
    case icSigPs2RenderingIntentTag: return "ps2i"; break;
    case icSigRedColorantTag: return "rXYZ"; break;
    case icSigRedTRCTag: return "rTRC"; break;
    case icSigScreeningDescTag: return "scrd"; break;
    case icSigScreeningTag: return "scrn"; break;
    case icSigTechnologyTag: return "tech"; break;
    case icSigUcrBgTag: return "bfd"; break;
    case icSigViewingCondDescTag: return "vued"; break;
    case icSigViewingConditionsTag: return "view"; break;
    case 1147500100: return "DevD"; break;
    case 1128875332: return "CIED"; break;
    case 1349350514: return "Pmtr"; break;
    case 1986226036: return "vcgt"; break;
    case 1667785060: return "chad"; break;
    case icSigChromaticityType: return "chrm"; break;
    case 1668051567: return "clro"; break;
    case 1668051572: return "clrt"; break;
    case 0x62303135: return "b015"; break; /* binuscan targ data */
    case 0: return "----"; break;
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }
  return "-";
}



/** @func  oyICCDeviceClassDescription
 *  @brief get the ICC profile icProfileClassSignature description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCDeviceClassDescription ( icProfileClassSignature sig )
{
  switch (sig)
  {
    case icSigInputClass: return _("Input"); break;
    case icSigDisplayClass: return _("Display"); break;
    case icSigOutputClass: return _("Output"); break;
    case icSigLinkClass: return _("Link"); break;
    case icSigAbstractClass: return _("Abstract"); break;
    case icSigColorSpaceClass: return _("Colour Space"); break;
    case icSigNamedColorClass: return _("Named Colour"); break;
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }
  return "-";
}

/** @func  oyICCPlatformDescription
 *  @brief get the ICC profile platform description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCPlatformDescription ( icPlatformSignature platform )
{
  switch (platform)
  {
    case icSigMacintosh: return _("Macintosh"); break;
    case icSigMicrosoft: return _("Microsoft"); break;
    case icSigSolaris: return _("Solaris"); break;
    case icSigSGI: return _("SGI"); break;
    case icSigTaligent: return _("Taligent"); break;
    case icSigUnix: return _("Unix and derivatives"); break;
    default: { icUInt32Number i = oyValueUInt32(platform);
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }
  return "-";
}


/** @func  oyICCTagTypeName
 *  @brief get tag type ~4 char name
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagTypeName    ( icTagTypeSignature  sig )
{

  switch (sig) {
    case icSigCurveType: return "curv"; break;
    case icSigDataType: return "data"; break;
    case icSigDateTimeType: return "dtim"; break;
    case icSigLut16Type: return "mft2"; break;
    case icSigLut8Type: return "mft1"; break;
    case icSigMeasurementType: return "meas"; break;
    case icSigNamedColorType: return "ncol"; break;
    case icSigProfileSequenceDescType: return "pseq"; break;
    case icSigProfileSequenceIdentifierType: return "psid"; break;
    case icSigS15Fixed16ArrayType: return "sf32"; break;
    case icSigScreeningType: return "scrn"; break;
    case icSigSignatureType: return "sig"; break;
    case icSigTextType: return "text"; break;
    case icSigTextDescriptionType: return "desc"; break;
    case icSigU16Fixed16ArrayType: return "uf32"; break;
    case icSigUcrBgType: return "bfd"; break;
    case icSigUInt16ArrayType: return "ui16"; break;
    case icSigUInt32ArrayType: return "ui32"; break;
    case icSigUInt64ArrayType: return "ui64"; break;
    case icSigUInt8ArrayType: return "ui08"; break;
    case icSigViewingConditionsType: return "view"; break;
    case icSigXYZType: return "XYZ"; break;
    /*case icSigXYZArrayType: return "XYZ"; break;*/
    case icSigNamedColor2Type: return "ncl2"; break;
    case icSigCrdInfoType: return "crdi"; break;
    case icSigChromaticityType: return "chrm"; break;
    case 1986226036: return "vcgt"; break;
    case icSigCopyrightTag: return "cprt?"; break; /*??? (Imacon)*/
    case 1835824483: return "mluc"; break;
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }

  return "-";
}

/** @func  oyICCTechnologyDescription
 *  @brief get ICC profile icTechnologySignature description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTechnologyDescription ( icTechnologySignature sig )
{
  switch (sig) {
    case icSigDigitalCamera: return _("Digital camera"); break; /*dcam*/
    case icSigFilmScanner: return _("Film scanner"); break; /*fscn*/
    case icSigReflectiveScanner: return _("Reflective scanner"); break; /*rscn*/
    case icSigInkJetPrinter: return _("InkJet printer"); break; /*ijet*/
    case icSigThermalWaxPrinter: return _("Thermal wax printer"); break; /*twax*/
    case icSigElectrophotographicPrinter: return _("Electrophotographic printer"); break; /*epho*/
    case icSigElectrostaticPrinter: return _("Electrostatic printer"); break; /*esta*/
    case icSigDyeSublimationPrinter: return _("Dye sublimation printer"); break; /*dsub*/
    case icSigPhotographicPaperPrinter: return _("Photographic paper printer"); break; /*rpho*/
    case icSigFilmWriter: return _("Film writer"); break; /*fprn*/
    case icSigVideoMonitor: return _("Video Monitor"); break; /*vidm*/
    case icSigVideoCamera: return _("Video camera"); break; /*vidc*/
    case icSigProjectionTelevision: return _("Projection Television"); break; /*pjtv*/
    case icSigCRTDisplay: return _("Cathode ray tube display"); break; /*CRT*/
    case icSigPMDisplay: return _("Passive matrix monitor"); break; /*PMD*/
    case icSigAMDisplay: return _("Active matrix monitor"); break; /*AMD*/
    case icSigPhotoCD: return _("Photo CD"); break; /*KPCD*/
    case icSigPhotoImageSetter: return _("PhotoImageSetter"); break; /*imgs*/
    case icSigGravure: return _("Gravure"); break; /*grav*/
    case icSigOffsetLithography: return _("Offset Lithography"); break; /*offs*/
    case icSigSilkscreen: return _("Silkscreen"); break; /*silk*/
    case icSigFlexography: return _("Flexography"); break; /*flex*/
    /*case icMaxEnumTechnology: return _("----"); break;*/
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
               break;
             }
  }
  return "-";
}

/** @func  oyICCChromaticityColorantDescription
 *  @brief get ICC profile Chromaticity Colorant description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCChromaticityColorantDescription ( icSignature sig )
{
  switch (sig) {
    case 0: return ""; break;
    case 1: return _("ITU-R BT.709"); break;
    case 2: return _("SMPTE RP145-1994"); break;
    case 3: return _("EBU Tech.3213-E"); break;
    case 4: return _("P22"); break;

    default: return _("???"); break;
  }
  return "-";
}

/** @func  oyICCIlluminantDescription
 *  @brief get ICC profile illuminant description
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2005/00/00 (Oyranos: 0.1.8)
 */
const oyChar *   oyICCIlluminantDescription ( icIlluminant sig )
{
  switch (sig) {
    case icIlluminantUnknown: return _("Illuminant unknown"); break;
    case icIlluminantD50: return _("Illuminant D50"); break;
    case icIlluminantD65: return _("Illuminant D65"); break;
    case icIlluminantD93: return _("Illuminant D93"); break;
    case icIlluminantF2: return _("Illuminant F2"); break;
    case icIlluminantD55: return _("Illuminant D55"); break;
    case icIlluminantA: return _("Illuminant A"); break;
    case icIlluminantEquiPowerE: return _("Illuminant with equal energy E"); break;
    case icIlluminantF8: return _("Illuminant F8"); break;
    /*case icMaxEnumIluminant: return _("Illuminant ---"); break;*/

    default: return _("???"); break;
  }
  return "-";
}

/** @func  oyICCStandardObserverDescription
 *  @brief get ICC profile Standard Observer description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCStandardObserverDescription ( icStandardObserver sig )
{
  switch (sig) {
    case icStdObsUnknown: return _("unknown"); break;
    case icStdObs1931TwoDegrees: return _("2 degree (1931)");
         break;
    case icStdObs1964TenDegrees: return _("10 degree (1964)");
         break;
    /*case icMaxStdObs: return _("---"); break;*/

    default: return _("???"); break;
  }
  return "-";
}


/** @func  oyICCMeasurementGeometryDescription
 *  @brief get ICC profile Measurement Geometry description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCMeasurementGeometryDescription ( icMeasurementGeometry sig )
{
  switch (sig) {
    case icGeometryUnknown: return _("unknown"); break;
    case icGeometry045or450: return _("0/45, 45/0"); break;
    case icGeometry0dord0: return _("0/d or d/0"); break;
    /*case icMaxGeometry: return _("---"); break;*/

    default: return _("???"); break;
  }
  return "-";
}

/** @func  oyICCMeasurementFlareDescription
 *  @brief get ICC profile Measurement Flare description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCMeasurementFlareDescription ( icMeasurementFlare sig )
{
  switch (sig) {
    case icFlare0: return _("0"); break;
    case icFlare100: return _("100"); break;
    /*case icMaxFlare: return _("---"); break;*/

    default: return _("???"); break;
  }
  return "-";
}






void
oyLab2XYZ (const double *CIELab, double * XYZ)
{
  const double * l = CIELab;
  /* double e = 216./24389.;             // 0.0088565 */
  /* double k = 24389./27.;              // 903.30 */
  double d = 6./29.;                  /* 0.20690 */

  double Xn = 0.964294;
  double Yn = 1.000000;
  double Zn = 0.825104;

  double fy = (l[0] + 16) / 116.;
  double fx = fy + l[1] / 500.;
  double fz = fy - l[2] / 200.;


  if(fy > d)
    XYZ[1] = Yn * pow( fy, 3 );
  else
    XYZ[1] = (fy - 16./116.) * 3 * pow( d, 2 ) * Yn;
  if(fx > d)
    XYZ[0] = Xn * pow( fx, 3 );
  else
    XYZ[0] = (fx - 16./116.) * 3 * pow( d, 2 ) * Xn;
  if(fz > d)
    XYZ[2] = Zn * pow( fz, 3 );
  else
    XYZ[2] = (fz - 16./116.) * 3 * pow( d, 2 ) * Zn;
}

void
oyXYZ2Lab (const double *XYZ, double * lab)
{
    /* white point D50 [0.964294 , 1.000000 , 0.825104]
     * XYZ->Lab is defined as (found with the help of Marti Maria):
     *
     * L* = 116*f(Y/Yn) - 16                     0 <= L* <= 100
     * a* = 500*[f(X/Xn) - f(Y/Yn)]
     * b* = 200*[f(Y/Yn) - f(Z/Zn)]
     *
     * and
     *
     *        f(t) = t^(1/3)                     1 >= t >  0.008856
     *         7.787*t + (16/116)          0 <= t <= 0.008856
     */

      double gamma = 1.0/3.0; /* standard is 1.0/3.0 */
      double XYZ_[3];
      double K = 24389./27.;
      double k = K/116.;      /* 7.787 */
      double e = 216./24389.; /* 0.008856 */
      int i;

      /* CIE XYZ -> CIE*Lab (D50) */
      XYZ_[0] = XYZ[0] / 0.964294;
      XYZ_[1] = XYZ[1] / 1.000000;
      XYZ_[2] = XYZ[2] / 0.825104;

      for(i = 0; i < 3; ++i)
      {
        if ( XYZ_[i] > e)
           XYZ_[i] = pow (XYZ_[i], gamma);
        else
           XYZ_[i] = k * XYZ_[i] + (16.0/116.0);
      }

      lab[0] = (116.0*  XYZ_[1] -  16.0);
      lab[1] = (500.0*( XYZ_[0] -  XYZ_[1]));
      lab[2] = (200.0*( XYZ_[1] -  XYZ_[2]));
}

/** @func oyCIEabsXYZ2ICCrelXYZ
 *  @brief CIE absolute colourimetric to ICC relative colorimetric
 *
 *  @param[in]     CIEXYZ              CIE absolute colourimetric input
 *  @param[out]    ICCXYZ              ICC relative colourimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    ICCXYZ[i] = (CIEXYZ[i] - XYZmin[i]) / (XYZmax[i] - XYZmin[i]) * XYZwhite[i];
}

/** @func oyICCXYZrel2CIEabsXYZ
 *  @brief ICC relative colourimetric to CIE absolute colorimetric
 *
 *  @param[in]     ICCXYZ              ICC relative colourimetric input
 *  @param[out]    CIEXYZ              CIE absolute colourimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    CIEXYZ[i] = (ICCXYZ[i] * (XYZmax[i] - XYZmin[i]) + XYZmin[i]) / XYZwhite[i];
}

#define PT_ANY       0    /* Don't check colorspace */
                          /* 1 & 2 are reserved */
#define PT_GRAY      3
#define PT_RGB       4
#define PT_CMY       5
#define PT_CMYK      6
#define PT_YCbCr     7
#define PT_YUV       8      /* Lu'v' */
#define PT_XYZ       9
#define PT_Lab       10
#define PT_YUVK      11     /* Lu'v'K */
#define PT_HSV       12
#define PT_HLS       13
#define PT_Yxy       14
#define PT_HiFi      15

/** @func lcmsColorSpace
 *  @brief get lcms notation from icColorSpaceSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
int lcmsColorSpace(icColorSpaceSignature ProfileSpace)
{    
       switch (ProfileSpace) {

       case icSigGrayData: return  PT_GRAY;
       case icSigRgbData:  return  PT_RGB;
       case icSigCmyData:  return  PT_CMY;
       case icSigCmykData: return  PT_CMYK;
       case icSigYCbCrData:return  PT_YCbCr;
       case icSigLuvData:  return  PT_YUV;
       case icSigXYZData:  return  PT_XYZ;
       case icSigLabData:  return  PT_Lab;
       case icSigLuvKData: return  PT_YUVK;
       case icSigHsvData:  return  PT_HSV;
       case icSigHlsData:  return  PT_HLS;
       case icSigYxyData:  return  PT_Yxy;

       case icSig6colorData:
       case icSigMCH6Data: return PT_HiFi;

       default:  return 0;
       }
}

/** @func  oyDumpColourToCGATS
 *  @brief create CGATS text from double value array and a profile
 *
 *  The funtion operates on raw colour values. Alpha is not supported.
 *  The intention is to have a small debugging only tool. The values are
 *  required to come prescaled (IE RGB 0...255, XYZ 0...1, CMYK 0...100 ...)
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
oyChar *     oyDumpColourToCGATS     ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAllocFunc_t       allocateFunc,
                                       const oyChar      * DESCRIPTOR )
{
  int channels_n = oyProfile_GetChannelsCount( prof );
  int i;
  size_t len = n * channels_n * 16 + 2024;
  oyChar * text = allocateFunc( len );
  const oyChar * prof_name = prof?prof->file_name_:"";
  const char * format = "";
  char * daten = text;
  /*double value = 0;*/

  const char *fomate[17] = {
    "SAMPLE_ID UNKNOWN 0", /* PT_ANY */
    "SAMPLE_ID UNKNOWN 1", /* 1 */
    "SAMPLE_ID UNKNOWN 2", /* 2 */
    "SAMPLE_ID        GRAY", /* 3 PT_GRAY */
    "SAMPLE_ID        RGB_R RGB_G RGB_B", /* 4 PT_RGB */
    "SAMPLE_ID        CMY_C  CMY_M  CMY_Y", /* 5 PT_CMY */
    "SAMPLE_ID        CMYK_C  CMYK_M  CMYK_Y  CMYK_K", /* 6 PT_CMYK */
    "SAMPLE_ID        ", /* 7 PT_YCbCr */
    "SAMPLE_ID        ", /* 8 PT_YUV */
    "SAMPLE_ID        XYZ_X XYZ_Y XYZ_Z", /* 9 PT_XYZ */
    "SAMPLE_ID        LAB_L LAB_A LAB_B", /* 10 PT_Lab */
    "SAMPLE_ID        ", /* 11 PT_YUVK */
    "SAMPLE_ID        ", /* 12 PT_HSV */
    "SAMPLE_ID        ", /* 13 PT_HLS */
    "SAMPLE_ID        ", /* 14 PT_Yxy */
    "SAMPLE_ID        ", /* 15 PT_HiFi */
    0
  };

  icColorSpaceSignature cspace = oyProfile_GetSignature( prof, oySIGNATURE_COLOUR_SPACE );

  int lcms_space = lcmsColorSpace( cspace );
  const char * save_locale = setlocale(LC_NUMERIC, 0 );

  format = fomate[lcms_space];

  n *= channels_n;

  oySprintf_( daten ,
              "OYCGATS\n"
              "\n"
              "ORIGINATOR        \"%s\"\n"
              "DESCRIPTOR        \"%s\"\n"
              "CREATED           \"%s\"\n"
              "\n"
              "NUMBER_OF_FIELDS  %d\n"
              "BEGIN_DATA_FORMAT\n"
              "%s\n"
              "END_DATA_FORMAT\n"
              "\n"
              "NUMBER_OF_SETS    %d\n"
              "BEGIN_DATA\n",
              prof_name?prof_name:"--",
              DESCRIPTOR?DESCRIPTOR:"--",
              "--",
              channels_n + 1,
              format,
              (int)n/channels_n
            );

  setlocale(LC_NUMERIC, "C");

    for(i = 0; i < n; ++i)
    {
      int modulo_k = i%(channels_n);
      if(modulo_k)
        oySprintf_( &daten[oyStrlen_(daten)], "       ");
      else
      {
        if(i)
          oySprintf_( &daten[oyStrlen_(daten)], "\n");
        oySprintf_( &daten[strlen(daten)], "%d     ", (i/channels_n) + 1);
      }
#if 0
      switch(lcms_space)
      {
        case PT_ANY:
        case PT_GRAY:
        case PT_YCbCr:
        case PT_YUV:
        case PT_XYZ:
        case PT_YUVK:
        case PT_HSV:
        case PT_HLS:
        case PT_Yxy:
             value = channels[i]; break;
        case PT_RGB:
             value = channels[i] * 255.; break;
        case PT_CMY:
        case PT_CMYK:
        case PT_HiFi:
             value = channels[i] * 100.; break;
        case PT_Lab:
             if(modulo_k == 0)
               value = channels[i] * 100.;
             else
               value = channels[i] * 255. - 127.;
             break;
        default: value = channels[i]; break;
      }
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", value);
#else
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", channels[i] );
#endif
    }
    oySprintf_( &daten[oyStrlen_(daten)], "\nEND_DATA\n");

  setlocale(LC_NUMERIC, save_locale);
    
  return text;
}

/** @brief copy pure colours

    handle colour only, without from set to -1 default
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
void
oyCopyColour ( const double * from,
               double       * to,
               int            n,
               oyProfile_s  * ref,
               int            channels_n )
{
  int i, j;
  icColorSpaceSignature sig = 0;
  int c = 0;
  int error = 0;

  if(!n || !to)
    return;

  if(ref)
    sig = oyProfile_GetSignature ( ref, oySIGNATURE_COLOUR_SPACE );
  else
    sig = icSigXYZData;

  c = oyICCColourSpaceGetChannelCount( sig );

  if(from)
    error = !memcpy( to, from, sizeof(double) * (n * c) );

  else
  {
    if(!channels_n && c)
      channels_n = c;
    else if(channels_n && !c)
      c = channels_n;

    if(channels_n)
    switch(sig)
    {
      case icSigLabData:
      case icSigLuvData:
      case icSigHsvData:
      case icSigHlsData:
      case icSigYCbCrData:
           for( i = 0; i < n; ++i )
           {
             to[i*channels_n+0] = -1;
             to[i*channels_n+1] = 0;
             to[i*channels_n+2] = 0;
             for( j = c; j < channels_n; ++j )
               if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
      case icSigXYZData:
      case icSigRgbData:
      default:
           for( i = 0; i < n; ++i )
           {
             for( j = 0; j < channels_n; ++j )
               if(j < c)
                 to[i*channels_n+j] = -1;
               else if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
    }
  }
}

/** @internal
 *  @brief deallocation and resetting
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int          oyPointerReleaseFunc_   ( oyPointer         * ptr )
{
  oyDeAllocateFunc_( *ptr );
  *ptr = 0;
  return 0;
}

#define oyPointerRelease_m( ptr_ ) oyPointerReleaseFunc_((oyPointer*)ptr_)

/** @} */





/* --- Object handling API's start ------------------------------------ */


/** \addtogroup misc Miscellaneous

 *  @{
 */

oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size )
{
  oyAllocFunc_t allocateFunc = oyAllocateFunc_;

  if(st && st->oy_ && st->oy_->allocateFunc_)
    allocateFunc = st->oy_->allocateFunc_;

  return allocateFunc( size );
}

/** @func  oyStruct_TypeToText
 *  @brief Objects type to small string
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
const char *     oyStruct_TypeToText ( oyStruct_s        * oy_struct )
{
  const char * text = 0;

  if(oy_struct && oy_struct->type_)
    switch(oy_struct->type_) {
    case oyOBJECT_TYPE_NONE: text = "Zero - none"; break;
    case oyOBJECT_TYPE_OBJECT_S: text = "oyObject_s"; break;
    case oyOBJECT_TYPE_DISPLAY_S: text = "oyDisplay_s"; break;
    case oyOBJECT_TYPE_NAMED_COLOUR_S: text = "oyNamedColour_s"; break;
    case oyOBJECT_TYPE_NAMED_COLOURS_S: text = "oyNamedColours_s"; break;
    case oyOBJECT_TYPE_PROFILE_S: text = "oyProfile_s"; break;
    case oyOBJECT_TYPE_PROFILE_TAG_S: text = "oyProfileTag_s"; break;
    case oyOBJECT_TYPE_PROFILE_LIST_S: text = "oyProfileList_s"; break;
    case oyOBJECT_TYPE_OPTION_S: text = "oyOption_s"; break;
    case oyOBJECT_TYPE_OPTIONS_S: text = "oyOptions_s"; break;
    case oyOBJECT_TYPE_WIDGET_S: text = "oyWidget_s"; break;
    case oyOBJECT_TYPE_WIDGET_BUTTON_S: text = "oyWidgetButton_s"; break;
    case oyOBJECT_TYPE_WIDGET_CHOICE_S: text = "oyWidgetChoice_s"; break;
    case oyOBJECT_TYPE_WIDGET_GROUP_S: text = "oyWidgetGroup_s"; break;
    case oyOBJECT_TYPE_WIDGET_SLIDER_S: text = "oyWidgetSlider_s"; break;
    case oyOBJECT_TYPE_WIDGET_TEXT_S: text = "oyWidgetText_s"; break;
    case oyOBJECT_TYPE_REGION_S: text = "oyRegion_s"; break;
    case oyOBJECT_TYPE_IMAGE_S: text = "oyImage_s"; break;
    case oyOBJECT_TYPE_COLOUR_CONVERSION_S: text = "oyColourConversion_s";break;
    case oyOBJECT_TYPE_FILTER_S: text = "oyFilter_s"; break;
    case oyOBJECT_TYPE_FILTERS_S: text = "oyFilters_s"; break;
    case oyOBJECT_TYPE_CONVERSIONS_S: text = "oyConversions_s"; break;
    case oyOBJECT_TYPE_CMM_HANDLE_S: text = "oyCMMhandle_s"; break;
    case oyOBJECT_TYPE_CMM_POINTER_S: text = "oyCMMptr_s"; break;
    case oyOBJECT_TYPE_CMM_INFO_S: text = "oyCMMInfo_s"; break;
    case oyOBJECT_TYPE_CMM_API_S: text = "oyCMMapi_s generic"; break;
    case oyOBJECT_TYPE_CMM_API1_S: text = "oyCMMapi1_s old CMM"; break;
    case oyOBJECT_TYPE_CMM_API2_S: text = "oyCMMapi2_s Monitors"; break;
    case oyOBJECT_TYPE_CMM_API3_S: text = "oyCMMapi3_s Profile tags"; break;
    case oyOBJECT_TYPE_CMM_API4_S: text = "oyCMMapi4_s Filter"; break;
    case oyOBJECT_TYPE_CMM_API_MAX: text = "not defined"; break;
    case oyOBJECT_TYPE_ICON_S: text = "oyIcon_s"; break;
    case oyOBJECT_TYPE_MODULE_S: text = "oyModule_s"; break;
    case oyOBJECT_TYPE_EXTERNFUNC_S: text = "oyExternFunc_s"; break;
    case oyOBJECT_TYPE_NAME_S: text = "oyName_s"; break;
    case oyOBJECT_TYPE_COMP_S_: text = "oyComp_s_"; break;
    case oyOBJECT_TYPE_FILE_LIST_S_: text = "oyFileList_s_"; break;
    case oyOBJECT_TYPE_HASH_S: text = "oyHash_s"; break;
    case oyOBJECT_TYPE_HANDLE_S: text = "oyHandle_s"; break;
    case oyOBJECT_TYPE_STRUCT_LIST_S: text = "oyStructList_s"; break;
    case oyOBJECT_TYPE_MAX: text = "Max - none"; break;
    default: text = "unknown"; break;
    }

  return text;
}


/** @brief oyName_s new
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_new              ( oyObject_s          object )
{
  oyAllocFunc_t allocateFunc = oyAllocateFunc_;
  /* ---- start of object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_NAME_S;
# define STRUCT_TYPE oyName_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  s = (STRUCT_TYPE*)allocateFunc(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;

  s->copy = (oyStruct_CopyF_t) oyName_copy;
  s->release = (oyStruct_ReleaseF_t) oyName_release;
# undef STRUCT_TYPE
  /* ---- end of object constructor ------- */

  return s;
}

/** @brief oyName_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_copy               ( oyName_s        * obj,
                                         oyObject_s        object )
{
  int error = 0;
  oyName_s * s = 0;
  oyAllocFunc_t   allocateFunc   = oyAllocateFunc_;
  oyDeAllocFunc_t deallocateFunc = oyDeAllocateFunc_;

  if(!obj)
    return 0;

  if(object)
  {
    allocateFunc = object->allocateFunc_;
    deallocateFunc = object->deallocateFunc_;
  }

  if(obj->name)
    s = oyName_set_ ( s, obj->name, oyNAME_NAME, allocateFunc, deallocateFunc );
  if(obj->nick)
    s = oyName_set_ ( s, obj->nick, oyNAME_NICK, allocateFunc, deallocateFunc );
  if(obj->description)
    s = oyName_set_ ( s, obj->description, oyNAME_DESCRIPTION, allocateFunc, deallocateFunc );

  if(!s)
    s = oyName_new(0);

  error = !memcpy( s->lang, obj->lang, 8 );

  return s;
}

/** @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
int          oyName_release          ( oyName_s         ** obj )
{
  int error = 0;

  if(!obj || !*obj)
    return 0;

  error = oyName_release_(obj, oyDeAllocateFunc_);

  *obj = 0;

  return error;
}

/** @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int          oyName_release_         ( oyName_s       ** obj,
                                       oyDeAllocFunc_t   deallocateFunc )
{
  /* ---- start of common object destructor ----- */
  oyName_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type != oyOBJECT_TYPE_NAME_S)
  {
    WARNc_S(("Attempt to release a non oyName_s object."))
    return 1;
  }
  /* ---- end of common object destructor ------- */

  *obj = 0;

  if(!deallocateFunc)
    return 0;

  if(s->nick)
    deallocateFunc(s->nick); s->nick = 0;

  if(s->name)
    deallocateFunc(s->name); s->name = 0;

  if(s->description)
    deallocateFunc(s->description); s->description = 0;

  deallocateFunc( s );

  return 0;
}


/** @brief naming plus automatic allocation
 *
 *  @param[in]    object         the oyName_s struct
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    type           the kind of name
 *  @param[in]    allocateFunc   memory management (defaults to oyAllocateFunc_)
 *  @param[in]    deallocateFunc optional memory management 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAllocFunc_t       allocateFunc,
                                       oyDeAllocFunc_t     deallocateFunc )
{
  int error = 0;
  oyName_s * s = obj;

  if(obj && obj->type != oyOBJECT_TYPE_NAME_S)
  {
    WARNc_S(("Attempt to edit a non oyName_s object."))
    return 0;
  }

  if(!s)
    oyAllocHelper_m_( s, oyName_s, 1, allocateFunc, error = 1 );

  if(!s) return 0;

  s->type = oyOBJECT_TYPE_NAME_S;

  {
#define oySetString_(n_type)\
    if(!error) { \
      if(s->n_type && deallocateFunc) \
        deallocateFunc( s->n_type ); \
      s->n_type = oyStringCopy_( text, allocateFunc ); \
      if( !s->n_type ) error = 1; \
    } 
    switch (type) {
    case oyNAME_NICK:
         oySetString_(nick) break;
    case oyNAME_NAME:
         oySetString_(name) break;
    case oyNAME_DESCRIPTION:
         oySetString_(description) break;
    default: break;
    }
#undef oySetString_
  }

  return s;
}





/** @internal
 *  @brief create a new pointer list
 *
 *  @param[in]     ptrCopy             This function is later called with the 
 *                                     pointer as the first arg. The second arg  *                                     will be zero.
 *  @param[in]     ptrRelease          A function to later release.
 *                                     With ptr_type part of oyOBJECT_TYPE_e,
 *                                     the pointer to the pointer will be given
 *                                     as argument (void)(*)(void**),
 *                                     otherwise the simple
 *                                     pointer (void)(*)(void*).
 *  @return                            on success a empty list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
oyStructList_s * oyStructList_New    ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_STRUCT_LIST_S;
# define STRUCT_TYPE oyStructList_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;

  s->copy = (oyStruct_CopyF_t) oyStructList_Copy;
  s->release = (oyStruct_ReleaseF_t) oyStructList_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief oyStructList_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 december 2007 (API 0.1.8)
 */
oyStructList_s * oyStructList_Copy   ( oyStructList_s    * list,
                                       oyObject_s          obj )
{
  oyStructList_s * s = 0;
  int error = !list;

  if(!list)
    return s;

  if(list && !obj)
  {
    s = list;
    s->oy_ = oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyStructList_New(obj);

  error = !s;

  oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(list)
  {
    int i;

    s->n_reserved_ = (list->n_ > 10) ? (int)(list->n_ * 1.5) : 10;
    s->n_ = list->n_;
    s->ptr_ = oyAllocateFunc_( sizeof(int*) * s->n_reserved_ );

    for(i = 0; i < list->n_; ++i)
      if(list->ptr_[i]->copy)
        s->ptr_[i] = list->ptr_[i]->copy( list->ptr_[i], 0 );
  }

  oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return s;
}

/** @internal
 *  @brief oyStructList_s pointer add
 *
 *  @param[in]     list                the list
 *  @param[in]     ptr                 the handle this function takes ownership
 *                                     of
 *  @param[in]     pos                 the prefered position in the list. This
 *                                     option has affect of the order in list.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
int              oyStructList_MoveIn ( oyStructList_s    * list,
                                       oyStruct_s       ** ptr,
                                       int                 pos)
{
  oyStructList_s * s = list;
  int error = 0;
  int i;
  int set = 0;

  s = list;
  error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
      error = 1;

  if(!error)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error)
    error = !(ptr && *ptr && (*ptr)->type_);

  if(!error)
    /* search for a empty pointer and set */
    if(0 > pos || pos >= s->n_)
    {
      for(i = 0; i < s->n_; ++i)
        if(s->ptr_[i] == 0)
        {
          s->ptr_[i] = *ptr;
          set = 1;
        }
    }

  if(!error && !set)
  {
    int mult = (s->n_<7) ? 10 : (int)(s->n_ * 1.5);
    int len = 0;
    oyStruct_s ** tmp = 0;
    int real_copy = 0;

    /* reserve new memory */
    if(s->n_ >= s->n_reserved_)
    {
      len =  sizeof(oyPointer) * mult;
      s->n_reserved_ = mult;
      tmp = oyAllocateFunc_(len);

      error = !tmp;

      if(!error)
        error = !memset( tmp, 0, len );
      real_copy = 1;

    } else {

      tmp = s->ptr_;

    }

    /* position the new */
    if(pos < 0 || pos >= s->n_)
      pos = s->n_;

    /* sort the old to the new */
    {
      int j; 

      if(!error)
      if(pos < s->n_ || real_copy)
      for(i = s->n_; i >= 0; --i)
      {
        j = (i >= pos) ? i-1 : i;
        if(i != pos)
          /*if(real_copy)
            tmp[i] = oyHandle_copy_( s->ptr_[j], 0 );
          else*/
            tmp[i] = s->ptr_[j];
      }

      if(!error)
      {
        tmp[pos] = /*oyHandle_copy_(*/ *ptr/*, 0 )*/;
      }

      /* release old data */
      if(!error)
      {
        if(real_copy)
        {
          if(s->ptr_)
            oyDeAllocateFunc_(s->ptr_);
          s->ptr_ = tmp;
        }
      }

      /* set the final count */
      if(!error)
        ++s->n_;
    }

    *ptr = 0;
  }

  if(!error)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @internal
 *  @brief oyStructList_s release
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyStructList_Release (oyStructList_s   ** obj )
{
  int i,
      error = 0;
  /* ---- start of common object destructor ----- */
  oyStructList_s * s = 0;

  if(!obj || !*obj)
    return 1;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
  {
    WARNc_S(("Attempt to release a non oyStructList_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  for(i = s->n_ - 1; i >= 0; --i)
    oyStructList_ReleaseAt( s, i );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->ptr_)
      deallocateFunc(s->ptr_);

    if(s->list_name)
      deallocateFunc(s->list_name);

    oyObject_Release( &s->oy_ );

    s->type_ = 0;

    deallocateFunc( s );
  }

  return error;
}

/** @func    oyStructList_ReleaseAt
 *  @brief   oyStructList_s pointer release
 *  @internal
 *
 *  release and shrink
 *
 *  @version Oyranos: 0.1.8
 *  @date    2007/11/21
 *  @since   2007/11/21 (Oyranos: 0.1.8)
 */
int            oyStructList_ReleaseAt( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s * s = list;
  int error = 0;

  error = !s;

  if(s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && list)
  {
      if(0 <= pos && pos < s->n_)
      {
          if(s->ptr_[pos])
            s->ptr_[pos]->release( (oyStruct_s**)&s->ptr_[pos] );

          if(pos < s->n_ - 1)
            error = !memmove( &s->ptr_[pos], &s->ptr_[pos+1],
                              sizeof(oyStruct_s*) * s->n_ - pos - 1);

          --s->n_;
      }
  }

  if(!error)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s    * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(!error)
  if(s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
    n = s->n_;

  if(!error)
  if(pos >= 0 && n > pos && s->ptr_[pos])
    obj = s->ptr_[pos];

  return obj;
}

/** @func oyStructList_GetType_
 *  @internal
 *  @brief oyStructList_s pointer access
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetType_(oyStructList_s    * list,
                                       int                 pos,
                                       oyOBJECT_TYPE_e     type )
{
  oyStruct_s * obj = oyStructList_Get_( list, pos );

  if(obj && obj->type_ != type)
    obj = 0;
  return obj;
}

/** @internal
 *  @brief oyStructList_s referenced pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRef ( oyStructList_s    * list,
                                       int                 pos )
{
  oyStructList_s * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  obj = oyStructList_Get_(list, pos);
  error = !obj;

  if(!error)
    error = oyStructList_ReferenceAt_(list, pos);

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return obj;
}

/** @func oyStructList_GetRefType
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetRefType( oyStructList_s * list,
                                       int                 pos,
                                       oyOBJECT_TYPE_e     type )
{
  oyStruct_s * obj = oyStructList_GetRef( list, pos );

  if(obj && obj->type_ != type)
  {
    if(obj->oy_ && obj->release)
      obj->release( &obj );
    obj = 0;
  }

  return obj;
}

/** @internal
 *  @brief oyStructList_s pointer referencing
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_( oyStructList_s * list,
                                       int                 pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s * s = list;
  oyStruct_s * p = 0;

  if(s)
    n = s->n_;
  else
    error = 1;

  if(!error)
  if(s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
  if(pos >= 0 && n > pos && s->ptr_[pos])
  {
    p = s->ptr_[pos];
    error = !(p && p->copy);

    if(!error)
      p = p->copy( p, 0 );
  }

  return !p;
}

/** @internal
 *  @brief oyStructList_s pointer access
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
/*oyHandle_s **    oyStructList_GetRaw_( oyStructList_s    * list )
{
  int error = 0;
  oyStructList_s * s = list;
  oyHandle_s ** p = 0;

  if(!error)
  if(s->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
    p = s->ptr_;

  return p;
}*/

/** @internal
 *  @brief oyStructList_s count
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
int              oyStructList_Count ( oyStructList_s   * list )
{
  int n = 0;
  oyStructList_s * s = list;
  int error = 0;

  if(!(s && s->type_ == oyOBJECT_TYPE_STRUCT_LIST_S))
    error = 1;

  if(!error)
    n = s->n_;

  return n;
}

/** @} */


/** \addtogroup cmm_handling CMM handling

 *  @{
 */


/** @internal
 *  @brief oyCMMptr_s allocator
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyCMMptr_s*
oyCMMptr_New_ ( oyAllocFunc_t     allocateFunc )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  s = oyAllocateWrapFunc_( sizeof(oyCMMptr_s), allocateFunc );
  error = !s;

  error = !memset(s, 0, sizeof(oyCMMptr_s));

  if(!error)
  {
    s->type = oyOBJECT_TYPE_CMM_POINTER_S;
    s->copy = (oyStruct_CopyF_t) oyCMMptr_Copy_;
    s->release = (oyStruct_ReleaseF_t) oyCMMptr_Release_;
  }

  ++s->ref;

  return s;
}

/** @internal
 *  @brief oyCMMptr_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
oyCMMptr_s *       oyCMMptr_Copy_    ( oyCMMptr_s        * cmm_ptr,
                                       oyAllocFunc_t       allocateFunc )
{
  oyCMMptr_s * s = cmm_ptr;
  int error = 0;

  error = !s;

  if(!error && s && s->type != oyOBJECT_TYPE_CMM_POINTER_S)
    error = 1;

  if(!error)
    ++s->ref;
  else
    s = 0;

  return s;
}

/** @internal
 *  @brief oyCMMptr_s release
 *
 *  @since Oyranos: version 0.1.8
 *  @date  30 november 2007 (API 0.1.8)
 */
int                oyCMMptr_Release_ ( oyCMMptr_s       ** obj )
{
  oyCMMptr_s * s = 0;
  int error = 0;

  if(!obj || !*obj)
    error = 1;

  if(!error)
  s = *obj;

  error = !s;

  if(!error && s && s->type != oyOBJECT_TYPE_CMM_POINTER_S)
    error = 1;

  if(!error)
    *obj = 0;

  if(!error)
    if(--s->ref)
      return error;

  if(!error)
  {
    s->type = 0;

    if(s->ptr)
    {
      if(s->ptrRelease)
        error = s->ptrRelease( &s->ptr );
      else
        error = oyPointerRelease_m( &s->ptr );

      if(!error)
        error = oyCMMdsoRelease_( s->cmm );
    }
    s->ptrRelease = 0;
    oyPointerRelease_m( &s );
  }

  return error;
}

/** @internal
 *  @brief set oyCMMptr_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
int                oyCMMptr_Set_     ( oyCMMptr_s        * cmm_ptr,
                                       const char        * cmm,
                                       const char        * func_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       oyStruct_releaseF_t ptrRelease )
{
  oyCMMptr_s * s = cmm_ptr;
  int error = !s;

  if(!error && cmm)
    memcpy(s->cmm, cmm, 4);
  s->cmm[4] = 0;

  if(!error && func_name)
    if(oyStrlen_(func_name) < 32)
      oySprintf_(s->func_name, func_name); 

  if(!error && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &ptr );
    s->ptr = ptr;
  }

  if(!error && resource)
    if(oyStrlen_(resource) < 5)
      oySprintf_(s->resource, resource); 

  if(!error && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}








oyStructList_s * oy_cmm_handles_ = 0;
/** @internal
 *  @brief Oyranos wrapper for dlopen
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int oyDlclose(oyPointer* handle)
{
  if(handle && *handle)
  {
    dlclose(*handle);
    *handle = 0;
    return 0;
  }
  return 1;
}

/** @internal
 *  @brief register Oyranos CMM dlopen handle
 *
 *  use in pair with oyCMMdsoRelease_
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyCMMdsoReference_    ( const char        * cmm,
                                     oyPointer           ptr )
{
  int i, n;
  int found = 0;
  int error = 0;

  if(!oy_cmm_handles_)
  {
    oy_cmm_handles_ = oyStructList_New( 0 );
    error = !oy_cmm_handles_;
  }

  if(!error && oy_cmm_handles_->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyCMMptr_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_TYPE_CMM_POINTER_S)
      s = (oyCMMptr_s*) obj;

    if( s && s->cmm && cmm &&
        !oyStrcmp_( s->cmm, cmm ) )
    {
      found = 1;
      oyStructList_ReferenceAt_(oy_cmm_handles_, i);
      if(ptr)
      {
        if(!s->ptr)
          s->ptr = ptr;
        /*else
          WARNc_S(("Attempt to register dso handle multiple times."));*/
      }
    }
  }

  if(!found)
  {
    oyCMMptr_s * s = oyCMMptr_New_(oyAllocateFunc_);
    oyStruct_s * oy_cmm_struct = 0;

    error = !s;

    if(!error)
      error = oyCMMptr_Set_( s, cmm, "oyDlclose", 0, ptr, oyDlclose );

    if(!error)
      oy_cmm_struct = (oyStruct_s*) s;

    if(!error)
      oyStructList_MoveIn(oy_cmm_handles_, &oy_cmm_struct, -1);
  }

  return error;
}

/** @internal
 *  @brief search a dlopen handle for a Oyranos CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int          oyCMMdsoSearch_         ( const char        * cmm )
{
  int i, n;
  int pos = -1;
  int error = 0;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_(oy_cmm_handles_, i);
    oyCMMptr_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_TYPE_CMM_POINTER_S)
      s = (oyCMMptr_s*) obj;

    error = !s;

    if(!error)
    if( s->cmm && cmm &&
        !oyStrcmp_( s->cmm, cmm ) )
      pos = i;;
  }

  return pos;
}

/** @internal
 *  @brief release Oyranos CMM dlopen handle
 *
 *  use in pair in oyCMMdsoReference_
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyCMMdsoRelease_      ( const char        * cmm )
{
  int error = 0;

  /* inactive */
#if 0
  int found = -1;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
    found = oyCMMdsoSearch_(cmm);

  if(found >= 0)
    oyStructList_ReleaseAt(oy_cmm_handles_, found);
#endif
  return error;
}

/** @internal
 *  @brief get Oyranos CMM dlopen handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name )
{
  int found = -1;
  oyPointer dso_handle = 0;

  if(!lib_name)
    return 0;

  found = oyCMMdsoSearch_(cmm);

  if(found >= 0)
  {
    oyCMMptr_s * s = (oyCMMptr_s*)oyStructList_GetType_( oy_cmm_handles_, found,
                                                  oyOBJECT_TYPE_CMM_POINTER_S );

    if(s)
      dso_handle = s->ptr;
  }

  if(!dso_handle)
  {
    dso_handle = dlopen(lib_name, RTLD_LAZY);

    if(!dso_handle)
      WARNc_S((dlerror()))
  }

  if(dso_handle)
    oyCMMdsoReference_( cmm, dso_handle );

  return dso_handle;
}



/** @internal
 *  @brief new module handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *    oyCMMhandle_New_    ( oyObject_s        object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_CMM_HANDLE_S;
# define STRUCT_TYPE oyCMMhandle_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyCMMhandle_Copy_;
  s->release = (oyStruct_ReleaseF_t) oyCMMhandle_Release_;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief copy module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMhandle_Copy_     ( oyCMMhandle_s   * handle,
                                         oyObject_s        object )
{
  int error = !handle;
  oyCMMhandle_s * s = 0;

  if(!error && object)
  {
    s = oyCMMhandle_New_(object);
    error = !s;
    if(!error)
    {
      if(handle)
        WARNc1_S("Dont know how to copy CMM[%s] handle.", handle->cmm);

      error = oyCMMhandle_Set_( s, 0, 0 );
    } else
      WARNc_S("Could not create a new object.");

  } else {

    oyObject_Copy( handle->oy_ );
    s = handle;

  }

  return s;
}

/** @internal
 *  @brief release module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMhandle_Release_( oyCMMhandle_s    ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyCMMhandle_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_CMM_HANDLE_S)
  {
    WARNc_S("Attempt to release a non oyCMMhandle_s object.")
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  error = oyCMMdsoRelease_( s->cmm );

  s->dso_handle = 0;
  s->info = 0;
  s->cmm[0] = 0;


  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMInfo_s       * info,
                                       oyPointer           dso_handle )
{
  int error = 0;

  if(!handle || !info || !dso_handle)
    error = 1;

  if(!error)
  {
    memcpy( handle->cmm, info->cmm, 4 );
    handle->info = info;
    oyCMMdsoReference_( handle->cmm, dso_handle );
    handle->dso_handle = dso_handle;
  }

  return error;
}



/** @internal
 *  @brief internal Oyranos module handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_infos_ = 0;

/** @internal
 *  @brief search a Oyranos module handle in a internal list
 *
 *  a intermediate step
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * cmm )
{
  int error = !cmm;
  int n, i;
  oyCMMhandle_s * cmm_handle = 0;

  if(!error && !oy_cmm_infos_)
  {
    oy_cmm_infos_ = oyStructList_New( 0 );
    error = !oy_cmm_infos_;
  }

  if(!error && oy_cmm_infos_->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_infos_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyCMMhandle_s * cmmh = (oyCMMhandle_s*) oyStructList_GetType_(oy_cmm_infos_,
                                                i, oyOBJECT_TYPE_CMM_HANDLE_S );
    oyCMMInfo_s * s = 0;

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_TYPE_CMM_INFO_S &&
        *(uint32_t*)&s->cmm && cmm &&
        !memcmp( s->cmm, cmm, 4 ) )
    {
      cmm_handle = oyCMMhandle_Copy_( cmmh, 0 );
      error = oyStructList_ReferenceAt_( oy_cmm_infos_, i );
    }
  }

  return cmm_handle;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  11 december 2007 (API 0.1.8)
 */
char *           oyCMMnameFromLibName_(const char        * lib_name)
{
  char * cmm = 0;

  if(lib_name && oyStrlen_(lib_name))
  {
    char * tmp = oyStrstr_( lib_name, OY_MODULE_NAME );

    if(tmp)
    {
      cmm = oyAllocateFunc_(5);
      memcpy(cmm, tmp-4, 4);
      cmm[4] = 0;
    }
  }

  return cmm;
}

/** @internal
 *  @brief get all module names
 *
 *  @since Oyranos: version 0.1.8
 *  @date  12 december 2007 (API 0.1.8)
 */
char **          oyCMMsGetLibNames_  ( int               * n,
                                       const char        * required_cmm )
{
  int error = !n;
  char ** files = 0;

  if(!error)
  {
    int  files_n = 0;
    char lib_string[24];
    const char * cmm = required_cmm;

    if(required_cmm)
      oySprintf_( lib_string, "%s%s", cmm, OY_MODULE_NAME );
    else
      oySprintf_( lib_string, "%s", OY_MODULE_NAME );

    /* search for a matching module file */
    files = oyLibFilesGet_( &files_n, 0, oyUSER_SYS,
                            "cmms", lib_string, 0, oyAllocateFunc_ );
    error = !files;

    *n = files_n;
  }

  return files;
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi_s * api = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  char * cmm = oyCMMnameFromLibName_(lib_name);

  if(!error)
  {
    oyPointer dso_handle = 0;

    if(!error)
    {
      if(lib_name)
        dso_handle = oyCMMdsoGet_(cmm, lib_name);

      error = !dso_handle;

      if(error)
        WARNc_S((dlerror()));
    }

    /* open the module */
    if(!error)
    {
      char info_sym[24];
      int api_found = 0;

      oySprintf_( info_sym, "%s%s", cmm, OY_MODULE_NAME );

      cmm_info = (oyCMMInfo_s*) dlsym (dso_handle, info_sym);

      error = !cmm_info;

      if(error)
        WARNc_S((dlerror()));

      if(!error)
        if(oyCMMapi_Check_( cmm_info->api ))
          api = cmm_info->api;

      if(!error && api)
      {
        error = api->oyCMMMessageFuncSet( oyMessageFunc_p );

        cmm_handle = oyCMMhandle_New_(0);

        /* init */
        if(!error)
        error = api->oyCMMInit();
        if(!error)
          error = oyCMMhandle_Set_( cmm_handle, cmm_info, dso_handle );

        api_found = 1;
      }

      /* store */
      if(!error && api_found)
        oyStructList_MoveIn(oy_cmm_infos_, (oyStruct_s**)&cmm_handle, -1);
    }

    oyCMMdsoRelease_( cmm );
  }

  if(cmm)
    oyDeAllocateFunc_(cmm);
  cmm = 0;

  return cmm_info;
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMInfoAtListFromLibName_(const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  int found = 0;
  char * cmm = oyCMMnameFromLibName_(lib_name);

  if(!error)
  {
    cmm_handle = oyCMMFromCache_(cmm);
    if(cmm_handle && cmm_handle->info->api)
    {
      cmm_info = cmm_handle->info;
      found = 1;
    }
  }

  if(!error && !found)
  {
    cmm_info = oyCMMOpen_(lib_name);
  }

  if(cmm)
    oyDeAllocateFunc_(cmm);
  cmm = 0;

  return cmm_info;
}

char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info )
{
  char * text = 0, num[48];
  oyCMMapi_s * tmp = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyOBJECT_TYPE_e type = 0;

  if(!cmm_info || cmm_info->type != oyOBJECT_TYPE_CMM_INFO_S)
    return oyStringCopy_("---", oyAllocateFunc_);

  oySprintf_(num,"%d", cmm_info->oy_compatibility );

  oyStringAdd_( &text, cmm_info->cmm, oyAllocateFunc_, oyDeAllocateFunc_ );
  oyStringAdd_( &text, " ", oyAllocateFunc_, oyDeAllocateFunc_ );
  oyStringAdd_( &text, cmm_info->backend_version, oyAllocateFunc_, oyDeAllocateFunc_ );
  oyStringAdd_( &text, "/", oyAllocateFunc_, oyDeAllocateFunc_ );
  oyStringAdd_( &text, num, oyAllocateFunc_, oyDeAllocateFunc_ );
  oyStringAdd_( &text, ":", oyAllocateFunc_, oyDeAllocateFunc_ );

#define CMMINFO_ADD_NAME_TO_TEXT( name_, name_s ) \
  oyStringAdd_( &text, "\n  " name_ ":\n    ", oyAllocateFunc_, oyDeAllocateFunc_ );\
  oyStringAdd_( &text, name_s.nick, oyAllocateFunc_, oyDeAllocateFunc_ ); \
  oyStringAdd_( &text, "\n    ", oyAllocateFunc_, oyDeAllocateFunc_ ); \
  oyStringAdd_( &text, name_s.name, oyAllocateFunc_, oyDeAllocateFunc_ ); \
  oyStringAdd_( &text, "\n    ", oyAllocateFunc_, oyDeAllocateFunc_ ); \
  oyStringAdd_( &text, name_s.description, oyAllocateFunc_, oyDeAllocateFunc_ ); \
  oyStringAdd_( &text, "\n", oyAllocateFunc_, oyDeAllocateFunc_ );

  CMMINFO_ADD_NAME_TO_TEXT( "Name", cmm_info->name )
  CMMINFO_ADD_NAME_TO_TEXT( "Manufacturer", cmm_info->manufacturer )
  CMMINFO_ADD_NAME_TO_TEXT( "Copyright", cmm_info->copyright )

      if(cmm_info)
      {
        tmp = cmm_info->api;
        oyStringAdd_( &text, "\n  API(s): ", oyAllocateFunc_, oyDeAllocateFunc_);

        while(tmp)
        {
          type = oyCMMapi_Check_(tmp);

          oySprintf_(num," %d:", type );
          oyStringAdd_( &text, num, oyAllocateFunc_, oyDeAllocateFunc_ );
          oyStringAdd_( &text, oyStruct_TypeToText((oyStruct_s*)tmp),
                        oyAllocateFunc_, oyDeAllocateFunc_ );

          if(type == oyOBJECT_TYPE_CMM_API4_S)
          {
            cmm_api4 = (oyCMMapi4_s*) tmp;
            oyStringAdd_( &text, "\n    Filter type: ",
                          oyAllocateFunc_, oyDeAllocateFunc_ );
            oyStringAdd_( &text, oyFilterTypeToText( cmm_api4->filter_type ),
                          oyAllocateFunc_, oyDeAllocateFunc_ );
            oyStringAdd_( &text, "\n    Registration: ",
                          oyAllocateFunc_, oyDeAllocateFunc_ );
            oyStringAdd_( &text, cmm_api4->registration,
                          oyAllocateFunc_, oyDeAllocateFunc_ );
            CMMINFO_ADD_NAME_TO_TEXT( "Name", cmm_api4->name )
          }

          tmp = tmp->next;
        }
      }
  oyStringAdd_( &text, "\n", oyAllocateFunc_, oyDeAllocateFunc_ );

  return text;
}

/** @internal @func oyCMMCanHandle_
 *  @brief query a module for certain capabilities
 *
 *  @return                           sum of asked capabilities
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/03 (API 0.1.8)
 */
int              oyCMMCanHandle_    ( oyCMMapi_s         * api,
                                      oyCMMapiQueries_s  * queries )
{
  int capable = 0;
  int error = !api;
  int i, ret = 0;

  if(!error)
  {
    if(queries)
    {
      for(i = 0; i < queries->n; ++i)
      {
        oyCMMapiQuery_s * query = queries->queries[i];
        ret = api->oyCMMCanHandle( query->query, query->value );

        if(ret)
          capable += query->request;
        else if(query->request == oyREQUEST_HARD)
          return 0;
      }

    } else
      capable = 1;
  }

  return capable;
}

/** @internal @func oyCMMsGetApi_
 *  @brief get a module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which make it desireable. So we have to search for match to our automatic
 *  criteria.\n
 *
 *
 *  This function allowes to obtain a API for a certain modul/CMM.
 *
 *  @param[in]   type                  the API to return
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   queries               search for a match to capabilities
 *  @param[out]  cmm_used              inform about the selected CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2007/12/12 (API 0.1.8)
 */
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_TYPE_e     type,
                                       const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       char              * cmm_used,
                                       const char        * category,
                                       oyFILTER_TYPE_e     filter_type )
{
  int error = !type;
  oyCMMapi_s * api = 0,
             * api_fallback = 0;
  char cmm_fallback[5] = {0,0,0,0,0},
       prefered_cmm[5] = {0,0,0,0,0};
  

  if(!cmm_required)
  {
    if(queries && *(uint32_t*)queries->prefered_cmm)
      error = !memcpy( prefered_cmm, queries->prefered_cmm, 4 );
    else
      error = !memcpy( prefered_cmm, oyModuleGetActual( type ), 4 );
  }

  if(!error &&
     !(oyOBJECT_TYPE_CMM_API1_S <= type && type < oyOBJECT_TYPE_CMM_API_MAX))
    error = 1;

  if(!error)
  {
    char ** files = 0;
    int  files_n = 0;
    int i;
    char cmm[5] = {0,0,0,0,0};

    files = oyCMMsGetLibNames_(&files_n, cmm_required);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoAtListFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api;

        error = !memcpy( cmm, cmm_info->cmm, 4 );

        while(tmp)
        {
          if(oyCMMapi_Check_(tmp) == type)
          {

            if(memcmp( cmm, prefered_cmm, 4 ) == 0)
            {
              api = tmp;
              error = !memcpy( cmm_used, cmm_info->cmm, 4 );

            } else {

              api_fallback = tmp;
              error = !memcpy( cmm_fallback, cmm, 4 );
            }

          }
          tmp = tmp->next;
        }
      }
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  if(!api)
  {
    error = !memcpy( cmm_used, cmm_fallback, 4 );
    return api_fallback;
  }

  return api;
}

/** @internal
 *  @brief get all module infos
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
char ** oyCMMsGetNames_              ( int               * n,
                                       oyOBJECT_TYPE_e   * types,
                                       int                 types_n )
{
  int error = !n;
  char ** cmms = 0;
  int cmms_n = 0;

  if(!error)
  {
    char ** files = 0;
    int  files_n = 0;
    int i, j;
    char cmm[5] = {0,0,0,0,0};
    oyOBJECT_TYPE_e type;

    files = oyCMMsGetLibNames_(&files_n, 0);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoAtListFromLibName_(files[i]);

      if(cmm_info)
      {
        type = oyCMMapi_Check_(cmm_info->api);

        memcpy(cmm, cmm_info->cmm, 4); cmm[4] = 0;

        if(type)
        {
          int found = 0;
          oyCMMapi_s * api = cmm_info->api;

          if(types && types_n)
          {
            while(api)
            {
              for(j = 0; j < types_n; ++j)
                if(oyCMMapi_Check_(api) == types[j])
                {
                  found = 1;
                  break;
                }

              if(found)
                api = 0;
              else
                api = api->next;
            }

          } else
            found = 1;

          if(found)
            oyStringListAddStaticString_( &cmms, &cmms_n, cmm,
                                       oyAllocateFunc_, oyDeAllocateFunc_ );
        }
      }
    }

    if(n)
      *n = cmms_n;

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return cmms;
}


/** @internal
 *  @brief ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s* oyCMMGet_               ( const char        * cmm )
{
  oyCMMInfo_s * cmm_info = 0;
  int error = 0;
  char ** files = 0;
  int  files_n = 0;

  if(!error && !cmm)
  {
    cmm = oyModuleGetActual(0);
    error = !cmm;
  }

  if(!error)
    files = oyCMMsGetLibNames_( &files_n, cmm );

  if(files_n > 1)
  {
    int n = 0, i;
    char ** libs = oyLibPathsGet_( &n, "cmms", oyUSER_SYS, oyAllocateFunc_ );

    WARNc1_S("Found more than one matching modul: %d", files_n)
    for(i = 0; i < n; ++i)
      WARNc2_S("   modul path %d.: %s.", i, libs[i]);

    oyStringListRelease_(&libs, n, oyDeAllocateFunc_);
  }

  if(!error)
    cmm_info = oyCMMInfoAtListFromLibName_(files[0]);

  oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );

  return cmm_info;
}

/** @internal
 *  @brief release Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMRelease_       ( const char        * cmm )
{
  int error = !cmm;
  int n = oyStructList_Count( oy_cmm_infos_ );
  int i;

  n = oyStructList_Count(oy_cmm_infos_);
  if(!error)
  for(i = 0; i < n; ++i)
  {
    oyCMMInfo_s * s = 0;
    oyCMMhandle_s * cmmh = (oyCMMhandle_s *) oyStructList_GetType_(
                                oy_cmm_infos_, i, oyOBJECT_TYPE_CMM_HANDLE_S );

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_TYPE_CMM_INFO_S &&
        *(uint32_t*)&s->cmm && cmm &&
        !memcmp( s->cmm, cmm, 4 ) )
    {
      oyCMMhandle_Release_( &cmmh );
      oyStructList_ReleaseAt( oy_cmm_infos_, 0 );
    }
  }

  return error;
}




/** @internal
 *  @brief check for completeness
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyOBJECT_TYPE_e  oyCMMapi_Check_     ( oyCMMapi_s        * api )
{
  int error = !api;
  oyOBJECT_TYPE_e type = 0;

  if(!error)
    type = api->type;

  if(!error)
  switch(type)
  {
    case oyOBJECT_TYPE_CMM_API1_S:
    {
      oyCMMapi1_s * s = (oyCMMapi1_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMCanHandle &&
           s->oyCMMProfile_Open &&
           /*s->oyCMMProfile_GetText &&*/
           s->oyCMMProfile_GetSignature &&
           s->oyCMMColourConversion_Create &&
           s->oyCMMColourConversion_FromMem &&
           s->oyCMMColourConversion_ToMem &&
           s->oyCMMColourConversion_Run ) )
        error = 1;
    } break;
    case oyOBJECT_TYPE_CMM_API2_S:
    {
      oyCMMapi2_s * s = (oyCMMapi2_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyGetMonitorInfo &&
           s->oyGetScreenFromPosition &&
           s->oyGetDisplayNameFromPosition &&
           s->oyGetMonitorProfile &&
           s->oyGetMonitorProfileName &&
           s->oySetMonitorProfile &&
           s->oyActivateMonitorProfiles ) )
        error = 1;
    } break;
    case oyOBJECT_TYPE_CMM_API3_S:
    {
      oyCMMapi3_s * s = (oyCMMapi3_s*)api;
      if(!(s->oyCMMInit &&
           s->oyCMMMessageFuncSet &&
           s->oyCMMCanHandle &&
           s->oyCMMProfileTag_GetValues &&
           /*s-> &&*/
           s->oyCMMProfileTag_Create ) )
        error = 1;
    } break;
    default: break;
  }

  return type;
}


/** @} */



/** \addtogroup misc Miscellaneous

 *  @{
 */

/** @brief object management 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_New  ( void )
{
  oyObject_s o = 0;
  int error = 0;
  int len = sizeof(struct oyObject_s_);

  o = oyAllocateFunc_(len);

  if(!o) return 0;

  error = !memset( o, 0, sizeof(len) );
  
  o = oyObject_SetAllocators_( o, oyAllocateFunc_, oyDeAllocateFunc_ );

  o->type_ = oyOBJECT_TYPE_OBJECT_S;
  o->copy = (oyStruct_CopyF_t) oyObject_Copy;
  o->release = (oyStruct_ReleaseF_t) oyObject_Release;
  o->version_ = oyVersion(0);
  ++o->ref_;

  return o;
}

/** @brief object management 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_NewWithAllocators  ( oyAllocFunc_t     allocateFunc,
                              oyDeAllocFunc_t   deallocateFunc )
{
  oyObject_s o = 0;
  int error = 0;
  int len = sizeof(struct oyObject_s_);

  o = oyAllocateWrapFunc_( len, allocateFunc );

  if(!o) return 0;

  error = !memset( o, 0, len );
  
  o = oyObject_SetAllocators_( o, allocateFunc, deallocateFunc );

  o->type_ = oyOBJECT_TYPE_OBJECT_S;
  o->copy = (oyStruct_CopyF_t) oyObject_Copy;
  o->release = (oyStruct_ReleaseF_t) oyObject_Release;
  o->version_ = oyVersion(0);
  ++o->ref_;

  return o;
}

/** @internal
 *  @brief object management 
 *
 *  @param[in]    object         the object
 *  @param[in]    type           object type
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
static oyObject_s
oyObject_NewFrom_( oyObject_s      object )
{
  oyObject_s o = 0;
  int error = 0;

  if(object)
    o = oyObject_NewWithAllocators( object->allocateFunc_,
                                    object->deallocateFunc_ );
  else
    o = oyObject_New( );

  if(!o)
    error = 1;

  if(!error && object && object->name_)
    error = oyObject_SetNames( o, object->name_->nick, object->name_->name,
                              object->name_->description );

  if(!error)
    error = 1;

  return o;
}

/** @brief object management 
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyObject_s
oyObject_NewFrom ( oyObject_s      object )
{
  return oyObject_NewFrom_( object );
}

/** @brief object management 
 *
 *  @param[in]    object         the object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s
oyObject_Copy ( oyObject_s      object )
{
  if(!object)
    return 0;

  oyObject_Ref( object );

  return object;
}

/** @internal
 *  @brief custom object memory managers
 *
 *  @param[in]    object         the object to modify
 *  @param[in]    allocateFunc   zero for default or user memory allocator
 *  @param[in]    deallocateFunc zero for default or user memory deallocator
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAllocFunc_t     allocateFunc,
                                        oyDeAllocFunc_t   deallocateFunc )
{
  if(!object) return 0;

  /* we require a allocation function to be present */
  if(allocateFunc)
    object->allocateFunc_ = allocateFunc;
  else
    object->allocateFunc_ = oyAllocateFunc_;

  object->deallocateFunc_ = deallocateFunc;

  return object;
}

/** @brief release an Oyranos object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_Release         ( oyObject_s      * obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyObject_s s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( s->type_ != oyOBJECT_TYPE_OBJECT_S)
  {
    WARNc_S(("Attempt to release a non oyObject_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s))
    return 0;
  /* ---- end of common object destructor ------- */

  oyName_release_( &s->name_, s->deallocateFunc_ );

  if(s->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->deallocateFunc_;
    oyPointer lock = s->lock_;

    s->hash_[0] = 0;

    if(s->backdoor_)
      deallocateFunc( s->backdoor_ ); s->backdoor_ = 0;

    error = oyStructList_Release(&s->handles_);

    deallocateFunc( s );
    oyLockReleaseFunc_( lock, __FILE__, __LINE__ );
  }

  return 0;
}

/** @func    oyObject_Ref
 *  @internal
 *  @brief   increase the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/07
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_Ref            ( oyObject_s          obj )
{
  oyObject_s s = obj;
  int error = !s;

  if( s->type_ != oyOBJECT_TYPE_OBJECT_S)
  {
    WARNc_S(("Attempt to manipulate a non oyObject_s object."))
    return 1;
  }

  if(!error)
    oyObject_Lock( s, __FILE__, __LINE__ );

  if(!error)
    ++s->ref_;

  if(obj->parent_type_ == oyOBJECT_TYPE_NAMED_COLOURS_S)
  {
    int e_a = error;
    error = pow(e_a,2.1);
    error = e_a;
  }

  if(!error)
    oyObject_UnLock( s, __FILE__, __LINE__ );

  return s->ref_;
}

/** @func    oyObject_UnRef
 *  @internal
 *  @brief   decrease the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/07
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_UnRef          ( oyObject_s          obj )
{
  int ref = 0;
  oyObject_s s = obj;
  int error = !s;

  if( s->type_ != oyOBJECT_TYPE_OBJECT_S)
  {
    WARNc_S(("Attempt to manipulate a non oyObject_s object."))
    return 1;
  }

  if(!error)
    oyObject_Lock( s, __FILE__, __LINE__ );

  if(!error && --s->ref_ > 0)
    ref = s->ref_;

  if(obj->parent_type_ == oyOBJECT_TYPE_NAMED_COLOURS_S)
  {
    int e_a = error;
    error = pow(e_a,2.1);
    error = e_a;
  }

  if(!error)
    oyObject_UnLock( s, __FILE__, __LINE__ );

  return ref;
}

/** @brief custom object memory managers
 *
 *  Normally a sub object should not know about its parent structure, but
 *  follow a hierarchical design. We break this here to allow the observation of
 *  all objects including the parents structures as a feature in oyObject_s.
 *  Then we just need a list of all oyObject_s objects and have an overview.
 *  This facility is intented to work even without debugging tools.
 *
 *  @param[in]    o              the object to modify
 *  @param[in]    type           the parents struct type
 *  @param[in]    parent         a pointer to the parent struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s   oyObject_SetParent       ( oyObject_s        o,
                                        oyOBJECT_TYPE_e   type,
                                        oyPointer         parent )
{
  int error = 0;
  if(!error && type)
  {
    o->parent_type_ = type;
    if(parent)
      o->parent_ = parent;
  }

  return o;
}

/** @brief object naming
 *
 *  @param[in]    object         the object
 *  @param[in]    name           the name to set
 *  @param[in]    type           the kind of name 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_SetName         ( oyObject_s        object,
                                        const oyChar    * text,
                                        oyNAME_e          type )
{
  object->name_ = oyName_set_( object->name_, text, type,
                               object->allocateFunc_, object->deallocateFunc_ );
  return (text && type && object && !object->name_);
}


/** @brief object naming
 *
 *  @param[in]    object         the object
 *  @param[in]    nick           short name, about 1-4 letters
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    description    the description 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description )
{
  int error = 0;
  

  if(!object) return 1;

  if(!error)
    error = oyObject_SetName( object, nick, oyNAME_NICK );
  if(!error)
    error = oyObject_SetName( object, name, oyNAME_NAME );
  if(!error)
    error = oyObject_SetName( object, description, oyNAME_DESCRIPTION );

  return error;
}

/** @func  oyObject_GetName
 *  @brief object get names
 *
 *  @param[in]    object         the object
 *  @param[in]    type           name type
 *  @return                      the text
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2007/11/00 (API 0.1.8)
 */
const oyChar * oyObject_GetName       ( const oyObject_s        obj,
                                        oyNAME_e                type )
{
  const oyChar * text = 0;
  if(!obj)
    return 0;

  if(!obj->name_)
    return 0;

  switch(type)
  {
    case oyNAME_NICK:
         text = obj->name_->nick; break;
    case oyNAME_DESCRIPTION:
         text = obj->name_->description; break;
    case oyNAME_NAME:
    default:
         text = obj->name_->name; break;
  }

  return text;
}

/** @func  oyObject_Lock
 *  @brief Lock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]    object         the object
 *  @return                      error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_Lock             ( oyObject_s        object,
                                         const char      * marker,
                                         int               line )
{
  int error = !object;

  if(!error)
  {
    if(!object->lock_)
      object->lock_ = oyStruct_LockCreateFunc_( object->parent_ );

    oyLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** @func  oyObject_UnLock
 *  @brief Unlock a object
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]    object         the object
 *  @return                      error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnLock           ( oyObject_s        object,
                                         const char      * marker,
                                         int               line )
{
  int error = !object;

  if(!error)
  {
    error = !object->lock_;
    if(!error)
      oyUnLockFunc_( object->lock_, marker, line );
  }

  return error;
}

/** @func  oyObject_UnSetLocking
 *  @brief remove a object's lock pointer
 *
 *  @see   oyThreadLockingSet
 *
 *  @param[in]    object         the object
 *  @return                      error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/22 (API 0.1.8)
 */
int          oyObject_UnSetLocking     ( oyObject_s        object,
                                         const char      * marker,
                                         int               line )
{
  int error = !object;

  if(!error)
  {
    oyLockReleaseFunc_( object->lock_, marker, line );
  }

  return error;
}


/** @brief get CMM specific data pointer
 *
 *  @param[in]    object         the object
 *  @param[in]    cmm            CMM name
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
/*oyCMMptr_s * oyObject_GetCMMPtr       ( oyObject_s        object,
                                        const char      * cmm )
{
  oyObject_s s = object;
  int error = 0;
  oyCMMptr_s * cmm_ptr = 0;

  if(!s)
    error = 1;

  if(!error && !cmm)
    cmm = oyModuleGetActual( 0 );

  if(!error && s->handles_)
  {
    int n = oyStructList_Count(s->handles_);
    int i;

    for(i = 0; i < n ; ++i)
    {
      oyHandle_s * h = oyStructList_Get_(s->handles_, i);

      if(h->ptr)
      {
        oyCMMptr_s * lcmm_ptr = h->ptr;

        if( lcmm_ptr->cmm &&
            !oyStrcmp_( lcmm_ptr->cmm, cmm ) )
          cmm_ptr = lcmm_ptr;
      }
    }
  }

  return cmm_ptr;
}*/

/** @internal
 *  @brief set CMM specific data pointer
 *
 *  @param[in]    object         the object
 *  @param[in]    cmm_ptr        CMM specific pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
/*oyObject_s  oyObject_SetCMMPtr       ( oyObject_s       object,
                                        oyCMMptr_s      * cmm_ptr )
{
  oyObject_s s = 0;
  if(cmm_ptr)
    s = oyObject_SetCMMPtr_( object, cmm_ptr->cmm,
                             cmm_ptr->ptr, cmm_ptr->resource,
                             cmm_ptr->ptrRelease, cmm_ptr->func_name);
  return s;
}*/

/** @internal
 *  @brief set CMM specific data pointer
 *
 *  @param[in]    object         the object
 *  @param[in]    cmm            CMM name
 *  @param[in]    cmm_ptr        CMM specific pointer or zero to remove
 *  @param[in]    resource       Oyranos CMM resource type
 *  @param[in]    ptrRelease     CMM specific pointer deallocator
 *  @param[in]    func_name      CMM function name to release the CMM pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
/*oyObject_s  oyObject_SetCMMPtr_      ( oyObject_s      object,
                                        const char      * cmm,
                                        oyPointer         ptr,
                                        const char      * resource,
                                        oyStructReleaseF_t ptrRelease,
                                        oyChar          * func_name )
{
  oyObject_s s = object;
  int error = 0;
  oyCMMptr_s * cmm_ptr = 0;

  if(!s) error = 1;

  if(!error && !cmm)
    cmm = oyModuleGetActual(0);

  if(!error && s->handles_)
  {
    int n = oyStructList_Count(s->handles_);
    int i;

    for(i = 0; i < n ; ++i)
    {
      oyHandle_s * h = oyStructList_Get_(s->handles_, i);

      if(h->ptr)
      {
        oyCMMptr_s * l_cmm_ptr = (oyCMMptr_s*)h->ptr;

        if( l_cmm_ptr->type == oyOBJECT_TYPE_CMM_POINTER_S &&
            l_cmm_ptr->cmm &&
            !oyStrcmp_( l_cmm_ptr->cmm, cmm ) )
          cmm_ptr = l_cmm_ptr;
      }
    }
  }

  if(!error)
  {
    if(!cmm_ptr)
    {
      cmm_ptr = oyCMMptr_New_(object->allocateFunc_);
      error = !cmm_ptr;
    }

    if(!error)
      error = oyCMMptr_Set_( cmm_ptr, cmm, func_name, resource,
                             ptr, ptrRelease );
    WARNc_S(("Better write a oyCMMptr_s to global cache. %s %s %s", cmm, resource, func_name))
  }

  return s;
}*/


#if 0

/** @internal
 *  @brief new Oyranos handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
oyHandle_s *       oyHandle_new_     ( oyAllocFunc_t       allocateFunc )
{
  oyHandle_s * s = 0;
  int error = 0;

  s = oyAllocateWrapFunc_( sizeof(oyHandle_s), allocateFunc );
  error = !s;

  if(!error)
    s->type_ = oyOBJECT_TYPE_HANDLE_S;

  return s;
}

/** @internal
 *  @brief copy Oyranos handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
oyHandle_s *       oyHandle_copy_    ( oyHandle_s        * orig,
                                       oyAllocFunc_t       allocateFunc )
{
  oyHandle_s * s = 0;
  int error = 0;

  s = oyAllocateWrapFunc_( sizeof(oyHandle_s), allocateFunc );
  error = !s;

  if(!error && orig)
  {
    error = oyHandle_set_( s, 0, orig->ptr_type,
                           orig->ptrRelease, orig->ptrCopy );

    if(orig->ptr && orig->ptrCopy)
      s->ptr = orig->ptrCopy( orig->ptr, 0 );
  }

  return s;
}

/** @internal
 *  @brief release Oyranos handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int                oyHandle_release_ ( oyHandle_s       ** handle )
{
  oyHandle_s * s = 0;
  int error = 0;

  if(handle && *handle)
    s = *handle;

  error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_TYPE_HANDLE_S)
      error = 1;

  if(!error)
  {
    oyHandle_set_(s, 0,0,0,0);
    oyPointerRelease_m( &s );
  }

  if(!error)
    *handle = 0;

  return error;
}

/** @internal
 *  @brief set properties of a Oyranos handle
 *
 *  The function sets all properties to the provided values.\n
 *  In case a pointer is found in handle, the release function will be applied.
 *  ptrRelease and ptrCopy should be set otherwise the behaviour is undefined.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int                oyHandle_set_     ( oyHandle_s        * handle,
                                       oyPointer           ptr,
                                       oyOBJECT_TYPE_e     ptr_type,
                                       oyStruct_releaseF_t  ptrRelease,
                                       oyStruct_copyF_t     ptrCopy )
{
  oyHandle_s * s = handle;
  int error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_TYPE_HANDLE_S)
      error = 1;

  if(!error)
  {
    if(s->ptr && s->ptrRelease)
      s->ptrRelease( &s->ptr );
    s->ptr = ptr;
    s->ptr_type = ptr_type;
    s->ptrRelease = ptrRelease;
    s->ptrCopy = ptrCopy;
  }

  return error;
}

#endif



/** @internal
 *  @brief new Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyHash_s *   oyHash_New_             ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_HASH_S;
# define STRUCT_TYPE oyHash_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyHash_Copy_;
  s->release = (oyStruct_ReleaseF_t) oyHash_Release_;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief copy a Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *   oyHash_CopyRef_         ( oyHash_s          * entry,
                                       oyObject_s          object )
{
  oyHash_s * s = entry;
  int error = !s;

  if(!error)
    if(s->type_ != oyOBJECT_TYPE_HASH_S)
      error = 1;

  if(!error)
    oyObject_Copy( s->oy_ );

  return s;
}

/** @internal
 *  @brief release a Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
int                oyHash_Release_   ( oyHash_s         ** obj )
{
  /* ---- start of common object destructor ----- */
  oyHash_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_HASH_S)
  {
    WARNc_S(("Attempt to release a non oyHash_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  /* should not happen */
  if(s->entry && s->entry->release)
    s->entry->release( &s->entry );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *  @brief get a new Oyranos cache entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *         oyHash_Get_       ( const char        * hash_text,
                                       oyObject_s          object )
{
  oyHash_s * s = 0;
  int error = !hash_text;
  uint32_t * val = 0;

  if(!error)
  {
    s = oyHash_New_(object);
    error = !s;
  }

  if(!error)
  {
    val = (uint32_t*) &s->oy_->hash_;

    if(oyStrlen_(hash_text) < OY_HASH_SIZE)
      memcpy(s->oy_->hash_, hash_text, oyStrlen_(hash_text)+1);
    else
#if 0
      error = oyMiscBlobGetMD5_( (void*)hash_text, oyStrlen_(hash_text),
                                 s->oy_->hash_ );
#else
      (*val) = oyMiscBlobGetL3_( (void*)hash_text, oyStrlen_(hash_text) );
#endif
  }

  if(!error)
    error = oyObject_SetName(s->oy_, hash_text, oyNAME_NAME);

  return s;
}

/** @internal
 *  @brief copy a Oyranos hash object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
oyHash_s *         oyHash_Copy_      ( oyHash_s          * orig,
                                       oyObject_s          object )
{
  oyHash_s * s = 0; 
  int error = 0;

  if(!orig)
  {
    WARNc_S(("Attempt to copy without original."))

    error = 1;
  }

  s = orig;

  if(!error)
    if(s->type_ != oyOBJECT_TYPE_HASH_S)
      error = 1;

  if(!error) 
  if(!object)
  {
    if(s->entry && s->entry->copy)
      s->entry = s->entry->copy( s->entry, 0 );
    oyObject_Copy( s->oy_ );
    return s;
  }

  if(!error && object)
  {
    s = oyHash_New_( object );

    error = !s;
    if(error)
      WARNc_S(("Could not create structure for hash."));
  }

  if(error)
    return 0;
  else
    return s;
}

/** @internal
 *  @brief hash is of type
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf_      ( oyHash_s          * hash,
                                       oyOBJECT_TYPE_e     type )
{
  return (hash && hash->entry && hash->entry->type_ == type);
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
oyStruct_s *       oyHash_GetPointer_( oyHash_s          * hash,
                                       oyOBJECT_TYPE_e     type )
{
  if(oyHash_IsOf_( hash, type))
    return hash->entry;
  else
    return 0;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_SetPointer_( oyHash_s          * hash,
                                       oyStruct_s        * obj )
{
  if(hash)
  {
    hash->entry = obj;
    return 0;
  } else
    return 1;
}




/** @internal
 *  @brief get always a Oyranos cache entry from a cache list
 *
 *  @param[in]     cache_list          the list to search in
 *  @param[in]     hash_text           the text to search for in the cache_list
 *  @return                            the cache entry may not have a entry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyHash_s *   oyCacheListGetEntry_    ( oyStructList_s    * cache_list,
                                       const char        * hash_text )
{
  oyHash_s * entry = 0;
  oyHash_s * search_key = 0;
  int error = !(cache_list && hash_text);
  int n = 0, i;

  if(!error && cache_list->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
  {
    search_key = oyHash_Get_(hash_text, 0);
    error = !search_key;
  }

  if(!error)
    n = oyStructList_Count(cache_list);

  for(i = 0; i < n; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_( cache_list, i,
                                                         oyOBJECT_TYPE_HASH_S );

    if(compare)
    if(memcmp(search_key->oy_->hash_, compare->oy_->hash_, OY_HASH_SIZE) == 0)
      entry = compare;
  }

  if(!error && !entry)
  {
    if(!error)
      entry = oyHash_Copy_( search_key, 0 );

    if(!error)
    {
      error = oyStructList_MoveIn(cache_list, (oyStruct_s**)&search_key, -1);
      search_key = 0;
    }
  }

  oyHash_Release_( &search_key );

  if(entry)
    return oyHash_Copy_( entry, 0 );
  else
    return 0;
}






/** @internal
 *  The lists are allocated one time and live until the application quits
 *  It contains the various caches for faster access of CPU intentsive data.\n
 *  We'd need a 3 dimensional table to map\n
 *    A: a function or resource type\n
 *    B: a CMM\n
 *    C: a hash value specifying under which conditions the resource was build\n
 *  The resulting cache entry is the result from the above 3 arguments. With the
 *  much implementation work and expectedly small to no speed advantage it is 
 *  not a good strategy. Even though argument C would be extensible.\n
 *  \n
 *  A different approach would use two values to map the search request to the 
 *  cache entry. The above hash map and the hash or combination of the two other *  values.\
 *  \n
 *  One point to consider is a readable string to end not with a anonymous list
 *  full of anonymous entries, where a user can pretty much nothing know.
 *  A transparent approach has to allow for easy identifying each entry.\n
 *  A help would be a function to compute both a md5 digest and a message from
 *  the 3 arguments outlined above. Probably it would allow much more arguments
 *  to add as we can not know how many optins and other influential parameters
 *  the cache entry depends on.\n
 *  \n
 *  A final implementation would consist of a
 *  - function to convert arbitrary (string) arguments to a single string and a
 *    hash sum 
 *  - a function to create a cache entry struct from above hash and a the 
 *    according description string plus the oyCMMptr_s struct. For simplicity
 *    the cache struct can be identical to the oyCMMptr_s, with the disadvantage
 *    of containing additional data not understandable for a CMM. We need to 
 *    mark these data (hash + description) as internal to Oyranos.
 *  - a list that hold above cache entry stucts
 *  - several functions to reference, release, maps a hash value to the
 *    cached resource
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_cache_ = 0;

/** @internal
 *  @brief get always a Oyranos cache entry from the CMM's cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_cmm_cache_)
    oy_cmm_cache_ = oyStructList_New( 0 );

  return oyCacheListGetEntry_(oy_cmm_cache_, hash_text);
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyStructList_s** oyCMMCacheList_()
{
  return &oy_cmm_cache_;
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyChar* oyCMMCacheListPrint_()
{
  oyStructList_s ** cache_list = oyCMMCacheList_();
  int n = oyStructList_Count( *cache_list ), i;
  oyChar * text = 0;
  oyChar refs[80];

  oySprintf_( refs,"%s:%d Oyranos CMM cache with %d entries:\n", 
              __FILE__,__LINE__, n);
  oyStringAdd_( &text, refs, oyAllocateFunc_, oyDeAllocateFunc_ );

  for(i = 0; i < n ; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_(*cache_list, i,
                                                         oyOBJECT_TYPE_HASH_S );

    if(compare)
    {
      oySprintf_(refs,"%d: ", compare->oy_->ref_);
      oyStringAdd_( &text, refs,
                           oyAllocateFunc_, oyDeAllocateFunc_ );
      oyStringAdd_( &text, oyObject_GetName(compare->oy_, oyNAME_NAME),
                           oyAllocateFunc_, oyDeAllocateFunc_ );
      oyStringAdd_( &text, "\n",
                           oyAllocateFunc_, oyDeAllocateFunc_ );
    }
  }

  return text;
}



/** @func    oyValueCopy
 *  @brief   copy a oyValue_u union
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
void           oyValueCopy           ( oyValue_u         * from,
                                       oyValue_u         * to,
                                       oyVALUETYPE_e       type,
                                       oyAllocFunc_t       allocateFunc,
                                       oyDeAllocFunc_t     deallocateFunc )
{
  int n = 0, i;

  if(!from || !to)
    return;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_; 

  switch(type)
  {
  case oyVAL_INT: to->int32 = from->int32; break;
  case oyVAL_INT_LIST:
       if(!from->int32_list)
         return;

       n = from->int32_list[0];

       if(to->int32_list && deallocateFunc)
         deallocateFunc(to->int32_list);

       to->int32_list = allocateFunc( (n+1) * sizeof(int32_t) );
       to->int32_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->int32_list[i] = from->int32_list[i];
       break;
  case oyVAL_DOUBLE: to->dbl = from->dbl; break;
  case oyVAL_DOUBLE_LIST:
       if(!from->dbl_list)
         return;

       n = from->dbl_list[0];

       if(to->dbl_list && deallocateFunc)
         deallocateFunc(to->dbl_list);

       to->dbl_list = allocateFunc( (n+1) * sizeof(double));

       to->dbl_list[0] = n;
       for(i = 1; i <= n; ++i)
         to->dbl_list[i] = from->dbl_list[i];

       break;
  case oyVAL_STRING:
       to->string = oyStringCopy_(from->string, allocateFunc);
       break;
  case oyVAL_STRING_LIST:
       if(!from->string_list)
         return;

       if(to->string_list && deallocateFunc)
       {
         i = 0;
         while(to->string_list[i])
           deallocateFunc(to->string_list[i++]);
         deallocateFunc(to->string_list);
       }

       i = 0;
       n = 0;
       while((size_t)from->string_list[i])
         ++n;

       to->string_list = allocateFunc( n * sizeof(char*));
       i = 0;
       while(from->string_list[i])
       {
         to->string_list[i] = oyStringCopy_(from->string_list[i], allocateFunc);
         ++i;
       }
       to->string_list[n] = 0;

       break;
  }
}
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAllocFunc_t     deallocateFunc )
{
  int i;

  if(!v)
    return;

  if(!deallocateFunc)
    return; 

  switch(type)
  {
  case oyVAL_INT:
  case oyVAL_DOUBLE:
       break;
  case oyVAL_INT_LIST:
  case oyVAL_DOUBLE_LIST:
       if(!v->int32_list)
         break;

       if(v->int32_list)
         deallocateFunc(v->int32_list);

       break;
  case oyVAL_STRING:
       deallocateFunc( v->string );
       break;
  case oyVAL_STRING_LIST:
       if(!v->string_list)
         break;

       if(v->string_list)
       {
         i = 0;
         while(v->string_list[i])
           deallocateFunc(v->string_list[i++]);
         deallocateFunc(v->string_list);
       }

       break;
  }
}
void           oyValueRelease        ( oyValue_u        ** v,
                                       oyVALUETYPE_e       type,
                                       oyDeAllocFunc_t     deallocateFunc )
{
  if(!v)
    return;

  if(!deallocateFunc)
    return; 

  oyValueClear( *v, type, deallocateFunc );

  deallocateFunc(*v);
  *v = 0;
}

/** @func    oyOption_New
 *  @brief   new option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOption_s *   oyOption_New          ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_OPTION_S;
# define STRUCT_TYPE oyOption_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyOption_Copy;
  s->release = (oyStruct_ReleaseF_t) oyOption_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @func    oyOption_Copy_
 *  @brief   real copy a option object
 *
 *  @param[in]     option              option object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOption_s * oyOption_Copy_          ( oyOption_s        * option,
                                       oyObject_s          object )
{
  oyOption_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc_ = 0;

  if(!option || !object)
    return s;

  s = oyOption_New( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->id = option->id;
    s->name = oyName_copy( option->name, s->oy_ );
    s->config_path = option->config_path;
    s->config_key = option->config_key;
    s->value_type = option->value_type;
    oyValueCopy( &s->value, &option->value, s->value_type,
                 allocateFunc_, s->oy_->deallocateFunc_ );
    oyValueCopy( &s->standard, &option->standard, s->value_type,
                 allocateFunc_, s->oy_->deallocateFunc_ );
    oyValueCopy( &s->start, &option->start, s->value_type,
                 allocateFunc_, s->oy_->deallocateFunc_ );
    oyValueCopy( &s->end, &option->end, s->value_type,
                 allocateFunc_, s->oy_->deallocateFunc_ );
    s->flags = option->flags;
  }

  return s;
}
/** @func    oyOption_Copy
 *  @brief   copy or reference a option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOption_s *   oyOption_Copy         ( oyOption_s        * option,
                                       oyObject_s          object )
{
  oyOption_s * s = 0;

  if(!option)
    return s;

  if(option && !object)
  {
    s = option;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOption_Copy_( option, object );

  return s;
}

/** @func    oyOption_Release
 *  @brief   release a option
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
int            oyOption_Release      ( oyOption_s       ** obj )
{
  /* ---- start of common object destructor ----- */
  oyOption_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_OPTION_S)
  {
    WARNc_S(("Attempt to release a non oyOption_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->id = 0;
  s->config_path = 0;
  s->config_key = 0;
  s->flags = 0;

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyValueClear( &s->value, s->value_type, deallocateFunc );
    oyValueClear( &s->standard, s->value_type, deallocateFunc );
    oyValueClear( &s->start, s->value_type, deallocateFunc );
    oyValueClear( &s->end, s->value_type, deallocateFunc );

    s->value_type = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @func    oyOptions_New
 *  @brief   new options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s *  oyOptions_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_OPTIONS_S;
# define STRUCT_TYPE oyOptions_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyOptions_Copy;
  s->release = (oyStruct_ReleaseF_t) oyOptions_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

oyOptions_s *  oyOptions_FromMem     ( size_t            * size,
                                       const char        * opts_text,
                                       oyObject_s          object );
/** @func    oyOptions_Copy_
 *  @brief   real copy a options object
 *
 *  @param[in]     options             options object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s * oyOptions_Copy_        ( oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc_ = 0;

  if(!options || !object)
    return s;

  s = oyOptions_New( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->opts = oyStructList_Copy( options->opts, s->oy_ );
  }

  return s;
}
/** @func    oyOptions_Copy
 *  @brief   release options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyOptions_s *  oyOptions_Copy        ( oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;

  if(!options)
    return s;

  if(options && !object)
  {
    s = options;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOptions_Copy_( options, object );

  return s;
}

/** @func    oyOptions_Release
 *  @brief   release options
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
int            oyOptions_Release     ( oyOptions_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyOptions_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_OPTIONS_S)
  {
    WARNc_S(("Attempt to release a non oyOptions_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->opts );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


int            oyOptions_ReleaseAt   ( oyOptions_s       * list,
                                       int                 pos );
oyOption_s *   oyOptions_Get         ( oyOptions_s       * list,
                                       int                 pos );
int            oyOptions_Count       ( oyOptions_s       * list );
int            oyOptions_MoveIn      ( oyOptions_s       * options,
                                       oyOption_s       ** option );
int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option );
char           oyOptions_GetMem      ( oyOptions_s       * options,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc );

/** @} */


/** \addtogroup profile_handling Profile handling API

 *  @{
 */


/** @internal
 *  @brief create a empty profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s *
oyProfile_New_ ( oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_PROFILE_S;
# define STRUCT_TYPE oyProfile_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyProfile_Copy;
  s->release = (oyStruct_ReleaseF_t) oyProfile_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->tags_ = oyStructList_New( 0 );

  return s;
}

/** @internal
 *  @brief hash for oyProfile_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
static int
oyProfile_GetHash_( oyProfile_s * s )
{
  int error = 1;
  if(s->block_ && s->size_)
  {
    memset( s->oy_->hash_, 0, OY_HASH_SIZE*2 );
    error = oyProfileGetMD5( s->block_, s->size_, s->oy_->hash_ );
    if(error)
      memset(s->oy_->hash_, 0, OY_HASH_SIZE*2 );
  }
  return error;
}


oyProfile_s ** oy_profile_s_std_cache_ = 0;

/** @brief create from default colour space settings
 *
 *  @param[in]    type           default colour space or zero to detect display?
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s *
oyProfile_FromStd     ( oyPROFILE_e       type,
                        oyObject_s        object)
{
  oyProfile_s * s = 0;
  char * name = 0;
  oyAllocFunc_t allocateFunc = 0;
  int pos = type - oyDEFAULT_PROFILE_START;

  if(!oy_profile_s_std_cache_)
  {
    int len = sizeof(oyProfile_s*) *
                            (oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START);
    oy_profile_s_std_cache_ = oyAllocateFunc_( len );
    memset( oy_profile_s_std_cache_, 0, len );
  }

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    if(oy_profile_s_std_cache_[pos])
      return oyProfile_Copy( oy_profile_s_std_cache_[pos], 0 );

  if(object)
    allocateFunc = object->allocateFunc_;

  if(type)
    name = oyGetDefaultProfileName ( type, allocateFunc );

  s = oyProfile_FromFile( name, 0, object );

  if(s)
    s->use_default_ = type;

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    oy_profile_s_std_cache_[pos] = oyProfile_Copy( s, 0 );

  return s;
}


oyStructList_s * oy_profile_s_file_cache_ = 0;


/** @brief create from file
 *
 *  @param[in]    name           profile file name or zero to detect display?
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports oyNO_CACHE_READ and oyNO_CACHE_WRITE to disable cache reading
 *  and writing. The cache flags are useful for one time profiles or scanning
 *  large numbers of profiles.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromFile            ( const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;
  int error = 0;
  size_t size = 0;
  oyPointer block = 0;
  oyAllocFunc_t allocateFunc = 0;
  oyHash_s * entry = 0;
  char * file_name = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  if(!oyToNoCacheRead_m(flags) || !oyToNoCacheWrite_m(flags))
  {
    if(!oy_profile_s_file_cache_)
      oy_profile_s_file_cache_ = oyStructList_New( 0 );

    if(!object)
    {
      entry = oyCacheListGetEntry_ ( oy_profile_s_file_cache_, name );

      if(!oyToNoCacheRead_m(flags))
      {
        s = (oyProfile_s*) oyHash_GetPointer_( entry, oyOBJECT_TYPE_PROFILE_S);
        s = oyProfile_Copy( s, 0 );
        if(s)
          return s;
      }
    }
  }

  if(!error && name && !s)
  {
    file_name = oyFindProfile_( name );
    block = oyGetProfileBlock( file_name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  if(!error)
  {
    s = oyProfile_FromMemMove_( size, &block, 0, object );

    if(!s)
      error = 1;

    /* We expect a incomplete filename attached to s and try to correct this. */
    if(!error && !file_name && s->file_name_)
    {
      file_name = oyFindProfile_( s->file_name_ );
      if(file_name && s->oy_->deallocateFunc_)
      {
        s->oy_->deallocateFunc_( s->file_name_ );
        s->file_name_ = 0;
      }
    }

    if(!error && file_name)
    {
      s->file_name_ = oyStringCopy_( file_name, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( file_name ); file_name = 0;
    }

    if(!error && !s->file_name_)
      error = 1;
  }

  if(!error && s && entry)
  {
    if(!oyToNoCacheWrite_m(flags))
    {
      /* 3b.1. update cache entry */
      error = oyHash_SetPointer_( entry, (oyStruct_s*) oyProfile_Copy( s, 0 ) );
#if 0
    } else {
      int i = 0, n = 0, pos = -1;

      n = oyStructList_Count( oy_profile_s_file_cache_ );
      for( i = 0; i < n; ++i )
        if((oyStruct_s*)entry == oyStructList_Get_( oy_profile_s_file_cache_,i))
          pos = i;
      if(pos >= 0)
        oyStructList_ReleaseAt( oy_profile_s_file_cache_, pos );
#endif
    }
  }

  oyHash_Release_( &entry );

  return s;
}

/** @brief create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    buf            pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyProfile_s* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object)
{
  oyProfile_s * s = oyProfile_New_( object );
  int error = 0;
  oyGetMonitorProfile_t funcP = 0;
  oyGetMonitorProfileName_t funcP2 = 0;
  char cmm[] = {0,0,0,0,0};

  if(block  && *block && size)
  {
    s->block_ = *block;
    *block = 0;
    if(!s->block_)
      error = 1;
    else
      s->size_ = size;
  }

  if (!s->block_)
  {
    if(!error)
    {
      oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API2_S,
                                        "oyX1", 0, cmm, 0, 0 );
      if(api && *(uint32_t*)&cmm)
      {
        oyCMMapi2_s * api2 = (oyCMMapi2_s*) api;
        funcP = api2->oyGetMonitorProfile;
        funcP2= api2->oyGetMonitorProfileName;
      }
      /*error = !funcP || !funcP2;*/
    }

    if(funcP)
      s->block_ = funcP( NULL, &s->size_, s->oy_->allocateFunc_ );

    if(!error && (!s->block_ || !s->size_))
    {
      s->file_name_ = oyGetDefaultProfileName( oyASSUMED_WEB,
                                               s->oy_->allocateFunc_ );
      if(!error && s->file_name_)
      {
        s->block_ = oyGetProfileBlock( s->file_name_, &s->size_,
                                       s->oy_->allocateFunc_ );
        if(!s->block_ || !s->size_)
        {
          error = 1;
          WARNc_S(("Did not find monitor profile; nor a substitute."))
        } else
          WARNc_S(("Did not find monitor profile; use sRGB instead."))
      }

    } else if(funcP2)
      s->file_name_ = funcP2( NULL, s->oy_->allocateFunc_ );
  }

  /* Comparision strategies
      A
       - search for similiar arguments in the structure
       - include the affect of the arguments (resolve type -> filename)
       - exclude paralell structure elements (filename -> ignore blob)
      B
       - use always the ICC profiles md5
      C
       - always the flattened Oyranos profile md5

       - A higher level API can maintain its own cache depending on costs.
   */

  if(!error)
    error = oyProfile_GetHash_( s );

  if(!error)
    error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );

  if(!error)
  {
    s->names_chan_ = 0;
    s->channels_n_ = 0;
    s->channels_n_ = oyProfile_GetChannelsCount( s );
    error = (s->channels_n_ <= 0);
  }

  if(error)
  {
    WARNc_S(("Could not create structure for profile."))
  }

  return s;
}

/** @brief create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    buf            pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromMem             ( size_t            size,
                                const oyPointer   block,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 0;

  if(block && size)
  {
    block_ = oyAllocateWrapFunc_( size, object ? object->allocateFunc_:0 );
    if(!block_)
      error = 1;
    else
    {
      size_ = size;
      error = !memcpy( block_, block, size );
    }
  }

  s = oyProfile_FromMemMove_( size_, &block_, flags, object );

  return s;
}


/** @brief create a fractional profile from signature
 *
 *  @param[in]    sig            signature
 *  @param[in]    type           type of signature to set
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
             oyProfile_FromSignature(  icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object )
{
  oyProfile_s * s = oyProfile_New_( object );
  int error = !s;

  if(!error)
    error = oyProfile_SetSignature( s, sig, type );

  return s;
}

/** @brief create new from existing profile struct
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_Copy_            ( const oyProfile_s  * profile,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc = 0;

  if(!profile)
    return s;

  s = oyProfile_New_( object );
  allocateFunc = s->oy_->allocateFunc_;

  if(profile->block_ && profile->size_)
  {
    s->block_ = s->oy_->allocateFunc_( profile->size_ );
    if(!s->block_)
      error = 1;
    else
    {
      s->size_ = profile->size_;
      error = !memcpy( s->block_, profile->block_, profile->size_ );
    }
  }

  if(!error)
  {
    if(!oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );
  }

  if(!error)
  {
    if(profile->sig_)
      s->sig_ = profile->sig_;
    else
      error = !oyProfile_GetSignature ( s, oySIGNATURE_COLOUR_SPACE );
  }

  if(!error)
    s->file_name_ = oyStringCopy_( profile->file_name_, allocateFunc );

  if(!error)
    s->use_default_ = profile->use_default_;

  if(!error)
  {
    s->channels_n_ = oyProfile_GetChannelsCount( s );
    error = (s->channels_n_ <= 0);
  }

  if(!error)
    oyProfile_SetChannelNames( s, profile->names_chan_ );

  if(error)
  {
    WARNc_S(("Could not create structure for profile."))
  }

  return s;
}

/** @brief copy from existing profile struct
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_Copy                ( oyProfile_s     * profile,
                                oyObject_s        object)
{
  oyProfile_s * s = 0;

  if(!profile)
    return s;

  if(profile && !object)
  {
    s = profile;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyProfile_Copy_( profile, object );
  /* TODO cache */
  return s;
}

/** @brief release correctly
 *
 *  set pointer to zero
 *
 *  @param[in]    address of Oyranos colour space struct pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int 
oyProfile_Release( oyProfile_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyProfile_s * s = 0;
  int i;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_PROFILE_S)
  {
    WARNc_S(("Attempt to release a non oyProfile_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->names_chan_)
    for(i = 0; i < s->channels_n_; ++i)
      if(s->names_chan_[i])
        oyObject_Release( &s->names_chan_[i] );
  /*oyOptions_Release( s->options );*/

  s->sig_ = (icColorSpaceSignature)0;

  oyStructList_Release(&s->tags_);

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->names_chan_)
      deallocateFunc( s->names_chan_ ); s->names_chan_ = 0;

    s->oy_->hash_[0] = 0;

    if(s->block_)
      deallocateFunc( s->block_ ); s->block_ = 0; s->size_ = 0;

    if(s->file_name_)
      deallocateFunc( s->file_name_ ); s->file_name_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @brief number of channels in a colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oyProfile_GetChannelsCount( oyProfile_s * profile )
{
  if(!profile)
    return 0;

  if(profile->channels_n_)
    return profile->channels_n_;

  profile->channels_n_ = oyICCColourSpaceGetChannelCount( profile->sig_ );

  return profile->channels_n_;
}

/** @brief get ICC colour space signature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI icSignature OYEXPORT
oyProfile_GetSignature ( oyProfile_s * s,
                         oySIGNATURE_TYPE_e type )
{
  icHeader *h = 0;
  icSignature sig = 0;

  if(!s)
    return 0;

  if(s->sig_ && type == oySIGNATURE_COLOUR_SPACE)
    return s->sig_;

  if(!s->block_)
  {
    if(type == oySIGNATURE_COLOUR_SPACE)
      sig = s->sig_ = icSigXYZData;
    return sig;
  }

  h = (icHeader*) s->block_;

  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /**< colour space */
       sig = s->sig_ = oyValueCSpaceSig( h->colorSpace ); break;
  case oySIGNATURE_PCS:                /**< profile connection space */
       sig = oyValueCSpaceSig( h->pcs ); break;
  case oySIGNATURE_SIZE:               /**< internal stored size */
       sig = oyValueUInt32( h->size ); break;
  case oySIGNATURE_CMM:                /**< prefered CMM */
       sig = oyValueUInt32( h->cmmId ); break;
  case oySIGNATURE_VERSION:            /**< version */
       sig = oyValueUInt32( h->version ); break;
  case oySIGNATURE_CLASS:              /**< usage class 'mntr' ... */
       sig = oyValueUInt32( h->deviceClass ); break;
  case oySIGNATURE_MAGIC:              /**< magic; ICC: 'acsp' */
       sig = oyValueUInt32( h->magic ); break;
  case oySIGNATURE_PLATFORM:           /**< operating system */
       sig = oyValueUInt32( h->platform ); break;
  case oySIGNATURE_OPTIONS:            /**< various ICC header flags */
       sig = oyValueUInt32( h->flags ); break;
  case oySIGNATURE_MANUFACTURER:       /**< device manufacturer */
       sig = oyValueUInt32( h->manufacturer ); break;
  case oySIGNATURE_MODEL:              /**< device modell */
       sig = oyValueUInt32( h->model ); break;
  case oySIGNATURE_INTENT:             /**< seldom used profile claimed intent*/
       sig = oyValueUInt32( h->renderingIntent ); break;
  case oySIGNATURE_CREATOR:            /**< profile creator ID */
       sig = oyValueUInt32( h->creator ); break;
  case oySIGNATURE_MAX: break;
  }

  return sig;
}

/** @brief set signature
 *
 *  @param[in]    sig            signature
 *  @param[in]    type           type of signature to set
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type )
{
  oyProfile_s * s = profile;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;

  if(!error)
    error = !s;

  if(!error && type == oySIGNATURE_COLOUR_SPACE)
  {
    if(sig)
      s->sig_ = sig;
    else
      s->sig_ = icSigXYZData;
    return error;
  }

  if(!error && !s->block_)
  {
    block_ = oyAllocateWrapFunc_( size_, s->oy_ ? s->oy_->allocateFunc_:0 );
    if(!block_)
      error = 1;

    if(!error)
      error = !memset( block_, 0, size_ );

    if(!error)
    {
      s->block_ = block_;
      s->size_ = size_;
    }
  }

  if(!error)
    h = (icHeader*) s->block_;

  if(!error)
  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /**< colour space */
       h->colorSpace = oyValueCSpaceSig( s->sig_ ); break;
  case oySIGNATURE_PCS:                /**< profile connection space */
       h->pcs = oyValueCSpaceSig( sig ); break;
  case oySIGNATURE_SIZE:               /**< internal stored size */
       h->size = oyValueUInt32( sig ); break;
  case oySIGNATURE_CMM:                /**< prefered CMM */
       h->cmmId = oyValueUInt32( sig ); break;
  case oySIGNATURE_VERSION:            /**< version */
       h->version = oyValueUInt32( sig ); break;
  case oySIGNATURE_CLASS:              /**< usage class 'mntr' ... */
       h->deviceClass = oyValueUInt32( sig ); break;
  case oySIGNATURE_MAGIC:              /**< magic; ICC: 'acsp' */
       h->magic = oyValueUInt32( sig ); break;
  case oySIGNATURE_PLATFORM:           /**< operating system */
       h->platform = oyValueUInt32( sig ); break;
  case oySIGNATURE_OPTIONS:            /**< various ICC header flags */
       h->flags = oyValueUInt32( sig ); break;
  case oySIGNATURE_MANUFACTURER:       /**< device manufacturer */
       h->manufacturer = oyValueUInt32( sig ); break;
  case oySIGNATURE_MODEL:              /**< device modell */
       h->model = oyValueUInt32( sig ); break;
  case oySIGNATURE_INTENT:             /**< seldom used profile claimed intent*/
       h->renderingIntent = oyValueUInt32( sig ); break;
  case oySIGNATURE_CREATOR:            /**< profile creator ID */
       h->creator = oyValueUInt32( sig ); break;
  case oySIGNATURE_MAX: break;
  }

  return error;
}

/** @brief set channel names
 *
 *  The function should be used to specify extra channels or unusual colour
 *  layouts like CMYKRB. The number of elements in names_chan should fit to the
 *  channels count or to the colour space signature.
 *
 *  You can let single entries empty if they are understandable by the
 *  colour space signature. Oyranos will set them for you on request.
 *
 *  @param[in] colour   address of a Oyranos named colour structure
 *  @param[in] names    pointer to channel names 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
void
oyProfile_SetChannelNames            ( oyProfile_s       * profile,
                                       oyObject_s        * names_chan )
{
  oyProfile_s * c = profile;
  int n = oyProfile_GetChannelsCount( profile );

  if(names_chan && n)
  {
    int i = 0;
    c->names_chan_ = c->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    c->names_chan_[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
        c->names_chan_[i] = oyObject_Copy( names_chan[i] );
  }
}

/** @brief get a channels name
 *
 *  A convinience function to get a single name with a certain type.
 *
 *  @param[in] profile  address of a Oyranos named colour structure
 *  @param[in] pos      position of channel 
 *  @param[in] type     sort of text 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
const oyChar *
oyProfile_GetChannelName           ( oyProfile_s   * profile,
                                        int                pos,
                                        oyNAME_e           type )
{
  oyProfile_s * s = profile;
  int n = oyProfile_GetChannelsCount( s );
  const oyChar * text = 0;

  if(!s)
    return 0;

  if( 0 <= pos && pos < n )
    return "-";

  if(!s->names_chan_)
    oyProfile_GetChannelNames ( s );

  if(s->names_chan_ && s->names_chan_[pos])
    text = oyObject_GetName( s->names_chan_[pos], type );

  return text;
}

/** @brief get channel names
 *
 *  @param[in] colour   address of a Oyranos named colour structure
 *  @param[in] names    pointer to channel names 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const oyObject_s *
oyProfile_GetChannelNames           ( oyProfile_s   * profile )
{
  oyProfile_s * s = profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = 0;
  icColorSpaceSignature sig = oyProfile_GetSignature( profile, oySIGNATURE_COLOUR_SPACE );

  if(!profile)
    return 0;

  if(!s->names_chan_ && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    if(!s->names_chan_)
      error = 1;
    if(!error)
    {
      s->names_chan_[ n ] = NULL;
      for( ; i < n; ++i )
      {
        s->names_chan_[i] = oyObject_NewFrom( s->oy_ );
        if(!s->names_chan_[i])
          error = 1;
        else
          error = oyObject_SetNames( s->names_chan_[i],
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NICK ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NAME ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_DESCRIPTION )
                      );
      }
    }
  }

  if(!error && s->names_chan_)
    return (const oyObject_s*) s->names_chan_;
  else
    return 0;
}

/** @brief get unique name
 *
 *  the returned string is identical to oyNAME_ID
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * s )
{
  int error = !s;
  const oyChar * text = 0;

  if(!error)
    text = oyObject_GetName( s->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    oyChar * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, oyChar, 1024, 0, error = 1 );

    /* A short number represents a default profile,
     * The length should not exceed OY_HASH_SIZE.
     */
    if(s->use_default_ && !found && !error)
    {
      oySprintf_(temp, "%d", s->use_default_);
      if(oyStrlen_(temp) < OY_HASH_SIZE)
        found = 1;
    }

    /* Do we have a file_name_? */
    if(s->file_name_ && !found && !error)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(oyStrlen_(temp))
        found = 1;
    }

    /* Do we have a hash_? */
    if(!found && !error)
    {
      if(!oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(!error)
      {
        uint32_t * i = (uint32_t*)&s->oy_->hash_[0];
        oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
        if(oyStrlen_(temp))
          found = 1;
      }
    }

    if(!error && !found)
      error = 1;

    if(!error)
      error = oyObject_SetName( s->oy_, temp, oyNAME_NICK );

    oyFree_m_( temp );

    if(!error)
      text = oyObject_GetName( s->oy_, oyNAME_NICK );
  }

  return text;
}

/** @brief get a presentable name
 *
 *  The type argument should select the folloing string in return:<br> 
 *  - oy_NAME_NAME - a readable ID
 *  - oy_NAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - profile internal name (icSigProfileDescriptionTag)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/06/23
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * s,
                                       oyNAME_e            type )
{
  int error = !s;
  const char * text = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  oyProfileTag_s * tag = 0;


  if(!error)
    if(type <= oyNAME_DESCRIPTION)
      text = oyObject_GetName( s->oy_, type );

  if(!error && !text)
  {
    oyChar * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, oyChar, 1024, 0, error = 1 );

    /* Ask the CMM? */
    if(!found && !error &&
       type == oyNAME_DESCRIPTION)
    {
      {
        tag = oyProfile_GetTagById( s, icSigProfileDescriptionTag );
        texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);

        if(texts_n && texts[0] && oyStrlen_(texts[0]))
        {
          memcpy(temp, texts[0], oyStrlen_(texts[0]));
          temp[oyStrlen_(texts[0])] = 0;
          found = 1;

          oyStringListRelease_( &texts, texts_n, tag->oy_->deallocateFunc_ );
        } else
          /* we try to get something as oyNAME_NAME */
        if(s->file_name_ && oyStrlen_(s->file_name_))
        {
          size_t len = oyStrlen_(s->file_name_);
          if(strrchr(s->file_name_,'/'))
          {
            len = oyStrlen_(s->file_name_);
            memcpy( temp, strrchr(s->file_name_,'/')+1, len );
          } else
            memcpy( temp, s->file_name_, len );
          temp[len] = 0;
          found = 1;
        }
      }
    }

    if(type == oyNAME_NAME)
    if(s->use_default_ && !found && !error)
    {
      oyWidgetTitleGet( s->use_default_, 0, &text, 0, 0 );

      oySprintf_(temp, "%s", text);
      if(oyStrlen_(temp))
        found = 1;
    }

    /* Do we have a file_name_? */
    if(type == oyNAME_NAME)
    if(s->file_name_ && !found && !error)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(oyStrlen_(temp))
        found = 1;
    }

    /* last rescue */
    if(!found && oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );

    if(!found && !error)
    {
      uint32_t * i = (uint32_t*)&s->oy_->hash_[0];
      oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
      if(oyStrlen_(temp))
        found = 1;
    }

    if(!error && !found)
      error = 1;

    if(!error)
      error = oyObject_SetName( s->oy_, temp, type );

    oyFree_m_( temp );

    if(!error)
      text = oyObject_GetName( s->oy_, type );
  }

  return text;
}

/** @func    oyProfile_GetMem
 *  @brief   get the ICC profile in memory
 *
 *  The prefered memory comes from a previously created tag list.
 *  Otherwise the memory block is copied.
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 */
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAllocFunc_t       allocateFunc )
{
  oyPointer block = 0;
  oyProfile_s * s = profile;
  int error = !s;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && s->type_ == oyOBJECT_TYPE_PROFILE_S)
  {
    if(oyStructList_Count( s->tags_ ))
    {
      block = oyProfile_TagsToMem_ ( profile, size, allocateFunc );

    } else
    if(s->size_ && s->block_)
    {

      block = oyAllocateWrapFunc_( s->size_, allocateFunc );
      error = !block;
      if(!error)
        error = !memcpy( block, s->block_, s->size_ );
      if(!error && size)
        *size = s->size_;
    }
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return block;
}

/** @func    oyProfile_GetFileName
 *  @brief   get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or as a
 *  from Oyranos defaults the assiciated filename will simply be retuned.
 *
 *  @param         profile             the profile
 *  @param         dl_pos              -1, or the position in a device links source chain
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
const oyChar *     oyProfile_GetFileName ( oyProfile_s   * profile,
                                       int                 dl_pos )
{
  const oyChar * name = 0;
  oyProfile_s * s = profile, * tmp = 0;
  int error = !s;
  oyChar ** names = 0;
  uint32_t count = 0, i = 0;
  oyProfileTag_s * psid = 0;
  oyChar ** texts = 0;
  int32_t   texts_n = 0;
  oyChar *  hash = 0;
  oyChar    tmp_hash[34];
  int       dl_n = 0;

  if(!error && s->type_ == oyOBJECT_TYPE_PROFILE_S)
  {
    if(dl_pos >= 0)
    {
      psid = oyProfile_GetTagById( s, icSigProfileSequenceIdentifierTag );
      texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);

      if(texts && texts_n > 1+dl_pos*5+2)
      {
        dl_n = 1 + dl_pos*5+2;
        hash = texts[dl_n];
      }
    }


    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else {

      names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], oyNO_CACHE_WRITE, 0 );

          if(hash && tmp)
          {
            uint32_t * h = (uint32_t*)&tmp->oy_->hash_[0];
            oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
            if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
            {
              name = names[i];
              break;
            }
          } else
          if(oyProfile_Equal_( s, tmp ))
          {
            name = names[i];
            break;
          }

          oyProfile_Release( &tmp );
        }
      }
      s->file_name_ = oyFindProfile_( name );
      name = oyStringCopy_( s->file_name_, s->oy_->allocateFunc_ );
      oyDeAllocateFunc_( s->file_name_ );
      s->file_name_ = (char*)name;

      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return name;
}


/** @brief get a CMM specific pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
oyCMMptr_s * oyProfile_GetCMMPtr_     ( oyProfile_s     * profile,
                                        const char      * cmm )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyCMMptr_s * cmm_ptr = 0;

  if(!error && !cmm)
  {
    cmm = oyModuleGetActual(0);
    error = !cmm;
  }

  if(!error)
  {
    /*oyCMMptr_s *cmm_ptr = 0;*/
    const oyChar * tmp = 0;
 
    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /*  Cache Search
     *  1.     hash from input
     *  2.     query for hash in cache
     *  3.     check
     *  3a.       eighter take cache entry
     *  3b.       or ask CMM
     *  3b.1.                update cache entry
     */

    /* 1. create hash text */
    hashTextAdd_m( cmm );
    hashTextAdd_m( " oyPR:" );
    tmp = oyProfile_GetID( s );
    hashTextAdd_m( tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );
    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );

    if(!error)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_TYPE_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        /* 3b. ask CMM */
        char cmm_used[] = {0,0,0,0,0};
        oyCMMProfile_Open_t funcP = 0;

        oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API1_S,
                                          cmm, 0, cmm_used, 0,0 );
        if(api && *(uint32_t*)&cmm_used)
        {
          oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
          funcP = api1->oyCMMProfile_Open;
        }

        if(funcP)
        {
          cmm_ptr = oyCMMptr_New_(s->oy_->allocateFunc_);
          error = !cmm_ptr;

          if(!error)
            error = oyCMMptr_Set_( cmm_ptr, cmm_used, 0,
                                   oyCMM_PROFILE, 0, 0 );

          if(!error)
          {
            error = funcP( s->block_, s->size_, cmm_ptr );

#if 0
            /* We have currently no means to trace all the spread resources. */
            if(!error)
              error = oyCMMRelease_( cmm );
#endif
          }
        }

        error = !cmm_ptr;

        if(!error && cmm_ptr && cmm_ptr->ptr)
          /* 3b.1. update cache entry */
          error = oyHash_SetPointer_( entry,
                                      (oyStruct_s*) oyCMMptr_Copy_(cmm_ptr, 0) );
      }
    }


    oyHash_Release_( &entry );
  }

  return cmm_ptr;
}

#if 0
/** @brief get a CMM specific pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
/*
oyChar *       oyProfile_GetCMMText_ ( oyProfile_s       * profile,
                                       oyNAME_e            type,
                                       const char        * language,
                                       const char        * country )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyChar * name = 0;
  char cmm_used[] = {0,0,0,0,0};

  if(!error)
  {
    oyCMMProfile_GetText_t funcP = 0;
    oyCMMptr_s  * cmm_ptr = 0;


    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API1_S,
                                      0, 0, cmm_used );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMProfile_GetText;
    }

    if(*(uint32_t*)&cmm_used)
      cmm_ptr = oyProfile_GetCMMPtr_( s, cmm_used );

    if(funcP && cmm_ptr)
    {
      name = funcP(cmm_ptr, type, language, country, s->oy_->allocateFunc_);

      oyCMMdsoRelease_( cmm_used );
    }
  }

  return name;
}
*/
#endif

/** @internal
 *  @func    oyProfile_Equal_
 *  @brief   check if two profiles are qual by their hash sum
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/03
 *  @since   2008/02/03 (Oyranos: 0.1.8)
 */
int          oyProfile_Equal_        ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB )
{
  int equal = 0;

  if(profileA && profileB)
  {
    equal = memcmp( profileA->oy_->hash_, profileB->oy_->hash_, OY_HASH_SIZE )
            == 0;
  }

  return equal;
}

/** @internal
 *  @func    oyProfile_Match_
 *  @brief   check if a profiles matches by some properties
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Match_        ( oyProfile_s       * pattern,
                                       oyProfile_s       * profile )
{
  int32_t match = 0;
  int i;
  icSignature pattern_sig, profile_sig;

  if(pattern && profile)
  {
    /*match = oyProfile_Equal_(pattern, profile);*/ /* too expensive */
    if(!match)
    {
      match = 1;
      for( i = 0; i < (int)oySIGNATURE_MAX; ++i)
      {
        pattern_sig = oyProfile_GetSignature( pattern, (oySIGNATURE_TYPE_e) i );
        profile_sig = oyProfile_GetSignature( profile, (oySIGNATURE_TYPE_e) i );

        if(pattern_sig && profile_sig && pattern_sig != profile_sig)
          match = 0;
      }
    }
  }

  return match;
}

/** @internal
 *  @func    oyProfile_Hashed_
 *  @brief   check if a profile has a hash sum computed
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
int32_t      oyProfile_Hashed_       ( oyProfile_s       * s )
{
  int32_t hashed = 0;
  if(s && s->type_ == oyOBJECT_TYPE_PROFILE_S)
      if(((uint32_t*)(&s->oy_->hash_[0])) ||
         ((uint32_t*)(&s->oy_->hash_[4])) ||
         ((uint32_t*)(&s->oy_->hash_[8])) ||
         ((uint32_t*)(&s->oy_->hash_[12])) )
        hashed = 1;
  return hashed;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  20 december 2007 (API 0.1.8)
 */
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name )
{
  oyProfile_s * s = profile;
  int error = !s || !file_name;
  oyPointer buf = 0;
  size_t size = 0;

  if(!error)
  {
    buf = oyProfile_GetMem ( s, &size, 0, 0 );
    if(buf && size)
    error = oyWriteMemToFile_( file_name, buf, size );

    if(buf) oyDeAllocateFunc_(buf);
    size = 0;
  }

  return error;
}

/** @func    oyProfile_WriteHeader_
 *  @brief   get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s       * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  char h[5] = {"head"};
  uint32_t * hi = (uint32_t*)&h;

  if(profile && profile->block_ && profile->size_ > 132 &&
     profile->tags_)
  {
    int n = oyProfile_GetTagCount( profile );
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, 0 );

    if(n && tag->use == *hi && tag->block_ && tag->size_ >= 128)
    {
      block = oyAllocateFunc_ (132);
      if(block)
        memcpy( block, tag->block_, 128 );
    }

    oyProfileTag_Release( &tag );
  }

  return block;
}

/** @func    oyProfile_WriteTagTable_
 *  @brief   get the parsed ICC profile back into memory
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTagTable_( oyProfile_s       * profile,
                                       size_t            * size )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(!error)
  {
    int n = oyProfile_GetTagCount( profile );
    size_t size = 0;

    size = sizeof (icTag) * n;
    error = !size;

    if(!error)
    {
      block = oyAllocateFunc_( size );
      error = !block;
    }

    if(!error)
      error = !memset( block, 0, size );
  }

  return block;
}

/** @func    oyProfile_WriteTags_
 *  @brief   get the parsed ICC profile back into memory
 *
 *  Call in following order:
 *         -  oyProfile_WriteHeader_
 *         -  oyProfile_WriteTagTable_
 *         -  oyProfile_WriteTags_
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_WriteTags_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAllocFunc_t       allocateFunc )
{
  char * block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size &&
                icc_header && icc_list);

  if(!error)
  {
    int n = 0, i;
    size_t len = 0;

    n = oyProfile_GetTagCount( profile );
    block = (char*) oyAllocateFunc_(132 + n * sizeof(icTag));
    error = !block;

    if(!error)
      error = !memcpy( block, icc_header, 132 );

    len = 132;

    if(!error)
      error = !memcpy( &block[len], icc_list, (n-1) * sizeof(icTag) );

    len += sizeof(icTag) * (n-1);

    for(i = 0; i < n - 1; ++i)
    {
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char * temp = 0;
      icTagList* list = (icTagList*) &block[128];
      oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( profile, i + 1 );
      size_t size = 0;

      if(!error)
        error = !tag;

      if(!error)
        size = tag->size_;

      if(!error && tag->use == *hi)
      {
        oyProfileTag_Release( &tag );
        continue;
      }

      if(!error)
      {
        list->tags[i].sig = oyValueUInt32( (icTagSignature)tag->use );
        list->tags[i].offset = oyValueUInt32( (icUInt32Number)len );
        list->tags[i].size = oyValueUInt32( (icUInt32Number)size );
        temp = (char*) oyAllocateFunc_ ( len + size + 
                                               (size%4 ? 4 - size%4 : 0));
      }

      if(temp)
        error = !memcpy( temp, block, len );
      if(!error)
      {
        error = !memcpy( &temp[len], tag->block_, tag->size_);
        len += size + (size%4 ? 4 - size%4 : 0);
      }

      if(!error)
      {
        oyDeAllocateFunc_(block);
        block = temp;

        oyProfileTag_Release( &tag );
      }
      temp = 0;
    }

    if(!error)
    {
      char h[5] = {OY_MODULE_NICK};
      uint32_t * hi = (uint32_t*)&h;
      icProfile* p = 0;
      icHeader* header = 0;
      oyPointer temp = oyAllocateWrapFunc_( len, allocateFunc );

      error = !temp;
      if(!error)
        error = !memcpy( temp, block, len );

      oyDeAllocateFunc_( block );
      block = temp; temp = 0;

      p = (icProfile*) block;
      p->count = oyValueUInt32( (icUInt32Number) n - 1);

      header = (icHeader*) block;
      header->size = oyValueUInt32( (icUInt32Number) len);
#if 0 /* we dont override the CMM's id */
      header->creator = *hi;
#endif
#if !defined(__APPLE__) && !defined(WIN32)
      oySprintf_( h, "*nix" );
      header->platform = *hi;
#endif
      *size = len;
    }
  }

  return block;
}

/** @func    oyProfile_TagsToMem_
 *  @internal
 *  @brief   get the parsed ICC profile back into memory
 *
 *  non thread save
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/30
 *  @since   2008/01/30 (Oyranos: 0.1.8)
 */
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc )
{
  oyPointer block = 0;
  int error = !(profile && profile->block_ &&
                profile->size_ > 132 && profile->tags_ && size);

  if(!error)
  {
    size_t size_ = 0;

    oyPointer icc_header = 0;
    oyPointer icc_tagtable = 0;

    /* 1. header */
    icc_header = oyProfile_WriteHeader_( profile, &size_ );

    error = !icc_header;

    /* 2. tag table */
    if(!error)
    {
      icc_tagtable = oyProfile_WriteTagTable_( profile, &size_ );
      error = !icc_tagtable;
    }

    /* 3. tags */
    if(!error)
    {
      block = oyProfile_WriteTags_( profile, &size_, icc_header, icc_tagtable,
                                    allocateFunc );
      error = !block;
    }

    if(!error)
    {
      oyDeAllocateFunc_(icc_header);
      oyDeAllocateFunc_(icc_tagtable);
      *size = size_;
    }
  }

  return block;
}

/** @func  oyProfile_GetTagById
 *  @internal
 *  @brief get a profile tag by its tag signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     id                  icTagSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2 january 2008 (API 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagById( oyProfile_s       * profile,
                                       icTagSignature      id )
{
  oyProfile_s * s = profile;
  int error = !s;
  oyProfileTag_s * tag = 0,
                 * tmp = 0;
  int i = 0, n = 0;
  icTagSignature tag_id_ = 0;

  if(!error && profile->type_ != oyOBJECT_TYPE_PROFILE_S)
    error = 1;

  if(!error)
  {
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );
    n = oyProfile_GetTagCount( s );
  }

  if(!error && n)
  {
    for(i = 0; i < n; ++i)
    {
      tmp = oyProfile_GetTagByPos_( s, i );
      tag_id_ = 0;

      if(tmp)
        tag_id_ = tmp->use;

      if(tag_id_ == id)
      {
        tag = tmp; tmp = 0;
        break;
      } else
        oyProfileTag_Release( &tmp );
    }
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}


/** @func  oyProfile_GetTag
 *  @internal
 *  @brief get a profile tag
 *
 *  non thread save
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s     * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s * s = 0;
  int error = !profile;
  int n = 0;

  if(!error && profile->type_ != oyOBJECT_TYPE_PROFILE_S)
    error = 1;

  if(!error)
  {
    s = profile;
    n = oyStructList_Count( profile->tags_ );
  }

  if(!error && n)
  {
    tag = (oyProfileTag_s*) oyStructList_GetRef( profile->tags_, pos );
    return tag;
  }

  /* parse the ICC profile struct */
  if(!error && s->block_)
  {
    icSignature magic = oyProfile_GetSignature( s, oySIGNATURE_MAGIC );
    icSignature profile_cmmId = oyProfile_GetSignature( s, oySIGNATURE_CMM );
    char profile_cmm[5] = {0,0,0,0,0};
    icProfile * ic_profile = s->block_;
    int min_icc_size = 132 + sizeof(icTag);

    error = (magic != icMagicNumber);

    profile_cmmId = oyValueUInt32( profile_cmmId );
    error = !memcpy( profile_cmm, &profile_cmmId, 4 );
    profile_cmmId = 0;

    if(!error && s->size_ > min_icc_size)
    {
      uint32_t size = 0;
      uint32_t tag_count = 0;
      icTag *tag_list = 0;
      int i = 0, j;
      oyProfileTag_s * tag_ = oyProfileTag_New( 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char *tag_block = 0;

      tag_block = oyAllocateFunc_( 132 );
      error = !memcpy( tag_block, s->block_, 132 );
      error = oyProfileTag_Set( tag_, (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, tag_block );
      if(!error)
        error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );

      if(0 == pos)
        tag = oyProfileTag_Copy( tag_, 0 );
      error = oyProfile_AddTag( s, &tag_, -1 );


      size = oyProfile_GetSignature( s, oySIGNATURE_SIZE );
      tag_count = oyValueUInt32( ic_profile->count );

      tag_list = (icTag*)&((char*)s->block_)[132];

      for(i = 0; i < tag_count; ++i)
      {
        icTag *ic_tag = &tag_list[i];
        size_t offset = oyValueUInt32( ic_tag->offset );
        size_t tag_size = oyValueUInt32( ic_tag->size );
        char *tmp = 0;
        oyChar **texts = 0;
        int32_t texts_n = 0;
        oySTATUS_e status = oyOK;
        icTagSignature sig = oyValueUInt32( ic_tag->sig );
        icTagTypeSignature tag_type = 0;

        oyProfileTag_s * tag_ = oyProfileTag_New( 0 );

        tag_block = 0;

        if((offset+tag_size) > s->size_)
          status = oyCORRUPTED;
        else
        {
          icTagBase * tag_base = 0;

          tag_block = oyAllocateFunc_( tag_size );
          tmp = &((char*)s->block_)[offset];
          error = !memcpy( tag_block, tmp, tag_size );

          tag_base = (icTagBase*) tag_block; 
          tag_type = oyValueUInt32( tag_base->sig );
        }

        error = oyProfileTag_Set( tag_, sig, tag_type,
                                  status, tag_size, tag_block );
        tag_->offset_orig = offset;
        if(!error)
          error = !memcpy( tag_->profile_cmm_, profile_cmm, 4 );
        if(!error)
          error = !memcpy( tag_->required_cmm, OY_MODULE_NICK, 4 );

        DBG_PROG5_S("%d[%d @ %d]: %s %s", 
          i, (int)tag_->size_, (int)tag_->offset_orig,
          oyICCTagTypeName( tag_->tag_type_ ),
          oyICCTagDescription( tag_->use ) );
        texts = oyProfileTag_GetText(tag_,&texts_n,0,0,0,0);
        for(j = 0; j < texts_n; ++j)
          DBG_PROG2_S("%s: %s", tag_->last_cmm_, texts[j]?texts[j]:"");
        if(texts_n && texts)
          oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );

        if(i == pos-1)
          tag = oyProfileTag_Copy( tag_, 0 );

        if(!error)
          error = oyProfile_AddTag( s, &tag_, -1 );
      }
    }
  }

  return tag;
}

/** @func  oyProfile_GetTag
 *  @brief get a profile tag
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/06
 *  @since   2008/02/06 (Oyranos: 0.1.8)
 */
oyProfileTag_s * oyProfile_GetTagByPos(oyProfile_s       * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s * s = profile;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  tag = oyProfile_GetTagByPos_( s, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}

/** @func oyProfile_GetTagCount
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
int                oyProfile_GetTagCount( oyProfile_s    * profile )
{
  int n = 0;
  oyProfile_s *s = profile;
  int error = !s;

  if(!error && !(s && s->type_ == oyOBJECT_TYPE_PROFILE_S && s->tags_))
    error = 1;

  if(!error)
    n = oyStructList_Count( s->tags_ );

  if(!n)
  {
    oyProfileTag_s * tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );
  }

  return n;
}

/** @func    oyProfile_AddTag
 *  @internal
 *  @brief   add a tag to a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 */
int                oyProfile_AddTag  ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = !s;

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_TYPE_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error)
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** @func    oyProfile_TagReleaseAt
 *  @internal
 *  @brief   remove a tag from a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/03/11
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 */
int                oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos )
{
  oyProfile_s * s = profile;
  int error = 0;

  if(!(s && s->type_ == oyOBJECT_TYPE_PROFILE_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error)
    error = oyStructList_ReleaseAt ( s->tags_, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}





/** @func oyProfileTag_New
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                   oyProfileTag_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_PROFILE_TAG_S;
# define STRUCT_TYPE oyProfileTag_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyProfileTag_Copy;
  s->release = (oyStruct_ReleaseF_t) oyProfileTag_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @func  oyProfileTag_Create
 *
 *  The API relies on an generic arguments inside a list. The arguments are not
 *  specified here but in the appropriate moduls. This allowes flixibility, 
 *  which needs more understanding.
 *
 *  For the effect of the parameters look at the appropriate module
 *  documentation and the function infos.
 *  @see oyraProfileTag_Create
 *  @see oyraFunctionGetInfo
 *
 *  @param[in]     list                a list of arguments
 *  @param[in]     tag_type            type to create
 *  @param[in]     version             version as supported
 *  @param[in,out] required_cmm        in: CMM to create the tag; out: used CMM
 *  @param[in]     object              the user object for the tag creation
 *  @return                            a profile tag
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 *  @date    2008/01/08
 */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       const char        * required_cmm,
                                       oyObject_s          object)
{
  oyProfileTag_s * s = 0, * tag = 0;
  int error = !list;
  oyCMMProfileTag_Create_t funcP = 0;
  char cmm[] = {0,0,0,0,0};
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_WRITE, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  if(!error && list->type_ != oyOBJECT_TYPE_STRUCT_LIST_S)
    error = 1;

  if(!error)
  {
    query.value = tag_type;
    query_[0] = &query;
    queries.queries = query_;
    if(required_cmm)
      error = !memcpy( queries.prefered_cmm, required_cmm, 4 ); 

    if(!error && required_cmm)
      error = !memcpy( cmm, required_cmm, 4 );
  }

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API3_S,
                                      cmm, &queries, cmm, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_Create;
    }
    error = !funcP;
  }

  if(!error)
  {
    tag = oyProfileTag_New( object );
    error = !tag;
    
    if(!error)
      error = funcP( tag, list, tag_type, version );

    if(!error)
      error = !memcpy( tag->last_cmm_, cmm, 4 );
    if(!error)
      s = tag;
  }

  return s;
}

/** @func  oyProfileTag_Copy
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                   oyProfileTag_Copy   ( oyProfileTag_s  * obj,
                                         oyObject_s        object)
{
  oyProfileTag_s * s = 0;
  int error = 0;

  if(!obj)
    return s;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(!error && !object && obj->oy_)
  {
    oyObject_Ref( obj->oy_ );
    return obj;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return s;
}

/** @func oyProfileTag_Release
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Release(oyProfileTag_s  ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyProfileTag_s * s = 0;

  if(!obj || !*obj)
    return error;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_PROFILE_TAG_S)
  {
    WARNc_S(("Attempt to release a non oyProfileTag_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return error;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->block_ && s->size_)
      deallocateFunc( s->block_ );
    s->block_ = 0; s->size_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/** @func oyProfileTag_Set
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileTag_Set  ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block )
{
  oyProfileTag_s * s = tag;
  int error = !s;

  if(!error && s->type_ != oyOBJECT_TYPE_PROFILE_TAG_S)
    error = 1;

  if(!error)
  {
    s->use = sig;
    s->tag_type_ = type;
    s->status_ = status;
    s->size_ = tag_size;
    s->block_ = tag_block;
  }

  return error;
}

/** @func oyProfileTag_GetText
 *
 *  For the affect of the parameters look at the appropriate module.
 *  @see oyraProfileTag_GetValues
 *
 *  Hint: to select a certain module use the oyProfileTag_s::required_cmm
 *  element from the tag parameter.
 *
 *  For localised strings, e.g. icSigMultiLocalizedUnicodeType:<br>
 *    - zero language and country args: all localisation strings are returned 
 *    - with language and/or country args: return appropriate matches
 *    - for language != "", the string starts with language code, the text follows after a colon ":"
 *    - a non zero but empty language argument == "", 
 *      returns the pure string in the actual Oyranos locale, no language code
 *
 *  @param[in]     tag                 the tag to read
 *  @param[out]    n                   the number of returned strings
 *  @param[in]     language            2 byte language code, or "" for current
 *  @param[in]     country             2 byte country code
 *  @param[out]    tag_size            the processed tag size
 *  @param[in]     allocateFunc        the user allocator for the returned list
 *  @return                            a list of strings
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2008/06/19
 */
char **        oyProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAllocFunc_t       allocateFunc )
{
  oyProfileTag_s * s = tag;
  int error = !s;
  oyCMMProfileTag_GetValues_t funcP = 0;
  char cmm[] = {0,0,0,0,0}, 
       t_l[8], t_c[8], *t_ptr;
  int implicite_i18n = 0;
  char ** texts = 0, * text = 0, *temp = 0;
  oyStructList_s * values = 0;
  oyName_s * name = 0;
  int values_n = 0, i = 0, k;
  int32_t texts_n = 0;
  oyCMMapiQuery_s query = {oyQUERY_PROFILE_TAG_TYPE_READ, 0, oyREQUEST_HARD};
  oyCMMapiQuery_s *query_[2] = {0,0};
  oyCMMapiQueries_s queries = {1,0};

  if(!error && s->type_ != oyOBJECT_TYPE_PROFILE_TAG_S)
    error = 1;

  if(!error)
  {
    query.value = tag->tag_type_;
    query_[0] = &query;
    queries.queries = query_;
    error = !memcpy( queries.prefered_cmm, tag->profile_cmm_, 4 ); 

    if(!error)
      error = !memcpy( cmm, tag->required_cmm, 4 );
  }

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API3_S,
                                      cmm, &queries, cmm, 0,0 );
    if(api && *(uint32_t*)&cmm)
    {
      oyCMMapi3_s * api3 = (oyCMMapi3_s*) api;
      funcP = api3->oyCMMProfileTag_GetValues;
    }
    error = !funcP;
  }

  if(!error)
  {
    /* check for a "" in the lang variable -> want the best i18n match */
    if(language && !oyStrlen_(language))
    {
      implicite_i18n = 1;
      language = oyLanguage() ? oyLanguage() : "";
      country  = oyCountry() ? oyCountry() : "";
    }

    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    values = funcP( tag );

      if(oyStructList_Count( values ) )
      {
        name = 0;
        values_n = oyStructList_Count( values );

        for(k = 0; k < 4; ++k)
        {
          for(i = 0; i < values_n; ++i)
          {
            text = 0;
            name = (oyName_s*) oyStructList_GetRefType( values, i,
                                                        oyOBJECT_TYPE_NAME_S );
            memcpy(t_l, name->lang, 8); t_c[0] = 0;
            t_ptr = oyStrchr_(t_l, '_');
            if(t_ptr)
            {
              memcpy(t_c, t_ptr+1, 3);
              *t_ptr = 0;
            }

            if(name)
              text = name->name;
            if(name)
            /* select by language and/or country or best i18n match or all */
            if(
               (k == 0 && language && oyStrcmp_( language, t_l ) == 0 &&
                          country  && oyStrcmp_( country, t_c ) )       ||
               (k == 1 && language && oyStrcmp_( language, t_l ) == 0 &&
                          (!country || implicite_i18n ))                ||
               (k == 2 && country  && oyStrcmp_( country, t_c ) == 0  &&
                          (!language || implicite_i18n ))               ||
               (k == 3 && ((!language && !country) || implicite_i18n))
              )
            {
              if(oyStrlen_(name->lang) && !implicite_i18n)
              {
                /* string with i18n infos -> "de_DE:Licht" */
                temp = oyStringAppend_(name->lang, ":", oyAllocateFunc_);
                temp = oyStringAppend_(temp, text, oyAllocateFunc_);
                oyStringListAddString_( &texts, &texts_n, &temp,
                                            oyAllocateFunc_, oyDeAllocateFunc_);

              } else {
                /* pure string -> "Licht" */
                oyStringListAddStaticString_( &texts, &texts_n, text,
                                            oyAllocateFunc_, oyDeAllocateFunc_);
                /* no selection for best i18n match and no lang: take all */
                if(k == 3 && implicite_i18n)
                {
                  implicite_i18n = 0;
                  language = 0;
                  country = 0;
                }
              }
            }

            /* best i18n match found -> end */
            if(implicite_i18n && texts_n)
            {
              k = 4;
              break;
            }
          }
        }

        *n = texts_n;
      }
    oyStructList_Release( &values );

    error = !memcpy( tag->last_cmm_, cmm, 4 );
  }

  return texts;
}




/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI oyProfileList_s * OYEXPORT
                   oyProfileList_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_PROFILE_LIST_S;
# define STRUCT_TYPE oyProfileList_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyProfileList_Copy;
  s->release = (oyStruct_ReleaseF_t) oyProfileList_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_New( 0 );

  return s;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI oyProfileList_s * OYEXPORT
                   oyProfileList_Copy  ( oyProfileList_s * obj,
                                         oyObject_s        object)
{
  oyProfileList_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc = 0;

  if(!obj)
    return s;

  s = oyProfileList_New( object );
  error = !s;

  if(!error)
    allocateFunc = s->oy_->allocateFunc_;

  if(!error)
  {
    if(obj->list_)
    {
      s->list_ = oyStructList_Copy(obj->list_, object);
      error = !s->list_;
    }
  }

  if(error)
  {
    WARNc_S(("Could not create structure for profile."))
    return 0;
  }

  return s;
}

/** @func    oyProfileList_Create
 *  @brief   get a list of installed profiles
 *
 *  @param[in]     patterns            a list properties, e.g. classes
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 *  @date    2008/06/20
 */
OYAPI oyProfileList_s * OYEXPORT
                 oyProfileList_Create( oyProfileList_s   * patterns,
                                       oyObject_s          object)
{
  oyProfileList_s * s = 0;
  int error = 0;

  oyProfile_s * tmp = 0, * pattern = 0;
  char  ** names = 0, * full_name = 0;
  oyPointer block = 0;
  uint32_t names_n = 0, i = 0, j = 0,
           patterns_n = oyProfileList_Count(patterns);
  size_t   size = 128;

  s = oyProfileList_New( object );
  error = !s;

  if(!error)
  {
    names = oyProfileListGet_ ( NULL, &names_n );

    for(j = 0; j < patterns_n; ++j)
    {
      pattern = oyProfileList_Get(patterns, j);

      if(pattern->size_ > 132)
        size = 0;
    }

    for(i = 0; i < names_n; ++i)
    {
      if(names[i])
      {
        if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
        {
          if(size && 0)
          { // TODO
            full_name = oyFindProfile_(names[i]);
            block = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
            tmp = oyProfile_FromMemMove_( size, &block, 0, 0 );
          }
          else
            tmp = oyProfile_FromFile( names[i], oyNO_CACHE_WRITE, 0 );
        }

        if(patterns_n > 0)
        {
          for(j = 0; j < patterns_n; ++j)
          {
            if(tmp)
              pattern = oyProfileList_Get(patterns, j);

            if(oyProfile_Match_( pattern, tmp ))
            {
              s = oyProfileList_MoveIn( s, &tmp, -1);
              error = !s;
              break;
            }
          }

        } else {

          s = oyProfileList_MoveIn( s, &tmp, -1);
          error = !s;
        }
      }
    }

    oyStringListRelease_( &names, names_n, oyDeAllocateFunc_ );
  }

  return s;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
OYAPI int  OYEXPORT
                   oyProfileList_Release(oyProfileList_s** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyProfileList_s * s = 0;

  if(!obj || !*obj)
    return error;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_PROFILE_LIST_S)
  {
    WARNc_S(("Attempt to release a non oyProfileList_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return error;
  /* ---- end of common object destructor ------- */

  if(!error && s->list_)
  error = oyStructList_Release(&s->list_);

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
oyProfileList_s* oyProfileList_MoveIn( oyProfileList_s   * list,
                                       oyProfile_s      ** obj,
                                       int                 pos )
{
  int error = 0;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_TYPE_PROFILE_S)
  {
    if(!list)
      list = oyProfileList_New(0);

    if(list && list->list_)
        error = oyStructList_MoveIn( list->list_, (oyStruct_s**) obj, pos );
  }

  return list;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfileList_ReleaseAt( oyProfileList_s * list,
                                       int                 pos )
{
  int error = 0;

  if(list && list->list_)
    error = oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** @internal
 *
 *  @param[in] list                    the profile list to use
 *  @param[in] pos                     the position in list
 *  @return                            a copy of the profile owned by the caller
 *
 *  @since Oyranos: version 0.1.8  2007/11/22
 *  @date  20 december 2007 (API 0.1.8)
 */
oyProfile_s *    oyProfileList_Get   ( oyProfileList_s   * list,
                                       int                 pos )
{
  oyProfile_s * obj = 0;

  if(list && list->list_)
  {
    oyProfile_s * p = (oyProfile_s*) oyStructList_GetType_( list->list_,
                                                 pos, oyOBJECT_TYPE_PROFILE_S );

    if(p)
      obj = oyProfile_Copy(p, 0);
  }

  return obj;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyCMMptr_s** oyProfileList_GetCMMptrs_(oyProfileList_s   * list,
                                       const char        * cmm )
{
  oyCMMptr_s ** obj = 0;
  int n = oyProfileList_Count( list );


  if(list && list->list_)
  {
    int i = 0;

    if(n)
      obj = list->oy_->allocateFunc_( sizeof(oyCMMptr_s*) * n); 

    for(i = 0; i < n; ++i)
    {
      oyProfile_s * p = (oyProfile_s*) oyStructList_GetType_( list->list_,
                                                 i, oyOBJECT_TYPE_PROFILE_S );

      if(p)
      {
        oyCMMptr_s * cmm_ptr = oyProfile_GetCMMPtr_(p, cmm);

        if(cmm_ptr && cmm_ptr->type == oyOBJECT_TYPE_CMM_POINTER_S)
          obj[i] = oyCMMptr_Copy_( cmm_ptr, 0 );
      }
    }
  }

  return obj;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfileList_Count ( oyProfileList_s   * list )
{
  int n = 0;

  if(list && list->list_)
    n = oyStructList_Count( list->list_ );

  return n;
}

/** @} */




/** \addtogroup misc Miscellaneous

 *  @{
 */

/** @internal
 *  @brief new
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_New_         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_REGION_S;
# define STRUCT_TYPE oyRegion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyRegion_Copy;
  s->release = (oyStruct_ReleaseF_t) oyRegion_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** 
 *  @brief new with geometry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_NewWith      ( oyObject_s          object,
                                       float               x,
                                       float               y,
                                       float               width,
                                       float               height )
{
  oyRegion_s * s = oyRegion_New_( object );
  if(s)
    oyRegion_SetGeo( s, x, y, width, height );
  return s;
}

/**
 *  @brief new from other region
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_NewFrom      ( oyObject_s          object,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = oyRegion_New_( object );
  if(s)
    oyRegion_SetByRegion(s, ref);
  return s;
}

/**
 *  @brief copy/reference from other region
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRegion_s *   oyRegion_Copy         ( oyRegion_s        * orig,
                                       oyObject_s          object )
{
  oyRegion_s * s = 0;

  if(!orig)
    return s;

  if(object)
  {
    s = oyRegion_NewFrom( object, orig );

  } else {

    s = orig;
    oyObject_Copy( s->oy_ );
  }

  return s;
}

/**
 *  @brief release
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_Release      ( oyRegion_s       ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyRegion_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_REGION_S)
  {
    WARNc_S(("Attempt to release a non oyRegion_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/**
 *  @brief set geometry
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_SetGeo       ( oyRegion_s        * edit_region,
                                       float               x,
                                       float               y,
                                       float               width,
                                       float               height )
{
  oyRegion_s * s = edit_region;
  if(!s)
    return;
  s->x = x;
  s->y = y;
  s->width = width;
  s->height = height;
}

/**
 *  @brief copy values
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_SetByRegion  ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  if(!s)
    return;

  oyRegion_SetGeo( s, ref->x, ref->y, ref->width, ref->height );
}

/**
 *  @brief trim edit_region to ref extents
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Trim         ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  if(!s)
    return;

  if (r->x < ref->x)
  { 
    r->width -= ref->x - r->x;
    r->x = ref->x;
  }
  if (r->x + r->width > ref->x + ref->width)
    r->width -= (r->x + r->width) - (ref->x + ref->width);
  if( r->width < 0 )
    r->width = 0;
  
  if (r->y < ref->y)
  {
    r->height -= ref->y - r->y;
    r->y = ref->y;
  }
  if (r->y + r->height > ref->y + ref->height)
    r->height -= (r->y + r->height) - (ref->y + ref->height);
  if( r->height < 0 )
    r->height = 0;

  oyRegion_Normalise( r );
}

/**
 *  @brief trim edit_region to ref extents
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_MoveInside   ( oyRegion_s        * edit_region,
                                       oyRegion_s        * ref )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * a = ref;

  if(!s)
    return;

  oyRegion_Normalise( s );

  if (s->x < a->x)
    s->x = a->x;
  if (s->x+s->width > a->x+a->width)
  { if (s->width > a->width)
      ; /* Lassen */
    else
      s->x = a->x+a->width - s->width;
  }
  if (s->y < a->y)
    s->y = a->y;
  if (s->y+s->height  > a->y+a->height)
  { if (s->height > a->height)
      ; /* Lassen */
    else
      s->y = a->y+a->height - s->height;
  }
}

/**
 *  @brief scale with origin in the top left corner
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Scale        ( oyRegion_s        * edit_region,
                                       float               factor )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  r->x *= factor;
  r->y *= factor;
  r->width *= factor;
  r->height *= factor;
}

/**
 *  @brief normalise swapped values for width and height
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Normalise    ( oyRegion_s        * edit_region )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  if(r->width < 0) {
    r->x += r->width;
    r->width = fabs(r->width);
  }
  if(r->height < 0) {
    r->y += r->height;
    r->height = fabs(r->height);
  }
}

/**
 *  @brief scale with origin in the top left corner
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRegion_Round        ( oyRegion_s        * edit_region )
{
  oyRegion_s * s = edit_region;
  oyRegion_s * r = s;
  
  if(!s)
    return;

  r->x = (int)OY_ROUND(r->x);
  r->y = (int)OY_ROUND(r->y);
  r->width = (int)OY_ROUND(r->width);
  r->height = (int)OY_ROUND(r->height);
}

/**
 *  @brief compare
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_IsEqual      ( oyRegion_s        * region1,
                                       oyRegion_s        * region2 )
{
  int gleich = TRUE;
  oyRegion_s * r1 = region1;
  oyRegion_s * r2 = region2;
  
  if(!r1 || !r2)
    return FALSE;

  if (r1->x != r2->x) gleich = FALSE;
  if (r1->y != r2->y) gleich = FALSE;
  if (r1->width != r2->width) gleich = FALSE;
  if (r1->height != r2->height) gleich = FALSE;
  return gleich;
}

/**
 *  @brief compare
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_IsInside     ( oyRegion_s        * region,
                                       float               x,
                                       float               y )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  int in = TRUE;
  
  if(!s)
    return FALSE;

  if (x < r->x) return FALSE;
  if (y < r->y) return FALSE;
  if (x > (r->x + r->width)) return FALSE;
  if (y > (r->y + r->height)) return FALSE;
  return in;
}

/**
 *  @brief count number of points covered by this region
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_CountPoints  ( oyRegion_s        * region )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  int w,h;
  
  if(!s)
    return FALSE;

  w = (int)OY_ROUND(r->width);
  h = (int)OY_ROUND(r->height);
  return (w) * (h);
}

/** @brief return position inside region, assuming region size
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRegion_Index        ( oyRegion_s        * region,
                                       float               x,
                                       float               y )
{
  oyRegion_s * s = region;
  oyRegion_s * r = s;
  
  if(!s)
    return FALSE;

  return (int)OY_ROUND((y - r->y) * r->width + (x - r->x));
}

/** @internal
 *  @brief debug text
 *  not so threadsafe
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyChar*        oyRegion_Show         ( oyRegion_s        * r )
{
  static oyChar *text = 0;

  if(!text)
    text = oyAllocateFunc_(sizeof(oyChar) * 512);

  if(r)
    oySprintf_(text, "%.02f+%.02f:%.02fx%.02f",r->x , r->y, r->width , r->height);
  else
    oySprintf_(text, "no region");

  return text;

}




/** @internal
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oySizeofDatatype (oyDATATYPE_e t)
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         n = 1; break;
    case oyUINT16:
    case oyHALF:
         n = 2; break;
    case oyUINT32:
    case oyFLOAT:
         n = 4; break;
    case oyDOUBLE:
         n = 8; break;
  }
  return n;
}

/** @internal
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const oyChar * oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const oyChar * text = 0;
  switch(t)
  {
    case oyUINT8:
         text = "oyUINT8"; break;
    case oyUINT16:
         text = "oyUINT16"; break;
    case oyHALF:
         text = "oyHALF"; break;
    case oyUINT32:
         text = "oyUINT32"; break;
    case oyFLOAT:
         text = "oyFLOAT"; break;
    case oyDOUBLE:
         text = "oyDOUBLE"; break;
  }
  return text;
}

/** @internal
 *  @brief describe a images channel and pixel layout
 *
 *  store some text in the images nick name as a ID
 *
 *  @since Oyranos: version 0.1.8
 *  @date 26 november 2007 (API 0.1.8)
 */
oyPixel_t*
oyCombinePixelLayout2Mask_ ( oyPixel_t     pixel_layout,
                             oyImage_s   * image,
                             oyProfile_s * profile )
{
  int n     = oyToChannels_m( pixel_layout );
  int coff_x = oyToColourOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColourChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  oyPixel_t *mask = image->oy_->allocateFunc_( sizeof(int) * (oyCHAN0 + n + 1));
  int error = !mask;
  int so = oySizeofDatatype( t );
  int w = image->width;
  int h = image->height;
  int cchan_n = 0;
  int i;
  oyChar * text = oyAllocateFunc_(512);
  oyChar * hash_text = 0;
  oyImage_s * s = image;

  if(!s)
    return 0;

  cchan_n = oyProfile_GetChannelsCount( profile );

  if(!error)
  {
    error = !memset( mask, 0, sizeof(mask) * sizeof(oyPixel_t*));
    if(oyToPlanar_m( pixel_layout ))
    {
      mask[oyPOFF_X] = 1;
      mask[oyCOFF] = w*h;
    } else {
      mask[oyPOFF_X] = n;
      mask[oyCOFF] = 1;
    }
    mask[oyPOFF_Y] = mask[oyPOFF_X] * w;
    mask[oyDATA_SIZE] = so;
    mask[oyLAYOUT] = pixel_layout;
    mask[oyCHANS] = n;

    if(swap)
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + cchan_n - i - 1;
    else
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + i;
  }


  hashTextAdd_m( "{\n" );
  oySprintf_( text, " channels:\n  all = %d\n  colour = %d\n", n, cchan_n );
  hashTextAdd_m( text );
  oySprintf_( text,
              " offsets:\n  first colour sample = %d\n  next pixel = %d\n"
              /*"  next line = %d\n"*/,
              coff_x, mask[oyPOFF_X]/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m( " swap:\n" );
    if(swap)
      hashTextAdd_m( "  colourswap = yes\n" );
    if( oyToByteswap_m( pixel_layout ) )
      hashTextAdd_m( "  byteswap = yes\n" );
  }

  if( oyToFlavor_m( pixel_layout ) )
  {
    oySprintf_( text, " flawor = yes\n" );
    hashTextAdd_m( text );
  }
  oySprintf_( text, " sample type = %s[%dByte]\n", oyDatatypeToText(t), so );
  hashTextAdd_m( text );
  hashTextAdd_m( "}\n" );

  if(!error)
    error = oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );


  oyDeAllocateFunc_(text);
  oyDeAllocateFunc_(hash_text);

  return mask;
}


/** @} */


/** \addtogroup image Image conversion API
 *  Colour conversion front end API's.

 *  @{
 */

/** @func    oyImage_GetPointContinous
 *  @brief   standard continus layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyPointer oyImage_GetPointContinous    ( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel )
{
  return &((char*)image->data)[ (point_y * image->layout_[oyPOFF_Y] +
                                 point_x * image->layout_[oyCHANS] +
                                 image->layout_[oyCHAN0+channel]) *
                                image->layout_[oyDATA_SIZE]           ]; 

}

/** @func    oyImage_GetPointPlanar
 *  @brief   standard planar layout pixel accessor
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyPointer oyImage_GetPointPlanar       ( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel )
{
  return &((char*)image->data)[ (point_y * point_x +
                                 image->layout_[oyCOFF] *  
                                 image->layout_[oyCHAN0+channel]) *
                                image->layout_[oyDATA_SIZE]           ]; 

}


/** @brief collect infos about a image 

    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    y_offset     offset between two pixel rows
    @param[in]    profile      colour space description

 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_Create         ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_IMAGE_S;
# define STRUCT_TYPE oyImage_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyImage_Copy;
  s->release = (oyStruct_ReleaseF_t) oyImage_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->width = width;
  s->height = height;
  s->data = channels;
  s->profile_ = oyProfile_Copy( profile, 0 );
  s->image_dimension = oyRegion_NewWith( s->oy_, 0, 0, s->width, s->height);

  s->layout_ = oyCombinePixelLayout2Mask_ ( pixel_layout, s, profile );

  if(s->data && s->layout_[oyCOFF] == 1)
    s->getPoint = oyImage_GetPointContinous;
  else if(s->data)
    s->getPoint = oyImage_GetPointPlanar;

  return s;
}

/** @brief collect infos about a image for showing one a display

    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    y_offset     offset between two pixel rows
    @param[in]    display_name  display name
    @param[in]    display_pos_x left image position on display
    @param[in]    display_pos_y top image position on display

 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_CreateForDisplay( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        oyPixel_t         pixel_layout,
                                        const char      * display_name,
                                        int               display_pos_x,
                                        int               display_pos_y,
                                        oyObject_s        object)
{
  oyImage_s * s = oyImage_Create( width, height, channels, pixel_layout,
                                  0, object );
  int error = !s;

  if(!error)
  {
    oyProfile_Release( &s->profile_ );
    s->profile_ = oyProfile_FromFile (0,0,0);
    if(!s->profile_)
      error = 1;

    if(!error)
    {
      if(s->layout_)
        s->oy_->deallocateFunc_(s->layout_);
      s->layout_ = oyCombinePixelLayout2Mask_ ( pixel_layout, s, s->profile_ );
    }

    s->display_pos_x = display_pos_x;
    s->display_pos_y = display_pos_y;
  }

  return s;
}

/** @brief copy a image
 *
 *  @todo  implement
 * 
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_Copy_          ( oyImage_s       * image,
                                        oyObject_s        object )
{
  oyImage_s * s = 0;

  if(!image)
    return s;

  /* TODO */

  s = image;
  s->oy_ = oyObject_Copy( s->oy_ );

  return s;
}

/** @brief copy a image
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyImage_s *    oyImage_Copy           ( oyImage_s       * image,
                                        oyObject_s        object )
{
  oyImage_s * s = 0;

  if(!image)
    return s;

  if(image && !object)
  {
    s = image;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyImage_Copy_( image, object );
  /* TODO cache */
  return s;
}

/** @brief release a image
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int            oyImage_Release        ( oyImage_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyImage_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_IMAGE_S)
  {
    WARNc_S(("Attempt to release a non oyImage_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->width = 0;
  s->height = 0;
  s->data = 0;

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->layout_)
      deallocateFunc( s->layout_ ); s->layout_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @brief set a image
 *
 *  set critical options
 *
 *  @since Oyranos: version 0.1.8
 *  @date  19 december 2007 (API 0.1.8)
 */
int            oyImage_SetCritical    ( oyImage_s       * image,
                                        oyPixel_t         pixel_layout,
                                        oyProfile_s     * profile,
                                        char            * options )
{
  oyImage_s * s = image;
  int error = !s;

  if(profile)
    s->profile_ = oyProfile_Copy( profile, 0 );

  if(pixel_layout)
  {
    if(s->layout_ && s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_(s->layout_);
    s->layout_ = oyCombinePixelLayout2Mask_ ( pixel_layout, s, s->profile_ );
  }

#if 0
  if(options)
    s->options_ = oyOptions_Copy( options );
#endif

  return error;
}


/** @func    oyFilterTypeToText
 *  @brief   oyFILTER_TYPE_e to small text
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
const char *   oyFilterTypeToText    ( oyFILTER_TYPE_e     type )
{
  const char * text = 0;

  if(type)
    switch(type) {
    case oyFILTER_TYPE_COLOUR: text = "colour"; break;
    case oyFILTER_TYPE_TONEMAP: text = "contrast or tone mapping"; break;
    case oyFILTER_TYPE_IMAGE: text = "image"; break;
    case oyFILTER_TYPE_GENERIC: text = "generic"; break;
    default: text = "unknown"; break;
    }

  return text;
}

/** @func    oyFilter_New_
 *  @brief   allocate and initialise a new filter object
 *
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
oyFilter_s * oyFilter_New_           ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_FILTER_S;
# define STRUCT_TYPE oyFilter_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyFilter_Copy;
  s->release = (oyStruct_ReleaseF_t) oyFilter_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

static uint32_t filter_ids = 0;

/** @func    oyFilter_New
 *  @brief   lookup and initialise a new filter object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
oyFilter_s * oyFilter_New            ( oyFILTER_TYPE_e     filter_type,
                                       const char        * category,
                                       const char        * cmm_required,
                                       oyObject_s          object )
{
  oyFilter_s * s = oyFilter_New_( object );
  int error = !s;
  uint32_t ret = 0;
  oyCMMapiQueries_s * capabilities = 0;
  char cmm_used[] = {0,0,0,0,0};
  oyCMMapi_s * cmm_api = oyCMMsGetApi_(oyOBJECT_TYPE_CMM_API4_S,
                                       cmm_required,
                                       capabilities,
                                       cmm_used,
                                       category,
                                       filter_type );
  oyCMMapi4_s * cmm_api4 = 0;
  oyAllocFunc_t allocateFunc_ = 0;

  if(!error)
    allocateFunc_ = s->oy_->allocateFunc_;

  error = !(cmm_api && cmm_api->type == oyOBJECT_TYPE_CMM_API4_S);

  if(!error)
  {
    cmm_api4 = (oyCMMapi4_s *) cmm_api;

    s->id_ = filter_ids++;
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);
    s->name_ = oyName_copy( &cmm_api4->name, s->oy_ );
    memcpy( s->cmm_, cmm_used, 4 ); s->cmm_[4] = 0;

    s->filter_type_ = filter_type;
    s->category_ = oyStringCopy_( cmm_api4->category, allocateFunc_ );

    s->options_ = cmm_api4->oyOptions_Get( 0, &ret );
    error = ret;
    s->opts_ui_ = oyStringCopy_( cmm_api4->opts_ui, allocateFunc_ );
  }

  if(error)
    s = 0;

  return s;
}

/** @func    oyFilter_Copy_
 *  @brief   real copy a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
oyFilter_s * oyFilter_Copy_          ( oyFilter_s        * filter,
                                       oyObject_s          object )
{
  oyFilter_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc_ = 0;

  if(!filter || !object)
    return s;

  s = oyFilter_New_( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->registration_ = oyStringCopy_( filter->registration_, allocateFunc_ );
    s->name_ = oyName_copy( filter->name_, s->oy_ );
    error = !memcpy( s->cmm_, filter->cmm_, 8 );
    s->filter_type_ = filter->filter_type_;
    s->category_ = oyStringCopy_( filter->category_, allocateFunc_ );
#if 0
    s->options_ = oyOptions_Copy( filter->options_, s->oy_ );
#endif
    s->opts_ui_ = oyStringCopy_( filter->opts_ui_, allocateFunc_ );
    s->image_ = oyImage_Copy_( filter->image_, s->oy_ );
    s->profiles_ = oyProfileList_Copy( filter->profiles_, s->oy_ );
    s->parents_ = s->children_ = 0;
    s->merged_to_ = 0;
    s->data_ = oyStructList_Copy( filter->data_, s->oy_ );
  }

  return s;
}

/** @func    oyFilter_Copy
 *  @brief   copy or reference a filter object
 *
 *  @param[in]     filter              filter object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
oyFilter_s * oyFilter_Copy           ( oyFilter_s        * filter,
                                       oyObject_s          object )
{
  oyFilter_s * s = 0;

  if(!filter)
    return s;

  if(filter && !object)
  {
    s = filter;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilter_Copy_( filter, object );

  return s;
}
/** @func    oyFilter_Release
 *  @brief   release and zero a filter object
 *
 *  @todo    complete the implementation
 *
 *  @param[in,out] obj                 filter object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
int          oyFilter_Release        ( oyFilter_s       ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilter_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_FILTER_S)
  {
    WARNc_S(("Attempt to release a non oyFilter_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  s->id_ = 0;
  s->registration_ = 0;

  oyProfileList_Release( &s->profiles_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->category_)
      deallocateFunc( s->category_ ); s->category_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


const char * oyFilter_NameGet        ( oyFilter_s        * filter,
                                       oyNAME_e            name_type )
{
  if(!filter)
    return 0;

  return oyObject_GetName(filter->oy_, name_type);
}
const char * oyFilter_CategoryGet    ( oyFilter_s        * filter,
                                       int                 nontranslated )
{
  if(!filter)
    return 0;

  return filter->category_;
}
#define oyFILTER_SET_TEST              0x01        /** only test */
#define oyFILTER_GET_DEFAULT           0x01        /** defaults */
/* decode */
#define oyToFilterSetTest_m(r)         ((r)&1)
#define oyToFilterGetDefaults_m(r)     ((r)&1)
oyOptions_s* oyFilter_OptionsSet     ( oyFilter_s        * filter,
                                       oyOptions_s       * options,
                                       int                 flags );
oyOptions_s* oyFilter_OptionsGet     ( oyFilter_s        * filter,
                                       int                 flags )
{
  if(!filter)
    return 0;

#if 0
  if(oyToFilterGetDefaults_m(flags))
    return oyOptions_Copy( filter->cmm_api4->options, filter->oy_ );
  else
    return oyOptions_Copy( filter->options_, 0 );
#endif
  return 0;
}
const char * oyFilter_WidgetsSet     ( oyFilter_s        * filter,
                                       const char        * widgets,
                                       int                 flags );
const char * oyFilter_WidgetsGet     ( oyFilter_s        * filter,
                                       int                 flags );
oyProfileList_s* oyFilter_ProfilesSet( oyFilter_s        * filter,
                                       oyProfileList_s   * profiles,
                                       int                 flags );
oyProfileList_s* oyFilter_ProfilesGet( oyFilter_s        * filter,
                                       int                 flags );
int          oyFilter_FilterSet      ( oyFilter_s        * filter,
                                       oyFilters_s       * parents,
                                       oyFilters_s       * children,
                                       int                 flags );
int          oyFilter_FilterGet      ( oyFilter_s        * filter,
                                       oyFilters_s      ** parents,
                                       oyFilters_s      ** cildren );
int          oyFilter_ImageSet       ( oyFilter_s        * filter,
                                       oyImage_s         * image )
{
  filter->image_ = oyImage_Copy( image, 0 );
  return !filter->image_;
}
oyImage_s *  oyFilter_ImageGet       ( oyFilter_s        * filter );



/** @brief create and possibly precalculate a transform for a given image

 *  @param[in]  cmm    zero or a cmm
 *  @param[in]  list   multi profiles, images should have already one profile
 *  @param[in]  opts   conversion opts
 *  @param[in]  in     input image
 *  @param[in]  out    output image
 *  @return            conversion
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Create (
                                        oyProfileList_s * list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        object)
{
  oyColourConversion_s * s = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CMMS);

  s = oyColourConversion_Create_(list, opts, in, out, object);

  oyExportEnd_();
  DBG_PROG_ENDE
  return s;
}

/** @func    oyConcatenateImageProfiles_
 *  @brief   oyCMMColourConversion_ToMem_t implementation
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2007/06/26
 */
oyProfileList_s * oyConcatenateImageProfiles_ (
                                        oyProfileList_s * list,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        obj )
{
  int error = 0;
  oyProfileList_s * p_list = 0;

  if(!error)
  {
    int i, n;

    /* collect profiles */
    if(!error)
    {
      int p_list_n = 0;
      oyProfile_s * tmp = 0;

      if(obj)
        p_list = oyProfileList_New( obj );
      else
        p_list = oyProfileList_New( 0 );
      error = !p_list;

      if(!error)
      {
        tmp = oyProfile_Copy( in->profile_, 0);
        p_list = oyProfileList_MoveIn( p_list, &tmp, 0 );
        error = !p_list;
      }

      p_list_n = oyProfileList_Count( p_list );

      if(!error && list && oyProfileList_Count(list))
      {
        n = oyProfileList_Count(list);
        for(i = 0; i < n; ++i)
        {
          tmp = oyProfileList_Get( list,i );
          p_list = oyProfileList_MoveIn( p_list, &tmp, i + p_list_n);
          error = !p_list;
        }
      }

      if(!error)
      {
        tmp = oyProfile_Copy(out->profile_, 0);
        p_list = oyProfileList_MoveIn( p_list, &tmp, p_list_n);
        error = !p_list;
      }
    }
  }
  
  return p_list;
}


/** @internal 
 *             precalculate a transform for a given image by the CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  24 november 2007 (API 0.1.8)
 */
oyCMMptr_s *       oyColourConversion_CallCMM_ (
                                        const char      * cmm,
                                        oyColourConversion_s * s,
                                        oyProfileList_s * list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyProfileTag_s ** psid,
                                        oyObject_s        obj)
{
  oyCMMptr_s * cmm_ptr = 0;
  oyCMMColourConversion_Create_t funcP = 0;
  int error = !s;
  char cmm_used[] = {0,0,0,0,0};

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API1_S,
                                      cmm, 0, cmm_used, 0,0 );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_Create;
    }
    error = !funcP;
  }

  if(!error)
  {
    oyProfileList_s * p_list = 0;
    int i, n;

    if(obj)
      cmm_ptr = oyCMMptr_New_(obj->allocateFunc_);
    else
      cmm_ptr = oyCMMptr_New_(s->oy_->allocateFunc_);
    error = !cmm_ptr;

    if(!error)
      error = oyCMMptr_Set_( cmm_ptr, cmm_used,0, oyCMM_COLOUR_CONVERSION,0,0 );

    /* collect profiles */
    if(!error)
    {
      p_list = oyConcatenateImageProfiles_( list, in, out, obj ? obj : s->oy_ );

      error = !p_list;
    }

    if(!error)
    {
      int intent = oyGetBehaviour( oyBEHAVIOUR_RENDERING_INTENT );
      oyCMMptr_s ** p = oyProfileList_GetCMMptrs_( p_list, cmm_used );
      int layout_in = in->layout_[oyLAYOUT];
      int layout_out = out->layout_[oyLAYOUT];

      n = oyProfileList_Count(p_list);
      
      error = funcP( p, n, layout_in, layout_out, intent, 0,0, cmm_ptr );

      for(i = 0; i < n; ++i)
        if(!error)
          error = oyCMMptr_Release_(&p[i]);
      p_list->oy_->deallocateFunc_(p);

      oyCMMdsoRelease_( cmm_used );

      if(psid)
        *psid = oyProfileTag_Create( p_list->list_,
                     icSigProfileSequenceIdentifierType, 0, OY_MODULE_NICK, 0 );

      oyProfileList_Release( &p_list );
    }
  }

  return cmm_ptr;
}

/** @internal describe a transform uniquely
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const oyChar *     oyColourConversion_GetID_ (
                                       oyColourConversion_s * s,
                                       oyProfileList_s * list,
                                       oyOptions_s     * opts,
                                       oyImage_s       * in,
                                       oyImage_s       * out)
{
  int error = !s;
  int i, n;

  oyChar * hash_text = 0;

  if(!error)
  {
    oyChar text[16];
    int intent = oyGetBehaviour( oyBEHAVIOUR_RENDERING_INTENT );

    hashTextAdd_m( "oyCC\n" );
    hashTextAdd_m( "profile in = \"" );
    hashTextAdd_m( oyProfile_GetID( in->profile_ ) );

    hashTextAdd_m( "\"\nimage in = " );
    hashTextAdd_m( oyObject_GetName( in->oy_, oyNAME_NICK ) );

    n = oyProfileList_Count( list );
    for(i = 0; i < n; ++i)
    {
      oyProfile_s * p = oyProfileList_Get( list, i );
      if(i == 0)
        hashTextAdd_m( "  effect = \"" );
      hashTextAdd_m( oyProfile_GetID( p ) );
      oyProfile_Release( &p );
      hashTextAdd_m( "\"\n" );
    }

    hashTextAdd_m( "profile out = \"" );
    hashTextAdd_m( oyProfile_GetID( out->profile_ ) );

    hashTextAdd_m( "\"\nintent = " );
    oySprintf_( text, "%d\n", intent );
    hashTextAdd_m( text );

    hashTextAdd_m( "image out = " );
    hashTextAdd_m( oyObject_GetName( out->oy_, oyNAME_NICK ) );

    oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );
    hash_text = 0;
  }

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  return hash_text;
}


/** @internal create and possibly precalculate a transform for a given image

 *  @param[in]  cmm    zero or a cmm
 *  @param[in]  list   multi profiles, images should have already one profile
 *  @param[in]  opts   conversion opts
 *  @param[in]  in     input image
 *  @param[in]  out    output image
 *  @return            conversion
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Create_ (
                                        oyProfileList_s * list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s        object)
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_COLOUR_CONVERSION_S;
# define STRUCT_TYPE oyColourConversion_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyColourConversion_Copy;
  s->release = (oyStruct_ReleaseF_t) oyColourConversion_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
  {
    oyCMMptr_s *cmm_ptr = 0;
    const char *cmm = oyModuleGetActual( type );
    const oyChar * tmp = 0;
 
    oyHash_s * entry = 0;
    oyChar * hash_text = 0;

    /*  Cache Search
     *  1.     hash from input
     *  2.     query for hash in cache
     *  3.     check
     *  3a.       eighter take cache entry
     *  3b.       or ask CMM
     *  3b.1.                update cache entry
     */

    /* 1. create hash text */
    hashTextAdd_m( cmm );
    tmp = oyColourConversion_GetID_( s, list, opts, in, out );
    hashTextAdd_m( tmp );

    /* 2. query in cache */
    entry = oyCMMCacheListGetEntry_( hash_text );
    /*WARNc_S(("%s\n", hash_text);)*/
    if(s->oy_->deallocateFunc_)
      s->oy_->deallocateFunc_( hash_text );

    if(!error)
      s->cmms_ = oyStructList_New( s->oy_ );

    if(!error)
    {
      /* 3. check and 3.a take*/
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( entry,
                                                  oyOBJECT_TYPE_CMM_POINTER_S);

      if(!cmm_ptr)
      {
        /* 3b. ask CMM */
        cmm_ptr = oyColourConversion_CallCMM_( cmm, s, list, opts, in, out,
                                               0, entry ? entry->oy_ : 0);
        error = !cmm_ptr;

        /* 3b.1. update cache entry */
        error = oyHash_SetPointer_( entry,
                                   (oyStruct_s*) oyCMMptr_Copy_(cmm_ptr, 0) );
      }

      if(!error && cmm_ptr && cmm_ptr->ptr)
      {
        oyHash_s * c = oyHash_Copy_(entry, 0);
        error = oyStructList_MoveIn( s->cmms_, (oyStruct_s**) &c, -1 );
      }

      oyCMMptr_Release_( &cmm_ptr );

      if(!error)
      {
        s->image_in_ = oyImage_Copy( in, 0 );
        s->image_out_ = oyImage_Copy( out, 0 );
      }

#if 0
    {
      const char * fn = 0;
      size_t size = 0;
      oyPointer mem = 0;
      /*oyStruct_s * h = 0;
      oyCMMptr_s * cmm_ptr_orig = 0;*/

      /*if(in)
        fn = in->profile_->file_name_;*/
      if(!fn) fn = "oy_dbg_in.icc";
      oyProfile_ToFile_(in->profile_, fn);

      mem = oyColourConversion_ToMem_( s, &size, oyAllocateFunc_ );
      error = oyWriteMemToFile_( "oy_dbg_dl.icc", mem, size );
      if(mem)
        oyDeAllocateFunc_(mem); mem = 0;

      fn = 0;
      /*if(out && out->profile_)
        fn = out->profile_->file_name_;*/
      if(!fn) fn = "oy_dbg_out.icc";
      oyProfile_ToFile_(in->profile_, fn);

#if 0
        cmm_ptr = oyColourConversion_CallCMM_( cmm, s, list, opts, in, out,
                                            &psid, entry ? entry->oy_ : 0);
        error = !cmm_ptr;
        h = oyStructList_Get_( s->cmms_, 0 );

      oyCMMptr_Release_( &cmm_ptr );
#endif
      /*cmm_ptr_orig = (oyCMMptr_s*) oyHash_GetPointer_(entry, oyOBJECT_TYPE_CMM_POINTER_S);

      cmm_ptr_orig->ptr = cmm_ptr->ptr;*/
    }
#endif
    }

    oyHash_Release_( &entry );
  }

  return s;
}

/** @func oyColourConversion_Copy
 *  @brief copy the struct
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
oyColourConversion_s* oyColourConversion_Copy (
                                       oyColourConversion_s * obj,
                                       oyObject_s          object )
{
  oyColourConversion_s * s = 0;
  int error = !obj;

  if(!error && obj->type_ != oyOBJECT_TYPE_COLOUR_CONVERSION_S)
    error = 1;

  if(!error && obj->oy_)
  {
    oyObject_Ref( obj->oy_ );
    s = obj;
  }

  return s;
}

/** @internal
 *  @brief oyCMMProgress_t
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void             oyCMMProgress_      ( int                 ID,
                                       double              progress )
{
  printf("%s:%d id: %d %0.02f \n",__FILE__,__LINE__, ID, progress);
}


/** @internal
 *  @brief run
 *
 *  @todo TODO: What to do when the CMM changes?
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int        oyColourConversion_Run    ( oyColourConversion_s * s )
{
  int error = !s;
  oyHash_s   * hash = 0;
  oyCMMptr_s * cmm_ptr = 0;
  int pos = 0;
  oyCMMColourConversion_Run_t funcP = 0;
  char cmm_used[] = {0,0,0,0,0},
       cc_cmm[] = {0,0,0,0,0};

  if(!error)
  {
    hash = (oyHash_s*) oyStructList_GetRefType( s->cmms_, pos,
                                                 oyOBJECT_TYPE_HASH_S);
    error = !hash;

    if(!error)
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( hash,
                                                  oyOBJECT_TYPE_CMM_POINTER_S);

    error = !cmm_ptr;

    oyHash_Release_( &hash );
  }

  if(!error)
    error = !memcpy( cc_cmm, cmm_ptr->cmm, 4 );

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API1_S,
                                      cc_cmm, 0, cmm_used, 0,0 );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_Run;
    }
    error = !funcP;
  }

  if(!error)
  {
    if(!error)
    {
      oyPointer in = 0, out = 0;
      int count = 0;

      if(s->image_in_ && s->image_in_->data)
      {
        in = s->image_in_->data;


        if(s->image_in_->regions)
        {
          WARNc_S(("TODO regions handling"));

        } else {

          /*intptr_t off_x = s->image_in_->layout_[oyPOFF_X],
                   sample_size = s->image_in_->layout_[oyDATA_SIZE];

          in += (intptr_t) oyRegion_Index( s->image_in_->region, 0,0 ) *
                           off_x * sample_size;*/

          count = s->image_in_->width * s->image_in_->height;

        }

      } else
        error = 1;

      if(!error)
      {
        if(s->image_out_)
        {
          if(s->image_out_->data)
            out = s->image_out_->data;
          else
            error = 1;
        } else
          out = s->image_in_->data;
      }

      if(!error)
      {
        oyCMMProgress_t progress = oyCMMProgress_;

        if(!error)
          error = funcP( cmm_ptr, in, out, count, progress );

        oyCMMdsoRelease_( cmm_ptr->cmm );
      }
    }
  }

  return error;
}

/** @func    oyColourConversion_Release
 *  @brief   release a oyColourConversion_Release
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/01/01
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 */
int        oyColourConversion_Release( oyColourConversion_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyColourConversion_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_COLOUR_CONVERSION_S)
  {
    WARNc_S(("Attempt to release a non oyColourConversion_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyImage_Release( &s->image_in_ );
  oyImage_Release( &s->image_out_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->profiles_)
      deallocateFunc( s->profiles_ ); s->profiles_ = 0;

    if(s->cmms_)
      oyStructList_Release( &s->cmms_ );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @func    oyColourConversion_ToMem_ 
 *  @internal
 *  @brief   save the profile linking data as a ICC device link to memory
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/21 (Oyranos: 0.1.8)
 *  @date    2008/03/11
 */
oyPointer    oyColourConversion_ToMem_( oyColourConversion_s * s,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc )
{
  int error = !s;
  oyHash_s   * hash = 0;
  oyCMMptr_s * cmm_ptr = 0;
  int pos = 0;
  oyCMMColourConversion_ToMem_t funcP = 0;
  char cmm_used[] = {0,0,0,0,0},
       cc_cmm[] = {0,0,0,0,0};
  oyPointer block = 0;
  size_t size_ = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!error)
  {
    hash = (oyHash_s*) oyStructList_GetRefType( s->cmms_, pos,
                                                 oyOBJECT_TYPE_HASH_S );
    error = !hash;

    if(!error)
      cmm_ptr = (oyCMMptr_s*) oyHash_GetPointer_( hash,
                                                  oyOBJECT_TYPE_CMM_POINTER_S);

    error = !cmm_ptr;

    oyHash_Release_( &hash );
  }

  if(!error)
    error = !memcpy( cc_cmm, cmm_ptr->cmm, 4 );

  if(!error)
  {
    oyCMMapi_s * api = oyCMMsGetApi_( oyOBJECT_TYPE_CMM_API1_S,
                                      cc_cmm, 0, cmm_used, 0,0 );
    if(api && *(uint32_t*)&cmm_used)
    {
      oyCMMapi1_s * api1 = (oyCMMapi1_s*) api;
      funcP = api1->oyCMMColourConversion_ToMem;
    }
    error = !funcP;
  }

  if(!error)
  {
    oyProfile_s * prof = 0;
    /*int n = 0;*/
    oyProfileTag_s * psid = 0,
                   * info = 0,
                   * cprt = 0;

    if(!error)
    {
      if(!error)
        block = funcP( cmm_ptr, &size_, oyAllocateFunc_ );
      error = !block;

      oyCMMdsoRelease_( cmm_ptr->cmm );
    }

    if(!error && size)
    {
      *size = size_;

      prof = oyProfile_FromMem( size_, block, 0, 0 );
      psid = oyProfile_GetTagById( prof, icSigProfileSequenceIdentifierTag );

      /* icSigProfileSequenceIdentifierType */
      if(!psid)
      {
        oyProfileList_s * p_list = 0;

        p_list = oyConcatenateImageProfiles_( s->profiles_,
                            s->image_in_, s->image_out_, s->oy_ );

        psid = oyProfileTag_Create( p_list->list_,
                     icSigProfileSequenceIdentifierType, 0, OY_MODULE_NICK, 0 );

        oyProfileList_Release( &p_list );

        if(psid)
          error = oyProfile_AddTag ( prof, &psid, -1 );
      }

      /* Info tag */
      if(!error)
      {
        oyStructList_s * list = 0;
        char h[5] = {"Info"};
        uint32_t * hi = (uint32_t*)&h;
        const char * cc_name = oyObject_GetName( s->oy_, oyNAME_NICK );
        oyName_s * name = oyName_new(0);

        name = oyName_set_ ( name, cc_name, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        name = oyName_set_ ( name, cc_cmm, oyNAME_NICK,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list,  (oyStruct_s**) &name, 0 );

        if(!error)
        {
          info = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !info;
        }

        if(!error)
          info->use = (icTagSignature)oyValueUInt32(*hi);

        oyStructList_Release( &list );

        if(info)
          error = oyProfile_AddTag ( prof, &info, -1 );
      }

      if(!error)
        cprt = oyProfile_GetTagById( prof, icSigCopyrightTag );

      /* icSigCopyrightTag */
      if(!error && !cprt)
      {
        oyStructList_s * list = 0;
        const char * c_text = "no copyright; use freely";
        oyName_s * name = oyName_new(0);

        name = oyName_set_ ( name, c_text, oyNAME_NAME,
                             oyAllocateFunc_, oyDeAllocateFunc_ );
        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list,  (oyStruct_s**) &name, 0 );

        if(!error)
        {
          cprt = oyProfileTag_Create( list, icSigTextType, 0,OY_MODULE_NICK, 0);
          error = !cprt;
        }

        if(!error)
          cprt->use = icSigCopyrightTag;

        oyStructList_Release( &list );

        if(cprt)
          error = oyProfile_AddTag ( prof, &cprt, -1 );
      }

      if(block)
        oyDeAllocateFunc_( block ); block = 0; size_ = 0;

      block = oyProfile_TagsToMem_( prof, &size_, allocateFunc );

      *size = size_;
      oyProfile_Release( &prof );
    }
  }

  return block;
}

/** @func    oyColourConversion_ToProfile
 *  @brief   convert a oyColourConversion_Release to a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 */
oyProfile_s* oyColourConversion_ToProfile ( oyColourConversion_s * cc )
{
  oyProfile_s * s = 0;
  int error = !cc;

  if(!error)
  {
    oyPointer block = 0;
    size_t size = 0;

    block = oyColourConversion_ToMem_( cc, &size, 0 );

    s = oyProfile_FromMem( size, block, 0, 0 );

    if(block && size)
      oyDeAllocateFunc_(block);
  }

  return s;
}


/** @func    oyConversions_New_
 *  @brief   allocate and initialise a new oyConversions_s object
 *
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
oyConversions_s *  oyConversions_New_( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_CONVERSIONS_S;
# define STRUCT_TYPE oyConversions_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyConversions_Copy;
  s->release = (oyStruct_ReleaseF_t) oyConversions_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @func    oyConversion_CreateBasic
 *  @brief   allocate initialise a basic oyConversions_s object
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/26
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 */
oyConversions_s  * oyConversions_CreateBasic (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConversions_s * s = oyConversions_New_( object );
  int error = !s;

  if(!error)
  {
    s->input = oyFilter_New( oyFILTER_TYPE_COLOUR, "image", 0, 0 );
    
    error = oyFilter_ImageSet ( s->input, input );
    //TODO
  }

  if(error)
    s = 0;

  return s;
}
oyConversions_s  * oyConversions_CreateInput (
                                       oyImage_s         * input,
                                       oyObject_s          object );
/** @func    oyConversions_Copy_
 *  @brief   real copy a oyConversions_s object
 *
 *  @param[in]     conversions         conversions object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyConversions_s * oyConversions_Copy_( oyConversions_s   * conversions,
                                       oyObject_s          object )
{
  oyConversions_s * s = 0;
  int error = 0;
  oyAllocFunc_t allocateFunc_ = 0;

  if(!conversions || !object)
    return s;

  s = oyConversions_New_( object );
  error = !s;
  allocateFunc_ = s->oy_->allocateFunc_;

  if(!error)
  {
    s->input = oyFilter_Copy( conversions->input, object );
  }

  return s;
}

/** @func    oyConversions_Copy
 *  @brief   copy or reference a oyConversions_s object
 *
 *  @param[in]     conversions         conversions object
 *  @param         object              the obligate object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2008/06/26
 */
oyConversions_s * oyConversions_Copy ( oyConversions_s   * conversions,
                                       oyObject_s          object )
{
  oyConversions_s * s = 0;

  if(!conversions)
    return s;

  if(conversions && !object)
  {
    s = conversions;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConversions_Copy_( conversions, object );

  return s;
}
/** @func    oyConversions_Release
 *  @brief   release and zero a conversions object
 *
 *  @todo    complete the implementation
 *
 *  @param[in,out] obj                 conversions object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/25
 */
int          oyConversions_Release   ( oyConversions_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConversions_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_CONVERSIONS_S)
  {
    WARNc_S(("Attempt to release a non oyConversions_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilter_Release( &s->input );
  oyFilter_Release( &s->out_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

oyConversions_s  * oyConversions_FilterAdd (
                                       oyFilter_s        * filter );
oyConversions_s  * oyConversions_OutputAdd (
                                       oyImage_s         * input );
int              * oyConversions_Run ( oyConversions_s   * conversion,
                                       uint32_t            feedback );
oyProfile_s      * oyConversions_ToProfile (
                                       oyConversions_s   * conversion );



/** @} */


/** \addtogroup single_colour Named Colour API
 *  Named Colour API's.

 *  @{
 */


/** @brief manage complex oyNamedColour_s inside Oyranos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  @param[in]  chan        pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]  sig         ICC colour space signature http://www.color.org
 *  @param[in]  channels_n  elements of channels including alpha ...
 *  @param[in]  names_chan  needed if sig provides no glue about the channel colour or you use more than one extra (non colour) channel. The first deteted extra channel is by default considered alpha.
 *  @param[in]  name        of the colour
 *  @param[in]  name_long   description
 *  @param[in]  nick_name   very short, only a few letters
 *  @param[in]  blob        CGATS or other reference data
 *  @param[in]  blob_len    length of the data blob
 *  @param[in]  ref_file    possibly a ICC profile, allowed abreviations are 'sRGB' 'XYZ' 'Lab'(D50)
 *  @param[in]  allocateFunc   user controled memory allocation
 *  @param[in]  deallocateFunc user controled memory deallocation
 * 
 */
oyNamedColour_s*
oyNamedColour_Create( const double      * chan,
                      const char        * blob,
                      int                 blob_len,
                      oyProfile_s       * ref,
                      oyObject_s          object )
{
  int n = 0;
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_NAMED_COLOUR_S;
# define STRUCT_TYPE oyNamedColour_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyNamedColour_Copy;
  s->release = (oyStruct_ReleaseF_t) oyNamedColour_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
    s->profile_  = oyProfile_Copy( ref, 0 );

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
  oyCopyColour( chan, &s->channels_[0], 1, ref, n );
  oyCopyColour( 0, &s->XYZ_[0], 1, 0, 0 );

  if(!error && blob && blob_len)
  {
    s->blob_ = s->oy_->allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(!error)
      error = !memcpy( s->blob_, blob, blob_len );

    if(!error)
      s->blob_len_ = blob_len;
  }

  return s;
}

/** @func oyNamedColour_CreateWithName
 *  @brief create a oyNamedColour_s struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 *
 *  @param[in]  nick        very short, only a few letters
 *  @param[in]  name        of the colour
 *  @param[in]  description description
 *  @param[in]  chan        pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]  XYZ         reference values
 *  @param[in]  blob        CGATS or other reference data
 *  @param[in]  blob_len    length of the data blob
 *  @param[in]  profile_ref profile
 *  @param[in]  object      memory management
 *  @return                 a oyNamedColour_s
 * 
 */
oyNamedColour_s*  oyNamedColour_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object )
{
  int error = 0;
  oyNamedColour_s * s = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );

  error = !s_obj;

  if(!error)
    error = oyObject_SetNames( s_obj, nick, name, description );

  s = oyNamedColour_Create( chan, blob, blob_len, profile_ref, s_obj );
  error =!s;

  oyObject_Release( &s_obj );

  if(!error && XYZ)
    oyCopyColour( XYZ, s->XYZ_, 1, 0, 0 );

  return s;
}

/** @brief copy
 *
 *  
 *
 *  @param[in] colour    address of Oyranos colour struct
 *  @return              copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyNamedColour_s*
oyNamedColour_Copy ( oyNamedColour_s   * colour,
                     oyObject_s          object )
{
  oyNamedColour_s * c = NULL,
                  * s = colour;

  if(!colour)
    return c;

  if(!object)
  {
    oyObject_Ref( s->oy_ );
    return s;
  }

  c = oyNamedColour_Create(
                     colour->channels_,
                     colour->blob_,
                     colour->blob_len_,
                     colour->profile_,
                     object );

  return c;
}

/** @brief release correctly
 *
 *  set pointer to zero
 *
 *  @param[in]    address of Oyranos colour struct pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int 
oyNamedColour_Release( oyNamedColour_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyNamedColour_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_NAMED_COLOUR_S)
  {
    WARNc_S(("Attempt to release a non oyNamedColour_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(s->channels_)
      deallocateFunc( s->channels_ ); s->channels_ = 0;

    if(s->blob_) /* s->blob_len */
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/*  @return                 pointer to D50 Lab doubles with L 0.0...1.0 a/b -1.27...1.27 */

/** @func  oyNamedColour_GetSpaceRef
 *  @brief get a colour space reference
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
oyProfile_s *     oyNamedColour_GetSpaceRef ( oyNamedColour_s  * colour )
{
  if(!colour)
    return 0;

  return oyProfile_Copy( colour->profile_, 0 );
}


/** @func  oyNamedColour_SetChannels
 *  @brief set colour channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @param[in]  channels    pointer to channel data
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
void
oyNamedColour_SetChannels( oyNamedColour_s  * colour,
                           const double     * channels,
                                       uint32_t            flags )
{
  int i, n;
  if(!colour)
    return;

  n = oyProfile_GetChannelsCount( colour->profile_ );
  if(channels)
    for(i = 0; i < n; ++i)
      colour->channels_[i] = channels[i];
}

/** @func  oyColour_Convert
 *  @internal
 *  @brief convert one colour
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyColourConversion_s * conv = NULL;
  int error = 0;

  in    = oyImage_Create( 1,1, 
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( 1,1, 
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyColourConversion_Create( 0,0, in,out, 0 );
  error  = oyColourConversion_Run( conv );

  oyColourConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );

  return error;
}

/** @func  oyNamedColour_GetColour
 *  @brief convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int          oyNamedColour_GetColour ( oyNamedColour_s   * colour,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags )
{
  int error = !colour || !profile || !buf || !buf_type;
  oyProfile_s * p_in = colour->profile_;

  /* XYZ has priority */
  if(!error &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    p_in = oyProfile_FromStd ( oyEDITING_XYZ, NULL );
    if(!profile)
      return 1;
 
    error = oyColourConvert_( p_in, profile,
                              colour->XYZ_, buf,
                              oyDOUBLE, buf_type);

    oyProfile_Release ( &p_in );

  } else if(!error)
    error = oyColourConvert_( p_in, profile,
                              colour->channels_, buf,
                              oyDOUBLE, buf_type);

  return error;
}

/** @func  oyNamedColour_SetColourStd
 *  @brief set colour channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @param[in]  colour_space Oyranos standard colour space
 *  @param[in]  channels    pointer to channel data
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags )
{
  oyNamedColour_s * s = colour;
  int error = !s || !colour_space || !channels;
  oyProfile_s * p_in = 0;

  /* abreviate */
  if(!error && channels_type == oyDOUBLE)
  {
    if     (colour_space == oyEDITING_LAB)
    {
      oyLab2XYZ( (double*)channels, s->XYZ_ );
      return error;

    }
    else if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( (double*)channels, s->XYZ_, 1, 0, 0 );
      return error;
    }
  }

  if(!error)
  {
    p_in = oyProfile_FromStd ( colour_space, NULL );
    error = !p_in;
  }

  /* reset and allocate */
  if(!error)
  {
    int n = oyProfile_GetChannelsCount( p_in );

    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    if(n > oyProfile_GetChannelsCount( s->profile_ ))
    {
      if(s->channels_)
        deallocateFunc(s->channels_); s->channels_ = 0;

      s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
    }

    error = !memset( s->channels_, 0, sizeof(double) * n );

    s->XYZ_[0] = s->XYZ_[1] = s->XYZ_[2] = -1;

    if(deallocateFunc && s->blob_)
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;
  }

  /* convert */
  if(!error)
  {
    oyProfile_s * p_out = s->profile_;
    oyColourConvert_( p_in, p_out,
                      channels, s->channels_,
                      channels_type , oyDOUBLE );
    oyProfile_Release ( &p_out );

    p_out = oyProfile_FromStd( oyEDITING_XYZ, 0 );
    oyColourConvert_( p_in, p_out,
                      channels, s->XYZ_,
                      channels_type , oyDOUBLE );
    oyProfile_Release ( &p_out );
  }

  return error;
}

/** @func  oyNamedColour_GetChannelsConst
 *  @brief get colour channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @return                 pointer channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const double *    oyNamedColour_GetChannelsConst ( oyNamedColour_s * colour,
                                       uint32_t          * flags )
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->channels_;
  else
    return l;
}

/** @func  oyNamedColour_GetiXYZConst
 *  @brief get XYZ channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @return                 pointer XYZ channels
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/18
 *  @since   2008/02/18 (Oyranos: 0.1.8)
 */
const double *    oyNamedColour_GetXYZConst      ( oyNamedColour_s * colour)
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->XYZ_;
  else
    return l;
}

/** @func  oyNamedColour_GetColourStd
 *  @brief convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_GetColourStd     ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags )
{                        
  int ret = 0;
  oyProfile_s * profile;

  if(!colour)
    return 1;

  /* abreviate */
  if(buf_type == oyDOUBLE &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    if(colour_space == oyEDITING_LAB)
    {
      oyXYZ2Lab( colour->XYZ_, (double*)buf );
      return 0;
    }

    if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( colour->XYZ_, (double*)buf, 1, 0, 0 );
      return 0;
    }
  }

  profile = oyProfile_FromStd ( colour_space, NULL );
  if(!profile)
    return 1;

  ret = oyNamedColour_GetColour ( colour, profile, buf, buf_type, 0 );
  oyProfile_Release ( &profile );

  return ret;
}

/** @func oyNamedColour_GetName
 *  @brief get colour channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
const oyChar *    oyNamedColour_GetName( oyNamedColour_s * s,
                                       oyNAME_e            type,
                                       uint32_t            flags )
{
  const oyChar * text = 0;
  if(!s)
    return 0;

  if(!s->oy_)
    return 0;

  text = oyObject_GetName( s->oy_, type );

  if(!text && flags)
  {
    const oyChar * tmp = 0;
    oyChar * txt = 0;
    double l[3];
    int i;
    icSignature sig = oyProfile_GetSignature( s->profile_,
                                              oySIGNATURE_COLOUR_SPACE );

    oyAllocHelper_m_( txt, oyChar, 1024, 0, return 0 );
    oyNamedColour_GetColourStd( s, oyEDITING_LAB, l, oyDOUBLE, 0 );

    switch(type)
    {
      case oyNAME_DESCRIPTION:
           tmp = oyObject_GetName( s->oy_, oyNAME_NAME );
           if(!tmp)
             tmp = oyObject_GetName( s->oy_, oyNAME_NICK );

           if(tmp)
             oySprintf_(txt, "%s: CIE*Lab: ", tmp );
           else
             oySprintf_(txt, "CIE*Lab: ");
           for(i = 0; i < 3; ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", l[i] );

           tmp = oyICCColourSpaceGetName( sig );
           if(tmp)
             oySprintf_( &txt[ oyStrlen_(txt) ], "; %s:", tmp );

           if( s->channels_ )
           for(i = 0; i < oyICCColourSpaceGetChannelCount( sig ); ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", s->channels_[i] );
           break;
      default:
           break;
    }

    oyObject_SetName ( s->oy_, txt, type );

    oyFree_m_( txt );
    
    text = oyObject_GetName( s->oy_, type );
  }

  if(!text && type > oyNAME_NICK)
    text = oyObject_GetName( s->oy_, type - 2 );
  if(!text && type > oyNAME_NAME)
    text = oyObject_GetName( s->oy_, type - 1 );
  if(!text && type < oyNAME_NICK )
    text = oyObject_GetName( s->oy_, type + 2 );
  if(!text && type < oyNAME_DESCRIPTION )
    text = oyObject_GetName( s->oy_, type + 1 );
  if(!text)
    text = _("---");

  return text;
}









/** @func  oyNamedColours_New
 *  @brief create a list object for oyNamedColour_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_New ( oyObject_s       object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_TYPE_e type = oyOBJECT_TYPE_NAMED_COLOURS_S;
# define STRUCT_TYPE oyNamedColours_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_CopyF_t) oyNamedColours_Copy;
  s->release = (oyStruct_ReleaseF_t) oyNamedColours_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(!error)
    s->list_ = oyStructList_New( object );

  return s;
}

/** @func  oyNamedColours_Copy
 *  @brief Copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          obj )
{
  oyNamedColours_s * s = 0;
  int error = !colours;

  if(error)
    return s;

  if(colours && !obj)
  {
    s = colours;
    s->oy_ = oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyNamedColours_New( obj );

  s->list_ = oyStructList_Copy( colours->list_, 0 );

  return s;
}

/** @func
 *  @brief 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Release ( oyNamedColours_s** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyNamedColours_s * s = 0;

  if(!obj || !*obj)
    return 1;

  s = *obj;

  if( !s->oy_ || s->type_ != oyOBJECT_TYPE_NAMED_COLOURS_S)
  {
    WARNc_S(("Attempt to release a non oyNamedColours_s object."))
    return 1;
  }

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    s->type_ = 0;

    deallocateFunc( s );
  }

  return error;
}

/** @func
 *  @brief 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Count( oyNamedColours_s * obj )
{
  if(obj && obj->type_ == oyOBJECT_TYPE_NAMED_COLOURS_S)
    return oyStructList_Count( obj->list_ );
  else
    return -1;
}

/** @func  oyNamedColours_Get
 *  @brief obtain a referenced patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColour_s*  oyNamedColours_GetRef ( oyNamedColours_s  * obj,
                                       int                 position)
{
  int error = !obj;
  oyNamedColour_s * s = 0;

  if(!error && obj->type_ != oyOBJECT_TYPE_NAMED_COLOURS_S)
    error = 1;

  if(!error)
    s = (oyNamedColour_s*) oyStructList_GetRefType( obj->list_, position,
                                                 oyOBJECT_TYPE_NAMED_COLOUR_S );

 
  return s;
}

/** @func  oyNamedColours_Add
 *  @brief add a patch to the colours list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s * oyNamedColours_MoveIn ( oyNamedColours_s  * list,
                                       oyNamedColour_s  ** obj,
                                       int                 pos )
{
  int error = 0;
  oyNamedColours_s * s = list;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_TYPE_NAMED_COLOUR_S)
  {
    if(!s)
    {
      s = oyNamedColours_New(0);
      error = !s;
    }

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_New( 0 );
      error = !s->list_;
    }

    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos );
  }

  return s;
}

/** @func  oyNamedColours_RemovePatch
 *  @brief release a patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_ReleaseAt ( oyNamedColours_s * obj,
                                       int                 position )
{
  int error = !obj;

  if(!error && obj->type_ != oyOBJECT_TYPE_NAMED_COLOURS_S)
    error = 1;

  if(!error)
    oyStructList_ReleaseAt( obj->list_, position );

  return error; 
}


/** @} */


/** \addtogroup cmm_handling CMM Handling API

 *  @{
 */
const char *  oyModuleGetActual       ( unsigned int       flags )
{
  oyExportStart_(EXPORT_CMMS);
  oyExportEnd_();
  return "lcms";
}

uint32_t     oyCMMtoId               ( const char        * cmm )
{
  if(cmm)
    return oyValueUInt32( *(uint32_t*)cmm );
  else
    return 0;
}
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm )
{
  cmmId = oyValueUInt32( cmmId );

  if(cmm)
    return !memcpy( cmm, &cmmId, 4 );
  else
    return 0;
}

/** @} */


/** \addtogroup misc Miscellaneous

 *  @{
 */


/** @brief MSB<->LSB */
icUInt16Number
oyValueUInt16 (icUInt16Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
# define BYTES 2
# define KORB  4
  unsigned char        *temp  = (unsigned char*) &val;
  unsigned char  korb[KORB];
  int i;
  for (i = 0; i < KORB ; i++ )
    korb[i] = (int) 0;  /* empty */

  {
  int klein = 0,
      gross = BYTES - 1;
  for (; klein < BYTES ; klein++ ) {
    korb[klein] = temp[gross--];
  }
  }

  {
  unsigned int *erg = (unsigned int*) &korb[0];

# undef BYTES
# undef KORB
  return (long)*erg;
  }
#else
  return (long)val;
#endif
}

icUInt32Number
oyValueUInt32 (icUInt32Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned char        *temp = (unsigned char*) &val;

  unsigned char  uint32[4];

  uint32[0] = temp[3];
  uint32[1] = temp[2];
  uint32[2] = temp[1];
  uint32[3] = temp[0];

  {
  unsigned int *erg = (unsigned int*) &uint32[0];


  return (icUInt32Number) *erg;
  }
#else
  return (icUInt32Number)val;
#endif
}

unsigned long
oyValueUInt64 (icUInt64Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned char        *temp  = (unsigned char*) &val;

  unsigned char  uint64[8];
  int little = 0,
      big    = 8;

  for (; little < 8 ; little++ ) {
    uint64[little] = temp[big--];
  }

  {
  unsigned long *erg = (unsigned long*) &uint64[0];

  return (long)*erg;
  }
#else
  return (long)val;
#endif
}

#define icValue_to_icUInt32Number_m(funkname, typ) \
typ \
funkname (typ val) \
{ \
  icUInt32Number i = (typ) val; \
  return (typ) oyValueUInt32 (i); \
}

icValue_to_icUInt32Number_m( oyValueCSpaceSig, icColorSpaceSignature )
icValue_to_icUInt32Number_m( oyValuePlatSig, icPlatformSignature )
icValue_to_icUInt32Number_m( oyValueProfCSig, icProfileClassSignature )
icValue_to_icUInt32Number_m( oyValueTagSig, icTagSignature )



/** @func  oyThreadLockingSet
 *  @brief set locking functions for threaded applications
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 january 2008 (API 0.1.8)
 */
void         oyThreadLockingSet        ( oyStruct_LockCreateF_t createLockFunc,
                                         oyLockReleaseF_t  releaseLockFunc,
                                         oyLockF_t         lockFunc,
                                         oyUnLockF_t       unlockFunc )
{
  oyStruct_LockCreateFunc_ = createLockFunc;
  oyLockReleaseFunc_ = releaseLockFunc;
  oyLockFunc_ = lockFunc;
  oyUnLockFunc_ = unlockFunc;

  if(!oyStruct_LockCreateFunc_ && !oyLockReleaseFunc_ &&
     !oyLockFunc_ && !oyUnLockFunc_)
  {
    oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
    oyLockReleaseFunc_ = oyLockReleaseDummy_;
    oyLockFunc_ = oyLockDummy_;
    oyUnLockFunc_ = oyUnLockDummy_;
  }
}


/** @} */

