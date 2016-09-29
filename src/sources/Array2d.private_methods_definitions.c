/** Function  oyArray2d_Init_
 *  @memberof oyArray2d_s
 *  @brief    Initialise Array
 *
 *  @version  Oyranos: 0.9.0
 *  @since    2012/10/20 (Oyranos: 0.9.0)
 *  @date     2012/10/20
 */
int                oyArray2d_Init_   ( oyArray2d_s_      * s,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type )
{
  int error = !(width || height);
  if(error <= 0)
  {
    int y_len = sizeof(unsigned char *) * (height + 1);
    oyOBJECT_e rtype = oyOBJECT_RECTANGLE_S;

    s->width = width;
    s->height = height;
    s->t = data_type;
    memcpy( &s->data_area, &rtype, sizeof(oyOBJECT_e) );
    oyRectangle_SetGeo( (oyRectangle_s*)&s->data_area, 0,0, width, height );
    s->array2d = s->oy_->allocateFunc_( y_len );
    error = !memset( s->array2d, 0, y_len );
    s->own_lines = oyNO;
  }
  return error;
}

/** Function  oyArray2d_Create_
 *  @memberof oyArray2d_s
 *  @brief    Allocate and initialise a oyArray2d_s object widthout pixel
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
oyArray2d_s_ *
                   oyArray2d_Create_ ( int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s_ * s = 0;
  int error = 0;

  if(!width || !height)
    return s;

  s = (oyArray2d_s_*)oyArray2d_New( object );
  error = !s;

  if(error <= 0)
  {
    error = oyArray2d_Init_( s, width, height, data_type );
  }

  return s;
}

/** Function  oyArray2d_ReleaseArray_
 *  @memberof oyArray2d_s
 *  @brief    Release Array2d::array member
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.9.1
 *  @date    2012/11/02
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 */
int          oyArray2d_ReleaseArray_ ( oyArray2d_s       * obj )
{
  int error = 0;
  oyArray2d_s_ * s = (oyArray2d_s_*)obj;

  if(s && s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int y, y_max = s->data_area.height + s->data_area.y;
    size_t dsize = oyDataTypeGetSize( s->t );

    if(oy_debug > 3)
    {
      int32_t channels = 1;
      oyOptions_FindInt( s->oy_->handles_, "channels", 0, &channels );
      oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)s,
                       OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,
                       oyArray2d_Show( (oyArray2d_s*)s, channels) );
    }

    for( y = s->data_area.y; y < y_max; ++y )
    {
      if((s->own_lines == 1 && y == s->data_area.y) ||
         s->own_lines == 2)
        deallocateFunc( &s->array2d[y][dsize * (int)OY_ROUND(s->data_area.x)] );
      s->array2d[y] = 0;
    }
    deallocateFunc( s->array2d + (int)OY_ROUND(s->data_area.y) );
    s->array2d = 0;
  }

  return error;
}
 
/** Function  oyArray2d_ToPPM_
 *  @memberof oyArray2d_s
 *  @brief    Dump array to a netppm file 
 *  @internal
 *
 *  @param[in]     array               the array to fill read from
 *  @param[in]     file_name           rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/18 (Oyranos: 0.1.10)
 *  @date    2009/06/18
 */
int              oyArray2d_ToPPM_    ( oyArray2d_s_      * array,
                                       const char        * file_name )
{
  oyArray2d_s_ * s = array;
  int error = 0, i,j, len, shift;
  size_t size,
         byteps;
  char * buf,
       * data;

  if(!array || !file_name || !file_name[0])
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(!error)
  {
    if(s->array2d[0] == NULL)
    {
      printf("oyArray2d_s[%d] is not yet used/allocated\n",
             oyObject_GetId(s->oy_));
      return 1;
    }

    byteps = oyDataTypeGetSize(s->t); /* byte per sample */
    size = s->width * s->height * byteps;
    buf = oyAllocateFunc_(size + 1024);

    if(buf && size)
    {
      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n255\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyUINT16:    /* 16-bit integer */
      case oyUINT32:    /* 32-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n65535\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyHALF:      /* 16-bit floating point number */
      case oyFLOAT:     /* IEEE floating point number */
      case oyDOUBLE:    /* IEEE double precission floating point number */
           sprintf( buf, "Pf\n#%s:%d oyArray2d_s[%d]\n%d %d\n%s\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height,
                    oyBigEndian()? "1.0" : "-1.0" );
           break;
      default: return 1;
      }

      len = oyStrlen_(buf);
      data = &buf[len];
      shift = oyBigEndian() ? 0 : 1;

      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
      case oyFLOAT:     /* IEEE floating point number */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT16:    /* 16-bit integer */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps + shift],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT32:    /* 32-bit integer */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] =
                                       *((uint32_t*)&s->array2d[i][j*byteps]) /
                                                     65537;
           break;
      case oyHALF:      /* 16-bit floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] = 
                                       *((uint16_t*)&s->array2d[i][j*byteps]);
           break;
      case oyDOUBLE:    /* IEEE double precission floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((float*)&data[i*s->width*2])[j] =
                                       *((double*)&s->array2d[i][j*byteps]);
           break;
      default: return 1;
      }

      error = oyWriteMemToFile_( file_name, buf, len + size );
    }

    if(buf) oyDeAllocateFunc_(buf);
      size = 0;
  }

  return error;
}
