{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

/** @enum    oyDATATYPE_e
 *  @brief   basic data types for anonymous pointers
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.8)
 *  @date    2007/00/00
 */
typedef enum oyDATATYPE_e {
  oyUINT8,     /*!<  8-bit integer */
  oyUINT16,    /*!< 16-bit integer */
  oyUINT32,    /*!< 32-bit integer */
  oyHALF,      /*!< 16-bit floating point number */
  oyFLOAT,     /*!< IEEE floating point number */
  oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE_e;

/** @enum    oyCHANNELTYPE_e
 *  @brief   channels types
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2009/08/03
 */
typedef enum oyCHANNELTYPE_e {
  oyCHANNELTYPE_UNDEFINED,            /**< as well for the list end */
  oyCHANNELTYPE_OTHER,
  oyCHANNELTYPE_LIGHTNESS,
  oyCHANNELTYPE_COLOUR,
  oyCHANNELTYPE_COLOUR_LIGHTNESS,
  oyCHANNELTYPE_COLOUR_LIGHTNESS_PREMULTIPLIED,
  oyCHANNELTYPE_DEPTH,
  oyCHANNELTYPE_ALPHA1,
  oyCHANNELTYPE_ALPHA2,
  oyCHANNELTYPE_ALPHA3,
  oyCHANNELTYPE_U,
  oyCHANNELTYPE_V
} oyCHANNELTYPE_e;

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
