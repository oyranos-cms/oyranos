#include <iomanip>
#include <fstream>

#include "RAW.hh"
#include "helper.c"

using std::string;
using std::ifstream;
using std::cerr;
using std::endl;
using std::ios;
using namespace oyranos;

RAW::RAW() :
   opened(false),
   rip(),
   imageRGB(NULL),
   filename(),
   imageExif(NULL),
   icc_profile(NULL), icc_profile_bytes(0), icc_profile_name(),
   version_num(0), version_str(NULL), oy_device(NULL)
{}

RAW::RAW(const string & raw, const string & icc) :
   opened(false),
   rip(),
   imageRGB(NULL),
   filename(raw),
   imageExif(NULL),
   icc_profile(NULL), icc_profile_bytes(0), icc_profile_name(icc),
   version_num(0), version_str(NULL), oy_device(NULL)
{}

RAW::~RAW()
{
   free(imageRGB);
   free(icc_profile);
   oyConfig_Release(&oy_device);
}

void RAW::release_members()
{
   free(imageRGB);
   free(icc_profile);
   rip.recycle();
   if (filename != "")
      filename = "";
   imageExif.reset();
}

void RAW::load_exif()
{
   imageExif = Exiv2::ImageFactory::open(filename.c_str());
   imageExif->readMetadata();
}

void RAW::open(const string & filename_)
{
   //if (filename_ == "" && filename != "");
   if (filename_ == "" && filename == "")
      return;
   if (filename_ != "")
      if (filename == "")
         filename = filename_;
      else {
         release_members();
         filename = filename_;
      }
   //EXIF
   //First handle exif data
   load_exif();
   cerr << "first exif tag is: " << imageExif->exifData().begin()->tagName() << endl;

   //DCRAW
   //Init 'rip' and store metadata to rip.imgdata
   rip.open_file(filename.c_str());
   cerr << "open " << filename << " [" << rip.imgdata.sizes.width << "," << rip.imgdata.sizes.height << "]" << endl;

   //extract Bayer pattern from file to rip.imgdata.image
   rip.unpack();
   cerr << "RGBG[0]=["
       << rip.imgdata.image[0][0] << ","
       << rip.imgdata.image[0][1] << "," << rip.imgdata.image[0][2] << "," << rip.imgdata.image[0][3] << "]" << endl;

   //extract thumbnail to rip.imgdata.thumbnail.thumb
   rip.unpack_thumb();
   cerr << "thumbnail is [" << rip.imgdata.thumbnail.twidth << "x" << rip.imgdata.thumbnail.theight << "]" << endl;

   rip.imgdata.params.output_bps = 16;
   //interpolate the Bayer pattern,
   //using dcraw compatible options in rip.imgdata.params
   rip.dcraw_process();

   //copy the rgb image data to imageRGB.data
   imageRGB = rip.dcraw_make_mem_image();
   cerr << "final image is [" << imageRGB->width << "x" << imageRGB->height << "]@" << imageRGB->
       bits << "bits with " << imageRGB->colors << " colors" << endl;
}

void RAW::get_color_info()
{
   cerr<<endl<<"get_color_info():"<<endl;

   oyConfigs_s *devices = NULL;
   //1.  Query Oyranos oyRE backend for all avaliable devices
   //    Use the "command" -> "list" option
   //    plus the driver(LibRaw) version
   oyOptions_s *list_options = oyOptions_New(0);

   oyOptions_SetFromText(&list_options, CMM_BASE_REG OY_SLASH "command", "list", OY_CREATE_NEW);
   /*The value is not used for the input option, just put `0'*/
   oyOptions_SetFromInt(&list_options, CMM_BASE_REG OY_SLASH "driver_version", 0, 0, OY_CREATE_NEW);

   printf("Sending the following options to Oyranos\n"); //DBG
   print_options(list_options); //DBG

   /*Now call Oyranos*/
   if (oyDevicesGet(OY_TYPE_STD, "raw-image", list_options, &devices) != 0)
      exit(1);
   if (!devices)
      exit(1);
   oyOptions_Release( &list_options );

   printf("Got the following devices from Oyranos\n"); //DBG
   print_devices(devices, "raw-image"); //DBG

   /*Get LibRAw version from the first(and only) device*/
   oyConfig_s *device = oyConfigs_Get(devices, 0);
   oyOptions_FindInt(device->data, "driver_version_num", 0, &version_num);
   version_str = oyOptions_FindString(device->data, "driver_version_num", NULL);
   cerr << "LibRaw, version: " << rip.version() << " [" << rip.versionNumber() << "]" << endl;

   oyConfig_Release(&device);
   oyConfigs_Release(&devices);

   //2.  Get the relevant color information from Oyranos
   //    This is the "command" -> "properties" call
   device = oyConfig_New(CMM_BASE_REG, 0);
   //A device_name option has to be present... (for Configs_Modify)
   oyOptions_SetFromText(&device->backend_core, CMM_BASE_REG OY_SLASH "device_name", "dummy", OY_CREATE_NEW);

   oyOptions_s *options = oyOptions_New(0);
   //Request the properties call
   oyOptions_SetFromText(&options, CMM_BASE_REG OY_SLASH "command", "properties", OY_CREATE_NEW);
   oyOptions_SetFromText(&options, CMM_BASE_REG OY_SLASH "device_name", "dummy", OY_CREATE_NEW);
   //Pass in the filename
   oyOptions_SetFromText(&options, CMM_BASE_REG OY_SLASH "device_handle", filename.c_str(), OY_CREATE_NEW);
   //Pass in the libraw object with the raw image rendering options
   oyOption_s *context_opt = oyOption_New(CMM_BASE_REG OY_SLASH "device_context", 0);
   libraw_output_params_t *device_context = &rip.imgdata.params;
   oyOption_SetFromData(context_opt, (oyPointer)&device_context, sizeof(libraw_output_params_t*));
   oyOptions_MoveIn(options, &context_opt, -1);
   oyOption_Release(&context_opt);

   printf("Sending the following options to Oyranos\n"); //DBG
   print_options(options); //DBG

   /*Call Oyranos*/
   oyDeviceGet(OY_TYPE_STD, "raw-image", "dummy", options, &device);
   //oyDeviceBackendCall(device, options);
   printf("Oyranos returns the following colour related options.\n"); //DBG
   print_device(device); //DBG

   oy_device = oyConfig_Copy(device, 0);
   oyConfig_Release(&device);
   oyOptions_Release(&options);
}

