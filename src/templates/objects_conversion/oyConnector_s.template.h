{% extends "Base_s.h" %}

{% block LocalIncludeFiles %}
{{ block.super }}
#include "oyConnector_s.h"
{% endblock %}

{% block SpecificPublicMethodsDeclarations %}
{{ block.super }}
OYAPI int  OYEXPORT
                 oy{{ class.baseName }}_SetTexts( {{ class.name }}     * obj,
                                       oyCMMGetText_f      getText,
                                       const char       ** text_classes );
OYAPI const char **  OYEXPORT
                 oy{{ class.baseName }}_GetTexts( {{ class.name }}     * obj );
OYAPI const char *  OYEXPORT
                 oy{{ class.baseName }}_GetText ( {{ class.name }}     * obj,
                                       const char        * name_class,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oy{{ class.baseName }}_IsPlug  ( {{ class.name }}     * obj );
OYAPI int  OYEXPORT
                 oy{{ class.baseName }}_SetIsPlug({{ class.name }}     * obj,
                                       int                 is_plug );
OYAPI const char *  OYEXPORT
                oy{{ class.baseName }}_GetReg  ( {{ class.name }}     * obj );
OYAPI int  OYEXPORT
                 oy{{ class.baseName }}_SetReg  ( {{ class.name }}     * obj,
                                       const char        * type_registration );

OYAPI int  OYEXPORT
                 oy{{ class.baseName }}_SetMatch( {{ class.name }}     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func );
OYAPI oyCMMFilterSocket_MatchPlug_f  OYEXPORT
                 oy{{ class.baseName }}_GetMatch (
                                       {{ class.name }}     * obj );
{% endblock %}

