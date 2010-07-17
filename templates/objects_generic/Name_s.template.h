{% include "source_file_header.txt" %}

#ifndef OY_{{ class.baseName|upper }}_S_H
#define OY_{{ class.baseName|upper }}_S_H

{% include "cpp_begin.h" %}

#include <oyranos_object.h>

#include "oyStruct_s.h"

typedef struct {{ class.name }} {{ class.name }};

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
oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
/* } {{ class.name }} common object functions */
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.baseName|upper }}_S_H */
