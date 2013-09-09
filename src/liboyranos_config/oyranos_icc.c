/** @file oyranos_icc.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    misc ICC support APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
#include "oyranos_image.h"
#include "oyranos_io.h"
#include "oyranos_texts.h"
#if !defined(WIN32)
#include <dlfcn.h>
#endif
#include <math.h>


/** \addtogroup color Color

 *  @{
 */

/** \addtogroup icc_low ICC helpers

 *  @{
 */

/** Function: oyICCColorSpaceGetChannelCount
 *  @brief number of channels in a color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int
oyICCColorSpaceGetChannelCount ( icColorSpaceSignature color )
{
  int n;

  switch ((unsigned int)color) {
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

/** Function: oyICCColorSpaceGetName
 *  @brief name of a color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
const char *
oyICCColorSpaceGetName ( icColorSpaceSignature sig )
{
  const char * text = 0;

  switch ((unsigned int)sig) {
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

/** Function: oyICCColorSpaceGetChannelName
 *  @brief channel names of a color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
const oyChar*
oyICCColorSpaceGetChannelName ( icColorSpaceSignature sig,
                                    int                   pos,
                                    int                   type )
{
  int n = oyICCColorSpaceGetChannelCount( sig );

  if( 0 <= pos && pos < n )
    return "-";

  if( type < 0 )
    type = 0;
  if( type >= oyNAME_DESCRIPTION )
    type = oyNAME_NAME;

  switch((unsigned int)sig)
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
         case 1: return type ? "Cb" : _("Color b");
         case 2: return type ? "Cr" : _("Color r");
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

/** Function: oyICCColorSpaceToChannelLayout
 *  @brief   describe a channels characteristic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
oyCHANNELTYPE_e oyICCColorSpaceToChannelLayout (
                                       icColorSpaceSignature sig,
                                       int                 pos )
{
  int n = oyICCColorSpaceGetChannelCount( sig );

    if(sig == icSigXYZData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 2) return oyCHANNELTYPE_COLOR_LIGHTNESS;
    }
    if(sig == icSigLabData) {
         if(pos == 0) return oyCHANNELTYPE_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_COLOR;
    }
    if(sig == icSigLuvData) {
         if(pos == 0) return oyCHANNELTYPE_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_COLOR;
    }
    if(sig == icSigYCbCrData) {
         if(pos == 0) return oyCHANNELTYPE_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_COLOR;
    }
    if(sig == icSigYxyData) {
         if(pos == 0) return oyCHANNELTYPE_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_COLOR;
    }
    if(sig == icSigRgbData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 2) return oyCHANNELTYPE_COLOR_LIGHTNESS;
    }
    if(sig == icSigGrayData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR_LIGHTNESS;
    }
    if(sig == icSigHsvData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_LIGHTNESS;
    }
    if(sig == icSigHlsData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR;
         if(pos == 1) return oyCHANNELTYPE_COLOR;
         if(pos == 2) return oyCHANNELTYPE_LIGHTNESS;
    }
    if(sig == icSigCmykData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 2) return oyCHANNELTYPE_COLOR_LIGHTNESS;
    }
    if(sig == icSigCmyData) {
         if(pos == 0) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 1) return oyCHANNELTYPE_COLOR_LIGHTNESS;
         if(pos == 2) return oyCHANNELTYPE_COLOR_LIGHTNESS;
    }

  if(pos < n)
    return oyCHANNELTYPE_COLOR_LIGHTNESS;
  else
    return oyCHANNELTYPE_OTHER;
}


/** Function: oyICCTagDescription
 *  @brief get tag description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagDescription ( icTagSignature      sig )
{

  switch ((unsigned int)sig) {
    case icSigAToB0Tag: return _("Lookup table, device to PCS, intent perceptual");
    case icSigAToB1Tag: return _("Lookup table, device to PCS, intent relative colorimetric");
    case icSigAToB2Tag: return _("Lookup table, device to PCS, intent saturation");
    case icSigBlueColorantTag: return _("Blue Colorant");
    case icSigBlueTRCTag: return _("Blue tone reproduction curve");
    case icSigBToA0Tag: return _("Lookup table, PCS to device, intent perceptual");
    case icSigBToA1Tag: return _("Lookup table, PCS to device, intent relative colorimetric");
    case icSigBToA2Tag: return _("Lookup table, PCS to device, intent saturation");
    case icSigCalibrationDateTimeTag: return _("Calibration date");
    case icSigCharTargetTag: return _("Color measurement data");
    case icSigCopyrightTag: return _("Copyright");
    case icSigCrdInfoTag: return _("Postscript CRD Information");
    case icSigDeviceMfgDescTag: return _("Device manufacturerer description");
    case icSigDeviceModelDescTag: return _("Device model description");
    case icSigDeviceSettingsTag: return _("Device Settings");
    case icSigGamutTag: return _("gamut");
    case icSigGrayTRCTag: return _("Gray tone reproduction curve");
    case icSigGreenColorantTag: return _("Green Colorant");
    case icSigGreenTRCTag: return _("Green tone reproduction curve");
    case icSigLuminanceTag: return _("Luminance");
    case icSigMeasurementTag: return _("Measurement");
    case icSigMediaBlackPointTag: return _("Media black point");
    case icSigMediaWhitePointTag: return _("Media white point");
    case icSigMetaDataTag: return _("Meta Data");
    case icSigNamedColorTag: return _("Named Color");
    case icSigNamedColor2Tag: return _("Named Color 2");
    case icSigPreview0Tag: return _("Preview, perceptual");
    case icSigPreview1Tag: return _("Preview, relative colorimetric");
    case icSigPreview2Tag: return _("Preview, saturated");
    case icSigProfileDescriptionTag: return _("Profile description");
    /*dscm*/
    case 1685283693: return _("Profile description, multilingual");
    case icSigProfileSequenceDescTag: return _("Profile sequence description");
    case icSigProfileSequenceIdentifierTag: return _("Profile sequence identification");
    case icSigPs2CRD0Tag: return _("Postscript 2 CRD, perceptual");
    case icSigPs2CRD1Tag: return _("Postscript 2 CRD, relative colorimetric");
    case icSigPs2CRD2Tag: return _("Postscript 2 CRD, saturated");
    case icSigPs2CRD3Tag: return _("Postscript 2 CRD, absolute colorimetric");
    case icSigPs2CSATag: return _("Postscript 2 CSA");
    case icSigPs2RenderingIntentTag: return _("Postscript 2 Rendering Intent");
    case icSigRedColorantTag: return _("Red Colorant");
    case icSigRedTRCTag: return _("Red tone reproduction curve");
    case icSigScreeningDescTag: return _("Screening Description");
    case icSigScreeningTag: return _("Screening");
    case icSigTechnologyTag: return _("Technologie");
    case icSigUcrBgTag: return _("Under Color Removal (UCR)");
    case icSigViewingCondDescTag: return _("Viewing conditions description");
    case icSigViewingConditionsTag: return _("Viewing Conditions");
    /*DevD*/
    case 1147500100: return _("Device colors");
    /*CIED*/
    case 1128875332: return _("Measured colors");
    /*Pmtr*/
    case 1349350514: return _("Profiling parameters");
    /*vcgt 1986226036*/
    case icSigVideoCardGammaTable: return _("VideoCardGammaTable");
    /*chad*/
    case 1667785060: return _("Color adaption matrix"); 
    /*chrm*/
    case icSigChromaticityType: return _("Chromaticity"); 
    /*clro*/
    case icSigColorantOrderType: return _("Color channel order");
    /*clrt*/
    case icSigColorantTableType: return _("Color channel names");
    /*clrt*/
    case icSigColorantTableOutType: return _("Color channel output names");
    case 0: return _("----");
    default: { icUInt32Number i = oyValueUInt32(sig);
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}


