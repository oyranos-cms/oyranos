#include <fstream>

#include "RAW.hh"

using namespace std;

int main( int argc, char *argv[] ) {
	if (argc<2) {
		cerr<<"Usage: "<<argv[0]<<" <raw image>"<<endl;
		return 1;
	}

	std::ofstream exif("exif.txt");

	RAW raw( argv[1] );
	raw.print_libraw_version();
	raw.open();
	raw.print_dcraw_settings( std::cout );
	raw.print_exif_data( exif );

	return 0;
}
