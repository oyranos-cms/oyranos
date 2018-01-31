#ifndef Oy_Fl_Shader_Box_H
#define Oy_Fl_Shader_Box_H


#if defined(__APPLE__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>
#include <FL/gl.h>

#include "Oy_Fl_Image_Widget.h"

#ifndef _DBG_FORMAT_
#define _DBG_FORMAT_ "%s:%d %s() "
#define _DBG_ARGS_ (strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__),__LINE__,__func__
#endif

class Oy_Fl_Shader_Box : public Fl_Gl_Window,
                         public Oy_Fl_Image_Widget
{
  /* texture storage */
  char * frame_data;
  int frame_height, frame_width;
  /* widget dimensions */
  int W,H;
  /* sub texture dimensions */
  int sw = 0, sh = 0;
  int grid_points;
  int max_texture_size;
  oyImage_s * clut_image,
            * image,
            * display_image;
public:
  Oy_Fl_Shader_Box(int x, int y, int w, int h)
    : Fl_Gl_Window(x,y,w,h), Oy_Fl_Image_Widget(x,y,w,h)
  { frame_data = NULL; W=0; H=0; clut_image = image = display_image = NULL;
    grid_points = 0; clut = 0; clut_filled = 0; clut_texture = -1; need_redraw=2;
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &max_texture_size );
  };

  ~Oy_Fl_Shader_Box(void) { oyImage_Release( &clut_image );
    oyImage_Release( &image ); oyImage_Release( &display_image ); };

  void damage( char c )
  {
    if(c & FL_DAMAGE_USER1)
      dirty = 1;
    Oy_Fl_Image_Widget::damage( c );
  }

