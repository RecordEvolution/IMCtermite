//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../lib/raweat.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
//  std::cout<<"number of CLI-arguments: "<<argc<<"\n";
//  for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";

//  path of filename provided ?
//  assert( argc > 2 && "please provide a filename and path for input and output" );

//  check number of CLI arguments
//  assert( argc == 3 );

  if ( argc != 3 ) //or ( argc == 1 && std::string(argv[1]) == str::string("--help") )
  {
    std::cout<<"\n"<<"Usage: ./eatit INPUTFILE OUTPUTFILE"<<"\n"
                   <<"Convert any file in imc-format .raw to plain text .csv"<<"\n"
                   <<"Example: ./eatit Druck_THZ_DK.raw druck_thz_dk.csv"<<"\n\n";
    return 0;
  }

  // get name/path of file from CLI argument
  std::string rawfile(argv[1]);

  // declare instance of "raw_eater"
  raw_eater eatraw(rawfile);

  //eatraw.show_markers();

//  std::cout<<"\n";
//  std::map<std::string,std::vector<unsigned char>> marks = eatraw.get_markers();
//  for ( auto mrk: marks )
//  {
//    // get data
//    std::vector<unsigned char> dat = eatraw.get_marker_data(mrk.first);
//
//    // print marker name, length and data
//    std::cout<<mrk.first<<" : "<<dat.size()<<'\n';
//    std::cout<<std::setfill('-')<<std::setw(96)<<'\n'<<std::setfill(' ');
//    eatraw.show_hex(dat,32,512);
//    std::cout<<"\n";
//
//    std::vector<std::string> segvec = eatraw.get_segment(mrk.first);
//    std::cout<<"number of elements in segment: "<<segvec.size()<<"\n\n";
//    //for ( auto el: segvec ) std::cout<<el<<"\n";
//  }

  // get array of encoded data 
//  std::vector<double> maindata = eatraw.get_data();
//  std::cout<<"\nsize of data array: "<<maindata.size()<<"\n\n";
//  for ( unsigned long int i = 0; i < 10; i++ ) std::cout<<mydata[i]<<"\n";

  // write data in csv-file
  eatraw.write_data(std::string(argv[2]));

  return 0;
}

//---------------------------------------------------------------------------//
