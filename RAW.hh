#include <iostream>
#include <string>

#include <exiv2/image.hpp>
#include <exiv2/exif.hpp>

#include "libraw/libraw.h"

#include <oyranos_alpha.h>
#define CMM_BASE_REG "//imaging/config.raw-image.oyRE"

class RAW {
 private:
   bool opened;                           ///< is the file opened?
   LibRaw rip;                            ///< raw image processor
   libraw_processed_image_t *imageRGB;    ///< struct with image data
   std::string filename;                  ///< Filename
   Exiv2::Image::AutoPtr imageExif;       ///< pointer to image with exif data
   char *icc_profile;                     ///< pointer to ICC profile
   unsigned icc_profile_bytes;            ///< size of profile in bytes
   std::string icc_profile_name;          ///< size of profile in bytes
   int version_num;                       ///< LibRaw version number
   const char *version_str;               ///< LibRaw version string

   /// Resets data members
   void release_members();
   /// Load exif data
   void load_exif();

 public:
   RAW();
   RAW(const std::string &raw, const std::string &icc = "");

   /// Open raw file and store RGB data to memmory
   void open(const std::string & filename = "");
   /// Open raw file and store RGB data to memmory
   /// Wrapper function.
   void open(const char *filename);
   //void open( const void * const buffer, size_t bufsize );
   //void open( libgphoto camera );
   /// Open an icc profile to memmory
   /// No cind of verification is done yet.
   void open_profile();

   /// Use Oyranos raw-image backend
   void GetColorInfo();

   /// Save image as tiff.
   void save_tiff();

   /// Print all settings that affect image read/unpack/debayer methods
   void print_dcraw_settings(std::ostream & out);
   /// Print all possible exif information found
   void print_exif_data(std::ostream & out);
   /// Print the LibRaw version
   void print_libraw_version();

   ~RAW();
};
