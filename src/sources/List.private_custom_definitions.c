/** Function    oyList_Release__Members
 *  @memberof   oyList_s
 *  @brief      Custom List destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyList_Release__Members( oyList_s_ * list OY_UNUSED )
{
}

/** Function    oyList_Init__Members
 *  @memberof   oyList_s
 *  @brief      Custom List constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Init__Members( oyList_s_ * list OY_UNUSED )
{
  return 0;
}

/** Function    oyList_Copy__Members
 *  @memberof   oyList_s
 *  @brief      Custom List copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyList_s_ input object
 *  @param[out]  dst  the output oyList_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Copy__Members( oyList_s_ * dst OY_UNUSED, oyList_s_ * src OY_UNUSED)
{
  return 0;
}
