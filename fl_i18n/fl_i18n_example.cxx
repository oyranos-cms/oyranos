// generated by Fast Light User Interface Designer (fluid) version 1.0107

#include <fl_i18n.H>
#include "fl_i18n_example.h"
#include <FL/fl_draw.H>//#include "fl_i18n.H"
#include <cstdio>
#ifndef WIN32
#include <langinfo.h>
#endif

void MyBox::draw() {
  fl_font( FL_HELVETICA, 24);
  
  int tw,th;
  fl_measure("Hallo", tw, th);

  fl_draw( _("Hello, world!"), x() + 10, y() + 5 + th );
}

MyBox::MyBox( int x, int y, int w, int h, const char *label): Fl_Box(x,y,w,h,label) {
}

MyBox::MyBox( int x, int y, int w, int h ): Fl_Box(x,y,w,h) {
}

MyBox *b=(MyBox *)0;

static void cb_Quit(Fl_Menu_*, void*) {
  b->window()->hide();
}

Fl_Menu_Item menu_[] = {
 {_("File"), 0,  0, 0, 64, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Open"), 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Save"), 0,  0, 0, 128, FL_NORMAL_LABEL, 0, 14, 0},
 {_("Quit"), 0,  (Fl_Callback*)cb_Quit, 0, 0, FL_NORMAL_LABEL, 0, 14, 0},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};

int main(int argc, char **argv) {
  Fl_Double_Window* w;
  const char *locale_paths[1] = {"./po"};
  const char *domain = {"fl_i18n"};

  fl_font( FL_HELVETICA, 24);
  fl_font( FL_COURIER, 24);
  printf("%s %s\n", Fl::get_font((Fl_Font)0), Fl::get_font_name((Fl_Font)0,0));

  if ( fl_search_locale_path  ( 1,
                                locale_paths,
                                "de",
                                domain) >= 0 )
  {
#if defined(_Xutf8_h) || HAVE_FLTK_UTF8
    FL_I18N_SETCODESET set_charset = FL_I18N_SETCODESET_UTF8;
#else
    FL_I18N_SETCODESET set_charset = FL_I18N_SETCODESET_SELECT;
#endif
    int err = fl_initialise_locale ( domain, locale_paths[0],
                                     set_charset );
    if(err) {
      fprintf( stderr,"i18n initialisation failed");
    } else
      fprintf( stderr, "Locale found in %s\n", locale_paths[0]);
  } else
    fprintf( stderr, "Locale not found in %s\n", locale_paths[0]);

#ifndef WIN32
  fprintf( stdout, "nl_langinfo(CODESET): %s\n", nl_langinfo(CODESET) );
#endif

  fprintf( stdout, "%s\n",_("Hello, world!"));
  { Fl_Double_Window* o = new Fl_Double_Window(235, 170, _("fl_i18n_example"));
    w = o;
    o->align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE);
    new Fl_Button(25, 96, 190, 69, _("Hello, world!"));
    { MyBox* o = b = new MyBox(0, 27, 235, 66);
      o->box(FL_FLAT_BOX);
      o->color(FL_BACKGROUND_COLOR);
      o->selection_color(FL_BACKGROUND_COLOR);
      o->labeltype(FL_NORMAL_LABEL);
      o->labelfont(0);
      o->labelsize(14);
      o->labelcolor(FL_FOREGROUND_COLOR);
      o->align(FL_ALIGN_CENTER);
      o->when(FL_WHEN_RELEASE);
    }
    { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 235, 25);
      o->menu(menu_);
    }
    o->end();
  }
  /*fl_font( FL_COURIER, 24);
  
  int tw,th;
  fl_meashure("Hallo", &tw, &th);

  fl_draw( "Hello, world!", b->x()+10, b->y()+20 + th );*/
  w->show(argc, argv);
  return Fl::run();
}
