{% include "source_file_header.txt" %}

#include "oyranos_object_internal.h"

#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"

#include "oyObject_s.h"
#include "oyOption_s_.h"
#include "oyOptions_s_.h"
#include "oyStructList_s_.h" /* not nice but probably faster list access */


/* {{ class.name }} common object functions { */
/** Function oyObserver_New
 *  @memberof oyObserver_s
 *  @brief   allocate a new Observer object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI oyObserver_s * OYEXPORT
           oyObserver_New            ( oyObject_s          object OY_UNUSED )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OBSERVER_S;
# define STRUCT_TYPE oyObserver_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  s = (STRUCT_TYPE*)oyAllocateFunc_(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc_S("Problem with memset.");

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyObserver_Copy;
  s->release = (oyStruct_Release_f) oyObserver_Release;
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** Function oyObserver_Copy
 *  @memberof oyObserver_s
 *  @brief   copy a Observer object
 *
 *  A reference is not possible as no reference counter is available.
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI oyObserver_s * OYEXPORT
           oyObserver_Copy           ( oyObserver_s      * obj,
                                       oyObject_s          object )
{
  oyObserver_s * s = obj;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 0 )

  s = oyObserver_Copy_( obj, object );

  return s;
}
 
/** Function oyObserver_Release
 *  @memberof oyObserver_s
 *  @brief   release and possibly deallocate a Observer object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/26 (Oyranos: 0.1.10)
 *  @date    2009/10/26
 */
OYAPI int  OYEXPORT
           oyObserver_Release        ( oyObserver_s     ** obj )
{
  /* ---- start of common object destructor ----- */
  oyObserver_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_OBSERVER_S, return 1 )

  *obj = 0;

  /* ---- end of common object destructor ------- */
  if(s->observer)
  { s->observer->release( &s->observer ); s->observer = 0; }
  if(s->model)
  { s->model->release( &s->model ); s->model = 0; }
  if(s->user_data)
  { s->user_data->release( &s->user_data ); s->user_data = 0; }

  {
    oyDeAlloc_f deallocateFunc = oyDeAllocateFunc_;

    deallocateFunc( s );
  }

  return 0;
}
/* } {{ class.name }} common object functions */

/* Include "{{ class.public_methods_definitions_c }}" { */
{% include class.public_methods_definitions_c %}
/* } Include "{{ class.public_methods_definitions_c }}" */
