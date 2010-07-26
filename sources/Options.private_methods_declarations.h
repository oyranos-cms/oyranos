oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object );
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object );
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s       * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value );
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value );
