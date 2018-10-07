/** @file oyImage_s_.c

   [Template file inheritance graph]
   +-> oyImage_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2018 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyImage_s.h"
#include "oyImage_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_image_init_ = 0;
static const char * oyImage_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyImage_s_ * s = (oyImage_s_*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 512;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &text[strlen(text)], "%dx%d", s->width, s->height);
  else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "(%dx%d)%dc", s->width, s->height,
             s->layout_[oyCHANS]);
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "(%dx%d)%dc%s", s->width, s->height,
             s->layout_[oyCHANS],
             oyDataTypeToText( oyToDataType_m( s->layout_[oyLAYOUT] )));


  return text;
}


/* Include "Image.private_custom_definitions.c" { */
/** Function    oyImage_Release__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  image  the Image object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyImage_Release__Members( oyImage_s_ * image )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &image->member );
   */
  image->width = 0;
  image->height = 0;
  if(image->pixel_data && image->pixel_data->release)
    image->pixel_data->release( &image->pixel_data );

  if(image->user_data && image->user_data->release)
    image->user_data->release( &image->user_data );

  oyProfile_Release( &image->profile_ );
  oyOptions_Release( &image->tags );


  if(image->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = image->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( image->member );
     */
    if(image->layout_)
    { deallocateFunc( image->layout_ ); image->layout_ = 0; }

    if(image->channel_layout)
    { deallocateFunc( image->channel_layout ); image->channel_layout = 0; }
  }
}

/** Function    oyImage_Init__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  image  the Image object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyImage_Init__Members( oyImage_s_ * image )
{
  image->tags = oyOptions_New(0);
  image->layout_ = image->oy_->allocateFunc_( sizeof(oyPixel_t) * (oyCHAN0 + 
                                             4 + 1));
  memset(image->layout_,0,sizeof(oyPixel_t) * (oyCHAN0 + 4 + 1));
  return 0;
}

/** Function    oyImage_Copy__Members
 *  @memberof   oyImage_s
 *  @brief      Custom Image copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyImage_s_ input object
 *  @param[out]  dst  the output oyImage_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyImage_Copy__Members( oyImage_s_ * dst, oyImage_s_ * src)
{
  int error = 0;
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  return error;
}

/* } Include "Image.private_custom_definitions.c" */


/** @internal
 *  Function oyImage_New_
 *  @memberof oyImage_s_
 *  @brief   allocate a new oyImage_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyImage_s_ * oyImage_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_IMAGE_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyImage_s_ * s = 0;

  if(s_obj)
    s = (oyImage_s_*)s_obj->allocateFunc_(sizeof(oyImage_s_));
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    if(s_obj)
      oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyImage_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyImage_Copy;
  s->release = (oyStruct_Release_f) oyImage_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Image constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_IMAGE_S, (oyPointer)s );
  /* ---- end of custom Image constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Image constructor ----- */
  error += oyImage_Init__Members( s );
  /* ---- end of custom Image constructor ------- */
  
  
  
  

  if(!oy_image_init_)
  {
    oy_image_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyImage_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyImage_Copy__
 *  @memberof oyImage_s_
 *  @brief   real copy a Image object
 *
 *  @param[in]     image                 Image struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyImage_s_ * oyImage_Copy__ ( oyImage_s_ *image, oyObject_s object )
{
  oyImage_s_ *s = 0;
  int error = 0;

  if(!image || !object)
    return s;

  s = (oyImage_s_*) oyImage_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Image copy constructor ----- */
    error = oyImage_Copy__Members( s, image );
    /* ---- end of custom Image copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyImage_Release_( &s );

  return s;
}

/** @internal
 *  Function oyImage_Copy_
 *  @memberof oyImage_s_
 *  @brief   copy or reference a Image object
 *
 *  @param[in]     image                 Image struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyImage_s_ * oyImage_Copy_ ( oyImage_s_ *image, oyObject_s object )
{
  oyImage_s_ *s = image;

  if(!image)
    return 0;

  if(image && !object)
  {
    s = image;
    
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        if(n != s->oy_->ref_)
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
#ifdef HAVE_BACKTRACE
          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;

          nptrs = backtrace(buffer, BT_BUF_SIZE);

          /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
             would produce similar output to the following: */

          strings = backtrace_symbols(buffer, nptrs);
          if( strings == NULL )
          {
            perror("backtrace_symbols");
          } else
          {
            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            fprintf(stderr, "\n");
            for(j = start; j >= 0; j--)
            {
              if(oy_debug)
                fprintf(stderr, "%s\n", strings[j]);
              else
              {
                char * t = NULL, * txt = NULL;
                const char * line = strings[j],
                           * tmp = strchr( line, '(' );
                if(tmp) t = oyStringCopy( &tmp[1], NULL );
                else t = oyStringCopy( line, NULL );
                txt = strchr( t, '+' );
                if(txt) txt[0] = '\000';
                if(j > 0 && (strstr(strings[j-1], t) != NULL) )
                  oyFree_m_(t);
                if(t)
                {
                  if(j==0)
                    fprintf(stderr, "%s() ", t);
                  else
                    fprintf(stderr, "%s()->", t);
                  oyFree_m_(t);
                }
              }
            }
            free(strings);
          }
#endif
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   track_name, s->oy_->id_, s->oy_->ref_, n );
          for(i = 0; i < n; ++i)
          {
            track_name = oyStructTypeToText(parents[i]->type_);
            fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                     track_name, parents[i]->oy_->id_ );
          }
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyImage_Copy__( image, object );

  return s;
}
 
