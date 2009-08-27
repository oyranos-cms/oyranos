/** @file oyranos.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */


#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <langinfo.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_cmms.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */
#if 1
#define ERR if (rc<=0 && oy_debug) { oyMessageFunc_p(oyMSG_WARN,0,"%s:%d %d\n", __FILE__,__LINE__,rc); perror("Error"); }
#else
#define ERR
#endif

#define WARNc_PROFILE_S(text_,fileName_) \
      { \
        int l_1 = oyStrlen_(text_); \
        int l_2 = oyStrlen_(oyNoEmptyName_m_(fileName_)); \
        char * tmp = oyAllocateFunc_ (l_1 + l_2 + 12); \
        memcpy(tmp, text_, l_1); \
        memcpy(&tmp[l_1], " ", 1); \
        memcpy(&tmp[l_1+1], oyNoEmptyName_m_(fileName_), l_2); \
        tmp[l_1+1+l_2] = 0; \
 \
        WARNc_S( tmp ); \
        oyDeAllocateFunc_(tmp); \
      }

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* --- function definitions --- */


/** @func    oyMessageFunc_
 *  @brief
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 *  @date    2009/07/20
 */
int oyMessageFunc_( int code, const oyStruct_s * context, const char * format, ... )
{
  char * text = 0;
  va_list list;
  int i,len;
  const char * type_name = "";
  int id = -1;
  size_t sz = 256;
  pid_t pid = 0;
  FILE * fp = 0;

  if(code == oyMSG_DBG && !oy_debug)
    return 0;

  if(context && oyOBJECT_NONE < context->type_) 
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
  }

  text = malloc( sz );
  if(!text)
  {
    fprintf(stderr,
     "oyranos.c:80 oyMessageFunc_() Could not allocate 4096 byte of memory.\n");
    return 1;
  }

  text[0] = 0;

# define MAX_LEVEL 20
  if(level_PROG < 0)
    level_PROG = 0;
  if(level_PROG > MAX_LEVEL)
    level_PROG = MAX_LEVEL;
  for (i = 0; i < level_PROG; i++)
    oySprintf_( &text[oyStrlen_(text)], " ");

  fprintf(stderr,text);

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  if (len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, _("WARNING"));
         break;
    case oyMSG_ERROR:
         fprintf( stderr, _("!!! ERROR"));
         break;
  }

  fprintf( stderr, " %03f: ", DBG_UHR_);
  fprintf( stderr, "%s[%d] ", type_name, id );

  i = 0;
  while(text[i])
    fputc(text[i++], stderr);
  fprintf( stderr, "\n" );

  if(oy_backtrace)
  {
#   define TMP_FILE "/tmp/oyranos_gdb_temp." OYRANOS_VERSION_NAME "txt"
    pid = (int)getpid();
    fp = fopen( TMP_FILE, "w" );

    if(fp)
    {
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "thread 1\nbacktrace\n"/*thread 2\nbacktrace\nthread 3\nbacktrace\n*/"detach" );
      fclose(fp);
      fprintf( stderr, "GDB output:\n" );
      system("gdb -batch -x " TMP_FILE);
    } else
      fprintf( stderr, "could not open " TMP_FILE "\n" );
  }

  free( text );
  return 0;
}


oyMessage_f     oyMessageFunc_p = oyMessageFunc_;

/** @func    oyMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/04/03
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 */
int            oyMessageFuncSet      ( oyMessage_f         message_func )
{
  if(message_func)
    oyMessageFunc_p = message_func;
  return 0;
}




int
oyGetPathFromProfileNameCb_          ( oyFileList_s      * data,
                                       const char        * full_name,
                                       const char        * filename )
{
  int success = 0;
  oyFileList_s * l = data;
  char* search = l->names[0];

  DBG_MEM_S( search )
  if(strcmp(filename,search)==0) {
    size_t size = 128;
    char* header = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
    success = !oyCheckProfileMem_ (header, size, 0);
    oyFree_m_ (header);
    if (success) {
      DBG_MEM_S(full_name)
      DBG_MEM_V(oyStrlen_(full_name))
      if (oyStrlen_(full_name) < MAX_PATH) {
        oySprintf_(search,full_name);
        search[oyStrlen_(full_name)] = '\000';
      } else
        search[0] = '\000';
    } else
      WARNc_PROFILE_S( _("not a profile:"), oyNoEmptyName_m_(full_name) )
  }
  /* break on success */
  DBG_MEM_V(success)
  return success;
}

char*
oyGetPathFromProfileName_       (const char*   fileName,
                                 oyAlloc_f     allocate_func)
{
  char  *fullFileName = 0;
  char  *pathName = 0;
  int    success = 0;
  char  *header = 0;
  size_t    size;

  DBG_PROG_START

  /*DBG_NUM_S((fileName)) */
  /* test for pure file without dir; search in configured paths */
  if (fileName && !strchr(fileName, OY_SLASH_C))
  {
    char search[MAX_PATH];
    int count = 0;
    size_t len = (oyStrlen_(fileName) < MAX_PATH) ? 
                          oyStrlen_(fileName) : MAX_PATH;
    char ** path_names = oyProfilePathsGet_( &count, oyAllocateFunc_ );
    char * l_names[2] = { 0, 0 };
    oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 0, NULL, 0, 0, 0};

    l_names[0] = search;
    l.names = l_names;

    DBG_PROG

    if(oyStrlen_(fileName) < MAX_PATH)
    {
      memcpy(search, fileName, len); search[len] = '\000';
    } else {
      WARNc2_S( "%s %d", _("name longer than"), MAX_PATH)
      DBG_PROG_ENDE
      return 0;
    }
    success = oyRecursivePaths_( oyGetPathFromProfileNameCb_, &l,
                                 (const char**)path_names, count );

    oyStringListRelease_( &path_names, count, oyDeAllocateFunc_ );

      if (success) { /* found */
        len = 0;
        DBG_PROG_S((search))
        if(search[0] != 0) len = oyStrlen_(search);
        if(len) {
          char *ptr = 0;
          oyAllocHelper_m_( pathName, char, len+1, allocate_func, return 0 );
          oyStrcpy_(pathName, search);
          ptr = oyStrrchr_(pathName , OY_SLASH_C);
          if(ptr)
            ptr[0] = '\000';
        }
        DBG_PROG_S( pathName )
        DBG_PROG_ENDE
        return pathName;
      } else

    if (!success) {
      if(oy_warn_)
        WARNc_PROFILE_S(_("profile not found in colour path:"), fileName);
      DBG_PROG_ENDE
      return 0;
    }


  } else
  {/* else use fileName as an full qualified name, check name and test profile*/
    DBG_PROG_S("dir/filename found")
    fullFileName = oyMakeFullFileDirName_ (fileName);

    if (oyIsFileFull_(fullFileName))
    {
      size = 128;
      header = oyReadFileToMem_ (fullFileName, &size, allocate_func);

      if (size >= 128)
        success = !oyCheckProfileMem_ (header, 128, 0);
    }

    if (!success) {
      WARNc_PROFILE_S( _("profile not found:"), oyNoEmptyName_m_(fileName))
      DBG_PROG_ENDE
      return 0;
    }

    pathName = oyExtractPathFromFileName_(fullFileName);

    oyFree_m_ (header);
  }

  if (!success)
  { oyFree_m_ (pathName);
    pathName = 0;
  }

  oyFree_m_ (fullFileName);

  DBG_PROG_ENDE
  return pathName;
}


