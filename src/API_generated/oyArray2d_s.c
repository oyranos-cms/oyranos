/** @file oyArray2d_s.c

   [Template file inheritance graph]
   +-> oyArray2d_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyArray2d_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyArray2d_s_.h"
  


/** Function oyArray2d_New
 *  @memberof oyArray2d_s
 *  @brief   allocate a new Array2d object
 */
OYAPI oyArray2d_s * OYEXPORT
  oyArray2d_New( oyObject_s object )
{
  oyObject_s s = object;
  oyArray2d_s_ * array2d = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  array2d = oyArray2d_New_( s );

  return (oyArray2d_s*) array2d;
}

/** Function  oyArray2d_Copy
 *  @memberof oyArray2d_s
 *  @brief    Copy or Reference a Array2d object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     array2d                 Array2d struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyArray2d_s* OYEXPORT
  oyArray2d_Copy( oyArray2d_s *array2d, oyObject_s object )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) array2d;

  if(s)
  {
    oyCheckType__m( oyOBJECT_ARRAY2D_S, return NULL )
  }
  else
    return NULL;

  s = oyArray2d_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyArray2d_s" );

  return (oyArray2d_s*) s;
}
 
/** Function oyArray2d_Release
 *  @memberof oyArray2d_s
 *  @brief   release and possibly deallocate a oyArray2d_s object
 *
 *  @param[in,out] array2d                 Array2d struct object
 */
OYAPI int OYEXPORT
  oyArray2d_Release( oyArray2d_s **array2d )
{
  oyArray2d_s_ * s = 0;

  if(!array2d || !*array2d)
    return 0;

  s = (oyArray2d_s_*) *array2d;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  *array2d = 0;

  return oyArray2d_Release_( &s );
}



/* Include "Array2d.public_methods_definitions.c" { */
#include "oyranos_image_internal.h"
#include "oyranos_core.h"

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


/** Function  oyArray2d_Create
 *  @memberof oyArray2d_s
 *  @brief    Allocate and initialise a oyArray2d_s object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s_ * s = NULL;
  int error = 0;

  if(!width || !height)
    return (oyArray2d_s*)s;

  s = oyArray2d_Create_( width, height, data_type, object );
  error = !s;

  if(error <= 0)
  {
    if(data)
      error = oyArray2d_SetData( (oyArray2d_s*)s, data );
    else
    {
      data = s->oy_->allocateFunc_( width * height *
                                    oyDataTypeGetSize( data_type ) );
      error = oyArray2d_SetData( (oyArray2d_s*)s, data );
      s->own_lines = oyYES;
    }
  }

  return (oyArray2d_s*)s;
}

#if 0
/** Function  oyArray2d_DataCopy
 *  @memberof oyArray2d_s
 *  @brief    Copy data
 *
 *  @todo just refere the other arrays, with refs_ and refered_ members,
 *        reuse memory
 *
 *  @param[in,out] obj                 the array to fill in
 *  @param[in]     roi_obj             rectangle of interesst in samples
 *  @param[in]     source              the source data
 *  @param[in]     roi_source          rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/12 (Oyranos: 0.1.10)
 *  @date    2009/03/12
 */
