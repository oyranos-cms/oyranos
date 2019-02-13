/** Function    oyCMMapi10_Create
 *  @memberof   oyCMMapi10_s
 *  @brief      Custom CMMapi10 constructor
 *
 *  @param         init                custom initialisation
 *  @param         msg_set             message function setter
 *  @param         registration        the modules @ref registration string,
 *  it shall contain the command name, so it can be selected
 *  @param         version             module version
 *  - 0: major - should be stable for the live time of a filter
 *  - 1: minor - mark new features
 *  - 2: patch version - correct errors
 *  @param         module_api          tell compatibility with Oyranos API
 *  - 0: last major Oyranos version during development time, e.g. 0
 *  - 1: last minor Oyranos version during development time, e.g. 9
 *  - 2: last Oyranos patch version during development time, e.g. 5
 *  @param         getText             for filter help descriptions;
 *  It shall at least contain "can_handle", "help" and the command name as
 *  used in the registration string
 *  @param         texts               types for getText member
 *  @param         oyMOptions_Handle   the function pointer to do the work
 *  @param         object              a optional object
 *
 *  @version Oyranos: 0.9.7
 *  @since   2019/02/07 (Oyranos: 0.9.7)
 *  @date    2019/02/07
 */
OYAPI oyCMMapi10_s*  OYEXPORT
                   oyCMMapi10_Create ( oyCMMInit_f         init,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       oyCMMGetText_f      getText,
                                       const char       ** texts,
                                       oyMOptions_Handle_f oyMOptions_Handle,
                                       oyObject_s          object )
{
  oyCMMapi10_s_ * api10 = (oyCMMapi10_s_*) oyCMMapi10_New( object ); 
  if(!api10)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  oyCMMapi_Set( (oyCMMapi_s*) api10, init, msg_set, registration,
                version, module_api );

  api10->getText = getText;
  {
    int n = 0, i;
    char ** list;
    while(texts && texts[n]) ++n;
    list = oyAllocateFunc_(sizeof(char*)*(n+1));
    for(i = 0; i < n; ++i)
      list[i] = oyjlStringCopy(texts[i], oyAllocateFunc_);
    api10->texts = list;
  }
  api10->oyMOptions_Handle = oyMOptions_Handle;

  return (oyCMMapi10_s*) api10;
}

