/** Function    oyName_Release__Members
 *  @memberof   oyName_s
 *  @brief      Custom Name destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members wich have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  name  the Name object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyName_Release__Members( oyName_s_ * name )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &name->member );
   */

  if(name->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = name->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( name->member );
     */
    if(name->nick)
      deallocateFunc(name->nick); name->nick = 0;

    if(name->name)
      deallocateFunc(name->name); name->name = 0;

    if(name->description)
      deallocateFunc(name->description); name->description = 0;
  }
}

/** Function    oyName_Init__Members
 *  @memberof   oyName_s
 *  @brief      Custom Name constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  name  the Name object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyName_Init__Members( oyName_s_ * name )
{
  name->nick = NULL:
  name->name = NULL:
  name->description = NULL:
}

/** Function    oyName_Copy__Members
 *  @memberof   oyName_s
 *  @brief      Custom Name copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyName_s_ input object
 *  @param[out]  dst  the output oyName_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyName_Copy__Members( oyName_s_ * dst, oyName_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = to->oy_->allocateFunc_;
  deallocateFunc_ = to->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  oyName_s_ * s = dst;

  if(src->name)
    s = oyName_set_ ( s, src->name, oyNAME_NAME, allocateFunc_, deallocateFunc_ );
  if(src->nick)
    s = oyName_set_ ( s, src->nick, oyNAME_NICK, allocateFunc_, deallocateFunc_ );
  if(src->description)
    s = oyName_set_ ( s, src->description, oyNAME_DESCRIPTION, allocateFunc_, deallocateFunc_ );

  return 0;
}
