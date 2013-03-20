#include <string.h>

#include "oyranos_core.h"
#include "oyranos_definitions.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_module.h"
#include "oyranos_object.h"
#include "oyranos_string.h"

/** @internal
 *  @brief   test a boolean operator
 *
 *  The function requires to receive proper object arguments and valid ranges.
 *  @todo test
 *
 *  @return                            -1 for undefined, 1 - true, 2 - both,
 *                                     0 - false
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/26 (Oyranos: 0.1.9)
 *  @date    2008/11/26
 */
int          oyTextboolean_          ( const char        * text_a,
                                       const char        * text_b,
                                       oyBOOLEAN_e         type )
{
  int erg = -1;
  int error = !text_a || !text_b ||
              0 > type || type > oyBOOLEAN_UNION;

  if(error <= 0)
  {
    if(type == oyBOOLEAN_INTERSECTION)
    {
      if(text_a && text_b)
        erg = (oyStrcmp_( text_a, text_b ) == 0) ? 2 : 0;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_SUBSTRACTION)
    {
      if(text_a && text_b)
        erg = oyStrcmp_( text_a, text_b ) != 0;
      else if(text_a)
        erg = 1;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_DIFFERENZ)
    {
      if(text_a && text_b)
        erg = oyStrcmp_( text_a, text_b ) != 0;
      else if(text_a || text_b)
        erg = 1;
      else
        erg = 0;
    }
    if(type == oyBOOLEAN_UNION)
    {
      if(text_a || text_b)
        erg = 2;
      else
        erg = 0;
    }
  }

  return erg;
}

/** @internal
 *  Function oyCMMapiNumberToChar
 *  @brief   convert a oyOBJECT_e to a char
 *
 *  @param         api_number          object type
 *  @return                            number as char or zero
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/02 (Oyranos: 0.1.10)
 *  @date    2009/06/02
 */
char   oyCMMapiNumberToChar          ( oyOBJECT_e          api_number )
{   
         if(api_number == oyOBJECT_CMM_API4_S)
      return '4';
    else if(api_number == oyOBJECT_CMM_API5_S)
      return '5';
    else if(api_number == oyOBJECT_CMM_API6_S)
      return '6';
    else if(api_number == oyOBJECT_CMM_API7_S)
      return '7';
    else if(api_number == oyOBJECT_CMM_API8_S)
      return '8';
    else if(api_number == oyOBJECT_CMM_API9_S)
      return '9';
  return 0;
}


/** Function oyFilterRegistrationToSTextField
 *  @brief   analyse registration string
 *
 *  @param         registration        registration string to analyse
 *  @param[in]     field               kind of answere in return, only one field
 *  @param[out]    len                 text length
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/05/27
 */
