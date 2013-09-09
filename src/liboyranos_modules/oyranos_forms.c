/** @file oyranos_forms.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    XFORMS renderer arguments handling
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/12
 */



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_forms.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_string.h"



/** Function oyFormsArgs_New
 *  @memberof oyFormsArgs_s
 *  @brief   allocate a new FormsArgs object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI oyFormsArgs_s * OYEXPORT
           oyFormsArgs_New           ( void              * object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FORMS_ARGS_S;
# define STRUCT_TYPE oyFormsArgs_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc1_S("memset error: %d", error);

  s->type_ = type;
  s->copy = oyFormsArgs_Copy;
  s->release = oyFormsArgs_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** @internal
 *  Function oyFormsArgs_Copy_
 *  @memberof oyFormsArgs_s
 *  @brief   real copy a FormsArgs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
oyFormsArgs_s * oyFormsArgs_Copy_
                                     ( oyFormsArgs_s     * obj,
                                       oyObject_s          object )
{
  oyFormsArgs_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyFormsArgs_New( object );
  error = !s;

  if(error)
    oyFormsArgs_Release( &s );

  return s;
}

/** Function oyFormsArgs_Copy
 *  @memberof oyFormsArgs_s
 *  @brief   copy or reference a FormsArgs object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI oyFormsArgs_s * OYEXPORT
           oyFormsArgs_Copy          ( oyFormsArgs_s     * obj,
                                       void              * object )
{
  oyFormsArgs_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( (oyObject_s)s->oy_ );
    return s;
  }

  s = oyFormsArgs_Copy_( obj, object );

  return s;
}
 
/** Function oyFormsArgs_Release
 *  @memberof oyFormsArgs_s
 *  @brief   release and possibly deallocate a FormsArgs object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
OYAPI int  OYEXPORT
           oyFormsArgs_Release       ( oyFormsArgs_s    ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFormsArgs_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef((oyObject_s)s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(((oyObject_s)s->oy_)->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = ((oyObject_s)s->oy_)->deallocateFunc_;

    oyObject_Release( (oyObject_s*)&s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** Function oyFormsArgs_ModelGet
 *  @memberof oyFormsArgs_s
 *  @brief   obtain a XFORMS model from a FormsArgs object
 *
 *  @param[in,out] args                struct object
 *  @return                            the model XML
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
const char * oyFormsArgs_ModelGet    ( oyFormsArgs_s     * args )
{
  if(args)
    return oyOptions_GetText( args->xforms_data_model_, oyNAME_NAME );
  else
    return 0;
}


/** Function oyFormsArgs_ResourceSet
 *  @memberof oyFormsArgs_s
 *  @brief   set a resource to be used by the renderer
 *
 *  @param[in,out] args                argument object
 *  @param[in]     registration        a registration key name for the resource
 *  @param[in]     resource            the resource
 *  @return                            0 - good; 1 - error; -1 <= issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int          oyFormsArgs_ResourceSet ( oyFormsArgs_s     * args,
                                       const char        * registration,
                                       oyPointer           resource )
{
  int error = !args;
  oyFormsArgs_s * s = args;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 1 );

  error = oyOptions_SetFromData( (oyOptions_s**)&args->data_, registration,
                                 resource, 0, OY_CREATE_NEW );

  return error;
}


/** Function oyFormsArgs_ResourceGet
 *  @memberof oyFormsArgs_s
 *  @brief   get a resource to be used inside the renderer
 *
 *  @param[in,out] args                argument object
 *  @param[in]     registration        a registration key name for the resource
 *  @param[out]    resource            the resource
 *  @return                            0 - good; 1 - error; -1 <= issue
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/07 (Oyranos: 0.1.10)
 *  @date    2009/11/07
 */
int          oyFormsArgs_ResourceGet ( oyFormsArgs_s     * args,
                                       const char        * name,
                                       oyPointer         * resource )
{
  int error = !args;
  oyFormsArgs_s * s = args;

  oyCheckType__m( oyOBJECT_FORMS_ARGS_S, return 1 );

  error = oyOptions_FindData( (oyOptions_s*)args->data_, name, resource, 0, 0 );

  return error;
}


