#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Counter.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Round_Button.H>
#include <FL/fl_ask.H>
#include <FL/fl_show_colormap.H>

#include <FLU/Flu_Tree_Browser.h>
#include <FLU/flu_pixmaps.h>

#include <iostream>

Flu_Tree_Browser *tree;

void makeTree();

inline void rebuildTreeCB( Fl_Widget*, void* )
{ makeTree(); }

void deleteNodeCB( Fl_Widget*, void* )
{
  Flu_Tree_Browser::Node *n = tree->get_selected( 1 );
  while( n )
    {
      tree->remove( n );
      n = tree->get_selected( 1 );
    }
}

Fl_Check_Button *showConnectors, *showRoot, *rootAlwaysOpen, *showLeaves, *showBranches, *openOnSelect,
  *active, *shaded, *animate;
Fl_Float_Input *collapseTime, *frameRate;
Fl_Input *newNode;
Fl_Counter *hGap, *vGap, *wGap;
Fl_Choice *selectionMode, *selectionDragMode, *insertionMode;
Fl_Button *shadedEvenColor, *shadedOddColor;
Fl_Button *rebuildTree, *deleteNode;

Fl_Pixmap blue_dot( (char*const*)bluedot_xpm ), green_dot( (char*const*)greendot_xpm ), red_dot( (char*const*)reddot_xpm ), teal_dot( (char*const*)tealdot_xpm ), 
  text_doc( (char*const*)textdoc_xpm ), computer( (char*const*)computer_xpm ), book( (char*const*)book_xpm ), cd_drive( (char*const*)cd_drive_xpm ),
  arrow_closed( (char*const*)arrow_closed_xpm ), arrow_open( (char*const*)arrow_open_xpm );

static void shadedColorCB( Fl_Widget *w, void* )
{
  Fl_Button *b = (Fl_Button*)w;
  b->color( fl_show_colormap( b->color() ) );
  b->redraw();
  tree->shaded_entry_colors( shadedEvenColor->color(), shadedOddColor->color() );
  tree->redraw();
}

static void callback( Fl_Widget*, void* )
{
  if( showRoot->value() )
    rootAlwaysOpen->activate();
  else
    rootAlwaysOpen->deactivate();

  if( shaded->value() )
    {
      shadedEvenColor->activate();
      shadedOddColor->activate();
      tree->shaded_entry_colors( shadedEvenColor->color(), shadedOddColor->color() );
    }
  else
    {
      shadedEvenColor->deactivate();
      shadedOddColor->deactivate();
      tree->shaded_entry_colors( FL_WHITE, FL_WHITE );
    }

  if( animate->value() )
    {
      collapseTime->activate();
      frameRate->activate();
    }
  else
    {
      collapseTime->deactivate();
      frameRate->deactivate();
    }

  tree->animate( animate->value() );
  tree->collapse_time( atof( collapseTime->value() ) );
  tree->frame_rate( atof( frameRate->value() ) );
  tree->show_connectors( showConnectors->value() );
  tree->show_root( showRoot->value() );
  //tree->get_root()->always_open( rootAlwaysOpen->value() );
  tree->show_leaves( showLeaves->value() );
  tree->show_branches( showBranches->value() );
  tree->horizontal_gap( (int)hGap->value() );
  tree->vertical_gap( (int)vGap->value() );
  tree->widget_gap( (int)wGap->value() );
  tree->open_on_select( openOnSelect->value() );

  if( active->value() )
    tree->activate();
  else
    tree->deactivate();

  tree->redraw();
}