/** Function: oyICCTagName
 *  @brief get tag ~4 char name
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagName        ( icTagSignature      sig )
{
  switch ((unsigned int)sig) {
    case icSigAToB0Tag: return "A2B0";
    case icSigAToB1Tag: return "A2B1";
    case icSigAToB2Tag: return "A2B2";
    case icSigBlueColorantTag: return "bXYZ";
    case icSigBlueTRCTag: return "bTRC";
    case icSigBToA0Tag: return "B2A0";
    case icSigBToA1Tag: return "B2A1";
    case icSigBToA2Tag: return "B2A2";
    case icSigCalibrationDateTimeTag: return "calt";
    case icSigCharTargetTag: return "targ";
    case icSigCopyrightTag: return "cprt";
    case icSigCrdInfoTag: return "crdi";
    case icSigDeviceMfgDescTag: return "dmnd";
    case icSigDeviceModelDescTag: return "dmdd";
    case icSigDeviceSettingsTag: return "DevS";
    case icSigGamutTag: return "gamt";
    case icSigGrayTRCTag: return "kTRC";
    case icSigGreenColorantTag: return "gXYZ";
    case icSigGreenTRCTag: return "gTRC";
    case icSigLuminanceTag: return "lumi";
    case icSigMeasurementTag: return "meas";
    case icSigMediaBlackPointTag: return "bkpt";
    case icSigMediaWhitePointTag: return "wtpt";
    case icSigMetaDataTag: return "meta";
    case icSigNamedColorTag: return "'ncol";
    case icSigNamedColor2Tag: return "ncl2";
    case icSigPreview0Tag: return "pre0";
    case icSigPreview1Tag: return "pre1";
    case icSigPreview2Tag: return "pre2";
    case icSigProfileDescriptionTag: return "desc";
    case 1685283693: return "dscm";
    case icSigProfileSequenceDescTag: return "pseq";
    case icSigProfileSequenceIdentifierTag: return "psid";
    case icSigPs2CRD0Tag: return "psd0";
    case icSigPs2CRD1Tag: return "psd1";
    case icSigPs2CRD2Tag: return "psd2";
    case icSigPs2CRD3Tag: return "psd3";
    case icSigPs2CSATag: return "ps2s";
    case icSigPs2RenderingIntentTag: return "ps2i";
    case icSigRedColorantTag: return "rXYZ";
    case icSigRedTRCTag: return "rTRC";
    case icSigScreeningDescTag: return "scrd";
    case icSigScreeningTag: return "scrn";
    case icSigTechnologyTag: return "tech";
    case icSigUcrBgTag: return "bfd";
    case icSigViewingCondDescTag: return "vued";
    case icSigViewingConditionsTag: return "view";
    case 1147500100: return "DevD";
    case 1128875332: return "CIED";
    case 1349350514: return "Pmtr";
    case icSigVideoCardGammaTable: return "vcgt";
    case 1667785060: return "chad";
    case icSigChromaticityType: return "chrm";
    case 1668051567: return "clro";
    case 1668051572: return "clrt";
    /* binuscan targ data */
    case 0x62303135: return "b015";
    case 0: return "----";
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}