OYAPI int  OYEXPORT
                 oyArray2d_DataCopy  ( oyArray2d_s      ** obj,
                                       oyRectangle_s     * obj_source,
                                       oyArray2d_s       * source,
                                       oyRectangle_s     * roi_source )
{
  oyArray2d_s_ * s = *obj,
               * src = source;
  int error = 0;
  int new_roi = !roi;

  if(!src || !s)
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(error <= 0)
  {
    if(!roi)
      roi = oyRectangle_NewWith( 0,0, s->width, s->height, s->oy_ );
    error = !roi;
  }

  /* possibly expensive hack */
  if(*obj)
    oyArray2d_Release( obj );

  if(!(*obj))
  {
    *obj = oyArray2d_Create( 0, roi->height, roi->width, src->t, src->oy_ );
    error = !*obj;
  }

  if(error <= 0)
  {
    oyAlloc_f allocateFunc_ = s->oy_->allocateFunc_;
    int i, size;

    s->own_lines = 2;
    for(i = 0; i < roi->height; ++i)
    {
      size = roi->width * oyDataTypeGetSize( s->t );
      if(!s->array2d[i])
        oyAllocHelper_m_( s->array2d[i], unsigned char, size, allocateFunc_,
                          error = 1; break );
      error = !memcpy( s->array2d[i], src->array2d[i], size );
    }
  }

  if(error)
    oyArray2d_Release( obj );

  if(new_roi)
    oyRectangle_Release( &roi );

  return error;
}
#endif

/** Function  oyArray2d_GetData
 *  @memberof oyArray2d_s
 *  @brief    Get the data blob
 *
 *  @param[in,out] obj                 struct object
 *  @return                            the data, remains in the property of the
 *                                     object
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/25
 *  @since    2012/09/25 (Oyranos: 0.5.0)
 */
OYAPI oyPointer  OYEXPORT
                 oyArray2d_GetData   ( oyArray2d_s       * obj )
{
  oyArray2d_s_ * s = (oyArray2d_s_*)obj;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, OY_BACKTRACE_PRINT; return 0 )

  return s->array2d;
}
/** Function  oyArray2d_SetData
 *  @memberof oyArray2d_s
 *  @brief    Set the data blob and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     data                the data, remains in the property of the
 *                                     caller
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
OYAPI int  OYEXPORT
                 oyArray2d_SetData   ( oyArray2d_s       * obj,
                                       oyPointer           data )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) obj;
  int error = 0;

  if(!data)
    return 1;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, OY_BACKTRACE_PRINT; return 1 )

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    uint8_t * u8 = data;

    error = !s->array2d;

    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = oyNO;

    if(error <= 0)
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = &u8[oyDataTypeGetSize( s->t ) * s->width * y];
  }

  return error;
}

/** Function  oyArray2d_SetRows
 *  @memberof oyArray2d_s
 *  @brief    Set the data and (re-)initialise the object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     rows                the data
 *  @param[in]     do_copy             - 0 : take the memory as is
 *                                     - 1 : copy the memory monolithic
 *                                     - 2 : copy the memory chunky
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 *  @date    2010/09/07
 */
OYAPI int  OYEXPORT
                 oyArray2d_SetRows   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy )
{
  oyArray2d_s_ * s = (oyArray2d_s_*)obj;
  int error = 0;

  if(!rows)
    return 1;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, OY_BACKTRACE_PRINT; return 1 )

  {
    int y_len = sizeof(unsigned char *) * (s->height + 1),
        y;
    size_t size = 0;

    error = !s->array2d;

    size = s->width * oyDataTypeGetSize( s->t );

    oyArray2d_ReleaseArray_( (oyArray2d_s*)s );

    /* allocate the base array */
    oyStruct_AllocHelper_m_( s->array2d, unsigned char *, s->height+1, s,
                             error = 1; return 1 );
    if(error <= 0)
      error = !memset( s->array2d, 0, y_len );

    s->own_lines = do_copy;

    if(error <= 0 && s->own_lines == 2)
    {
      /* allocate each line separately */
      for(y = 0; y < s->height; ++y)
      {
        oyStruct_AllocHelper_m_( s->array2d[y], unsigned char, size, s,
                                 error = 1; break );
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 1)
    {
      /* allocate all lines at once */
      unsigned char * u8 = 0;
      oyStruct_AllocHelper_m_( u8, unsigned char, size * s->height, s,
                               error = 1; return 1 );

      s->own_lines = do_copy;
      if(error <= 0)
      for( y = 0; y < s->height; ++y )
      {
        s->array2d[y] = &u8[size * y];
        error = !memcpy( s->array2d[y], rows[y], size );
      }

    } else
    if(error <= 0 && s->own_lines == 0)
    {
      /* just assign */
      for( y = 0; y < s->height; ++y )
        s->array2d[y] = rows[y];
    }
  }

  return error;
}