/* public API implementation */

/* path names API */

/* default profiles API */
int
oySetDefaultProfile_       (oyPROFILE_e       type,
                            const char*       file_name)
{
  int r = 0;

  DBG_PROG_START

  if( type == oyASSUMED_WEB &&
      !strstr( file_name,"sRGB" ) )
  {
    WARNc_S(_("wrong profile for static web colour space selected, need sRGB"))
    return 1;
  }
  r = oySetProfile_ (file_name, type, 0);
  DBG_PROG_ENDE
  return r;
}

int
oySetDefaultProfileBlock_  (oyPROFILE_e type,
                            const char* file_name, void* mem, size_t size)
{
  int r;

  DBG_PROG_START

  r = oySetProfile_Block (file_name, mem, size, type, 0);
  DBG_PROG_ENDE
  return r;
}

#ifdef __APPLE__
#include <Carbon/Carbon.h>

typedef struct {
  char   *data;
  SInt32  size;
} refcon;

OSErr
oyFlattenProfileProc (
   SInt32 command, 
   SInt32 *size, 
   void *data, 
   void *refCon)
{
  /* Alle Bestandteile einsammeln */
  if(*size)
  {
    refcon *ref = (refcon*) refCon;

    char* block = NULL;

    oyAllocHelper_m_( block,char, ref->size + *size, oyAllocateFunc_, return 1);
    /* old data */
    if(ref->data && ref->size) {
      memcpy(block, ref->data, ref->size);
      oyDeAllocateFunc_(ref->data);
    }
    /* new data */
    memcpy( &block[ref->size], data, *size );

    ref->data = block;
    ref->size += *size;
  }
  DBG_PROG2_S("command:%d size:%d", (int)command, (int)*size)

  return 0;
}

OSErr
oyFlattenProfileProcSize (
   SInt32 command, 
   SInt32 *size, 
   void *data, 
   void *refCon)
{
  /* Alle Bestandteile einsammeln */
  if(*size)
  {
    refcon *ref = (refcon*) refCon;
    ref->size += *size;
  }
  DBG_PROG2_S("command:%d size:%d", (int)command, (int)*size)

  return 0;
}


int
oyGetProfileBlockOSX (CMProfileRef prof, char **block, size_t *size, oyAlloc_f allocate_func)
{
    CMProfileLocation loc;
    Boolean bol;
    refcon ref = {0,0};
    Str255 str;
    ScriptCode code;
    CMError err = 0;
    const unsigned char *profil_name;

  DBG_PROG_START

    CMGetProfileLocation(prof, &loc);
    switch(loc.locType)
    {
      case cmNoProfileBase:
             DBG_PROG_S("The profile is a temporary profile.")
             break;
      case cmFileBasedProfile:
             DBG_PROG_S("The profile is a file based profile.")
             break;
      case cmHandleBasedProfile:
             DBG_PROG_S("The profile is a profile handle.")
             break;
      case cmPtrBasedProfile:
             DBG_PROG_S("The profile is a pinter based profile.")
             break;
      case cmProcedureBasedProfile:
             DBG_PROG_S("The profile is a prozedural profile.")
             break;
      case cmPathBasedProfile:
             DBG_PROG_S("The profile is a path profile.")
             break;
      case cmBufferBasedProfile:
             DBG_PROG_S("The profile is a memory block Profile.")
             break;
      default:
             DBG_PROG_S("no profile found?")
             break;
    }

#if 0
    /* only the size */
    if(*size == 0) {
      err = CMFlattenProfile ( prof, 0, oyFlattenProfileProcSize, &ref, &bol);
      *size = ref.size;
      return err;
    }
#endif
    err = CMFlattenProfile ( prof, 0, oyFlattenProfileProc, &ref, &bol);

    err = 0;
    CMGetScriptProfileDescription(prof, str, &code);
      DBG_PROG_V(( (int)str[0] ))

    profil_name = str; ++profil_name;
    if(ref.size && ref.data)
    {
        *size = ref.size;
        oyAllocHelper_m_( *block, char, ref.size, allocate_func, return 1);
        memcpy(*block, ref.data, ref.size);
          DBG_PROG_V( size )
    }
  DBG_PROG_ENDE
  return 0;
}