/** Function: oyICCDeviceClassDescription
 *  @brief get the ICC profile icProfileClassSignature description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCDeviceClassDescription ( icProfileClassSignature sig )
{
  switch (sig)
  {
    /* scnr */
    case icSigInputClass: return _("Input");
    /* mntr */
    case icSigDisplayClass: return _("Display");
    /* prtr */
    case icSigOutputClass: return _("Output");
    /* link */
    case icSigLinkClass: return _("Link");
    /* abst */
    case icSigAbstractClass: return _("Abstract");
    /* spac */
    case icSigColorSpaceClass: return _("Color Space");
    /* nmcl */
    case icSigNamedColorClass: return _("Named Color");
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}

/** Function: oyICCPlatformDescription
 *  @brief get the ICC profile platform description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCPlatformDescription ( icPlatformSignature platform )
{
  switch ((unsigned int)platform)
  {
    case icSigMacintosh: return _("Macintosh");
    case icSigMicrosoft: return _("Microsoft");
    case icSigSolaris: return _("Solaris");
    case icSigSGI: return _("SGI");
    case icSigTaligent: return _("Taligent");
    case icSigUnix: return _("Unix and derivatives");
    default: { icUInt32Number i = oyValueUInt32(platform);
               static oyChar t[5];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}


/** Function: oyICCTagTypeName
 *  @brief get tag type ~4 char name
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTagTypeName    ( icTagTypeSignature  sig )
{

  switch ((unsigned int)sig) {
    case icSigCurveType: return "curv";
    case icSigDataType: return "data";
    case icSigDateTimeType: return "dtim";
    case icSigDeviceSettingsType: return "DevS";
    case icSigLut16Type: return "mft2";
    case icSigLut8Type: return "mft1";
    case icSigMeasurementType: return "meas";
    case icSigDictType: return "dict";
    case icSigNamedColorType: return "ncol";
    case icSigProfileSequenceDescType: return "pseq";
    case icSigProfileSequenceIdentifierType: return "psid";
    case icSigS15Fixed16ArrayType: return "sf32";
    case icSigScreeningType: return "scrn";
    case icSigSignatureType: return "sig";
    case icSigTextType: return "text";
    case icSigTextDescriptionType: return "desc";
    case icSigU16Fixed16ArrayType: return "uf32";
    case icSigUcrBgType: return "bfd";
    case icSigUInt16ArrayType: return "ui16";
    case icSigUInt32ArrayType: return "ui32";
    case icSigUInt64ArrayType: return "ui64";
    case icSigUInt8ArrayType: return "ui08";
    case icSigViewingConditionsType: return "view";
    case icSigXYZType: return "XYZ";
    /*case icSigXYZArrayType: return "XYZ";*/
    case icSigNamedColor2Type: return "ncl2";
    case icSigCrdInfoType: return "crdi";
    case icSigChromaticityType: return "chrm";
    case 1986226036: return "vcgt";
    /*??? (Imacon)*/
    case icSigCopyrightTag: return "cprt?";
    case 1835824483: return "mluc";
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}

