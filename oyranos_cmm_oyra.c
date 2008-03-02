
/** @file @internal
 *  @brief   backends for Oyranos
 *  @license new BSD
 */

/** @date      02. 01. 2008 */


#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* --- internal definitions --- */

#define CMM_NICK "oyra"

int oyraCMMWarnFunc( int code, const char * format, ... );
oyMessageFunc_t message = oyraCMMWarnFunc;



/* --- implementations --- */

/** @func  oyraCMMInit
 *  @brief API requirement
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
int                oyraCMMInit       ( )
{
  int error = 0;
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

/** @func  oyraCMMWarnFunc
 *  @brief message handling
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
int oyraCMMWarnFunc( int code, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;

  va_start( list, format);
  vsprintf( text, format, list);
  va_end  ( list );

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, "WARNING"); fprintf( stderr, ": " );
         break;
    case oyMSG_ERROR:
         fprintf( stderr, "!!! ERROR"); fprintf( stderr, ": " );
         break;
  }
  fprintf( stderr, text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

/** @func  oyraCMMMessageFuncSet
 *  @brief API requirement
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
int            oyraCMMMessageFuncSet ( oyMessageFunc_t     message_func )
{
  message = message_func;
  return 0;
}


/** @func  oyraProfileCanHandle
 *  @brief inform about icTagTypeSignature capabilities
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/03 (API 0.1.8)
 */
int        oyraProfileCanHandle      ( oyCMMQUERY_e      type,
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
    case oyQUERY_PROFILE_TAG_TYPE:
         switch(value) {
         case icSigTextType:
         case icSigTextDescriptionType:
         case icSigMultiLocalizedUnicodeType: ret = 1; break;
         default: ret = 0; break;
         }
         break;
    default: break;
  }

  return ret;
}


/** @func  oyraProfileTag_GetText
 *  @brief get textual informations from ICC profile tags
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
oyChar *     oyraProfileTag_GetText  ( oyProfileTag_s    * tag,
                                       const char          language[4],
                                       const char          country[4],
                                       oyAllocFunc_t       allocateFunc )
{
  oyProfileTag_s * s = tag;
  icUInt32Number error = !s, len, count = 0;
  oyChar * text = 0;
  char * tmp = 0;
  char * mem = 0;
  char * pos = 0;
  icTagBase * tag_base = 0;
  icTagTypeSignature  sig = 0;

  if(!error && tag->status_ == oyOK)
  {
    tag_base = tag->block_;
    mem = tag->block_;
    sig = tag->tag_type_;

    switch( (uint32_t)sig )
    {
      case icSigTextType:
      case icSigCopyrightTag:

           len = tag->size_ * sizeof(oyChar);
           tmp = allocateFunc( len );
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
             text = tmp;
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

             text = oyStringAdd_( text, _("Error in ICC profile tag found!"),
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             text = oyStringAdd_( text, "\n Wrong \"desc\" tag count. Difference is :\n   ",
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             text = oyStringAdd_( text, nt,
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             text = oyStringAdd_( text, "\n Try ordinary tag length instead (?):\n   ",
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             memcpy (txt, &mem[12], tag->size_ - 12);
             txt[ tag->size_ - 12 ] = 0;
             text = oyStringAdd_( text, txt,
                                  oyAllocateFunc_, oyDeAllocateFunc_);
             oyDeAllocateFunc_(txt);
           }
           else
           {
             text = oyAllocateFunc_(count + 1);
             memset(text, 0, count + 1);
             memcpy(text, mem+12, count);
             text[count] = 0;
           }
           /* reallocate */
           tmp = text; text = 0;
           text = oyStringAppend_( 0, tmp, allocateFunc );
           oyDeAllocateFunc_( tmp );

           break;
      case icSigMultiLocalizedUnicodeType:
           break;
      case icSigMeasurementType:
           break;
    }
  }

  return text;
}

/** @func  oyraProfileTag_GetValues
 *  @brief get values from ICC profile tags
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
double *     oyraProfileTag_GetValues( oyProfileTag_s    * tag,
                                       oyAllocFunc_t       allocateFunc )
{
  double * values = 0;
  return values;
}


/**   
 *  @brief oyra oyCMMapi3_s implementations
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
oyCMMapi3_s  oyTP_api3 = {

  oyOBJECT_TYPE_CMM_API3_S,
  0,0,0,
  0,
  
  oyraCMMInit,
  oyraCMMMessageFuncSet,

  oyraProfileCanHandle,
  oyraProfileTag_GetText,
  oyraProfileTag_GetValues
};

/* CMM_NICK */
#define oyraSignature 0x6f797261

/**
 *  @brief oyra module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/02 (API 0.1.8)
 */
oyCMMInfo_s oyra_cmm_module = {

  oyOBJECT_TYPE_CMM_INFO_S,
  0,0,0,
  oyraSignature,
  "0.1",
  {oyOBJECT_TYPE_NAME_S, 0,0,0, CMM_NICK, "Oyranos modules", "Oyranos supplied modules"},
  {oyOBJECT_TYPE_NAME_S, 0,0,0, "Kai-Uwe", "Kai-Uwe Behrmann", "Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/#download"},
  {oyOBJECT_TYPE_NAME_S, 0,0,0, "new BSD", "Copyright (c) 2005-2007 Kai-Uwe Behrmann", "new BSD license: http://www.opensource.org/licenses/bsd-license.php"},
  108,

  (oyCMMapi_s*) & oyTP_api3,
  0,

  {oyOBJECT_TYPE_ICON_S, 0,0,0, 0,0,0, "oyranos_logo.png"}

};

