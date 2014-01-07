/** Function  oyCMMui_Create
 *  @memberof oyCMMui_s
 *  @brief    Create a oyCMMui_s object
 *
 *  @param         category            menu structure
 *  @param         getText             translated UI texts
 *  @param         texts
 *  zero terminated list of string classes for getText,
 *  e.g. {"name","category","help",0}
 *  - The "name" texts shall include information about the module.\n
 *  The oyNAME_NICK for the several oyCMMui_s::getText() texts is typical not
 *  translated. For "name" the oyNAME_NICK should be the module name. 
 *  - The "category" gives in oyNAME_NAME a translated version of
 *  oyCMMui_s::category.
 *  - The "help" texts should provide general infromations about the module.\n
 *  For other texts like "help" and "category" oyNAME_NICK makes no sense.
 *  @param         object              optional object
 *  @return                            the struct
 *
 *  @see oyCMMui_GetTextF() oyCMMui_GetTexts()
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/11
 *  @since    2013/06/11 (Oyranos: 0.9.5)
 */
OYAPI oyCMMui_s *  OYEXPORT
                   oyCMMui_Create    ( const char        * category,
                                       oyCMMGetText_f      getText,
                                       const char       ** texts,
                                       oyObject_s          object )
{
  oyCMMui_s_ * s = NULL;

  if(!category) WARNc_S("Filter category is missed! Please add.")
  if(!getText) WARNc_S("Filter texts are missed! Please add name, help and category texts.")
  if(!texts) WARNc_S("Filter text_classes are missed! Please add them to make getText useable")
  if(!category || !getText || !texts)
    return (oyCMMui_s*)s;

  s = (oyCMMui_s_*) oyCMMui_New( object );
  if(!s) return (oyCMMui_s*)s;

  s->category = category;
  s->getText = getText;
  s->texts = texts;

  return (oyCMMui_s*) s;
}

/** Function  oyCMMui_SetUiOptions
 *  @memberof oyCMMui_s
 *  @brief    Set UI for possible options
 *
 *  @param[in]     ui                  ui object
 *  @param         options             the default options as oFORMS model
 *                                     compatible XML
 *  @param         oyCMMuiGet          the oFORMS UI for filter options
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/13
 *  @since    2013/06/13 (Oyranos: 0.9.5)
 */
OYAPI void  OYEXPORT
                   oyCMMui_SetUiOptions(
                                       oyCMMui_s         * ui,
                                       const char        * options,
                                       oyCMMuiGet_f        oyCMMuiGet )
{
  oyCMMui_s_ * s = (oyCMMui_s_*) ui;

  oyCheckType__m( oyOBJECT_CMM_UI_S, return )

  s->options = options;
  s->oyCMMuiGet = oyCMMuiGet;
}

/** Function  oyCMMui_GetTextF
 *  @memberof oyCMMui_s
 *  @brief    Get the GetText function
 *
 *  @param[in]     ui                  ui object
 *  @return                            the oyCMMGetText_f function
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.5.0)
 */
oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->getText;
}
/** Function  oyCMMui_GetTexts
 *  @memberof oyCMMui_s
 *  @brief    Get texts for GetTextF
 *
 *  @param[in]     ui                  ui object
 *  @return                            the zero terminated string list
 *
 *  zero terminated categories for getText,
 *  e.g. {"name","category","help",0}
 *
 *  The "name" texts shall include information about the module.
 *  The "category" gives in oyNAME_NAME a translated version of
 *  oyCMMui_s::category.
 *  The "help" texts should provide general infromations about the module.
 *
 *  The oyNAME_NICK for the several oyCMMui_s::getText() texts is typical not
 *  translated. For "name" the oyNAME_NICK should be the module name. For 
 *  other texts like "help" and "category" oyNAME_NICK makes no sense.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/15
 *  @since    2012/09/15 (Oyranos: 0.5.0)
 */
const char **      oyCMMui_GetTexts  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->texts;
}

/** @memberof oyCMMui_s
 *  @brief    Get the parent structure
 *
 *  @param[in]     ui                  ui object
 *  @return                            the parent object
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2014/01/07
 *  @since    2014/01/07 (Oyranos: 0.9.5)
 */
oyCMMapiFilter_s * oyCMMui_GetParent ( oyCMMui_s         * ui )
{
  oyCMMapiFilter_s * p = ((oyCMMui_s_*)ui)->parent;
  if(p && p->copy)
    return (oyCMMapiFilter_s*) p->copy( (oyStruct_s*) p, NULL);
  else
   return p;
}
