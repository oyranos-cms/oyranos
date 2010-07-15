/** Function oyName_releaseMembers
 *  @memberof oyName_s
 *  @brief   release only members
 *
 *  Useful to release the member strings but not the struct itself, which can
 *  in this case be static. Deallocation uses oyDeAllocateFunc_().
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/13
 */
OYAPI int OYEXPORT oyName_ReleaseMembers   ( oyName_s          * name )
{
  if(!name)
    return 0;

  oyName_Release__Members( (oyName_s_*)name );

  return 0;
}
