
oyCMMGetText_f     oyCMMui_GetTextF  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->getText;
}
/** Function  oyCMMui_GetTexts
 *  @memberof oyCMMui_s
 *  @brief    Get texts for GetTextF
 *
 *  @param[in]     node                ui object
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
const char *       oyCMMui_GetTexts  ( oyCMMui_s         * ui )
{
  return ((oyCMMui_s_*)ui)->texts;
}
