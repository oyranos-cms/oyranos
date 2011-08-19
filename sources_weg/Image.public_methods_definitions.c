/** Function  oyImage_Width
 *  @memberof oyImage_s
 *  @brief    Get the image width
 *
 *  @version Oyranos: 0.1.13
 *  @since   2011/08/19 (Oyranos: 0.1.13)
 *  @date    2011/08/19
 */
int            oyImage_Width     ( oyImage_s         * image )
{
  if (image)
    return ((oyImage_s_*)image)->width;
  else
    return 0;
}

/** Function  oyImage_Height
 *  @memberof oyImage_s
 *  @brief    Get the image height
 *
 *  @version Oyranos: 0.1.13
 *  @since   2011/08/19 (Oyranos: 0.1.13)
 *  @date    2011/08/19
 */
int            oyImage_Height    ( oyImage_s         * image )
{
  if (image)
    return ((oyImage_s_*)image)->height;
  else
    return 0;
}
