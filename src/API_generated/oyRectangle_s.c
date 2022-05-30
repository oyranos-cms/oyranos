/** @file oyRectangle_s.c

   [Template file inheritance graph]
   +-> oyRectangle_s.template.c
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


  
#include "oyRectangle_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyRectangle_s_.h"

#include "oyranos_image_internal.h"
#include "oyImage_s_.h"
  


/** Function oyRectangle_New
 *  @memberof oyRectangle_s
 *  @brief   allocate a new Rectangle object
 */
OYAPI oyRectangle_s * OYEXPORT
  oyRectangle_New( oyObject_s object )
{
  oyObject_s s = object;
  oyRectangle_s_ * rectangle = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  rectangle = oyRectangle_New_( s );

  return (oyRectangle_s*) rectangle;
}

/** Function  oyRectangle_Copy
 *  @memberof oyRectangle_s
 *  @brief    Copy or Reference a Rectangle object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     rectangle                 Rectangle struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyRectangle_s* OYEXPORT
  oyRectangle_Copy( oyRectangle_s *rectangle, oyObject_s object )
{
  oyRectangle_s_ * s = (oyRectangle_s_*) rectangle;

  if(s)
  {
    oyCheckType__m( oyOBJECT_RECTANGLE_S, return NULL )
  }
  else
    return NULL;

  s = oyRectangle_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyRectangle_s" );

  return (oyRectangle_s*) s;
}
 
/** Function oyRectangle_Release
 *  @memberof oyRectangle_s
 *  @brief   release and possibly deallocate a oyRectangle_s object
 *
 *  @param[in,out] rectangle                 Rectangle struct object
 */
OYAPI int OYEXPORT
  oyRectangle_Release( oyRectangle_s **rectangle )
{
  oyRectangle_s_ * s = 0;

  if(!rectangle || !*rectangle)
    return 0;

  s = (oyRectangle_s_*) *rectangle;

  oyCheckType__m( oyOBJECT_RECTANGLE_S, return 1 )

  *rectangle = 0;

  return oyRectangle_Release_( &s );
}



/* Include "Rectangle.public_methods_definitions.c" { */
/** Function  oyRectangle_CountPoints
 *  @memberof oyRectangle_s
 *  @brief    Count number of points covered by this rectangle
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
double         oyRectangle_CountPoints(oyRectangle_s     * rectangle )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  oyRectangle_s_ * r = s;

  if(!s)
    return FALSE;

  return r->width * r->height;
}

/** Function  oyRectangle_Index
 *  @memberof oyRectangle_s
 *  @brief    Return position inside rectangle, assuming rectangle size
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_Index     ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  oyRectangle_s_ * r = s;

  if(!s)
    return FALSE;

  return OY_ROUND((y - r->y) * r->width + (x - r->x));
}

/**
 *  @brief   compare
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
int            oyRectangle_IsEqual   ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 )
{
  int equal = TRUE;
  oyRectangle_s_ * r1 = (oyRectangle_s_*)rectangle1;
  oyRectangle_s_ * r2 = (oyRectangle_s_*)rectangle2;

  if(!r1 || !r2)
    return FALSE;

  if (r1->x != r2->x) equal = FALSE;
  if (r1->y != r2->y) equal = FALSE;
  if (r1->width != r2->width) equal = FALSE;
  if (r1->height != r2->height) equal = FALSE;
  return equal;
}

/** Function  oyRectangle_IsInside
 *  @memberof oyRectangle_s
 *  @brief    Compare
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/23 (Oyranos: 0.1.10)
 *  @date    2009/02/23
 */
int            oyRectangle_IsInside  ( oyRectangle_s     * test,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s_ * test_ = (oyRectangle_s_*)test;

  return oyRectangle_PointIsInside( ref, test_->x, test_->y ) &&
         oyRectangle_PointIsInside( ref, test_->x + test_->width - 1, test_->y ) &&
         oyRectangle_PointIsInside( ref, test_->x + test_->width - 1,
                                      test_->y + test_->height - 1) &&
         oyRectangle_PointIsInside( ref, test_->x, test_->y + test_->height - 1 );
}

