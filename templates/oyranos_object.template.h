{% include "source_file_header.txt" %}

#ifndef OYRANOS_OBJECT_H
#define OYRANOS_OBJECT_H

{% include "cpp_begin.h" %}

#include <oyranos.h>

/* Function Pointers Definitions Start */
typedef oyPointer (*oyPointer_copy_f )   ( oyPointer, size_t );
typedef int       (*oyPointer_release_f )( oyPointer * );
typedef void      (*oyLockRelease_f )( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyLock_f )       ( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyUnLock_f )     ( oyPointer           look,
                                       const char        * marker,
                                       int                 line );
/* Function Pointers Definitions Stop */



/**
 *  @brief   Oyranos structure type
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/22 (Oyranos: 0.1.x)
 *  @date    2008/07/31
 */
typedef enum {
  oyOBJECT_NONE,
{% for class in classes %}
  oyOBJECT_{{ class.baseName|upper }}_S,   /**< {{ class.name }} */
{% endfor %}
  oyOBJECT_MAX
} oyOBJECT_e;

/** @enum    oyNAME_e
 *  @brief   describe the base types of a oyObject_s name
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 */
typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

{% include "cpp_end.h" %}

#endif /* OYRANOS_OBJECT_H */
