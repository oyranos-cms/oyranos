/*
 * jpegmarkers.h
 *
 * Copyright (C) 1991-1998, Thomas G. Lane.
 * Modified 2002-2013 by Guido Vollbeding.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the application interface for the JPEG library.
 * Most applications using the library need only include this file,
 * and perhaps jerror.h if they want to know the exact error codes.
 */

#ifndef JPEGMARKERS_H
#define JPEGMARKERS_H

#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
extern "C" {
#endif
#endif

#include <stdio.h>		/* needed to define "FILE", "NULL" */
#include "jpeglib.h"


/*
 * This routine writes the given data into a JPEG file.
 * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
 * the first call to jpeg_write_scanlines().
 * (This ordering ensures that the APP0 marker(s) will appear after the
 * SOI and JFIF or Adobe markers, but before all else.)
 * Data size can exceed the JPEG 65533-marker_name_length limit like 
 * with ICC profiles. For that APP0+2 markers can contain longer markers,
 * which are split into as many as needed parts.
 */
EXTERN(void) jpeg_write_marker_APP JPP((j_compress_ptr cinfo,
                   unsigned int marker_code,
                   const JOCTET *marker_name,
                   unsigned int marker_name_length,
		   const JOCTET *data_ptr,
		   unsigned int data_len));
/*
 * Tell how many APP0 markers are present.
 * Return error.
 */
EXTERN(int) jpeg_count_markers JPP((j_decompress_ptr cinfo,
                              int *markers_count));
/*
 * Obtain single marker.
 * Return error.
 */
EXTERN(int) jpeg_get_marker JPP((j_decompress_ptr cinfo,
                              int pos,
                              jpeg_saved_marker_ptr *marker_return));
/*
 * Obtain single marker name, name size estimation and APP0 code.
 * Return error.
 */
EXTERN(int) jpeg_get_marker_name JPP((j_decompress_ptr cinfo,
                          int pos,
                          unsigned int *marker_code,
                          JOCTET **marker_name,
                          int *marker_name_length));
/*
 * Obtain single marker size in order to allocate memory.
 * This function can be used to check for the existence of a marker.
 * Return error.
 */
EXTERN(int) jpeg_get_marker_size JPP((j_decompress_ptr cinfo,
                          unsigned int marker_code,
                          JOCTET *marker_name,
                          int marker_name_length,
                          unsigned int *data_len));
/*
 * Obtain single marker data in user allocated memory.
 * Return error.
 */
EXTERN(int) jpeg_get_marker_data JPP((j_decompress_ptr cinfo,
                          unsigned int marker_code,
                          JOCTET *marker_name,
                          int marker_name_length,
                          unsigned int data_len,
                          JOCTET *data_ptr));



#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
}
#endif
#endif

#endif /* JPEGMARKERS_H */
