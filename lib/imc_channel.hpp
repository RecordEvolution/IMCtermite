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
    std::string origin_, origin_comment_, text_;
    std::string yname_, yunit_;
    std::string xname_, xunit_;
    double xstepwidth_;

    // buffer and data
    int signbits_, num_bytes_;
    // unsigned long int byte_offset_;
    unsigned long int buffer_offset_, buffer_size_;
    int datatp_;
    imc::datatype dattyp_;
    std::vector<imc::datatype> ydata_;
    std::vector<imc::datatype> xdata_;

    // range, factor and offset
    double factor_, offset_;

    // group reference the channel belongs to
    int group_index_;
    std::string group_uuid_, group_name_, group_comment_;

    // constructor takes channel's block environment
    channel(channel_env chnenv, std::map<std::string,imc::block>* blocks):
      chnenv_(chnenv), blocks_(blocks), group_index_(-1)
    {
      // declare list of block parameters
      std::vector<imc::parameter> prms;

      // use uuid from CN block
      uuid_ = chnenv_.CNuuid_;

      // extract associated CB data
      if ( blocks->count(chnenv_.CBuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CBuuid_).get_parameters();
        group_index_ = std::stoi(blocks->at(chnenv_.CBuuid_).get_parameter(prms[2]));
        group_name_ = blocks->at(chnenv_.CBuuid_).get_parameter(prms[4]);
        group_comment_ = blocks->at(chnenv_.CBuuid_).get_parameter(prms[6]);
      }

      // extract associated CT data
      if ( blocks->count(chnenv_.CTuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CTuuid_).get_parameters();
        text_ = blocks->at(chnenv_.CTuuid_).get_parameter(prms[4]) + std::string(" - ")
              + blocks->at(chnenv_.CTuuid_).get_parameter(prms[6]) + std::string(" - ")
              + blocks->at(chnenv_.CTuuid_).get_parameter(prms[8]);
      }

      // extract associated CD data
      if ( blocks->count(chnenv_.CDuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CDuuid_).get_parameters();
        xstepwidth_ = std::stod(blocks->at(chnenv_.CDuuid_).get_parameter(prms[2]));
        xunit_ = blocks->at(chnenv_.CDuuid_).get_parameter(prms[5]);
        // TODO
        xname_ = std::string("time");
      }

      // extract associated CP data
      if ( blocks->count(chnenv_.CPuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CPuuid_).get_parameters();
        datatp_ = std::stoi(blocks->at(chnenv_.CPuuid_).get_parameter(prms[3]));
        num_bytes_ = std::stoi(blocks->at(chnenv_.CPuuid_).get_parameter(prms[4]));
        signbits_ = std::stoi(blocks->at(chnenv_.CPuuid_).get_parameter(prms[5]));
        // byte_offset_ = std::stoul(blocks->at(chnenv_.CPuuid_).get_parameter(prms[7]));
      }

      // extract associated Cb data
      if ( blocks->count(chnenv_.Cbuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.Cbuuid_).get_parameters();
        buffer_offset_ = std::stoul(blocks->at(chnenv_.Cbuuid_).get_parameter(prms[6]));
        buffer_size_ = std::stoul(blocks->at(chnenv_.Cbuuid_).get_parameter(prms[7]));
      }

      // extract associated CR data
      if ( blocks->count(chnenv_.CRuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CRuuid_).get_parameters();
        factor_ = std::stod(blocks->at(chnenv_.CRuuid_).get_parameter(prms[3]));
        offset_ = std::stod(blocks->at(chnenv_.CRuuid_).get_parameter(prms[4]));
        yunit_ = blocks->at(chnenv_.CRuuid_).get_parameter(prms[7]);
      }

      // extract associated CN data
      if ( blocks->count(chnenv_.CNuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.CNuuid_).get_parameters();
        name_ = blocks->at(chnenv_.CNuuid_).get_parameter(prms[6]);
        yname_ = name_;
        comment_ = blocks->at(chnenv_.CNuuid_).get_parameter(prms[8]);
        // group_index_ = std::stoi(blocks->at(chnenv_.CNuuid_).get_parameter(prms[2]));
      }

      // extract associated NO data
      if ( blocks->count(chnenv_.NOuuid_) == 1 )
      {
        prms = blocks->at(chnenv_.NOuuid_).get_parameters();
        origin_ = blocks->at(chnenv_.NOuuid_).get_parameter(prms[4]);
        origin_comment_ = blocks->at(chnenv_.NOuuid_).get_parameter(prms[6]);
      }
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"uuid:"<<uuid_<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n"
        <<std::setw(width)<<std::left<<"origin:"<<origin_<<"\n"
        <<std::setw(width)<<std::left<<"description:"<<text_<<"\n"
        <<std::setw(width)<<std::left<<"yname:"<<yname_<<"\n"
        <<std::setw(width)<<std::left<<"yunit:"<<yunit_<<"\n"
        <<std::setw(width)<<std::left<<"datatype:"<<datatp_<<"\n"
        <<std::setw(width)<<std::left<<"significant bits:"<<signbits_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-offset:"<<buffer_offset_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-size:"<<buffer_size_<<"\n"
        <<std::setw(width)<<std::left<<"xname:"<<xname_<<"\n"
        <<std::setw(width)<<std::left<<"xunit:"<<xunit_<<"\n"
        <<std::setw(width)<<std::left<<"xstepwidth:"<<xstepwidth_<<"\n"
        <<std::setw(width)<<std::left<<"factor:"<<factor_<<"\n"
        <<std::setw(width)<<std::left<<"offset:"<<offset_<<"\n"
        <<std::setw(width)<<std::left<<"group:"<<"("<<group_index_<<","<<group_name_
                                                    <<","<<group_comment_<<")"<<"\n"
        <<std::setw(width)<<std::left<<"ydata:"<<imc::joinvec<imc::datatype>(ydata_)<<"\n"
        <<std::setw(width)<<std::left<<"xdata:"<<imc::joinvec<imc::datatype>(xdata_)<<"\n"
        <<std::setw(width)<<std::left<<"aff. blocks:"<<chnenv_.get_json()<<"\n";
      return ss.str();
    }

    // provide JSON string of metadata
    std::string get_json()
    {
      std::stringstream ss;
      ss<<"{"<<"\"uuid\":\""<<uuid_
             <<"\",\"name\":\""<<name_
             <<"\",\"comment\":\""<<comment_
             <<"\",\"origin\":\""<<origin_
             <<"\",\"description\":\""<<text_
             <<"\",\"yname\":\""<<yname_
             <<"\",\"yunit\":\""<<yunit_
             <<"\",\"significantbits\":\""<<signbits_
             <<"\",\"xname\":\""<<xname_
             <<"\",\"xunit\":\""<<xunit_
             <<"\",\"xstepwidth\":\""<<xstepwidth_
             <<"\",\"group\":{"<<"\"index\":\""<<group_index_
                               <<"\",\"name\":\""<<group_name_
                               <<"\",\"comment\":\""<<group_comment_<<"\""<<"}"
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