char *         oyFilterRegistrationToSTextField (
                                       const char        * registration,
                                       oyFILTER_REG_e      field,
                                       int               * len )
{
  char  * text = 0, * erg = 0, * t = 0;
  int     texts_n = 0;
  int pos = 0, single = 0;

  if(registration)
  {
    text = (char*) registration;

         if(field & oyFILTER_REG_TOP)
      pos = 1;
    else if(field & oyFILTER_REG_DOMAIN)
      pos = 2;
    else if(field & oyFILTER_REG_TYPE)
      pos = 3;
    else if(field & oyFILTER_REG_APPLICATION)
      pos = 4;
    else if(field & oyFILTER_REG_OPTION)
      pos = 5;
    else if(field & oyFILTER_REG_MAX)
      pos = 6;

    if(field == oyFILTER_REG_TOP ||
       field == oyFILTER_REG_DOMAIN ||
       field == oyFILTER_REG_TYPE ||
       field == oyFILTER_REG_APPLICATION ||
       field == oyFILTER_REG_OPTION ||
       field == oyFILTER_REG_MAX)
      single = 1;

    texts_n = oyStringSegments_( registration, OY_SLASH_C );
    if(texts_n >= pos && field == oyFILTER_REG_TOP)
    {
      erg = oyStringSegment_( text, OY_SLASH_C, 0, len );

      /** We can not allow attributes in the oyFILTER_TOP_TYPE section, as this
       *  would conflict with the Elektra namespace policy. */
      t = oyStrnchr_( erg, '.', *len );
      if(t)
      {
        text = oyStringCopy_( erg, oyAllocateFunc_ );
        text[*len] = 0;
        /* i18n hint: a string "with '.' is not allowed" */
        WARNc3_S( "oyFILTER_TOP_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        goto clean;
      }
    }
    if(texts_n >= pos && field == oyFILTER_REG_DOMAIN)
      erg = oyStringSegment_( text, OY_SLASH_C, 1, len );
    if(texts_n >= pos && field == oyFILTER_REG_TYPE)
    {
      erg = oyStringSegment_( text, OY_SLASH_C, 2, len );

      /** We can not allow attributes in the oyFILTER_REG_TYPE section, as this
       *  would conflict with robust module cache lookup. */
      t = oyStrnchr_( erg, '.', *len );
      if(t)
      {
        text = oyStringCopy_( erg, oyAllocateFunc_ );
        text[*len] = 0;
        WARNc3_S( "oyFILTER_REG_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        goto clean;
      }
    }
    if(texts_n >= pos && field == oyFILTER_REG_APPLICATION)
      erg = oyStringSegment_( text, OY_SLASH_C, 3, len );
    if(texts_n >= pos && field == oyFILTER_REG_OPTION)
      erg = oyStringSegment_( text, OY_SLASH_C, 4, len );
#if 0
    if(erg && field == oyFILTER_REG_OPTION)
    {
      erg = oyStrnchr_( erg, '.', len );
      if(erg)
        erg[0] = 0;
    }
#endif
    /** oyFILTER_REG_MAX returns the last level which is the key name. */
    if(field == oyFILTER_REG_MAX)
      erg = oyStringSegment_( text, OY_SLASH_C, texts_n-1, len );


    /** For several oyFILTER_REG bits we compose a new registration string. */
    if(!single && field)
    {
      WARNc_S("Multiple fields are not allowed.\n"
              "  Use oyFilterRegistrationToText instead!");
    } else if( !field )
    {
      if(erg)
        WARNc1_S("text variable should be zero, found: %s", erg);
    }
  }

  return erg;

  clean:
  if(text)
    oyDeAllocateFunc_( text );
  return 0;
}


/** Function oyFilterRegistrationToText
 *  @brief   analyse registration string
 *
 *  @param         registration        registration string to analyse
 *  @param[in]     fields              kind of answere in return
 *  @param[in]     allocateFunc        use this or Oyranos standard allocator
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/05/27
 */
char *         oyFilterRegistrationToText (
                                       const char        * registration,
                                       oyFILTER_REG_e      fields,
                                       oyAlloc_f           allocateFunc )
{
  char  * text = 0, * tmp = 0;
  int     texts_n = 0,
          single = 0, len = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(registration)
  {
    if(fields == oyFILTER_REG_TOP ||
       fields == oyFILTER_REG_DOMAIN ||
       fields == oyFILTER_REG_TYPE ||
       fields == oyFILTER_REG_APPLICATION ||
       fields == oyFILTER_REG_OPTION ||
       fields == oyFILTER_REG_MAX)
      single = 1;

#if USE_OLD_STRING_API
    int pos = 0;
         if(fields & oyFILTER_REG_TOP)
      pos = 1;
    else if(fields & oyFILTER_REG_DOMAIN)
      pos = 2;
    else if(fields & oyFILTER_REG_TYPE)
      pos = 3;
    else if(fields & oyFILTER_REG_APPLICATION)
      pos = 4;
    else if(fields & oyFILTER_REG_OPTION)
      pos = 5;
    else if(fields & oyFILTER_REG_MAX)
      pos = 6;

    char ** texts = oyStringSplit_( registration, OY_SLASH_C, &texts_n,oyAllocateFunc_);
    if(texts_n >= pos && fields == oyFILTER_REG_TOP)
    {
      text = oyStringCopy_( texts[0], allocateFunc );

      /** We can not allow attributes in the oyFILTER_TOP_TYPE section, as this
       *  would conflict with the Elektra namespace policy. */
      tmp = oyStrchr_( text, '.' );
      if(tmp)
      {
        /* i18n hint: a string "with '.' is not allowed" */
        WARNc3_S( "oyFILTER_TOP_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        return 0;
      }
    }
    if(texts_n >= pos && fields == oyFILTER_REG_DOMAIN)
      text = oyStringCopy_( texts[1], allocateFunc );
    if(texts_n >= pos && fields == oyFILTER_REG_TYPE)
    {
      text = oyStringCopy_( texts[2], allocateFunc );

      /** We can not allow attributes in the oyFILTER_REG_TYPE section, as this
       *  would conflict with robust module cache lookup. */
      tmp = oyStrchr_( text, '.' );
      if(tmp)
      {
        WARNc3_S( "oyFILTER_REG_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), text, registration );
        return 0;
      }
    }
    if(texts_n >= pos && fields == oyFILTER_REG_APPLICATION)
      text = oyStringCopy_( texts[3], allocateFunc );
    if(texts_n >= pos && fields == oyFILTER_REG_OPTION)
      text = oyStringCopy_( texts[4], allocateFunc );
    if(text && fields == oyFILTER_REG_OPTION)
    {
      tmp = oyStrchr_( text, '.' );
      if(tmp)
        tmp[0] = 0;
    }
    /** oyFILTER_REG_MAX returns the last level which is the key name. */
    if(fields == oyFILTER_REG_MAX)
      text = oyStringCopy_( texts[texts_n-1], allocateFunc );

    oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
#else
    if(single)
      tmp = oyFilterRegistrationToSTextField ( registration, fields, &len );
    if(tmp)
    {
      text = allocateFunc( len + 1 );
      memcpy( text, tmp, len );
      text[len] = 0;
    }
#endif

    /** For several oyFILTER_REG bits we compose a new registration string. */
    if(!single && fields)
    {
      text = 0;

      if(fields & oyFILTER_REG_TOP)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_TOP, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_DOMAIN)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_DOMAIN, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_TYPE)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_TYPE, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_APPLICATION)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_APPLICATION, 0 );
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(fields & oyFILTER_REG_OPTION)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_OPTION, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      } else
        STRING_ADD( text, "/" );

      if(texts_n > 5 && fields & oyFILTER_REG_MAX)
      {
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_OPTION, 0);
        STRING_ADD( text, tmp );
        oyFree_m_(tmp);
      }

      tmp = text; 
      text = oyStringCopy_( tmp, allocateFunc );
      oyFree_m_(tmp);

    } else if( !fields )
    {
      if(text)
        WARNc1_S("text variable should be zero, found: %s", text);
      text = oyStringCopy_( registration, allocateFunc );
    }
  }

  return text;
}


