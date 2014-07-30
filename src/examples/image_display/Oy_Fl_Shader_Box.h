#ifndef Oy_Fl_Shader_Box_H
#define Oy_Fl_Shader_Box_H


#include <GL/gl.h>
#include <GL/glu.h>

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
  char * frame_data;
  int W,H;
  int grid_points;
  oyImage_s * clut_image,
            * image,
            * display_image;
public:
  Oy_Fl_Shader_Box(int x, int y, int w, int h)
    : Fl_Gl_Window(x,y,w,h), Oy_Fl_Image_Widget(x,y,w,h)
  { frame_data = NULL; W=0; H=0; clut_image = image = display_image = NULL;
    grid_points = 0; clut = 0; clut_filled = 0; need_redraw=2; };

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
  void check_error (const char *text)
  {
    GLenum err = glGetError ();

    if (err != GL_NO_ERROR) {
    	if (text)
	    fprintf (stderr, _DBG_FORMAT_"%s: ",_DBG_ARGS_, text);
	while (err != GL_NO_ERROR) {
	    fprintf (stderr, _DBG_FORMAT_"GL error %#x\n",_DBG_ARGS_,(int) err);
	    err = glGetError ();
	}
	exit (1);
    }
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

    /* back to texture 0 (image) */
    glActiveTextureARB (GL_TEXTURE0_ARB);
    glBindTexture (GL_TEXTURE_2D, img_texture);
  }

  int  load3DTextureFromFile( const char * file_name )
  {
    int w,h;
    oyPixel_t pt;
    oyDATATYPE_e data_type;

    if (file_name == NULL) {
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
    clut = (GLushort*)oyImage_GetPointF( clut_image )( clut_image, 0,0, 0, &is_allocated );

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
 
  void make_image_texture (void)
  {
    int w_ = 8;
    int h_ = 8;
    oyPointer disp_img;
    oyPixel_t pt = oyImage_GetPixelLayout( image, oyLAYOUT );
    oyDATATYPE_e data_type = oyToDataType_m( pt );
    oyConversion_s * cc;
    int width = oyImage_GetWidth( image ),
        height = oyImage_GetHeight( image );

    /* set image texture size to the smaller of the image or viewport */
    if(width > W)
      width = W;
    if(height > H)
      height = H;

    while (w_ < width)
    {
      w_ += w_;
      if(w_ > width)
        break;
    }
    while (h_ < height)
    {
      h_ += h_;
      if(h_ > height)
        break;
    }

    /*if(data_type == oyUINT8)
      data_type = oyUINT16;*/

    oyOptions_s * old_tags = oyImage_GetTags( display_image ), * tags;
    oyImage_Release( &display_image );
    display_image = oyImage_Create( w_, h_,
                         0,
                         oyChannels_m(3) | oyDataType_m(data_type),
                         oyImage_GetProfile( image ),
                         0 );
    tags = oyImage_GetTags( display_image );
    oyOptions_AppendOpts( tags, old_tags );
    oyOptions_Release( &old_tags );
    oyOptions_Release( &tags );

    if(oy_display_verbose)
      fprintf( stderr,_DBG_FORMAT_"%dx%d+%d+%d %dx%d+%d+%d %dx%d %s\n",
        _DBG_ARGS_,
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y(),
        w_,h_, oyDataTypeToText(data_type));

    cc = oyConversion_FromImageForDisplay( image, display_image,
                         0, oyOPTIONATTRIBUTE_ADVANCED, data_type, 0, 0 );
    conversion( cc );

    glGenTextures (1, &img_texture);
    glBindTexture (GL_TEXTURE_2D, img_texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F_ARB, w_, h_,
    // glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE8, w_, h_,

    int channels = oyToChannels_m( pt );
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
        w_,h_, oyDataTypeToText(data_type));

    if(oy_display_verbose)
      fprintf( stderr,_DBG_FORMAT_"%dx%d %s %d %d %d %d %d %d\n", _DBG_ARGS_,
        w_,h_, oyDataTypeToText(data_type),
        GL_RGB16, gl_type, GL_UNSIGNED_SHORT, gl_data_type, GL_RGB, gl_channels);

    glTexImage2D (GL_TEXTURE_2D, 0, gl_type, w_, h_,
		  0, gl_channels, gl_data_type, NULL);

    /* size may be too big */
    check_error("glTexImage2D failed (image too large?)");

    disp_img = oyImage_GetPointF( display_image )( display_image, 0,0, 0, 0 );
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, w_, h_,
		     gl_channels, gl_data_type, disp_img);
    if(0&&oy_display_verbose)
      oyImage_ToFile( display_image, "display_image.ppm", 0 );
  }

  void setupShaderTexture()
  {
    glGenTextures (1, &clut_texture);
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
  }

  int need_redraw;
  void draw()
  {
    int w_,h_;

    W = Oy_Fl_Image_Widget::w(),
    H = Oy_Fl_Image_Widget::h();
    if(oy_display_verbose)
      fprintf( stderr, _DBG_FORMAT_"%dx%d+%d+%d %dx%d+%d+%d\n",_DBG_ARGS_, 
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y());

    glClearColor (0.5, 0.5, 0.5, 0.0);
    glShadeModel (GL_FLAT);
    glDisable (GL_DITHER);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glDisable (GL_BLEND);
    glDisable (GL_DEPTH_TEST);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glViewport(0,0, W,H );
    glOrtho( -W,W, -H,H, -1.0,1.0);

    if(image && clut_image && !valid())
    {
      make_image_texture ();

      setupShaderTexture();

      if(GLEE_ARB_fragment_shader && GLEE_ARB_shading_language_100)
	init_shaders ();
    }

    oyImage_s * draw_image = 0;
    if(conversion())
    {
      oyPixel_t pt;
      oyDATATYPE_e data_type = oyUINT8;

      pt = oyImage_GetPixelLayout( display_image, oyLAYOUT );
      data_type = oyToDataType_m( pt );

      int result = drawPrepare( &draw_image, data_type, 1 );

      pt = oyImage_GetPixelLayout( draw_image, oyLAYOUT );

#if 0
      int channels = oyToChannels_m( pt );
      int gl_type = 0;
      if(channels == 3)
        gl_type = GL_RGB;
      if(channels == 4)
        gl_type = GL_RGBA;
#endif

      if(oy_display_verbose && draw_image)
        fprintf(stdout, _DBG_FORMAT_"pixellayout: %d width: %d channels: %d\n",
                    _DBG_ARGS_,
                    pt, oyImage_GetWidth(draw_image), oyToChannels_m(pt) );

      if(!valid())
      {
        valid(1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      }

      if((!draw_image || result != 0) && valid())
      {
        if(!need_redraw)
        {
          oyImage_Release( &draw_image );
          return;
        }
        --need_redraw;
      }
    }

    w_ = oyImage_GetWidth(  draw_image );
    h_ = oyImage_GetHeight( draw_image );
    /* border values */
    float bw = OY_MAX( 0, W - oyImage_GetWidth(image)),
          bh = OY_MAX( 0, H - oyImage_GetHeight(image));
    /* maintain texture ratio */
    float tw = (W - bw)/(float)w_;
    float th = (H - bh)/(float)h_;

    if(oy_display_verbose)
      fprintf( stderr, _DBG_FORMAT_"w_ %d h_ %d  parent:%dx%d\n"
               "img:%d clut:%d scale:%f offset:%f prog:%d shader:%d %gx%g\n",
               _DBG_ARGS_, w_,h_,
               Oy_Fl_Image_Widget::parent()->w(),
               Oy_Fl_Image_Widget::parent()->h(),
               img_texture, clut_texture, clut_scale, clut_offset,
               cmm_prog, cmm_shader, tw,th);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_STRIP); glVertex2f(W, H); glVertex2f(-W,-H); glEnd();
    glBegin(GL_LINE_STRIP); glVertex2f(W,-H); glVertex2f(-W, H); glEnd();

    glEnable (GL_TEXTURE_2D);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    /* update texture 0 (image) */
    glBindTexture (GL_TEXTURE_2D, img_texture);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16, w_, h_,
		  0, GL_RGB, GL_UNSIGNED_SHORT, NULL);
    oyPointer disp_img = oyImage_GetPointF( draw_image )( draw_image, 0,0, 0, 0 );
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, w_, h_,
		     GL_RGB, GL_UNSIGNED_SHORT, disp_img);

    /* draw surface */
    glBegin (GL_QUADS);
	glTexCoord2f (0.0, th);  glVertex2f (-W + bw, -H + bh);
	glTexCoord2f (0.0, 0.0); glVertex2f (-W + bw,  H - bh);
	glTexCoord2f (tw,  0.0); glVertex2f ( W - bw,  H - bh);
	glTexCoord2f (tw,  th);  glVertex2f ( W - bw, -H + bh);
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
                                       oyOptions_s       * cc_options,
                                       const char        * clut_name )
  {
    oyImage_Release( &image );
    oyImage_FromFile( file_name, 0, &image, 0 );

    int error = load3DTextureFromFile( clut_name );
    if(!error && oy_display_verbose)
      fprintf(stderr, _DBG_FORMAT_"successfully loaded clut: %s\n", _DBG_ARGS_,
                      clut_name );

    return error;
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
