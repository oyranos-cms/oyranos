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
                                       oyOptions_s       * cc_options )
  {
    int icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                "//" OY_TYPE_STD "/icc_color", cc_options, 0 );

    oyImage_s * image = 0;
    oyImage_FromFile( file_name, icc_profile_flags, &image, 0 );

    oyDATATYPE_e data_type = oyUINT8;
    oyImage_s * display_image = oyImage_Create( oyImage_GetWidth( image ), oyImage_GetHeight( image ),
                         0,
                         oyChannels_m(3) | oyDataType_m(data_type),
                         oyImage_GetProfile( image ),
                         0 );

    oyFilterNode_s * icc = setImageType( image, display_image, data_type,
                                         cc_options );
    oyImage_Release( &image );
    oyImage_Release( &display_image );

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
      uint8_t * image_data = 0;
      oyPixel_t pt;
      int channels = 0;
      oyImage_s * image = 0;

      drawPrepare( &image, oyUINT8, 0 );
      fprintf(stderr,".");

      pt = oyImage_GetPixelLayout( image, oyLAYOUT );
      channels = oyToChannels_m( pt );

      /* get the data and draw the image */
      if(image)
      {
        int iheight = oyImage_GetHeight( image ),
            iwidth = oyImage_GetWidth( image );
        for(i = 0; i < iheight; ++i)
        {
          image_data = (uint8_t*) oyImage_GetLineF(image)( image, i, &height, -1, &is_allocated );

          /* on osX it uses sRGB without alternative */
          fl_draw_image( (const uchar*)image_data, 0, i, iwidth, 1,
                         channels, Oy_Fl_Image_Widget::w()*channels);
          if(is_allocated)
            free( image_data );
        }
      }

      oyImage_Release( &image );
    }
  }
};


#endif /* Oy_Fl_Image_Box_H */
