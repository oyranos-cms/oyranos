#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <oyranos_cmm.h>
#include <oyranos_alpha.h>

static const char *oyValueTypeText( oyVALUETYPE_e       type )
{
  switch(type)
  {
  case oyVAL_INT:         return "Int";
  case oyVAL_DOUBLE:      return "Double";
  case oyVAL_INT_LIST:    return "IntList";
  case oyVAL_DOUBLE_LIST: return "DoubleList";
  case oyVAL_STRING:      return "String";
  case oyVAL_STRING_LIST: return "StringList";
  case oyVAL_STRUCT:      return "Struct";
  }
  return 0;
}

static void print_option(oyOption_s * opt, int j)
{
   int id = oyOption_GetId(opt);
   oyStruct_s *opt_struct = oyOption_StructGet( opt, oyOBJECT_NONE );
   if (opt_struct) {
      oyCMMptr_s *cmm = NULL;
      oyBlob_s *blob = NULL;
      switch (opt_struct->type_) {
         case oyOBJECT_CMM_PTR_S:
            cmm = (oyCMMptr_s*)opt_struct;
            if (strcmp(cmm->lib_name,"SANE") == 0)
               printf("\t\t[%s]\n\t\tOption[%d] ID=%d\tCMMptr{%p,%s}\n\n",
                      oyOption_GetRegistration(opt), j, id, cmm->ptr,
                      cmm->lib_name);
            break;
         case oyOBJECT_BLOB_S:
            blob = (oyBlob_s *)opt_struct;
            printf("\t\t[%s]\n\t\tOption[%d] ID=%d\tblob{%p,%d}\n\n",
                   oyOption_GetRegistration(opt), j, id, blob->ptr,
                   (int)blob->size);
            break;
         default:
            printf("\t\tCan't handle struct of type %d\n", opt_struct->type_);
            break;
      }
   } else {
      char *text = oyOption_GetValueText(opt, malloc);
      const char *type = "";/* FIXME: oyValueTypeText(opt->value_type); */
      printf("\t\t[%s]\n\t\tOption[%d] ID=%d\t%s{%s}\n\n",
             oyOption_GetRegistration(opt), j, id, type, text);
      free(text);
   }
}

static int print_options(oyOptions_s * options)
{
   int j;
   int num_options = oyOptions_Count(options);
   printf("Found %d option%s\n", num_options, num_options > 1 ? "s" : "");
   for (j = 0; j < num_options; j++) {
      oyOption_s *opt = oyOptions_Get(options, j);
      print_option(opt, j);
      oyOption_Release(&opt);
   }

   return num_options;
}

static void print_device(oyConfig_s *device)
{
   int j;
   int num_options = oyConfig_Count(device);
   printf("\tFound %d option%s for device\n", num_options, num_options > 1 ? "s" : "");
   for (j = 0; j < num_options; j++) {
      oyOption_s *opt = oyConfig_Get(device, j);
      print_option(opt, j);
      oyOption_Release(&opt);
   }
}

static int print_devices(oyConfigs_s * devices, const char *name)
{
   int i,j;
   int num_devices = oyConfigs_Count(devices);
   printf("Found %d %s device%s\n", num_devices, name, num_devices > 1 ? "s" : "");

   for (i = 0; i < num_devices; i++) {
      oyConfig_s *device = oyConfigs_Get(devices, i);
      int num_options = oyConfig_Count(device);
      printf("\tFound %d option%s for device %d\n", num_options, num_options > 1 ? "s" : "", i);
      for (j = 0; j < num_options; j++) {
         oyOption_s *opt = oyConfig_Get(device, j);
         print_option(opt, j);
         oyOption_Release(&opt);
      }
      oyConfig_Release(&device);
   }
   return num_devices;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
