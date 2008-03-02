/*
 * fl_i18n is a internationalisation helper library for FLTK.
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann 
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * -----------------------------------------------------------------------------
 *
 * I18N initialisation for FLTK
 * 
 */

/* Date:      Januar 2006 */

#ifndef FL_I18N_H
#define FL_I18N_H

#define FL_I18N_VERSION 101

# include <libintl.h>
# define _(text) threadGettext(text)
const char* threadGettext( const char* text);


/** tell about the locale path and select locale from environment */
void        fl_initialise_locale   ( const char   *domain,
                                     const char   *locale_paths,
                                     int           set_locale );
/** help with searching for the right locale path */
int         fl_search_locale_path  ( int           n_locale_paths,
                                     const char  **locale_paths,
                                     const char   *search_lang, /*"de" or "it"*/
                                     const char   *app_name);   /*"my_fl_app"*/


#include <FL/Fl_Menu_Item.H>
/* I18N in fl_i18n.cxx */
void        fl_translate_menue     ( Fl_Menu_Item *menueleiste );
void        fl_translate_file_chooser ();

#endif /* FL_I18N_H */
