//---------------------------------------------------------------------------//

#include <iomanip>
#include <iostream>
#include "../lib/raweat.hpp"
#include "../lib/rawmerge.hpp"

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{

  if ( argc != 3 ) //or ( argc == 1 && std::string(argv[1]) == str::string("--help") )
  {
    std::cout<<"\n"<<"Usage: ./eatit INPUTFILE OUTPUTFILE"<<"\n"
                   <<"Convert any file in imc-format .raw to plain text .csv"<<"\n"
                   <<"Example: ./eatit Druck_THZ_DK.raw druck_thz_dk.csv"<<"\n\n";
    return 0;
  }

  // get name/path of file from CLI argument
  std::string rawfile(argv[1]);

  // declare instance of 'raw_eater' (and decode data)
  raw_eater eatraw; //rawfile,false);

  // set file to be decoded
  eatraw.set_file(rawfile,true);

  // do conversion
  eatraw.setup_and_conversion(false);

  // eatraw.show_markers();

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
  if ( eatraw.get_valid() )
  {
    eatraw.write_table(std::string(argv[2]),' ');
  }
  else
  {
    std::cout<<"=> invalid/corrupt .raw file\n";
  }

//---------------------------------------------------------------------------//

  // declare instance of 'raw_merger'
  raw_merger eatmea;

  // add channels
  eatmea.add_channel(rawfile,false);
  eatmea.add_channel("smp/VehicleSpeed_HS.raw",false);
  // eatmea.add_channel("smp/Mercedes_E-Klasse-2019-08-08/Flex_StWhl_AnglSpd.raw",false);
  // eatmea.add_channel("smp/Rangerover_Evoque_F-RR534_2019-05-07/Temp_Disc_FR.raw",false);

  // write merged table
  if ( eatmea.get_num_channels() > 0 )
  {
    eatmea.write_table_all("output/allchannels.csv",' ');
  }
  else
  {
    std::cout<<"no valid channel added\n";
  }

  return 0;
}

//---------------------------------------------------------------------------//
