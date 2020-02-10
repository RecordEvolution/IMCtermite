//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../src/raweat.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
  // path of filename provided ?
  assert( argc > 2 && "please provide a filename and path for input and output" );

  std::cout<<"number of CLI-arguments: "<<argc<<"\n";
  for ( int i = 0; i < argc; i++ ) std::cout<<std::setw(5)<<i<<":   "<<argv[i]<<"\n";

  // check number of CLI arguments
  assert( argc == 3 );

  // get name/path of file from CLI argument
  std::string rawfile(argv[1]);

  // declare instance of "raw_eater"
  raw_eater eatraw(rawfile);

  //eatraw.show_markers();

  // determine position of markers in buffer and read data
  eatraw.find_markers();

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
  }

  // convert unsigned char data in buffer to desired data type
  //eatraw.convert_data();
  eatraw.convert_data_16_bit_float();

  // get array of encoded data
  std::vector<double> maindata = eatraw.get_data();
  std::cout<<"\nsize of data array: "<<maindata.size()<<"\n\n";
  //for ( unsigned long int i = 0; i < 10; i++ ) std::cout<<mydata[i]<<"\n";

  // write data in csv-file
  eatraw.write_data(std::string(argv[2]));

  float ab = -0.75;
  std::cout<<ab<<"\n";
  uint8_t* pab = reinterpret_cast<uint8_t*>(&ab);
  for ( int i = 0; i < (int)sizeof(float); i++ ) std::cout<<std::hex<<(int)pab[i]<<std::dec<<" ";
  std::cout<<"\n";

  uint8_t sc = 0x4f;
  std::bitset<8> bssc(sc);
  for ( int i = 0; i < 8; i++ ) std::cout<<bssc[i]<<" ";
  std::cout<<"\n";
  std::cout<<std::hex<<bssc.to_ulong()<<"\n\n";

  endian endi;
  std::cout<<(endi.little_endian()?"little endian":"big endian")<<"\n";

  return 0;
}

//---------------------------------------------------------------------------//
