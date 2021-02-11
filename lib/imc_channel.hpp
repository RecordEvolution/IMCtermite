//---------------------------------------------------------------------------//

#ifndef IMCCHANNEL
#define IMCCHANNEL

#include <sstream>
#include "imc_datatype.hpp"

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

    // get JSON info string
    std::string get_json()
    {
      std::stringstream ss;
      ss<<"{"<<"\"uuid\":\""<<uuid_
             <<"\",\"CBuuid\":\""<<CBuuid_
             <<"\",\"CGuuid\":\""<<CGuuid_
             <<"\",\"CCuuid\":\""<<CCuuid_
             <<"\",\"CNuuid\":\""<<CNuuid_
             <<"\",\"CDuuid\":\""<<CDuuid_
             <<"\",\"CTuuid\":\""<<CTuuid_
             <<"\",\"Cbuuid\":\""<<Cbuuid_
             <<"\",\"CPuuid\":\""<<CPuuid_
             <<"\",\"CRuuid\":\""<<CRuuid_
             <<"\",\"CSuuid\":\""<<CSuuid_
             <<"\",\"NTuuid\":\""<<NTuuid_
             <<"\",\"NOuuid\":\""<<NOuuid_
             <<"\"}";
      return ss.str();
    }
  };

  // given a list of numeric objects, join it into a string
  template<typename dt>
  std::string joinvec(std::vector<dt> myvec, unsigned long int limit = 10)
  {
    std::stringstream ss;
    ss<<"[";
    if ( myvec.size() <= limit )
    {
      for ( dt el: myvec ) ss<<el<<",";
    }
    else
    {
      unsigned long int heals = (unsigned long int)(limit/2.);
      for ( unsigned long int i = 0; i < heals; i++ ) ss<<myvec[i]<<",";
      ss<<"...";
      for ( unsigned long int i = myvec.size()-heals; i < myvec.size(); i++ )
      {
        ss<<myvec[i]<<",";
      }
    }
    std::string sumstr = ss.str();
    if ( sumstr.size() > 1 ) sumstr.pop_back();
    sumstr += std::string("]");
    return sumstr;
  }

  // channel
  struct channel
  {
    // associated environment of blocks and map of blocks
    channel_env chnenv_;
    const std::map<std::string,imc::block>* blocks_;

    // collect meta-data of channels according to env,
    //  just everything valueable in here
    std::string uuid_;
    std::string name_, comment_;
    std::string yname_, yunit_;
    imc::datatype dattyp_;
    std::string xname_, xunit_;
    std::vector<imc::datatype> ydata_;
    std::vector<imc::datatype> xdata_;

    // group reference the channel belongs to
    int group_index_;
    std::string group_uuid_, group_name_;

    //

    // constructor takes channel's block environment
    channel(channel_env chnenv, std::map<std::string,imc::block>* blocks):
      chnenv_(chnenv), blocks_(blocks)
    {
      // extract info from corresponding blocks in environment
      uuid_ = chnenv_.CNuuid_;
      std::vector<imc::parameter> prms = blocks->at(chnenv_.CNuuid_).get_parameters();
      name_ = blocks->at(chnenv_.CNuuid_).get_parameter(prms[6]);
      comment_ = blocks->at(chnenv_.CNuuid_).get_parameter(prms[8]);
      group_index_ = std::stoi(blocks->at(chnenv_.CNuuid_).get_parameter(prms[2]));
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<"uuid:"<<std::setw(width)<<std::left<<uuid_<<"\n"
        <<"name:"<<std::setw(width)<<std::left<<name_<<"\n"
        <<"comment:"<<std::setw(width)<<std::left<<comment_<<"\n"
        <<"yname:"<<std::setw(width)<<std::left<<yname_<<"\n"
        <<"yunit:"<<std::setw(width)<<std::left<<yunit_<<"\n"
        <<"xname:"<<std::setw(width)<<std::left<<xname_<<"\n"
        <<"xunit:"<<std::setw(width)<<std::left<<xunit_<<"\n"
        <<"group:"<<std::setw(width)<<std::left<<"("<<group_index_<<","<<group_name_<<")"<<"\n"
        <<"aff. blocks:"<<std::setw(width)<<std::left<<chnenv_.get_json()<<"\n";
      return ss.str();
    }

    // provide JSON string of metadata
    std::string get_json()
    {
      std::stringstream ss;
      ss<<"{"<<"\"uuid\":\""<<uuid_
             <<"\",\"name\":\""<<name_
             <<"\",\"comment\":\""<<comment_
             <<"\",\"yname\":\""<<yname_
             <<"\",\"yunit\":\""<<yunit_
             <<"\",\"xname\":\""<<xname_
             <<"\",\"xunit\":\""<<xunit_
             <<"\",\"group\":\""<<"("<<group_index_<<","<<group_name_<<")"
             <<"\",\"ydata\":\""<<imc::joinvec<imc::datatype>(ydata_)
             <<"\",\"xdata\":\""<<imc::joinvec<imc::datatype>(xdata_)
             <<"\",\"aff. blocks\":\""<<chnenv_.get_json()
             <<"\"}";
      return ss.str();
    }


  };
}

#endif

//---------------------------------------------------------------------------//