private:
    GLushort * clut;
    int clut_filled;

    GLuint img_texture;
    GLuint clut_texture;

    GLfloat clut_scale;
    GLfloat clut_offset;

    GLhandleARB cmm_prog;
    GLhandleARB cmm_shader;

    static const char * cmm_shader_source;

  void print_log (GLhandleARB obj)
  {
    int len = 0;
    int nwritten = 0;
    char *log;

    glGetObjectParameterivARB (obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &len);

    if (len > 0) {
      log = (char*) malloc (len);
      glGetInfoLogARB (obj, len, &nwritten, log);
      fprintf (stderr, "%s\n", log);
      free (log);
    }
  }
  int check_error (const char *text)
  {
    GLenum err = glGetError ();

    if (err != GL_NO_ERROR)
    {
      if (text)
        fprintf (stderr, _DBG_FORMAT_"%s:\n",_DBG_ARGS_, text);
      while (err != GL_NO_ERROR)
      {
        fprintf (stderr, _DBG_FORMAT_"GL error %#x\n",_DBG_ARGS_,(int) err);
        err = glGetError ();
      }
      return 1;
    }
    return 0;
  }


  void init_shaders (void)
  {
    GLint loc;

    /* compile shader program */

    cmm_shader = glCreateShaderObjectARB (GL_FRAGMENT_SHADER_ARB);
    glShaderSourceARB (cmm_shader, 1, &cmm_shader_source, NULL);
    glCompileShaderARB (cmm_shader);
    print_log (cmm_shader);

    cmm_prog = glCreateProgramObjectARB ();
    glAttachObjectARB (cmm_prog, cmm_shader);
    glLinkProgramARB (cmm_prog);
    print_log (cmm_prog);

    glUseProgramObjectARB (cmm_prog);

    loc = glGetUniformLocation ((GLintptr)cmm_prog, "scale");
    glUniform1fARB (loc, clut_scale);

    loc = glGetUniformLocation ((GLintptr)cmm_prog, "offset");
    glUniform1fARB (loc, clut_offset);

    loc = glGetUniformLocation ((GLintptr)cmm_prog, "image");
    glUniform1iARB (loc, 0);

    /* texture 1 = clut */
    glActiveTextureARB (GL_TEXTURE0_ARB + 1);
    glBindTexture (GL_TEXTURE_3D, clut_texture);

    loc = glGetUniformLocation ((GLintptr)cmm_prog, "clut");
    glUniform1iARB (loc, 1);
  }

  int  load3DTextureFromFile( const char * file_name )
  {
    int w,h;
    oyPixel_t pt;
    oyDATATYPE_e data_type;

    if (file_name == NULL)
    {
    	fprintf(stderr, _DBG_FORMAT_"Cannot open clut file\n", _DBG_ARGS_);
      return 1;
    }

    oyImage_Release( &clut_image );
    oyImage_FromFile( file_name, 0, &clut_image, 0 );
    if(!clut_image)
    {
      fprintf(stderr, _DBG_FORMAT_"Cannot open clut file: %s\n", _DBG_ARGS_,
                      file_name);
      return 1;
    }

    pt = oyImage_GetPixelLayout( clut_image, oyLAYOUT );
    data_type = oyToDataType_m(pt);
    w = grid_points = oyImage_GetWidth( clut_image );
    h = oyImage_GetHeight( clut_image );
    int is_allocated = 0;
    clut = (GLushort*)oyImage_GetLineF( clut_image )( clut_image, 0,0, 0, &is_allocated );
    if(!clut)
      fprintf( stderr,_DBG_FORMAT_"obtained no data from clut image\n%s\n", _DBG_ARGS_, oyStruct_GetText( (oyStruct_s*)clut_image, oyNAME_NAME, 0));

    if( h != w*w || data_type != oyUINT16 )
    {
    	fprintf( stderr,_DBG_FORMAT_
                 "Cannot use clut file: %dx%d %s\nneed %dx%d oyUINT16\n",
                 _DBG_ARGS_, w,h,oyDataTypeToText(data_type), w,w*w);
      return 1;
    }

    clut_scale = (double) (w - 1) / w;
    clut_offset = 1.0 / (2 * w);

    clut_filled = 1;
    if( oy_display_verbose )
    {
    	fprintf( stderr,_DBG_FORMAT_
                "loaded clut: %s\ngrid_points:%d h:%d %s need %dx%d oyUINT16\n",
                 _DBG_ARGS_, file_name,
                 grid_points, h,oyDataTypeToText(data_type), w,w*w);
    }
    return 0;
  }

  int change_frame_dimensions( int width, int height )
  {
    int changed = 0;
    int old_frame_width = frame_width,
        old_frame_height = frame_height;
    frame_width = 8;
    frame_height = 8;

    if(width > W)
      width = W;
    if(height > H)
      height = H;

    while (frame_width < width)
    {
      frame_width += frame_width;
      if(frame_width > width)
        break;
    }
    while (frame_height < height)
    {
      frame_height += frame_height;
      if(frame_height > height)
        break;
    }
    changed = old_frame_width != frame_width || old_frame_height != frame_height;

    if(!changed)
      return changed;

    if( frame_width > max_texture_size ||
        frame_height > max_texture_size )
      fprintf( stderr,_DBG_FORMAT_"frame size is likely too big frame(%dx%d) sub(%dx%d) max:%d\n", _DBG_ARGS_, frame_width, frame_height, sw,sh, max_texture_size);
    else if(oy_display_verbose)
      fprintf( stderr,_DBG_FORMAT_"frame size (%dx%d) max:%d\n", _DBG_ARGS_, frame_width, frame_height, max_texture_size);


    oyPixel_t pt = oyImage_GetPixelLayout( image, oyLAYOUT );
    oyDATATYPE_e data_type = oyToDataType_m( pt );
    int channels = oyToChannels_m( pt );
    int sample_size = oyDataTypeGetSize( data_type );

    if(frame_data) free(frame_data);
    frame_data = (char*)malloc(frame_width*frame_height*channels*sample_size);
    if(!frame_data)
      fprintf( stderr,_DBG_FORMAT_"failed to allocate frame_data (%dx%d)%dc\n", _DBG_ARGS_, frame_width, frame_height, channels);

    return changed;
  }

  void make_image_texture (void)
  {
    oyImage_s * draw_image = NULL;

    if(conversion())
      draw_image = oyConversion_GetImage( conversion(), OY_OUTPUT );

    oyPixel_t pt = oyImage_GetPixelLayout( image, oyLAYOUT );
    oyDATATYPE_e data_type = oyToDataType_m( pt );
    int channels = oyToChannels_m( pt );
    oyConversion_s * cc;
    int width = oyImage_GetWidth( draw_image ? draw_image : image ),
        height = oyImage_GetHeight( draw_image ? draw_image : image );
    oyImage_Release( &draw_image );

    /* set image texture size to the smaller of the image or viewport */
    change_frame_dimensions( width, height );

    /*if(data_type == oyUINT8)
      data_type = oyUINT16;*/
    oyOptions_s * old_tags = oyImage_GetTags( display_image ), * tags;
    oyImage_Release( &display_image );
    display_image = oyImage_Create( frame_width, frame_height,
                         0,
                         oyChannels_m(3) | oyDataType_m(data_type),
                         oyImage_GetProfile( image ),
                         0 );
    tags = oyImage_GetTags( display_image );
    oyOptions_AppendOpts( tags, old_tags );
    oyOptions_Release( &old_tags );
    oyOptions_Release( &tags );

    if(oy_display_verbose)
      fprintf( stderr,_DBG_FORMAT_"Wid:%dx%d+%d+%d Parent:%dx%d+%d+%d texture:(%dx%d)%dc %s\n",
        _DBG_ARGS_,
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y(),
        frame_width,frame_height, channels, oyDataTypeToText(data_type));

    cc = oyConversion_FromImageForDisplay( image, display_image,
                         0, OY_SKIP_ICC, data_type, 0, 0 );
    conversion( cc );

    int gl_channels = 0;
    if(channels == 3)
      gl_channels = GL_RGB;
    if(channels == 4)
      gl_channels = GL_RGBA;

    int gl_data_type = 0;
    int gl_type = 0;
    if(data_type == oyUINT16)
    {
      gl_data_type = GL_UNSIGNED_SHORT;
      if(channels == 3)
        gl_type = GL_RGB16;
      if(channels == 4)
        gl_type = GL_RGBA16;
    } else if(data_type == oyUINT8)
    {
      gl_data_type = GL_UNSIGNED_BYTE;
      if(channels == 3)
        gl_type = GL_RGB8;
      if(channels == 4)
        gl_type = GL_RGBA8;
    } else
      fprintf( stderr,_DBG_FORMAT_"%dx%d+%d+%d %dx%d+%d+%d %dx%d not supported %s\n",
        _DBG_ARGS_,
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y(),
        frame_width,frame_height, oyDataTypeToText(data_type));

    if(oy_debug)
      fprintf( stderr,_DBG_FORMAT_"texture:(%dx%d)%dc %s %s %s %s\n", _DBG_ARGS_,
        frame_width,frame_height, channels, oyDataTypeToText(data_type),
        printType( gl_type ), printDataType(gl_data_type), printChannelType( gl_channels));

    /* texture 0 (image) */
    glGenTextures (1, &img_texture);
    glActiveTextureARB (GL_TEXTURE0_ARB);
    glBindTexture (GL_TEXTURE_2D, img_texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D (GL_TEXTURE_2D, 0, gl_type, frame_width, frame_height,
		  0, gl_channels, gl_data_type, frame_data);

    /* size may be too big */
    if(check_error("glTexImage2D failed (image too large?)"))
    {
      fprintf( stderr,_DBG_FORMAT_"texture:(%dx%d)%dc %s %s %s %s\n", _DBG_ARGS_,
        frame_width,frame_height, channels, oyDataTypeToText(data_type),
        printType( gl_type ), printDataType(gl_data_type), printChannelType( gl_channels));
      //exit(1);
    }

    glBindTexture (GL_TEXTURE_2D, 0);

    if(0&&oy_display_verbose)
      oyImage_ToFile( display_image, "display_image.ppm", 0 );
  }

  void setupShaderTexture()
  {
    glGenTextures (1, &clut_texture);
    if(oy_display_verbose) fprintf(stderr,_DBG_FORMAT_ "clut_texture: %d\n", _DBG_ARGS_, clut_texture);
    /* texture 1 = clut */
    glActiveTextureARB (GL_TEXTURE0_ARB + 1);
    glBindTexture (GL_TEXTURE_3D, clut_texture);

    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    /*
     * Don't use FP luts, but only 16-bit integer ones, since the ATI card
     * does not support GL_LINEAR for GL_RGB32F_ARB, but ony GL_NEAREST
     */
    int n = oyImage_GetWidth(clut_image);
    glTexImage3D (GL_TEXTURE_3D, 0, GL_RGB16, n, n, n,
		  0, GL_RGB, GL_UNSIGNED_SHORT, clut);

    /* back to texture 0 (image) */
    glActiveTextureARB (GL_TEXTURE0_ARB);
    glBindTexture (GL_TEXTURE_2D, img_texture);
  }

  int need_redraw;
  void draw()
  {
    int W_ = Oy_Fl_Image_Widget::w(),
        H_ = Oy_Fl_Image_Widget::h();

    if(oy_display_verbose)
      fprintf( stderr, _DBG_FORMAT_"Widget:%dx%d+%d+%d  Parent:%dx%d+%d+%d\n",_DBG_ARGS_, 
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y());

    // - not needed, but might improve speed - START //
    glShadeModel (GL_FLAT);
    glDisable (GL_DITHER);

    glDisable (GL_BLEND);
    glDisable (GL_DEPTH_TEST);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    // - not needed, but might improve speed - END //

    glClearColor (0.5, 0.5, 0.5, 0.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glViewport( 0,0, W,H );
    glOrtho( -W,W, -H,H, -1.0,1.0);
    glEnable (GL_TEXTURE_2D);



    if(image && clut_image)
    if(!valid() || need_redraw ||
       W_ != W || H_ != H ||
       !frame_data)
    {
      W = W_;
      H = H_;
      valid(1);

      make_image_texture();

      setupShaderTexture();

      if(GLEE_ARB_fragment_shader && GLEE_ARB_shading_language_100)
        init_shaders ();
    }

    oyImage_s * draw_image = NULL;
    int channels = 0;
    oyDATATYPE_e data_type = oyUINT8;
    int sample_size = 0;
    if(conversion())
    {
      oyPixel_t pt = oyImage_GetPixelLayout( display_image, oyLAYOUT );
      data_type = oyToDataType_m( pt );
      sample_size = oyDataTypeGetSize( data_type );

      int result = drawPrepare( &draw_image, data_type, 1 );

      if(!draw_image || result != 0)
      {
        if(need_redraw && valid())
        {
          valid(0);
          --need_redraw;
          // needed for proper first time displaying
          oyConversion_RunPixels( conversion(), 0 );
          oyImage_Release( &draw_image );
          return;
        }
      }

      pt = oyImage_GetPixelLayout( draw_image, oyLAYOUT );

      channels = oyToChannels_m( pt );

      if(oy_debug && draw_image)
        fprintf(stdout, _DBG_FORMAT_"pixellayout: %d width: %d channels: %d\n",
                    _DBG_ARGS_,
                    pt, oyImage_GetWidth(draw_image), oyToChannels_m(pt) );

#ifdef __APPLE__
      glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

      /* glTexSubImage2D texture dimensions */
      sw = OY_MIN(oyImage_GetWidth( draw_image), W);
      sh = OY_MIN(oyImage_GetHeight(draw_image), H);

      if( frame_width < sw || frame_height < sh )
      {
        if(change_frame_dimensions( sw, sh ))
        {
          oyImage_SetCritical( display_image, 0, NULL, NULL, frame_width, frame_height );
          valid(0);
          oyImage_Release( &draw_image );
          return;
        }
        //make_image_texture();
      }

      /* a buffer alignment of 4 byte is what most GPUs prefer */
      if((sw*channels*sample_size)%4)
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
      else
        glPixelStorei(GL_UNPACK_ALIGNMENT,4);

      /* get the data */
      if(draw_image && frame_data)
      {
        if(sh > frame_height)
          fprintf( stderr, _DBG_FORMAT_"draw_image height: %d frame_height: %d\n",
                   _DBG_ARGS_, sh, frame_height );
        for(int y = 0; y < sh; ++y)
        {
          int is_allocated = 0, height = 0;
          void * image_data = oyImage_GetLineF(draw_image)( draw_image, y, &height, -1, &is_allocated );

          memcpy( &frame_data[sw*(sh-y-1)*channels*sample_size], image_data,
                  sw*channels*sample_size );

          if(is_allocated)
            free( image_data );
        }
      }
    } else
      fprintf( stderr, _DBG_FORMAT_ "conversion not ready\n", _DBG_ARGS_ );

    if(!draw_image)
    {
      fprintf( stderr, _DBG_FORMAT_ "no draw image\n", _DBG_ARGS_ );
      return;
    }

    /* border values */
    float bw = OY_MAX( 0, W - sw),
          bh = OY_MAX( 0, H - sh);
    /* maintain texture ratio */
    float tw = (W - bw)/(float)frame_width;
    float th = (H - bh)/(float)frame_height;

    if(oy_display_verbose)
      fprintf( stderr, _DBG_FORMAT_"frame: (%dx%d)%dc WxH(%dx%d border %g,%g) parent:%dx%d\n"
               "img:%d clut:%d(scale:%f offset:%f) prog:%tx shader:%tx texture ratio:%gx%g align:%d\n",
               _DBG_ARGS_, frame_width,frame_height,channels, W,H, bw,bh,
               Oy_Fl_Image_Widget::parent()->w(),
               Oy_Fl_Image_Widget::parent()->h(),
               img_texture, clut_texture, clut_scale, clut_offset,
               cmm_prog, cmm_shader, tw,th,
               (OY_MIN(oyImage_GetWidth( draw_image), W)*channels*sample_size)%2);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP); glVertex2f(W, H); glVertex2f(-W,-H); glEnd();
    glBegin(GL_LINE_STRIP); glVertex2f(W,-H); glVertex2f(-W, H); glEnd();

    if(need_redraw)
    {
      oyImage_Release( &draw_image );
      return;
    }

    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    int gl_data_type = 0;
    if(data_type == oyUINT16)
      gl_data_type = GL_UNSIGNED_SHORT;
    else if(data_type == oyUINT8)
      gl_data_type = GL_UNSIGNED_BYTE;

    /* upload texture 0 (image) */
    glBindTexture (GL_TEXTURE_2D, img_texture);
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, sw, sh,
                     GL_RGB, gl_data_type, frame_data);
    glBindTexture (GL_TEXTURE_2D, 0);

    if(oy_display_verbose)
      fprintf(stderr, _DBG_FORMAT_"subImage(%dx%d) frame(%dx%d)\n", _DBG_ARGS_, sw,sh, frame_width, frame_height );

    /* draw surface */
    glBegin (GL_QUADS);
      glTexCoord2f (0.0, 0.0); glVertex2f (-W + bw, -H + bh);
      glTexCoord2f (0.0, th ); glVertex2f (-W + bw,  H - bh);
      glTexCoord2f (tw,  th ); glVertex2f ( W - bw,  H - bh);
      glTexCoord2f (tw,  0.0); glVertex2f ( W - bw, -H + bh);
    glEnd ();

    glDisable (GL_TEXTURE_2D);
    oyImage_Release( &draw_image );
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

public:
  int              setImage          ( const char        * file_name,
                                       oyOptions_s       * cc_options OY_UNUSED,
                                       const char        * clut_name )
  {
    oyImage_Release( &image );
    int error = oyImage_FromFile( file_name, 0, &image, 0 );
    if(oy_display_verbose)
      fprintf(stderr, _DBG_FORMAT_"loaded image: %s\n%s\n", _DBG_ARGS_,
                      file_name, oyStruct_GetText( (oyStruct_s*)image, oyNAME_NAME, 0));
    if(!image)
      error = 1;

    int lerror = load3DTextureFromFile( clut_name );
    if(!error && !lerror && oy_display_verbose)
      fprintf(stderr, _DBG_FORMAT_"successfully loaded clut: %s\n", _DBG_ARGS_,
                      clut_name );

    return error + lerror;
  }

  const char * printDataType( int gl_data_type )
  {
    static char t[64] = {0};
    switch( gl_data_type )
    {
      case GL_UNSIGNED_SHORT: strcpy(t, "GL_UNSIGNED_SHORT"); break;
      case GL_UNSIGNED_BYTE:  strcpy(t, "GL_UNSIGNED_BYTE"); break;
      case GL_HALF_FLOAT:     strcpy(t, "GL_HALF_FLOAT"); break;
      case GL_FLOAT:          strcpy(t, "GL_FLOAT"); break;
      case GL_DOUBLE:         strcpy(t, "GL_DOUBLE"); break;
      default: sprintf( t, "%d", gl_data_type );
    }
    return t;
  }
  const char * printChannelType( int type )
  {
    static char t[64] = {0};
    switch( type )
    {
      case GL_RGB:  strcpy(t, "GL_RGB"); break;
      case GL_RGBA: strcpy(t, "GL_RGBA"); break;
      default: sprintf( t, "%d", type );
    }
    return t;
  }
  const char * printType( int type )
  {
    static char t[64] = {0};
    switch( type )
    {
      case GL_RGB8:  strcpy(t, "GL_RGB8"); break;
      case GL_RGBA8: strcpy(t, "GL_RGBA8"); break;
      case GL_RGB16:  strcpy(t, "GL_RGB16"); break;
      case GL_RGBA16: strcpy(t, "GL_RGBA16"); break;
      default: sprintf( t, "%d", type );
    }
    return t;
  }
};

    const char * Oy_Fl_Shader_Box::cmm_shader_source =
    "uniform sampler2D image;					\n"
    "uniform sampler3D clut;					\n"
    "uniform float scale;					\n"
    "uniform float offset;					\n"
    "								\n"
    "void main()						\n"
    "{								\n"
    "    vec3 img = texture2D(image, gl_TexCoord[0].xy).rgb;	\n"
    "								\n"
    "    // interpolate CLUT					\n"
    "    img = img * scale + offset;				\n"
    "    gl_FragColor = texture3D(clut, img);			\n"
    "								\n"
    "    // w/o color management				\n"
    "    // gl_FragColor = vec4(img, 1.0);			\n"
    "}								\n"
    ;

#endif /* Oy_Fl_Shader_Box_H */
