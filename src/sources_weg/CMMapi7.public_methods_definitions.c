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
