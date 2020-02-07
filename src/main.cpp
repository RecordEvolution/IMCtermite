//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../src/raweat.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
  // path of filename provided ?
  assert( argc > 1 && "please provide a filename and path" );

  std::cout<<"number of CLI-arguments: "<<argc<<"\n";
  for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";

  // check number of CLI arguments
  assert( argc == 2 );

  // get name/path of file from CLI argument
  std::string rawfile(argv[1]);

  // declare instance of "raw_eater"
  raw_eater eatraw(rawfile);

  eatraw.show_markers();
  eatraw.find_markers();

  std::map<std::string,std::vector<unsigned char>> marks = eatraw.get_markers();
  for ( auto mrk: marks )
  {
    // get data
    std::vector<unsigned char> dat = eatraw.get_marker_data(mrk.first);

    // print marker name, length and data
    std::cout<<mrk.first<<" : "<<dat.size()<<'\n';
    std::cout<<std::setfill('-')<<std::setw(96)<<'\n'<<std::setfill(' ');
    eatraw.show_hex(dat,32,512);
    std::cout<<"\n";
  }

  eatraw.convert_data();

  std::vector<double> mydata = eatraw.get_data();
  std::cout<<"\n"<<mydata.size()<<"\n\n";
  //for ( unsigned long int i = 0; i < 10; i++ ) std::cout<<mydata[i]<<"\n";

  eatraw.write_data(std::string("csv/myfile.csv"));

  return 0;
}

//---------------------------------------------------------------------------//