char*
oyGetProfileNameOSX (CMProfileRef prof, oyAlloc_f     allocate_func)
{
  char * name = NULL;
  CMProfileLocation loc;

  CMGetProfileLocation(prof, &loc);

      switch(loc.locType)
      {
        case cmNoProfileBase:
               DBG_PROG_S("The profile is a temporary profile.")
               break;
        case cmFileBasedProfile:
               DBG_PROG_S("The profile is a file based profile.")
               {
                 CMFileLocation file_loc = loc.u.fileLoc;
                 FSSpec spec = file_loc.spec;
                 unsigned char *name_ = NULL;
                 FSRef ref;
                 CMError err = 0;

                 oyAllocHelper_m_( name_, unsigned char, 1024, 
                                   allocate_func, return 0 );
                 err = FSpMakeFSRef( &spec, &ref );
                 err = FSRefMakePath( &ref, name_, 1024 );
                 DBG_PROG1_S("file is at: %s\n", name_ );
                 if(err == noErr)
                   name = (char*) name_;
               }
               break;
        case cmHandleBasedProfile:
               DBG_PROG_S("The profile is a profile handle.")
               break;
        case cmPtrBasedProfile:
               DBG_PROG_S("The profile is a pinter based profile.")
               break;
        case cmProcedureBasedProfile:
               DBG_PROG_S("The profile is a prozedural profile.")
               break;
        case cmPathBasedProfile:
               DBG_PROG_S("The profile is a path profile.")
               break;
        case cmBufferBasedProfile:
               DBG_PROG_S("The profile is a memory block Profile.")
               break;
        default:
               DBG_PROG_S("no profile found?")
               break;
      }

  return name;
}
#endif

/*int*
oyGroupSetGet            (oyGROUP_e group, int * count )
{
  return 0;
}*/

char*
oyGetDefaultProfileName_   (oyPROFILE_e       type,
                            oyAlloc_f         allocate_func)
{
  char* name = 0;
  
  DBG_PROG_START

  DBG_PROG_V( type )

  /* a static_profile */
  if(type == oyASSUMED_WEB) {
    oyAllocHelper_m_( name, char, MAX_PATH, allocate_func, return NULL );
    oySprintf_(name, OY_WEB_RGB);
    DBG_PROG_S( name )
    return name;
  }

#ifdef __APPLE__
  {
    OSType dataColorSpace = 0;
    CMError err;
    CMProfileRef prof=NULL;
    UInt32 version = 0;
    const oyOption_t_ * t = 0;

    err = CMGetColorSyncVersion(&version);
    if(err == noErr)
      DBG_PROG1_S( "ColorSync version: %d\n", (int)version );

    switch(type)
    {
      case oyEDITING_RGB:            /**< Rgb Editing (Workspace) Profile */
                dataColorSpace = cmRGBData; break;
      case oyEDITING_CMYK:           /**< Cmyk Editing (Workspace) Profile */
                dataColorSpace = cmCMYKData; break;
      case oyEDITING_XYZ:            /**< XYZ Editing (Workspace) Profile */
                dataColorSpace = cmXYZData; break;
      case oyEDITING_LAB:            /**< Lab Editing (Workspace) Profile */
                dataColorSpace = cmLabData; break;
      case oyEDITING_GRAY:           /**< Gray Editing (Workspace) Profile */
                dataColorSpace = cmGrayData; break;

      case oyASSUMED_RGB:            /**< standard RGB assumed source profile */
      case oyASSUMED_WEB:            /**< std internet assumed source static_profile*/
      case oyASSUMED_CMYK:           /**< standard Cmyk assumed source profile*/
      case oyASSUMED_XYZ:            /**< standard XYZ assumed source profile */
      case oyASSUMED_LAB:            /**< standard Lab assumed source profile */
      case oyASSUMED_GRAY:           /**< standard Gray assumed source profile*/
      case oyPROFILE_PROOF:          /**< standard proofing profile */
                t = oyOptionGet_((oyWIDGET_e)type);
                if(t && t->config_string)
                  name = oyGetKeyString_( t->config_string, allocate_func );
                break;
      case oyDEFAULT_PROFILE_START:
      case oyDEFAULT_PROFILE_END:
                break;
    }

    if(dataColorSpace)
      err = CMGetDefaultProfileBySpace ( dataColorSpace, &prof);
    if(!name && !err && prof)
    {
      CMProfileLocation loc;
      err = CMGetProfileLocation( prof, &loc );
      
      switch(loc.locType)
      {
        case cmNoProfileBase:
               DBG_PROG_S("The profile is a temporary profile.")
               break;
        case cmFileBasedProfile:
               DBG_PROG_S("The profile is a file based profile.")
               {
                 CMFileLocation file_loc = loc.u.fileLoc;
                 FSSpec spec = file_loc.spec;
                 unsigned char *name_ = NULL;
                 FSRef ref;

                 oyAllocHelper_m_( name_, unsigned char, MAX_PATH, allocate_func,
                                   return NULL );
                 err = FSpMakeFSRef( &spec, &ref );
                 err = FSRefMakePath( &ref, name_, 1024 );
                 DBG_PROG1_S( "file is at: %s\n", name_ );
                 if(err == noErr)
                   name = (char*) name_;
               }
               break;
        case cmHandleBasedProfile:
               DBG_PROG_S(("The profile is a profile handle."))
               break;
        case cmPtrBasedProfile:
               DBG_PROG_S(("The profile is a pinter based profile."))
               break;
        case cmProcedureBasedProfile:
               DBG_PROG_S(("The profile is a prozedural profile."))
               break;
        case cmPathBasedProfile:
               DBG_PROG_S("The profile is a path profile.")
               {
                 CMPathLocation path_loc = loc.u.pathLoc;
                 char* path = path_loc.path;
                 char *name_ = NULL;

                 oyAllocHelper_m_( name_, char, MAX_PATH, allocate_func,
                                   return NULL );
                 snprintf( name_, 256, "%s", path );
                 DBG_PROG1_S( "file is to: %s\n", name_ );
                 name = name_;
               }
               break;
        case cmBufferBasedProfile:
               DBG_PROG_S("The profile is a memory block Profile.")
               break;
        default:
               DBG_PROG_S("no profile found?")
               break;
      }
    } else if(!name)
      DBG_PROG2_S( "could not find:%d Profile:%s\n", (int)err, prof ? "no" : "yes" );

    err = CMCloseProfile( prof );
  }
#else
  {
    const oyOption_t_ * t = oyOptionGet_((oyWIDGET_e)type);
    if( !t || !t->config_string )
    {
      WARNc2_S( "%s %d", _("Option not supported type:"), type)
      return NULL;
    } else
      name = oyGetKeyString_( t->config_string, allocate_func );
  }
#endif

  if(name && name[0])
  {
    DBG_PROG_S(name);
    /* cut off the path part of a file name */
    if (oyStrrchr_ (name, OY_SLASH_C))
    {
      char * f = NULL;

      oyAllocHelper_m_( f, char, oyStrlen_(name) + 1, oyAllocateFunc_, return 0);
      oySprintf_( f, "%s", name );
      oySprintf_( name, oyStrrchr_ (f, OY_SLASH_C) + 1 );
      oyFree_m_(f);
    }
  } else {
    const oyOption_t_ * t = oyOptionGet_((oyWIDGET_e)type);
    if(t && t->default_string)
    {
      oyAllocHelper_m_( name, char, oyStrlen_( t->default_string ) + 1,
                        allocate_func, return NULL );
      oySprintf_( name, "%s", t->default_string );
    } else {
      WARNc2_S( "%s %d", _("Option not supported type:"), type)
    }
  }

  DBG_PROG_ENDE
  return name;
}




