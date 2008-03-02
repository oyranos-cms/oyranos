// translate with:
// c++ -Wall -g dE2000_test.cpp -o dE2000_test2 icc_utils.o icc_formeln.o icc_helfer.o fl_i18n.o icc_speicher.o

#include "icc_formeln.h"
#include "icc_utils.h"
#include "ciede2000testdata.h"


int
main ()
{
  DBG_PROG_START

  Lab quelle, ziel; 
  double de00;

  for (int i = 0 ; i < cietest_; i++) {
    FarbeZuDouble (&quelle, &cietest[i][0]);
    FarbeZuDouble (&ziel, &cietest[i][3]);
    de00 =  dE2000(quelle, ziel, 1.0,1.0,1.0);
    cout << i << ": " << de00 << " - " << cietest[i][6] << " = " << de00 - cietest[i][6] << endl;
  }

  quelle.L =  54.44073903;
  quelle.a = -34.82222743;
  quelle.b =  0.18986800;
  ziel.L =  54.92340686;
  ziel.a = -33.45703125;
  ziel.b =  0.00000000;
  de00 =  dE2000(quelle, ziel, 1.0,1.0,1.0);
  cout << de00 << " - " << "Test = " << de00 << endl;

  DBG_PROG_ENDE
  return 0;
}
