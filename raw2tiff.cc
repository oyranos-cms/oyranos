#include <string>
#include <fstream>

#include "RAW.hh"

using namespace std;

int main(int argc, char *argv[])
{
   if (argc < 2) {
      cerr << "Usage: " << argv[0] << " <raw image> [-i <icc_profile>]" << endl;
      return 1;
   }
   string profile;
   if (argc == 4 && argv[2][0] == '-' && argv[2][1] == 'i')
      profile = argv[3];

   ofstream exif("exif.txt");

   RAW raw(argv[1], profile);
   raw.print_libraw_version();
   raw.open();
   raw.print_dcraw_settings(std::cout);
   raw.print_exif_data(exif);
   raw.open_profile();
   raw.save_tiff();

   raw.GetColorInfo();

   return 0;
}
