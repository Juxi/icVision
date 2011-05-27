//#include <iostream>
//#include <cstdlib>
#include "envision_lib.h"

//using namespace std;

int main(int argc, const char** argv)
{ 
  char* instem = argv[1];
  char* outstem = argv[2];
  const int first = atoi(argv[3]);
  const int last = atoi(argv[4]);
  
  //cout << "HELLO!" << endl;
  
  envision_init();
  for(int x=first; x<last; x++)
    {
      float redbias=0, greenbias=1, bluebias=1, yellowbias=1;
      /*envision_nextpic_bias(x, instem, outstem, redbias, 
	greenbias, bluebias, yellowbias);*/
      envision_nextpic(x, instem, outstem);
    }
  envision_cleanup();
  //cout << "DONE!" << endl;
  return 0;
}
