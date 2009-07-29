#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sane/sane.h>

#include "sane_color_options.c"

SANE_Status status;
SANE_Int version, num_options;
SANE_Handle device_handle;
const SANE_Device **device_list = NULL;
SANE_String_Const device_name = NULL;

char *image_buffer = NULL;
char *icc_profile = NULL;
char *icc_profile_name = NULL;
int icc_profile_bytes;

int width, height, bps, size;

void print_sane_version()
{
   printf("SANE version %d.%d.%d\n\n",
          SANE_VERSION_MAJOR(version), SANE_VERSION_MINOR(version), SANE_VERSION_BUILD(version));
}

/// Initialise the device_list of sane devices
void init()
{
   int i = 0;

   status = sane_init(&version, NULL);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot initialise sane!\n");
      exit(1);
   }

   status = sane_get_devices(&device_list, SANE_FALSE);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot get sane devices!\n");
      exit(1);
   }

   print_sane_version();
   do {
      printf("[Device %d]\n", i);
      printf("name:\t%s\nvendor:\t%s\nmodel:\t%s\ntype:\t%s\n\n", device_list[i]->name, device_list[i]->vendor,
             device_list[i]->model, device_list[i]->type);
      i++;
   } while (device_list[i]);
}

/// Select the sane device to use
void select_device()
{
   if (device_name == NULL)
      device_name = device_list[0]->name;

   printf("%s has been selected\n", device_name);
}

void scan_it()
{
   SANE_Int bytes_left, bytes_written;
   SANE_Option_Descriptor *opt = NULL;
   SANE_Parameters params;

   char *buffer_aux = NULL;

   //1. Open the device
   status = sane_open(device_name, &device_handle);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot open device %s!\n", device_name);
      exit(1);
   }
   //2. Setup the device
   //This stage is not implemented in this simple programm
   status = sane_control_option(device_handle, 0, SANE_ACTION_GET_VALUE, &num_options, NULL);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot count device options!\n");
      exit(1);
   }
   printf("%s has %d options in total.\n", device_name, num_options);

   //An early implementation of color option handling, currently outside Oyranos
   OyInit_color_options(device_handle);
   OyPrint_color_options();
   //3. Acquire the scanned image
   status = sane_start(device_handle);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot start scanning!\n");
      exit(1);
   } else
      printf("sane_start()\n");
   //3.1 Take care of all scan parameters
   status = sane_get_parameters(device_handle, &params);
   if (status != SANE_STATUS_GOOD) {
      printf("Cannot get scanning parameters!\n");
      exit(1);
   }
   if (params.format != SANE_FRAME_RGB) {
      printf("Not interested in non RGB images. Bye\n!");
      exit(1);
   }
   if (params.lines == -1) {
      printf("Number of lines unknown. Bye!\n");
      exit(1);
   }
   height = params.lines;
   bps = params.depth;
   width = params.pixels_per_line;
   size = params.bytes_per_line * params.lines; //Total size of image in bytes
   if (params.bytes_per_line != 3 * (bps / 8) * width) {
      printf("Cannot cope with padded image formats!\n");
      exit(1);
   } else
      printf("ready to read image[%dx%d]@%dbits\n", width, height, bps);
   //3.2 Init image and fill with data.
   bytes_left = size;
   image_buffer = malloc(size);
   buffer_aux = image_buffer;
   if (image_buffer) {
      do {
         status = sane_read(device_handle, buffer_aux, bytes_left, &bytes_written);
         bytes_left -= bytes_written;
         buffer_aux += bytes_written;
         printf("size[%d]: %d bytes written, %d bytes left\n", size, bytes_written, bytes_left);
         if (status == SANE_STATUS_GOOD)
            continue;
         else if (status == SANE_STATUS_EOF && bytes_left == 0)
            break;
         else {
            printf("Something bad happened while reading image data. [status=%d]\n", status);
            if (bytes_left == 0) {
               printf("All seems OK, though. Let's ignore it.\n");
               break;
            } else
               exit(1);
         }
      } while (SANE_TRUE);
   } else {
      printf("Not enough memmory!\n");
      exit(1);
   }
   //3.3 Stop scanning
   sane_cancel(device_handle);

   //4. Close the device
   sane_close(device_handle);
}

#include <tiffio.h>
void save_tiff()
{
   char *tiffile = "out.tiff";
   int spp = 3;
   int h;

   TIFF *img = TIFFOpen(tiffile, "w");

   TIFFSetField(img, TIFFTAG_IMAGEDESCRIPTION, "GSoC 2009 test image");
   TIFFSetField(img, TIFFTAG_IMAGEWIDTH, width);
   TIFFSetField(img, TIFFTAG_IMAGELENGTH, height);
   TIFFSetField(img, TIFFTAG_SAMPLESPERPIXEL, spp);
   TIFFSetField(img, TIFFTAG_BITSPERSAMPLE, bps);
   TIFFSetField(img, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
   TIFFSetField(img, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
   TIFFSetField(img, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
   if (icc_profile) {
      printf("Attaching profile %s\n", icc_profile_name);
      TIFFSetField(img, TIFFTAG_ICCPROFILE, icc_profile_bytes, icc_profile);
   }

   tsize_t row_bytes = TIFFScanlineSize(img);
   for (h = 0; h < height; h++)
      TIFFWriteScanline(img, ((uint8 *) image_buffer) + h * row_bytes, h, 0);

   TIFFClose(img);
   printf("Saved image %s\n", tiffile);
}

void read_profile()
{
   FILE *profile = NULL;

   profile = fopen(icc_profile_name, "r");
   if (profile) {
      fseek(profile, 0L, SEEK_END);
      icc_profile_bytes = ftell(profile);
      rewind(profile);
      icc_profile = malloc(icc_profile_bytes);
      fread(icc_profile, 1, icc_profile_bytes, profile);
      fclose(profile);
   } else {
      printf("Cannot open file %s\n", icc_profile_name);
      exit(1);
   }
}

void cleanup()
{
   free(image_buffer);
   free(icc_profile);
   sane_exit();
}

int main(int argc, char **argv)
{
   int arg;

   init();

   if (argc == 1) {
      printf("\nUsage: %s [-d <device>] [-i [icc profile]]\n", argv[0]);
      printf
          ("<device>:\n\t\tA sane device string\n<icc profile>:\n\t\t(a) A path to an *.ic[cm] file\n\t\t(b) If empty use Oyranos\n",
           argv[0]);
      return 0;
   }
   for (arg = 1; arg < argc; arg++) {
      if (strcmp(argv[arg], "-d") == 0)
         device_name = argv[++arg];
      else if (strcmp(argv[arg], "-i") == 0) {
         arg++;
         if (arg < argc && argv[arg][0] != '-')
            icc_profile_name = argv[arg];
         else
            icc_profile_name = "Oyranos";
      }
   }

   select_device();
   scan_it();
   if (icc_profile_name)
      read_profile();
   save_tiff();
   cleanup();

   return 0;
}
