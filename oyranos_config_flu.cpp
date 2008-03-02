#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/fl_ask.H>
#include <FL/fl_show_colormap.H>
#include <FL/Fl_Output.H>

#include <FLU/Flu_Tree_Browser.h>
#include <FLU/flu_pixmaps.h>

#include "oyranos.h"
#include "oyranos_debug.h"
using namespace oyranos;

#include <iostream>

Flu_Tree_Browser *tree;

void buildOptions();
void selectDefaultProfile_callback( Fl_Widget* w, void* );
void path_callback( Fl_Widget* w, void* );

void buildBaseTree();
void buildDefaultProfilesLeaves();
void removeDefaultProfilesLeaves();
void buildPathLeaves();
void removePathLeaves();
void buildOptionsLeaves();
void buildTree();

#define OY_DEFAULTPROFILES_COUNT 6
char *default_profiles[] = {
                                "Image Profile",
                                "Workspace Profile",
                                "XYZ Profile",
                                "Lab Profile",
                                "RGB Profile",
                                "CMYK Profile",
                           };

typedef char* funcGetDefaultProfile();
funcGetDefaultProfile *functions_getDefaultProfile[] = {
                           oyGetDefaultImageProfileName,
                           oyGetDefaultWorkspaceProfileName,
                           oyGetDefaultXYZProfileName,
                           oyGetDefaultLabProfileName,
                           oyGetDefaultRGBProfileName,
                           oyGetDefaultCmykProfileName
};

typedef int funcSetDefaultProfile(const char*);
funcSetDefaultProfile *functions_setDefaultProfile[] = {
                           oySetDefaultImageProfile,
                           oySetDefaultWorkspaceProfile,
                           oySetDefaultXYZProfile,
                           oySetDefaultLabProfile,
                           oySetDefaultRGBProfile,
                           oySetDefaultCmykProfile
};


struct BuildPathLeavesC: public Fl_Button 
{
  BuildPathLeavesC() : Fl_Button(0,0,0,0) {;}
  int handle(int event)
{
  buildPathLeaves();
  Fl::pushed(0);
  removeDefaultProfilesLeaves();
  buildDefaultProfilesLeaves();

  return 1;
}
};

static BuildPathLeavesC bPL;

struct DefaultProfile: public Fl_Pack {
  Fl_Box   *box;
  Fl_Choice*choice;
  #if 0
  Fl_Input *input;
  Fl_Button*button;
  #endif
  int       type, i;

  DefaultProfile( int x, int y, int w, int h, int default_profile_type ,
                  char** names, int count )
    : Fl_Pack(x, y, w, h)
  {
    Fl_Pack::type( FL_HORIZONTAL );
    Fl_Pack::spacing(0);
	type = default_profile_type;
    box = new Fl_Box( 0, 0, 150, 20, default_profiles[type] );
    box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    choice = new Fl_Choice( 0, 0, 200, 20 );
    choice->callback( selectDefaultProfile_callback );
    DBG_PROG_V((choice->size()))
    choice->add( "[none]" );
    DBG_PROG_V((choice->size()))
    oyOpen();
    char* default_p = functions_getDefaultProfile[type]();
    int val = 0, occurence = 0;
    for (i = 0; i < count; ++i)
    {
      choice->add( names[i] );
      if(strstr(names[i], default_p) && strlen(names[i]) == strlen(default_p))
      {
        if(val) {
          ++occurence;
        } else {
          val = i+1;
          ++occurence;
        }
      }
    }
    if(occurence > 1)
      WARN_S(("multiple ocurence of default %s profile: %d times",
               default_profiles[type], occurence))
    oyClose();

    DBG_PROG_V((choice->size()))
    const Fl_Menu_Item* new_val = choice->find_item(default_p);
    int erfolg = choice->value( new_val );
    DBG_PROG_V((erfolg))
    DBG_PROG_V((val))
    DBG_PROG_V((count))
    
    #if 0
    input = new Fl_Input( 0, 0, 120, 20 );
    input->callback( selectDefaultProfile_callback );
    button = new Fl_Button( 0, 0, 30, 20, "..." );
    button->callback( selectDefaultProfile_callback );
    #endif
  }
};