/* profile lists API */

#define MAX_DEPTH 64
struct OyProfileList_s_ {
  int hopp;
  const char* coloursig;
  int mem_count;
  int count_files;
  char** names;
};


/* profile handling API */

int
oySetProfile_Block (const char* name, void* mem, size_t size,
                    oyPROFILE_e type, const char* comnt)
{
  int r = 0, len = 0;
  char *fullFileName = NULL, *resolvedFN = NULL;
  const char *fileName = NULL;

  DBG_PROG_START

  if (oyStrrchr_ (name, OY_SLASH_C))
    fileName = oyStrrchr_ (name, OY_SLASH_C);
  else
    fileName = name;

  oyAllocHelper_m_( fullFileName, char,
                    oyStrlen_(OY_PROFILE_PATH_USER_DEFAULT) +
                      oyStrlen_ (fileName) + 4,
                    oyAllocateFunc_, return 1);

  oySprintf_ (fullFileName, "%s%s", OY_PROFILE_PATH_USER_DEFAULT, OY_SLASH);
  len = oyStrlen_(fullFileName);
  memcpy( &fullFileName[len], fileName, oyStrlen_(fileName) );
  fullFileName[len + oyStrlen_(fileName)] = 0;

  resolvedFN = oyResolveDirFileName_ (fullFileName);
  oyFree_m_(fullFileName);
  fullFileName = resolvedFN;

  if (!oyCheckProfileMem_( mem, size, 0))
  {
    DBG_PROG_S(fullFileName)
    if ( oyIsFile_(fullFileName) ) {
      WARNc_PROFILE_S( fullFileName,
                _("file exists, please remove befor installing new profile."))
    } else
    { r = oyWriteMemToFile_ (fullFileName, mem, size);
      oySetProfile_ ( name, type, comnt);
    }
  }

  DBG_PROG1_S("%s", name)
  DBG_PROG1_S("%s", fileName)
  DBG_PROG2_S("%ld %d", (long int)&((char*)mem)[0] , (int)size)
  oyFree_m_(fullFileName);

  DBG_PROG_ENDE
  return r;
}

/* small search engine */

oyComp_t_*
oyInitComp_ (oyComp_t_ *list, oyComp_t_ *top)
{ DBG_PROG_START
  if (!list)
    oyAllocHelper_m_( list, oyComp_t_, 1, oyAllocateFunc_, );

  list->next = 0;

  list->type_ = oyOBJECT_COMP_S_;
  if (top)
    list->begin = top;
  else
    list->begin = list;
  list->name = 0;
  list->val = 0;
  list->hits = 0;
  DBG_PROG_ENDE
  return list;
}

oyComp_t_*
oyAppendComp_ (oyComp_t_ *list, oyComp_t_ *new)
{ DBG_PROG_START

  /* no list yet => first and only entry */
  if (!list) {
    list = oyInitComp_(list,0);
    DBG_PROG_ENDE
    return list;
  }

  list = list->begin;
  while (list->next)
    list = list->next;

  /* no new => add */
  if (!new)
    new = oyInitComp_(new, list->begin);

  list->next = new;

  DBG_PROG_ENDE
  return new;
}

void
oySetComp_         (oyComp_t_ *compare, const char* keyName,
                    const char* value, int hits )
{
  DBG_PROG_START
  oyAllocHelper_m_( compare->name, char, oyStrlen_(keyName)+1, oyAllocateFunc_, );
  memcpy (compare->name, keyName, oyStrlen_(keyName)+1); 
  oyAllocHelper_m_( compare->val, char, oyStrlen_(value)+1, oyAllocateFunc_, );
  memcpy (compare->val, value, oyStrlen_(value)+1); 
  compare->hits = hits;
  DBG_PROG_ENDE
}

void
oyDestroyCompList_ (oyComp_t_ *list)
{
  oyComp_t_ *before;

  DBG_PROG_START

  list = list->begin;
  while (list->next)
  {
    before = list;
    list = list->next;
    oyFree_m_(before)
  }

  if(list->name)
    oyDeAllocateFunc_(list->name);
  if(list->val)
    oyDeAllocateFunc_(list->val);
  oyFree_m_(list);

  DBG_PROG_ENDE
}

char*
printComp (oyComp_t_* entry)
{
  static char text[MAX_PATH] = {0};

  DBG_PROG_START


# ifdef DEBUG
  DBG_PROG1_S("%d", (int)(intptr_t)entry)
  oySprintf_( text, "%s:%d %s() begin %d next %d\n",
           __FILE__,__LINE__,__func__,
           (int)(intptr_t)entry->begin, (int)(intptr_t)entry->next );

  if(entry->name)
    oySprintf_( &text[oyStrlen_(text)], " name %s %d", entry->name,
                                  (int)(intptr_t)entry->name);
  if(entry->val)
    oySprintf_( &text[oyStrlen_(text)], " val %s %d", entry->val,
                                  (int)(intptr_t)entry->val);
  oySprintf_( &text[oyStrlen_(text)], " hits %d\n", entry->hits);

  DBG_PROG_ENDE
  return text;
# else
  DBG_PROG_ENDE
  return 0;
# endif
}



