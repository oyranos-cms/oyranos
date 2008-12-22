/** @file oyranos_cmm_oyIM_profile.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    backends for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/12/16
 */

#include "config.h"
#include "oyranos_cmm_oyIM.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* --- implementations --- */

/** @func  oyIMProfileCanHandle
 *  @brief inform about icTagTypeSignature capabilities
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2008/05/23
 */
int        oyIMProfileCanHandle      ( oyCMMQUERY_e      type,
                                       uint32_t          value )
{
  int ret = -1;

  switch(type)
  {
    case oyQUERY_OYRANOS_COMPATIBILITY:
         ret = OYRANOS_VERSION; break;
    case oyQUERY_PROFILE_FORMAT:
         if(value == 1)
           ret = 1;
         else
           ret = 0;
         break;
    case oyQUERY_PROFILE_TAG_TYPE_READ:
         switch(value) {
         case icSigMakeAndModelType:
         case icSigMultiLocalizedUnicodeType:
         case icSigWCSProfileTag:
         case icSigProfileSequenceDescType:
         case icSigProfileSequenceIdentifierType:
         case icSigSignatureType:
         case icSigTextDescriptionType:
         case icSigTextType:
              ret = 1; break;
         default: ret = 0; break;
         }
         break;
    case oyQUERY_PROFILE_TAG_TYPE_WRITE:
         switch(value) {
         case icSigMultiLocalizedUnicodeType:
         case icSigProfileSequenceIdentifierType:
         case icSigTextDescriptionType:
         case icSigTextType:
              ret = 1; break;
         default: ret = 0; break;
         }
         break;
    default: break;
  }

  return ret;
}

/** @func    oyIMProfileTag_GetValues
 *  @brief   get values from ICC profile tags
 *
 *  The function implements oyCMMProfileTag_GetValues_t for 
 *  oyCMMapi3_s::oyCMMProfileTag_GetValues.
 *
 *  - function description
 *    - set the tag argument to zero
 *    - the returned list will be filled in with oyName_s' each matching a tag_type
 *      - oyNAME_NICK contains the module info, e.g. 'oyIM'
 *      - oyNAME_NAME contains the tag_type, e.g. 'icSigMultiLocalizedUnicodeType' or 'mluc'
 *      - oyNAME_DESCRIPTION contains text as in above documentation
 *    - dont copy the list as content may be statically allocated
 *
 *  The output depends on the tags type signature in tag->tag_type_ as follows:
 *
 *  - icSigTextType and icSigWCSProfileTag:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns one string
 *
 *  - icSigTextDescriptionType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns one string
 *
 *  - icSigMultiLocalizedUnicodeType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - list: will contain oyName_s objects
 *      - oyName_s::name will hold the name
 *      - oyName_s::lang will hold i18n specifier, e.g. \"en_GB\"
 *
 *  - icSigSignatureType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns one string
 *    - for the value see oyICCTechnologyDescription
 *
 *  - icSigMakeAndModelType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns four strings each originating from a uint32_t
 *      - manufacturer id
 *      - model id
 *      - serialNumber id
 *      - manufacturer date id
 *
 *  - icSigProfileSequenceDescType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns
 *      - first string as ascii the number (i) of the found elements
 *      - a profile anounce string in 1 + i * 7
 *      - the translated "Manufacturer:" string in 1 + i * 7 + 1
 *      - the manufacturer string in 1 + i * 7 + 2, the full lenght or 4 byte
 *      - the translated "Model:" string in 1 + i * 7 + 3
 *      - the model string in 1 + i * 7 + 4, the full lenght or 4 byte one
 *      - the translated "Technology:" string in 1 + i * 7 + 5
 *      - the tech string in 1 + i * 7 + 6, see oyICCTechnologyDescription
 *
 *  - icSigProfileSequenceIdentifierType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - returns
 *      - first string as ascii the number (i) of the found elements
 *      - a profile anounce string in 1 + i * 5
 *      - the string "md5id:" in in 1 + i * 5 + 1
 *      - the low letter hexadecimal hash value in 1 + i * 5 + 2
 *      - mluc translated by oyICCTagDescription in 1 + i * 5 + 3
 *      - the icSigProfileDescriptionTag according to language in 1 + i * 5 + 4
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/05/23
 */
