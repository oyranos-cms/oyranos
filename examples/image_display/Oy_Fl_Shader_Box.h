#ifndef Oy_Fl_Shader_Box_H
#define Oy_Fl_Shader_Box_H

extern "C" {
int              oyArray2d_ToPPM_    ( oyArray2d_s       * array,
                                       const char        * file_name );
    int oyImage_WritePPM( oyImage_s *, const char * , const char * );
}

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
    grid_points = 0; clut = 0; clut_filled = 0; };

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

    GLuint cmm_prog;
    GLuint cmm_shader;

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
	printf ("%s\n", log);
	free (log);
    }
  }
  void check_error (const char *text)
  {
    GLenum err = glGetError ();

    if (err != GL_NO_ERROR) {
    	if (text)
	    fprintf (stderr, "%s: ", text);
	while (err != GL_NO_ERROR) {
	    fprintf (stderr, "GL error %#x\n", (int) err);
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

    loc = glGetUniformLocation (cmm_prog, "scale");
    glUniform1fARB (loc, clut_scale);

    loc = glGetUniformLocation (cmm_prog, "offset");
    glUniform1fARB (loc, clut_offset);

    /* texture 1 = clut */
    glActiveTextureARB (GL_TEXTURE0_ARB + 1);
    glBindTexture (GL_TEXTURE_3D, clut_texture);

    loc = glGetUniformLocation (cmm_prog, "clut");
    glUniform1iARB (loc, 1);

    /* back to texture 0 (image) */
    glActiveTextureARB (GL_TEXTURE0_ARB);
    glBindTexture (GL_TEXTURE_2D, img_texture);

    loc = glGetUniformLocation (cmm_prog, "image");
    glUniform1iARB (loc, 0);
  }

  int  load3DTextureFromFile( const char * file_name )
  {
    int w,h;
    oyPixel_t pt;
    oyDATATYPE_e data_type;

    if (file_name == NULL) {
    	fprintf(stderr, "Cannot open clut file\n");
	return 1;
    }

    oyImage_Release( &clut_image );
    oyImage_FromFile( file_name, &clut_image, 0 );
    if(!clut_image)
    {
      fprintf(stderr, "Cannot open clut file: %s\n", file_name);
      return 1;
    }

    pt = oyImage_GetPixelLayout( clut_image );
    data_type = oyToDataType_m(pt);
    w = grid_points = oyImage_GetWidth( clut_image );
    h = oyImage_GetHeight( clut_image );
    int is_allocated = 0;
    clut = (GLushort*)oyImage_GetPoint( clut_image, 0,0, 0, &is_allocated );

    if( h != w*w || data_type != oyUINT16 )
    {
    	fprintf( stderr,"Cannot use clut file: %dx%d %s\nneed %dx%d oyUINT16\n",
                 w,h,oyDatatypeToText(data_type), w,w*w);
	return 1;
    }

    clut_scale = (double) (w - 1) / w;
    clut_offset = 1.0 / (2 * w);

    clut_filled = 1;
    return 0;
  }
 
  void make_image_texture (void)
  {
    int w_ = 8;
    int h_ = 8;
    oyPointer disp_img;
    oyPixel_t pt = oyImage_GetPixelLayout( image );
    oyDATATYPE_e data_type = oyToDataType_m( pt );
    oyConversion_s * cc;

    while (w_ < W)
    {
      w_ += w_;
      if(w_ > W)
        break;
    }
    while (h_ < H)
    {
      h_ += h_;
      if(h_ > H)
        break;
    }

    /*if(data_type == oyUINT8)
      data_type = oyUINT16;*/

    oyImage_Release( &display_image );
    display_image = oyImage_Create( w_, h_,
                         0,
                         oyChannels_m(3) | oyDataType_m(data_type),
                         oyImage_GetProfile( image ),
                         0 );

    cc = oyConversion_FromImageForDisplay( image, display_image,
                         0, 0, data_type, "", 0, 0 );
    conversion( cc );

    glGenTextures (1, &img_texture);
    glBindTexture (GL_TEXTURE_2D, img_texture);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16F_ARB, w_, h_,
    // glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE8, w_, h_,
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB16, w_, h_,
		  0, GL_RGB, GL_UNSIGNED_SHORT, NULL);

    /* size may be too big */
    check_error("glTexImage2D failed (image too large?)");

    disp_img = oyImage_GetPoint( display_image, 0,0, 0, 0 );
    glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, w_, h_,
		     GL_RGB, GL_UNSIGNED_SHORT, disp_img);
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

    /* back to texture 0 */
    glActiveTextureARB (GL_TEXTURE0_ARB);
  }

  void draw()
  {
    int w_,h_;

    W = Oy_Fl_Image_Widget::w(),
    H = Oy_Fl_Image_Widget::h();
    if(oy_display_verbose)
      printf(_DBG_FORMAT_"%dx%d+%d+%d %dx%d+%d+%d\n",_DBG_ARGS_, 
        W,H,Oy_Fl_Image_Widget::x(),Oy_Fl_Image_Widget::y(),
        Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h(),
        Oy_Fl_Image_Widget::parent()->x(), Oy_Fl_Image_Widget::parent()->y());

    if(image && clut_image && !valid())
    {
      glClearColor (0.0, 0.0, 0.0, 0.0);
      glShadeModel (GL_FLAT);
      glDisable (GL_DITHER);
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

      glClearColor (0.0, 0.0, 0.0, 0.0);
      glShadeModel (GL_FLAT);
      glDisable (GL_BLEND);
      glDisable (GL_DEPTH_TEST);

      glMatrixMode (GL_PROJECTION);
      glLoadIdentity ();
      glMatrixMode (GL_MODELVIEW);
      glLoadIdentity ();

      make_image_texture ();

      w_ = oyImage_GetWidth(  display_image );
      h_ = oyImage_GetHeight( display_image );
      glViewport(0,0, w_,h_ );

      //setupShaderTexture();

      if(GLEE_ARB_fragment_shader &&
         GLEE_ARB_shading_language_100)
      {
	//init_shaders ();
      }

      /*
      glLoadIdentity();
      glViewport(0,0,w(),h());
      glEnable(GL_DEPTH_TEST);
      glFrustum(-1,1,-1,1,2,10000);
      glTranslatef(0,0,-10);
      gl_font(FL_HELVETICA_BOLD, 16 );
      */
      fprintf( stderr, "initialise %d %d  %dx%d\n", w_, h_,
          Oy_Fl_Image_Widget::parent()->w(), Oy_Fl_Image_Widget::parent()->h());
    }
    if(conversion())
    {
      int sample_size;
      oyPixel_t pt;
      int channels = 0;
      oyDATATYPE_e data_type = oyUINT8;
      int gl_type = 0;

      pt = oyImage_GetPixelLayout( image );
      data_type = oyToDataType_m( pt );
      sample_size = oySizeofDatatype( data_type );

      drawPrepare( &image, data_type, 1 );

      pt = oyImage_GetPixelLayout( image );
      channels = oyToChannels_m( pt );

      if(channels == 3)
        gl_type = GL_RGB;
      if(channels == 4)
        gl_type = GL_RGBA;

      if(oy_display_verbose && image)
        fprintf(stdout, _DBG_FORMAT_"pixellayout: %d width: %d channels: %d\n",
                    _DBG_ARGS_,
                    pt, image->width, oyToChannels_m(pt) );

      if(!valid())
      {
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

      if(!image)
      {
        fprintf( stderr, _DBG_FORMAT_"no image!!!\n", _DBG_ARGS_);
        return;
      }

      int frame_height = OY_MIN(image->height,H),
          frame_width = OY_MIN(image->width,W);

      int pos[4] = {-2,-2,-2,-2};
      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );
      if(oy_display_verbose)
        fprintf( stderr, _DBG_FORMAT_"%d,%d %d %d\n",_DBG_ARGS_,
                 pos[0],pos[1],pos[2], pos[3] );

      /* get the data */
      if(image)
        frame_data = (char*)oyImage_GetPoint( image, 0,0, 0, 0 );

      glRasterPos2i(-frame_width, -frame_height);
      /* on osX it uses sRGB without alternative */
      if(data_type == oyUINT16)
        glDrawPixels( frame_width, frame_height, gl_type,
                      GL_UNSIGNED_SHORT, frame_data );
      else if(data_type == oyFLOAT)
        glDrawPixels( frame_width, frame_height, gl_type,
                      GL_FLOAT, frame_data );

      glGetIntegerv( GL_CURRENT_RASTER_POSITION, &pos[0] );

      if(oy_display_verbose)
        fprintf(stderr, _DBG_FORMAT_"draw %dx%d %dx%d\n",_DBG_ARGS_,
                        frame_width,frame_height, W,H );
    }
    w_ = oyImage_GetWidth(  display_image );
    h_ = oyImage_GetHeight( display_image );
    float tw = 1.0;
    float th = 1.0;

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable (GL_TEXTURE_2D);
    glBegin (GL_QUADS);
	glTexCoord2f (0.0, th);  glVertex2f (-1.0, -1.0);
	glTexCoord2f (0.0, 0.0); glVertex2f (-1.0,  1.0);
	glTexCoord2f (tw,  0.0); glVertex2f ( 1.0,  1.0);
	glTexCoord2f (tw,  th);  glVertex2f ( 1.0, -1.0);
    glEnd ();
    glFlush ();
    glDisable (GL_TEXTURE_2D);
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
                                       const char        * cc_name,
                                       oyOptions_s       * cc_options,
                                       const char        * clut_name )
  {
    oyImage_Release( &image );
    oyImage_FromFile( file_name, &image, 0 );

    int error = load3DTextureFromFile( clut_name );
    if(!error)
      fprintf(stderr, "successfully loaded clut: %s\n", clut_name );

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
