/** @file oyranos_io.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    input / output  methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_check.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_xml.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);



/* --- Helpers  --- */
/* small helpers */

/* --- function definitions --- */



char*
oyFindProfile_ (const char* fileName)
{
  char  *fullFileName = 0;
  char* path_name = 0;

  DBG_PROG_START

  if(!fileName || !fileName[0])
    return fullFileName;

  /*DBG_NUM_S((fileName)) */
  if (fileName && fileName[0] != OY_SLASH_C)
  {
    path_name = oyGetPathFromProfileName_(fileName, oyAllocateFunc_);

    if(!path_name)
    {
      FILE * fp = fopen( fileName, "rb" );
      if(fp)
      {
        path_name = oyStringCopy_( "./", oyAllocateFunc_ );
        fclose(fp); fp = 0;
      }
    }

    DBG_PROG
    if(path_name)
    {
      oyAllocString_m_( fullFileName, MAX_PATH,
                        oyAllocateFunc_, return 0 );
      if(strrchr(fileName,OY_SLASH_C) == NULL)
        oySprintf_(fullFileName, "%s%s%s", path_name, OY_SLASH, fileName);
      else
        oySprintf_( fullFileName, "%s%s%s", path_name, OY_SLASH,
                    strrchr( fileName, OY_SLASH_C ) + 1 );
    } else
    {
      DBG_PROG_ENDE
      return NULL;
    }
    DBG_PROG_S( fullFileName )
  }


  if(!path_name)
  {
    if (oyIsFileFull_(fileName,"rb")) {
      fullFileName = oyStringCopy_( fileName, oyAllocateFunc_ );
    } else
      fullFileName = oyMakeFullFileDirName_ (fileName);
  }

  if(path_name)
    oyFree_m_(path_name)

  DBG_PROG_ENDE
  return fullFileName;
}




/* public API implementation */

/* profile and other file lists API */

int oyProfileListCb_ (oyFileList_s * data,
                      const char* full_name, const char* filename)
{
  oyFileList_s *l = (oyFileList_s*)data;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

      if (!oyCheckProfile_(full_name, l->coloursig))
      {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(filename) + 1,
                          oyAllocateFunc_, return 1 );
        strcpy(l->names[l->count_files], filename);
        ++l->count_files;
      }

  return 0;
}

int oyPolicyListCb_ (oyFileList_s * data,
                     const char* full_name, const char* filename)
{
  oyFileList_s *l = (oyFileList_s*)data;
  /* last 4 chars */
  const char * end = NULL;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

  if(strlen(full_name) > 4)
    end = full_name + strlen(full_name) - 4;

      if( (end[0] == '.') &&
          (end[1] == 'x' || end[1] == 'X') &&
          (end[2] == 'm' || end[2] == 'M') &&
          (end[3] == 'l' || end[3] == 'L') &&
          !oyCheckPolicy_(full_name) )
      {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(full_name),
                          oyAllocateFunc_, return 1 );
        
        oyStrcpy_(l->names[l->count_files], full_name);
        ++l->count_files;
      } /*else */
        /*WARNc_S(("%s in %s is not a valid profile", file_name, path)); */
  return 0;
}

int oyFileListCb_ ( oyFileList_s * data,
                    const char * full_name, const char * filename)
{
  oyFileList_s *l = (oyFileList_s*)data;

  if(l->type != oyOBJECT_FILE_LIST_S_)
    WARNc_S("Could not find a oyFileList_s objetc.");

  {
        if(l->count_files >= l->mem_count)
        {
          char** temp = l->names;

          l->names = 0;
          oyAllocHelper_m_( l->names, char*, l->mem_count+l->hopp,
                            oyAllocateFunc_, return 1);
          memcpy(l->names, temp, sizeof(char*) * l->mem_count);
          l->mem_count += l->hopp;
        }

        oyAllocString_m_( l->names[l->count_files], oyStrblen_(full_name),
                          oyAllocateFunc_, return 1 );
        
        oyStrcpy_(l->names[l->count_files], full_name);
        ++l->count_files;
  }

  return 0;
}


char **  oyProfileListGet_           ( const char        * coloursig,
                                       uint32_t          * size )
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int32_t count = 0;/*oyPathsCount_();*/
  char ** path_names = NULL;

  path_names = oyProfilePathsGet_( &count, oyAllocateFunc_ );

  l.coloursig = coloursig;

  DBG_PROG_START
 
  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_( oyProfileListCb_, &l,
                     (const char**)path_names, count );

  oyStringListRelease_( &path_names, count, oyDeAllocateFunc_ );

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

char**
oyPolicyListGet_                  (int * size)
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
  DBG_PROG_START
 
  path_names = oyDataPathsGet_( &count, "color/settings", oyALL, oyUSER_SYS,
                                oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyPolicyListCb_, &l,(const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

/** @internal
 *  @brief get files out of the Oyranos default paths
 *
 *  Since: 0.1.8
 */
char**
oyFileListGet_                  (const char * subpath,
                                 int        * size,
                                 int          data,
                                 int          owner)
{
  oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
 
  DBG_PROG_START
 
  path_names = oyDataPathsGet_( &count, subpath, oyALL, oyUSER_SYS,
                                oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyFileListCb_, &l, (const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;

  if(!l.count_files && l.names)
  {
    oyDeAllocateFunc_(l.names);
    l.names = 0;
  }

  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}


/** @internal
 *  @brief get files out of the Oyranos default paths
 *
 *  Since: 0.1.8
 */
char**
oyLibListGet_                   (const char * subpath,
                                 int        * size,
                                 int          owner)
{
  struct oyFileList_s l = {oyOBJECT_FILE_LIST_S_, 128, NULL, 128, 0, 0};
  int count = 0;
  char ** path_names = NULL;
 
  DBG_PROG_START
 
  path_names = oyLibPathsGet_( &count, subpath, oyUSER_SYS,
                                  oyAllocateFunc_ );

  oy_warn_ = 0;

  l.names = 0;
  l.mem_count = l.hopp;
  l.count_files = 0;

  oyAllocHelper_m_(l.names, char*, l.mem_count, oyAllocateFunc_, return 0);

  oyRecursivePaths_(oyFileListCb_, &l, (const char**)path_names, count);

  oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);

  *size = l.count_files;
  oy_warn_ = 1;
  DBG_PROG_ENDE
  return l.names;
}

/* profile handling API */
size_t
oyReadFileSize_(const char* name);

size_t
oyGetProfileSize_                  (const char* profilename)
{
  size_t size = 0;
  char* fullFileName = oyFindProfile_ (profilename);

  DBG_PROG_START

  size = oyReadFileSize_ (fullFileName);

  DBG_PROG_ENDE
  return size;
}

void*
oyGetProfileBlock_                 (const char* profilename, size_t *size,
                                    oyAlloc_f     allocate_func)
{
  char* fullFileName = 0;
  char* block = 0;

  DBG_PROG_START

  fullFileName = oyFindProfile_ (profilename);

  if(fullFileName)
  {
    block = oyReadFileToMem_ (fullFileName, size, allocate_func);
    oyFree_m_( fullFileName );
  }

  DBG_PROG_ENDE
  return block;
}