oyStructList_s * oyIMProfileTag_GetValues(
                                       oyProfileTag_s    * tag )
{
  oyStructList_s * values = 0;
  icUInt32Number error = 0, len = 0, count = 0;
  oyStructList_s * texts = 0, * temp = 0;
  char * tmp = 0;
  char * mem = 0;
  char * pos = 0;
  icTagBase * tag_base = 0;
  icTagTypeSignature  sig = 0;
  int32_t size_ = -1;
  char num[32];
  oyName_s * name = 0;

  /* provide information about the function */
  if(!tag)
  {
    oyStructList_s * list = oyStructList_New( 0 );
    oyName_s description_mluc = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "mluc",
      "\
- icSigMultiLocalizedUnicodeType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: will contain oyName_s objects\
    - oyName_s::name will hold the name\
    - oyName_s::lang will hold i18n specifier, e.g. \"en_GB\""
    };
    oyName_s description_psid = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "psid",
      "\
- icSigProfileSequenceIdentifierType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: will contain oyName_s objects\
    - first string as ascii the number (i) of the found elements\
    - a profile anounce string in 1 + i * 5\
    - the string \"md5id:\" in in 1 + i * 5 + 1\
    - the low letter hexadecimal hash value in 1 + i * 5 + 2\
    - mluc translated by oyICCTagDescription in 1 + i * 5 + 3\
    - the icSigProfileDescriptionTag according to language in 1 + i * 5 + 4"
    };
    oyName_s description_MS10 = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "MS10",
      "\
- icSigWCSProfileTag:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyName_s"
    };
    oyName_s description_text = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "text",
      "\
- icSigTextType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyName_s"
    };
    oyName_s description_desc = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "desc",
      "\
- icSigTextDescriptionType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyName_s"
    };
    oyStruct_s * description = 0;

    description = (oyStruct_s*) &description_mluc;
    error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_psid;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_MS10;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_text;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_desc;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    return list;
  }

  texts = oyStructList_New(0);
  temp = oyStructList_New(0);

  error = !texts || ! temp;

  if(!error && tag->status_ == oyOK)
  {
    tag_base = tag->block_;
    mem = tag->block_;
    sig = tag->tag_type_;

    error = !mem || !tag->size_ > 12;

    if(!error)
    switch( (uint32_t)sig )
    {
      case icSigTextType:

           len = tag->size_ * sizeof(oyChar);
           tmp = oyAllocateFunc_( len );
           error = !memcpy( tmp, &mem[8], len - 8 );

           while (strchr(tmp, 13) > (char*)0) { /* \r 013 0x0d */
             pos = strchr(tmp, 13);
             if (pos > (char*)0) {
               if (*(pos+1) == '\n')
                 *pos = ' ';
               else
                 *pos = '\n';
             }
             count++;
           };
           if(!error)
           {
             oyStructList_MoveInName( texts, &tmp, -1 );
             size_ = len;
           }
           break;
      case icSigWCSProfileTag:
           len = tag->size_ * sizeof(oyChar);
           tmp = oyAllocateFunc_( len*2 + 1 );

           {
                 int  dversatz = 8 + 24;

                 len = len - dversatz;

                 if(!error)
                 {
                   /* WCS provides UTF-16LE */
                   error = oyIMIconv( &mem[dversatz], len, tmp, "UTF-16LE" );

                   if(error != 0 || !oyStrlen_(tmp))
                   {
                     error = 1;
                     oyDeAllocateFunc_(tmp); tmp = 0;
                   }
                 }
           }

           if(!error)
           while (strchr(tmp, 13) > (char*)0) { /* \r 013 0x0d */
             pos = strchr(tmp, 13);
             if (pos > (char*)0) {
               if (*(pos+1) == '\n')
                 *pos = ' ';
               else
                 *pos = '\n';
             }
             count++;
           };

           if(!error)
           {
             size_ = oyStrlen_(tmp);
             oyStructList_MoveInName( texts, &tmp, -1 );
           }
           break;
      case icSigTextDescriptionType:
           count = *(icUInt32Number*)(mem+8);
           count = oyValueUInt32( count );

           if((int)count > tag->size_- 20)
           {
             int diff = count - tag->size_ - 20;
             char nt[128];
             char * txt = oyAllocateFunc_( tag->size_ );
             snprintf( nt, 128, "%d", diff );

             oyStructList_AddName( texts,
                                   _("Error in ICC profile tag found!"),
                                   -1 );
             
             oyStructList_AddName( texts,
                                 " Wrong \"desc\" tag count. Difference is :",
                                   -1 );
             oyStringAdd_( &tmp, "   ", oyAllocateFunc_, oyDeAllocateFunc_);
             oyStringAdd_( &tmp, nt, oyAllocateFunc_, oyDeAllocateFunc_);
             oyStructList_MoveInName( texts, &tmp, -1 );
             oyStructList_AddName( texts,
                                 " Try ordinary tag length instead (?):",
                                   -1 );
             
             oyStringAdd_( &tmp, "  ",
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             error = !memcpy (txt, &mem[12], tag->size_ - 12);
             txt[ tag->size_ - 12 ] = 0;
             oyStringAdd_( &tmp, txt, oyAllocateFunc_, oyDeAllocateFunc_);
             oyStructList_MoveInName( texts, &tmp, -1 );
           }
           else
           {
             tmp = oyAllocateFunc_(count + 1);
             memset(tmp, 0, count + 1);
             error = !memcpy(tmp, mem+12, count);
             tmp[count] = 0;
             oyStructList_MoveInName( texts, &tmp, -1 );
           }

           {
             uint32_t off = 0, n_ascii = 0, n_uni16 = 0;

               /* 'desc' type */
               off += 8;

               /* ascii in 'desc' */
               if(off < tag->size_)
               {
                 len = *(uint32_t*)&mem[off];
                 n_ascii = oyValueUInt32( len );

                 off += 4;
                 off += n_ascii;
                 /*off += (off%4 ? 4 - off%4 : 0);*/
               }

               /* unicode section in 'desc' */
               if(off < tag->size_)
               {
                 off += 4;

                 len = *(icUInt32Number*)&mem[off];
                 n_uni16 = oyValueUInt32( len );
                 off += 4 + n_uni16*2 - 1;
               }
               /* script in 'desc' */
               if(off < tag->size_)
               {
                 len = *(icUInt32Number*)&mem[off];
                 len = oyValueUInt32( len );
                 off += 4 + 67;
               }
             size_ = off;
           }
           break;
      case icSigMultiLocalizedUnicodeType:
           {
             int anzahl = oyValueUInt32( *(icUInt32Number*)&mem[8] );
             int groesse = oyValueUInt32( *(icUInt32Number*)&mem[12] ); /* 12 */
             int i;
             int all = 1;

             error = tag->size_ < 24 + anzahl * groesse;

             if(!error)
             for (i = 0; i < anzahl; i++)
             {
               char c = mem[16+ i*groesse],
                    d = mem[17+ i*groesse];
               int  g = 0,
                    dversatz = 0;

               error = tag->size_ < 20 + i * groesse;
               if(!error)
                 g = oyValueUInt32( *(icUInt32Number*)&mem[20+ i*groesse] );

               {
                 oyName_s * name = 0;
                 oyStruct_s * oy_struct = 0;
                 char * t = 0;

                 error = tag->size_ < 20 + i * groesse + g + 4;
                 if(!error)
                 {
                   len = (g > 1) ? g : 8;
                   t = (char*) oyAllocateFunc_(len*4);
                   error = !t;
                 }

                 if(!error && all)
                 {
                   name = oyName_new(0);
                   oySprintf_( name->lang, "%c%c_%c%c", c, d,
                               mem[18+ i*groesse], mem[19+ i*groesse] );
                 }

                 if(!error)
                   t[0] = 0;

                 if(!error)
                   error = (24 + i*groesse + 4) > tag->size_;

                 if(!error)
                   dversatz = oyValueUInt32( *(icUInt32Number*)&mem
                                                  [24+ i*groesse] );

                 if(!error)
                   error = dversatz + len > tag->size_;

                 if(!error)
                 {
                   /* ICC says UTF-16BE */
                   error = oyIMIconv( &mem[dversatz], len, t, "UTF-16BE" );

                   oy_struct = (oyStruct_s*) name;
                   /* eigther text or we have a non translatable string */
                   if(oyStrlen_(t) || oyStructList_Count(texts))
                   {
                     name->name = t;
                     oyStructList_MoveIn( texts, &oy_struct, -1 );
                   } else
                     name->release(&oy_struct);
                 }
               }

               if(i == anzahl-1 && !error)
               {
                 if(!error)
                   error = (24 + i*groesse + 4) > tag->size_;

                 dversatz = oyValueUInt32( *(icUInt32Number*)&mem
                                                  [24+ i*groesse] );
                 size_ = dversatz + g;
               }
             }

             if (!oyStructList_Count(texts)) /* first entry */
             {
               int g =        oyValueUInt32(*(icUInt32Number*)&mem[20]),
                   dversatz = oyValueUInt32(*(icUInt32Number*)&mem[24]);
               char * t = 0;
               int n_;

               error = tag->size_ < dversatz + g;

               if(!error)
                 t = (char*) oyAllocateFunc_( g + 1 );
               error = !t;

               if(!error)
               {
                 for (n_ = 1; n_ < g ; n_ = n_+2)
                   t[n_/2] = mem[dversatz + n_];
                 t[n_/2] = 0;
                 oyStringAdd_( &tmp, t, oyAllocateFunc_, oyDeAllocateFunc_);
                 oyStructList_MoveInName( texts, &tmp, -1 );
                 oyFree_m_( t );
               }
             }
           }

           break;
      case icSigSignatureType:
           if (tag->size_ < 12)
           { return texts; }
           else
           {
             icTechnologySignature tech;
             const char * t =  0;

             error = !memcpy (&tech, &mem[8] , 4);
             tech = oyValueUInt32( tech );
             t = oyICCTechnologyDescription( tech );

             size_ = 8 + 4;

             tmp = oyAllocateFunc_(5);
             error = !memcpy (tmp, &mem[8] , 4);
             tmp[4] = 0;
             oyStructList_MoveInName( texts, &tmp, -1 );
             tmp = oyStringAppend_( 0, t, oyAllocateFunc_ );
             oyStructList_MoveInName( texts, &tmp, -1 );
           }
           break;
      case icSigMakeAndModelType:
           if(tag->size_ < 40)
           { return texts; }
           else
           {
             uint32_t val = 0, i;

             for(i = 0; i < 4; ++i)
             {
               val = oyValueUInt32( (uint32_t)*((uint32_t*)&mem[8 + i*4]) );
               oySprintf_(num, "%u              ", *((uint32_t*)&mem[8 + i*4]));
               error = !memcpy (&num[16], &mem[8 + i*4], 4);
               oyStructList_AddName( texts, num, -1 );
             }
             size_ = 8 + 32;
           }
           break;
      case icSigProfileSequenceDescType:
           if(tag->size_ > 12 + 20 + sizeof(icTextDescription)*2)
           {
             int off = 8;
             uint32_t i=0;
             icDescStruct * desc = 0;
             const char * mfg = 0;
             const char * model = 0;
             const char * tech = 0;
             oyStructList_s * mfg_tmp = 0, * model_tmp = 0;
             oyProfileTag_s * tmptag = 0;
             int32_t size = -1;
             icTagSignature tag_sig = (icTagSignature)0;

             count = *(icUInt32Number*)(mem+off);
             count = oyValueUInt32( count );
             off += 4;
#if 0
             len = *(icUInt32Number*)(mem+off);
             len = oyValueUInt32( len );
             off += 4;
#endif

             oySprintf_(num, "%d", count);
             oyStringAdd_( &tmp, num, oyAllocateFunc_, oyDeAllocateFunc_);
             oyStructList_MoveInName( texts, &tmp, -1 );

             if(count > 256) count = 256;
             for(i = 0; i < count; ++i)
             {
               if(tag->size_ > off + sizeof(icDescStruct))
                 desc = (icDescStruct*) &mem[off];

               off += 4+4+2*4+4;
               if(off < tag->size_)
               {

                 oySprintf_(num, "%d", i);
                 oyStringAdd_( &tmp, "profile[",
                                      oyAllocateFunc_, oyDeAllocateFunc_);
                 oyStringAdd_( &tmp, num, oyAllocateFunc_, oyDeAllocateFunc_);
                 oyStringAdd_( &tmp, "]:", oyAllocateFunc_, oyDeAllocateFunc_);
                 oyStructList_MoveInName( texts, &tmp, -1 );

                 mfg = oyICCTagName( oyValueUInt32(desc->deviceMfg) );
                 model = oyICCTagName( oyValueUInt32(desc->deviceModel) );
                 tech = oyICCTechnologyDescription( oyValueUInt32(desc->technology ));
               }

               /* first mnf */
               tmptag = oyProfileTag_New(0);
               tmp = oyAllocateFunc_(tag->size_ - off);
               error = !memcpy(tmp, &mem[off], tag->size_ - off);
               tag_sig = *(icUInt32Number*)(tmp);
               tag_sig = oyValueUInt32( tag_sig );
               oyProfileTag_Set( tmptag, icSigDeviceMfgDescTag,
                                         tag_sig, oyOK,
                                         tag->size_ - off, tmp );
               mfg_tmp = oyIMProfileTag_GetValues( tmptag );
               if(oyStructList_Count( mfg_tmp ) )
               {
                 name = 0;
                 name = (oyName_s*) oyStructList_GetRefType( mfg_tmp,
                                                   0, oyOBJECT_NAME_S );
                 if(name)
                   mfg = name->name;
               }
               oyProfileTag_Release( &tmptag );
               tmp = 0;

               if(size > 0)
                 off += size;

               /* next model */
               tmptag = oyProfileTag_New(0);
               tmp = oyAllocateFunc_(tag->size_ - off);
               error = !memcpy(tmp, &mem[off], tag->size_ - off);
               tag_sig = *(icUInt32Number*)(tmp);
               tag_sig = oyValueUInt32( tag_sig );
               oyProfileTag_Set( tmptag, icSigDeviceModelDescTag,
                                         tag_sig, oyOK,
                                         tag->size_ - off, tmp );
               mfg_tmp = oyIMProfileTag_GetValues( tmptag );
               if(oyStructList_Count( model_tmp ) )
               {
                 name = 0;
                 name = (oyName_s*) oyStructList_GetRefType( model_tmp,
                                                   0, oyOBJECT_NAME_S );
                 if(name)
                   model = name->name;
               }
               oyProfileTag_Release( &tmptag );
               tmp = 0;

               if(size > 0)
                 off += size;

               /* write to string */
               if(mfg && oyStrlen_(mfg))
               {
                 oyStructList_AddName( texts, _("Manufacturer:"), -1 );
                 oyStructList_AddName( texts, mfg, -1 );
               } else
               {
                 oyStructList_AddName( texts, 0, -1 );
                 oyStructList_AddName( texts, 0, -1 );
               }
               if(model && oyStrlen_(model))
               {
                 oyStructList_AddName( texts, _("Modell:"), -1 );
                 oyStructList_AddName( texts, model, -1 );
               } else
               {
                 oyStructList_AddName( texts, 0, -1 );
                 oyStructList_AddName( texts, 0, -1 );
               }
               if(tech && oyStrlen_(tech))
               {
                 oyStructList_AddName( texts, _("Technology:"), -1 );
                 oyStructList_AddName( texts, tech, -1 );
               } else
               {
                 oyStructList_AddName( texts, 0, -1 );
                 oyStructList_AddName( texts, 0, -1 );
               }

               oyStructList_Release( &mfg_tmp );
               oyStructList_Release( &model_tmp );
             }
             size_ = off;
           }
           break;
      case icSigProfileSequenceIdentifierType:
           /*
                ICC Votable Proposal Submission
                Profile Sequence Identifier Tag

		Proposer: Manish Kulkarni, Adobe Systems Inc.
		Date: November 27, 2006
		Proposal Version: 1.0
            */
           if(tag->size_ > 12)
           {
             int32_t off = 0;
             int i;
             int offset = 0, old_offset = 0;
             int size = 0;
             int mluc_size = 0;
             uint32_t * hash = 0;
             oyStructList_s * desc_tmp = 0;
             int desc_tmp_n = 0;
             oyProfileTag_s * tmptag = 0;

             off += 8;

             count = *(icUInt32Number*)(mem+off);
             count = oyValueUInt32( count );
             off += 4;

             if(count > 256) count = 256;

             oySprintf_(num, "%d", count);
             oyStructList_AddName( texts, num, -1 );

             for(i = 0; i < count; ++i)
             {
               oySprintf_(num, "%d", i);
               oyStringAdd_( &tmp, "profile[",
                                      oyAllocateFunc_, oyDeAllocateFunc_);
               oyStringAdd_( &tmp, num, oyAllocateFunc_, oyDeAllocateFunc_);
               oyStringAdd_( &tmp, "]:", oyAllocateFunc_, oyDeAllocateFunc_);
               oyStructList_MoveInName( texts, &tmp, -1 );

               if(!error && 12 + i*8 + 8 < tag->size_)
               {
                 /* implicite offset and size */
                 len = *(icUInt32Number*)&mem[12 + i*8 + 0];
                 offset = oyValueUInt32( len );
                 len = *(icUInt32Number*)&mem[12 + i*8 + 4];
                 size = oyValueUInt32( len );
               }

               if(!error)
                 error = offset + size < old_offset + 16 + mluc_size;

               if(!error && offset + size <= tag->size_)
               {
                 hash = (uint32_t*)&mem[offset];
                 tmp = oyAllocateFunc_(80);
                 error = !tmp;
                 oySprintf_(tmp, "%x%x%x%x",hash[0], hash[1], hash[2], hash[3]);
                 oyStructList_AddName( texts, "md5id:", -1 );
                 oyStructList_MoveInName( texts, &tmp, -1 );

                 old_offset = offset;

                 offset += 16;

                 /* 'mluc' type - desc */
                 tmptag = oyProfileTag_New(0);
                 tmp = oyAllocateFunc_(tag->size_ - offset);
                 error = !memcpy(tmp, &mem[offset], tag->size_ - offset);
                 oyProfileTag_Set( tmptag, icSigProfileDescriptionTag,
                                           icSigMultiLocalizedUnicodeType, oyOK,
                                           tag->size_ - offset, tmp );
                 tmp = 0;
                 desc_tmp_n = 0;
                 desc_tmp = oyIMProfileTag_GetValues( tmptag );
                 if(oyStructList_Count( desc_tmp ) )
                 {
                   name = 0;
                   name = (oyName_s*) oyStructList_GetRefType( desc_tmp,
                                                     0, oyOBJECT_NAME_S );
                   if(name)
                     tmp = name->name;
                 }
                 oyProfileTag_Release( &tmptag );

                 if(size_ < offset + mluc_size)
                   size_ = offset + mluc_size;

                 if(!error)
                   error = size < mluc_size;

                 oyStructList_AddName( texts,
                      oyICCTagDescription(icSigMultiLocalizedUnicodeType), -1 );
                 oyStructList_MoveInName( texts, &tmp, -1 );
               } else
                 error = 1;
             }
           }
           break;
    }
  }

  values = texts;

  return values;
}


