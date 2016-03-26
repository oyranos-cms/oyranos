int                oyPixelAccess_ChangeRectangle (
                                       oyPixelAccess_s   * pixel_access,
                                       double              start_x,
                                       double              start_y,
                                       oyRectangle_s     * output_rectangle );
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object );
oyImage_s *        oyPixelAccess_GetOutputImage (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetOutputImage (
                                       oyPixelAccess_s   * pixel_access,
                                       oyImage_s         * image );
oyFilterGraph_s *  oyPixelAccess_GetGraph (
                                       oyPixelAccess_s   * pixel_access );
oyOptions_s *      oyPixelAccess_GetRequestQueue (
                                       oyPixelAccess_s   * pixel_access );
oyArray2d_s *      oyPixelAccess_GetArray (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetArray (
                                       oyPixelAccess_s   * pixel_access,
                                       oyArray2d_s       * array );
oyRectangle_s *    oyPixelAccess_GetArrayROI (
                                       oyPixelAccess_s   * pixel_access );
oyStruct_s *       oyPixelAccess_GetUserData (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetUserData (
                                       oyPixelAccess_s   * pixel_access,
                                       oyStruct_s        * user_data );
int32_t            oyPixelAccess_GetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access );
int                oyPixelAccess_SetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access,
                                       int32_t             workspace_id );
double             oyPixelAccess_GetStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical );
double             oyPixelAccess_GetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical );
int                oyPixelAccess_SetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical,
                                       double              start );
