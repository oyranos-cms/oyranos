/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 * test
 * 
 */

/*  Date:      25. 11. 2004 */

#include <kdb.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_texts.h> // oyStringListRelease_

/* forward declaration for oyranos_alpha.c */
char ** oyCMMsGetNames_              ( int               * n,
                                       oyOBJECT_TYPE_e   * types,
                                       int                 types_n );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );

int
main(int argc, char** argv)
{
  int i, count = 0;
  uint32_t size = 0;
  char ** profiles = oyProfileListGet ( 0, &size, malloc ),
       ** texts = 0,
        * text = 0;
  oyProfileList_s * iccs, * patterns;
  oyProfile_s * profile, * temp_prof;
  oyCMMInfo_s * cmm_info = 0;
  oyFilter_s * filter = 0;

  for( i = 0; i < (int) size; ++i )
    printf( "%d: %s\n", i, profiles[i]);

  oyStringListRelease_( &profiles, size, free );

  profile = oyProfile_FromSignature( icSigInputClass,
                                        oySIGNATURE_CLASS, 0 );
  patterns = oyProfileList_MoveIn( 0, &profile, -1 );
  profile = oyProfile_FromSignature( icSigDisplayClass,
                                        oySIGNATURE_CLASS, 0 );
  patterns = oyProfileList_MoveIn( patterns, &profile, -1 );
  profile = oyProfile_FromSignature( icSigOutputClass,
                                        oySIGNATURE_CLASS, 0 );
  patterns = oyProfileList_MoveIn( patterns, &profile, -1 );
  profile = oyProfile_FromSignature( icSigColorSpaceClass,
                                        oySIGNATURE_CLASS, 0 );
  patterns = oyProfileList_MoveIn( patterns, &profile, -1 );

  iccs = oyProfileList_Create( patterns, 0 );

  size = oyProfileList_Count(iccs);
  for( i = 0; i < size; ++i)
  {
    temp_prof = oyProfileList_Get( iccs, i );
    printf("%d: \"%s\" %s\n", i,
                             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
                             oyProfile_GetFileName(temp_prof, 0));
    oyProfile_Release( &temp_prof );
  }

  texts = oyCMMsGetNames_(&count, 0 ,0 );
  for( i = 0; i < count; ++i)
  {
    cmm_info = oyCMMGet_( texts[i] );
    text = oyCMMInfoPrint_( cmm_info );
    printf("%d: \"%s\": %s\n", i, texts[i], text );
  }
  oyStringListRelease_( &texts, count, free );

  filter = oyFilter_New( oyFILTER_TYPE_COLOUR, "image", 0, 0 );

  return 0;
}


