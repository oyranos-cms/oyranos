/** @internal
 *  Function    oyProfileTag_Release__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyProfileTag_Release__Members( oyProfileTag_s_ * profiletag OY_UNUSED )
{
  /* Nothing to deallocate here */
}

/** @internal
 *  Function    oyProfileTag_Init__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Init__Members( oyProfileTag_s_ * profiletag OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oyProfileTag_Copy__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfileTag_s_ input object
 *  @param[out]  dst  the output oyProfileTag_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Copy__Members( oyProfileTag_s_ * dst, oyProfileTag_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */
  /* Copy the whole struct mem block as is - is this safe? */
  memcpy(dst, src, sizeof(oyProfileTag_s_));

  return 0;
}
