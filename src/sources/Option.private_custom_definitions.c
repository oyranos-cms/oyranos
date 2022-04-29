#include "oyStructList_s.h"

/** @internal
 *  Function    oyOption_Release__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyOption_Release__Members( oyOption_s_ * option )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &option->member );
   */

  option->id = 0;

  if(option->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = option->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( option->member );
     */

    if(option->registration)
      deallocateFunc(option->registration);
    option->registration = NULL;

    oyOption_Clear( (oyOption_s*) option );
  }
}

static int oy_option_id_ = 0;

/** @internal
 *  Function    oyOption_Init__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Init__Members( oyOption_s_ * option )
{
  option->id = oy_option_id_++;
  option->registration = NULL;
  option->value = NULL;

  return 0;
}

/** @internal
 *  Function    oyOption_Copy__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyOption_s_ input object
 *  @param[out]  dst  the output oyOption_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Copy__Members( oyOption_s_ * dst, oyOption_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
  int error = 0;

  if(!dst || !src || dst == src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  if(! (dst->value_type == src->value_type &&
        oyValueEqual( dst->value, src->value, dst->value_type, -1 )) )
  {
    /* oyOption_Clear does normally signal emitting; block that. */
    oyStruct_DisableSignalSend( (oyStruct_s*)dst );
    error = oyOption_Clear( (oyOption_s*)dst );
    oyStruct_EnableSignalSend( (oyStruct_s*)dst );

    dst->value_type = src->value_type;
    dst->value = allocateFunc_(sizeof(oyValue_u));
    memset(dst->value, 0, sizeof(oyValue_u));
    oyValueCopy( dst->value, src->value, dst->value_type,
                 allocateFunc_, deallocateFunc_ );
  }

  if(dst->registration)
    deallocateFunc_(dst->registration);
  dst->registration = oyStringCopy_( src->registration, allocateFunc_ );
  dst->source = src->source;
  dst->flags = src->flags;
  oyStruct_ObserverSignal( (oyStruct_s*)dst, oySIGNAL_DATA_CHANGED, 0 );

  return error;
}