/** @func  oyIMProfileTag_Create
 *  @brief create a ICC profile tag
 *
 *  This is a module function. For usage in Oyranos 
 *  @see oyProfileTag_Create
 *
 *  The output depends on the tag type signature and arguments in list:
 *
 *  - icSigProfileSequenceIdentifierType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - list: should contain only profiles
 *    - version: is not honoured; note 'psid' is known after ICC v4.2
 *  - icSigMultiLocalizedUnicodeType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - list: should contain only names in oyName_s objects
 *      - oyName_s::name is considered to hold the name
 *      - oyName_s::lang is required to hold i18n specifier, e.g. "en_GB"
 *      - the frist oyName_s::lang can have no i18n specifier as a default
 *    - version: is not honoured; note 'mluc' is known since ICC v4
 *  - icSigTextType:
 *    - since Oyranos 0.1.8 (API 0.1.8)
 *    - list: should contain only names in oyName_s objects
 *      - oyName_s::name is considered to hold the name
 *
 *  - non supported types
 *    - the tag->status_ field will be set to oyUNDEFINED 
 *
 *  - function description
 *    - set the tag argument to zero
 *    - provide a empty list to fill in with oyName_s' each matching a tag_type
 *      - oyNAME_NICK contains the module info, e.g. 'oyIM'
 *      - oyNAME_NAME contains the tag_type, e.g. 'icSigMultiLocalizedUnicodeType' or 'mluc'
 *      - oyNAME_DESCRIPTION contains text as in above documentation
 *    - dont copy the list as content may be statically allocated
 *
 *  @param[in,out] tag                 the profile tag
 *  @param[in,out] list                parameters
 *  @param[in]     tag_type            the ICC tag type
 *  @param[in]     version             version as supported
 *  @return                            oySTATUS_e status
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/08 (Oyranos: 0.1.8)
 *  @date    2008/03/11
 */
