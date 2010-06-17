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
OYAPI int OYEXPORT oyName_releaseMembers   ( oyName_s          * obj,
                                       oyDeAlloc_f         deallocateFunc )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return 0;

  if(!deallocateFunc)
    deallocateFunc = oyDeAllocateFunc_;

  s = obj;

  if(s->nick)
    deallocateFunc(s->nick); s->nick = 0;

  if(s->name)
    deallocateFunc(s->name); s->name = 0;

  if(s->description)
    deallocateFunc(s->description); s->description = 0;

  return error;
}
