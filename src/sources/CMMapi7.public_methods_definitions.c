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

  if(!api7)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API7_S, return 1 )

  if(!plug)
  {
    WARNcc_S(api7, "no plug argument given" );
    return error;
  }

  error = s->oyCMMFilterPlug_Run( plug, ticket );

  return error;
}