/** Function: oyICCTechnologyDescription
 *  @brief get ICC profile icTechnologySignature description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCTechnologyDescription ( icTechnologySignature sig )
{
  switch (sig) {
    /*dcam*/
    case icSigDigitalCamera: return _("Digital camera");
    /*fscn*/
    case icSigFilmScanner: return _("Film scanner");
    /*rscn*/
    case icSigReflectiveScanner: return _("Reflective scanner");
    /*ijet*/
    case icSigInkJetPrinter: return _("InkJet printer");
    /*twax*/
    case icSigThermalWaxPrinter: return _("Thermal wax printer");
    /*epho*/
    case icSigElectrophotographicPrinter: return _("Electrophotographic printer");
    /*esta*/
    case icSigElectrostaticPrinter: return _("Electrostatic printer");
    /*dsub*/
    case icSigDyeSublimationPrinter: return _("Dye sublimation printer");
    /*rpho*/
    case icSigPhotographicPaperPrinter: return _("Photographic paper printer");
    /*fprn*/
    case icSigFilmWriter: return _("Film writer");
    /*vidm*/
    case icSigVideoMonitor: return _("Video Monitor");
    /*vidc*/
    case icSigVideoCamera: return _("Video camera");
    /*pjtv*/
    case icSigProjectionTelevision: return _("Projection Television");
    /*CRT*/
    case icSigCRTDisplay: return _("Cathode ray tube display");
    /*PMD*/
    case icSigPMDisplay: return _("Passive matrix monitor");
    /*AMD*/
    case icSigAMDisplay: return _("Active matrix monitor");
    /*KPCD*/
    case icSigPhotoCD: return _("Photo CD");
    /*imgs*/
    case icSigPhotoImageSetter: return _("PhotoImageSetter");
    /*grav*/
    case icSigGravure: return _("Gravure");
    /*offs*/
    case icSigOffsetLithography: return _("Offset Lithography");
    /*silk*/
    case icSigSilkscreen: return _("Silkscreen");
    /*flex*/
    case icSigFlexography: return _("Flexography");
    /*case icMaxEnumTechnology: return _("----"); break;*/
    default: { icUInt32Number i = oyValueUInt32( sig );
               static oyChar t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               return t;
             }
  }
}