int
oySetDeviceProfile_                (const char* manufacturer,
                                    const char* model,
                                    const char* product_id,
                                    const char* host,
                                    const char* port,
                                    const char* attrib1,
                                    const char* attrib2,
                                    const char* attrib3,
                                    const char* profileName,
                                    const void* mem,
                                    size_t size)
{
  int rc = 0;
  char* comment = 0;

  DBG_PROG_START

  if (mem && size && profileName)
  {
    rc = oyCheckProfileMem_ (mem, size, 0); ERR
  }

  if (!rc)
  { DBG_PROG
    oyEraseDeviceProfile_          ( manufacturer, model, product_id,
                                     host, port,
                                     attrib1, attrib2, attrib3);

    if (manufacturer || model || product_id || host || port || attrib1
        || attrib2 || attrib3)
    { int len = 0;
      DBG_PROG
      if (manufacturer) len += oyStrlen_(manufacturer);
      if (model) len += oyStrlen_(model);
      if (product_id) len += oyStrlen_(product_id);
      if (host) len += oyStrlen_(host);
      if (port) len += oyStrlen_(port);
      if (attrib1) len += oyStrlen_(attrib1);
      if (attrib2) len += oyStrlen_(attrib2);
      if (attrib3) len += oyStrlen_(attrib3);
      oyAllocHelper_m_( comment, char, len+10, oyAllocateFunc_, );
      if (manufacturer) oySprintf_ (comment, "%s", manufacturer); DBG_PROG
      if (model) oySprintf_ (&comment[oyStrlen_(comment)], "%s", model); DBG_PROG
      if (product_id) oySprintf_ (&comment[oyStrlen_(comment)], "%s", product_id);
      if (host) oySprintf_ (&comment[oyStrlen_(comment)], "%s", host);
      if (port) oySprintf_ (&comment[oyStrlen_(comment)], "%s", port);
      if (attrib1) oySprintf_ (&comment[oyStrlen_(comment)], "%s", attrib1);
      if (attrib2) oySprintf_ (&comment[oyStrlen_(comment)], "%s", attrib2);
      if (attrib3) oySprintf_ (&comment[oyStrlen_(comment)], "%s", attrib3);
    } DBG_PROG

    rc =  oySetProfile_ (profileName, oyDEVICE_PROFILE, comment); ERR
  }

  DBG_PROG_ENDE
  return rc;
}



/* --- internal API decoupling --- */

#include "oyranos.h"

/** \addtogroup options Options API
 *  The idea behind this API is to provide one layout for
 *  presenting a configuration dialog to users. The advantage is, every 
 *  application, like KDE and Gnome control panels, will inherit the same logic.
 *  A user can easily use the one and the other panel
 *  without too much relearning.

 *  To illustrate a bit:
 *  In analogy to the WWW, I suggest to implement something like a minimalistic
 *  HTML display.
 *  Once the HTML alike content logic is done in Oyranos, the HTML page works
 *  and appears everywhere compareable. Smaller changes to the Oyranos HTML 
 *  alike pages will appear everywhere in all HTML page display programms.
 *  To remain in the WWW analogy, CSS would be optional to adapt visually.

 *  Functions are provided to set and query for Options layout and
 *  UI strings in Oyranos.

 *  @{
 */


/** @brief all widgets of a group
 *
 *  @param       group           specify which group dialog to build
 *  @param[out]  count           number of widgets contained in list
 *
 *  @return                      list of widgets to create in correct order
 */
