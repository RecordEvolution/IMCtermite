//---------------------------------------------------------------------------//

#ifndef IMCCHANNEL
#define IMCCHANNEL

#include <sstream>

//---------------------------------------------------------------------------//

namespace imc
{
  // collect uuid's of blocks required for full channel reconstruction
  struct channel_env
  {
    // define unique identifer for channel_env
    std::string uuid_;

    // collect affiliate blocks for a single channel
    std::string CBuuid_, CGuuid_, CCuuid_, CNuuid_;
    std::string CDuuid_, CTuuid_, Cbuuid_, CPuuid_, CRuuid_, CSuuid_;
    std::string NTuuid_, NOuuid_;

    // reset all members
    void reset()
    {
      uuid_.clear();
      CBuuid_.clear();
      CGuuid_.clear();
      CCuuid_.clear();
      CNuuid_.clear();
      CDuuid_.clear();
      CTuuid_.clear();
      Cbuuid_.clear();
      CPuuid_.clear();
      CRuuid_.clear();
      CSuuid_.clear();
      NTuuid_.clear();
      NOuuid_.clear();
    }

    // get info
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"uuid:"<<uuid_<<"\n"
        <<std::setw(width)<<std::left<<"CBuuid:"<<CBuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CGuuid:"<<CGuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CCuuid:"<<CCuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CNuuid:"<<CNuuid_<<"\n"
        //
        <<std::setw(width)<<std::left<<"CDuuid:"<<CDuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CTuuid:"<<CTuuid_<<"\n"
        <<std::setw(width)<<std::left<<"Cbuuid:"<<Cbuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CPuuid:"<<CPuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CRuuid:"<<CRuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CSuuid:"<<CSuuid_<<"\n"
        //
        <<std::setw(width)<<std::left<<"NTuuid:"<<NTuuid_<<"\n"
        <<std::setw(width)<<std::left<<"NOuuid:"<<NOuuid_<<"\n";
      return ss.str();
    }
  };

  // actual result and (meta)data of channel
  struct channel_data
  {
    // collect meta-data of channels according to env,
    //  just everything valueable in here
    std::string uuid_;
    std::string name_;
    std::string yunit_;
    std::string xunit_;
    std::vector<imc::datatype> ydata_;
    std::vector<imc::datatype> xdata_;

    // provide JSON sttring of metadata
    std::string get_json()
    {
      std::stringstream ss;
      ss<<""<<"\n";
      return ss.str();
    }


  };
}

#endif

//---------------------------------------------------------------------------//
