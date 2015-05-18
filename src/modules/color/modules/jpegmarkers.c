/*
 * jpegmarkers.c
 *
 * Copyright (C) 1991-1996, Thomas G. Lane.
 * Modified 2014 by Kai-Uwe Behrmann
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides code to read and write International Color Consortium
 * (ICC) device profiles embedded in JFIF JPEG image files.  The ICC has
 * defined a standard format for including such data in JPEG "APP2" markers.
 * The code given here does not know anything about the internal structure
 * of the ICC profile data; it just knows how to put the profile data into
 * a JPEG file being written, or get it back out when reading.
 *
 * This code depends on new features added to the IJG JPEG library as of
 * IJG release 6b; it will not compile or work with older IJG versions.
 *
 * NOTE: this code would need surgery to work on 16-bit-int machines
 * with ICC profiles exceeding 64K bytes in size.  If you need to do that,
 * change all the "unsigned int" variables to "INT32".  You'll also need
 * to find a malloc() replacement that can allocate more than 64K.
 *
 * The code and API was transformed to support any kind of APP markers.
 */


#include "jpegmarkers.h"
#include <stdlib.h>			/* define malloc() */
#include <string.h>			/* define strstr() */

/*
 * Since some data can be larger than the maximum size of a JPEG marker
 * (64K), we need provisions to split it into multiple markers.  The format
 * defined by the ICC specifies one or more APP2 markers containing the
 * following data:
 *	Identifying string	ASCII "ICC_PROFILE\0"  (12 bytes)
 *	Marker sequence number	1 for first APP2, 2 for next, etc (1 byte)
 *	Number of markers	Total number of APP2's used (1 byte)
 *      Profile data		(remainder of APP2 data)
 * Decoders should use the marker sequence numbers to reassemble the profile,
 * rather than assuming that the APP2 markers appear in the correct sequence.
 */

#define APP2_MARKER  (JPEG_APP0 + 2)	/* JPEG marker code for ICC */
#define MAX_BYTES_IN_MARKER  65533	/* maximum data len of a JPEG marker */
#define MAX_DATA_BYTES_IN_MARKER  (MAX_BYTES_IN_MARKER - marker_name_length)
#define MAX_APP2_SEQ_NO  255		/* sufficient since marker numbers are bytes */

/*
 * This routine writes the given APP2 data into a JPEG file.
 * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
 * the first call to jpeg_write_scanlines().
 * (This ordering ensures that the APP2 marker(s) will appear after the
 * SOI and JFIF or Adobe markers, but before all else.)
 */

void jpeg_write_marker_APP2 (j_compress_ptr cinfo,
                   const JOCTET *marker_name,
                   unsigned int marker_name_length,
		   const JOCTET *data_ptr,
		   unsigned int data_len)
{
  unsigned int num_markers;	/* total number of markers we'll write */
  int cur_marker = 1;		/* per spec, counting starts at 1 */
  unsigned int length;		/* number of bytes to write in this marker */
  unsigned char *marker_name_bytes = (unsigned char*) marker_name;
  int i;

  /* Calculate the number of markers we'll need, rounding up of course */
  num_markers = data_len / MAX_DATA_BYTES_IN_MARKER;
  if (num_markers * MAX_DATA_BYTES_IN_MARKER != data_len)
    num_markers++;

  while (data_len > 0) {
    /* length of profile to put in this marker */
    length = data_len;
    if (length > MAX_DATA_BYTES_IN_MARKER)
      length = MAX_DATA_BYTES_IN_MARKER;
    data_len -= length;

    /* Write the JPEG marker header (APP2 code and marker length) */
    jpeg_write_m_header(cinfo, APP2_MARKER,
			(unsigned int) (length + marker_name_length));

    /* Write the marker identifying string (null-terminated).
     * We code it in this less-than-transparent way so that the code works
     * even if the local character set is not ASCII.
     */
    for(i = 0; i < marker_name_length; ++i)
      jpeg_write_m_byte(cinfo, marker_name_bytes[i]);

    /* Add the sequencing info */
    jpeg_write_m_byte(cinfo, cur_marker);
    jpeg_write_m_byte(cinfo, (int) num_markers);

    /* Add the profile data */
    while (length--) {
      jpeg_write_m_byte(cinfo, *data_ptr);
      data_ptr++;
    }
    cur_marker++;
  }
}

