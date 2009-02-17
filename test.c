/*
 * Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 *
 * 
 */

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

  w = 7;
  h = 32;
  size = sizeof(double)*w*h*3;
  d = malloc(size);
  dest = malloc(size);
  for(i = 0; i < w*h; ++i)
    for(j=0; j < 3; ++j)
      d[i*3+j] = (double)(i*3+j)/(double)size*sizeof(double);

  prof = oyProfile_FromStd( oyASSUMED_WEB, 0 );
  image_in = oyImage_Create( w, h, d, OY_TYPE_123_DBL, prof, 0 );
  image_out = oyImage_Create( w, h, dest, OY_TYPE_123_DBL, prof, 0 );
  conversion = oyConversion_CreateBasic( image_in, image_out, 0, 0 );
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
      fprintf( stdout, "%.03f %.03f %.03f %c",
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
    /*if(x >= w)
    {
      pixel_access->start_xy[0] = 0;
      pixel_access->start_xy[1] = ++y;
      x = 0;
    }*/
  }


  if(conversion->input->filter->api4_->oyCMMFilterNode_ContextToMem)
    ptr = conversion->input->filter->api4_->oyCMMFilterNode_ContextToMem( conversion->input, &size, malloc );

  if (0) /* dump the colour transformation */
  {
    oyFilterPlug_s * plug = oyFilterNode_GetPlug( conversion->out_, 0 );
    ptr = plug->remote_socket_->node->filter->api4_->oyCMMFilterNode_ContextToMem(
             plug->remote_socket_->node, &size, malloc );
  }

  if(ptr)
    oyWriteMemToFile_( "test_dbg.icc", ptr, size );

  oyConversion_Release( &conversion );


  conversion = oyConversion_CreateInput ( image_in, 0, 0 );

  filter = oyFilter_New( "//colour/icc", 0,0, 0 );

  error = oyConversion_FilterAdd( conversion, filter );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//colour" );
  
  error = oyConversion_OutputAdd( conversion, 0, image_out );

  result = oyConversion_Run( conversion, pixel_access, 0 );

  oyImage_Release( &image_in );
  oyImage_Release( &image_out );
  oyPixelAccess_Release( &pixel_access );



  return 0;
}


