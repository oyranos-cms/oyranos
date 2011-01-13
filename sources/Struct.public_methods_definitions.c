/** Function oyStruct_GetText
 *  @memberof oyStruct_s
 *  @brief   get a text dump
 *
 *  As the object type module for text informations.
 *
 *  @param         obj                 the object
 *  @param         name_type           the text type
 *  @param         flags
 *                                     - 0: get object infos
 *                                     - 1: get object type infos
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/14 (Oyranos: 0.1.10)
 *  @date    2009/09/15
 */
const char * oyStruct_GetText        ( oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;
  oyOBJECT_e type = oyOBJECT_NONE;

  if(!error)
    text = oyObject_GetName( obj->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    type = obj->type_;

    if(type)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( 0, api_reg, oyOBJECT_CMM_API9_S, 0, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

        j = 0;
        while( cmm_api9->object_types && cmm_api9->object_types[j] &&
               cmm_api9->object_types[j]->type == oyOBJECT_CMM_DATA_TYPES_S &&
               cmm_api9->object_types[j]->oyCMMobjectGetText &&
               cmm_api9->object_types[j]->id == obj->type_ )
        {
          text = cmm_api9->object_types[j]->oyCMMobjectGetText( flags ? 0 : obj,
                                                   name_type, 0 );
          if(text)
            break;
          ++j;
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );

        if(text)
          break;
      }
      oyCMMapiFilters_Release( &apis );
    }
  }

  if(!error && !text)
    text = oyStructTypeToText( obj->type_ );

  return text;
}

/** Function oyStructTypeToText
 *  @brief   Objects type to small string
 *
 *  Give a basic description of inbuild object types.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/12/28
 */