struct ProfilePath: public Fl_Pack {
  Fl_Box   *box;
  int       n;
  Fl_Button*button_add, *button_remove;

  ProfilePath( int x, int y, int w, int h, int num )
    : Fl_Pack(x, y, w, h)
  {
    Fl_Pack::type( FL_HORIZONTAL );
    Fl_Pack::spacing(0);
    Fl_Group::resizable(0);
    n = num;
    char *name = oyPathName(n) ;
    std::cout << name << std::endl;
    box = new Fl_Box( 0, 0, 350, 20, name );
    box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

    button_remove = new Fl_Button( 0, 0, 30, 20, "-" );
    button_remove->callback( path_callback );
    button_add = new Fl_Button( 0, 0, 30, 20, "+" );
    button_add->callback( path_callback );
  }
};


Fl_Pixmap blue_dot( (char*const*)bluedot_xpm ), green_dot( (char*const*)greendot_xpm ), red_dot( (char*const*)reddot_xpm ), teal_dot( (char*const*)tealdot_xpm ), 
  text_doc( (char*const*)textdoc_xpm ), computer( (char*const*)computer_xpm ), book( (char*const*)book_xpm ), cd_drive( (char*const*)cd_drive_xpm ),
  arrow_closed( (char*const*)arrow_closed_xpm ), arrow_open( (char*const*)arrow_open_xpm );


void
selectDefaultProfile_callback( Fl_Widget* w, void* )
{
  DefaultProfile *dp = dynamic_cast<DefaultProfile*> (w->parent());
  if(dp) {
    Fl_Choice *c = dynamic_cast<Fl_Choice*> (w);
    if(c) {
      std::cout << c->value() << c->text() << std::endl;
      char text[64];
      oyOpen();
      int error = 0;
      if(strcmp(c->text(),"[none]") == 0)
        error = functions_setDefaultProfile[dp->type] (0);
      else
        error = functions_setDefaultProfile[dp->type] (c->text());
      oyClose();
      if(error) {
        sprintf(text, "setting %s failed!", default_profiles[dp->type]);
        fl_alert( text );
      }
    } else fl_alert( "no Fl_Choice" );
  } else fl_alert( "Select Profile" );
}

void
path_callback( Fl_Widget* w, void* )
{
  ProfilePath *pp = dynamic_cast<ProfilePath*> (w->parent());
  if(pp) {
    Fl_Button *b = dynamic_cast<Fl_Button*> (w);
    if(b && b == pp->button_add) {
      std::cout << b->value() << std::endl;
      char text[512];
      int error = oyPathAdd (pp->box->label());
      if(error) {
        sprintf(text, "error setting %s path!", pp->box->label());
        fl_alert( text );
      }
    } else 
    if(b && b == pp->button_remove) {
      std::cout << b->value() << std::endl;
      oy_debug=1;
      oyPathRemove ( pp->box->label() );
      oy_debug=0;
      Fl::pushed(&bPL);
      
      // Alle Blätter Löschen
      removePathLeaves();
      // Nun ist der Speicherblock für diese Funktion bereits freigegeben
      // und wird aber weiter benutzt
    } else fl_alert( "no Fl_Button" );
  } else fl_alert( "Path" );
}

void buildBaseTree()
{
  Flu_Tree_Browser::Node* n;

  // set the default leaf icon to be a blue dot
  tree->leaf_icon( &blue_dot );
  tree->insertion_mode( FLU_INSERT_BACK );

  tree->clear();
  tree->label( "Colour Management Settings" );
  tree->select_under_mouse( true );
  tree->always_open( true );

  n = tree->get_root();
  if( n ) n->branch_icons( &computer, &computer );


  tree->open( true );
}

