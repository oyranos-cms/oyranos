#include "oyTest.h"

{% for class in classes %}
  {% if class.hiddenStruct %}
#include "{{ class.name }}.h"
#include "{{ class.privName }}.h"
  {% else %}
#include "{{ class.name }}.h"
#include "{{ class.name }}_impl.h"
  {% endif %}
{% endfor %}

{% for class in classes %}
void oyTest::{{ class.name }}_generic_methods()
{
  {{ class.name }} *{{ class.baseName|lower }} = NULL;
  {{ class.baseName|lower }} = oy{{ class.baseName }}_New(0);

  QVERIFY( {{ class.baseName|lower }} != NULL );

  {{ class.name }} *{{ class.baseName|lower }}_copy = NULL;
  {{ class.baseName|lower }}_copy = oy{{ class.baseName }}_Copy({{ class.baseName|lower }}, 0);

  QVERIFY( {{ class.baseName|lower }} == {{ class.baseName|lower }}_copy );

  oyObject_s obj = oyObject_New();
  {{ class.name }} *{{ class.baseName|lower }}_new = NULL;
  {{ class.baseName|lower }}_new = oy{{ class.baseName }}_Copy({{ class.baseName|lower }}, obj);

  QVERIFY( {{ class.baseName|lower }} != {{ class.baseName|lower }}_new );

  oy{{ class.baseName }}_Release( &{{ class.baseName|lower }} );
  oy{{ class.baseName }}_Release( &{{ class.baseName|lower }}_copy );
  oy{{ class.baseName }}_Release( &{{ class.baseName|lower }}_new );
}

{% if class.hiddenStruct %}
void oyTest::{{ class.privName }}_generic_methods()
{% else %}
void oyTest::{{ class.name }}_impl_generic_methods()
{% endif %}
{

}
{% endfor %}
