#ifndef Oy_Fl_Image_Box_H
#define Oy_Fl_Image_Box_H

#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include "Oy_Fl_Image_Widget.h"

class Oy_Fl_Image_Box : public Fl_Box, public Oy_Fl_Image_Widget
{
public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Box::damage( c );
  }

  Oy_Fl_Image_Box(int x, int y, int w, int h) : Fl_Box(x,y,w,h), Oy_Fl_Image_Widget(x,y,w,h)
  {
  };

  ~Oy_Fl_Image_Box(void)
  {
  };

  oyFilterNode_s * setImage          ( const char        * file_name,
                                       const char        * cc_name,
                                       oyOptions_s       * cc_options )
  {
    oyFilterNode_s * icc = setImageType( file_name, oyUINT8,
                                         cc_name, cc_options );
    return icc;
  }

  void observeICC(                     oyFilterNode_s * icc,
                     int(*observator)( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data ) )
  {
    /* observe the icc node */
    oyPointer_s * oy_box_ptr = oyPointer_New(0);
    oyPointer_Set( oy_box_ptr,
                   __FILE__,
                   "Oy_Fl_Image_Box",
                   this, 0, 0 );
    oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion(),
                          (oyStruct_s*)oy_box_ptr,
                          observator );
    oyPointer_Release( &oy_box_ptr );
  }
private:
  void draw()
  {
    if(conversion())
    {
      int i, height = 0, is_allocated = 0;
      oyPointer image_data = 0;
      oyPixel_t pt;
      int channels = 0;
      oyImage_s * image = 0;

      drawPrepare( &image, oyUINT8, 0 );

      pt = oyImage_GetPixelLayout( image, oyLAYOUT );
      channels = oyToChannels_m( pt );

      /* get the data and draw the image */
      if(image)
      for(i = 0; i < oyImage_GetHeight( image ); ++i)
      {
        image_data = oyImage_GetLineF(image)( image, i, &height, -1, &is_allocated );

        /* on osX it uses sRGB without alternative */
        fl_draw_image( (const uchar*)image_data, 0, i, oyImage_GetWidth( image ), 1,
                       channels, Oy_Fl_Image_Widget::w()*channels);
        if(is_allocated)
          free( image_data );
      }

      oyImage_Release( &image );
    }
  }
};


#endif /* Oy_Fl_Image_Box_H */
