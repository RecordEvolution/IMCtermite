//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../lib/raweat.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
  std::cout<<"number of CLI-arguments: "<<argc<<"\n";
  for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";
  std::cout<<"\n";

  // get name/path of file from CLI argument
  std::string rawfile(argv[1]);

  // declare instance of "raw_eater"
  raw_eater eatraw(rawfile);

  // // show structure of data
  // eatraw.show_buffer(32);
  //
  // // display predefined markers
  // eatraw.show_markers();

  std::cout<<"\n";
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

   std::vector<std::string> segvec = eatraw.get_segment(mrk.first);
   std::cout<<"number of elements in segment: "<<segvec.size()<<"\n\n";
   //for ( auto el: segvec ) std::cout<<el<<"\n";
  }

  // get array of encoded data
  std::vector<double> mydata = eatraw.get_data();
  std::cout<<"\nsize of data array: "<<mydata.size()<<"\n";
  std::cout<<"\ndata header:\n";
  for ( unsigned long int i = 0; i < 10 && i < mydata.size(); i++ )
  {
    std::cout<<mydata[i]<<"\n";
  }
  std::cout<<"\n";

  // write data in csv-file
  if ( argc == 3 )
  {
    std::cout<<"writing data into file "<<argv[2]<<"\n";
    eatraw.write_data(std::string(argv[2]));
  }

  return 0;
}

//---------------------------------------------------------------------------//
