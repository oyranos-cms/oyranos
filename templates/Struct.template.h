#ifndef OY_{{ class_name|upper }}_H
#define OY_{{ class_name|upper }}_H

typedef struct {
   {% block BaseMembers %}
   /* Struct base class start */
   oyOBJECT_e           type_;          /**< struct type oyOBJECT_{{ class_name|upper  }}_S */ 
   oyStruct_Copy_f      copy;           /**< copy function */
   oyStruct_Release_f   release;        /**< release function */
   oyObject_s           oy_;            /**< base object */
   /* Struct base class stop */
   {% endblock %}
   {% block ChildMembers %}{% endblock %}
} {% block ClassName %}oy{{ class_name }}_s{% endblock %}

#endif /* OY_{{ class_name|upper }}_H */