void buildDefaultProfilesLeaves()
{
  char* default_profiles_dirname = "Default Profiles";
  int count = 0, i;
  char** names = oyProfileList ( 0, &count );

  for (i = 0 ; i < OY_DEFAULTPROFILES_COUNT ; ++i) {
    char  t[128];
    DefaultProfile *dp = new DefaultProfile( 0, 0, 300, 20, i, names, count );
    sprintf( t, "/%s/ ", default_profiles_dirname );
    tree->add( t, dp );
    dp->end();
  }

  for (i = 0; i < count; ++i)
    if(names[i]) free(names[i]);
  free(names);
}

void removeDefaultProfilesLeaves()
{
  char  pn[64];
  Flu_Tree_Browser::Node* n;

  sprintf( pn, "/%s/ ", "Default Profiles" );

  n = tree->find(pn);
  while (n) {
    tree->remove(n);
    n = tree->find(pn);
  }
}

void buildPathLeaves()
{
  Flu_Tree_Browser::Node* n;

  char  pn[64];
  sprintf( pn, "/%s/ ", "Profile Paths" );
  int count = oyPathsCount();
  if(count < 2)
    count = oyPathsCount();
  for (int i = 0 ; i < count ; ++i) {
    ProfilePath *pp = new ProfilePath( 0, 0, 300, 20, i );
    tree->add( pn, pp );
    pp->end();
  }
  n = tree->find( "Profile Paths" );
  if( n ) n->collapse_icons( &arrow_closed, &arrow_open );
}

void removePathLeaves()
{
  char  pn[64];
  sprintf( pn, "/%s/ ", "Profile Paths" );
  #if 0
  int count = oyPathsCount();
  for (int i = 0 ; i < count ; ++i) {
    tree->remove( pn );
  }
  #else
  Flu_Tree_Browser::Node* n;
  n = tree->find(pn);
  while(n) {
    tree->remove(n);
    n = tree->find(pn);
  }
  #endif
}


void buildOptionsLeaves()
{
  Flu_Tree_Browser::Node* n;

  Fl_Choice *c = new Fl_Choice( 0, 0, 100, 20 );
  c->add( "Perceptual" );
  c->add( "Relative Colorimetric" );
  c->add( "Saturation" );
  c->add( "Absolute Colorimetric" );
  c->value( 1 );
  n = tree->add( "Options (dont do anything)/Default Rendering Intent", c );
  if( n ) n->expand_to_width( true );
  if( n ) n->leaf_icon( NULL );
  n = tree->find( "Options" );
  if( n ) n->collapse_icons( &arrow_closed, &arrow_open );

  tree->collapse_time( 0.2 );
  tree->frame_rate( 60.0 );
  //tree->animate(true);
  tree->redraw();
}

void buildTree()
{
  buildBaseTree();

  Flu_Tree_Browser::Node* n;
  char* default_profiles_dirname = "/Default Profiles/";
  tree->add( default_profiles_dirname );
  n = tree->find( default_profiles_dirname );
  if( n ) n->collapse_icons( &arrow_closed, &arrow_open );

  buildPathLeaves();
  buildDefaultProfilesLeaves();
  buildOptionsLeaves();
}

int main( int argc, char **argv )
{
  if(getenv("OYRANOS_DEBUG") > 0)
    oy_debug = 1;

  FL_NORMAL_SIZE = 12;
  Fl_Double_Window *win = new Fl_Double_Window( 500, 400, "Oyranos Colour Management" );

  tree = new Flu_Tree_Browser( 0, 0, 500, 400 );
  tree->allow_dnd( true );
  //tree->when( FL_WHEN_RELEASE );
  //tree->color( FL_GRAY );

  win->end();
  win->resizable( tree );
  win->show( argc, argv );

  buildTree();

  //Fl::dnd_text_ops( 1 );

  printf( "sizeof(Flu_Tree_Browser::Node): %d\n", sizeof(Flu_Tree_Browser::Node) );

  return( Fl::run() );
}
