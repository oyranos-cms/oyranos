/** @file oyranos_widgets_fltk.cxx
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
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
//#include "../fl_i18n/fl_i18n.H"
#include <FL/Flmm_Tabs.H>


#include "config.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"


using namespace oyranos;


OyFl_Box_c::OyFl_Box_c(int x, int y, int w, int h , const char *t )
  : Fl_Box(x,y,w,h,t)
{
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


#if 0
OyOption_c::OyOption_c( int x, int y, int w, int h, const char *name,
  int choices_n,
  const char **choices,
  int current,
  const char *tooltip,
  int flags) : OyFl_Pack_c(x, y, w, h) {
  Fl_Pack::type( FL_HORIZONTAL );
  Fl_Pack::spacing(H_SPACING);

  // == Behaviour ==
  {
    // name box
    box = new OyFl_Box_c( x, y,
                      w - SELECT_WIDTH - BUTTON_HEIGHT - 4*H_SPACING, BUTTON_HEIGHT,
                      name );
    box->tooltip( tooltip );
    box->align(FL_ALIGN_LEFT | FL_ALIGN_CLIP|FL_ALIGN_INSIDE);

    if( flags & OY_LAYOUT_NO_CHOICES )
    {
      const char* default_p = NULL;
      if(current >= 0)
        default_p = choices[current];
      if(default_p) {
        DBG_PROG_S( (default_p) )
      } else
        default_p = "";
      if(default_p) {
        box = new OyFl_Box_c( w - BOX_WIDTH - SELECT_WIDTH - BUTTON_HEIGHT - 3*H_SPACING, y,
                          190, BUTTON_HEIGHT, default_p );
        box->align( FL_ALIGN_LEFT | FL_ALIGN_CLIP | FL_ALIGN_INSIDE );

        Fl_Group::end();
        Fl_Group::resizable( box );
        return;
      }
    }

    // Choice
    choice = new Fl_Choice( w - SELECT_WIDTH - H_SPACING, y,
                            SELECT_WIDTH + BUTTON_HEIGHT + H_SPACING, h );
    /*choice->callback( selectBehaviourCallback );
    choice->tooltip( tooltip );*/

    // Set choice
    for (i = 0; i < choices_n; ++i)
    {
      choice->add( choices[i] );
    }
    choice->value( current );

    // identify
    //choice->user_data( (void*)(intptr_t)option );
  }

  
  end();
  resizable( box );
}

#endif

