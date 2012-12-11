/** @file oyranos_widgets_fltk.cxx
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    additional widgets for the FLTK toolkit
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/10
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "oyranos_widgets_fltk.h"
#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Scroll.H>


#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"


using namespace oyranos::forms;


OyFl_Box_c::OyFl_Box_c(int x, int y, int w, int h , const char *t )
  : Fl_Box(x,y,w,h,t)
{
  hint_callback = 0;

  if(t && t[0])
  {
    label_orig = new char [strlen(t)+1];
    //printf("%s %d\n",t, strlen(t));
    sprintf(label_orig, "%s", t);
  } else
    label_orig = 0;
}

void OyFl_Box_c::draw() {
  int wl=0, hl=0, // have to be zeroed
       ws; // ws - label widgets start width
  char *txt = 0;
  int len = 0;
  int last_utf8_len = 0;
  if(label_orig)
  {
    ws = w();
    if(label_orig)
      Fl_Box::label( label_orig );
    measure_label(wl,hl);
    if(wl < ws)
    {
      if(strlen(label_orig))
        Fl_Box::copy_label(label_orig);
    } else {

      txt = new char [strlen(label_orig)+1];
      sprintf(txt, "%s", label_orig);
      Fl_Box::copy_label(txt);
      do {
        //txt = (char*)Fl_Box::label();
        len = strlen(txt);
        if(len > 3)
        {
/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
          // now we have to get the last character length
          int ilen = 0;
          last_utf8_len = 0;
          do {
            last_utf8_len = 1 + trailingBytesForUTF8[ (unsigned char)txt[ilen] ];
            ilen += last_utf8_len;
          } while (len-3 > ilen);
          sprintf( &txt[ilen - last_utf8_len], "..." );
          Fl_Box::copy_label(txt);
        } else
          break;
        measure_label(wl,hl);
      } while(wl > ws);

      //Fl_Box::label(txt);
      if(txt)
        delete [] txt;
      txt = 0;
    }
  }
#ifdef DEBUG_
  if(label_orig)
    if(strlen(label_orig))
      printf("%d %s %d %d/%d %d\n",__LINE__,Fl_Box::label(), strlen(Fl_Box::label()), wl, ws, last_utf8_len);
#endif
  Fl_Box::draw();
}

const char * OyFl_Box_c::label() {
  return label_orig;
}

void OyFl_Box_c::label(const char * l) {
  if(label_orig) delete [] label_orig;
  label_orig = 0;
  if(l && l[0])
  {
    label_orig = new char [strlen(l)+1];
    sprintf(label_orig, "%s", l);
  }
  Fl_Box::label( l );
}

void OyFl_Box_c::copy_label(const char* l) {
  int len;
  if(label_orig) delete [] label_orig;
  label_orig = 0;
  if(l && l[0])
  {
    len = strlen(l) + 1;
    label_orig = new char [len];
    sprintf(label_orig, "%s", l);
  }
  Fl_Box::copy_label(l);
}

int OyFl_Box_c::handle(int event)
{
    int result = Fl_Box::handle(event);
    switch (event)
    {
      case FL_ENTER:
           if(hint_callback)
           {
             oyFormsFltkHelpViewCallback_f userCallback = 0;
             userCallback =(oyFormsFltkHelpViewCallback_f)
                                                        hint_callback->callback;
             if(userCallback)
               userCallback( hint_callback->data, (const char*)user_data() );
           }
           break;

      case FL_LEAVE:
           if(hint_callback)
           {
             oyFormsFltkHelpViewCallback_f userCallback = 0;
             userCallback =(oyFormsFltkHelpViewCallback_f)
                                                        hint_callback->callback;
             if(userCallback)
               userCallback( hint_callback->data, 0 );
           }
           break;
    }
    return result;
}

OyFl_Pack_c::OyFl_Pack_c(int x, int y, int w, int h , const char *t ) : Fl_Pack(x,y,w,h,t) {
}

void OyFl_Pack_c::draw() {
  int nw = 0, nh = 0;
  Fl_Scroll * scroll = dynamic_cast <Fl_Scroll*> (parent());
  if( scroll )
  {
    nw = parent()->w() - Fl::box_dw( parent()->box() ) - scroll->scrollbar.w() - spacing();
  } else {
    nw = parent()->w() - Fl::box_dw( parent()->box() );
  }
  w( nw );


  nh = parent()->h() - Fl::box_dh( parent()->box() );
  if( horizontal() )
    Fl_Pack::size( nw, Fl_Pack::h() );
  else
    Fl_Pack::size( Fl_Pack::w(), nh );

  // prearrange if one widget is resizeable
  Fl_Widget *re = resizable();
  if( re )
  {
    int n = children();
    int pos_x = Fl::box_dx(box()) + spacing(),
        pos_y = Fl::box_dy(box()) + spacing(),
        pos_x2 = pos_x + w() - Fl::box_dw(box()) - spacing(),
        pos_y2 = pos_y + h() - Fl::box_dh(box()) - spacing();
    int re_i = 0;
    
    for( int i = 0; i < n; ++i )
    {
      Fl_Widget *o = child( i );
      if( o == re )
      {
        re_i = i;
        break;
      }
      
      if( horizontal() )
      {
        o->position( pos_x, o->y() );
        pos_x += o->w() + spacing();
      } else {
        o->position( o->x(), pos_y );
        pos_y += o->h() + spacing();
      }
    }
    
    for( int i = n - 1; i > re_i; --i )
    {
      Fl_Widget *o = child( i );
      if( horizontal() )
      {
        o->position( pos_x2 - o->w(), o->y() );
        pos_x2 -= o->w() + spacing();
      } else {
        o->position( o->x(), pos_y2 - o->h());
        pos_y2 -= o->h() + spacing();
      }
    }
    
    if(horizontal())
      re->resize( pos_x, re->y(), pos_x2-pos_x, re->h() );
    else
      re->resize( re->x(), pos_y, re->w(), pos_y2-pos_y );

  }


  Fl_Pack::draw();
}



