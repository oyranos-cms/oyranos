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
 * Internationalisation functions
 * 
 */

#include "fl_i18n.H"

#ifdef USE_GETTEXT
#include <locale.h>
#include <libintl.h>
#endif
#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <cstring>

#ifdef USE_GETTEXT
# if !defined(WIN32) || (defined(WIN32) && defined(__MINGW32__))
#  define fl_i18n_printf(text) printf text
# else
#  define fl_i18n_printf(text)
# endif
   static int lc = LC_ALL;
#else
# define fl_i18n_printf(text)
#endif

const char *fl_i18n_codeset = 0;

/* include pthread.h here for threads support */
#ifdef USE_THREADS
#include "threads.h"
#endif

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif


/* The following macros belong to Kai-Uwe's debug environment. */
#ifdef DEBUG_
#include "../icc_utils.h"
#else
#ifndef icc_strdup_m
char* icc_strdup_m (const char* t)
{
  size_t len = 0;
  char *temp = NULL;

  if(t)
    len = strlen(t);
  if(len)
  {
    temp = (char*) malloc(len+1);
    memcpy( temp, t, len );
    temp[len] = 0;
  }
  return temp;
}
#endif
/*extern int icc_debug;*/
#endif
#ifndef DBG_PROG_START
#define DBG_PROG_START
#endif
#ifndef DBG_PROG
#define DBG_PROG
#endif
#ifndef DBG_PROG_V
#define DBG_PROG_V(text)
#endif
#ifndef DBG_PROG_S
#define DBG_PROG_S(text)
#endif
#ifndef DBG_PROG_ENDE
#define DBG_PROG_ENDE
#endif
#ifndef WARN_S
#define WARN_S(text)
#endif


#define TEXTLEN 128

#ifdef USE_GETTEXT
int
fl_set_codeset_    ( const char* lang, const char* codeset_,
                     char* locale, char* codeset,
                     FL_I18N_SETCODESET set_codeset )
{
    if( strlen(locale) )
    {
      char* pos = strstr(locale, lang);
      if(pos != 0)
      {
        /* 1 a. select an appropriate charset (needed for non UTF-8 fltk/gtk1)*/
        sprintf (codeset, codeset_); DBG_PROG_V( locale <<" "<< strrchr(locale,'.'))
 
          /* merge charset with locale string */
        if(set_codeset != FL_I18N_SETCODESET_NO)
        {
          if((pos = strrchr(locale,'.')) != 0)
          {
            *pos = 0; DBG_PROG_V( locale )
          } else
            pos = & locale[strlen(locale)];
          if(set_codeset == FL_I18N_SETCODESET_SELECT)
            snprintf(pos, TEXTLEN-strlen(locale), ".%s",codeset);
          else if(set_codeset == FL_I18N_SETCODESET_UTF8)
            snprintf(pos, TEXTLEN-strlen(locale), ".%s","UTF-8");
        }
        DBG_PROG_V( locale )

        /* 1b. set correct environment variable LANG */
        {
          char *settxt = (char*)calloc(sizeof(char), TEXTLEN);
          if(!settxt) {
            fl_i18n_printf(("%s:%d no memory available",__FILE__,__LINE__));
            return 1;
          }
          snprintf( settxt, 63, "LANG=%s", locale );
#ifdef __APPLE__
          putenv( settxt );
#endif
          free(settxt);
        }
#ifdef _POSIX_C_SOURCE
#ifdef __APPLE__
        setenv("LANG", locale, 1); /* setenv is Posix, but might be ignored */
#endif
#endif

        /* 1c. set the locale info after LANG */
        {
        char* ptr = NULL;
        switch(set_codeset)
        {
        case FL_I18N_SETCODESET_SELECT:
            ptr = setlocale (lc, "");
            break;
        case FL_I18N_SETCODESET_UTF8:
            ptr = setlocale (lc, locale);
            break;
        default: break;
        }
        if(ptr) snprintf( locale, TEXTLEN, ptr); DBG_PROG_V( locale )
        }
      }
    }
  return 0;
}
#endif

