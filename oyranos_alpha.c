
/** @file @internal
 *  @brief misc alpha APIs
 */

/** @date      25. 11. 2004 */


#include "oyranos_alpha.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_monitor.h" /* TODO */
#if !defined(WIN32)
#include <dlfcn.h>
#endif
#include <math.h>

/** @internal
 *  pixel mask description 
 */
enum {
  oyPOFF_X = 0, /* pixel count x offset */
  oyPOFF_Y,     /* pixel count y offset */
  oyDATA_SIZE,  /* sample size in byte */
  oyLAYOUT,     /* remembering the layout */
  oyCHANS,      /* number of channels */
  oyCHAN0       /* first colour channel */
};

/*void* myCAllocFunc(size_t size)
{
  return calloc( sizeof(char), size );
}

void  myCDeAllocFunc(void * buf)
{
  if(buf)
    free(buf);
}*/

/** @brief number of channels in a colour space
 *
 *  since: Oyranos: version 0.1.8
 */
int
oyColourSpaceGetChannelCountFromSig ( icColorSpaceSignature color )
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

/** @brief name of a colour space
 *
 *  since: Oyranos: version 0.1.8
 */
const char *
oyColourSpaceGetNameFromSig( icColorSpaceSignature sig )
{
  const char * text;

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
               char t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = '?';
               t[5] = 0;
               text = &t[0];
               break;
             }
  }
  return text;
}

/** @brief channel names of a colour space
 *
 *  since: Oyranos: version 0.1.8
 */
const oyChar*
oyColourSpaceGetChannelNameFromSig (icColorSpaceSignature sig,
                                    int                   pos,
                                    int                   type )
{
  int n = oyColourSpaceGetChannelCountFromSig( sig );

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
         case 0: return type ? "1." : _("1. Colour");
         case 1: return type ? "2." : _("2. Colour");
         case 2: return type ? "3." : _("3. Colour");
         case 3: return type ? "4." : _("4. Colour");
         case 4: return type ? "5." : _("5. Colour");
         case 5: return type ? "6." : _("6. Colour");
         case 6: return type ? "7." : _("7. Colour");
         case 7: return type ? "8." : _("8. Colour");
         case 8: return type ? "9." : _("9. Colour");
         case 9: return type ? "10." : _("10. Colour");
         case 10: return type ? "11." : _("11. Colour");
         case 11: return type ? "12." : _("12. Colour");
         case 12: return type ? "13." : _("13. Colour");
         case 13: return type ? "14." : _("14. Colour");
         case 14: return type ? "15." : _("15. Colour");
         case 15: return type ? "16." : _("16. Colour");
         } break;
    default: return ("-"); break;
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


/** @brief copy pure colours

    handle colour only, without from set to -1 default
 *
 *  since: Oyranos: version 0.1.8
 */