/** Function: oyICCChromaticityColorantDescription
 *  @brief get ICC profile Chromaticity Colorant description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCChromaticityColorantDescription ( icSignature sig )
{
  switch (sig) {
    case 0: return "";
    case 1: return _("ITU-R BT.709");
    case 2: return _("SMPTE RP145-1994");
    case 3: return _("EBU Tech.3213-E");
    case 4: return _("P22");

    default: return _("???");
  }
}

/** Function: oyICCIlluminantDescription
 *  @brief get ICC profile illuminant description
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/01
 *  @since   2005/00/00 (Oyranos: 0.1.8)
 */
const oyChar *   oyICCIlluminantDescription ( icIlluminant sig )
{
  switch (sig) {
    case icIlluminantUnknown: return _("Illuminant unknown");
    case icIlluminantD50: return _("Illuminant D50");
    case icIlluminantD65: return _("Illuminant D65");
    case icIlluminantD93: return _("Illuminant D93");
    case icIlluminantF2: return _("Illuminant F2");
    case icIlluminantD55: return _("Illuminant D55");
    case icIlluminantA: return _("Illuminant A");
    case icIlluminantEquiPowerE: return _("Illuminant with equal energy E");
    case icIlluminantF8: return _("Illuminant F8");
    /*case icMaxEnumIluminant: return _("Illuminant ---"); break;*/

    default: return _("???");
  }
}

/** Function: oyICCStandardObserverDescription
 *  @brief get ICC profile Standard Observer description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCStandardObserverDescription ( icStandardObserver sig )
{
  switch (sig) {
    case icStdObsUnknown: return _("unknown");
    case icStdObs1931TwoDegrees: return _("2 degree (1931)");
    case icStdObs1964TenDegrees: return _("10 degree (1964)");
    /*case icMaxStdObs: return _("---"); break;*/

    default: return _("???");
  }
}


/** Function: oyICCMeasurementGeometryDescription
 *  @brief get ICC profile Measurement Geometry description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCMeasurementGeometryDescription ( icMeasurementGeometry sig )
{
  switch (sig) {
    case icGeometryUnknown: return _("unknown");
    case icGeometry045or450: return _("0/45, 45/0");
    case icGeometry0dord0: return _("0/d or d/0");
    /*case icMaxGeometry: return _("---");*/

    default: return _("???");
  }
}

/** Function: oyICCMeasurementFlareDescription
 *  @brief get ICC profile Measurement Flare description
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
const oyChar *   oyICCMeasurementFlareDescription ( icMeasurementFlare sig )
{
  switch (sig) {
    case icFlare0: return _("0");
    case icFlare100: return _("100");
    /*case icMaxFlare: return _("---");*/

    default: return _("???");
  }
}




#define icValue_to_icUInt32Number_m(funkname, typ) \
typ \
funkname (typ val) \
{ \
  icUInt32Number i = (icUInt32Number) val; \
  return (typ) oyValueUInt32 (i); \
}

icValue_to_icUInt32Number_m( oyValueCSpaceSig, icColorSpaceSignature )
icValue_to_icUInt32Number_m( oyValuePlatSig, icPlatformSignature )
icValue_to_icUInt32Number_m( oyValueProfCSig, icProfileClassSignature )
icValue_to_icUInt32Number_m( oyValueTagSig, icTagSignature )



/**
 *  @} *//* icc_low
 */


/** @} *//* color */