/** Function  oyRectangle_MoveInside
 *  @memberof oyRectangle_s
 *  @brief    Trim edit_rectangle to ref extents
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_MoveInside( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)edit_rectangle;
  oyRectangle_s_ * a = (oyRectangle_s_*)ref;

  if(!s)
    return;

  oyRectangle_Normalise( edit_rectangle );

  if (s->x < a->x)
    s->x = a->x;
  if (s->x+s->width > a->x+a->width)
  { if (s->width > a->width)
      ; /* keep */
    else
      s->x = a->x+a->width - s->width;
  }
  if (s->y < a->y)
    s->y = a->y;
  if (s->y+s->height  > a->y+a->height)
  { if (s->height > a->height)
      ; /* keep */
    else
      s->y = a->y+a->height - s->height;
  }
}

/** Function  oyRectangle_NewFrom
 *  @memberof oyRectangle_s
 *  @brief    New from other rectangle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewFrom   ( oyRectangle_s     * ref,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New( object );
  if(s)
    oyRectangle_SetByRectangle(s, ref);
  return s;
}

/** Function  oyRectangle_NewWith
 *  @brief    New with geometry
 *  @memberof oyRectangle_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
oyRectangle_s* oyRectangle_NewWith   ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object )
{
  oyRectangle_s * s = oyRectangle_New( object );
  if(s)
    oyRectangle_SetGeo( s, x, y, width, height );
  return s;
}

/** Function  oyRectangle_Normalise
 *  @memberof oyRectangle_s
 *  @brief    Normalise swapped values for width and height
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Normalise ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s_ * r = (oyRectangle_s_*)s;

  if(!s)
    return;

  if(r->width < 0) {
    r->x += r->width;
    r->width = fabs(r->width);
  }
  if(r->height < 0) {
    r->y += r->height;
    r->height = fabs(r->height);
  }
}

/** Function  oyRectangle_PointIsInside
 *  @memberof oyRectangle_s
 *  @brief    Compare
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2009/02/23
 */
int            oyRectangle_PointIsInside (
                                       oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y )
{
  oyRectangle_s * s = rectangle;
  oyRectangle_s_ * r = (oyRectangle_s_*)s;
  int in = TRUE;

  if(!s)
    return FALSE;

  if (x < r->x) return FALSE;
  if (y < r->y) return FALSE;
  if (x >= (r->x + r->width)) return FALSE;
  if (y >= (r->y + r->height)) return FALSE;
  return in;
}

/** Function  oyRectangle_Round
 *  @memberof oyRectangle_s
 *  @brief    Scale with origin in the top left corner
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void           oyRectangle_Round     ( oyRectangle_s     * edit_rectangle )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s_ * r = (oyRectangle_s_*)s;

  if(!s)
    return;

  r->x = OY_ROUND(r->x);
  r->y = OY_ROUND(r->y);
  r->width = OY_ROUND(r->width);
  r->height = OY_ROUND(r->height);
}

/** Function  oyRectangle_Scale
 *  @brief    Scale with origin in the top left corner
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2007/12/04
 */
void           oyRectangle_Scale     ( oyRectangle_s     * edit_rectangle,
                                       double              factor )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s_ * r = (oyRectangle_s_*)s;

  if(!s)
    return;

  r->x *= factor;
  r->y *= factor;
  r->width *= factor;
  r->height *= factor;
}

/** Function  oyRectangle_SetByRectangle
 *  @memberof oyRectangle_s
 *  @brief    Copy values
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2007/12/04
 */
void           oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s_ ** ref_ = (oyRectangle_s_**)&ref;

  if(!s || !ref)
    return;

  oyRectangle_SetGeo( s, (*ref_)->x, (*ref_)->y, (*ref_)->width, (*ref_)->height );
}

/** Function  oyRectangle_SetGeo
 *  @brief    Set geometry
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2007/12/04
 */
void           oyRectangle_SetGeo    ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)edit_rectangle;
  if(!s)
    return;

  s->x = x;
  s->y = y;
  s->width = width;
  s->height = height;
}

