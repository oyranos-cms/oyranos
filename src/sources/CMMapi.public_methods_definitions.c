/** Function    oyCMMapi_Set
 *  @memberof   oyCMMapi_s
 *  @brief      CMMapi member setter
 *
 *  @param         api                 the struct
 *  @param         init                custom initialisation
 *  @param         reset               custom deinitialisation
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
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/09/03
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 */
void               oyCMMapi_Set      ( oyCMMapi_s        * api,
                                       oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3] )
{
  oyCMMapi_s_ * s = (oyCMMapi_s_*) api;
  int i;

  s->oyCMMInit = init;
  s->oyCMMReset = reset;
  s->oyCMMMessageFuncSet = msg_set;
  s->registration = registration;
  for(i = 0; i < 3; i++)
  {
    if(version)
      s->version[i] = version[i];
    if(module_api)
      s->module_api[i] = module_api[i];
  }
}
/** Function    oyCMMapi_SetNext
 *  @memberof   oyCMMapi_s
 *  @brief      chain CMMapi's
 *
 *  A oyCMMapi_s is initially not chained and will thus not be detected during
 *  parsing the oyCMMinfo_s structure.
 *  Append a CMMapi by searching for a empty place through
 *  oyCMMapi_s::oyCMMapi_GetNext ( api ) == NULL . Then append the new CMMapi with
 *  oyCMMapi_s::oyCMMapi_SetNext ( old_api, my_new_api ). 
 *  @see oyCMMinfo_SetApi for setting the first visible api.
 *
 *  @param         api                 the struct
 *  @param         next                struct to chain
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 *  @date    2013/06/10
 */
void               oyCMMapi_SetNext  ( oyCMMapi_s        * api,
                                       oyCMMapi_s        * next )
{
  ((oyCMMapi_s_*)api)->next = next;
}

/** Function    oyCMMapi_GetNext
 *  @memberof   oyCMMapi_s
 *  @brief      get next CMMapi
 *
 *  oyCMMapi_s forms a single linked list. The start must be set in oyCMMinfo_s
 *  to be parsed by the module loader.
 *  @see oyCMMapi_SetNext oyCMMinfo_SetApi
 *
 *  @param         api                 the struct
 *  @return                            the next struct
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/10 (Oyranos: 0.9.5)
 *  @date    2013/06/10
 */
oyCMMapi_s *       oyCMMapi_GetNext  ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->next;
}
oyCMMInit_f        oyCMMapi_GetInitF ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->oyCMMInit;
}
oyCMMReset_f       oyCMMapi_GetResetF( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->oyCMMReset;
}
oyCMMMessageFuncSet_f
                   oyCMMapi_GetMessageFuncSetF
                                     ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->oyCMMMessageFuncSet;
}
const char *       oyCMMapi_GetRegistration
                                     ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->registration;
}