void RAW::open(const char *filename)
{
   open(string(filename));
}

void RAW::open_profile()
{
   if (icc_profile_name == "")
      return;
   ifstream file(icc_profile_name.c_str(), ios::in | ios::binary | ios::ate);
   if (!file) {
      cerr<<"Can't open file \""<<icc_profile_name<<"\""<<endl;
      return;
   }
   icc_profile_bytes = file.tellg();
   icc_profile = new char[icc_profile_bytes];
   file.seekg(0, ios::beg);
   file.read(icc_profile, icc_profile_bytes);
   file.close();
}

void RAW::get_oyranos_profile()
{
   oyProfile_s* profile = NULL;
   oyDeviceGetProfile(oy_device, &profile);
   const char* tmp = oyProfile_GetFileName(profile, -1);

   printf("Found profile: \"%s\"\n", tmp?tmp:"---");

   if (tmp)
      icc_profile_name = tmp;
}

#include <tiffio.h>
void RAW::save_tiff()
{
   string tiffile = filename + ".tiff";
   TIFF *img = TIFFOpen(tiffile.c_str(), "w");

   uint32 width = rip.imgdata.sizes.iwidth,
       height = rip.imgdata.sizes.iheight, spp = 3, bps = rip.imgdata.params.output_bps;
   TIFFSetField(img, TIFFTAG_IMAGEDESCRIPTION, "GSoC 2009 test image");
   TIFFSetField(img, TIFFTAG_IMAGEWIDTH, width);
   TIFFSetField(img, TIFFTAG_IMAGELENGTH, height);
   TIFFSetField(img, TIFFTAG_SAMPLESPERPIXEL, spp);
   TIFFSetField(img, TIFFTAG_BITSPERSAMPLE, bps);
   TIFFSetField(img, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
   TIFFSetField(img, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
   TIFFSetField(img, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
   if (icc_profile)
      TIFFSetField(img, TIFFTAG_ICCPROFILE, icc_profile_bytes, icc_profile);

   tsize_t row_bytes = TIFFScanlineSize(img);
   for (int h = 0; h < height; h++)
      TIFFWriteScanline(img, ((uint8 *) imageRGB->data) + h * row_bytes, h);

   TIFFClose(img);
}

void RAW::print_dcraw_settings(std::ostream & out)
{
   //libraw_output_params_t holds all dcraw options
   //rip.imgdata.params
   out << "\n<--- dcraw params: {R}epair {C}olor {I}nterpolation {O}utput --->" << endl;

   out << "\noptions affecting open_file()" << endl;
   /////////////////////////////////////////////
   out << "\tuser_flip = " << rip.imgdata.params.user_flip << " -> [noop]" << endl;

   out << "\noptions affecting unpack()" << endl;
   //////////////////////////////////////////
   out << "\tuse_camera_wb = " << rip.imgdata.params.use_camera_wb << " -> [-w] {C}" << endl;
   out << "\tuse_camera_matrix = " << rip.imgdata.params.use_camera_matrix << " -> [+M/-M] {C}" << endl;
   out << "\tshot_select = " << rip.imgdata.params.shot_select << " -> [-s] {O}" << endl; //or open_file()??
   out << "\thalf_size = " << rip.imgdata.params.half_size << " -> [-h] {I}" << endl;  //affects data loading
   //out<<"\tfiltering_mode = "<<rip.imgdata.params.filtering_mode<<" -> [LibRaw_filtering]"<<endl;
   out << "\tthreshold = " << rip.imgdata.params.threshold << " -> [-n] {R}" << endl;
   out << "\taber[4] = ["
       << rip.imgdata.params.aber[0] << ","
       << rip.imgdata.params.aber[1] << "," << rip.imgdata.params.aber[2] << "," << rip.imgdata.params.aber[3]
       << "] -> [-C] {R}" << endl;

   out << "\noptions affecting dcraw_process()" << endl;
   /////////////////////////////////////////////////
   out << "\tgreybox[4] = ["
       << rip.imgdata.params.greybox[0] << ","
       << rip.imgdata.params.greybox[1] << "," << rip.imgdata.params.greybox[2] << "," << rip.imgdata.params.greybox[3]
       << "] -> [-A  x1 y1 x2 y2] {C}" << endl;

   out << "\tgamm[6] = ["
       << rip.imgdata.params.gamm[0] << ","
       << rip.imgdata.params.gamm[1] << ","
       << rip.imgdata.params.gamm[2] << ","
       << rip.imgdata.params.gamm[3] << "," << rip.imgdata.params.gamm[4] << "," << rip.imgdata.params.gamm[5]
       << "] -> [-g] {C}" << endl;

   out << "\tuser_mul[4] = ["
       << rip.imgdata.params.user_mul[0] << ","
       << rip.imgdata.params.user_mul[1] << ","
       << rip.imgdata.params.user_mul[2] << "," << rip.imgdata.params.user_mul[3]
       << "] -> [-r mul0 mul1 mul2 mul3] {C}" << endl;

   out << "\tbright = " << rip.imgdata.params.bright << " -> [-b] {O}" << endl;
   out << "\tfour_color_rgb = " << rip.imgdata.params.four_color_rgb << " -> [-f] {I}" << endl;
   out << "\thighlight = " << rip.imgdata.params.highlight << " -> [-H] {R}" << endl;
   out << "\tuse_auto_wb = " << rip.imgdata.params.use_auto_wb << " -> [-a] {C}" << endl;
   out << "\toutput_color = " << rip.imgdata.params.output_color << " -> [-o] {C}" << endl;
   //out<<"\toutput_profile = "<<rip.imgdata.params.output_profile<<" -> [-o]"<<endl;
   //out<<"\tcamera_profile = "<<rip.imgdata.params.camera_profile<<" -> [-p] {C}"<<endl;
   //out<<"\tbad_pixels = "<<rip.imgdata.params.bad_pixels<<" -> [-P] {R}"<<endl;
   //out<<"dark_frame = "<<rip.imgdata.params.dark_frame <<" -> [-K] {R}"<<endl;
   out << "\toutput_bps = " << rip.imgdata.params.output_bps << " -> [-4] {O}" << endl;
   out << "\toutput_tiff = " << rip.imgdata.params.output_tiff << " -> [-T] {O}" << endl;
   out << "\tuser_flip = " << rip.imgdata.params.user_flip << " -> [-t] {O}" << endl;
   out << "\tuser_qual = " << rip.imgdata.params.user_qual << " -> [-q] {I}" << endl;
   out << "\tuser_black = " << rip.imgdata.params.user_black << " -> [-k] {R}" << endl;
   out << "\tuser_sat = " << rip.imgdata.params.user_sat << " -> [-S] {R}" << endl;
   out << "\tmed_passes = " << rip.imgdata.params.med_passes << " -> [-m] {I}" << endl;
   out << "\tauto_bright_thr = " << rip.imgdata.params.auto_bright_thr << " -> [noop]" << endl;
   out << "\tno_auto_bright = " << rip.imgdata.params.no_auto_bright << " -> [-W] {O}" << endl;
   out << "\tuse_fuji_rotate = " << rip.imgdata.params.use_fuji_rotate << " -> [-j] {O}" << endl;

   out << "\noptions not currently used by LibRaw" << endl;
   ////////////////////////////////////////////////////
   out << "\tdocument_mode = " << rip.imgdata.params.document_mode << " -> [-d/-D] {I}" << endl;
}

void RAW::print_exif_data(std::ostream & out)
{
   if (imageExif.get() == 0)
      load_exif();

   Exiv2::ExifData::const_iterator end = imageExif->exifData().end();
   Exiv2::ExifData::const_iterator i = imageExif->exifData().begin();
   //copy-paste from exifprint.cpp:
   for (; i != end; ++i) {
      const char *tn = i->typeName();
      out << std::setw(44) << std::setfill(' ') << std::left
          << i->key() << " "
          << "0x" << std::setw(4) << std::setfill('0') << std::right
          << std::hex << i->tag() << " "
          << std::setw(9) << std::setfill(' ') << std::left << (tn ? tn : "Unknown") << " " << std::dec << std::setw(3)
          << std::setfill(' ') << std::right << i->count() << "  " << std::dec << i->value()
          << "\n";
   }
}

void RAW::print_libraw_version()
{
   cerr << "LibRaw, version: " << rip.version() << " [" << rip.versionNumber() << "]" << endl;
}
