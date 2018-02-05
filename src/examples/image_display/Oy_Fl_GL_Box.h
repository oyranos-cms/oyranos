#ifndef Oy_Fl_GL_Box_H
#define Oy_Fl_GL_Box_H

extern "C" {
int              oyArray2d_ToPPM_    ( oyArray2d_s       * array,
                                       const char        * file_name );
}


#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>

#include "Oy_Fl_Image_Widget.h"
#include "oyranos_threads.h"

class Oy_Fl_GL_Box : public Fl_Gl_Window,
                     public Oy_Fl_Image_Widget
{
  char * frame_data;
  int frame_height, frame_width;
  int gl_type;
  oyDATATYPE_e data_type;
  int W,H;
public:
  Oy_Fl_GL_Box(int x, int y, int w, int h)
    : Fl_Gl_Window(x,y,w,h), Oy_Fl_Image_Widget(x,y,w,h)
  { frame_data = NULL; W=0; H=0; need_redraw=2;
# define TEST_GL(modus) { \
    this->mode(modus); \
    if(this->can_do()) { \
      mod |= modus; \
      this->mode(mod); \
      fprintf(stderr, "OpenGL understand: yes  %s %d\n", #modus, this->mode() ); \
    } else {  printf("can_do() false: %d\n", modus); \
      fprintf(stderr, "OpenGL understand: no   %s %d\n", #modus, this->mode() ); \
    } \
  }
  long mod = 0;
  TEST_GL(FL_RGB)
  //TEST_GL(FL_DOUBLE)
  //TEST_GL(FL_ALPHA)
  //TEST_GL(FL_DEPTH)
  //TEST_GL(FL_MULTISAMPLE)
  mode(mod);
  };

  ~Oy_Fl_GL_Box(void) { };

  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Oy_Fl_Image_Widget::damage( c );
  }


private:
  int need_redraw;
  void drawFrame()
  {
      glRasterPos2i(-frame_width, -frame_height);
      /* on osX it uses sRGB without alternative */
      if(data_type == oyUINT16)
        glDrawPixels( frame_width, frame_height, gl_type,
                      GL_UNSIGNED_SHORT, frame_data );
      else if(data_type == oyFLOAT)
        glDrawPixels( frame_width, frame_height, gl_type,
                      GL_FLOAT, frame_data );
      else if(data_type == oyHALF)
        glDrawPixels( frame_width, frame_height, gl_type,
                      GL_HALF_FLOAT, frame_data );
      else
        fprintf(stdout, "%s:%d unsupported data type %s %dx%d %dx%d\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, oyDataTypeToText( data_type ),
                    frame_width,frame_height,W,H);

#ifndef __APPLE__
      glDrawBuffer(GL_BACK);
#endif // !MESA

  }
  void draw()
  {
    if(!context_valid())
      return;

    int W_ = Oy_Fl_Image_Widget::w(),
        H_ = Oy_Fl_Image_Widget::h();
    if(oy_display_verbose)
      printf("%s:%d %s() %dx%d+%d+%d %dx%d+%d+%d\n", 
     strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__, __LINE__, __func__,
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y());

    if(conversion())
    {
      int y, height = 0, is_allocated = 0;
      oyPointer image_data = 0;
      int sample_size;
      oyPixel_t pt;
      int channels = 0;
      oyImage_s * draw_image = 0;

      gl_type = 0;
      data_type = oyUINT8;

      draw_image = oyConversion_GetImage( conversion(), OY_INPUT );
      pt = oyImage_GetPixelLayout( draw_image, oyLAYOUT );
      data_type = oyToDataType_m( pt );
      if(data_type == oyUINT8) /* avoid ICC conversion quantisation */
        data_type = oyUINT16;
      if(data_type == oyDOUBLE) /* OpenGL appears to support only <= float buffers */
        data_type = oyFLOAT;
      sample_size = oyDataTypeGetSize( data_type );
      oyImage_Release( &draw_image );

      int result = drawPrepare( &draw_image, data_type, 1 );

      pt = oyImage_GetPixelLayout( draw_image, oyLAYOUT );
      channels = oyToChannels_m( pt );

      if(channels == 3)
        gl_type = GL_RGB;
      if(channels == 4)
        gl_type = GL_RGBA;

      if(0&&oy_display_verbose && draw_image)
        fprintf(stdout, "%s:%d pixellayout: %d width: %d channels: %d\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__,
                    pt, oyImage_GetWidth( draw_image ), oyToChannels_m(pt) );

      if(!valid() ||
         W_ != W || H_ != H || !frame_data)
      {
        W = W_;
        H = H_;
        if(frame_data) free(frame_data);
        frame_data = (char*)malloc(W*H*channels*sample_size);
        valid(1);
        glLoadIdentity();
        glViewport( 0,0, W,H );
        glOrtho( -W,W, -H,H, -1.0,1.0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      }

      glClear(GL_COLOR_BUFFER_BIT);
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINE_STRIP); glVertex2f(W, H); glVertex2f(-W,-H); glEnd();
      glBegin(GL_LINE_STRIP); glVertex2f(W,-H); glVertex2f(-W, H); glEnd();

      if((!draw_image || result == 0) && valid())
      {
        if(!need_redraw)
        {
          oyImage_Release( &draw_image );
          if(oy_debug >= 4)
            fprintf(stderr, "%s:%d %s() return after result:%d ======  %lu ==========================\n0x%tx\n", 
                  strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                  __LINE__, __func__,result, (long unsigned)draw_image,
                  (intptr_t)frame_data);

          drawFrame();

          return;
        }
        --need_redraw;
      }

      //if(result < 0) ++need_redraw;

#ifdef __APPLE__
      glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

      frame_height = OY_MIN(oyImage_GetHeight( draw_image ),H),
      frame_width = OY_MIN(oyImage_GetWidth( draw_image ),W);

      int pos[4] = {-2,-2,-2,-2};
      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );
      if(0&&oy_display_verbose)
        fprintf( stderr, "%s():%d %d,%d %d %d\n", __FILE__,__LINE__,
                 pos[0],pos[1],pos[2], pos[3] );

      /* get the data */
      if(draw_image && frame_data)
      for(y = 0; y < frame_height; ++y)
      {
        image_data = oyImage_GetLineF(draw_image)( draw_image, y, &height, -1, &is_allocated );

        memcpy( &frame_data[frame_width*(frame_height-y-1)*channels*sample_size], image_data,
                frame_width*channels*sample_size );

        if(is_allocated)
          free( image_data );
      }

      drawFrame();

      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );

      if(oy_display_verbose)
        fprintf(stdout, "%s:%d draw %dx%d %dx%d\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__,
                    frame_width,frame_height,W,H);

      if(oy_debug && getenv("OY_DEBUG_WRITE"))
        oyImage_WritePPM( draw_image, "dbg.ppm", __FILE__);

      oyImage_Release( &draw_image );
      if(oy_debug )
          fprintf(stderr, "%s:%d %s() ========== finished ========== result:%d valid:%d dirty:%d\n", 
                  strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                  __LINE__, __func__, result, valid(), dirty);
    }
  }

  int  handle (int e)
  {
    int ret = 1;
    redraw();
    ret = Oy_Fl_Image_Widget::handle( e );
    if(!ret)
    ret = Fl_Gl_Window::handle( e );
    return ret;
  }

  void redraw()
  {
    Fl::awake(this);
    Fl_Gl_Window::redraw();
  }

  static int loadImageName( oyJob_s * job )
  {
    char * t = NULL;
    const char * tmp = oyOption_GetText((oyOption_s*)job->context,oyNAME_NICK);
    if(job->cb_progress)
    {
      if(tmp)
      {
        t = (char*) malloc(80+strlen(__func__)+strlen(tmp));
        sprintf( t, "%s():%d --->\n%s", __func__,__LINE__,
                 oyOption_GetText((oyOption_s*)job->context,oyNAME_NICK));
      }
      oyMsg_Add(job, .1, &t);
    }
    return 0;
  }
  static int finishImageName( oyJob_s * job )
  {
    char * t = NULL;
    const char * tmp = oyOption_GetText((oyOption_s*)job->context,oyNAME_NICK);
    if(job->cb_progress)
    {
      if(tmp)
      {
        t = (char*) malloc(80+strlen(__func__)+strlen(tmp));
        sprintf( t, "%s():%d --->\n%s\n<--- finished", __func__,__LINE__,
                 oyOption_GetText((oyOption_s*)job->context,oyNAME_NICK));
      }
      oyMsg_Add(job, 1., &t);
    }
    return 0;
  }
  static void jobCallback            ( double              progress_zero_till_one,
                                       char              * status_text,
                                       int                 thread_id_,
                                       int                 job_id,
                                       oyStruct_s        * cb_progress_context OY_UNUSED )
  { if(oy_display_verbose) fprintf(stderr,"%s():%d %02f %s %d/%d\n",__func__,__LINE__,progress_zero_till_one,
           status_text?status_text:"",thread_id_,job_id); }

public:
  oyFilterNode_s * setImage          ( const char        * file_name,
                                       oyOptions_s       * cc_options )
  {
    int icc_profile_flags = 0;
    oyImage_s * image = 0;
    oyJob_s * job = oyJob_New(0);

    icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                "//" OY_TYPE_STD "/icc_color", cc_options, 0 );

    job->work = loadImageName;
    job->finish = finishImageName;
    oyOption_s * o = oyOption_FromRegistration( "///file_name", NULL );
    oyOption_SetFromString( o, file_name, 0 );
    job->context = (oyStruct_s*)o;
    job->cb_progress = jobCallback;
    oyJob_Add(&job, 0, 0);

    oyImage_FromFile( file_name, icc_profile_flags, &image, 0 );
    oyPixel_t pt;
    oyDATATYPE_e data_type = oyUINT8;

    pt = oyImage_GetPixelLayout( image, oyLAYOUT );
    data_type = oyToDataType_m( pt );
    if(data_type == oyUINT8)
      data_type = oyUINT16;
    if(data_type == oyDOUBLE)
      data_type = oyFLOAT;

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

};

#endif /* Oy_Fl_GL_Box_H */