#ifdef WIN32
#define DIR_SEPARATOR "\\"
#else
#define DIR_SEPARATOR "/"
#endif

signed int
fl_search_locale_path (int n_places, const char **locale_paths,
                    const char *search_lang, const char *app_name)
{
  /* search in a given set of locale paths for a valid locale file */
  for (int i = 0; i < n_places; ++i)
  {
    if(locale_paths[i])
    {
      char *test = (char*) calloc(sizeof(char), 1024);
      FILE *fp = 0;
      if(!test) {
        fl_i18n_printf(("%s:%d Could not allocate enough memory.",
            __FILE__,__LINE__));
        return -1;
      }
      /* construct the full path to a possibly valid locale file */
	  if(strlen(locale_paths[i])+strlen(app_name) < 1000)
        sprintf(test, "%s%s%s%sLC_MESSAGES%s%s.mo",
                           locale_paths[i], DIR_SEPARATOR,
                           search_lang, DIR_SEPARATOR, DIR_SEPARATOR, app_name);
      /* test the file for existence */
      fp = fopen(test, "r");
      if(fp)
      {
        fclose(fp);
        /* tell about the hit place */
        return i;
      }
      free(test);
    }
  }
  return -1;
}

int
fl_initialise_locale( const char *domain, const char *locale_path,
                      FL_I18N_SETCODESET set_codeset )
{

#ifdef USE_GETTEXT
  DBG_PROG_START
  char *locale = (char*) calloc(sizeof(char), TEXTLEN);
# ifdef __APPLE__
  const char* tmp = 0;
# endif
  char *codeset = (char*) calloc(sizeof(char), TEXTLEN);
  const char *loc = NULL;
  char* bdtd = 0;
  char *txd = 0;
  char* cs = NULL;
  int ret = 0;

  if(!locale || !codeset) {
    fl_i18n_printf(("%s:%d Could not allocate enough memory.",
            __FILE__,__LINE__));
    return 1;
  }

  sprintf(codeset, "ISO-8859-1");

# ifdef __APPLE__
  // 1. get the locale info
  CFLocaleRef userLocaleRef = CFLocaleCopyCurrent();
  CFStringRef cfstring = CFLocaleGetIdentifier( userLocaleRef );

  CFShow( cfstring );
  DBG_PROG_V( CFStringGetLength(cfstring) )

    // copy to a C buffer
  CFIndex gr = 36;
  char text[36];
  Boolean fehler = CFStringGetCString( cfstring, text, gr, kCFStringEncodingISOLatin1 );

  if(fehler) {
      DBG_PROG_S( _("osX locale obtained: ") << text )
    snprintf(locale,TEXTLEN, text);
  } else {
      DBG_PROG_S( _("osX locale not obtained") )
  }
  DBG_PROG_V( locale )

  // set the locale info
  if(strlen(locale) && set_codeset == FL_I18N_SETCODESET_SELECT)
  {
     tmp = setlocale (lc, locale);
  }
  if (tmp)
    snprintf(locale,TEXTLEN, tmp);
  //set_codeset = 0;
# else
  DBG_PROG

  // 1. get default locale info ..
    // use the standard way
    // this is dangerous
  char *temp = setlocale (lc, NULL);
  char *previous_locale = temp ? icc_strdup_m(temp) : NULL;
  if(set_codeset != FL_I18N_SETCODESET_NO)
    temp = setlocale (lc, "");
  char *tmp = temp ? icc_strdup_m(temp) : NULL;
  if(tmp) {
    snprintf(locale,TEXTLEN, tmp);
    DBG_PROG_V( locale )
  }

  if(previous_locale) free(previous_locale); previous_locale = NULL;
  if(tmp) free(tmp); tmp = NULL;

    // .. or take locale info from environment
  if(!strlen(locale) && getenv("LANG"))
    snprintf(locale,TEXTLEN, getenv("LANG"));
# endif

  if(set_codeset == FL_I18N_SETCODESET_SELECT &&
     locale && strstr(locale, "UTF-8"))
  {
      // add more LINGUAS here
      // borrowed from http://czyborra.com/charsets/iso8859.html
    ret = fl_set_codeset_( "af", "ISO-8859-1", locale, codeset, set_codeset ); // Afrikaans
    ret = fl_set_codeset_( "ca", "ISO-8859-1", locale, codeset, set_codeset ); // Catalan
    ret = fl_set_codeset_( "da", "ISO-8859-1", locale, codeset, set_codeset ); // Danish
    ret = fl_set_codeset_( "de", "ISO-8859-1", locale, codeset, set_codeset ); // German
    ret = fl_set_codeset_( "en", "ISO-8859-1", locale, codeset, set_codeset ); // English
    ret = fl_set_codeset_( "es", "ISO-8859-1", locale, codeset, set_codeset ); // Spanish
    ret = fl_set_codeset_( "eu", "ISO-8859-1", locale, codeset, set_codeset ); // Basque
    ret = fl_set_codeset_( "fi", "ISO-8859-1", locale, codeset, set_codeset ); // Finnish
    ret = fl_set_codeset_( "fo", "ISO-8859-1", locale, codeset, set_codeset ); // Faroese
    ret = fl_set_codeset_( "fr", "ISO-8859-1", locale, codeset, set_codeset ); // French
    ret = fl_set_codeset_( "ga", "ISO-8859-1", locale, codeset, set_codeset ); // Irish
    ret = fl_set_codeset_( "gd", "ISO-8859-1", locale, codeset, set_codeset ); // Scottish
    ret = fl_set_codeset_( "is", "ISO-8859-1", locale, codeset, set_codeset ); // Icelandic
    ret = fl_set_codeset_( "it", "ISO-8859-1", locale, codeset, set_codeset ); // Italian
    ret = fl_set_codeset_( "nl", "ISO-8859-1", locale, codeset, set_codeset ); // Dutch
    ret = fl_set_codeset_( "no", "ISO-8859-1", locale, codeset, set_codeset ); // Norwegian
    ret = fl_set_codeset_( "pt", "ISO-8859-1", locale, codeset, set_codeset ); // Portuguese
    ret = fl_set_codeset_( "rm", "ISO-8859-1", locale, codeset, set_codeset ); // Rhaeto-Romanic
    ret = fl_set_codeset_( "sq", "ISO-8859-1", locale, codeset, set_codeset ); // Albanian
    ret = fl_set_codeset_( "sv", "ISO-8859-1", locale, codeset, set_codeset ); // Swedish
    ret = fl_set_codeset_( "sw", "ISO-8859-1", locale, codeset, set_codeset ); // Swahili

    ret = fl_set_codeset_( "cs", "ISO-8859-2", locale, codeset, set_codeset ); // Czech
    ret = fl_set_codeset_( "hr", "ISO-8859-2", locale, codeset, set_codeset ); // Croatian
    ret = fl_set_codeset_( "hu", "ISO-8859-2", locale, codeset, set_codeset ); // Hungarian
    ret = fl_set_codeset_( "pl", "ISO-8859-2", locale, codeset, set_codeset ); // Polish
    ret = fl_set_codeset_( "ro", "ISO-8859-2", locale, codeset, set_codeset ); // Romanian
    ret = fl_set_codeset_( "sk", "ISO-8859-2", locale, codeset, set_codeset ); // Slovak
    ret = fl_set_codeset_( "sl", "ISO-8859-2", locale, codeset, set_codeset ); // Slovenian

    ret = fl_set_codeset_( "eo", "ISO-8859-3", locale, codeset, set_codeset ); // Esperanto
    ret = fl_set_codeset_( "mt", "ISO-8859-3", locale, codeset, set_codeset ); // Maltese

    ret = fl_set_codeset_( "et", "ISO-8859-4", locale, codeset, set_codeset ); // Estonian
    ret = fl_set_codeset_( "lv", "ISO-8859-4", locale, codeset, set_codeset ); // Latvian
    ret = fl_set_codeset_( "lt", "ISO-8859-4", locale, codeset, set_codeset ); // Lithuanian
    ret = fl_set_codeset_( "kl", "ISO-8859-4", locale, codeset, set_codeset ); // Greenlandic

    ret = fl_set_codeset_( "be", "ISO-8859-5", locale, codeset, set_codeset ); // Byelorussian
    ret = fl_set_codeset_( "bg", "ISO-8859-5", locale, codeset, set_codeset ); // Bulgarian
    ret = fl_set_codeset_( "mk", "ISO-8859-5", locale, codeset, set_codeset ); // Macedonian
    ret = fl_set_codeset_( "ru", "ISO-8859-5", locale, codeset, set_codeset ); // Russian
    ret = fl_set_codeset_( "sr", "ISO-8859-5", locale, codeset, set_codeset ); // Serbian
    ret = fl_set_codeset_( "uk", "ISO-8859-5", locale, codeset, set_codeset ); // Ukrainian

    ret = fl_set_codeset_( "ar", "ISO-8859-6", locale, codeset, set_codeset ); // Arabic
    ret = fl_set_codeset_( "fa", "ISO-8859-6", locale, codeset, set_codeset ); // Persian
    ret = fl_set_codeset_( "ur", "ISO-8859-6", locale, codeset, set_codeset ); // Pakistani Urdu

    ret = fl_set_codeset_( "el", "ISO-8859-7", locale, codeset, set_codeset ); // Greek

    ret = fl_set_codeset_( "iw", "ISO-8859-8", locale, codeset, set_codeset ); // Hebrew
    ret = fl_set_codeset_( "ji", "ISO-8859-8", locale, codeset, set_codeset ); // Yiddish

    ret = fl_set_codeset_( "tr", "ISO-8859-9", locale, codeset, set_codeset ); // Turkish

    ret = fl_set_codeset_( "th", "ISO-8859-11", locale, codeset, set_codeset ); // Thai

    ret = fl_set_codeset_( "zh", "ISO-8859-15", locale, codeset, set_codeset ); // Chinese

    ret = fl_set_codeset_( "ja", "EUC", locale, codeset, set_codeset ); // Japan ; eucJP, ujis, EUC, PCK, jis7, SJIS

    ret = fl_set_codeset_( "hy", /*"UTF-8"*/"ARMSCII-8", locale, codeset, set_codeset ); // Armenisch
  }

  if(ret)
    return 1;

  if(strlen(locale))
    DBG_PROG_S( locale );


  // 2. for GNU gettext, the locale info is usually stored in the LANG variable
  loc = getenv("LANG");

  if(loc) {

      // good
    DBG_PROG_V( loc )

  } else {

    DBG_PROG_S( _("try to set LANG") );

      // set LANG
#   ifdef __APPLE__
    if (strlen(locale))
      setenv("LANG", locale, 0);
#   endif

      // good?
    if(getenv("LANG"))
      DBG_PROG_S( getenv("LANG") );
  }

  DBG_PROG_V( system("set | grep LANG") )
  if(strlen(locale))
    DBG_PROG_S( locale );

  // 3. where to find the MO file? select an appropriate directory
  bdtd = bindtextdomain (domain, locale_path);
  DBG_PROG_S( _("try locale in ") << bdtd );

  // 4. set our charset
  switch(set_codeset)
  {
  case FL_I18N_SETCODESET_SELECT:
       cs = bind_textdomain_codeset(domain, codeset);
       break;
  case FL_I18N_SETCODESET_UTF8:
       cs = bind_textdomain_codeset(domain, "UTF-8");
       break;
  default: break;
  }

  // 5. our translations
  txd = textdomain (domain);

  if(cs)
  {
    fl_i18n_codeset = cs;
    DBG_PROG_S( _("set codeset for") << domain << " to " << cs );
  }
  if(set_codeset != FL_I18N_SETCODESET_NO)
    setlocale (LC_NUMERIC, "C");

  // gettext initialisation end
  free(codeset);
  free(locale);

  DBG_PROG_ENDE
#endif
  return 0;
}


