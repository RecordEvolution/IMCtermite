//---------------------------------------------------------------------------//

#ifndef IMCMETA
#define IMCMETA

//---------------------------------------------------------------------------//

namespace imc
{
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
