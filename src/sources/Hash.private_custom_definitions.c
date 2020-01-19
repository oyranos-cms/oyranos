/** @internal
 *  Function    oyHash_Release__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyHash_Release__Members( oyHash_s_ * hash )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &hash->member );
   */

  /* should not happen */
  if(hash->oy_struct && hash->oy_struct->release)
    hash->oy_struct->release( &hash->oy_struct );

  if(hash->oy_->deallocateFunc_)
  {
    /*oyDeAlloc_f deallocateFunc = hash->oy_->deallocateFunc_;*/

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( hash->member );
     */
  }
}

/** @internal
 *  Function    oyHash_Init__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  hash  the Hash object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Init__Members( oyHash_s_ * hash )
{
  oyStruct_AllocHelper_m_( hash->oy_->hash_ptr_,
                    unsigned char,
                    OY_HASH_SIZE*2,
                    hash,
                    oyHash_Release_( &hash ));

  return 0;
}

/** @internal
 *  Function    oyHash_Copy__Members
 *  @memberof   oyHash_s
 *  @brief      Custom Hash copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyHash_s_ input object
 *  @param[out]  dst  the output oyHash_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyHash_Copy__Members( oyHash_s_ * dst, oyHash_s_ * src)
{
  int error = 0;

  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  return error;
}
