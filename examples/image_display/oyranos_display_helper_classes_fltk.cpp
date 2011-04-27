/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009-2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/10
 *
 *  Classes to to build colour managed widgets with Oyranos in FLTK.
 */

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>

extern "C" {
const char *   oyDatatypeToText      ( oyDATATYPE_e        t); }
#include "oyranos_display_helpers.c"
#include "oyranos_display_helper_classes.cpp"


#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#ifdef HAVE_X11
#include <FL/x.H>
#endif


#include <cmath>

#ifdef USE_GETTEXT
#include "config.h" /* I18N */
#include "fl_i18n/fl_i18n.H"
#else
#define _(text) text
#endif

int oy_display_verbose = 0;


class Oy_Fl_Double_Window : public Fl_Double_Window
{
  public:
  Oy_Fl_Double_Window(int X, int Y, int W, int H, const char *title = 0)
   : Fl_Double_Window(X,Y,W,H,title) {};
  Oy_Fl_Double_Window(int W, int H, const char* title = 0) 
   : Fl_Double_Window(W,H,title) {};
  ~Oy_Fl_Double_Window() {};

  int pos_x;
  int pos_y;

  int  handle (int e)
  { 
    int lx_,ly_,lw_,lh_,wx,wy;
    lx_=ly_=lw_=lh_=wx=wy=0;
#if ((FL_MAJOR_VERSION * 100 + FL_MINOR_VERSION) > 106)
    Fl::screen_xywh(lx_,ly_,lw_,lh_);
#endif
    Fl_Window * win = 0;
    win = dynamic_cast<Fl_Window*> (this);
    wx = win->x();
    wy = Fl_Window::y();

#if defined(__APPLE__)
    /* osX virtualises window placement, but we need exact positioning */
    if(pos_x != x() || pos_y != y())
    {
      redraw();
    }
#endif

    pos_x = x();
    pos_y = y();

    if(e == FL_FOCUS)
      redraw();

    //printf("%d,%d event: %d\n", pos_x, pos_y, e );

    return Fl_Double_Window::handle(e);
  };
};


class Oy_Fl_Widget : public Fl_Widget, public Oy_Widget
{
  int e, ox, oy, px, py;
public:
  int handle(int event)
  {
      e = event;
      switch(e) {
        case FL_PUSH:
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
          fl_cursor( FL_CURSOR_MOVE, FL_BLACK, FL_WHITE );
#if DEBUG
      printf("%s:%d e: %d ox:%d px:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px);
#endif
          return (1);
        case FL_RELEASE:
          fl_cursor( FL_CURSOR_DEFAULT, FL_BLACK, FL_WHITE );
#if DEBUG
      printf("%s:%d e: %d ox:%d px:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px);
#endif
          return (1);
        case FL_DRAG:
          px += ox + Fl::event_x();
          py += oy + Fl::event_y();
          ox = x() - Fl::event_x();
          oy = y() - Fl::event_y();
#if DEBUG
      printf("%s:%d e: %d ox:%d px:%d oy: %d py:%d\n",strrchr(__FILE__,'/')+1,__LINE__,e, ox, px,oy,py);
#endif
          redraw();
          return (1);
      }
      int ret = Fl_Widget::handle(e);
      return ret;
  }

private:
  oyPixelAccess_s * ticket;
public:
  void drawPrepare( oyImage_s ** draw_image, oyDATATYPE_e data_type_request )
  {
    {
      Oy_Fl_Double_Window * win = 0;
      win = dynamic_cast<Oy_Fl_Double_Window*> (window());
      int X = win->pos_x + x();
      int Y = win->pos_y + y();
      int W = w();
      int H = h();
      int channels = 0;
      oyPixel_t pt;
      oyDATATYPE_e data_type;
      oyImage_s * image = 0;
      oyRectangle_s * display_rectangle = 0;
      oyFilterNode_s * node_out = oyConversion_GetNode( conversion(),OY_OUTPUT);
      void * display = 0,
           * window = 0;

#if defined(HAVE_X11)
      /* add X11 window and display identifiers to output image */
      display = fl_display;
      window = (void*)fl_xid(win);
#endif

      /* Inform about the images display coverage.  */
      display_rectangle = oyRectangle_NewWith( X,Y,W,H, 0 );

      /* Load the image before creating the oyPicelAccess_s object. */
      image = oyConversion_GetImage( conversion(), OY_OUTPUT );

      if(image && !ticket)
      {
        oyFilterPlug_s * plug = oyFilterNode_GetPlug( node_out, 0 );
        ticket = oyPixelAccess_Create( 0,0, plug, oyPIXEL_ACCESS_IMAGE, 0 );
      }

      if(image)
      {
        /* take care to not go over the borders */
        if(px < W - image->width) px = W - image->width;
        if(py < H - image->height) py = H - image->height;
        if(px > 0) px = 0;
        if(py > 0) py = 0;
      }

#if DEBUG
      printf( "%s:%d new display rectangle: %s +%d+%d +%d+%d\n",
              strrchr(__FILE__,'/')+1, __LINE__,
              oyRectangle_Show(display_rectangle), x(), y(), px, py );
#endif

      if(ticket)
      {
        oyRectangle_s output_rectangle = {oyOBJECT_RECTANGLE_S,0,0,0};
        oyRectangle_SamplesFromImage( image, 0, &output_rectangle );
        output_rectangle.width = W;
        output_rectangle.height = H;
        oyRectangle_Scale( &output_rectangle, 1.0/image->width );
#if DEBUG
        if(px != 0)
        {
        oyRectangle_s r = {oyOBJECT_RECTANGLE_S,0,0,0};
        oyRectangle_SetByRectangle( &r, &output_rectangle );
        oyRectangle_Scale( &r, image->width );
        printf( "%s:%d output rectangle: %s start_xy:%.04g %.04g\n",
                strrchr(__FILE__,'/')+1, __LINE__,
                oyRectangle_Show(&r),
                ticket->start_xy[0]*image->width, ticket->start_xy[1]*image->width );
        }
#endif
        oyConversion_ChangeRectangle ( conversion(), ticket,
                                       -px/(double)image->width,
                                       -py/(double)image->width,
                                       &output_rectangle );
        
      }

      dirty = oyDrawScreenImage(conversion(), ticket, display_rectangle,
                                old_display_rectangle,
                                old_roi_rectangle, "X11",
                                data_type_request,
                                display, window, dirty,
                                image );

      oyRectangle_Release( &display_rectangle );

      if(oy_display_verbose)
        oyShowGraph_( conversion()->input, 0 ); oy_display_verbose = 0;

      /* some error checks */
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != data_type_request))
      {
        printf( "WARNING: wrong image data format: %s\n"
                "need 4 or 3 channels with %s\n",
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "",
                oyDatatypeToText( data_type_request ) );
        return;
      }

      *draw_image = image;
    }
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

      drawPrepare( &image, oyUINT8 );

      pt = oyImage_PixelLayoutGet( image );
      channels = oyToChannels_m( pt );

      /* get the data and draw the image */
      if(image)
      for(i = 0; i < image->height; ++i)
      {
        image_data = image->getLine( image, i, &height, -1, &is_allocated );

        /* on osX it uses sRGB without alternative */
        fl_draw_image( (const uchar*)image_data, 0, i, image->width, 1,
                       channels, w()*channels);
        if(is_allocated)
          free( image_data );
      }

      oyImage_Release( &image );
    }
  }

