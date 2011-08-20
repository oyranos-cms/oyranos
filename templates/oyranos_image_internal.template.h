{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include "oyranos_image.h"


/** @internal
 *  pixel mask description
 */
enum {
  oyLAYOUT = 0, /* remembering the layout */
  oyPOFF_X,     /* pixel count x offset */
  oyPOFF_Y,     /* pixel count y offset */
  oyCOFF,       /* channel offset */
  oyDATA_SIZE,  /* sample size in byte */
  oyCHANS,      /* number of channels */
  oyCHAN0       /* first colour channel */
};


{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
