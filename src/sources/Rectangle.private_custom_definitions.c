/** @internal
 *  Function    oy{{ class.baseName }}_Release__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  rectangle  the Rectangle object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyRectangle_Release__Members( oyRectangle_s_ * rectangle )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &rectangle->member );
   */

  if(rectangle->oy_->deallocateFunc_)
  {
    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( rectangle->member );
     */
  }
}

/** @internal
 *  Function    oy{{ class.baseName }}_Init__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  rectangle  the Rectangle object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyRectangle_Init__Members( oyRectangle_s_ * rectangle OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oy{{ class.baseName }}_Copy__Members
 *  @memberof   {{ class.name }}
 *  @brief      Custom {{ class.baseName }} copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyRectangle_s_ input object
 *  @param[out]  dst  the output oyRectangle_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyRectangle_Copy__Members( oyRectangle_s_ * dst, oyRectangle_s_ * src)
{
  int error = 0;

  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */
  dst->x = src->x;
  dst->y = src->y;
  dst->width = src->width;
  dst->height = src->height;

  return error;
}
