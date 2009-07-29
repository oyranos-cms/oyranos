
static char **color_option_names = NULL;
static char **color_option_values = NULL;

#define BUFSIZE 1024
static unsigned int print_int_vec(const SANE_Int * val, SANE_Int size, char *buf, unsigned buf_size)
{
   char *buf_start = buf;
   unsigned int pos = 0, chars = 0;
   int chars_left = buf_size;
   SANE_Int i, num = size / (SANE_Int) sizeof(SANE_Word);

   for (i = 0; i < num; i++) {
      chars = snprintf(buf += chars, chars_left, "%d,", *(SANE_Int *) val + i);
      chars_left -= chars;
      /* printf("i=%d, chars=%d, chars_left=%d\n", i,chars,chars_left); */
      if (chars_left <= 0 && i < (num - 1))
         return buf_size;       /* -1? */
   }
   buf[chars - 1] = '\0';       /* erase last `,' */
   /* printf("i=%d, chars=%d, chars_left=%d: %s\n", i,chars,chars_left,buf_start); */
   return buf_size - chars_left;
}

static unsigned int print_fixed_vec(const SANE_Fixed * val, SANE_Int size, char *buf, unsigned buf_size)
{
   char *buf_start = buf;
   unsigned int pos = 0, chars = 0;
   int chars_left = buf_size;
   SANE_Int i, num = size / (SANE_Int) sizeof(SANE_Word);

   for (i = 0; i < num; i++) {
      chars = snprintf(buf += chars, chars_left, "%f,", SANE_UNFIX(*(SANE_Fixed *) val + i));
      chars_left -= chars;
      /* printf("i=%d, chars=%d, chars_left=%d\n", i,chars,chars_left); */
      if (chars_left <= 0 && i < (num - 1))
         return buf_size;       /* -1? */
   }
   buf[chars - 1] = '\0';       /* erase last `,' */
   /* printf("i=%d, chars=%d, chars_left=%d: %s\n", i,chars,chars_left,buf_start); */
   return buf_size - chars_left;
}

static void OyInit_color_options(SANE_Handle device)
{
   const SANE_Option_Descriptor *opt = NULL;
   SANE_Int num_options = 0;
   SANE_Status status;
   unsigned int opt_num = 0, i = 0, chars = 0;
   char buf[BUFSIZE];

   /* We got a device, find out how many options it has */
   status = sane_control_option(device, 0, SANE_ACTION_GET_VALUE, &num_options, 0);
   if (status != SANE_STATUS_GOOD) {
      fprintf(stderr, "unable to determine option count\n");
      exit(1);
   }

   color_option_names = (char **)malloc(sizeof(char *) * num_options);
   color_option_values = (char **)malloc(sizeof(char *) * num_options);
   memset(color_option_names, 0, sizeof(char *) * num_options);
   memset(color_option_values, 0, sizeof(char *) * num_options);

   for (opt_num = 1; opt_num < num_options; opt_num++) {
      opt = sane_get_option_descriptor(device, opt_num);
      if (opt->cap & SANE_CAP_COLOUR) {
         void *val = malloc(opt->size);

         color_option_names[i] = (char *)malloc(strlen(opt->name) + 1);
         strcpy(color_option_names[i], opt->name);

         sane_control_option(device, opt_num, SANE_ACTION_GET_VALUE, val, 0);
         switch (opt->type) {
            case SANE_TYPE_BOOL:
               color_option_values[i] = *(SANE_Bool *) val ? "true" : "false";
               break;
            case SANE_TYPE_INT:
               if (opt->size == (SANE_Int) sizeof(SANE_Word))
                  chars = sprintf(buf, "%d", *(SANE_Int *) val);  /* use snprintf( ..,128,.. ) */
               else
                  chars = print_int_vec((SANE_Int *) val, opt->size, buf, BUFSIZE);
               color_option_values[i] = (char *)malloc(chars + 1);
               strcpy(color_option_values[i], buf);
               break;
            case SANE_TYPE_FIXED:
               chars = sprintf(buf, "%f", SANE_UNFIX(*(SANE_Fixed *) val));
               color_option_values[i] = (char *)malloc(chars + 1);
               strcpy(color_option_values[i], buf);
               break;
            case SANE_TYPE_STRING:
               color_option_values[i] = (char *)malloc(strlen((char *)val) + 1);
               strcpy(color_option_values[i], (char *)val);
               break;
            case SANE_TYPE_BUTTON:
               color_option_values[i] = "button";
               break;
            default:
               fprintf(stderr, "Do not know what to do with option %d\n", opt->type);
               exit(0);
               break;
         }
         i++;
      }
   }
}

void OyPrint_color_options(void)
{
   int i = 0;
   printf("Options actually affecting color:\n");
   while (color_option_names[i]) {
      printf("\t%s -> %s\n", color_option_names[i], color_option_values[i]);
      i++;
   }
   putchar('\n');
}
