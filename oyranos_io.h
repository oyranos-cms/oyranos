/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 *  input / output methods
 * 
 */

/* Date:      25. 11. 2004 */

#ifndef OYRANOS_IO_H
#define OYRANOS_IO_H

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

/* --- Helpers  --- */

/* --- static variables   --- */

#define OY_WEB_RGB "sRGB.icc"
static int oyranos_init;
extern int oy_warn_;


int     oySetProfile_Block                (const char* name,
                                           void* mem,
                                           size_t size,
                                           oyDEFAULT_PROFILE type,
                                           const char* comnt);

/* --- internal API definition --- */

/* separate from the external functions */
char* oyGetPathFromProfileName_ (const char*   profilename,
                                 oyAllocFunc_t allocate_func);


char**  oyProfileList_                 (const char* coloursig, int * size);
void    oyProfileListFree_             (char** list, int size);

int	oyCheckProfile_                    (const char* name,const char* coloursig);
int	oyCheckProfile_Mem                 (const void* mem, size_t size,
                                        const char* coloursig);

size_t	oyGetProfileSize_                 (const char*   profilename);
void*	oyGetProfileBlock_                (const char*   profilename,
                                           size_t       *size,
                                           oyAllocFunc_t allocate_func);


/* complete an name from file including oyResolveDirFileName */
char*   oyMakeFullFileDirName_     (const char* name);
/* find an file/dir and do corrections on  ~ ; ../  */
char*   oyResolveDirFileName_      (const char* name);
char*   oyExtractPathFromFileName_ (const char* name);
char*   oyGetHomeDir_              ();
char*   oyGetParent_               (const char* name);
int     oyRecursivePaths_      (int (*doInPath) (void*,const char*,const char*),
                                void* data);

int oyIsDir_      (const char* path);
int oyIsFile_     (const char* fileName);
int oyIsFileFull_ (const char* fullFileName);
int oyMakeDir_    (const char* path);

int   oyWriteMemToFile_ (const char* name, void* mem, size_t size);
char* oyReadFileToMem_  (const char* fullFileName, size_t *size,
                         oyAllocFunc_t allocate_func);

/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();
/* search in profile path and in current path */
char* oyFindProfile_ (const char* name);


#endif /* OYRANOS_IO_H */