#if USE_OLD_STRING_API
/** Function oyFilterRegistrationMatch
 *  @brief   analyse registration string and compare with a given pattern
 *
 *  The rules are described in the @ref module_api overview.
 *  The function is intensively used.
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern or key name to compare with
 *  @param         api_number          select object type
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/04/13
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  char ** reg_texts = 0;
  int     reg_texts_n = 0;
  char ** regc_texts = 0;
  int     regc_texts_n = 0;
  int     regc_texts_pos[3] = {0,0,0};
  char  * reg_text = 0;
  char ** p_texts = 0;
  int     p_texts_n = 0;
  char ** pc_texts = 0;
  int     pc_texts_n = 0;
  char  * pc_text = 0;
  int     match = 0, match_tmp = 0, i,j,k, api_num = oyOBJECT_NONE, pc_api_num;
  char    pc_match_type = '+';
  char  * key_tmp = 0,
        * max_segment = 0;
 
  if(registration && pattern)
  {
    api_num = oyCMMapiNumberToChar(api_number);
    match_tmp = 1;
    reg_texts = oyStringSplit_( registration, OY_SLASH_C, &reg_texts_n,
                                oyAllocateFunc_);
    p_texts = oyStringSplit_( pattern, OY_SLASH_C, &p_texts_n, oyAllocateFunc_);

    for( i = 0; i < reg_texts_n && i < p_texts_n; ++i)
    {
      regc_texts_n = 0;

      /* allow a key only in *pattern to filter from *registration */
      if(p_texts_n == 1)
      {
        key_tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_MAX,0);
        regc_texts = oyStringSplit_( key_tmp,'.',&regc_texts_n,
                                     oyAllocateFunc_);
        /*if(oyStringSegment_(key_tmp, '.', &regc_texts_n, &regc_texts_pos,
                            &max_segment ))
          return 0;*/
        oyFree_m_( key_tmp );
        pc_texts = oyStringSplit_( p_texts[i],'.',&pc_texts_n, oyAllocateFunc_);
        i = reg_texts_n;
      } else
      /* level by level comparision */
      {
        regc_texts = oyStringSplit_( reg_texts[i],'.',&regc_texts_n,
                                     oyAllocateFunc_);
        pc_texts = oyStringSplit_( p_texts[i],'.',&pc_texts_n, oyAllocateFunc_);
      }

      if(match_tmp && pc_texts_n && regc_texts_n)
      {
        for( j = 0; j < pc_texts_n; ++j)
        {
          match_tmp = 0;
          pc_api_num = 0;
          pc_match_type = '+';
          pc_text = pc_texts[j];

          if(pc_text[0] == '4' ||
             pc_text[0] == '5' ||
             pc_text[0] == '6' ||
             pc_text[0] == '7')
          {
            pc_api_num = pc_text[0];
            ++ pc_text;
            pc_match_type = pc_text[0];
            ++ pc_text;
          }

          for( k = 0; k < regc_texts_n; ++k )
          {
            reg_text = regc_texts[k];
            if((!pc_api_num || (pc_api_num && api_num == pc_api_num)) &&
               oyStrcmp_( reg_text, pc_text ) == 0)
            {
              if(pc_match_type == '+' ||
                 pc_match_type == '_')
              {
                ++ match;
                match_tmp = 1;
              } else /* if(pc_match_type == '-') */
                goto clean_up;
            }
          }

          if(pc_match_type == '+' && !match_tmp)
            goto clean_up;
        }
      }

      oyStringListRelease_( &pc_texts, pc_texts_n, oyDeAllocateFunc_ );
      oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    }
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &p_texts, p_texts_n, oyDeAllocateFunc_ );
  }

  if(match_tmp == 1 && !match)
    match = 1;

  return match;


  clean_up:
    oyStringListRelease_( &pc_texts, pc_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );
    oyStringListRelease_( &p_texts, p_texts_n, oyDeAllocateFunc_ );
  return 0;
}
#else
/** Function oyFilterRegistrationMatch 
 *  @brief   analyse registration string and compare with a given pattern
 *
 *  The rules are described in the @ref module_api overview.
 *  The function is intensively used.
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern or key name to compare with
 *  @param         api_number          select object type
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2009/07/16
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  char  * reg_text = 0;
  int     reg_n = 0;
  int     reg_len = 0;
  char  * regc_text = 0;
  int     regc_n = 0;
  int     regc_len = 0;
  char  * p_text = 0;
  int     p_n = 0;
  int     p_len = 0;
  char  * pc_text = 0;
  int     pc_n = 0;
  int     pc_len = 0;
  int     match = 0, match_tmp = 0, i,j,k, api_num = oyOBJECT_NONE, pc_api_num;
  char    pc_match_type = '+';
 
  if(registration && pattern)
  {
    api_num = oyCMMapiNumberToChar(api_number);
    match_tmp = 1;
    reg_n = oyStringSegments_(registration, OY_SLASH_C);
    p_n = oyStringSegments_(pattern, OY_SLASH_C);

    for( i = 0; i < reg_n && i < p_n; ++i)
    {
      regc_n = 0;

      /* allow a key only in *pattern to filter from *registration */
      if(p_n == 1)
      {
        reg_text = oyFilterRegistrationToSTextField( registration,
                                                    oyFILTER_REG_MAX, &reg_len);
        regc_n = oyStringSegmentsN_( reg_text, reg_len, '.' );
        /*if(oyStringSegment_(key_tmp, '.', &regc_n, &regc_texts_pos,
                              &max_segment ))
          return 0;*/
        p_text = oyStringSegment_( (char*)pattern, OY_SLASH_C, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, '.' );
        i = reg_n;
      } else
      /* level by level comparision */
      {
        reg_text = oyStringSegment_( (char*)registration,
                                        OY_SLASH_C, i, &reg_len );
        regc_n = oyStringSegmentsN_( reg_text, reg_len, '.' );
        p_text = oyStringSegment_( (char*)pattern, OY_SLASH_C, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, '.' );
      }

      if(match_tmp && pc_n && regc_n)
      {
        for( j = 0; j < pc_n; ++j)
        {
          match_tmp = 0;
          pc_api_num = 0;
          pc_match_type = '+';
          pc_text = oyStringSegmentN_( p_text, p_len, '.', j, &pc_len );

          if(pc_text[0] == '4' ||
             pc_text[0] == '5' ||
             pc_text[0] == '6' ||
             pc_text[0] == '7' ||
             pc_text[0] == '8' ||
             pc_text[0] == '9')
          {
            pc_api_num = pc_text[0];
            ++ pc_text;
            pc_match_type = pc_text[0];
            ++ pc_text;
            pc_len -= 2;
          } else
          if(pc_text[0] == '_' ||
             pc_text[0] == '-')
          {
            pc_match_type = pc_text[0];
            ++ pc_text;
            -- pc_len;
          }

          for( k = 0; k < regc_n; ++k )
          {
            regc_text = oyStringSegmentN_( reg_text, reg_len, '.', k,
                                              &regc_len );
            if(regc_text[0] == '4' ||
               regc_text[0] == '5' ||
               regc_text[0] == '6' ||
               regc_text[0] == '7' ||
               regc_text[0] == '8' ||
               regc_text[0] == '9')
            {
              ++ regc_text;
              ++ regc_text;
              regc_len -= 2;
            } else
            if(regc_text[0] == '_' ||
               regc_text[0] == '-')
            {
              ++ regc_text;
              -- regc_len;
            }

            if((!pc_api_num || (pc_api_num && api_num == pc_api_num)) &&
               memcmp( regc_text, pc_text, OY_MIN(regc_len,pc_len) ) == 0 &&
	       (regc_len == pc_len || !regc_len || !pc_len))
            {
              if(pc_match_type == '+' ||
                 pc_match_type == '_')
              {
                ++ match;
                match_tmp = 1;
              } else /* if(pc_match_type == '-') */
                return 0;
            }
          }

          if(pc_match_type == '+' && !match_tmp)
            return 0;
        }
      }
    }
  }

  if(match_tmp == 1 && !match)
    match = 1;

  return match;
}
#endif