const char *     oyStructTypeToText  ( oyOBJECT_e          type )
{
  const char * text = "unknown";

  switch(type) {
    case oyOBJECT_NONE: text = "Zero - none"; break;
    case oyOBJECT_OBJECT_S: text = "oyObject_s"; break;
    case oyOBJECT_MONITOR_S: text = "oyMonitor_s"; break;
    case oyOBJECT_NAMED_COLOUR_S: text = "oyNamedColour_s"; break;
    case oyOBJECT_NAMED_COLOURS_S: text = "oyNamedColours_s"; break;
    case oyOBJECT_PROFILE_S: text = "oyProfile_s"; break;
    case oyOBJECT_PROFILE_TAG_S: text = "oyProfileTag_s"; break;
    case oyOBJECT_PROFILES_S: text = "oyProfiles_s"; break;
    case oyOBJECT_OPTION_S: text = "oyOption_s"; break;
    case oyOBJECT_OPTIONS_S: text = "oyOptions_s"; break;
    case oyOBJECT_RECTANGLE_S: text = "oyRectangle_s"; break;
    case oyOBJECT_IMAGE_S: text = "oyImage_s"; break;
    case oyOBJECT_ARRAY2D_S: text = "oyArray2d_s"; break;
    case oyOBJECT_COLOUR_CONVERSION_S: text = "oyColourConversion_s";break;
    case oyOBJECT_FILTER_CORE_S: text = "oyFilterCore_s"; break;
    case oyOBJECT_FILTER_CORES_S: text = "oyFilterCores_s"; break;
    case oyOBJECT_CONVERSION_S: text = "oyConversion_s"; break;
    case oyOBJECT_CONNECTOR_S: text = "oyConnector_s"; break;
    case oyOBJECT_CONNECTOR_IMAGING_S: text = "oyConnectorImaging_s"; break;
    case oyOBJECT_FILTER_PLUG_S: text = "oyFilterPlug_s"; break;
    case oyOBJECT_FILTER_PLUGS_S: text = "oyFilterPlugs_s"; break;
    case oyOBJECT_FILTER_SOCKET_S: text = "oyFilterSocket_s"; break;
    case oyOBJECT_FILTER_NODE_S: text = "oyFilterNode_s"; break;
    case oyOBJECT_FILTER_NODES_S: text = "oyFilterNodes_s"; break;
    case oyOBJECT_FILTER_GRAPH_S: text = "oyFilterGraph_s"; break;
    case oyOBJECT_PIXEL_ACCESS_S: text = "oyPixelAccess_s"; break;
    case oyOBJECT_CMM_HANDLE_S: text = "oyCMMhandle_s"; break;
    case oyOBJECT_CMM_PTR_S: text = "oyCMMptr_s"; break;
    case oyOBJECT_CMM_INFO_S: text = "oyCMMInfo_s"; break;
    case oyOBJECT_CMM_API_S: text = "oyCMMapi_s generic"; break;
    case oyOBJECT_CMM_APIS_S: text = "oyCMMapis_s generic"; break;
    case oyOBJECT_CMM_API1_S: text = "oyCMMapi1_s old CMM"; break;
    case oyOBJECT_CMM_API2_S: text = "oyCMMapi2_s Monitors"; break;
    case oyOBJECT_CMM_API3_S: text = "oyCMMapi3_s Profile tags"; break;
    case oyOBJECT_CMM_API4_S: text = "oyCMMapi4_s Filter"; break;
    case oyOBJECT_CMM_API5_S: text = "oyCMMapi5_s MetaFilter"; break;
    case oyOBJECT_CMM_API6_S: text = "oyCMMapi6_s Context convertor"; break;
    case oyOBJECT_CMM_API7_S: text = "oyCMMapi7_s Filter run"; break;
    case oyOBJECT_CMM_API8_S: text = "oyCMMapi8_s Devices"; break;
    case oyOBJECT_CMM_API9_S: text = "oyCMMapi9_s Graph Policies"; break;
    case oyOBJECT_CMM_API10_S: text = "oyCMMapi10_s generic command"; break;
    case oyOBJECT_CMM_DATA_TYPES_S: text = "oyCMMDataTypes_s Filter"; break;
    case oyOBJECT_CMM_API_FILTERS_S: text="oyCMMapiFilters_s Filter list";break;
    case oyOBJECT_CMM_API_MAX: text = "not defined"; break;
    case oyOBJECT_ICON_S: text = "oyIcon_s"; break;
    case oyOBJECT_MODULE_S: text = "oyModule_s"; break;
    case oyOBJECT_EXTERNFUNC_S: text = "oyExternFunc_s"; break;
    case oyOBJECT_NAME_S: text = "oyName_s"; break;
    case oyOBJECT_COMP_S_: text = "oyComp_s_"; break;
    case oyOBJECT_FILE_LIST_S_: text = "oyFileList_s_"; break;
    case oyOBJECT_HASH_S: text = "oyHash_s"; break;
    case oyOBJECT_STRUCT_LIST_S: text = "oyStructList_s"; break;
    case oyOBJECT_BLOB_S: text = "oyBlob_s"; break;
    case oyOBJECT_CONFIG_S: text = "oyConfig_s"; break;
    case oyOBJECT_CONFIGS_S: text = "oyConfigs_s"; break;
    case oyOBJECT_UI_HANDLER_S: text = "oyUiHandler_s"; break;
    case oyOBJECT_FORMS_ARGS_S: text = "oyFormsArgs_s"; break;
    case oyOBJECT_CALLBACK_S: text = "oyCallback_s"; break;
    case oyOBJECT_OBSERVER_S: text = "oyObserver_s"; break;
    case oyOBJECT_CONF_DOMAIN_S: text = "oyConfDomain_s"; break;
    case oyOBJECT_MAX: text = "Max - none"; break;
  }

  return text;
}

/** @internal
 *  Function oyStruct_TypeToText
 *  @brief   Objects type to small string
 *  @deprecated
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
const char * oyStruct_TypeToText     ( const oyStruct_s  * st )
{ return oyStructTypeToText( st->type_ ); }


/* Locking function definitions { */
oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj )    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyLockDummy_                ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyUnLockDummy_              ( oyPointer         look,
                                         const char      * marker,
                                         int               line ) {;}

oyStruct_LockCreate_f   oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockRelease_f         oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLock_f                oyLockFunc_        = oyLockDummy_;
oyUnLock_f              oyUnLockFunc_      = oyUnLockDummy_;
/* } Locking function definitions */