int          oyIMProfileTag_Create   ( oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version )
{
  oyProfileTag_s * s = tag,
                 * tmptag = 0;
  int error = !list;
  int n = oyStructList_Count( list ),
      i = 0, mem_len = 0, tmp_len = 0, mluc_len = 0, mluc_sum = 0,
      len = 0, j = 0;
  char * mem = 0,
       * tmp = 0;
  oyProfile_s * prof = 0;
  oyStructList_s * tmp_list = 0,
                 * tag_list = 0;
  oyName_s * string = 0;

  /* provide information about the function */
  if(!error && !s)
  {
    oyName_s description_mluc = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "mluc",
      "\
- icSigMultiLocalizedUnicodeType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only names in oyName_s objects\
    - oyName_s::name is considered to hold the name\
    - oyName_s::lang is required to hold i18n specifier, e.g. \"en_GB\"\
    - the frist oyName_s::lang can have no i18n specifier as a default\
  - version: is not honoured; note 'mluc' is known since ICC v4"
    };
    oyName_s description_psid = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "psid",
      "\
- icSigProfileSequenceIdentifierType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyProfile_s\
  - version: is not honoured; note 'psid' is known after ICC v4.2"
    };
    oyName_s description_text = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "text",
      "\
- icSigTextType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyName_s\
  - version: is not honoured"
    };
    oyName_s description_desc = {
      oyOBJECT_NAME_S, 0,0,0,
      CMM_NICK,
      "desc",
      "\
- icSigTextDescriptionType:\
  - since Oyranos 0.1.8 (API 0.1.8)\
  - list: should contain only oyName_s\
  - version: is not honoured; note 'desc' is a pre ICC v4.0 tag"
    };
    oyStruct_s * description = 0;

    description = (oyStruct_s*) &description_mluc;
    error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_psid;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_text;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    description = (oyStruct_s*) &description_desc;
    if(!error)
      error = oyStructList_MoveIn( list, &description, -1 );

    return error;
  }

  if(!error)
  switch((uint32_t)tag_type)
  {
    case icSigMultiLocalizedUnicodeType:
       {
         size_t size = 0;
         /*      base   #  size  lang len off */
         mluc_len = 8 + 4 + 4 + (2+2 + 4 + 4) * n;
         /*             8  12   16    20  24 */

         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
               tmp_len = strlen( string->name );
             error = !tmp_len;
             
             if(i)
               error = !string->lang;

             len = tmp_len * 2 + 4;
             mluc_len += len + (len%4 ? len%4 : 0);
           }
         }
         printf("%s:%d mluc_len: %d\n",__FILE__,__LINE__, mluc_len);

         if(!error)
           mem = oyStruct_Allocate( (oyStruct_s*)s, mluc_len );

         error = !mem;

         if(!error)
         {
           *((uint32_t*)&mem[8]) = oyValueUInt32( n );
           *((uint32_t*)&mem[12]) = oyValueUInt32( 12 );
           mem_len += 16 + n*12;
         }

         if(!error)
         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
               tmp_len = strlen( string->name );
             error = !tmp_len;

             if(i)
               error = !string->lang;
           }

           if(!error)
           {
               if(string->lang && oyStrlen_(string->lang))
               {
                 if(strlen(string->lang) >= 2)
                   memcpy( &mem[16+i*12 + 0], string->lang, 2 );
                 if(strlen(string->lang) > 4)
                   memcpy( &mem[16+i*12 + 2], &string->lang[3], 2 );
               }

               *((uint32_t*)&mem[16+i*12 + 4]) = oyValueUInt32( tmp_len * 2 );
               *((uint32_t*)&mem[16+i*12 + 8]) = oyValueUInt32( mem_len );
           }

           if(!error)
           {
#if 0
             /* broken with glibc-2.3.3 */
             size = mbstowcs( (wchar_t*)&mem[mem_len], string->name,
                              tmp_len );
#else
             size = tmp_len;
             for(j = 0; j < tmp_len; ++j)
               mem[mem_len+2*j+1] = string->name[j];
#endif

             error = (size != tmp_len);

             if(!error)
             {
               len = tmp_len * 2 + 4;
               mem_len += len + (len%4 ? len%4 : 0);
             }
           }
         }

         if(error || !n)
         {
           s->status_ = oyUNDEFINED;

         } else {
           oyProfileTag_Set( s, s->use, tag_type, oyOK, mem_len, mem );
         }
       }

       break;

    case icSigProfileSequenceIdentifierType:
       {
         tag_list = oyStructList_New( 0 );

         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             prof = (oyProfile_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_PROFILE_S );
             error = !prof;
           }

           if(!error)
           {
             tmptag = oyProfile_GetTagById( prof, icSigProfileDescriptionTag );
             error = !tmptag;

             oyProfile_Release( &prof );
           }

           if(!error && tmptag->tag_type_ != icSigMultiLocalizedUnicodeType)
           {
             mluc_len = 0;
             tmp_list = oyIMProfileTag_GetValues( tmptag );

             if(!error)
             {
               error = oyIMProfileTag_Create( tmptag, tmp_list,
                                         icSigMultiLocalizedUnicodeType, 0 );
               tmp = 0;

               if(!error)
                 error = tmptag->status_;
             }
           }

           if(!error)
           {
             mluc_sum += tmptag->size_;
             error = oyStructList_MoveIn( tag_list, (oyStruct_s**)&tmptag, -1 );
           }
         }

         if(!error)
         {
           mem_len = 12 + 8*n + 16*n + mluc_sum + 3*n;
           mem = oyStruct_Allocate( (oyStruct_s*)tag, mem_len );
           error = !mem;

           if(!error)
           oyProfileTag_Set( s, icSigProfileSequenceIdentifierType,
                                icSigProfileSequenceIdentifierType, oyOK,
                                mem_len, mem );

           tmp_len = 0;

           for(i = 0; i < n; ++i)
           {
             if(!error)
             {
               tmptag = (oyProfileTag_s*) oyStructList_GetRefType( tag_list,
                                               i, oyOBJECT_PROFILE_TAG_S );
               error = !tmptag;
             }

             if(!error)
             {
               int pos = 12 + 8*n + tmp_len;
               error = !memcpy( &mem[pos + 16],
                                tmptag->block_, tmptag->size_ );
               *((uint32_t*)&mem[12 + 8*i + 0]) = oyValueUInt32( pos );

               prof = (oyProfile_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_PROFILE_S );
               error = !prof || !prof->block_ || !prof->size_;
               error = oyProfileGetMD5( prof->block_, prof->size_,
                                        (unsigned char*)&mem[pos] );
               oyProfile_Release( &prof );

               len = 16 + tmptag->size_;
               *((uint32_t*)&mem[12 + 8*i + 4]) = oyValueUInt32( len );
               tmp_len += len + (len%4 ? len%4 : 0);
             }
             oyProfileTag_Release( &tmptag );
           }

           if(!error)
             *((uint32_t*)&mem[8]) = oyValueUInt32( n );
         }
       }
       break;

    case icSigTextType:
       {
         mem_len = 8;

         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
               mem_len += strlen( string->name ) + 1;
             error = !mem_len;
             
             len = mem_len;
             mem_len = len + (len%4 ? len%4 : 0);
           }
         }

         if(!error)
           mem = oyStruct_Allocate( (oyStruct_s*)s, mem_len );
         mem[0] = 0;
         mem_len = 8;

         error = !mem;

         if(!error)
         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
               tmp_len = strlen( string->name );
             error = !tmp_len;
           }

           if(!error)
           {
             if(i)
               mem[mem_len++] = '\n';

             tmp = &mem[mem_len];
             error = !memcpy( tmp, string->name, tmp_len );
             mem_len += tmp_len;
             if(!error)
               mem[mem_len] = 0;
           }
         }

         if(error || !n)
         {
           s->status_ = oyUNDEFINED;

         } else {
           len = mem_len + 1;
           mem_len = len + (len%4 ? len%4 : 0);
           oyProfileTag_Set( s, s->use, tag_type, oyOK, mem_len, mem );
         }
       }

       break;

    case icSigTextDescriptionType:
       {
         mem_len = 8 + 4;
         tmp_len = 0;

         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
             {
               mem_len += strlen( string->name ) + 1;
               tmp_len += strlen( string->name ) + 1;
             }
             error = !mem_len;
           }
         }

         /* we are guessing here */
         len = mem_len + 8 + 8 + 67;
         len = len + (len%4 ? len%4 : 0);

         if(!error)
           mem = oyStruct_Allocate( (oyStruct_s*)s, len );
         error = !mem;

         if(!error)
         {
           memset( mem, 0, mem_len );

           *((uint32_t*)&mem[8]) = oyValueUInt32( tmp_len );

           mem_len = 8 + 4;
         }

         if(!error)
         for(i = 0; i < n; ++i)
         {
           if(!error)
           {
             string = (oyName_s*) oyStructList_GetRefType( list,
                                                   i, oyOBJECT_NAME_S );
             error = !string;
           }

           if(!error)
           {
             if(string->name)
               tmp_len = strlen( string->name );
             error = !tmp_len;
           }

           if(!error)
           {
             if(i)
               mem[mem_len++] = '\n';

             tmp = &mem[mem_len];
             error = !memcpy( tmp, string->name, tmp_len );
             mem_len += tmp_len;
             if(!error)
               mem[mem_len] = 0;
           }
         }

         if(error || !n)
         {
           s->status_ = oyUNDEFINED;

         } else {
           /*len = mem_len + 1;
           mem_len = len + (len%4 ? len%4 : 0);*/
           oyProfileTag_Set( s, s->use, tag_type, oyOK, len, mem );
         }
       }

       break;

    default:
       s->status_ = oyUNDEFINED;
       break;
  }

  if(s)
  {
    memcpy( s->last_cmm_, CMM_NICK, 4 );
    if(s->status_ == oyOK && s->block_)
      *((uint32_t*)&mem[0]) = oyValueUInt32( tag_type );
  }

  return error;
}


/** @instance oyIM_api3
 *  @brief    oyIM oyCMMapi3_s implementations
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
oyCMMapi3_s  oyIM_api3 = {

  oyOBJECT_CMM_API3_S,
  0,0,0,
  (oyCMMapi_s*) & oyIM_api5_colour_icc,
  
  oyIMCMMInit,
  oyIMCMMMessageFuncSet,
  oyIMProfileCanHandle,

  oyIMProfileTag_GetValues,
  oyIMProfileTag_Create
};