/*
 * This routine writes the given APP0+n data into a JPEG file.
 * It *must* be called AFTER calling jpeg_start_compress() and BEFORE
 * the first call to jpeg_write_scanlines().
 * (This ordering ensures that the APP0+n marker(s) will appear after the
 * SOI and JFIF or Adobe markers, but before all else.)
 */
void jpeg_write_marker_APP (j_compress_ptr cinfo,
                   unsigned int marker_code,
                   const JOCTET *marker_name,
                   unsigned int marker_name_length,
		   const JOCTET *data_ptr,
		   unsigned int data_len)
{
  unsigned char *marker_name_bytes = (unsigned char*) marker_name;
  unsigned int i;

  if (marker_code == JPEG_APP0+2)
    jpeg_write_marker_APP2(cinfo, marker_name, marker_name_length, data_ptr, data_len);

  else if (data_len > 0) {

    /* Write the JPEG marker header (APP0+n code and marker length) */
    jpeg_write_m_header(cinfo, marker_code,
			(unsigned int) (data_len + marker_name_length));

    /* Write the marker identifying string (null-terminated).
     * We code it in this less-than-transparent way so that the code works
     * even if the local character set is not ASCII.
     */
    for(i = 0; i < marker_name_length; ++i)
      jpeg_write_m_byte(cinfo, marker_name_bytes[i]);

    /* Add the profile data */
    while (data_len--) {
      jpeg_write_m_byte(cinfo, *data_ptr);
      data_ptr++;
    }
  }
}




int marker_guess_name_length       (jpeg_saved_marker_ptr marker)
{
  int len = 0;
  while(len < marker->data_length && marker->data[len])
    ++len;
  /* count terminating '\000' */
  if(len < marker->data_length) ++len;
  return len;
}

int jpeg_count_markers   (j_decompress_ptr cinfo,
                              int * markers_count)
{
  jpeg_saved_marker_ptr marker;

  *markers_count = 0;


  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    *markers_count += 1;
  }

  return FALSE;
}

int jpeg_get_marker      (j_decompress_ptr cinfo,
                              int pos,
                              jpeg_saved_marker_ptr *marker_return)
{
  jpeg_saved_marker_ptr marker;
  int marker_pos = 0;


  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (marker_pos == pos)
    {
      *marker_return = marker;
      return FALSE;
    }
    ++marker_pos;
  }

  return TRUE;
}

int jpeg_get_marker_name (j_decompress_ptr cinfo,
                              int pos,
                              unsigned int *marker_code,
                              JOCTET **marker_name,
                              int *marker_name_length)
{
  jpeg_saved_marker_ptr marker;
  int marker_pos = 0;


  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (marker_pos == pos)
    {
      *marker_code = marker->marker;
      *marker_name = marker->data;
      *marker_name_length = marker_guess_name_length(marker);
      return FALSE;
    }
    ++marker_pos;
  }

  return TRUE;
}

int jpeg_marker_is (jpeg_saved_marker_ptr marker,
                        unsigned int marker_code,
                        JOCTET *marker_name,
                        int marker_name_length)
{
  return
    marker->marker == marker_code &&
    marker->data_length >= marker_name_length &&
    /* verify the identifying string */
    memcmp(marker->data, marker_name, marker_name_length) == 0;
}

int jpeg_get_marker_size (j_decompress_ptr cinfo,
                          unsigned int marker_code,
                          JOCTET *marker_name,
                          int marker_name_length,
                          unsigned int *data_len)
{
  jpeg_saved_marker_ptr marker;
  int num_markers = 0;
  int seq_no;
  unsigned int total_length;
  char marker_present[MAX_APP2_SEQ_NO+1];	  /* 1 if marker found */
  unsigned int data_length[MAX_APP2_SEQ_NO+1]; /* size of profile data in marker */
  int marker_overhead = (marker_code == (JPEG_APP0 + 2)) ? marker_name_length + 2 : marker_name_length;

  *data_len = 0;

  /* This first pass over the saved markers discovers whether there are
   * any APP0+2 markers and verifies the consistency of the marker numbering.
   */

  for (seq_no = 1; seq_no <= MAX_APP2_SEQ_NO; seq_no++)
    marker_present[seq_no] = 0;


  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (jpeg_marker_is(marker, marker_code, marker_name, marker_name_length)) {
      if (marker_code == (JPEG_APP0 + 2)) {
        if (num_markers == 0)
          num_markers = GETJOCTET(marker->data[13]);
        else if (num_markers != GETJOCTET(marker->data[13]))
          return TRUE;		/* inconsistent num_markers fields */
        seq_no = GETJOCTET(marker->data[12]);
        if (seq_no <= 0 || seq_no > num_markers)
          return TRUE;		/* bogus sequence number */
        if (marker_present[seq_no])
          return TRUE;		/* duplicate sequence numbers */
        marker_present[seq_no] = 1;
      } else {
        seq_no = 1;
        num_markers = 1;
        marker_present[seq_no] = 1;
      }
      data_length[seq_no] = marker->data_length - marker_overhead;
    }
  }

  if (num_markers == 0)
    return TRUE;

  /* Check for missing markers, count total space needed,
   * compute offset of each marker's part of the data.
   */

  total_length = 0;
  for (seq_no = 1; seq_no <= num_markers; seq_no++) {
    if (marker_present[seq_no] == 0)
      return TRUE;		/* missing sequence number */
    total_length += data_length[seq_no];
  }

  if (total_length <= 0)
    return TRUE;		/* found only empty markers? */

  *data_len = total_length;

  return FALSE;
}

