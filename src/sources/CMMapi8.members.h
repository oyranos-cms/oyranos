  /** obtain configs matching to a set of options */
  oyConfigs_FromPattern_f oyConfigs_FromPattern;
  /** manipulate given configs */
  oyConfigs_Modify_f oyConfigs_Modify;
  oyConfig_Rank_f  oyConfig_Rank;      /**< test config */

  oyCMMui_s      * ui;                 /**< a UI description */
  oyIcon_s       * icon;               /**< module associated icon */

  /** zero terminated list of rank attributes;
   *  The data is just informational. In case all properties to rank a given 
   *  device pair are well known, this rank_map can be copied into each 
   *  oyConfig_s device created by oyConfigs_FromPattern():
   *  @code
      oyConfig_s * device = oyConfig_FromRegistration( CMM_BASE_REG, 0);
      device->rank_map = oyRankMapCopy( dDev_api8->rank_map,
                                        device->oy_->allocateFunc_);
      @endcode
   *  For additional dynamic rank pads the allocation can be done individually.
   */
  oyRankMap      * rank_map;
