/** @file oyNamedColor_s_.c

   [Template file inheritance graph]
   +-> oyNamedColor_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2013/09/04
 */



  
#include "oyNamedColor_s.h"
#include "oyNamedColor_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "NamedColor.private_custom_definitions.c" { */
/** Function    oyNamedColor_Release__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  namedcolor  the NamedColor object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyNamedColor_Release__Members( oyNamedColor_s_ * namedcolor )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &namedcolor->member );
   */
  oyNamedColor_s_ * s = namedcolor;

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->channels_)
      deallocateFunc( s->channels_ ); s->channels_ = 0;

    if(s->blob_) /* s->blob_len */
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;
  }
}

/** Function    oyNamedColor_Init__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  namedcolor  the NamedColor object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColor_Init__Members( oyNamedColor_s_ * namedcolor )
{
  return 0;
}

/** Function    oyNamedColor_Copy__Members
 *  @memberof   oyNamedColor_s
 *  @brief      Custom NamedColor copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyNamedColor_s_ input object
 *  @param[out]  dst  the output oyNamedColor_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColor_Copy__Members( oyNamedColor_s_ * dst, oyNamedColor_s_ * src)
{
  int error = 0;
  oyNamedColor_s_ * s = dst;
  oyAlloc_f allocateFunc_ = 0;
  const double    * chan;
  const char      * blob;
  int               blob_len;
  oyProfile_s     * ref;
  int n = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  chan = src->channels_;
  blob = src->blob_;
  blob_len = src->blob_len_;
  ref = src->profile_;

  /* Copy each value of src to dst here */
  if(error <= 0)
    s->profile_  = oyProfile_Copy( ref, 0 );

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = allocateFunc_( n * sizeof(double) );
  oyCopyColor( chan, &s->channels_[0], 1, ref, n );
  oyCopyColor( 0, &s->XYZ_[0], 1, 0, 0 );

  if(error <= 0 && blob && blob_len)
  {
    s->blob_ = allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(error <= 0)
      error = !memcpy( s->blob_, blob, blob_len );

    if(error <= 0)
      s->blob_len_ = blob_len;
  }

  return error;
}

/* } Include "NamedColor.private_custom_definitions.c" */



/** @internal
 *  Function oyNamedColor_New_
 *  @memberof oyNamedColor_s_
 *  @brief   allocate a new oyNamedColor_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColor_s_ * oyNamedColor_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLOR_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyNamedColor_s_ * s = 0;

  if(s_obj)
    s = (oyNamedColor_s_*)s_obj->allocateFunc_(sizeof(oyNamedColor_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyNamedColor_s_) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyNamedColor_Copy;
  s->release = (oyStruct_Release_f) oyNamedColor_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom NamedColor constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_NAMED_COLOR_S, (oyPointer)s );
  /* ---- end of custom NamedColor constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom NamedColor constructor ----- */
  error += oyNamedColor_Init__Members( s );
  /* ---- end of custom NamedColor constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyNamedColor_Copy__
 *  @memberof oyNamedColor_s_
 *  @brief   real copy a NamedColor object
 *
 *  @param[in]     namedcolor                 NamedColor struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColor_s_ * oyNamedColor_Copy__ ( oyNamedColor_s_ *namedcolor, oyObject_s object )
{
  oyNamedColor_s_ *s = 0;
  int error = 0;

  if(!namedcolor || !object)
    return s;

  s = (oyNamedColor_s_*) oyNamedColor_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom NamedColor copy constructor ----- */
    error = oyNamedColor_Copy__Members( s, namedcolor );
    /* ---- end of custom NamedColor copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyNamedColor_Release_( &s );

  return s;
}

/** @internal
 *  Function oyNamedColor_Copy_
 *  @memberof oyNamedColor_s_
 *  @brief   copy or reference a NamedColor object
 *
 *  @param[in]     namedcolor                 NamedColor struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColor_s_ * oyNamedColor_Copy_ ( oyNamedColor_s_ *namedcolor, oyObject_s object )
{
  oyNamedColor_s_ *s = namedcolor;

  if(!namedcolor)
    return 0;

  if(namedcolor && !object)
  {
    s = namedcolor;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyNamedColor_Copy__( namedcolor, object );

  return s;
}
 
/** @internal
 *  Function oyNamedColor_Release_
 *  @memberof oyNamedColor_s_
 *  @brief   release and possibly deallocate a NamedColor object
 *
 *  @param[in,out] namedcolor                 NamedColor struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyNamedColor_Release_( oyNamedColor_s_ **namedcolor )
{
  /* ---- start of common object destructor ----- */
  oyNamedColor_s_ *s = 0;

  if(!namedcolor || !*namedcolor)
    return 0;

  s = *namedcolor;

  *namedcolor = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom NamedColor destructor ----- */
  oyNamedColor_Release__Members( s );
  /* ---- end of custom NamedColor destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "NamedColor.private_methods_definitions.c" { */
/** @brief copy pure colors
 *
 *  Handle color only.
 *  With a empty \b from variable set -1 as default in \b to.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
void   oyCopyColor                   ( const double      * from,
                                       double            * to,
                                       int                 n,
                                       oyProfile_s       * ref,
                                       int                 channels_n )
{
  int i, j;
  icColorSpaceSignature sig = 0;
  int c = 0;
  int error = 0;

  if(!n || !to)
    return;

  if(ref)
    sig = oyProfile_GetSignature ( ref, oySIGNATURE_COLOUR_SPACE );
  else
    sig = icSigXYZData;

  c = oyICCColourSpaceGetChannelCount( sig );

  if(from)
  {
    error = !memcpy( to, from, sizeof(double) * (n * c) );
    if(error)
      WARNc_S("Problem with memcpy.");

  } 
  else
  {
    if(!channels_n && c)
      channels_n = c;
    else if(channels_n && !c)
      c = channels_n;

    if(channels_n)
    switch(sig)
    {
      case icSigLabData:
      case icSigLuvData:
      case icSigHsvData:
      case icSigHlsData:
      case icSigYCbCrData:
           for( i = 0; i < n; ++i )
           {
             to[i*channels_n+0] = -1;
             to[i*channels_n+1] = 0;
             to[i*channels_n+2] = 0;
             for( j = c; j < channels_n; ++j )
               if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
      case icSigXYZData:
      case icSigRgbData:
      default:
           for( i = 0; i < n; ++i )
           {
             for( j = 0; j < channels_n; ++j )
               if(j < c)
                 to[i*channels_n+j] = -1;
               else if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
    }
  }
}

/* } Include "NamedColor.private_methods_definitions.c" */