/** @internal
 *  Function oyImage_Release_
 *  @memberof oyImage_s_
 *  @brief   release and possibly deallocate a Image object
 *
 *  @param[in,out] image                 Image struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyImage_Release_( oyImage_s_ **image )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyImage_s_ *s = 0;

  if(!image || !*image)
    return 0;

  s = *image;

  *image = 0;

  observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 );

  if(oy_debug_objects >= 0 && s->oy_)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      if(n != s->oy_->ref_)
      {
        int i;
#ifdef HAVE_BACKTRACE
          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;

          nptrs = backtrace(buffer, BT_BUF_SIZE);

          /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
             would produce similar output to the following: */

          strings = backtrace_symbols(buffer, nptrs);
          if( strings == NULL )
          {
            perror("backtrace_symbols");
          } else
          {
            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            fprintf(stderr, "\n");
            for(j = start; j >= 0; j--)
            {
              if(oy_debug)
                fprintf(stderr, "%s\n", strings[j]);
              else
              {
                char * t = NULL, * txt = NULL;
                const char * line = strings[j],
                           * tmp = strchr( line, '(' );
                if(tmp) t = oyStringCopy( &tmp[1], NULL );
                else t = oyStringCopy( line, NULL );
                txt = strchr( t, '+' );
                if(txt) txt[0] = '\000';
                if(j > 0 && (strstr(strings[j-1], t) != NULL) )
                  oyFree_m_(t);
                if(t)
                {
                  if(j==0)
                    fprintf(stderr, "%s() ", t);
                  else
                    fprintf(stderr, "%s()->", t);
                  oyFree_m_(t);
                }
              }
            }
            free(strings);
          }
#endif
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if((oyObject_UnRef(s->oy_) - 2 * observer_refs) > 0)
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] destruct\n", track_name, s->oy_->id_);
    }
  }

  
  /* ---- start of custom Image destructor ----- */
  oyImage_Release__Members( s );
  /* ---- end of custom Image destructor ------- */
  
  
  
  



  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;
    int refs = s->oy_->ref_;

    if(refs > 1)
      fprintf( stderr, "!!!ERROR: node[%d]->object can not be untracked with refs: %d\n", id, refs);

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Image.private_methods_definitions.c" { */
#include "oyranos_image_internal.h"

