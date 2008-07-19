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
#include <oyranos_texts.h> /* oyStringListRelease_ */
#include <oyranos_cmm.h>   /* for hacking into backend API */

/* forward declaration for oyranos_alpha.c */
char ** oyCMMsGetNames_              ( int               * n,
                                       oyOBJECT_TYPE_e   * types,
                                       int                 types_n );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );
char**             oyStringSplit_    ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc );


int
main(int argc, char** argv)
{
  oyPointer pixel = 0;
  oyPixelAccess_s * pixel_access = 0;
  oyConversions_s * conversions = 0;
  oyFilter_s      * filter = 0;
  int32_t result = 0;
  oyImage_s * image = 0;
  float buf[24] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
  oyProfile_s * prof = 0;
  int x,y,w,h, i;
  char * ptr = 0;
  uint32_t size = 0;

#if 0
  int count = 0;
  char ** profiles = 0,
       ** texts = 0,
        * text = 0;
  oyProfileList_s * iccs, * patterns;
  oyProfile_s * profile, * temp_prof;
  oyCMMInfo_s * cmm_info = 0;

  profiles = oyProfileListGet ( 0, &size, malloc );
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
#endif


  prof = oyProfile_FromStd( oyASSUMED_WEB, 0 );
  w = 4;
  h = 2;
  image = oyImage_Create( w, h, buf, oyTYPE_123_FLOAT, prof, 0 );

  conversions = oyConversions_CreateInput ( image, 0 );
  filter = oyFilter_New( oyFILTER_TYPE_COLOUR, "..colour.cmm.icc", 0,0, 0 );
  oyConversions_FilterAdd( conversions, filter );
  oyConversions_OutputAdd( conversions, image );
  oyImage_Release( &image );

  /* create a very simple pixel iterator */
  pixel_access = oyPixelAccess_Create( 0,0, conversions->input->filter,
                                       oyPIXEL_ACCESS_IMAGE, 0 );
  result = 0;
  while(result == 0)
  {
    float * p = 0;
    pixel = oyConversions_GetNextPixel( conversions, pixel_access, &result );
    p = pixel;

    if(result == 0)
      fprintf( stdout, "%.01f %.01f %.01f\n", p[0], p[1], p[2] );
  }

  /* turn into a line itereator */
  pixel_access->array_xy = malloc(sizeof(uint32_t) * 2);
  pixel_access->array_xy[0] = 1;
  pixel_access->array_xy[1] = 0;
  pixel_access->array_n = 1;
  pixel_access->array_cache_pixels = w;
  result = 0;
  x = y = 0;
  while(result == 0)
  {
    float * p = 0;

    pixel_access->start_xy[0] = 1;
    pixel_access->start_xy[1] = y++;
    pixel = oyConversions_GetNextPixel( conversions, pixel_access, &result );
    p = pixel;

    if(result == 0)
    for(i = 0; i < pixel_access->array_cache_pixels*3; i += 3)
      fprintf( stdout, "%.01f %.01f %.01f\n", p[i+0], p[i+1], p[i+2] );
  }

  /* itereate in chunks */
  pixel_access->array_xy = malloc(sizeof(uint32_t) * 2);
  pixel_access->array_xy[0] = 1;
  pixel_access->array_xy[1] = 0;
  pixel_access->array_n = 1;
  pixel_access->array_cache_pixels = 2;
  result = 0;
  x = y = 0;
  while(result == 0)
  {
    float * p = 0;

    pixel_access->start_xy[0] = x;
    pixel_access->start_xy[1] = y++;
    pixel = oyConversions_GetNextPixel( conversions, pixel_access, &result );
    p = pixel;

    if(result == 0)
    for(i = 0; i < pixel_access->array_cache_pixels*3; i += 3)
      fprintf( stdout, "%.01f %.01f %.01f\n", p[i+0], p[i+1], p[i+2] );
  }

  if(conversions->input->filter->api_->oyCMMFilter_ContextToMem)
    ptr = conversions->input->filter->api_->oyCMMFilter_ContextToMem( conversions->input->filter, &size, 0, malloc );

  if(ptr)
    oyWriteMemToFile_( "test_dbg.icc", ptr, size );

  oyConversions_Release( &conversions );
  oyPixelAccess_Release( &pixel_access );

  return 0;
}


