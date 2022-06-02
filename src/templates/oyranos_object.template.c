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
              type > oyBOOLEAN_UNION;

  if(error <= 0)
  {
    if(type == oyBOOLEAN_INTERSECTION)
      erg = (oyStrcmp_( text_a, text_b ) == 0) ? 2 : 0;
    if(type == oyBOOLEAN_SUBSTRACTION)
      erg = oyStrcmp_( text_a, text_b ) != 0;
    if(type == oyBOOLEAN_DIFFERENZ)
      erg = oyStrcmp_( text_a, text_b ) != 0;
    if(type == oyBOOLEAN_UNION)
        erg = 2;
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
        if(text)
          text[*len] = 0;
        /* i18n hint: a string "with '.' is not allowed" */
        WARNc3_S( "oyFILTER_TOP_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), oyNoEmptyString_m_(text), registration );
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
        if(text)
          text[*len] = 0;
        WARNc3_S( "oyFILTER_REG_TYPE %s: %s (%s)",
                  _("with \'.\' is not allowed"), oyNoEmptyString_m_(text), registration );
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

    if(single)
      tmp = oyFilterRegistrationToSTextField ( registration, fields, &len );
    if(tmp)
    {
      text = allocateFunc( len + 1 );
      memcpy( text, tmp, len );
      text[len] = 0;
    }

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
        tmp = oyFilterRegistrationToText( registration, oyFILTER_REG_MAX, 0);
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

/** Function oyGetKeyFromRegistration
 *  @brief   get oyFILTER_REG_MAX from registration string
 *
 *  This function is similar to oyFilterRegistrationToText() .
 *
 *  @param         registration        registration string to analyse
 *  @return                            the key name
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/03/09
 *  @since   2015/03/09 (Oyranos: 0.9.6)
 */
const char * oyGetKeyFromRegistration( const char        * registration )
{
  const char * key = strrchr( registration, OY_SLASH_C );

  if(key)
    ++key;
  else
    key = registration;
        
  return key;
}

int  oyMemCmp( const void * a, size_t an, const void * b, size_t bn )
{
  return memcmp( a, b, OY_MIN(an,bn) ) == 0;
}
int  oyMemMemFound( const void * a, size_t an, const void * b, size_t bn )
{
  void * ptr = oyMemMem(a,an,b,bn);
  if(ptr) return 1;
  else return 0;
}

/** @brief   analyse string and compare with a given pattern
 *
 *  The rules are described in the @ref module_api overview.
 *  This version is configurable
 *
 *  @param         registration        registration string to analise
 *  @param         pattern             pattern or key name to compare with
 *  @param         api_number          select object type
 *  @param         path_separator      a char to split into hierarchical levels
 *  @param         key_separator       a char to split key strings
 *  @param         flags               options:
 *                                     - OY_MATCH_SUB_STRING - find sub string;
 *                                       default is whole word match
 *                                     - OY_SYNTAX_SKIP_REG - avoid "-,_,+"
 *                                       syntax checking in registration
 *                                     - OY_SYNTAX_SKIP_PATTERN - avoid "-,_,+"
 *                                       syntax checking in pattern
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/01/05
 *  @since   2017/01/05 (Oyranos: 0.9.7)
 */
int    oyFilterStringMatch           ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number,
                                       char                path_separator,
                                       char                key_separator,
                                       int                 flags )
{
  char  * reg_text = 0;
  int     reg_n = 0;
  int     reg_len = 0;
  char  * regc_text = 0;
  char  * tempr = NULL;
  char  * tempp = NULL;
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
  int     (*memmatch) (const void *s1, size_t s1n,
                       const void *s2, size_t s2n) = oyMemCmp;

  if(flags & OY_MATCH_SUB_STRING)
    memmatch = oyMemMemFound;

  if(registration && pattern)
  {
    char * t;
    api_num = oyCMMapiNumberToChar(api_number);
    match_tmp = 1;

    if(strstr(registration, "\\."))
    {
      tempr = oyjlStringCopy(registration, oyAllocateFunc_);
      t = strstr(tempr, "\\.");
      t[0] = '\000';
      registration = tempr;
    }
    if(strstr(pattern, "\\."))
    {
      tempp = oyjlStringCopy(pattern, oyAllocateFunc_);
      t = strstr(tempp, "\\.");
      t[0] = '\000';
      pattern = tempp;
    }
 
    reg_n = oyStringSegments_(registration, path_separator);
    p_n = oyStringSegments_(pattern, path_separator);

    for( i = 0; i < reg_n && i < p_n; ++i)
    {
      regc_n = 0;

      /* allow a key only in *pattern to filter from *registration */
      if(p_n == 1)
      {
        reg_text = oyFilterRegistrationToSTextField( registration,
                                                    oyFILTER_REG_MAX, &reg_len);
        regc_n = oyStringSegmentsN_( reg_text, reg_len, key_separator );
        p_text = oyStringSegment_( (char*)pattern, path_separator, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, key_separator );
        i = reg_n;
      } else
      /* level by level comparision */
      {
        reg_text = oyStringSegment_( (char*)registration,
                                        path_separator, i, &reg_len );
        regc_n = oyStringSegmentsN_( reg_text, reg_len, key_separator );
        p_text = oyStringSegment_( (char*)pattern, path_separator, i, &p_len );
        pc_n = oyStringSegmentsN_( p_text, p_len, key_separator );
      }

      if(match_tmp && pc_n && regc_n)
      {
        for( j = 0; j < pc_n; ++j)
        {
          match_tmp = 0;
          pc_api_num = 0;
          pc_match_type = '+';
          pc_text = oyStringSegmentN_( p_text, p_len, key_separator, j, &pc_len );

          if(pc_text[0] == '4' ||
             pc_text[0] == '5' ||
             pc_text[0] == '6' ||
             pc_text[0] == '7' ||
             pc_text[0] == '8' ||
             pc_text[0] == '9')
          {
            pc_api_num = pc_text[0];
            ++ pc_text;
            pc_len --;
            if(!(flags & OY_SYNTAX_SKIP_PATTERN))
            {
              pc_match_type = pc_text[0];
              ++ pc_text;
              pc_len --;
            }
          } else
          if(!(flags & OY_SYNTAX_SKIP_PATTERN) && (
             pc_text[0] == '_' ||
             pc_text[0] == '-' ||
             pc_text[0] == '+'))
          {
            pc_match_type = pc_text[0];
            ++ pc_text;
            -- pc_len;
          }

          for( k = 0; k < regc_n; ++k )
          {
            regc_text = oyStringSegmentN_( reg_text, reg_len, key_separator, k,
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
            if(!(flags & OY_SYNTAX_SKIP_REG) && (
               regc_text[0] == '_' ||
               regc_text[0] == '-'))
            {
              ++ regc_text;
              -- regc_len;
            }

            if((!pc_api_num || (pc_api_num && api_num == pc_api_num)) &&
               memmatch( regc_text, regc_len, pc_text, pc_len ) &&
	       (flags & OY_MATCH_SUB_STRING ||
                regc_len == pc_len || !regc_len || !pc_len))
            {
              if(pc_match_type == '+' ||
                 pc_match_type == '_')
              {
                ++ match;
                match_tmp = 1;
              } else /* if(pc_match_type == '-') */
              {
                match = 0;
                goto clean_oyFilterStringMatch;
              }
            }
          }

          if(pc_match_type == '+' && !match_tmp)
          {
            match = 0;
            goto clean_oyFilterStringMatch;
          }
        }
      }
    }
  } else
    if((flags & OY_MATCH_SUB_STRING) &&
       registration && !pattern)
      match = 1;

  if(match_tmp == 1 && !match)
    match = 1;

  clean_oyFilterStringMatch:
  if(tempp) oyDeAllocateFunc_(tempp);
  if(tempr) oyDeAllocateFunc_(tempr);
  return match;
}

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
 *  @date    2009/07/16
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 */
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number )
{
  return oyFilterStringMatch( registration, pattern, api_number, '/', '.', 0 );
}

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
    goto clean_oyFilterRegistrationMatchKey;

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
                                       oyFILTER_REG_MAX, 0 );
    tmp = oyStrchr_( tmp1, '.' );
    if(tmp)
      tmp[0] = 0;
    tmp2 = oyFilterRegistrationToText( registration_b,
                                       oyFILTER_REG_MAX, 0 );
    tmp = oyStrchr_( tmp2, '.' );
    if(tmp)
      tmp[0] = 0;

    if(oyStrcmp_( tmp1, tmp2 ) == 0)
      ++match;
    else
      match = 0;
  }

  clean_oyFilterRegistrationMatchKey:
    oyFree_m_(tmp1);
    oyFree_m_(tmp2);
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
 *  @param         path_separator      ignored
 *  @param         key_separator       split text and pattern by a char;
 *                                     usually comma - ','
 *  @return                            match, useable for ranking
 *
 *  @version Oyranos: 0.9.7
 *  @date    2022/04/18
 *  @since   2010/11/21 (Oyranos: 0.1.3)
 */