/**
 *  @brief    get geometry
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
void           oyRectangle_GetGeo    ( oyRectangle_s     * rectangle,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height )
{
  oyRectangle_s_ * s = (oyRectangle_s_*) rectangle;
  if(!s)
    return;

  *x = s->x;
  *y = s->y;
  *width = s->width;
  *height = s->height;
}

/** Function  oyRectangle_GetGeo1
 *  @brief    Get single geometry
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/06 (Oyranos: 0.5.0)
 *  @date    2012/09/06
 */
OYAPI double  OYEXPORT
                 oyRectangle_GetGeo1 ( oyRectangle_s     * rectangle,
                                       int                 x_y_w_h )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  if(!s)
    return 0;

  if(x_y_w_h == 0)
    return s->x;
  else if(x_y_w_h == 1)
    return s->y;
  else if(x_y_w_h == 2)
    return s->width;
  else if(x_y_w_h == 3)
    return s->height;
  else
    return 0;
}

/** Function  oyRectangle_SetGeo1
 *  @brief    Set single geometry
 *  @memberof oyRectangle_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/25
 *  @since    2012/09/25 (Oyranos: 0.5.0)
 */
OYAPI double *  OYEXPORT
                 oyRectangle_SetGeo1 ( oyRectangle_s     * rectangle,
                                       int                 x_y_w_h )
{
  oyRectangle_s_ * s = (oyRectangle_s_*)rectangle;
  if(!s)
    return NULL;

  if(x_y_w_h == 0)
    return &s->x;
  else if(x_y_w_h == 1)
    return &s->y;
  else if(x_y_w_h == 2)
    return &s->width;
  else if(x_y_w_h == 3)
    return &s->height;
  return NULL;
}

/** Function  oyRectangle_Show
 *  @memberof oyRectangle_s
 *  @brief    Debug text
 *
 *  This function is not not thread safe.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/04
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 */
const char*    oyRectangle_Show      ( oyRectangle_s     * rect )
{
  char *t = NULL;
  const char * text = NULL;
  oyRectangle_s_ * s = (oyRectangle_s_*)rect;
  oyAlloc_f alloc;
  oyDeAlloc_f dealloc;

  if(!s)
    return "";

  alloc = oyObject_GetAlloc( rect->oy_ );
  dealloc = oyObject_GetDeAlloc( rect->oy_ );

  oyStringAddPrintf( &t, alloc,dealloc, "%gx%g%s%g%s%g", s->width,s->height,
                     s->x<0?"":"+", s->x, s->y<0?"":"+", s->y);

  oyObject_SetName( rect->oy_, t, oyNAME_NAME );
  if(t) dealloc(t);
  text = oyObject_GetName( rect->oy_, oyNAME_NAME );

  return (text&&text[0])?text:"----";
}

/** Function  oyRectangle_Trim
 *  @brief    Trim edit_rectangle to ref extents
 *  @memberof oyRectangle_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/12/04 (Oyranos: 0.1.8)
 *  @date    2007/12/04
 */
void           oyRectangle_Trim      ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref )
{
  oyRectangle_s * s = edit_rectangle;
  oyRectangle_s_ * e = (oyRectangle_s_*)edit_rectangle;
  oyRectangle_s_ * r = (oyRectangle_s_*)ref;
  if(!s)
    return;

  if (e->x < r->x)
  {
    e->width -= r->x - e->x;
    e->x = r->x;
  }
  if (e->x + e->width > r->x + r->width)
    e->width -= (e->x + e->width) - (r->x + r->width);
  if( e->width < 0 )
    e->width = 0;

  if (e->y < r->y)
  {
    e->height -= r->y - e->y;
    e->y = r->y;
  }
  if (e->y + e->height > r->y + r->height)
    e->height -= (e->y + e->height) - (r->y + r->height);
  if( e->height < 0 )
    e->height = 0;

  oyRectangle_Normalise( edit_rectangle );
}

/* } Include "Rectangle.public_methods_definitions.c" */