/** Function  oyArray2d_SetFocus
 *  @memberof oyArray2d_s
 *  @brief    Move a arrays active area to a given rectangle
 *
 *  The array works in absolute coordinates.
 *  For working in relative coordinates get first the current data_area by 
 *  oyArray2d_GetDataGeo1().
 *
 *  @param[in,out] array               the channels array
 *  @param[in]     rectangle           the new region in the array's wholes data
 *  @return                            0 - success, 1 - error, -1 - focus changed
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/09/15
 *  @since   2011/04/19 (Oyranos: 0.3.0)
 */
int          oyArray2d_SetFocus      ( oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle )
{
  oyRectangle_s_ * array_roi_chan = (oyRectangle_s_*)rectangle;
  oyArray2d_s_ * a = (oyArray2d_s_*)array;
  int error = 0;
  int y;

  if(array && rectangle)
  {
    /* shift array focus to requested region */
    int bps = oyDataTypeGetSize( a->t );
    if(a->data_area.x != OY_ROUNDf(array_roi_chan->x))
    {
      int height = a->data_area.height + a->data_area.y;
      int shift = (OY_ROUNDf(array_roi_chan->x) + OY_ROUNDf(a->data_area.x)) * bps;
      for(y = a->data_area.y; y < height; ++y)
        a->array2d[y] += shift;
      a->data_area.x = -OY_ROUNDf(array_roi_chan->x);
      error = -1;
    }
    if(a->data_area.y != OY_ROUNDf(array_roi_chan->y))
    {
      a->array2d += OY_ROUNDf(array_roi_chan->y + a->data_area.y);
      a->data_area.y = -array_roi_chan->y;
      error = -1;
    }
    a->width = array_roi_chan->width;
    a->height = array_roi_chan->height;

    if(array_roi_chan->x + array_roi_chan->width  > a->data_area.width) error = 1;
    if(array_roi_chan->y + array_roi_chan->height > a->data_area.height) error = 2;

    if(error > 0 || oy_debug > 3)
    {
      int32_t channels = 1;
      oyOptions_FindInt( a->oy_->handles_, "channels", 0, &channels );
      oyMessageFunc_p( error ? oyMSG_WARN:oyMSG_DBG, (oyStruct_s*)a,
                       OY_DBG_FORMAT_ "%s rect: %gx%g+%g+%g (err:%d)", OY_DBG_ARGS_,
                       oyArray2d_Show( (oyArray2d_s*)a, channels ),
                       array_roi_chan->width, array_roi_chan->height,
                       array_roi_chan->x, array_roi_chan->y, error );
    }

  } else
    error = 1;

  return error;
}

/** Function  oyArray2d_Reset
 *  @memberof oyArray2d_s
 *  @brief    Reinitialise Array
 *
 *  @version  Oyranos: 0.9.0
 *  @since    2012/10/20 (Oyranos: 0.9.0)
 *  @date     2012/10/20
 */
OYAPI int OYEXPORT
                 oyArray2d_Reset     ( oyArray2d_s       * array,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type )
{
  oyArray2d_s_ * s = (oyArray2d_s_*)array;
  int error = 0;
  int y_len = sizeof(unsigned char *) * (height + 1);

  if(!oyDataTypeGetSize(data_type))
    return 1;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, OY_BACKTRACE_PRINT; return 1 )

  if(!width || !height)
    return 1;

  error = oyArray2d_ReleaseArray_( (oyArray2d_s*)s );
  /* allocate the base array */
  oyStruct_AllocHelper_m_( s->array2d, unsigned char *, height+1,
                           s, error = 1; return 1 );
  if(error <= 0)
    error = !memset( s->array2d, 0, y_len );

  if(error <= 0)
  {
    error = oyArray2d_Init_( s, width, height, data_type );
  }

  return error;
}

