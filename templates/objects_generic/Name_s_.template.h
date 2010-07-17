{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S__H
#define OY_{{ class.baseName|upper }}_S__H

#include <oyranos_object.h>

#include "{{ class.name }}.h"

int          oyName_release_         ( oyName_s         ** name,
                                       oyDeAlloc_f         deallocateFunc );

int          oyName_copy_            ( oyName_s          * dest,
                                       oyName_s          * src,
                                       oyObject_s          object );
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
const char * oyName_get_             ( const oyName_s    * obj,
                                       oyNAME_e            type );
int          oyName_releaseMembers   ( oyName_s          * obj,
                                       oyDeAlloc_f         deallocateFunc );

#endif /* OY_{{ class.baseName|upper }}_S__H */
