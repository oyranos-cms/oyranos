const char *       oyCMMapi6_GetDataType
                                     ( oyCMMapi6_s       * api,
                                       int                 out_direction )
{
  if(out_direction)
    return ((oyCMMapi6_s_*)api)->data_type_out;
  else
    return ((oyCMMapi6_s_*)api)->data_type_in;
}

oyModuleData_Convert_f
                   oyCMMapi6_GetConvertF
                                     ( oyCMMapi6_s       * api )
{
  return ((oyCMMapi6_s_*)api)->oyModuleData_Convert;
}