public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Widget::damage( c );
  }

  Oy_Fl_Widget(int x, int y, int w, int h) : Fl_Widget(x,y,w,h)
  {
    px=py=ox=oy=0;
    ticket = 0;
  };

  ~Oy_Fl_Widget(void)
  {
    oyPixelAccess_Release( &ticket );
  };

  void observeICC(                     oyFilterNode_s * icc,
                     int(*observator)( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data ) )
  {
    /* observe the icc node */
    oyPointer_s * oy_box_ptr = oyPointer_New(0);
    oyPointer_Set( oy_box_ptr,
                   __FILE__,
                   "Oy_Fl_Widget",
                   this, 0, 0 );
    oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion(),
                          (oyStruct_s*)oy_box_ptr,
                          observator );
    oyPointer_Release( &oy_box_ptr );
  }
};

class Oy_Fl_Box : public Fl_Box, public Oy_Fl_Widget
{
public:
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Fl_Box::damage( c );
  }

  Oy_Fl_Box(int x, int y, int w, int h) : Fl_Box(x,y,w,h), Oy_Fl_Widget(x,y,w,h)
  {
  };

  ~Oy_Fl_Box(void)
  {
  };

  oyFilterNode_s * setImage( const char * file_name )
  {
    oyFilterNode_s * icc = setImageType( file_name, oyUINT8 );
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
                   "Oy_Fl_Box",
                   this, 0, 0 );
    oyStruct_ObserverAdd( (oyStruct_s*)icc, (oyStruct_s*)conversion(),
                          (oyStruct_s*)oy_box_ptr,
                          observator );
    oyPointer_Release( &oy_box_ptr );
  }
};