void
oyCopyColour ( const double * from,
               double       * to,
               int            n,
               oyProfile_s  * ref,
               int            channels_n )
{
  int i, j;
  icColorSpaceSignature sig = oyProfileGetSignature ( ref );
  int c = oyColourSpaceGetChannelCountFromSig( sig );
  int error = 0;

  if(!n || !to)
    return;

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

/* Has to go to oyranos_texts.x */
char*
oyStringCopy_      ( const char    * text,
                     oyAllocFunc_t   allocateFunc )
{
  char * text_copy = NULL;

  if(text)
    if( oyStrlen_(text) )
    {
      text_copy = allocateFunc(strlen(text) + 1);
      oyAllocHelper_m_( text_copy, oyChar, oyStrlen_(text) + 1,
                        allocateFunc, return 0 );
      sprintf( text_copy, "%s", text );
    }
  return text_copy;
}





/* --- Object handling API's start ------------------------------------ */

void         oyNameRelease_           ( oyName_s       ** obj,
                                        oyDeAllocFunc_t   deallocateFunc )
{
  /* ---- start of common object destructor ----- */
  oyName_s * s = 0;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( s->type != oyOBJECT_TYPE_NAME_S)
  {
    WARNc_S(("Attampt to release a non oyName_s object."))
    return;
  }
  /* ---- end of common object destructor ------- */

  *obj = 0;

  if(!deallocateFunc)
    return;

  if(s->nick)
    deallocateFunc(s->nick); s->nick = 0;

  if(s->name)
    deallocateFunc(s->name); s->name = 0;

  if(s->description)
    deallocateFunc(s->description); s->description = 0;

  deallocateFunc( s );

}



/** @brief object management 
 *
 *  since: Oyranos version 0.1.8
 */
oyObject_s *
oyObjectNew  ( void )
{
  oyObject_s * o = 0;
  int error = 0;

  o = oyAllocateFunc_(sizeof(oyObject_s));

  if(!o) return 0;

  error = !memset( o, 0, sizeof(oyObject_s) );
  
  o = oyObjectSetAllocators( o, oyAllocateFunc_, oyDeAllocateFunc_ );

  o->type = oyOBJECT_TYPE_OBJECT_S;
  o->version = oyVersion(0);
  o->ref++;

  return o;
}

/** @brief object management 
 *
 *  since: Oyranos version 0.1.8
 */
oyObject_s *
oyObjectNewWithAllocators  ( oyAllocFunc_t     allocateFunc,
                             oyDeAllocFunc_t   deallocateFunc )
{
  oyObject_s * o = 0;
  int error = 0;

  if(allocateFunc)
    o = allocateFunc(sizeof(oyObject_s));
  else
    o = oyAllocateFunc_(sizeof(oyObject_s));

  if(!o) return 0;

  error = !memset( o, 0, sizeof(oyObject_s) );
  
  o = oyObjectSetAllocators( o, allocateFunc, deallocateFunc );

  o->type = oyOBJECT_TYPE_OBJECT_S;
  o->version = oyVersion(0);
  o->ref++;

  return o;
}

/** @internal
 *  @brief object management 
 *
 *  @param[in]    object         the object
 *  @param[in]    type           object type
 *
 *  since: Oyranos version 0.1.8
 */
static oyObject_s *
oyObjectCopy_( oyObject_s    * object )
{
  oyObject_s * o = 0;
  int error = 0;

  if(object)
    o = oyObjectNewWithAllocators( object->allocateFunc,
                                   object->deallocateFunc );
  else
    o = oyObjectNew( );

  if(!o)
    error = 1;

  if(!error && object && object->name)
    o = oyObjectSetNames( o, object->name->nick, object->name->name,
                             object->name->description );

  if(!o)
    error = 1;

  return o;
}

/** @brief object management 
 *
 *  @param[in]    object         the object
 *
 *  since: Oyranos version 0.1.8
 */
oyObject_s *
oyObjectCopy ( oyObject_s    * object )
{
  return oyObjectCopy_( object );
}

/** @brief custom object memory managers
 *
 *  @param[in]    object         the object to modify
 *  @param[in]    allocateFunc   zero for default or user memory allocator
 *  @param[in]    deallocateFunc zero for default or user memory deallocator
 *
 *  since: Oyranos: version 0.1.8
 */
oyObject_s * oyObjectSetAllocators    ( oyObject_s      * object,
                                        oyAllocFunc_t     allocateFunc,
                                        oyDeAllocFunc_t   deallocateFunc )
{
  if(!object) return 0;

  /* we require a allocation function to be present */
  if(allocateFunc)
    object->allocateFunc = allocateFunc;
  else
    object->allocateFunc = oyAllocateFunc_;

  object->deallocateFunc = deallocateFunc;

  return object;
}

/** @brief release an Oyranos object
 *
 *  since: Oyranos: version 0.1.8
 */
void         oyObjectRelease          ( oyObject_s     ** obj )
{
  int i;
  /* ---- start of common object destructor ----- */
  oyObject_s * s = 0;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( s->type != oyOBJECT_TYPE_OBJECT_S)
  {
    WARNc_S(("Attampt to release a non oyObject_s object."))
    return;
  }

  if(--s->ref > 0)
    return;
  /* ---- end of common object destructor ------- */

  oyNameRelease_( &s->name, s->deallocateFunc );

  i = 0;
  if(s->cmm_ptr)
  while(s->cmm_ptr[i])
  {
    if(s->cmm_ptr[i]->deallocateFunc && s->cmm_ptr[i]->ptr)
      s->cmm_ptr[i]->deallocateFunc( s->cmm_ptr[i]->ptr );
    s->cmm_ptr[i]->ptr = 0;
    ++i;
  }

  if(s->deallocateFunc)
  {
    oyDeAllocFunc_t deallocateFunc = s->deallocateFunc;

    if(s->hash)
      deallocateFunc( s->hash ); s->hash = 0;

    if(s->backdoor)
      deallocateFunc( s->backdoor ); s->backdoor = 0;

    if(s->cmm_ptr)
    {
      i = 0;
      while(s->cmm_ptr[i])
      {
        if(s->cmm_ptr[i]->cmm)
          deallocateFunc( s->cmm_ptr[i]->cmm ); s->cmm_ptr[i]->cmm = 0;
        deallocateFunc( s->cmm_ptr[i] );
        s->cmm_ptr[i] = 0;
        ++i;
      }
      deallocateFunc( s->cmm_ptr ); s->cmm_ptr = 0;
    }

    deallocateFunc( s );
  }

  *obj = 0;

  return;
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
 *  since: Oyranos: version 0.1.8
 */
oyObject_s * oyObjectSetParent        ( oyObject_s      * o,
                                        oyOBJECT_TYPE_e   type,
                                        oyPointer         parent )
{
  int error = 0;
  if(!error && type)
  {
    o->parent_type = type;
    if(parent)
      o->parent = parent;
  }

  return o;
}

/** @brief object naming
 *
 *  @param[in]    object         the object
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    type           the kind of name 
 *
 *  since: Oyranos: version 0.1.8
 */
oyObject_s * oyObjectSetName          ( oyObject_s      * object,
                                        const char      * text,
                                        oyNAME_e          type )
{
  int error = 0;

  if(!object) return 0;

  {
#define oySetString_(n_type)\
    if(!object->name) \
    { \
      oyAllocHelper_m_( object->name, oyName_s, 1, object->allocateFunc, \
                        error = 1 ) \
      if(!error) \
        error = !memset(object->name, 0, sizeof(oyName_s)); \
    } \
    if(!error) { \
      if(object->name->n_type && object->deallocateFunc) \
        object->deallocateFunc( object->name->n_type ); \
      object->name->n_type = oyStringCopy_( text, object->allocateFunc ); \
      if( !object->name->n_type ) error = 1; \
    } 
    switch (type) {
    case oyNAME_NICK:
         oySetString_(nick) break;
    case oyNAME_NAME:
         oySetString_(name) break;
    case oyNAME_DESCRIPTION:
         oySetString_(description) break;
    }
#undef oySetString_
  }

  return object;
}


/** @brief object naming
 *
 *  @param[in]    object         the object
 *  @param[in]    nick           short name, about 1-4 letters
 *  @param[in]    name           the name should fit into usual labels
 *  @param[in]    description    the description 
 *
 *  since: Oyranos: version 0.1.8
 */
oyObject_s * oyObjectSetNames         ( oyObject_s      * object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description )
{
  int error = 0;
  

  if(!object) return 0;

  if(!error)
    error = !oyObjectSetName( object, nick, oyNAME_NICK );
  if(!error)
    error = !oyObjectSetName( object, name, oyNAME_NAME );
  if(!error)
    error = !oyObjectSetName( object, description, oyNAME_DESCRIPTION );

  return object;
}

/** @brief object get names
 *
 *  @param[in]    object         the object
 *  @param[in]    type           name type
 *
 *  since: Oyranos: version 0.1.8
 */
const char * oyObjectGetName          ( const oyObject_s      * obj,
                                        oyNAME_e                type )
{
  const oyChar * text = 0;
  if(!obj)
    return 0;

  if(!obj->name)
    return 0;

  switch(type)
  {
    case oyNAME_NICK:
         text = obj->name->nick; break;
    case oyNAME_DESCRIPTION:
         text = obj->name->description; break;
    case oyNAME_NAME:
    default:
         text = obj->name->name; break;
  }

  return text;
}

/** @brief get CMM specific data pointer
 *
 *  @param[in]    object         the object
 *  @param[in]    cmm            CMM name
 *
 *  since: Oyranos: version 0.1.8
 */
oyPointer    oyObjectGetCMMPtr        ( oyObject_s      * object,
                                        const char      * cmm )
{
  oyObject_s * s = object;
  int error = 0;
  oyPointer ptr = 0;
  int pos = 0;

  if(!s)
    error = 1;

  if(!error)
    cmm = oyCMMGetActual( 0 );

  if(!error && s->cmm_ptr)
  {
    int cmm_n = 0;

    while(s->cmm_ptr[cmm_n])
    {
      if( s->cmm_ptr[cmm_n]->cmm &&
          !oyStrcmp_( s->cmm_ptr[cmm_n]->cmm, cmm ) )
      {
        pos = cmm_n;
        ptr = s->cmm_ptr[cmm_n]->ptr;
      }
      ++cmm_n;
    }
  }

  return ptr;
}

/** @brief set CMM specific data pointer
 *
 *  @param[in]    object         the object
 *  @param[in]    cmm            CMM name
 *  @param[in]    ptr            CMM specific pointer or zero to remove
 *
 *  since: Oyranos: version 0.1.8
 */
oyObject_s * oyObjectSetCMMPtr        ( oyObject_s      * object,
                                        const char      * cmm,
                                        oyPointer         cmm_ptr,
                                        oyDeAllocFunc_t   deallocateFunc )
{
  oyObject_s * s = object;
  int error = 0;
  int pos = -1;
  int cmm_n = 0;

  if(!s) error = 1;

  if(!cmm)
    cmm = oyCMMGetActual(0);

  if(!error && s->cmm_ptr)
  {
    while(s->cmm_ptr[cmm_n])
    {
      if( s->cmm_ptr[cmm_n]->cmm &&
          !oyStrcmp_( s->cmm_ptr[cmm_n]->cmm, cmm ) )
        pos = cmm_n;
      ++cmm_n;
    }
  }

  if(!error)
  {
    if(pos < 0)
    {
      oyCMMptr_s ** temp = s->cmm_ptr;
      s->cmm_ptr = object->allocateFunc( sizeof(oyCMMptr_s*) * (cmm_n + 2) );
      error = !s->cmm_ptr;
      if(!error && cmm_n)
      {
        error = !memset( s->cmm_ptr, 0, sizeof(oyCMMptr_s*) * (cmm_n + 2) );
        error = !memcpy( s->cmm_ptr, temp, sizeof(oyCMMptr_s*) * cmm_n );
        if(object->deallocateFunc)
          object->deallocateFunc( temp );
      }

    } else {

      if(object->deallocateFunc)
        object->deallocateFunc( s->cmm_ptr[pos]->cmm );
      s->cmm_ptr[pos]->cmm = 0;

      if(s->cmm_ptr[pos]->deallocateFunc)
        s->cmm_ptr[pos]->deallocateFunc( s->cmm_ptr[pos]->ptr );
      s->cmm_ptr[pos]->ptr = 0;

      s->cmm_ptr[pos]->deallocateFunc = 0;
    }

    if(pos < 0)
      pos = 0;

    if(!error)
    {
      if(!s->cmm_ptr[pos])
      {
        s->cmm_ptr[pos] = object->allocateFunc( sizeof(oyCMMptr_s) );
        s->cmm_ptr[pos]->type = oyOBJECT_TYPE_CMM_POINTER_S;
      }

      s->cmm_ptr[pos]->cmm = oyStringCopy_( cmm, object->allocateFunc );
      error = !s->cmm_ptr[pos]->cmm;

      s->cmm_ptr[pos]->ptr = cmm_ptr;

      s->cmm_ptr[pos]->deallocateFunc = deallocateFunc;
    }
  }

  return s;
}







static oyProfile_s *
oyProfileNew_ ( oyObject_s      * object)
{
  /* ---- start of common object constructor ----- */
# define OBJECT_TYPE oyOBJECT_TYPE_PROFILE_S
# define STRUCT_TYPE oyProfile_s
  int error = 0;
  oyObject_s  * s_obj = oyObjectCopy_( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = OBJECT_TYPE;

  s->oy = s_obj;

  error = !oyObjectSetParent( s_obj, OBJECT_TYPE, (oyPointer)s );
# undef OBJECT_TYPE
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */
  return s;
}

#define OY_HASH_SIZE 16

static int
oyProfileGetHash_( oyProfile_s * s )
{
  int error = 1;
  if(s->block && s->size)
  {
    s->hash = (char*) s->oy->allocateFunc(OY_HASH_SIZE);
    error = oyProfileGetMD5( s->block, s->size, s->hash );
    if(error && s->oy->deallocateFunc)
    {
      s->oy->deallocateFunc( s->hash );
      s->hash = 0;
    }
  }
  return error;
}


/** @brief create from default colour space settings
 *
 *  @param[in]    type           default colour space or zero to detect display?
 *  @param[in]    object         the optional base
 *
 *  since: Oyranos: version 0.1.8
 */
oyProfile_s *
oyProfileFromStd      ( oyPROFILE_e       type,
                        oyObject_s      * object)
{
  oyProfile_s * s = 0;
  char * name = 0;
  oyAllocFunc_t allocateFunc = 0;

  if(object)
    allocateFunc = object->allocateFunc;

  name = oyGetDefaultProfileName ( type, allocateFunc );

  s = oyProfileFromFile( name, 0, object );

  if(s)
    s->use_default = type;

  return s;
}

/** @brief create from file
 *
 *  @param[in]    name           profile file name or zero to detect display?
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  since: Oyranos: version 0.1.8
 */
OYAPI oyProfile_s * OYEXPORT
oyProfileFromFile             ( const char      * name,
                                int               flags,
                                oyObject_s      * object)
{
  oyProfile_s * s = 0;
  int error = 0;
  size_t size = 0;
  oyPointer block = 0;
  oyAllocFunc_t allocateFunc = 0;

  if(object)
    allocateFunc = object->allocateFunc;

  if(!error && name)
  {
    block = oyGetProfileBlock( name, &size, allocateFunc );
    if(!block || !size)
      error = 1;
  }

  if(!error)
    s = oyProfileFromMem( size, block, 0, object );

  if(!s)
    error = 1;

  if(!error && name)
    s->file_name = oyStringCopy_( name, s->oy->allocateFunc );

  if(!error && !s->file_name)
    error = 1;

  return s;
}

/** @brief create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    buf            pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  since: Oyranos: version 0.1.8
 */
OYAPI oyProfile_s * OYEXPORT
oyProfileFromMem              ( size_t            size,
                                const oyPointer   block,
                                int               flags,
                                oyObject_s      * object)
{
  oyProfile_s * s = oyProfileNew_( object );
  int error = 0;

  if(block && size)
  {
    s->block = s->oy->allocateFunc( size );
    if(!s->block)
      error = 1;
    else
    {
      s->size = size;
      error = !memcpy( s->block, block, size );
    }
  }

  if (!s->block)
  {
    s->block = oyGetMonitorProfile( NULL, &s->size, s->oy->allocateFunc );
    if(!s->block || !s->size)
      error = 1;
    if(!error)
      s->file_name = oyGetMonitorProfileName( NULL, s->oy->allocateFunc );
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
    error = oyProfileGetHash_( s );

  if(!error)
    error = !oyProfileGetSignature ( s );

  if(!error)
  {
    s->names_chan = 0;
    s->channels_n = 0;
    s->channels_n = oyProfileGetChannelsCount( s );
    error = (s->channels_n <= 0);
  }

  if(error)
  {
    WARNc_S(("Could not create structure for profile."))
  }

  return s;
}

/** @brief create new from existing profile struct
 *
 *  @param[in]    profile        other profile
 *  @param[in]    object         the optional base
 *
 *  since: Oyranos: version 0.1.8
 */
OYAPI oyProfile_s * OYEXPORT
oyProfileCopy_                ( const oyProfile_s*profile,
                                oyObject_s      * object)
{
  oyProfile_s * s = oyProfileNew_( object );
  int error = 0;
  oyAllocFunc_t allocateFunc = 0;

  if(!profile || !s)
    return s;

  allocateFunc = s->oy->allocateFunc;

  if(profile->block && profile->size)
  {
    s->block = s->oy->allocateFunc( profile->size );
    if(!s->block)
      error = 1;
    else
    {
      s->size = profile->size;
      error = !memcpy( s->block, profile->block, profile->size );
    }
  }

  if(!error)
  {
    if(profile->hash)
    {
      s->hash = allocateFunc(OY_HASH_SIZE);
      if(!s->hash)
        error = 1;
      else
        error = !memcpy( s->hash, profile->hash, OY_HASH_SIZE );
    }
    else
      error = oyProfileGetHash_( s );
  }

  if(!error)
  {
    if(profile->sig)
      s->sig = profile->sig;
    else
      error = !oyProfileGetSignature ( s );
  }

  if(!error)
    s->file_name = oyStringCopy_( profile->file_name, allocateFunc );

  if(!error)
    s->use_default = profile->use_default;

  if(!error)
  {
    s->channels_n = oyProfileGetChannelsCount( s );
    error = (s->channels_n <= 0);
  }

  if(!error)
    oyProfileSetChannelNames( s, profile->names_chan );

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
 *  since: Oyranos: version 0.1.8
 */
OYAPI oyProfile_s * OYEXPORT
oyProfileCopy                 ( oyProfile_s     * profile,
                                oyObject_s      * object)
{
  oyProfile_s * s = 0;

  if(profile && !object)
  {
    s = profile;
    s->oy->ref++;
    return s;
  }

  s = oyProfileCopy_( profile, object );
  /* TODO cache */
  return s;
}

/** @brief release correctly
 *
 *  set pointer to zero
 *
 *  @param[in]    address of Oyranos colour space struct pointer
 *
 *  since: Oyranos: version 0.1.8
 */
void
oyProfileRelease( oyProfile_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyProfile_s * s = 0;
  int i;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( !s->oy || s->type != oyOBJECT_TYPE_PROFILE_S)
  {
    WARNc_S(("Attampt to release a non oyProfile_s object."))
    return;
  }

  if(--s->oy->ref > 0)
    return;
  /* ---- end of common object destructor ------- */

  if(s->names_chan)
    for(i = 0; i < s->channels_n; ++i)
      if(s->names_chan[i])
        oyObjectRelease( &s->names_chan[i] );
  /*oyOptionsRelease( s->options );*/

  s->sig = (icColorSpaceSignature)0;

  if(s->oy->deallocateFunc)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy->deallocateFunc;

    if(s->names_chan)
      deallocateFunc( s->names_chan ); s->names_chan = 0;

    if(s->hash)
      deallocateFunc( s->hash ); s->hash = 0;

    if(s->block)
      deallocateFunc( s->block ); s->block = 0; s->size = 0;

    if(s->file_name)
      deallocateFunc( s->file_name ); s->file_name = 0;

    oyObjectRelease( &s->oy );

    deallocateFunc( s );
  }

  *obj = 0;

  return;
}

/** @brief number of channels in a colour space
 *
 *  since: Oyranos: version 0.1.8
 */
int
oyProfileGetChannelsCount( oyProfile_s * profile )
{
  if(!profile)
    return 0;
  if(profile->channels_n)
    return profile->channels_n;
  profile->channels_n = oyColourSpaceGetChannelCountFromSig( profile->sig );
  return profile->channels_n;
}

/** @brief get ICC colour space signature
 *
 *  since: Oyranos: version 0.1.8
 */
OYAPI icColorSpaceSignature OYEXPORT
oyProfileGetSignature ( oyProfile_s * s )
{
  icHeader *h = 0;

  if(!s)
    return 0;

  if(s->sig || !s->block)
    return s->sig;

  h = (icHeader*) s->block;

  s->sig = oyValueCSpaceSig( h->colorSpace );
  return s->sig;
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
 *  since: Oyranos: version 0.1.8
 */
void
oyProfileSetChannelNames              ( oyProfile_s      * profile,
                                        oyObject_s      ** names_chan )
{
  oyProfile_s * c = profile;
  int n = oyProfileGetChannelsCount( profile );

  if(names_chan && n)
  {
    int i = 0;
    c->names_chan = c->oy->allocateFunc( (n + 1 ) * sizeof(oyObject_s*) );
    c->names_chan[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
        c->names_chan[i] = oyObjectCopy( names_chan[i] );
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
 *  since: Oyranos: version 0.1.8
 */
const oyChar *
oyProfileGetChannelName               ( oyProfile_s      * profile,
                                        int                pos,
                                        oyNAME_e           type )
{
  oyProfile_s * s = profile;
  int n = oyProfileGetChannelsCount( s );
  const oyChar * text = 0;

  if(!s)
    return 0;

  if( 0 <= pos && pos < n )
    return "-";

  if(!s->names_chan)
    oyProfileGetChannelNames ( s );

  if(s->names_chan && s->names_chan[pos])
    text = oyObjectGetName( s->names_chan[pos], type );

  return text;
}

/** @brief get channel names
 *
 *  @param[in] colour   address of a Oyranos named colour structure
 *  @param[in] names    pointer to channel names 
 *
 *  since: Oyranos: version 0.1.8
 */
const oyObject_s **
oyProfileGetChannelNames              ( oyProfile_s      * profile )
{
  oyProfile_s * s = profile;
  int n = oyProfileGetChannelsCount( profile );
  int error = 0;
  icColorSpaceSignature sig = oyProfileGetSignature( profile );

  if(!profile)
    return 0;

  if(!s->names_chan && n)
  {
    int i = 0;
    s->names_chan = s->oy->allocateFunc( (n + 1 ) * sizeof(oyObject_s*) );
    if(!s->names_chan)
      error = 1;
    if(!error)
    {
      s->names_chan[ n ] = NULL;
      for( ; i < n; ++i )
      {
        s->names_chan[i] = oyObjectCopy( s->oy );
        if(!s->names_chan[i])
          error = 1;
        else
          error = !oyObjectSetNames( s->names_chan[i],
                     oyColourSpaceGetChannelNameFromSig ( sig, i, oyNAME_NICK ),
                     oyColourSpaceGetChannelNameFromSig ( sig, i, oyNAME_NAME ),
                     oyColourSpaceGetChannelNameFromSig ( sig, i, oyNAME_DESCRIPTION )
                      );
      }
    }
  }

  if(!error && s->names_chan)
    return (const oyObject_s**) s->names_chan;
  else
    return 0;
}

/** @internal
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  since: Oyranos: version 0.1.8
 */
int
oySizeofDatatype (oyDATATYPE_e t)
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         n = 1;
    case oyUINT16:
    case oyHALF:
         n = 2;
    case oyUINT32:
    case oyFLOAT:
         n = 4;
    case oyDOUBLE:
         n = 8;
  }
  return n;
}

/** @internal
 *  @brief describe a images channel and pixel layout
 *
 *  since: Oyranos: version 0.1.8
 */
int*
oyCombinePixelLayout2Mask_ ( int           pixel_layout,
                             oyImage_s   * image,
                             oyProfile_s * profile )
{
  int n     = oyT_CHANNELS_M( pixel_layout );
  int coff_x = oyT_COLOUR_OFFSET_M( pixel_layout );
  oyDATATYPE_e t = oyT_DATATYPE_M( pixel_layout );
  int swap  = oyT_SWAP_COLOUR_CHANNELS_M( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  int *mask = image->oy->allocateFunc( sizeof(int) * (oyCHAN0 + n + 1) );
  int error = !mask;
  int so = oySizeofDatatype( t );
  int w = image->width;
  int cchan_n = 0;
  int i;

  if(!profile)
    return 0;

  cchan_n = oyProfileGetChannelsCount( profile );

  if(!error)
  {
    memset( mask, 0, sizeof(mask) );
    if(oyT_PLANAR_M( pixel_layout ))
      mask[oyPOFF_X] = 1;
    else
      mask[oyPOFF_X] = n;
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
  return mask;
}

/** @brief collect infos about a image 

    @param[in]    width        image width
    @param[in]    height       image height
    @param[in]    channels     pointer to the data buffer
    @param[in]    pixel_layout i.e. oyTYPE_123_16 for 16-bit RGB data
    @param[in]    y_offset     offset between two pixel rows
    @param[in]    profile      colour space description

 *
 *  since: Oyranos: version 0.1.8
 */
oyImage_s *    oyImageCreate          ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        int               pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s      * object)
{
  /* ---- start of common object constructor ----- */
# define OBJECT_TYPE oyOBJECT_TYPE_IMAGE_S
# define STRUCT_TYPE oyImage_s
  int error = 0;
  oyObject_s  * s_obj = oyObjectCopy_( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = OBJECT_TYPE;

  s->oy = s_obj;

  error = !oyObjectSetParent( s_obj, OBJECT_TYPE, (oyPointer)s );
# undef OBJECT_TYPE
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->width = width;
  s->height = height;
  s->data = channels;
  s->profile = oyProfileCopy( profile, 0 );

  s->layout_ = oyCombinePixelLayout2Mask_ ( pixel_layout, s, profile );

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
 *  since: Oyranos: version 0.1.8
 */
oyImage_s *    oyImageCreateForDisplay( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        int               pixel_layout,
                                        const char      * display_name,
                                        int               display_pos_x,
                                        int               display_pos_y,
                                        oyObject_s      * object)
{
  oyImage_s * s = oyImageCreate( width, height, channels, pixel_layout,
                                 0, object );
  int error = !s;

  if(!error)
  {
    oyProfileRelease( &s->profile );
    s->profile = oyProfileFromFile (0,0,0);
    if(!s->profile)
      error = 1;

    s->display_pos_x = display_pos_x;
    s->display_pos_y = display_pos_y;
  }

  return s;
}

/** @brief copy a image
 *
 *  since: Oyranos: version 0.1.8
 */
oyImage_s *    oyImageCopy_           ( oyImage_s       * image,
                                        oyObject_s      * object )
{
  oyImage_s * s = 0;

  if(!image)
    return s;

  s = image;
  /* @todo implement copy */

  return s;
}

/** @brief copy a image
 *
 *  since: Oyranos: version 0.1.8
 */
oyImage_s *    oyImageCopy            ( oyImage_s       * image,
                                        oyObject_s      * object )
{
  oyImage_s * s = 0;

  if(image && !object)
  {
    s = image;
    s->oy->ref++;
    return s;
  }

  s = oyImageCopy_( image, object );
  /* TODO cache */
  return s;
}

/** @brief release a image
 *
 *  since: Oyranos: version 0.1.8
 */
void           oyImageRelease         ( oyImage_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyImage_s * s = 0;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( !s->oy || s->type != oyOBJECT_TYPE_IMAGE_S)
  {
    WARNc_S(("Attampt to release a non oyImage_s object."))
    return;
  }

  if(--s->oy->ref > 0)
    return;
  /* ---- end of common object destructor ------- */

  s->width = 0;
  s->height = 0;
  s->data = 0;

  oyProfileRelease( &s->profile );

  if(s->oy->deallocateFunc)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy->deallocateFunc;

    if(s->layout_)
      deallocateFunc( s->layout_ ); s->layout_ = 0;

    oyObjectRelease( &s->oy );

    deallocateFunc( s );
  }

  *obj = 0;

  return;
}



/** @nrief create and possibly precalculate a transform for a given image

 *  @param[in]  cmm    zero or a cmm
 *  @param[in]  list   multi profiles, images should have already one profile
 *  @param[in]  opts   conversion opts
 *  @param[in]  in     input image
 *  @param[in]  out    output image
 *  @return            conversion
 *
 *  since: Oyranos: version 0.1.8
 */
oyColourConversion_s* oyColourConversionCreate (
                                        oyProfileList_s *list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s      * object)
{
  void    *dso_handle;
  oyPointer (*oyCMMcreateTransform)( oyPointer, int,
                                     oyPointer, int, int, int);
  void      (*oyCMMreleaseTransform)( oyPointer );
  oyPointer ptr = 0;

  /* ---- start of common object constructor ----- */
# define OBJECT_TYPE oyOBJECT_TYPE_COLOUR_CONVERSION_S
# define STRUCT_TYPE oyColourConversion_s
  int error = 0;
  oyObject_s  * s_obj = oyObjectCopy_( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = OBJECT_TYPE;

  s->oy = s_obj;

  error = !oyObjectSetParent( s_obj, OBJECT_TYPE, (oyPointer)s );
# undef OBJECT_TYPE
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  dso_handle = dlopen("liblcms.so", RTLD_LOCAL | RTLD_LAZY);

  *(void **)(&oyCMMcreateTransform) = dlsym(dso_handle, "cmsCreateTransform");
  *(void **)(&oyCMMreleaseTransform) = dlsym(dso_handle, "cmsDeleteTransform");

  ptr = (*oyCMMcreateTransform)(0,0,0,0,0,0);
  oyObjectSetCMMPtr (s->oy, "lcms", ptr, oyCMMreleaseTransform);

  s->image_in = oyImageCopy( in, 0 );
  s->image_out = oyImageCopy( out, 0 );

  return s;
}

int             oyColourConversionRun ( oyColourConversion_s *colour )
{
  int error = 0;
  return error;
}

void        oyColourConversionRelease ( oyColourConversion_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyColourConversion_s * s = 0;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( !s->oy || s->type != oyOBJECT_TYPE_COLOUR_CONVERSION_S)
  {
    WARNc_S(("Attampt to release a non oyColourConversion_s object."))
    return;
  }

  if(--s->oy->ref > 0)
    return;
  /* ---- end of common object destructor ------- */

  oyImageRelease( &s->image_in );
  oyImageRelease( &s->image_out );
  oyObjectSetCMMPtr (s->oy, "lcms", 0, 0);

  if(s->oy->deallocateFunc)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy->deallocateFunc;

    if(s->profiles)
      deallocateFunc( s->profiles ); s->profiles = 0;

    oyObjectRelease( &s->oy );

    deallocateFunc( s );
  }

  *obj = 0;

  return;
}



/** @brief manage complex oyNamedColour_s inside Oyranos
 *
 *  since: Oyranos: version 0.1.8
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
oyNamedColourCreate ( oyObject_s        * object,
                      const double      * chan,
                      const char        * blob,
                      int                 blob_len,
                      oyProfile_s * ref)
{
  int n = 0;
  /* ---- start of common object constructor ----- */
# define OBJECT_TYPE oyOBJECT_TYPE_NAMED_COLOUR_S
# define STRUCT_TYPE oyNamedColour_s
  int error = 0;
  oyObject_s  * s_obj = oyObjectCopy_( object );
  STRUCT_TYPE * s = (STRUCT_TYPE*)s_obj->allocateFunc(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(("MEM Error."))
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = OBJECT_TYPE;

  s->oy = s_obj;

  error = !oyObjectSetParent( s_obj, OBJECT_TYPE, (oyPointer)s );
# undef OBJECT_TYPE
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  n = oyProfileGetChannelsCount( s->profile );
  if(n)
    s->channels = s->oy->allocateFunc( n * sizeof(double) );
  oyCopyColour( chan, &s->channels[0], 1, ref, n );

  s->oy = oyObjectSetNames( s->oy,
                             oyObjectGetName (object, oyNAME_NICK),
                             oyObjectGetName (object, oyNAME_NAME),
                             oyObjectGetName (object, oyNAME_DESCRIPTION) );

  s->profile   = oyProfileCopy( ref, s->oy );

  if(blob && blob_len)
  {
    s->blob = s->oy->allocateFunc( blob_len );
    if(!s->blob) error = 1;
    error = !memcpy( s->blob, blob, blob_len );
  }

  return s;
}

/** @brief copy
 *
 *  
 *
 *  @param[in] colour    address of Oyranos colour struct
 *  @return              copy
 *
 *  since: Oyranos: version 0.1.8
 */
oyNamedColour_s*
oyNamedColourCopy  ( const oyNamedColour_s * colour,
                     oyObject_s        * object )
{
  oyNamedColour_s * c = NULL;

  if(!colour)
    return c;

  c = oyNamedColourCreate( object,
                     colour->channels,
                     colour->blob,
                     colour->blob_len,
                     colour->profile );

  return c;
}


/*  @return                 pointer to D50 Lab doubles with L 0.0...1.0 a/b -1.27...1.27 */

int
oyNamedColourSetSpace ( oyNamedColour_s   * colour,
                        oyProfile_s * profile )
{
  WARNc_S(("not implemented"));
  return 1;
}

int
oyNamedColourConvert  ( oyNamedColour_s   * colour,
                        oyProfile_s * profile,
                        oyPointer           buf,
                        oyDATATYPE_e        buf_type )
{                        
  oyImage_s * in  = NULL,
            * out = NULL;
  oyColourConversion_s * conv = NULL;
  int ret = 0;
  oyProfile_s * p_in = colour->profile;

  in    = oyImageCreate( 1,1, 
                         (oyPointer) colour->channels ,
                         oyCHANNELS_M(oyProfileGetChannelsCount(p_in)) |
                          oyDATATYPE_M(oyDOUBLE),
                         p_in,
                         0 );
  out   = oyImageCreate( 1,1, 
                         buf ,
                         oyCHANNELS_M(oyProfileGetChannelsCount(profile)) |
                          oyDATATYPE_M(buf_type),
                         profile,
                         0 );

  conv = oyColourConversionCreate( 0,0, in,out, 0 );
  ret  = oyColourConversionRun( conv );
  oyColourConversionRelease( &conv );
  return ret;
}

int
oyNamedColourConvertStd(oyNamedColour_s * colour,
                        oyPROFILE_e       colour_space,
                        oyPointer         buf,
                        oyDATATYPE_e      buf_type )
{                        
  int ret = 0;
  oyProfile_s * profile;

  if(!colour)
    return 1;

  profile = oyProfileFromStd ( colour_space, NULL );
  if(!profile)
    return 1;

  ret = oyNamedColourConvert  ( colour, profile, buf, buf_type );
  oyProfileRelease ( &profile );

  return ret;
}

/** @brief get colour channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @return                 pointer channels
 *
 *  since: Oyranos: version 0.1.8
 */
const double*
oyNamedColourGetChannelsConst ( oyNamedColour_s * colour )
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->channels;
  else
    return l;
}

/** @brief set colour channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @param[in]  channels    pointer to channel data
 *
 *  since: Oyranos: version 0.1.8
 */
void
oyNamedColourSetChannels ( oyNamedColour_s  * colour,
                           const double     * channels )
{
  int i, n;
  if(!colour)
    return;

  n = oyProfileGetChannelsCount( colour->profile );
  if(channels)
    for(i = 0; i < n; ++i)
      colour->channels[i] = channels[i];
}



/** @brief release correctly
 *
 *  set pointer to zero
 *
 *  @param[in]    address of Oyranos colour struct pointer
 *
 *  since: Oyranos: version 0.1.8
 */
void
oyNamedColourRelease( oyNamedColour_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyNamedColour_s * s = 0;

  if(!obj || !*obj)
    return;

  s = *obj;

  if( !s->oy || s->type != oyOBJECT_TYPE_NAMED_COLOUR_S)
  {
    WARNc_S(("Attampt to release a non oyNamedColour_s object."))
    return;
  }

  if(--s->oy->ref > 0)
    return;
  /* ---- end of common object destructor ------- */

  oyProfileRelease( &s->profile );

  if(s->oy->deallocateFunc)
  {
    oyDeAllocFunc_t deallocateFunc = s->oy->deallocateFunc;

    if(s->channels)
      deallocateFunc( s->channels ); s->channels = 0;

    if(s->blob) /* s->blob_len */
      deallocateFunc( s->blob ); s->blob = 0; s->blob_len = 0;

    oyObjectRelease( &s->oy );

    deallocateFunc( s );
  }

  *obj = 0;

  return;
}



const char *  oyCMMGetActual          ( int               flags )
{
  return "lcms";
}


/** @brief MSB<->LSB */
icUInt16Number
oyValueUInt16 (icUInt16Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
# define BYTES 2
# define KORB  4
  unsigned char        *temp  = (unsigned char*) &val;
  static unsigned char  korb[KORB];
  int i;
  for (i = 0; i < KORB ; i++ )
    korb[i] = (int) 0;  /* empty */

  {
  int klein = 0,
      gross = BYTES - 1;
  for (; klein < BYTES ; klein++ ) {
    korb[klein] = temp[gross--];
#   ifdef DEBUG_ICCFUNKT
    cout << klein << " "; DBG_PROG
#   endif
  }
  }

  {
  unsigned int *erg = (unsigned int*) &korb[0];

# ifdef DEBUG_ICCFUNKT
# if 0
  cout << *erg << " Groesse nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG_PROG
# else
  cout << *erg << " size after conversion " << (int)temp[0] << " " << (int)temp[1]
       << " "; DBG_PROG
# endif
# endif
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

  static unsigned char  uint32[4];

  uint32[0] = temp[3];
  uint32[1] = temp[2];
  uint32[2] = temp[1];
  uint32[3] = temp[0];

  {
  unsigned int *erg = (unsigned int*) &uint32[0];

# ifdef DEBUG_ICCFUNKT
  cout << *erg << " size after conversion " << (int)temp[0] << " "
       << (int)temp[1] << " " << (int)temp[2] << " " <<(int)temp[3]
       << " "; DBG_PROG
# endif

  return (icUInt32Number) *erg;
  }
#else
# ifdef DEBUG_ICCFUNKT
  cout << "BIG_ENDIAN" << " "; DBG_PROG
# endif
  return (icUInt32Number)val;
#endif
}

unsigned long
oyValueUInt64 (icUInt64Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned char        *temp  = (unsigned char*) &val;

  static unsigned char  uint64[8];
  int little = 0,
      big    = 8;

  for (; little < 8 ; little++ ) {
    uint64[little] = temp[big--];
  }

  {
  unsigned long *erg = (unsigned long*) &uint64[0];

# ifdef DEBUG_ICCFUNKT
  cout << *erg << " size after conversion " << (int)temp[0] << " "
       << (int)temp[1] << " " << (int)temp[2] << " " <<(int)temp[3]
       << " "; DBG_PROG
# endif
  return (long)*erg;
  }
#else
  return (long)val;
#endif
}

#define icValue_to_icUInt32Number(funkname, typ) \
typ \
funkname (typ val) \
{ \
  icUInt32Number i = (typ) val; \
  return (typ) oyValueUInt32 (i); \
}

icValue_to_icUInt32Number( oyValueCSpaceSig, icColorSpaceSignature )
icValue_to_icUInt32Number( oyValuePlatSig, icPlatformSignature )
icValue_to_icUInt32Number( oyValueProfCSig, icProfileClassSignature )
icValue_to_icUInt32Number( oyValueTagSig, icTagSignature )