/** Function  oyArray2d_GetDataGeo1
 *  @memberof oyArray2d_s
 *  @brief    Get Geometry of the data rectangle
 *
 *  The function informs about reserves and possible offsets.
 *
 *  @param[in,out] array               the channels array
 *  @param[in]     x_y_w_h             dimension selector
 *  @return                            the position or dimension
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 */
OYAPI double  OYEXPORT
                 oyArray2d_GetDataGeo1(oyArray2d_s       * array,
                                       int                 x_y_w_h )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) array;
  if(!array)
    return 0;
  return oyRectangle_GetGeo1( (oyRectangle_s*)&s->data_area, x_y_w_h );
}

/** Function  oyArray2d_GetWidth
 *  @memberof oyArray2d_s
 *  @brief    Get data Width
 *
 *  @param[in,out] array               the channels array
 *  @return                            the width
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2012/10/05 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyArray2d_GetWidth  ( oyArray2d_s       * array )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) array;
  if(!array)
    return 0;
  return s->width;
}

/** Function  oyArray2d_GetHeight
 *  @memberof oyArray2d_s
 *  @brief    Get data Height
 *
 *  @param[in,out] array               the channels array
 *  @return                            the height
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2012/10/05 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyArray2d_GetHeight ( oyArray2d_s       * array )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) array;
  if(!array)
    return 0;
  return s->height;
}

/** Function  oyArray2d_GetType
 *  @memberof oyArray2d_s
 *  @brief    Get data Type
 *
 *  @param[in,out] array               the channels array
 *  @return                            the data type
 *
 *  @version  Oyranos: 0.9.0
 *  @date     2012/10/11
 *  @since    2012/10/11 (Oyranos: 0.9.0)
 */
OYAPI oyDATATYPE_e  OYEXPORT
                 oyArray2d_GetType   ( oyArray2d_s       * array )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) array;
  if(!array)
    return 0;
  return s->t;
}

/** Function  oyArray2d_Show
 *  @memberof oyArray2d_s
 *  @brief    Print array geometries
 *
 *  This function is not thread safe.
 *
 *  @param[in]     array               the channel array
 *  @param[in]     channels            the channel count
 *  @return                            a description string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/11
 *  @since   2016/09/26 (Oyranos: 0.9.6)
 */
OYAPI const char* OYEXPORT oyArray2d_Show (
                                       oyArray2d_s       * array,
                                       int                 channels )
{
  int error = 0;
  char * t = NULL;
  const char * text = NULL;
  oyArray2d_s_ * a = (oyArray2d_s_*) array;
  double c = channels;
  oyAlloc_f alloc;
  oyDeAlloc_f dealloc;

  if(!array)
    error = 1;

  if(c == 0)
    c = -1.0;

  if(!error)
  {
    const char * key = "///channels";

    alloc = oyObject_GetAlloc( array->oy_ );
    dealloc = oyObject_GetDeAlloc( array->oy_ );

    oyStringAddPrintf( &t, alloc,dealloc, "a[%d](%gx%g+%g+%g(%gx%d)%dc",
             oyStruct_GetId((oyStruct_s*)a),
             a->data_area.width / c, a->data_area.height,
             a->data_area.x / c, a->data_area.y,
             a->width / c, a->height,
             channels );

    /* store for debugging */
    error = oyOptions_SetFromInt( &a->oy_->handles_,
                                  key,
                                  channels, 0, OY_CREATE_NEW );
    oyObject_SetName( array->oy_, t, oyNAME_NAME );
    if(t) dealloc(t);
    text = oyObject_GetName( array->oy_, oyNAME_NAME );
  }

  return (text&&text[0])?text:"----";
}



/* } Include "Array2d.public_methods_definitions.c" */