int jpeg_get_marker_data (j_decompress_ptr cinfo,
                          unsigned int marker_code,
                          JOCTET *marker_name,
                          int marker_name_length,
                          unsigned int data_len,
                          JOCTET *data_ptr)
{
  jpeg_saved_marker_ptr marker;
  int num_markers = 0;
  int seq_no;
  unsigned int total_length,
               pos;
  char marker_present[MAX_APP2_SEQ_NO+1];	  /* 1 if marker found */
  unsigned int data_length[MAX_APP2_SEQ_NO+1]; /* size of profile data in marker */
  unsigned int data_offset[MAX_APP2_SEQ_NO+1]; /* offset for data in marker */
  int marker_overhead = (marker_code == (JPEG_APP0 + 2)) ? marker_name_length + 2 : marker_name_length;

  /* This first pass over the saved markers discovers whether there are
   * any APP0+2 markers and verifies the consistency of the marker numbering.
   */

  for (seq_no = 1; seq_no <= MAX_APP2_SEQ_NO; seq_no++)
    marker_present[seq_no] = 0;


  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (jpeg_marker_is(marker, marker_code, marker_name, marker_name_length)) {
      if (marker_code == (JPEG_APP0 + 2)) {
        if (num_markers == 0)
          num_markers = GETJOCTET(marker->data[13]);
        else if (num_markers != GETJOCTET(marker->data[13]))
          return TRUE;		/* inconsistent num_markers fields */
        seq_no = GETJOCTET(marker->data[12]);
        if (seq_no <= 0 || seq_no > num_markers)
          return TRUE;		/* bogus sequence number */
        if (marker_present[seq_no])
          return TRUE;		/* duplicate sequence numbers */
        marker_present[seq_no] = 1;
      } else {
        seq_no = 1;
        num_markers = 1;
        marker_present[seq_no] = 1;
      }
      data_length[seq_no] = marker->data_length - marker_overhead;
    }
  }

  if (num_markers == 0)
    return TRUE;

  /* Check for missing markers, count total space needed,
   * compute offset of each marker's part of the data.
   */

  total_length = 0;
  for (seq_no = 1; seq_no <= num_markers; seq_no++) {
    if (marker_present[seq_no] == 0)
      return TRUE;		/* missing sequence number */
    data_offset[seq_no] = total_length;
    total_length += data_length[seq_no];
  }

  if (total_length <= 0)
    return TRUE;		/* found only empty markers? */

  pos = 0;

  /* and fill it in */
  for (marker = cinfo->marker_list; marker != NULL; marker = marker->next) {
    if (jpeg_marker_is(marker, marker_code, marker_name, marker_name_length)) {
      JOCTET FAR *src_ptr;
      JOCTET *dst_ptr;
      unsigned int length;
      if (marker_code == (JPEG_APP0 + 2))
        seq_no = GETJOCTET(marker->data[12]);
      else
        seq_no = 1;
      dst_ptr = data_ptr + data_offset[seq_no];
      src_ptr = marker->data + marker_overhead;
      length = data_length[seq_no];
      while (length--) {
        if (pos++ > data_len)
          break;
	*dst_ptr++ = *src_ptr++;
      }
    }
  }

  return FALSE;
}

/*------------------------------------------------------------------------*/
/* added from Tom Lanes jpegicc.c and modified by ku.b */
