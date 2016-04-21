/** Function    oyCMMapi4_Create
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor
 *
 *  @param         init                custom initialisation
 *  @param         msg_set             message function setter
 *  @param         registration        the modules @ref registration string,
 *  @param         version             module version
 *  - 0: major - should be stable for the live time of a filter
 *  - 1: minor - mark new features
 *  - 2: patch version - correct errors
 *  @param         module_api          tell compatibility with Oyranos API
 *  - 0: last major Oyranos version during development time, e.g. 0
 *  - 1: last minor Oyranos version during development time, e.g. 9
 *  - 2: last Oyranos patch version during development time, e.g. 5
 *  @param         context_type        tell the context type, if any
 *  the data type of the context returned by oyCMMapi4_s::oyCMMFilterNode_ContextToMem_f, mandatory in case of a set oyCMMapi4_s::oyCMMFilterNode_ContextToMem
 *  e.g. oyCOLOR_ICC_DEVICE_LINK / "oyDL"
 *  @param         contextToMem 
 *  only mandatory for context producing filters, e.g. "//color/icc"
 *  @param         getText             optionally set a 
 *                                     oyCMMFilterNode_ContextToMem,
 *  used to override a Oyranos side hash creation
 *  @param         ui                  a UI description
 *  provide a oyCMMapi4_s::ui->getText( select, type ) call. The "select"
 *  argument shall cover at least "name" and "help"
 *  @param         object              a optional object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
OYAPI oyCMMapi4_s*  OYEXPORT
                   oyCMMapi4_Create  ( oyCMMInit_f         init,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterNode_ContextToMem_f contextToMem,
                                       oyCMMFilterNode_GetText_f getText,
                                       oyCMMui_s         * ui,
                                       oyObject_s          object )
{
  oyCMMapi4_s_ * api4 = (oyCMMapi4_s_*) oyCMMapi4_New( object ); 
  if(!api4)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  oyCMMapi_Set( (oyCMMapi_s*) api4, init, msg_set, registration,
                version, module_api );

  if(context_type)
    memcpy( api4->context_type, context_type, 8 );
  api4->oyCMMFilterNode_ContextToMem = contextToMem;
  api4->oyCMMFilterNode_GetText = getText;
  api4->ui = (oyCMMui_s_*) oyCMMui_Copy( ui, object );
  if(api4->ui)
  {
    api4->ui->parent = (oyCMMapiFilter_s*) oyCMMapi4_Copy( (oyCMMapi4_s*) api4, NULL );
  }

  return (oyCMMapi4_s*) api4;
}

/** Function    oyCMMapi4_GetUi
 *  @memberof   oyCMMapi4_s
 *  @brief      Get ui object from module
 *
 *  @param         obj                 the api4 module
 *  @return                            a ui object
 *
 *  @version Oyranos: 0.9.6
 *  @since   2014/06/10 (Oyranos: 0.9.4)
 *  @date    2014/06/10
 */
OYAPI oyCMMui_s*  OYEXPORT
                   oyCMMapi4_GetUi   ( oyCMMapi4_s       * obj )
{
  oyCMMapi4_s_ * api4 = (oyCMMapi4_s_*) obj; 
  if(!api4)
    return NULL;

  return (oyCMMui_s*) api4->ui;
}