int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern,
                                       double              delta,
                                       char                path_separator OY_UNUSED,
                                       char                key_separator )
{
  int match = 0;
  int n = 0, p_n = 0, i, j;
  char ** texts = 0, * t;
  char ** patterns = 0, * p;
  long num[2] = {0,0};
  int num_valid[2] = {0,0};
  const char * num_end[2] = {0,0};
  double dbl[2] = {0,0};
  int dbl_valid[2] = {0,0}; 
  const char * dbl_end[2] = {0,0};

  DBG_MEM_START

  if(text && pattern)
  {
    texts = oyStringSplit_(text, key_separator, &n, oyAllocateFunc_ );
    patterns = oyStringSplit_(pattern, key_separator, &p_n, oyAllocateFunc_ );

    for( i = 0; i < n; ++i)
    {
      t = texts[i];
      DBG_MEM3_S( "%d: "OY_PRINT_POINTER" \"%s\"", i, (intptr_t)t, t );
      num_valid[0] = oyjlStringToLong(t,&num[0],&num_end[0]) <= 0 ? 1 : 0;
      dbl_valid[0] = oyjlStringToDouble(t,&dbl[0],&dbl_end[0]) <= 0 ? 1 : 0;
      DBG_MEM
      for( j = 0; j < p_n; ++j )
      {
        p = patterns[j];
        DBG_MEM4_S( "%d %d: "OY_PRINT_POINTER" \"%s\"", i, j, (intptr_t)t, p );
        num_valid[1] = oyjlStringToLong(p,&num[1],&num_end[1]) <= 0 ? 1 : 0;
        dbl_valid[1] = oyjlStringToDouble(p,&dbl[1],&dbl_end[1]) <= 0 ? 1 : 0;
        DBG_MEM

        if(strcmp( t, p ) == 0)
        {
          match = 1;
          goto clean_oyTextIccDictMatch;
        } else if (dbl_valid[0] && dbl_valid[1])
        {
          if ((fabs(dbl[0] - dbl[1])/2.0 < delta) && (dbl_end[0] == 0 || dbl_end[1] == 0 || strcmp(dbl_end[0],dbl_end[1]) == 0))
          {
            match = 1;
            goto clean_oyTextIccDictMatch;
          }
        } else if (num_valid[0] && num_valid[1] && num[0] == num[1] && (num_end[0] == 0 || num_end[1] == 0 || strcmp(num_end[0],num_end[1]) == 0))
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

