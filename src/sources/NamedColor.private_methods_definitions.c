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
    sig = oyProfile_GetSignature ( ref, oySIGNATURE_COLOR_SPACE );
  else
    sig = icSigXYZData;

  c = oyICCColorSpaceGetChannelCount( sig );

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
