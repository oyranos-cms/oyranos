{% load gsoc_extensions %}{% include "source_file_header.txt" %}

#ifndef {{ file_name|cut:".h"|upper }}_H
#define {{ file_name|cut:".h"|upper }}_H


{% include "cpp_begin.h" %}

#include "oyOptions_s.h"
#include "oyStructList_s.h"

/** @struct  oyObjectInfo_s
 *  @brief   a static object
 *  @extends oyStruct_s
 *
 *  The data field is to be placed alternatively to a oyObject_s at
 *  oyStruct_s::oy_ for a static object. It provides basic object data.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/02 (Oyranos: 0.3.0)
 *  @date    2011/02/02
 */
typedef struct oyObjectInfoStatic_s {
  oyOBJECT_e       type;               /**< oyOBJECT_INFO_STATIC_S */
  oyPointer        dummy1;             /**< keep zero */
  oyPointer        dummy2;             /**< keep zero */
  oyPointer        dummy3;             /**< keep zero */
  /** inherited structures
   *  start with the position of the last member, followed by the first
   *  object type until the last one. Seven levels of inheritance are possible
   *  at maximum.
   */
  oyOBJECT_e       inheritance[8];
} oyObjectInfoStatic_s;

extern oyObjectInfoStatic_s oy_connector_imaging_static_object;


const char *   oyContextCollectData_ ( oyStruct_s        * s,
                                       oyOptions_s       * opts,
                                       oyStructList_s    * ins,
                                       oyStructList_s    * outs );

{% include "cpp_end.h" %}

#endif /* {{ file_name|cut:".h"|upper }}_H */
