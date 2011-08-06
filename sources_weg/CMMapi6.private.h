/**
 *  typedef oyCMMdata_Convert_f
 *  @brief   convert between data formats
 *  @ingroup module_api
 *  @memberof oyCMMapi6_s
 *
 *  The function might be used to provide a module specific context.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
typedef int(*oyCMMdata_Convert_f)    ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node );
