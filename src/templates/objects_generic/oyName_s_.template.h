{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

#include <oyranos_object.h>

#include "{{ class.name }}.h"

#include "oyStruct_s.h"


{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct {{ class.name }} {
  oyOBJECT_e           type;          /*!< internal struct type oyOBJECT_NAME_S */
  oyStruct_Copy_f      copy;          /**< copy function */
  oyStruct_Release_f   release;       /**< release function */
  oyPointer        dummy;             /**< keep to zero */
  char               * nick;          /*!< few letters for mass representation, eg. "A1" */
  char               * name;          /*!< normal visible name, eg. "A1-MySys"*/
  char               * description;   /*!< full user description, eg. "A1-MySys from Oyranos" */
  char                 lang[8];       /**< i18n language, eg. "en_GB" */
};

{% block GeneralPublicMethodsDeclarations %}
/* {{ class.name }} common object functions { */
oyName_s *   oyName_newWith          ( oyAlloc_f           allocateFunc );
oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
/* } {{ class.name }} common object functions */
{% endblock %}

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

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
