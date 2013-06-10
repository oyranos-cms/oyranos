/** Function    oyCMMapi4_Set
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
OYAPI oyCMMapi4_s*  OYEXPORT
                   oyCMMapi4_Create  ( const char        * context_type,
                                       oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem,
                                       oyCMMFilterNode_GetText_f oyCMMFilterNode_GetText,
                                       oyCMMui_s         * ui,
                                       oyObject_s          object )
{
  oyCMMapi4_s_ * api4 = (oyCMMapi4_s_*) oyCMMapi4_New( object ); 
  if(!api4)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  memcpy( api4->context_type, context_type, 8 );
  api4->oyCMMFilterNode_ContextToMem = oyCMMFilterNode_ContextToMem;
  api4->oyCMMFilterNode_GetText = oyCMMFilterNode_GetText;
  api4->ui = (oyCMMui_s_*) oyCMMui_Copy( ui, object );

  return (oyCMMapi4_s*) api4;
}