static void mode_callback( Fl_Widget*, void* )
{
  switch( selectionMode->value() )
    {
    case 0:
      tree->selection_mode( FLU_NO_SELECT );
      break;
    case 1:
      tree->selection_mode( FLU_SINGLE_SELECT );
      break;
    case 2:
      tree->selection_mode( FLU_MULTI_SELECT );
      break;
    }

  switch( selectionDragMode->value() )
    {
    case 0:
      tree->selection_drag_mode( FLU_DRAG_IGNORE );
      break;
    case 1:
      tree->selection_drag_mode( FLU_DRAG_TO_SELECT );
      break;
    case 2:
      tree->selection_drag_mode( FLU_DRAG_TO_MOVE );
      break;
    }

  switch( insertionMode->value() )
    {
    case 0:
      tree->insertion_mode( FLU_INSERT_FRONT );
      break;
    case 1:
      tree->insertion_mode( FLU_INSERT_BACK );
      break;
    case 2:
      tree->insertion_mode( FLU_INSERT_SORTED );
      break;
    case 3:
      tree->insertion_mode( FLU_INSERT_SORTED_REVERSE );
      break;
    }

  tree->redraw();
}

static void
selectDefaultProfile_callback( Fl_Widget*, void* )
{
  fl_alert( "Select Profile" );
}


static void button_callback( Fl_Widget*, void* )
{
  fl_alert( "pushed" );
}

static void tree_callback( Fl_Widget* w, void* )
{
  Flu_Tree_Browser *t = (Flu_Tree_Browser*)w;

  int reason = t->callback_reason();
  Flu_Tree_Browser::Node *n = t->callback_node();

  if( tree->num_selected() )
    deleteNode->activate();
  else
    deleteNode->deactivate();

  switch( reason )
    {
    case FLU_HILIGHTED:
      printf( "%s hilighted\n", n->label() );
      break;

    case FLU_UNHILIGHTED:
      printf( "%s unhilighted\n", n->label() );
      break;

    case FLU_SELECTED:
      printf( "%s selected\n", n->label() );
      break;

    case FLU_UNSELECTED:
      printf( "%s unselected\n", n->label() );
      break;

    case FLU_OPENED:
      printf( "%s opened\n", n->label() );
      break;

    case FLU_CLOSED:
      printf( "%s closed\n", n->label() );
      break;

    case FLU_DOUBLE_CLICK:
      printf( "%s double-clicked\n", n->label() );
      break;

    case FLU_WIDGET_CALLBACK:
      printf( "%s widget callback\n", n->label() );
      break;

    case FLU_MOVED_NODE:
      printf( "%s moved\n", n->label() );
      break;

    case FLU_NEW_NODE:
      printf( "node '%s' added to the tree\n", n->label() );
      break;
    }
}

void makeTree()
{
  Flu_Tree_Browser::Node* n;

  // set the default leaf icon to be a blue dot
  tree->leaf_icon( &blue_dot );

  tree->clear();
  tree->label( "Colour Management Settings" );
  tree->select_under_mouse( true );
  tree->always_open( true );

  n = tree->get_root();
  if( n ) n->branch_icons( &computer, &computer );

  //tree->move_only_same_group( true );

  /*
  char buf[32];
  for( int i = 0; i < 1000; i++ )
    {
      sprintf( buf, "%d", i );
      tree->add( "/", buf );
    }
  return;
  */

  n = tree->add( "Text Entries!" );
  n = tree->add( "/widgets!/" );
  n = tree->add_branch( "/widgets!" );
  n->branch_icon( NULL );
  tree->add_branch( "two" );
  n = tree->add( "/widgets!/text item" );
  if( n ) n->leaf_icon( &text_doc );

  tree->open( true );

  // test the depth-first traversal interface
  /*
  printf( "----------------\n" );
  n = tree->last();//tree->first();
  while( n )
    {
      printf( "%s\n", n->label() );
      n = n->previous();//n->next();
    }
  */

  //return;

  char *default_profiles[] = {
                                "Image Profile",
                                "Workspace Profile",
                                "XYZ Profile",
                                "Lab Profile",
                                "RGB Profile",
                                "CMYK Profile",
                               };
  //std::cout << sizeof(default_profiles) << std::endl;
  char* default_profiles_dirname = "Default Profiles";

  for (int i = 5 ; i >= 0 ; --i) {
    char  t[128];
    sprintf( t, "/%s/ ", default_profiles_dirname/*, default_profiles[i]*/ );
    Fl_Pack *p  = new Fl_Pack( 0, 0, 300, 20 );
    p->type( FL_HORIZONTAL );
    p->spacing(0);
    Fl_Box *box = new Fl_Box( 0, 0, 150, 20, default_profiles[i] );
    box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    Fl_Input *i = new Fl_Input( 0, 0, 120, 20 );
    Fl_Button *b = new Fl_Button( 0, 0, 30, 20, "..." );
    b->callback( selectDefaultProfile_callback );
    //Fl_Group::current()->resizable(b);

    tree->add( t, p);
    p->end();
  }
  n = tree->find( default_profiles_dirname );
  if( n ) n->collapse_icons( &arrow_closed, &arrow_open );

  Fl_Choice *c = new Fl_Choice( 0, 0, 100, 20 );
  c->add( "Perceptual" );
  c->add( "Relative Colorimetric" );
  c->add( "Saturation" );
  c->add( "Absolute Colorimetric" );
  c->value( 0 );
  n = tree->add( "Settings/Default Rendering Intent", c );
  if( n ) n->expand_to_width( true );
  if( n ) n->leaf_icon( NULL );
  n = tree->find( "Settings" );
  if( n ) n->collapse_icons( &arrow_closed, &arrow_open );

  tree->redraw();
}