/** Function oyFilterRegistrationMatchKey
 *  @brief   compare two registration strings, skip key attributes
 *
 *  The rules are described in the @ref module_api overview.
 *  The rules in this function map especially to key storage rules for 
 *  Oyranos DB.
 *
 *  The non key part is handled as namespace and should match in order to
 *  consider the two keys from the same namespace.
 *  The second condition is the key name is stripped from all attributes.
 *  This means the string part after the last slash '/' is taken into account
 *  only until the first point '.' or end of string appears.
 *
 *  @param         registration_a      registration key
 *  @param         registration_b      registration key
 *  @param         api_number          select object type
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/03 (Oyranos: 0.1.10)
 *  @date    2010/06/03
 */
int    oyFilterRegistrationMatchKey  ( const char        * registration_a,
                                       const char        * registration_b,
                                       oyOBJECT_e          api_number )
{
  char * tmp1 = 0,
       * tmp2 = 0,
       * tmp = 0;
  int match = 0;

  if(registration_a && oyStrrchr_( registration_a, '/' ))
    tmp1 = oyStringCopy_( registration_a, oyAllocateFunc_ );
  if(registration_b && oyStrrchr_( registration_b, '/' ))
    tmp2 = oyStringCopy_( registration_b, oyAllocateFunc_ );

  if(!tmp1 || !tmp2)
    return match;

  if(tmp1)
  {
    tmp = oyStrrchr_( tmp1, '/' );
    tmp[0] = 0;
  }
  if(tmp2)
  {
    tmp = oyStrrchr_( tmp2, '/' );
    tmp[0] = 0;
  }

  match = oyFilterRegistrationMatch( tmp1, tmp2, api_number );

  if(match)
  {
    oyFree_m_(tmp1);
    oyFree_m_(tmp2);
    tmp1 = oyFilterRegistrationToText( registration_a,
                                       oyFILTER_REG_OPTION, 0 );
    tmp = oyStrchr_( tmp1, '.' );
    if(tmp)
      tmp[0] = 0;
    tmp2 = oyFilterRegistrationToText( registration_b,
                                       oyFILTER_REG_OPTION, 0 );
    tmp = oyStrchr_( tmp2, '.' );
    if(tmp)
      tmp[0] = 0;

    if(oyStrcmp_( tmp1, tmp2 ) == 0)
      ++match;
    else
      match = 0;

    oyFree_m_(tmp1);
    oyFree_m_(tmp2);
  }

  return match;
}

