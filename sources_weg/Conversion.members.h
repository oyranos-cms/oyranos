  oyFilterNode_s_    * input;          /**< the input image filter; Most users will start logically with this pice and chain their filters to get the final result. */
  oyFilterNode_s_    * out_;           /**< @private the Oyranos output image. Oyranos will stream the filters starting from the end. This element will be asked on its first plug. */