int main( int argc, char **argv )
{
  //FL_NORMAL_SIZE = 12;
  Fl_Double_Window *win = new Fl_Double_Window( 500, 460, "Flu_Tree_Browser_Try" );

  tree = new Flu_Tree_Browser( 200, 0, 300, 460 );
  tree->box( FL_DOWN_BOX );
  tree->allow_dnd( true );
  //tree->when( FL_WHEN_RELEASE );
  tree->callback( tree_callback );
  //tree->color( FL_GRAY );

  Fl_Group *g = new Fl_Group( 0, 0, 200, 460 );
  g->resizable( NULL );

  showConnectors = new Fl_Check_Button( 10, 10, 140, 20, "Show Connectors" );
  showConnectors->value( tree->show_connectors() );
  showConnectors->callback( callback, 0 );

  showRoot = new Fl_Check_Button( 10, 30, 100, 20, "Show Root" );
  showRoot->value( tree->show_root() );
  showRoot->callback( callback, 0 );

  rootAlwaysOpen = new Fl_Check_Button( 10, 50, 140, 20, "Root Always Open" );
  rootAlwaysOpen->value( tree->get_root()->always_open() );
  rootAlwaysOpen->callback( callback, 0 );

  showLeaves = new Fl_Check_Button( 10, 70, 110, 20, "Show Leaves" );
  showLeaves->value( tree->show_leaves() );
  showLeaves->callback( callback, 0 );

  showBranches = new Fl_Check_Button( 10, 90, 120, 20, "Show Branches" );
  showBranches->value( tree->show_branches() );
  showBranches->callback( callback, 0 );

  openOnSelect = new Fl_Check_Button( 10, 110, 130, 20, "Open On Select" );
  openOnSelect->value( tree->open_on_select() );
  openOnSelect->callback( callback, 0 );

  active = new Fl_Check_Button( 10, 130, 100, 20, "Active" );
  active->value( tree->active() );
  active->callback( callback, 0 );

  animate = new Fl_Check_Button( 10, 150, 100, 20, "Animate" );
  animate->value( tree->animate() );
  animate->callback( callback, 0 );

  collapseTime = new Fl_Float_Input( 55, 170, 50, 20, "Time" );
  {
    char buf[32]; sprintf( buf, "%.2f", tree->collapse_time() );
    collapseTime->value( buf );
  }
  collapseTime->callback( callback, 0 );
  collapseTime->deactivate();

  frameRate = new Fl_Float_Input( 140, 170, 50, 20, "FPS" );
  {
    char buf[32]; sprintf( buf, "%.2f", tree->frame_rate() );
    frameRate->value( buf );
  }
  frameRate->callback( callback, 0 );
  frameRate->deactivate();

  shaded = new Fl_Check_Button( 10, 190, 130, 20, "Shaded Entries" );
  shaded->value( 0 );
  shaded->callback( callback, 0 );

  shadedEvenColor = new Fl_Button( 70, 210, 20, 20, "Even" );
  shadedEvenColor->align( FL_ALIGN_LEFT );
  shadedEvenColor->color( FL_LIGHT2 );
  shadedEvenColor->callback( shadedColorCB );
  shadedEvenColor->deactivate();

  shadedOddColor = new Fl_Button( 150, 210, 20, 20, "Odd" );
  shadedOddColor->align( FL_ALIGN_LEFT );
  shadedOddColor->color( FL_WHITE );
  shadedOddColor->callback( shadedColorCB );
  shadedOddColor->deactivate();

  selectionMode = new Fl_Choice( 110, 235, 85, 25, "Selection Mode" );
  selectionMode->add( "NO_SELECT" );
  selectionMode->add( "SINGLE_SELECT" );
  selectionMode->add( "MULTI_SELECT" );
  selectionMode->value( 2 );
  selectionMode->callback( mode_callback, 0 );

  selectionDragMode = new Fl_Choice( 110, 265, 85, 25, "Selection Drag" );
  selectionDragMode->add( "DRAG_IGNORE" );
  selectionDragMode->add( "DRAG_TO_SELECT" );
  if( tree->have_dnd() )
    selectionDragMode->add( "DRAG_TO_MOVE" );
  selectionDragMode->value( 1 );
  selectionDragMode->callback( mode_callback, 0 );

  insertionMode = new Fl_Choice( 110, 295, 85, 25, "Insertion Mode" );
  insertionMode->add( "INSERT_FRONT" );
  insertionMode->add( "INSERT_BACK" );
  insertionMode->add( "INSERT_SORTED" );
  insertionMode->add( "INSERT_SORTED_REVERSE" );
  insertionMode->value( 2 );
  insertionMode->callback( mode_callback, 0 );

  hGap = new Fl_Counter( 105, 325, 90, 20, "Horizontal Gap" );
  hGap->type( FL_SIMPLE_COUNTER );
  hGap->align( FL_ALIGN_LEFT );
  hGap->precision( 0 );
  hGap->range( 0, 20 );
  hGap->value( tree->horizontal_gap() );
  hGap->callback( callback, 0 );

  vGap = new Fl_Counter( 105, 350, 90, 20, "Vertical Gap" );
  vGap->type( FL_SIMPLE_COUNTER );
  vGap->align( FL_ALIGN_LEFT );
  vGap->precision( 0 );
  vGap->range( 0, 20 );
  vGap->value( tree->vertical_gap() );
  vGap->callback( callback, 0 );

  wGap = new Fl_Counter( 105, 375, 90, 20, "Widget Gap" );
  wGap->type( FL_SIMPLE_COUNTER );
  wGap->align( FL_ALIGN_LEFT );
  wGap->precision( 0 );
  wGap->range( 0, 20 );
  wGap->value( tree->widget_gap() );
  wGap->callback( callback, 0 );

  if( tree->have_dnd() )
    newNode = new Fl_Input( 50, 400, 50, 20, "New" );

  rebuildTree = new Fl_Button( 10, 405, 120, 20, "Rebuild Tree" );
  rebuildTree->callback( rebuildTreeCB, NULL );

  deleteNode = new Fl_Button( 10, 430, 120, 20, "Delete Selected" );
  deleteNode->callback( deleteNodeCB, NULL );

  g->end();

  win->end();
  win->resizable( tree );
  win->show( argc, argv );

  makeTree();

  //Fl::dnd_text_ops( 1 );

  printf( "sizeof(Flu_Tree_Browser::Node): %d\n", sizeof(Flu_Tree_Browser::Node) );

  return( Fl::run() );
}