/** Function oyTextIccDictMatch
 *  @brief   analyse a string and compare with a given pattern
 *
 *  The rules are described in the ICC meta tag dict type at
 *  http://www.color.org    ICCSpecRevision_25-02-10_dictType.pdf
 *
 *  @param         text                value string
 *  @param         pattern             pattern to compare with
 *  @param         delta               say how far a difference can drift
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.3.3
 *  @since   2010/11/21 (Oyranos: 0.1.3)
 *  @date    2011/12/29
 */
int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern,
                                       double              delta )
{
  int match = 0;
  int n = 0, p_n = 0, i, j;
  char ** texts = 0, * t;
  char ** patterns = 0, * p;
  long num[2] = {0,0};
  int num_valid[2] = {0,0};
  double dbl[2] = {0,0};
  int dbl_valid[2] = {0,0}; 

  DBG_MEM_START

  if(text && pattern)
  {
    texts = oyStringSplit_(text, ',', &n, oyAllocateFunc_ );
    patterns = oyStringSplit_(pattern, ',', &p_n, oyAllocateFunc_ );

    for( i = 0; i < n; ++i)
    {
      t = texts[i];
      DBG_MEM3_S( "%d: "OY_PRINT_POINTER" \"%s\"", i, (intptr_t)t, t );
      num_valid[0] = !oyStringToLong(t,&num[0]);
      dbl_valid[0] = !oyStringToDouble(t,&dbl[0]);
      DBG_MEM
      for( j = 0; j < p_n; ++j )
      {
        p = patterns[j];
        DBG_MEM4_S( "%d %d: "OY_PRINT_POINTER" \"%s\"", i, j, (intptr_t)t, p );
        num_valid[1] = !oyStringToLong(p,&num[1]);
        dbl_valid[1] = !oyStringToDouble(p,&dbl[1]);
        DBG_MEM

        if((strcmp( t, p ) == 0) ||
           (num_valid[0] && num_valid[1] && num[0] == num[1]) ||
           (dbl_valid[0] && dbl_valid[1] && fabs(dbl[0] - dbl[1])/2.0 < delta))
        {
          match = 1;
          goto clean_oyTextIccDictMatch;
        }
      }
    }
    clean_oyTextIccDictMatch:
      oyStringListRelease_( &texts, n, oyDeAllocateFunc_ );
      oyStringListRelease_( &patterns, p_n, oyDeAllocateFunc_ );
  }

  DBG_MEM_ENDE
  return match;
}