#include <FL/Fl_Menu_Item.H>
void
fl_translate_menue( Fl_Menu_Item* menueleiste )
{
#ifdef USE_GETTEXT
  DBG_PROG_START
  int size = menueleiste->size();
  for(int i = 0; i < size ; ++i) {
    const char* text = menueleiste[i].label();
    if(text)
    {
      menueleiste[i].label( _(text) );
      /*WARN_S("no text at label item #/n:" << i <<"/"<< size)
      break;*/
    }
  }
  DBG_PROG_ENDE
#endif
}

#include <FL/Fl_File_Chooser.H>
void
fl_translate_file_chooser( )
{
#ifdef USE_GETTEXT
  DBG_PROG_START
    Fl_File_Chooser::add_favorites_label = _("Add to Favorites");
    Fl_File_Chooser::all_files_label = _("All Files (*)");
    Fl_File_Chooser::custom_filter_label = _("Custom Filter");
    Fl_File_Chooser::existing_file_label = _("Please choose an existing file!");
    Fl_File_Chooser::favorites_label = _("Favorites");
    Fl_File_Chooser::filename_label = _("Filename");
    Fl_File_Chooser::manage_favorites_label = _("Manage Favorites");
#   ifdef WIN32
    Fl_File_Chooser::filesystems_label = _("My Computer");
#   else
    Fl_File_Chooser::filesystems_label = _("Filesystems");
#   endif
    Fl_File_Chooser::new_directory_label = _("New Directory?");
    Fl_File_Chooser::preview_label = _("Preview");
#   if (FL_MAJOR_VERSION == 1 && FL_MINOR_VERSION >= 1 && FL_PATCH_VERSION >= 7)
    Fl_File_Chooser::save_label = _("Save"); // since 1.1.7?
#   endif
    Fl_File_Chooser::show_label = _("Show:");
  DBG_PROG_ENDE
#endif
}



