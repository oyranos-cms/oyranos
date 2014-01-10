/**
 *  @memberof   oyCMMapi6_s
 *  @brief      Get data type identifier
 *
 *  @param         api                 object
 *  @param         out_direction       
 *                                     - 0 - input data type
 *                                     - 1 - output data type
 *  @return                            the identifier string
 *
 *  @version Oyranos: 0.9.5
 *  @since   2014/01/10 (Oyranos: 0.9.5)
 *  @date    2014/01/10
 */
const char *       oyCMMapi6_GetDataType
                                     ( oyCMMapi6_s       * api,
                                       int                 out_direction )
{
  if(out_direction)
    return ((oyCMMapi6_s_*)api)->data_type_out;
  else
    return ((oyCMMapi6_s_*)api)->data_type_in;
}

/**
 *  @memberof   oyCMMapi6_s
 *  @brief      Get data conversion function
 *
 *  @param         api                 object
 *  @return                            the function
 *
 *  @version Oyranos: 0.9.5
 *  @since   2014/01/10 (Oyranos: 0.9.5)
 *  @date    2014/01/10
 */
oyModuleData_Convert_f
                   oyCMMapi6_GetConvertF
                                     ( oyCMMapi6_s       * api )
{
  return ((oyCMMapi6_s_*)api)->oyModuleData_Convert;
}
