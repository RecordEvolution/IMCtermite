//---------------------------------------------------------------------------//

#ifndef IMCOBJECTS
#define IMCOBJECTS

//---------------------------------------------------------------------------//

namespace imc
{
  // start of group of keys (corresponds to key CK)
  struct group_keys
  {
    int version_;
    int length_;
    bool closed_;  // corresponds to true = 1 and false = 0 in file

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"version:"<<version_<<"\n"
        <<std::setw(width)<<std::left<<"length:"<<length_<<"\n"
        <<std::setw(width)<<std::left<<"closed:"<<(closed_?"yes":"no")<<"\n";
      return ss.str();
    }
  };

  // origin of data (corresponds to key NO)
  struct origin_data
  {
    bool origin_;  // corresponds to true = 1 ("verrechnet") and false = 0 ("Original")
    std::string generator_;
    bool comment_;

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"origin:"<<(origin_?"verrechnet":"Original")<<"\n"
        <<std::setw(width)<<std::left<<"generator:"<<generator_<<"\n"
        <<std::setw(width)<<std::left<<"comment_:"<<comment_<<"\n";
      return ss.str();
    }
  };

  struct info
  {
    // timestamp of measurement
    std::string timestamp_;
    bool valid_;

    // device and software
    std::string origin_;
  };

  struct abscissa
  {
    // name of entity
    std::string name_;

    // its unit
    std::string unit_;
  };

  struct channel
  {
    // name of channel
    std::string name_;

    // unit of channel's measurement entity
    std::string unit_;

    // datatype of channel
    imc::datatype daty_;

    // entity measurement depends on, i.e. channel's abscissa (mostly time)
    abscissa channel_xaxis_;
  };

}

#endif

//---------------------------------------------------------------------------//
