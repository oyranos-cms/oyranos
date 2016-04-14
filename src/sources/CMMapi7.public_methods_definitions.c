/** Function  oyCMMapi7_Create
 *  @memberof oyCMMapi7_s
 *  @brief    Create a oyCMMapi7_s filter object
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
 *  @param         context_type        four byte identifier if a context is needed
 *                                     A additional oyCMMapi6_s module can, 
 *                                     then be provided to convert between
 *                                     context formats, registred by this
 *                                     identifier.
 *  @param         run                 the filter function
 *  @param         plugs               a zero terminated list of connectors
 *  @param         plugs_n             the number of plugs in 'plugs'
 *  @param         plugs_last_add      the number of accepted repeats of the
 *                                     last plug
 *  @param         sockets             a zero terminated list of connectors
 *  @param         sockets_n           the number of sockets in 'sockets'
 *  @param         sockets_last_add    the number of accepted repeats of the
 *                                     last socket
 *  @param         properties
 *  A zero terminated list of strings. The semantics are plugin defined.
 *  e.g.: {"key1=value1","key2=value2",0}
 *  @param         object              Oyranos object (optional)
 *
 *  @see     oyCMMapi_s::oyCMMapi_Set()
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/07/11 (Oyranos: 0.9.5)
 *  @date    2013/07/11
 */
OYAPI oyCMMapi7_s *  OYEXPORT
             oyCMMapi7_Create        ( oyCMMInit_f         init,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterPlug_Run_f run,
                                       oyConnector_s    ** plugs,
                                       uint32_t            plugs_n,
                                       uint32_t            plugs_last_add,
                                       oyConnector_s    ** sockets,
                                       uint32_t            sockets_n,
                                       uint32_t            sockets_last_add,
                                       const char       ** properties,
                                       oyObject_s          object )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*) oyCMMapi7_New( object );
  int n = 0;

  if(!s) return NULL;

  oyCMMapi_Set( (oyCMMapi_s*) s, init, msg_set, registration,
                version, module_api );

  s->oyCMMFilterPlug_Run = run;
  if(context_type)
    memcpy( s->context_type, context_type, 4 );
  s->plugs = plugs;
  s->plugs_n = plugs_n;
  s->plugs_last_add = plugs_last_add;
  s->sockets = sockets;
  s->sockets_n = sockets_n;
  s->sockets_last_add = sockets_last_add;
  while(properties && properties[n]) ++n;
  oyStringListAdd_( &s->properties, 0, properties, n,
                    oyObject_GetAlloc( s->oy_ ),
                    oyObject_GetDeAlloc( s->oy_ ) );

  return (oyCMMapi7_s*) s;
}


OYAPI int OYEXPORT
             oyCMMapi7_Run           ( oyCMMapi7_s       * api7,
                                       oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * ticket )
{
  oyCMMapi7_s_ * s = (oyCMMapi7_s_*)api7;
  int error = !plug;
  OY_TRACE_

  if(!api7)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  if(!plug)
  {
    WARNcc_S(api7, "no plug argument given" );
    return error;
  }

  OY_TRACE_START_
  if(oy_debug)
  {
    DBGs_NUM2_S( ticket,"CALLING %s[%s]->oyCMMFilterPlug_Run()",
                 oyNoEmptyString_m_(s->id_),s->registration );
  }

  error = s->oyCMMFilterPlug_Run( plug, ticket );

  OY_TRACE_END_(s->registration)
  if(oy_debug)
  {
    DBGs_NUM2_S( ticket,"DONE %s[%s]->oyCMMFilterPlug_Run()",
                 oyNoEmptyString_m_(s->id_),s->registration );
  }

  return error;
}

