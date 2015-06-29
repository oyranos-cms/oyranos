/** @file oyranos_forms.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    common Oyranos forms declarations
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2009/10/13
 */

#ifndef OYRANOS_FORMS_H
#define OYRANOS_FORMS_H


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct oyFormsArgs_s oyFormsArgs_s;

/** @struct  oyFormsArgs_s
 *  @brief   a FormsArgs object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2010/11/02
 */
struct oyFormsArgs_s {
  int              type_;
  oyFormsArgs_s *  (*copy) (oyFormsArgs_s*, void*);
  int              (*release) (oyFormsArgs_s**);
  void           * oy_;

  /** 0 means keep silent,
   *  - 0x01 print options and names
   *  - 0x02 print as well help texts
   *  - 0x04 print all available options, can be rather long
   *  A renderer has no obligation to follow iall hints. E.g.
   *  flags 0x02 and 0x04 have less meaning for a GUI.
   */
  int              print;
  void           * xforms_data_model_;
  void           * data_;
};

oyFormsArgs_s *  oyFormsArgs_New     ( void              * private_object_ );
oyFormsArgs_s *  oyFormsArgs_Copy    ( oyFormsArgs_s     * args,
                                       void              * private_object_ );
int          oyFormsArgs_Release     ( oyFormsArgs_s    ** args );

const char * oyFormsArgs_ModelGet    ( oyFormsArgs_s     * args );
int          oyFormsArgs_ResourceSet ( oyFormsArgs_s     * args,
                                       const char        * name,
                                       oyPointer           resource );
int          oyFormsArgs_ResourceGet ( oyFormsArgs_s     * args,
                                       const char        * name,
                                       oyPointer         * resource );

#include "oyranos_alpha.h"
extern oyUiHandler_s * oy_ui_cmd_line_handlers[];

#define OYFORMS_FLTK_HELP_VIEW_REG  OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD \
                               OY_SLASH "forms/fltk/help_view.oyCallback_s"
typedef void (*oyFormsFltkHelpViewCallback_f) (
                                       oyPointer           ptr,
                                       const char        * help_text );

int          oyFormsAddHeadline      ( char             ** stream,
                                       int                 type,
                                       const char        * value,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc );
int          oyFormsAddItem          ( char             ** stream,
                                       const char        * value,
                                       const char        * label,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc );
int          oyFormsStartChoice      ( char             ** stream,
                                       const char        * reference,
                                       const char        * label,
                                       const char        * choice,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_FORMS_H */
