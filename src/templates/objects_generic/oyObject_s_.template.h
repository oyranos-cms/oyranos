{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include <oyranos_object.h>
#include <oyName_s_.h>

#include "{{ class.name }}.h"

{% block doxygenPublicClass %}
/* Include "{{ class.dox }}" { */
{% include class.dox %}
/* } Include "{{ class.dox }}" */
{% endblock %}
struct oyObject_s_ {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_OBJECT_S*/
  oyObject_s         (*copy) (oyObject_s object); /**< copy function */
  int                (*release) (oyObject_s * object); /**< release function */
  int                  id_;            /**< @private identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< @private data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< @private data release function */
  oyStruct_s         * parent_;        /*!< @private parent struct of parent_type */
  /** @private The first number in the array means the number of inherited 
   *  classes. It is the index to access top class. 
   *  parent_types_[parent_types_[0]] == actual_class */
  oyOBJECT_e         * parent_types_;  /**< @private the first number in the array means the number of inherited classes. */
  oyPointer            backdoor_;      /*!< @private allow non breaking extensions */
  oyOptions_s        * handles_;       /**< @private addational data and infos*/
  oyName_s           * name_;          /*!< @private naming feature */
  int                  ref_;           /*!< @private reference counter */
  int                  version_;       /*!< @private OYRANOS_VERSION */
  unsigned char      * hash_ptr_;      /**< @private 2*OY_HASH_SIZE */
  oyPointer            lock_;          /**< @private the user provided lock */
};

{% block SpecificPrivateMethodsDeclarations %}
/* Include "{{ class.private_methods_declarations_h }}" { */
{% include class.private_methods_declarations_h %}
/* } Include "{{ class.private_methods_declarations_h }}" */
{% endblock %}

{% include "cpp_end.h" %}
#endif /* {{ file_name|underscores|upper|tr:". _" }} */
