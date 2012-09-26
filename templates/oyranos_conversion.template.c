{% include "source_file_header.txt" %}

#include "oyranos_conversion.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_string.h"
#include "oyranos_types.h"

/** Function oyFilterRegistrationModify
 *  @brief   process a registration string
 *
 *  A semantical overview is given in @ref module_api.
 *
 *  @param[in]     registration        registration key
 *  @param[in]     mode                the processing rule
 *  @param[out]    result              allocated by allocateFunc
 *  @param[in]     allocateFunc        optional user allocator; defaults to 
 *                                     oyAllocateFunc_
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/08/12 (Oyranos: 0.1.11)
 *  @date    2010/08/12
 */
char   oyFilterRegistrationModify    ( const char        * registration,
                                       oyFILTER_REG_MODE_e mode,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc )
{
  if(!result) return 1;
  if(!registration) return -1;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  switch(mode)
  {
  case oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR:
  {
    char ** reg_texts = 0;
    int     reg_texts_n = 0;
    char ** regc_texts = 0;
    int     regc_texts_n = 0;
    char  * reg_text = 0;
    int     i,j;
 
    reg_texts = oyStringSplit_( registration, OY_SLASH_C, &reg_texts_n,
                                oyAllocateFunc_);

    for( i = 0; i < reg_texts_n; ++i)
    {
      regc_texts_n = 0;

      /* level by level */
      regc_texts = oyStringSplit_( reg_texts[i],'.',&regc_texts_n,
                                   oyAllocateFunc_);

      if(i > 0)
        STRING_ADD( reg_text, OY_SLASH );

      for( j = 0; j < regc_texts_n; ++j)
      {
        /* '_' underbar is the feature char; omit such attributes */
        if(regc_texts[j][0] != '_')
        {
          if(j > 0)
            STRING_ADD( reg_text, "." );
          STRING_ADD( reg_text, regc_texts[j] );
        }
      }

      oyStringListRelease_( &regc_texts, regc_texts_n, oyDeAllocateFunc_ );
    }
    oyStringListRelease_( &reg_texts, reg_texts_n, oyDeAllocateFunc_ );


    if(allocateFunc != oyAllocateFunc_)
      *result = oyStringCopy_( reg_text, allocateFunc );
    else
      *result = reg_text;
  }
  break;

  default: return -1;
  }

  return 0;
}


