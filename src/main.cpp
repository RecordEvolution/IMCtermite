//---------------------------------------------------------------------------//

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


//  tdm_ripper ripper(argv[1],argv[2],false); //,"samples/SineData.tdx",false);
//
//  // ripper.list_datatypes();
//  // ripper.show_structure();
//
//  ripper.print_hash_local("data/hash_table_xml_local.dat");
//  ripper.print_hash_values("data/hash_table_xml_value.dat");
//  ripper.print_hash_double("data/hash_table_xml_double.dat");
//  ripper.print_extid("data/channel_ext_id.dat");
//
//  ripper.list_groups();
//  std::ofstream gout("data/list_of_groups.dat");
//  ripper.list_groups(gout);
//  gout.close();
//
//  ripper.list_channels();
//  std::ofstream fout("data/list_of_channels.dat");
//  ripper.list_channels(fout);
//  fout.close();
//
//  // long int nsa = 6.3636349745e10; // expected result: 22.07.2016, 19:49:05
//  // long int nsb = 6.3636350456e10;
//  // std::string ts = std::to_string(nsa);
//  // std::cout<<ripper.unix_timestamp(ts)<<"\n";
//
//  std::cout<<"number of channels "<<ripper.num_channels()<<"\n";
//  std::cout<<"number of groups "<<ripper.num_groups()<<"\n\n";
//
//  // obtain some specific meta information tags
//  std::cout<<"\n"<<ripper.get_meta("SMP_Name")<<"\n";
//  std::cout<<ripper.get_meta("SMP_Aufbau_Nr")<<"\n";
//  std::cout<<ripper.get_meta("SMP_Type")<<"\n";
//  std::cout<<ripper.get_meta("Location")<<"\n\n";
//
//  // print all meta information
//  ripper.print_meta("data/meta_info.csv");
//
//  // for ( int i = 0; i < ripper.num_groups(); i++ )
//  // {
//  //   std::cout<<std::setw(10)<<i+1<<std::setw(10)<<ripper.no_channels(i+1)<<"\n";
//  //   for ( int j = 0; j < ripper.no_channels(i+1); j++ )
//  //   {
//  //     std::cout<<std::setw(10)<<i+1<<std::setw(10)<<j+1<<std::setw(30)<<ripper.channel_name(i+1,j+1)<<"\n";
//  //   }
//  // }
//
//  // for ( int i = 3; i < 10; i++ )
//  for ( int i = 0; i < ripper.num_channels(); i++ )
//  // for ( int i = 11880; i < ripper.num_channels(); i++ )
//  {
//    ripper.print_channel(i,("data/channel_"+std::to_string(i+1)+"_"
//                                        +ripper.channel_name(i)+".dat").c_str());
//  }

  return 0;
}

//---------------------------------------------------------------------------//