/** Function oyFormsAddHeadline
 *  @brief   add an html:Hxxx to a existing oforms stream
 *
 *  @param[in,out] stream              the oforms stream
 *  @param[in]     type                the type of headline; typical 1...4
 *  @param[in]     value               the value for the html:hx headline
 *  @param[in]     allocateFunc        the stream allocator
 *  @param[in]     deAllocateFunc      the stream deallocator
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/01/09 (Oyranos: 0.1.10)
 *  @date    2010/01/09
 */
int          oyFormsAddHeadline      ( char             ** stream,
                                       int                 type,
                                       const char        * value,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc )
{
  char num[24];
  if(!value || 0 >= type || type > 4)
    return 1;

  sprintf( num, "%d", type );

  oyStringAdd_( stream, "  <h", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,  num, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   ">", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,    value, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,     "</h", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,      num, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,       ">\n", allocateFunc, deAllocateFunc );

  return 0;
}

/** Function oyFormsStartChoice
 *  @brief   start an xf:choice to a existing oforms stream
 *
 *  The function is to be followed xf:items entries.
 *  The stream should be finished by a closing xf:choice.
 *
 *  @param[in,out] stream              the oforms stream
 *  @param[in]     reference           the ref attribute for xf:choice
 *  @param[in]     label               the label for the xf:choice
 *  @param[in]     help                the help text for the xf:choice
 *  @param[in]     allocateFunc        the stream allocator
 *  @param[in]     deAllocateFunc      the stream deallocator
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/01/09 (Oyranos: 0.1.10)
 *  @date    2010/01/09
 */
int          oyFormsStartChoice      ( char             ** stream,
                                       const char        * reference,
                                       const char        * label,
                                       const char        * help,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc )
{
  oyStringAdd_( stream, "     <xf:select1", allocateFunc, deAllocateFunc );
  if(reference)
  {
  oyStringAdd_( stream,  " ref=\"/", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   reference, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,    "\"", allocateFunc, deAllocateFunc );
  }
  oyStringAdd_( stream,     ">\n", allocateFunc, deAllocateFunc );
  if(label)
  {
  oyStringAdd_( stream, "      <xf:label>", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,  label, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   "</xf:label>\n", allocateFunc, deAllocateFunc );
  }
  if(help)
  {
  oyStringAdd_( stream, "      <xf:help>", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,  help, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   "</xf:help>\n", allocateFunc, deAllocateFunc );
  }
  oyStringAdd_( stream, "      <xf:choices>\n", allocateFunc, deAllocateFunc );

  return 0;
}

/** Function oyFormsAddItem
 *  @brief   add an xf:item to a existing oforms stream
 *
 *  The funtction is to be called after a xf:choices or other xf:items.
 *  The stream should be finished by other xf:items or a closing xf:choice.
 *
 *  @param[in,out] stream              the oforms stream
 *  @param[in]     value               the value for the xf:item
 *  @param[in]     label               the label for the xf:item
 *  @param[in]     allocateFunc        the stream allocator
 *  @param[in]     deAllocateFunc      the stream deallocator
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/01/09 (Oyranos: 0.1.10)
 *  @date    2010/01/09
 */
int          oyFormsAddItem          ( char             ** stream,
                                       const char        * value,
                                       const char        * label,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc )
{
  if(!value && !label)
    return 1;

  oyStringAdd_( stream, "       <xf:item>\n", allocateFunc, deAllocateFunc );
  if(value)
  {
  oyStringAdd_( stream, "        <xf:value>", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,  value, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   "</xf:value>\n", allocateFunc, deAllocateFunc );
  }
  if(label)
  {
  oyStringAdd_( stream, "        <xf:label>", allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,  label, allocateFunc, deAllocateFunc );
  oyStringAdd_( stream,   "</xf:label>\n", allocateFunc, deAllocateFunc );
  }
  oyStringAdd_( stream, "       </xf:item>\n", allocateFunc, deAllocateFunc );

  return 0;
}