/**
 *  @internal
 *  Function oyImage_CombinePixelLayout2Mask_
 *  @memberof oyImage_s
 *  @brief   describe a images channel and pixel layout
 *
 *  - gather informations about the pixel layout
 *  - describe the color channels characteristic into oyImage_s::channel_layout
 *  - store some text in the images nick name as a ID
 *
 *  @version Oyranos: 0.5.0
 *  @date    2012/09/06
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 */
int oyImage_CombinePixelLayout2Mask_ ( oyImage_s_        * image,
                                       oyPixel_t           pixel_layout )
{
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = image->profile_;
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColorOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColorChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  oyPixel_t *mask = image->oy_->allocateFunc_( sizeof(oyPixel_t) * (oyCHAN0 + 
                    OY_MAX(n,cchan_n) + 1));
  int error = !mask;
  int so = oyDataTypeGetSize( t );
  int w = image->width;
  int h = image->height;
  int i;
  char * text,
       * hash_text = NULL;
  oyImage_s_ * s = image;
  oyCHANNELTYPE_e * clayout = 0; /* non profile described channels */

  if(!mask) return error;
  text = oyAllocateFunc_(512);
  if(!text) { image->oy_->deallocateFunc_(mask); return 1; }

  if(!n && cchan_n)
    n = cchan_n;

  /* describe the pixel layout and access */
  if(error <= 0)
  {
    error = !memset( mask, 0, sizeof(mask) * sizeof(oyPixel_t));
    if(oyToPlanar_m( pixel_layout ))
    {
      mask[oyPOFF_X] = 1;
      mask[oyCOFF] = w*h*n;
    } else {
      mask[oyPOFF_X] = n;
      mask[oyCOFF] = 1;
    }
    mask[oyPOFF_Y] = mask[oyPOFF_X] * w;
    mask[oyDATA_SIZE] = so;
    mask[oyLAYOUT] = pixel_layout;
    mask[oyCHANS] = n;

    if(swap)
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + cchan_n - i - 1;
    else
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + i;
  }

  /* describe the channels characters */
  if(!s->channel_layout)
  {
    clayout = image->oy_->allocateFunc_( sizeof(int) * ( OY_MAX(n,cchan_n)+ 1));
    if(!clayout) { image->oy_->deallocateFunc_(mask); oyDeAllocateFunc_(text); return 1; }
    /* we dont know about the content */
    for(i = 0; i < n; ++i)
      clayout[i] = oyCHANNELTYPE_OTHER;
    /* describe profile colors */
    for(i = coff_x; i < coff_x + cchan_n; ++i)
      clayout[i] = oyICCColorSpaceToChannelLayout( 
                     oyProfile_GetSignature(profile, oySIGNATURE_COLOR_SPACE),
                     i - coff_x );
    /* place a end marker */
    clayout[n] = oyCHANNELTYPE_UNDEFINED;
      s->channel_layout = clayout;
  }

  /* describe the image */
  oySprintf_( text, 
                  "{ \"oyImage_s\": { \"id\": \"%d\", \"width\": \"%d\", \"height\": \"%d\", \"resolution\": [\"%.02f\", \"%.02f\"],\n",
                  oyObject_GetId(image->oy_),
                  image->width,
                  image->height,
                  image->resolution_x,
                  image->resolution_y);
  hashTextAdd_m( text );
  i = strlen(text);
  oySprintf_( text, "\"icc_profile\": %s\n", oyProfile_GetText(profile, oyNAME_JSON));
  if(text[16] == '\n') text[16] = ' ';
  i = strlen(text);
  if(text[i-3] == '\n') text[i-3] = ' ';
  if(text[i-1] == '\n') text[i-1] = ',';
  hashTextAdd_m( text );
  oySprintf_( text, "\n \"channels\": { \"all\": \"%d\", \"color\": \"%d\" }, ", n, cchan_n );
  hashTextAdd_m( text );
  oySprintf_( text,
              "\"offsets\": { \"first_color_sample\": \"%d\", \"next_pixel\": \"%d\"}"
              /*"  next line = %d\n"*/,
              coff_x, mask[oyPOFF_X]/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m( ", \"swap\": {" );
    if(swap)
    {
      if(oyToByteswap_m( pixel_layout ))
        hashTextAdd_m( " \"colorswap\": \"yes\"," );
      else
        hashTextAdd_m( " \"colorswap\": \"yes\"" );
    }
    if( oyToByteswap_m( pixel_layout ) )
      hashTextAdd_m( " \"byteswap\": \"yes\"" );
    hashTextAdd_m( " }" );
  }

  if( oyToFlavor_m( pixel_layout ) )
  {
    oySprintf_( text, ", \"flawor value\": \"yes\"" );
    hashTextAdd_m( text );
  }
  oySprintf_( text, ",\n\"sample_type\": { \"value\": \"%s\", \"byte\": \"%d\" }",
                    oyDataTypeToText(t), so );
  hashTextAdd_m( text );
  oySprintf_( text, "}}");
  hashTextAdd_m( text );

  if(error <= 0)
    error = oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );


  oyDeAllocateFunc_(text);
  oySTRUCT_FREE_m(s, hash_text);

  if(s->oy_->deallocateFunc_)
  {
    if(s->layout_)
      s->oy_->deallocateFunc_(s->layout_);
    s->layout_ = 0;
  }
  s->layout_ = mask;


  return 0;
}

/* } Include "Image.private_methods_definitions.c" */

