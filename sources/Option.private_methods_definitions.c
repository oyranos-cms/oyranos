/**
 *  @internal
 *  Function oyOption_Match_
 *  @memberof oyOption_s
 *  @brief   two option key name matches
 *
 *  A registration name match is not required.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
int            oyOption_Match_       ( oyOption_s_        * option_a,
                                       oyOption_s_        * option_b )
{
  int erg = 0;

  if( option_a && option_b )
  {
    char * a =  oyFilterRegistrationToText( option_a->registration,
                                            oyFILTER_REG_TYPE, 0 );
    char * b =  oyFilterRegistrationToText( option_b->registration,
                                            oyFILTER_REG_TYPE, 0 );

    if( oyStrcmp_( a, b ) == 0 )
      erg = 1;

    oyDeAllocateFunc_(a);
    oyDeAllocateFunc_(b);
  }

  return erg;
}

/**
 *  @internal
 *  Function oyOption_UpdateFlags_
 *  @memberof oyOption_s
 *  @brief   set the ::flags member
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
void         oyOption_UpdateFlags_   ( oyOption_s_        * o )
{
  char * tmp = 0;

  if(o && o->registration && oyStrrchr_( o->registration, '/' ))
  {
      tmp = oyStrrchr_( o->registration, '/' );
      if(oyStrstr_( tmp, "front" ))
         o->flags |= oyOPTIONATTRIBUTE_FRONT;
      if(oyStrstr_( tmp, "advanced" ))
         o->flags |= oyOPTIONATTRIBUTE_ADVANCED;
      tmp = 0;
  }
}