#ifndef WIN32
#include <sys/time.h>
#include <unistd.h>
#endif
#include <time.h>
#include <math.h>
const char*
threadGettext( const char* text)
{
  const char *translation = text;
#ifdef USE_GETTEXT
# ifdef HAVE_PTHREAD_H
  static pthread_mutex_t translation_mutex_         = PTHREAD_MUTEX_INITIALIZER;
  static Fl_Thread       translation_mutex_thread_  = (Fl_Thread)pthread_self();
  static int             translation_mutex_threads_ = 0;
  // im selben Zweig gesperrten Rat ausschliesen
  if( translation_mutex_thread_ != pthread_self() ||
      translation_mutex_threads_ == 0 )
    // Warten bis der Rat von einem anderen Zweig freigegeben wird
    while (pthread_mutex_trylock( &translation_mutex_ )) {
      float sekunden = 0.001;
#            if defined(__GNUC__) || defined(__APPLE__)
             timespec ts;
             double ganz;
             double rest = modf(sekunden, &ganz);
             ts.tv_sec = (time_t)ganz;
             ts.tv_nsec = (time_t)(rest * 1000000000);
             //DBG_PROG_V( sekunden<<" "<<ts.tv_sec<<" "<<ganz<<" "<<rest )
             nanosleep(&ts, 0);
#            else
             usleep((time_t)(sekunden/(double)CLOCKS_PER_SEC));
#            endif
    }
  translation_mutex_threads_++ ;
  if(translation_mutex_threads_ == 1)
     translation_mutex_thread_ = pthread_self() ;

  translation = gettext( text );

  --translation_mutex_threads_;
  if(!translation_mutex_threads_)
    pthread_mutex_unlock( &translation_mutex_ );
# else
  translation = gettext( text );
# endif
#endif
  return translation;
}