/** @internal
 *  @brief   wrapper for oyDeAllocateFunc_
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
int oyPointerRelease                 ( oyPointer         * ptr )
{
  if(ptr && *ptr)
  {
    oyDeAllocateFunc_(*ptr);
    *ptr = 0;
    return 0;
  }
  return 1;
}

const char *       oyConnectorEventToText (
                                       oyCONNECTOR_EVENT_e e )
{
  const char * text = "unknown";
  switch(e)
  {
    case oyCONNECTOR_EVENT_OK: text = "oyCONNECTOR_EVENT_OK: kind of ping"; break;
    case oyCONNECTOR_EVENT_CONNECTED: text = "oyCONNECTOR_EVENT_CONNECTED: connection established"; break;
    case oyCONNECTOR_EVENT_RELEASED: text = "oyCONNECTOR_EVENT_RELEASED: released the connection"; break;
    case oyCONNECTOR_EVENT_DATA_CHANGED: text = "oyCONNECTOR_EVENT_DATA_CHANGED: call to update image views"; break;
    case oyCONNECTOR_EVENT_STORAGE_CHANGED: text = "oyCONNECTOR_EVENT_STORAGE_CHANGED: new data accessors"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_DATA: can not process data"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION: can not handle option"; break;
    case oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT: text = "oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT: can not handle context"; break;
    case oyCONNECTOR_EVENT_INCOMPLETE_GRAPH: text = "oyCONNECTOR_EVENT_INCOMPLETE_GRAPH: can not completely process"; break;
  }
  return text;
}

int oyCheckType_( oyOBJECT_e type1, oyOBJECT_e type2 )
{
  int error = 0;

  if(type1 != type2)
    error = 1;

  return error;
}

