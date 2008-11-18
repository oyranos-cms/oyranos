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
#include <stdio.h>
#include <string.h>

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_texts.h> /* oyStringListRelease_ */
#include <oyranos_cmm.h>   /* for hacking into backend API */

/* forward declaration for oyranos_alpha.c */
char ** oyCMMsGetNames_              ( int               * n,
                                       oyOBJECT_e        * types,
                                       int                 types_n );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );
char**             oyStringSplit_    ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc );
int                oyWriteMemToFile_ ( const char*, void*, size_t );


int
main(int argc, char** argv)
{
  oyPointer pixel = 0;
  oyPixelAccess_s * pixel_access = 0;
  oyConversion_s * conversion = 0;
  oyFilter_s      * filter = 0;
  int32_t result = 0;
  oyImage_s * image_in = 0, * image_out = 0;
  double buf[24] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
  double buf2[24] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  double * d = 0, * dest = 0;
  oyProfile_s * prof = 0;
  int x,y,w,h, i,j;
  char * ptr = 0;
  uint32_t size = 0;
  int error = 0;

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


  w = 7;
  h = 32;
  size = sizeof(double)*w*h*3;
  d = malloc(size);
  dest = malloc(size);
  for(i = 0; i < w*h; ++i)
    for(j=0; j < 3; ++j)
      d[i*3+j] = i*3+j;

  prof = oyProfile_FromStd( oyASSUMED_WEB, 0 );
  image_in = oyImage_Create( w, h, d, OY_TYPE_123_DBL, prof, 0 );
  image_out = oyImage_Create( w, h, dest, OY_TYPE_123_DBL, prof, 0 );
  conversion = oyConversion_CreateBasic( image_in, image_out, 0, 0 );
  oyImage_Release( &image_in );
  oyImage_Release( &image_out );
  oyProfile_Release( &prof );
  /* create a very simple pixel iterator */
  pixel_access = oyPixelAccess_Create( 0,0,
                                 oyFilterNode_GetSocket( conversion->input, 0 ),
                                       oyPIXEL_ACCESS_IMAGE, 0 );

  /* show the Oyranos graph with ghostview */
  ptr =  oyConversion_ToText( conversion, "Oyranos simple Test Graph",0,malloc);
  oyWriteMemToFile_( "test.dot", ptr, strlen(ptr) );
#if 1
  system("dot -Tps test.dot -o test.ps; gv -spartan -antialias -magstep 0.7 test.ps &");
#endif
  free(ptr); ptr = 0;


  result = 0;
  x = y = 0;
  {
    double * p = dest;

    pixel_access->start_xy[0] = x;
    pixel_access->start_xy[1] = y;
    result = oyConversion_Run( conversion, pixel_access, 0 );

    if(result == 0)
    for(i = 0; i < w*h*3; i += 3)
    {
      char br = (i/3+1)%w? ' ':'\n';
      fprintf( stdout, "%.01f %.01f %.01f %c",
               p[i+0], p[i+1], p[i+2], br );
    }
  }

  /* itereate in chunks */
  result = 0;
  x = y = 0;
  pixel_access->start_xy[0] = pixel_access->start_xy[1] = x;
  while(result == 0)
  {
    double * p = 0;

    result = oyConversion_Run( conversion, pixel_access, 0 );
    p = pixel;

    if(result == 0)
    for(i = 0; i < pixel_access->pixels_n*3; i += 3)
      fprintf( stdout, "%.01f %.01f %.01f\n", p[i+0], p[i+1], p[i+2] );

    x += 2;
    if(x >= w)
    {
      pixel_access->start_xy[0] = 0;
      pixel_access->start_xy[1] = ++y;
      x = 0;
    }
  }


  if(conversion->input->filter->api4_->oyCMMFilterNode_ContextToMem)
    ptr = conversion->input->filter->api4_->oyCMMFilterNode_ContextToMem( conversion->input, &size, 0, malloc );

  if (0) /* dump the colour transformation */
  {
    oyFilterPlug_s * plug = oyFilterNode_GetPlug( conversion->out_, 0 );
    ptr = plug->remote_socket_->node->filter->api4_->oyCMMFilterNode_ContextToMem(
             plug->remote_socket_->node, &size, 0, malloc );
  }

  if(ptr)
    oyWriteMemToFile_( "test_dbg.icc", ptr, size );

  oyConversion_Release( &conversion );
  oyPixelAccess_Release( &pixel_access );

  return 0;
}


