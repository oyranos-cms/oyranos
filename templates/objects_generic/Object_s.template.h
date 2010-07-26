{% include "source_file_header.txt" %}

#ifndef OY_{{ class.underscoreName|upper }}_S_H
#define OY_{{ class.underscoreName|upper }}_S_H

{% include "cpp_begin.h" %}

#define OY_HASH_SIZE 16

#include <oyranos_object.h>

#include "oyStruct_s.h"
#include "oyOptions_s.h"
#include "oyName_s.h"

{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct oyObject_s_ {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_OBJECT_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  int                  id_;            /**< @private identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< @private data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< @private data release function */
  oyPointer            parent_;        /*!< @private parent struct of parent_type */
  oyOBJECT_e           parent_type_;   /*!< @private parents struct type */
  oyPointer            backdoor_;      /*!< @private allow non breaking extensions */
  oyOptions_s        * handles_;       /**< @private addational data and infos*/
  oyName_s           * name_;          /*!< @private naming feature */
  int                  ref_;           /*!< @private reference counter */
  int                  version_;       /*!< @private OYRANOS_VERSION */
  unsigned char      * hash_ptr_;      /**< @private 2*OY_HASH_SIZE */
  oyPointer            lock_;          /**< @private the user provided lock */
};

{% block GeneralPublicMethodsDeclarations %}
/* oyObject_s common object functions { */
oyObject_s   oyObject_New             ( void );
oyObject_s   oyObject_NewWithAllocators(oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
oyObject_s   oyObject_NewFrom         ( oyObject_s        object );
oyObject_s   oyObject_Copy            ( oyObject_s        object );
int          oyObject_Release         ( oyObject_s      * oy );
/* } oyObject_s common object functions */
{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
/* Include "{{ class.public_methods_declarations_h }}" { */
{% include class.public_methods_declarations_h %}
/* } Include "{{ class.public_methods_declarations_h }}" */
{% endblock %}

{% include "cpp_end.h" %}
#endif /* OY_{{ class.underscoreName|upper }}_S_H */
