//---------------------------------------------------------------------------//

#include <iostream>
#include <filesystem>

#include "imc_key.hpp"
#include "imc_block.hpp"
#include "imc_raw.hpp"

//---------------------------------------------------------------------------//

const std::string gittag("TAGSTRING");
const std::string githash("HASHSTRING");

//---------------------------------------------------------------------------//

// define type of key-value map object
typedef std::map<std::string,std::string> optkeys;

optkeys parse_args(int argc, char* argv[], bool list_args = false)
{
  if ( list_args )
  {
    std::cout<<"number of CLI-arguments: "<<argc<<"\n";
    for ( int i = 0; i < argc; i++ ) std::cout<<i<<":   "<<argv[i]<<"\n";
  }

  // declare empty key-value object
  optkeys prsdkeys;

  // parse all CLI arguments
  for ( int i = 1; i < argc; i++ )
  {
    if ( std::string(argv[i]) == std::string("--showmeta")
      || std::string(argv[i]) == std::string("-m") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("showmeta",argv[i]));
    }
    else if ( std::string(argv[i]) == std::string("--listgroups")
           || std::string(argv[i]) == std::string("-g") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("listgroups",argv[i]));
    }
    else if ( std::string(argv[i]) == std::string("--listchannels")
           || std::string(argv[i]) == std::string("-c") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("listchannels",argv[i]));
    }
    else if ( std::string(argv[i]) == std::string("--listblocks")
           || std::string(argv[i]) == std::string("-b") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("listblocks",argv[i]));
    }
    else if ( std::string(argv[i]) == std::string("--filename")
           || std::string(argv[i]) == std::string("-f") )
    {
      if ( i+1 == argc || argv[i+1][0] == '-' )
      {
        std::cerr<<"invalid or missing --filename argument\n";
        prsdkeys.insert(std::pair<std::string,std::string>("invalid","filename"));
      }
      else
      {
        prsdkeys.insert(std::pair<std::string,std::string>("filename",argv[i+1]));
        i = i + 1;
      }
    }
    else if ( std::string(argv[i]) == std::string("--help")
           || std::string(argv[i]) == std::string("-h") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("help",argv[i]));
    }
    else if ( std::string(argv[i]) == std::string("--version")
           || std::string(argv[i]) == std::string("-v") )
    {
      prsdkeys.insert(std::pair<std::string,std::string>("version",argv[i]));
    }
    else
    {
      // check for invalid arguments
      if ( argv[i][0] == '-' )
      {
        std::cerr<<"invalid or unkown argument: "<<argv[i]<<"\n";
        prsdkeys.insert(std::pair<std::string,std::string>("invalid",argv[i]));
      }
      // or missing filenames
      else if ( std::string(argv[i]).find(".raw") == std::string::npos )
      {
        std::cerr<<"doesn't look like a .raw file (make sure to include extension in filename!): "
                 <<argv[i]<<"\n";
        prsdkeys.insert(std::pair<std::string,std::string>("invalid",argv[i]));
      }
    }
  }

  return prsdkeys;
}

//---------------------------------------------------------------------------//

void show_version()
{
  std::cout<<"imctermite ["<<gittag<<"-g"<<githash<<"]"<<"\n";
}

void show_usage()
{
  std::cout<<"\n"
           <<"imctermite ["<<gittag<<"-g"<<githash<<"] (github.com/RecordEvolution/IMCtermite.git)"
           <<"\n\n"
           <<"Decode IMC raw files and dump data as *.csv"
           <<"\n\n"
           <<"Usage:\n\n"
           <<" imctermite <raw-file> [options]"
           <<"\n\n"
           <<"Options:"
           <<"\n\n"
           <<" -m, --showmeta          show meta information about IMC dataset\n"
           <<" -g, --listgroups        list channelgroups\n"
           <<" -c, --listchannels      list channels\n"
           <<" -b, --listblocks        list IMC key-blocks\n"
           <<" -f, --filename          filename for csv output\n"
           <<" -h, --help              show this help message \n"
           <<" -v, --version           display version\n"
           <<"\n";
}

//---------------------------------------------------------------------------//

int main(int argc, char* argv[])
{
  // parse CLI arguments
  optkeys cfgopts = parse_args(argc,argv);

  // exit on any invalid arguments
  if ( cfgopts.count("invalid") > 0 ) return 1;

  if ( cfgopts.count("version") == 1 )
  {
    show_version();
  }
  else if ( cfgopts.count("help") == 1 )
  {
    show_usage();
  }
  else
  {
    // check for at least one file argument
    if ( cfgopts.size() == (unsigned int)argc-1 )
    {
      std::cerr<<"no .raw file given => check --help for usage\n";
      return 1;
    }
    std::string rawfile(argv[1]);

    // check existence of file
    std::filesystem::path rawpath = rawfile;
    if ( !std::filesystem::exists(rawpath) )
    {
      std::cerr<<"file does not exist: "<<rawfile<<"\n";
      return 1;
    }

    // initialize "imc::raw" instance
    imc::raw imcraw;
    try {
      imcraw.set_file(rawfile);
    } catch (const std::exception& e ) {
      std::cerr<<"failed to open and parse raw-file: "<<e.what()<<"\n";
      return 1;
    }

    // list blocks
    for ( imc::block blk: imcraw.blocks() )
    {
      std::cout<<blk.get_key().get_info()<<"\n";
      std::cout<<blk.get_info()<<"\n";
    }
    std::cout<<"number of blocks: "<<imcraw.blocks().size()<<"\n";
    std::cout<<"computational complexity: "<<imcraw.computational_complexity()
                                           <<"/"<<imcraw.buffer_size()<<"\n";
  }

  return 0;
}

//---------------------------------------------------------------------------//
