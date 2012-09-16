oyCMMapi_s *       oyCMMapi_GetNext  ( oyCMMapi_s        * api )
{
  return ((oyCMMapi_s_*)api)->next;
}
oyCMMInit_f        oyCMMapi_GetInitF ( oyCMMapi_s        * api );
{
  return ((oyCMMapi_s_*)api)->oyCMMInit;
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
