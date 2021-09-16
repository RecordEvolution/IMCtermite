//---------------------------------------------------------------------------//

#ifndef IMCCHANNEL
#define IMCCHANNEL

#include "imc_datatype.hpp"
#include "imc_conversion.hpp"
#include <sstream>
#include <math.h>
#include <chrono>
#include <ctime>

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
    std::string NTuuid_, NOuuid_, NLuuid_;

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
      NLuuid_.clear();
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
        <<std::setw(width)<<std::left<<"NOuuid:"<<NOuuid_<<"\n"
        <<std::setw(width)<<std::left<<"NLuuid:"<<NLuuid_<<"\n";
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
             <<"\",\"NLuuid\":\""<<NLuuid_
             <<"\"}";
      return ss.str();
    }
  };

  // adjust stream object
  void customize_stream(std::ostream& stout, int prec, bool fixed)
  {
    if ( fixed )
    {
      stout<<std::setprecision(prec)<<std::fixed;
    }
    else
    {
      stout<<std::setprecision(prec);
    }
  }

  // given a list of numeric objects, join it into a string
  template<typename dt>
  std::string joinvec(std::vector<dt> myvec, unsigned long int limit = 10, int prec = 10, bool fixed = true)
  {
    // include entire list for limit = 0
    unsigned long int myvecsize = (unsigned long int)myvec.size();
    limit = (limit == 0) ? myvecsize : limit;

    std::stringstream ss;
    ss<<"[";
    if ( myvec.size() <= limit )
    {
      for ( dt el: myvec )
      {
        customize_stream(ss,prec,fixed);
        ss<<el<<",";
      }
    }
    else
    {
      unsigned long int heals = limit/2;
      for ( unsigned long int i = 0; i < heals; i++ )
      {
        customize_stream(ss,prec,fixed);
        ss<<myvec[i]<<",";
      }
      ss<<"...";
      for ( unsigned long int i = myvecsize-heals; i < myvecsize; i++ )
      {
        customize_stream(ss,prec,fixed);
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
    std::map<std::string,imc::block>* blocks_;
    std::vector<unsigned char>* buffer_;

    // collect meta-data of channels according to env,
    // just everything valueable in here
    std::string uuid_;
    std::string name_, comment_;
    std::string origin_, origin_comment_, text_;
    std::chrono::system_clock::time_point trigger_time_, absolute_trigger_time_;
    double trigger_time_frac_secs_;
    std::string language_code_, codepage_;
    std::string yname_, yunit_;
    std::string xname_, xunit_;
    double xstepwidth_, xoffset_;
    int xprec_;

    // buffer and data
    int signbits_, num_bytes_;
    // unsigned long int byte_offset_;
    unsigned long int buffer_offset_, buffer_size_;
    long int addtime_;
    int datatp_;
    imc::datatype dattyp_;
    std::vector<imc::datatype> ydata_;
    std::vector<double> xdata_;

    // range, factor and offset
    double factor_, offset_;

    // group reference the channel belongs to
    int group_index_;
    std::string group_uuid_, group_name_, group_comment_;

    // constructor takes channel's block environment
    channel(channel_env &chnenv, std::map<std::string,imc::block>* blocks,
                                 std::vector<unsigned char>* buffer):
      chnenv_(chnenv), blocks_(blocks), buffer_(buffer),
      factor_(1.), offset_(0.),
      group_index_(-1)
    {
      // declare list of block parameters
      std::vector<imc::parameter> prms;

      // use uuid from CN block
      uuid_ = chnenv_.CNuuid_;

      // extract associated CB data
      if ( blocks_->count(chnenv_.CBuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CBuuid_).get_parameters();
        group_index_ = std::stoi(blocks_->at(chnenv_.CBuuid_).get_parameter(prms[2]));
        group_name_ = blocks_->at(chnenv_.CBuuid_).get_parameter(prms[4]);
        group_comment_ = blocks_->at(chnenv_.CBuuid_).get_parameter(prms[6]);
      }

      // extract associated CT data
      if ( blocks_->count(chnenv_.CTuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CTuuid_).get_parameters();
        text_ = blocks_->at(chnenv_.CTuuid_).get_parameter(prms[4]) + std::string(" - ")
              + blocks_->at(chnenv_.CTuuid_).get_parameter(prms[6]) + std::string(" - ")
              + blocks_->at(chnenv_.CTuuid_).get_parameter(prms[8]);
      }

      // extract associated CD data
      if ( blocks_->count(chnenv_.CDuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CDuuid_).get_parameters();
        xstepwidth_ = std::stod(blocks_->at(chnenv_.CDuuid_).get_parameter(prms[2]));
        xunit_ = blocks_->at(chnenv_.CDuuid_).get_parameter(prms[5]);
        // TODO
        // xname_ = std::string("time");

        // find appropriate precision for "xdata_" by means of "xstepwidth_"
        xprec_ = (xstepwidth_ > 0 ) ? (int)ceil(fabs(log10(xstepwidth_))) : 10;
      }

      // extract associated CP data
      if ( blocks_->count(chnenv_.CPuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CPuuid_).get_parameters();
        num_bytes_ = std::stoi(blocks_->at(chnenv_.CPuuid_).get_parameter(prms[3]));
        datatp_ = std::stoi(blocks_->at(chnenv_.CPuuid_).get_parameter(prms[4]));
        signbits_ = std::stoi(blocks_->at(chnenv_.CPuuid_).get_parameter(prms[5]));
        // byte_offset_ = std::stoul(blocks_->at(chnenv_.CPuuid_).get_parameter(prms[7]));
      }

      // extract associated Cb data
      if ( blocks_->count(chnenv_.Cbuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.Cbuuid_).get_parameters();
        buffer_offset_ = std::stoul(blocks_->at(chnenv_.Cbuuid_).get_parameter(prms[6]));
        buffer_size_ = std::stoul(blocks_->at(chnenv_.Cbuuid_).get_parameter(prms[7]));
        xoffset_ = std::stod(blocks_->at(chnenv_.Cbuuid_).get_parameter(prms[11]));
        addtime_ = (long int)std::stod(blocks_->at(chnenv_.Cbuuid_).get_parameter(prms[12]));
      }

      // extract associated CR data
      if ( blocks_->count(chnenv_.CRuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CRuuid_).get_parameters();
        factor_ = std::stod(blocks_->at(chnenv_.CRuuid_).get_parameter(prms[3]));
        offset_ = std::stod(blocks_->at(chnenv_.CRuuid_).get_parameter(prms[4]));
        yunit_ = blocks_->at(chnenv_.CRuuid_).get_parameter(prms[7]);
      }

      // extract associated CN data
      if ( blocks_->count(chnenv_.CNuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.CNuuid_).get_parameters();
        name_ = blocks_->at(chnenv_.CNuuid_).get_parameter(prms[6]);
        yname_ = name_;
        comment_ = blocks_->at(chnenv_.CNuuid_).get_parameter(prms[8]);
        // group_index_ = std::stoi(blocks_->at(chnenv_.CNuuid_).get_parameter(prms[2]));
      }

      // extract associated NO data
      if ( blocks_->count(chnenv_.NOuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.NOuuid_).get_parameters();
        origin_ = blocks_->at(chnenv_.NOuuid_).get_parameter(prms[4]);
        origin_comment_ = blocks_->at(chnenv_.NOuuid_).get_parameter(prms[6]);
      }

      // extract associated NL data
      // codepage:
      // - http://www.iana.org/assignments/character-sets/character-sets.xhtml
      // - https://de.wikipedia.org/wiki/Zeichensatztabelle
      // language-code:
      // - https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-lcid/a9eac961-e77d-41a6-90a5-ce1a8b0cdb9c?redirectedfrom=MSDN
      if ( blocks_->count(chnenv_.NLuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.NLuuid_).get_parameters();
        codepage_ = blocks_->at(chnenv_.NLuuid_).get_parameter(prms[2]);
        language_code_ = blocks_->at(chnenv_.NLuuid_).get_parameter(prms[3]);
      }

      // obtain NT data
      // - https://en.cppreference.com/w/cpp/chrono/c/tm
      // - https://en.cppreference.com/w/cpp/io/manip/put_time
      if ( blocks_->count(chnenv_.NTuuid_) == 1 )
      {
        prms = blocks_->at(chnenv_.NTuuid_).get_parameters();
        //std::tm tm{};
        std::tm tms = std::tm();
        tms.tm_mday = std::stoi(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[2]));
        tms.tm_mon = std::stoi(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[3])) - 1;
        tms.tm_year = std::stoi(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[4])) - 1900;
        tms.tm_hour = std::stoi(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[5]));
        tms.tm_min = std::stoi(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[6]));
        long double secs = std::stold(blocks_->at(chnenv_.NTuuid_).get_parameter(prms[7]));
        double secs_int;
        trigger_time_frac_secs_ = modf((double)secs,&secs_int);
        tms.tm_sec = (int)secs_int;

        // generate std::chrono::system_clock::time_point type
        std::time_t ts = std::mktime(&tms);
        trigger_time_ = std::chrono::system_clock::from_time_t(ts);
      }

      // start converting binary buffer to imc::datatype
      if ( !chnenv_.CSuuid_.empty() ) convert_buffer();

      // calculate absolute trigger-time
      absolute_trigger_time_ = trigger_time_ + std::chrono::seconds(addtime_);
      //                                       + std::chrono::nanoseconds((long int)(trigger_time_frac_secs_*1.e9));
    }

    // convert buffer to actual datatype
    void convert_buffer()
    {
      // TODO no clue how/if/when to handle buffer offset/mask/subsequent_bytes
      // etc. and whatever that shit is!
      std::vector<imc::parameter> prms = blocks_->at(chnenv_.CSuuid_).get_parameters();
      if ( prms.size() < 4)
      {
        throw std::runtime_error("CS block is invalid and features to few parameters");
      }
      unsigned long int buffstrt = prms[3].begin();
      std::vector<unsigned char> CSbuffer( buffer_->begin()+buffstrt+1,
                                           buffer_->begin()+buffstrt+buffer_size_+1 );

      // determine number of values in buffer
      unsigned long int num_values = (unsigned long int)(CSbuffer.size()/(signbits_/8));
      if ( num_values*(signbits_/8) != CSbuffer.size() )
      {
        throw std::runtime_error("CSbuffer and significant bits of datatype don't match");
      }

      // adjust size of ydata
      ydata_.resize(num_values);

      // distinguish numeric datatypes included in "imc_datatype"
      if ( datatp_ == 1 )
      {
        imc::convert_data_to_type<imc_Ubyte>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 2 )
      {
        imc::convert_data_to_type<imc_Sbyte>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 3 )
      {
        imc::convert_data_to_type<imc_Ushort>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 4 )
      {
        imc::convert_data_to_type<imc_Sshort>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 5 )
      {
        imc::convert_data_to_type<imc_Ulongint>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 6 )
      {
        imc::convert_data_to_type<imc_Slongint>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 7 )
      {
        imc::convert_data_to_type<imc_float>(CSbuffer,ydata_);
      }
      else if ( datatp_ == 8 )
      {
        imc::convert_data_to_type<imc_double>(CSbuffer,ydata_);
      }
      // ...
      else if ( datatp_ == 11 )
      {
        imc::convert_data_to_type<imc_digital>(CSbuffer,ydata_);
      }
      else
      {
        throw std::runtime_error(std::string("unsupported/unknown datatype") + std::to_string(datatp_));
      }

      // fill xdata_
      for ( unsigned long int i = 0; i < num_values; i++ )
      {
        xdata_.push_back(xoffset_+(double)i*xstepwidth_);
      }

      // employ data transformation
      if ( factor_ != 1.0 || offset_ != 0.0 )
      {
        for ( imc::datatype& el: ydata_ )
        {
          //std::cout<<"value:"<<el.as_double()<<"\n";
          double fact = ( factor_ == 0.0 ) ? 1.0 : factor_;
          el = imc::datatype(el.as_double()*fact + offset_);
        }
      }
    }

    // get info string
    std::string get_info(int width = 20)
    {
      // prepare printable trigger-time
      std::time_t tt = std::chrono::system_clock::to_time_t(trigger_time_);
      std::time_t att = std::chrono::system_clock::to_time_t(absolute_trigger_time_);

      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"uuid:"<<uuid_<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n"
        <<std::setw(width)<<std::left<<"origin:"<<origin_<<"\n"
        <<std::setw(width)<<std::left<<"description:"<<text_<<"\n"
        <<std::setw(width)<<std::left<<"trigger-time-nt:"<<std::put_time(std::localtime(&tt),"%FT%T")<<"\n"
        <<std::setw(width)<<std::left<<"trigger-time:"<<std::put_time(std::localtime(&att),"%FT%T")<<"\n"
        <<std::setw(width)<<std::left<<"language-code:"<<language_code_<<"\n"
        <<std::setw(width)<<std::left<<"codepage:"<<codepage_<<"\n"
        <<std::setw(width)<<std::left<<"yname:"<<yname_<<"\n"
        <<std::setw(width)<<std::left<<"yunit:"<<yunit_<<"\n"
        <<std::setw(width)<<std::left<<"datatype:"<<datatp_<<"\n"
        <<std::setw(width)<<std::left<<"significant bits:"<<signbits_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-offset:"<<buffer_offset_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-size:"<<buffer_size_<<"\n"
        <<std::setw(width)<<std::left<<"add-time:"<<addtime_<<"\n"
        <<std::setw(width)<<std::left<<"xname:"<<xname_<<"\n"
        <<std::setw(width)<<std::left<<"xunit:"<<xunit_<<"\n"
        <<std::setw(width)<<std::left<<"xstepwidth:"<<xstepwidth_<<"\n"
        <<std::setw(width)<<std::left<<"xoffset:"<<xoffset_<<"\n"
        <<std::setw(width)<<std::left<<"factor:"<<factor_<<"\n"
        <<std::setw(width)<<std::left<<"offset:"<<offset_<<"\n"
        <<std::setw(width)<<std::left<<"group:"<<"("<<group_index_<<","<<group_name_
                                                    <<","<<group_comment_<<")"<<"\n"
        <<std::setw(width)<<std::left<<"ydata:"<<imc::joinvec<imc::datatype>(ydata_,6,9,true)<<"\n"
        <<std::setw(width)<<std::left<<"xdata:"<<imc::joinvec<double>(xdata_,6,xprec_,true)<<"\n";
        // <<std::setw(width)<<std::left<<"aff. blocks:"<<chnenv_.get_json()<<"\n";
      return ss.str();
    }

    // provide JSON string of metadata
    std::string get_json(bool include_data = false)
    {
      // prepare printable trigger-time
      std::time_t tt = std::chrono::system_clock::to_time_t(trigger_time_);
      std::time_t att = std::chrono::system_clock::to_time_t(absolute_trigger_time_);

      std::stringstream ss;
      ss<<"{"<<"\"uuid\":\""<<uuid_
             <<"\",\"name\":\""<<name_
             <<"\",\"comment\":\""<<comment_
             <<"\",\"origin\":\""<<origin_
             <<"\",\"description\":\""<<text_
             <<"\",\"trigger-time-nt\":\""<<std::put_time(std::localtime(&tt),"%FT%T")
             <<"\",\"trigger-time\":\""<<std::put_time(std::localtime(&att),"%FT%T")
             <<"\",\"language-code\":\""<<language_code_
             <<"\",\"codepage\":\""<<codepage_
             <<"\",\"yname\":\""<<yname_
             <<"\",\"yunit\":\""<<yunit_
             <<"\",\"significantbits\":\""<<signbits_
             <<"\",\"addtime\":\""<<addtime_
             <<"\",\"xname\":\""<<xname_
             <<"\",\"xunit\":\""<<xunit_
             <<"\",\"xstepwidth\":\""<<xstepwidth_
             <<"\",\"xoffset\":\""<<xoffset_
             <<"\",\"group\":{"<<"\"index\":\""<<group_index_
                               <<"\",\"name\":\""<<group_name_
                               <<"\",\"comment\":\""<<group_comment_<<"\""<<"}";
      if ( include_data )
      {
        ss<<",\"ydata\":"<<imc::joinvec<imc::datatype>(ydata_,0,9,true)
          <<",\"xdata\":"<<imc::joinvec<double>(xdata_,0,xprec_,true);
      }
      // ss<<"\",\"aff. blocks\":\""<<chnenv_.get_json()
      ss<<"}";

      return ss.str();
    }

    // print channel
    void print(std::string filename, const char sep = ',', int width = 25, int yprec = 9)
    {
      std::ofstream fou(filename);

      // header
      if ( sep == ' ' )
      {
        fou<<std::setw(width)<<std::left<<xname_
           <<std::setw(width)<<std::left<<yname_<<"\n"
           <<std::setw(width)<<std::left<<xunit_
           <<std::setw(width)<<std::left<<yunit_<<"\n";
      }
      else
      {
        fou<<xname_<<sep<<yname_<<"\n"<<xunit_<<sep<<yunit_<<"\n";
      }

      for ( unsigned long int i = 0; i < xdata_.size(); i++ )
      {
        if ( sep == ' ' )
        {
          fou<<std::setprecision(xprec_)<<std::fixed
             <<std::setw(width)<<std::left<<xdata_[i]
             <<std::setprecision(yprec)<<std::fixed
             <<std::setw(width)<<std::left<<ydata_[i]<<"\n";
        }
        else
        {
          fou<<std::setprecision(xprec_)<<std::fixed<<xdata_[i]
             <<sep
             <<std::setprecision(yprec)<<std::fixed<<ydata_[i]<<"\n";
        }
      }

      fou.close();
    }
  };

}

#endif

//---------------------------------------------------------------------------//
