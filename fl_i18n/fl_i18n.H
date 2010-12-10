/*
 * fl_i18n is a internationalisation helper library for FLTK.
 * 
 * Copyright (C) 2004-2008  Kai-Uwe Behrmann 
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

/* Date:      Januar 2006-2007 */

#ifndef FL_I18N_H
#define FL_I18N_H

#define FL_I18N_VERSION 101

#if defined(WIN32) && !defined(__MINGW32__)
#define USE_GETTEXT 1
#endif

#ifdef _
#undef _
#endif
#ifdef USE_GETTEXT
# include <libintl.h>
#include <FL/x.H>
# ifndef _
#  define _(text) threadGettext(text)
# endif
#else
# ifndef _
#  define _(text) text
# endif
#endif

const char* threadGettext( const char* text);

/** 
 *   fl_i18n_codeset is set to the choosen codeset after the call to 
 *   fl_initialise_locale. e.g. "UTF-8"
 *   It can remain unset by fl_i18n.
 */
extern const char *fl_i18n_codeset;

typedef enum {
  FL_I18N_SETCODESET_NO,
  FL_I18N_SETCODESET_SELECT,
  FL_I18N_SETCODESET_UTF8
} FL_I18N_SETCODESET;

/** @func   fl_initialise_locale
 *  @brief  Tell about the locale path and select locale from environment.
 *
 *  You might call fl_search_locale_path previously to find locale_path.
 */
int         fl_initialise_locale   ( const char   *domain,
                                     const char   *locale_path,
                                     FL_I18N_SETCODESET set_codeset );

/** @func   fl_search_locale_path
 *  @brief  Help with searching for the right locale path.
 *
 *  fl_search_locale_path will search for each
 *  locale_paths[i]/search_lang/LC_MESSAGES/app_name.mo
 *
 *  param   n_locale_paths           number of paths in locale_paths
 *  param   locale_paths             path string array
 *  param   search_lang              e.g. "de" or "it" ...
 *  param   app_name                 e.g. "my_fl_app"
 */
int         fl_search_locale_path  ( int           n_locale_paths,
                                     const char  **locale_paths,
                                     const char   *search_lang,
                                     const char   *app_name);


#include <FL/Fl_Menu_Item.H>
/* I18N in fl_i18n.cxx */
void        fl_translate_menue     ( Fl_Menu_Item *menueleiste );
void        fl_translate_file_chooser ();

#endif /* FL_I18N_H */