oyWIDGET_e    * oyWidgetListGet          (oyGROUP_e           group,
                                        int             * count,
                                        oyAlloc_f         allocate_func )
{
  oyWIDGET_e *list = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  list = oyWidgetListGet_                  ( group, count, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return list;
}

/** @brief Get a special Options UI strings.
 *
 *  @param       option          merges oyBEHAVIOUR_e and oyPROFILE_e
 *  @param[out]  categories      enums list, to place into the right group widget
 *                              { n, first category, second c., ... , n'th c. } 
 *                              for widget of type oyGROUP_e this is empty
 *  @param[out]  name            transated widget title
 *  @param[out]  tooltip         transated tooltip
 *
 *  @return                      widget type, gives a hint to further properties,
 *                              { like choices or int/float value ranges ... }
 */
oyWIDGET_TYPE_e oyWidgetTitleGet         (oyWIDGET_e          option,
                                        const oyGROUP_e  ** categories,
                                        const char     ** name,
                                        const char     ** tooltip,
                                        int             * flags )
{
  oyWIDGET_TYPE_e type = oyWIDGETTYPE_START;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  type = oyWidgetTitleGet_                 ( option,
                                             categories,
                                             name, tooltip,
                                             flags );
  oyExportEnd_();
  DBG_PROG_ENDE
  return type;
}

/** @brief Get a Options choices UI strings.
 *
 *  This function is checked for double occuring profiles. Such are sorted out.
 *  @todo In the future a second choices_string_list may appear for displaying.
 *
 *  @param       option          merge oyBEHAVIOUR_e and oyPROFILE_e
 *  @param[out]  choices         n choices; if choices is zero then you need to
 *                              optain the choices otherwise, like for profiles
 *  @param[out]  choices_strings translated list of n choices
 *  @param[out]  current         the actual setting
 *
 *  @return                      success
 */
int         oyOptionChoicesGet         (oyWIDGET_e          option,
                                        int             * choices,
                                        const char    *** choices_string_list,
                                        int             * current )
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  error =   oyOptionChoicesGet_            ( option,
                                             choices, choices_string_list,
                                             current );
  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @brief delete list of choices from a option
 *  @param list     string list
 *  @param size     number of strings in the list to free
 */
void
oyOptionChoicesFree                  (oyWIDGET_e        option,
                                      char          *** list,
                                      int               size)
{ DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  oyOptionChoicesFree_ (option, list, size);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/*  @} */


/** \addtogroup behaviour Behaviour API
 *  Functions to set and query for behaviour on various actions in Oyranos.

 *  @todo is the missing description of available choices<br>

 *  Take a look at the Oyranos settings xml file. Options are described <a href="../../../color/settings/office.policy.xml">there</a>.
 
 *  Otherwise look at the \b oyranos-config-fltk application
    or the description on

 *  <a href="http://www.oyranos.com/wiki/index.php?title=Oyranos_Use_Cases">ColourWiki</a>. 

 *  @{
 */

/** Set a special behaviour. Usual in control panel in Oyranos.\n 
 *
 *  @param  type      the type of behaviour
 *  @param  choice    the selected option
 *  @return error
 */
int
oySetBehaviour         (oyBEHAVIOUR_e       type,
                        int               choice)
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  error = oySetBehaviour_(type, choice);

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Get a special behaviour.\n 

 *  @todo The options should silently fallback to defaults.

 *
 *  @param  type      the type of behaviour
 *  @return           the behaviour option (-1 if not available or not set)
 */
int
oyGetBehaviour         (oyBEHAVIOUR_e       type)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  n = oyGetBehaviour_(type);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/*  @} */

/** \addtogroup policy Policy API
 *  Functions to set and export policies in Oyranos.
 *
 *  @todo define some default policies internally
 *
 *  @{
 */

/** @brief save a options group to a file
 *
 *  The function is basically a wrapper for oyPolicyToXML() and will write
 *  the resulting XML to a file somewhere in XDG_CONFIG_HOME.
 *
 *  @param         group               use oyGROUP_ALL for a typical snapshot
 *  @param         name                the name will become part of a filename
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 *  @date    2008/07/23
 */
int         oyPolicySaveActual        ( oyGROUP_e         group,
                                        const char      * name )
{
  int error = !name;
  char * text = 0;
  char * filename = 0;
  const char * xdg_home_dir = getenv("XDG_CONFIG_HOME");
  char * path = 0, * ptr;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  if(!error)
  {
    text = oyPolicyToXML_(group, 1, oyAllocateFunc_);
    error = !text;
  }

  if(!error)
  {
    if(xdg_home_dir)
    {
      path = oyStringCopy_( xdg_home_dir, oyAllocateFunc_ );
      ptr = oyStrchr_( path, ':' );
      if(ptr)
        *ptr = '\000';
      oyStringAdd_( &filename, path, oyAllocateFunc_, oyDeAllocateFunc_ );
    } else
      oyStringAdd_( &filename, "~/.config", oyAllocateFunc_, oyDeAllocateFunc_ );

    oyStringAdd_( &filename, "/color/settings/", oyAllocateFunc_, oyDeAllocateFunc_ );
    oyStringAdd_( &filename, name, oyAllocateFunc_, oyDeAllocateFunc_ );
    oyStringAdd_( &filename, ".xml", oyAllocateFunc_, oyDeAllocateFunc_ );
    if(oyIsFile_(filename))
      WARNc2_S("%s %s",_("will overwrite policy file"), filename);
    error = oyWriteMemToFile_( filename, text, oyStrlen_(text)+1 );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Save a group of policy settings.\n
 *  Write only such variables, which are available and ignore unknown ones.
 *  This currently produces pseudo xml configuration files.
 *
 *  @param  group     the policy group
 *  @param  add_header     add description
 *  @param allocate_func user provided function for allocating the strings memory
 *  @return           the configuration as XML to save to file
 */
char*
oyPolicyToXML          (oyGROUP_e           group,
                        int               add_header,
                        oyAlloc_f         allocate_func)
{
  char* text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  text = oyPolicyToXML_(group, add_header, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return text;
}

/** Load a group of policy settings.\n
 *  use xml-ish input produced by oyPolicyToXML()
 *
 *  @param  group     the policy group
 *  @param  xml       xml configuration string
 *  @return           errors
 */
int
oyReadXMLPolicy        (oyGROUP_e           group,
                        const char       *xml)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  n = oyReadXMLPolicy_(group, xml);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Load a group of policy settings.\n
 *  use xml-ish file input produced by oyPolicyToXML()
 *
 *  @param  policy_file  the policy file, will be locked up in standard paths
 *  @param  full_name    file name including path
 *  @return              errors
 */
int
oyPolicySet                (const char      * policy_file,
                            const char      * full_name )
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_SETTING);

  n = oyPolicySet_(policy_file, full_name);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/*  @} */

/**
 *  @internal
 *  \addtogroup path_names Path Names API
 *  Functions to handle path configuration for Oyranos.
 *
 *  Paths include operating system standard paths. For linux these are:
 *  <ul><li>  /usr/share/color/icc - for global system profiles</li>
 *  <li>    ~/.color/icc - for user profiles</li>
 *  <li>    /usr/local/share/color/icc - for local system profiles</li>
 *  </ul>
 *  Paths are scanned recursively excluding symbolic links.

 *  @see @ref profile_lists

 *  @{
 */

#if 0
/** Determin the count of configured search paths.\n */
int
oyPathsCount         (void)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oyPathsCount_();
  /*if(!n)
    oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT);
  n = oyPathsCount_();*/

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Get the path name at a certain position in the Oyranos search path list.
 *
 *  @param number        number in list - dont rely on through sessions
 *  @param allocate_func user provided function for allocating the string memory
 *  @return              the pathname at position \e number
 */
char*
oyPathName           (int           number,
                      oyAlloc_f     allocate_func)
{
  char* name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  name = oyPathName_ (number, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return name;
}

/** Add a path name statically to allready configured ones.
 *
 *  @param  pathname new pathname to use by Oyranos for profile search
 *  @return success
 */
int
oyPathAdd            (const char* pathname)
{ int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oyPathAdd_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Remove a path name statically from the list of configured ones.
 *
 *  @param  pathname the pathname to remove from the Oyranos search path list
 */
void
oyPathRemove         (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathRemove_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}

void
oyPathSleep          (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathSleep_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}

void
oyPathActivate       (const char* pathname)
{ DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  oyPathActivate_ (pathname);

  oyExportEnd_();
  DBG_PROG_ENDE
}
#endif

/*  @} */

/** \addtogroup default_profiles Default Profiles API
 *  Functions to handle and configure default profiles.

 *  @see @ref path_names

 *  @{
 */

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *                    specified profile type
 *  @return success
 */
int
oySetDefaultProfile        (oyPROFILE_e       type,
                            const char*       file_name)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oySetDefaultProfile_ (type, file_name);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Sets a profile, which is available in the current configured path.
 *
 *  @param  type      the kind of default profile
 *  @param  file_name the profile which shall become the default for the above
 *          specified profile type
 *  @param  mem       the memory containing the profile
 *  @param  size      the profile size in mem
 *  @return success
 */
int
oySetDefaultProfileBlock   (oyPROFILE_e       type,
                            const char*       file_name,
                            void*             mem,
                            size_t            size)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  n = oySetDefaultProfileBlock_ (type, file_name, mem, size);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** Gets a default profile filename.
 *
 *  @param  type          the kind of default profile
 *  @param  allocate_func the allocation function needed to reserve memory for
                          the filename string
 *  @return filename
 */
char*
oyGetDefaultProfileName    (oyPROFILE_e       type,
                            oyAlloc_f         allocate_func)
{
  char* name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  name = oyGetDefaultProfileName_ (type, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return name;
}
/*  @} */


/** \addtogroup profile_lists Profile Lists API
 *  Functions to handle profile name lists.

 *  @see @ref path_names

 *  @{
 */

/** @brief get a list of profile filenames
 *  @param coloursig filter as ICC 4 byte string
 *  @param[out] size profile filenames count
 *  @return the profiles filename list allocated within Oyranos
 *
 *  @see @ref path_names
 */
char **  oyProfileListGet            ( const char        * coloursig,
                                       uint32_t          * size,
                                       oyAlloc_f           allocateFunc )
{
  char **names = NULL,
       ** tmp = 0;
  uint32_t n = 0;
  int tmp_n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  tmp = oyProfileListGet_(coloursig, &n);
  names = oyStringListAppend_( (const char**)tmp, n, 0,0, &tmp_n,
                               allocateFunc );

  oyStringListRelease_( &tmp, n, oyDeAllocateFunc_ );

  *size = tmp_n;

  oyExportEnd_();
  DBG_PROG_ENDE
  return names;
}

/** @} */


/** \addtogroup profile_handling Profile Handling API
 *  Functions to handle profiles.

 *  @{
 */

/** @brief check an file if it is a profile
 *
 *  @param  filename  the filename to check
 *  @param  coloursig colour space signature as described in the
                      <a href="http://www.color.org">ICC specification</a>
 *  @return true/false
 */
int
oyCheckProfile (const char* filename, const char* coloursig)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  /* coloursig is currently ignored */
  n = oyCheckProfile_ (filename, coloursig);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** @brief check an memory block if it is a profile
 *
 *  @param  mem  the memory containing the profile
 *  @param  size the size of the memory block
 *  @param  coloursig currently ignored
 *  @return true/false
 */
int
oyCheckProfileMem (const void* mem, size_t size, const char* coloursig)
{
  int n = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  /* coloursig is currently ignored */
  n = oyCheckProfileMem_ (mem, size, coloursig);

  oyExportEnd_();
  DBG_PROG_ENDE
  return n;
}

/** @brief get the profiles size
 *
 *  @param  profilename  specifies the profile
 *  @return size
 */
size_t
oyGetProfileSize                  (const char* profilename)
{
  size_t size = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  size = oyGetProfileSize_ (profilename);

  oyExportEnd_();
  DBG_PROG_ENDE
  return size;
}

/** @brief obtain an memory block in the responsibility of the user
 *
 *  Please note the size parameter is sensible. Eighter set it to zero or to  
 *  the size obtained with oyGetProfileSize to get the full data or
 *  specify the size you want to get a partitial profile. \n
 *  This feature is useful for instance to get only the headers of many profiles.
 *
 *  @param  profilename  specifies the profile
 *  @param[in]  size     desired size, or zero for a complete copy
    @param[out] size     the size of the returned block
 *  @param      allocate_func the users memory allocation function
 *  @return             the profile block in memory allocated by oyAlloc_f    
 */
void*
oyGetProfileBlock                 (const char* profilename, size_t *size,
                                   oyAlloc_f     allocate_func)
{
  char* block = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  block = oyGetProfileBlock_ (profilename, size, allocate_func);
  DBG_PROG3_S( "%s %hd %d", profilename, (int)(intptr_t)block, (int)(intptr_t)*size)
  DBG_PROG

  oyExportEnd_();
  DBG_PROG_ENDE
  return block;
}

#if 1
/** Find out where in the Oyranos search path the specified profile resides.
 *
 *  @deprecated This function will be substituded by oyProfile_GetFileName.
 *
 *  @param  profile_name  the filename find in the Oyranos search path
 *  @param  allocate_func user provided function for allocating the string
                          memory
 *  @return the path name where the profile was found in the Oyranos search path
 */
char*
oyGetPathFromProfileName (const char* profile_name, oyAlloc_f     allocate_func)
{
  char* path_name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  path_name = oyGetPathFromProfileName_ (profile_name, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return path_name;
}
#endif

/** @} */



/**
 *  @internal 
 *  \addtogroup device_profiles Device Profiles API
 * 
 * There different approaches to select an (mostly) fitting profile
 *
 * A: search and compare all available profiles by \n
 *    - ICC profile class
 *    - Manufacturer / Model (as written in profile tags)
 *    - other hints\n
 *    .
 * B: install an profile and tell Oyranos about the belonging device and the
 *    invalidating period\n
 * C: look for similarities of devices of allready installed profiles\n
 * D: use the md5 checksum stored in the profile

 * @see @ref path_names

 * @{
 */

/** @brief ask for a profile name by specifying device attributes
 *
 *  @param typ            kind of device
 *  @param manufacturer   the device manufacturer (EIZO)
 *  @param model          the model (LCD2100)
 *  @param product_id     the ID reported during connection (ID_701200xx)
 *  @param host           useful for monitor identification (grafic:0.0)
 *  @param port           kind of connection (Matrox G650)
 *  @param attrib1        additional attribute
 *  @param attrib2        additional attribute
 *  @param attrib3        additional attribute
 *  @param allocate_func  the users memory allocation function
 * 
 *  simply pass 0 for not specified properties<br>

   \code
   char* profile_name = oyGetDeviceProfile ("EIZO", "LCD2100",
                                            "ID 87-135.19",
                                            "grafic:0.0", "Matrox G650",
                                             "100lux", 0,
                                            "", allocate_func);
   if (profile_name)
   { char* ptr = NULL;
     ptr = oyGetProfileBlock (profile_name, allocate_func);
       // do something
     free (ptr);
   }
   \endcode

   \return allocated by oyAlloc_f    
 */
char*
oyGetDeviceProfile                (oyDEVICETYP_e typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   oyAlloc_f     allocate_func)
{
  char* profile_name = NULL;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  profile_name = oyGetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    allocate_func);
  if(profile_name)
    DBG_PROG_S( (profile_name) );

  oyExportEnd_();
  DBG_PROG_ENDE
  return profile_name;
}

/** @brief set a profile name with specifying device attributes
 *  @param mem remains in the users domain
 */
int
oySetDeviceProfile                (oyDEVICETYP_e typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3,
                                   const char* profileName,
                                   const void* mem,
                                   size_t size)
{
  int rc = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  rc =         oySetDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3,
                                    profileName, mem, size);

  oyExportEnd_();
  DBG_PROG_ENDE
  return rc;
}

/** @brief remove or deinstall the profile from the current path */
int
oyEraseDeviceProfile              (oyDEVICETYP_e typ,
                                   const char* manufacturer,
                                   const char* model,
                                   const char* product_id,
                                   const char* host,
                                   const char* port,
                                   const char* attrib1,
                                   const char* attrib2,
                                   const char* attrib3)
{
  int rc = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  rc =       oyEraseDeviceProfile_ (manufacturer, model, product_id,
                                    host, port, attrib1, attrib2, attrib3);

  oyExportEnd_();
  DBG_PROG_ENDE
  return rc;
}


/** @} */



/**
 *  @internal
 *  \addtogroup cmm_handling CMM Handling API
 *
 *  @{
 */

/**
 *  @internal
 *  @brief  get the user allocated CMM 4 char ID's
 *
 *  @param  count          the number of CMM's available
 *  @param  allocate_func  the users memory allocation function
 *  @return allocated by oyAlloc_f    
 * 
   \code
   int    count, i;
   char** ids = oyModulsGetNames ( &count, allocate_func);
   for (i = 0; i < count; ++i)
   { printf( "CMM short name: %s\n", ids[i]);
     free (ids[i]);
   }
   if(count && ids)
     free (ids);
   \endcode

 */
char**
oyModulsGetNames       ( int        *count,
                         oyAlloc_f     allocate_func )
{
  char** ids = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CMMS);

  ids = oyModulsGetNames_(count, allocate_func);

  oyExportEnd_();
  DBG_PROG_ENDE
  return ids;
}
/** @} */

/** \addtogroup misc Miscellaneous
 *  Miscellaneous stuff.

 *  @{
 */

/** @brief  switch internationalisation of strings on or off
 *
 *  @param  active         bool
 *  @param  reserved       for future use
 */
void 
oyI18NSet              ( int active,
                         int reserved )
{
  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  oyI18NSet_(active, reserved);

  oyExportEnd_();
  DBG_PROG_ENDE
}

/** @brief  get language code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLanguage            ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyLanguage_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get country code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyCountry             ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyCountry_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get LANG code/variable
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLang                ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  text = oyLang_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief   reset i18n language and  country variables
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/05 (Oyranos: 0.1.10)
 *  @date    2009/01/05
 */
void           oyI18Nreset           ( void )
{
  DBG_PROG_START
  oyExportReset_(EXPORT_I18N);
  oyExportStart_(EXPORT_CHECK_NO);
  oyExportEnd_();
  DBG_PROG_ENDE
}


/** @brief  give the compiled in library version
 *
 *  @param[in]  type           0 - Oyranos API
 *                             1 - start month
 *                             2 - start year
 *                             3 - development last month
 *                             4 - development last year
 *
 *  @return                    OYRANOS_VERSION at library compile time
 */
int            oyVersion             ( int                 type )
{
  if(type == 1)
    return OYRANOS_START_MONTH;
  if(type == 2)
    return OYRANOS_START_YEAR;
  if(type == 3)
    return OYRANOS_DEVEL_MONTH;
  if(type == 4)
    return OYRANOS_DEVEL_YEAR;

  return OYRANOS_VERSION;
}

#include "config.log.h"
/** @brief  give the configure options for Oyranos
 *
 *  @param[in] type
                               - 1  OYRANOS_VERSION_NAME;
                               - 2  git master hash;
                               - 3  OYRANOS_CONFIG_DATE,
                               - 4  development period
 *  @param     allocateFunc    user allocator, e.g. malloc
 *
 *  @return                    Oyranos configure output
 *
 *  @since     Oyranos: version 0.1.8
 *  @date      18 december 2007 (API 0.1.8)
 */
char *       oyVersionString         ( int                 type,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0, * tmp = 0;
  char temp[24];
  char * git = OYRANOS_GIT_MASTER;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(type == 1)
    return oyStringCopy_(OYRANOS_VERSION_NAME, allocateFunc);
  if(type == 2)
  {
    if(git[0])
      return oyStringCopy_(git, allocateFunc);
    else
      return 0;
  }
  if(type == 3)
    return oyStringCopy_(OYRANOS_CONFIG_DATE, allocateFunc);

  if(type == 4)
  {
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(1)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
    oySprintf_( temp, " %d - ", oyVersion(2) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(3)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
    oySprintf_( temp, " %d", oyVersion(4) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);

    tmp = oyStringCopy_( text , allocateFunc);
    oyDeAllocateFunc_(text);
    return tmp;
  }

  return oyStringCopy_(oy_config_log_, allocateFunc);
}


/** @brief  calculate a md5 digest beginning after the header offset
 *
 *  The md5 implementation is written by L. Peter Deutsch
 *
 *  @param[in]  buffer         complete profiles buffer
 *  @param[in]  size           over all profile size
 *  @param[out] md5_return     buffer to write in the md5 digest (128 bytes)
 *
 *  @return                    error
 */
int
oyProfileGetMD5        ( void       *buffer,
                         size_t      size,
                         unsigned char *md5_return )
{
  int error = 0;
  DBG_PROG_START
  oyExportStart_(EXPORT_CHECK_NO);

  error = oyProfileGetMD5_(buffer, size, md5_return);

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}
/** @} */



