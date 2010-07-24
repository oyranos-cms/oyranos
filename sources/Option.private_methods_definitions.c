/**
 *  @internal
 *  Function oyOption_FromStatic_
 *  @memberof oyOption_s
 *  @brief   provide the current state of Oyranos behaviour settings
 *
 *  @todo harmonise the xml key names to the elektra ones to form one namespace
 *  @deprecated we move to the oyCMMapi5_s for static stuff
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/06 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
oyOption_s *   oyOption_FromStatic_  ( oyOption_t_       * opt,
                                       oyObject_s          object )
{
  oyOption_s_ * s = 0;
  int error = !opt;

  if(error)
    return s;

  s = (oyOption_s_*)oyOption_New( 0, object );
  if(!s)
    return s;

  /*s->id = opt->id;*/
  s->registration = oyStringAppend_( opt->config_string, opt->config_string_xml,
                                     s->oy_->allocateFunc_ );
  s->value = s->oy_->allocateFunc_(sizeof(oyValue_u));
  memset( s->value, 0, sizeof(oyValue_u) );

  if(oyWIDGET_BEHAVIOUR_START < opt->id && opt->id < oyWIDGET_BEHAVIOUR_END)
  {
    s->value_type = oyVAL_DOUBLE;
    s->value->dbl = opt->default_value;
    s->source = oyOPTIONSOURCE_FILTER;
  } else
  {
    s->value_type = oyVAL_STRING;
    s->value->string = oyGetDefaultProfileName( (oyPROFILE_e)opt->id,
                                                s->oy_->allocateFunc_ );
    if(!s->value->string)
      s->value->string = oyStringCopy_( opt->default_string, s->oy_->allocateFunc_);
    s->source = oyOPTIONSOURCE_FILTER;
    oyOption_UpdateFlags_( s );
  }

  return s;
}

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