extern "C" {
int      conversionObserve           ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data )
{
  int handled = 0;
  oyObserver_s * obs = observer;

  if(observer && observer->model &&
     observer->model->type_ == oyOBJECT_FILTER_NODE_S)
  {
    /*if(oy_debug_signals)*/
      printf("%s:%d INFO: \n\t%s %s: %s[%d]->%s[%d]\n",
                    strrchr(__FILE__,'/')?strrchr(__FILE__,'/')+1:__FILE__,
                    __LINE__, _("Signal"),
                    oySignalToString(signal_type),
                    oyStruct_GetText( obs->model, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->model->oy_),
                    oyStruct_GetText( obs->observer, oyNAME_NAME, 1),
                    oyObject_GetId(   obs->observer->oy_) );

    oyConversion_Correct( (oyConversion_s*)obs->observer,
                          "//" OY_TYPE_STD "/icc", oyOPTIONATTRIBUTE_ADVANCED,
                          0 );

    Oy_Fl_Widget * oy_widget = (Oy_Fl_Widget*) oyPointer_GetPointer(
                                             (oyPointer_s*)observer->user_data);
    oy_widget->damage( FL_DAMAGE_USER1 );

  }

  return handled;
}
}


#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>

#ifndef OY_MIN
#define OY_MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#endif

class Oy_Fl_Shader_Box : public Fl_Gl_Window,
                         public Oy_Fl_Widget
{
  char * frame_data;
public:
  Oy_Fl_Shader_Box(int x, int y, int w, int h)
    : Fl_Gl_Window(x,y,w,h), Oy_Fl_Widget(x,y,w,h)
  { frame_data = NULL; };
  ~Oy_Fl_Shader_Box(void) { };
  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Oy_Fl_Widget::damage( c );
  }


private:
  int frame_dirty;
  int frame_pos;
  void draw()
  {
    int W = Oy_Fl_Widget::w(),
        H = Oy_Fl_Widget::h();
    if(conversion())
    {
      int y, height = 0, is_allocated = 0;
      oyPointer image_data = 0;
      oyPixel_t pt;
      int channels = 0;
      oyImage_s * image = 0;

      drawPrepare( &image, oyUINT16 );

      pt = oyImage_PixelLayoutGet( image );
      channels = oyToChannels_m( pt );

      if(!valid())
      {
        valid(1);
        glLoadIdentity();
        glViewport( 0,0, W,H );
        glOrtho( -W,W, -H,H, -1.0,1.0);
      }

      glClear(GL_COLOR_BUFFER_BIT);
      glColor3f(1.0, 1.0, 1.0);
      glBegin(GL_LINE_STRIP); glVertex2f(W, H); glVertex2f(-W,-H); glEnd();
      glBegin(GL_LINE_STRIP); glVertex2f(W,-H); glVertex2f(-W, H); glEnd();

      int frame_height = OY_MIN(image->height,H),
          frame_width = OY_MIN(image->width,W);

      if(!frame_data)
        frame_data = (char*)malloc(W*H*3*2);

      int pos[4] = {-2,-2,-2,-2};
      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );
      if(oy_display_verbose)
        fprintf(stderr,"%s():%d %d,%d\n",__FILE__,__LINE__,pos[0],pos[1]);

      /* get the data */
      if(image && frame_data)
      for(y = 0; y < frame_height; ++y)
      {
        image_data = image->getLine( image, y, &height, -1, &is_allocated );

        memcpy( &frame_data[frame_width*(frame_height-y-1)*3*2], image_data,
                frame_width*3*2 );

        if(is_allocated)
          free( image_data );
      }

      /*double scale = (double)H/(double)frame_height;

      if(scale * frame_width > W)
        scale = (double)W/(double)frame_width;

      glPixelZoom( scale,-scale );*/
      glRasterPos2i(-frame_width, -frame_height);
      /* on osX it uses sRGB without alternative */
      glDrawPixels( frame_width, frame_height, GL_RGB,
                    GL_UNSIGNED_SHORT, frame_data );

      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );

      //glDrawPixels( frame_width, frame_height, GL_RGB, GL_UNSIGNED_BYTE,
      //              frame_data );
      frame_dirty = 0;
      printf("draw %dx%d[%d] %dx%d\n",frame_width,frame_height,frame_pos,W,H);

      oyImage_Release( &image );
    }
  }

  int  handle (int e)
  {
    int ret = 1;
    ret = Oy_Fl_Widget::handle( e );
    if(!ret)
    ret = Fl_Gl_Window::handle( e );
    return ret;
  }

  void resize(int X,int Y,int W,int H)
  {
    if(frame_data) free(frame_data);
    frame_data = (char*)malloc(W*H*3*2);
    Fl_Gl_Window::resize(X,Y,W,H);
    glLoadIdentity();
    glViewport( 0,0, W,H );
    glOrtho( -W,W, -H,H, -1.0,1.0);
    Oy_Fl_Widget::resize(X,Y,W,H);
    redraw();
  }

  void redraw()
  {
    Fl::awake(this);
  }

public:
  oyFilterNode_s * setImage( const char * file_name )
  {
    oyFilterNode_s * icc = setImageType( file_name, oyUINT16 );
    return icc;
  }

};

