/** @file oyranos_forms.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    common Oyranos forms declarations
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/10/13
 */

#ifndef OYRANOS_FORMS_H
#define OYRANOS_FORMS_H


#ifdef __cplusplus
namespace oyranos {
namespace forms {
extern "C" {
#endif /* __cplusplus */

typedef struct oyFormsArgs_s oyFormsArgs_s;

/** @struct  oyFormsArgs_s
 *  @brief   a FormsArgs object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/10/12 (Oyranos: 0.1.10)
 *  @date    2009/10/12
 */
struct oyFormsArgs_s {
  int              type_;
  oyFormsArgs_s *  (*copy) (oyFormsArgs_s*, void*);
  int              (*release) (oyFormsArgs_s**);
  void           * oy_;

  int              silent;
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

extern oyUiHandler_s * oy_ui_cmd_line_handlers[];

typedef struct oyFormsCallback_s oyFormsCallback_s;
struct oyFormsCallback_s {
  int              type_;
  void           * dummy1;
  void           * dummy2;
  void           * dummy3;

  void           (*callback)();
  oyPointer        data;
};
#define OYFORMS_FLTK_HELP_VIEW_REG  OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD \
                                  OY_SLASH "forms/fltk/help_view.Fl_Text_Buffer"
typedef void (*oyFormsFltkHelpViewCallback_f) (
                                       oyFormsArgs_s     * forms_args,
                                       const char        * help_text );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace forms */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_FORMS_H */
