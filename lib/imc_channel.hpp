//---------------------------------------------------------------------------//

#ifndef IMCCHANNEL
#define IMCCHANNEL

#include "imc_datatype.hpp"
#include "imc_conversion.hpp"
#include "imc_block.hpp"
#include <sstream>
#include <math.h>
#include <chrono>
#include <ctime>
#include <time.h>
#if defined(__linux__) || defined(__APPLE__)
#include <iconv.h>
#elif defined(__WIN32__) || defined(_WIN32)
#define timegm _mkgmtime
#endif

//---------------------------------------------------------------------------//

namespace imc
{
  struct component_env
  {
    std::string uuid_;
    // required channel components for CG channels only
    std::string CCuuid_, CPuuid_;
    // optional channel components for CG channels only
    std::string CDuuid_, NTuuid_;
    std::string Cbuuid_, CRuuid_;

    // reset all members
    void reset()
    {
      uuid_.clear();
      CCuuid_.clear();
      CPuuid_.clear();
      CDuuid_.clear();
      Cbuuid_.clear();
      CRuuid_.clear();
      NTuuid_.clear();
    }
  };

  // collect uuid's of blocks required for full channel reconstruction
  struct channel_env
  {
    // define unique identifer for channel_env
    std::string uuid_;

    // collect common affiliate blocks for every channel
    std::string NOuuid_, NLuuid_;
    // collect affiliate blocks for a single channel
    // channel types
    std::string CBuuid_, CGuuid_, CIuuid_, CTuuid_;
    std::string CNuuid_, CDuuid_, NTuuid_;
    std::string CSuuid_;

    component_env compenv1_;
    component_env compenv2_;


    // reset all members
    void reset()
    {
      uuid_.clear();
      NOuuid_.clear();
      NLuuid_.clear();
      CBuuid_.clear();
      CGuuid_.clear();
      CIuuid_.clear();
      CTuuid_.clear();
      CNuuid_.clear();
      CDuuid_.clear();
      NTuuid_.clear();
      CSuuid_.clear();
      compenv1_.reset();
      compenv2_.reset();
    }

    // get info
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"uuid:"<<uuid_<<"\n"
        <<std::setw(width)<<std::left<<"NOuuid:"<<NOuuid_<<"\n"
        <<std::setw(width)<<std::left<<"NLuuid:"<<NLuuid_<<"\n"
        //
        <<std::setw(width)<<std::left<<"CBuuid:"<<CBuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CGuuid:"<<CGuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CIuuid:"<<CIuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CTuuid:"<<CTuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CNuuid:"<<CNuuid_<<"\n"
        //
        <<std::setw(width)<<std::left<<"CCuuid:"<<compenv1_.CCuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CPuuid:"<<compenv1_.CPuuid_<<"\n"
        //
        <<std::setw(width)<<std::left<<"CDuuid:"<<compenv1_.CDuuid_<<"\n"
        <<std::setw(width)<<std::left<<"Cbuuid:"<<compenv1_.Cbuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CRuuid:"<<compenv1_.CRuuid_<<"\n"
        <<std::setw(width)<<std::left<<"NTuuid:"<<compenv1_.NTuuid_<<"\n"
        <<std::setw(width)<<std::left<<"CSuuid:"<<CSuuid_<<"\n";
      return ss.str();
    }

    // get JSON info string
    std::string get_json()
    {
      std::stringstream ss;
      ss<<"{"<<"\"uuid\":\""<<uuid_
             <<"\",\"NOuuid\":\""<<NOuuid_
             <<"\",\"NLuuid\":\""<<NLuuid_
             <<"\",\"CBuuid\":\""<<CBuuid_
             <<"\",\"CGuuid\":\""<<CGuuid_
             <<"\",\"CIuuid\":\""<<CIuuid_
             <<"\",\"CTuuid\":\""<<CTuuid_
             <<"\",\"CNuuid\":\""<<CNuuid_
             <<"\",\"CCuuid\":\""<<compenv1_.CCuuid_
             <<"\",\"CPuuid\":\""<<compenv1_.CPuuid_
             <<"\",\"CDuuid\":\""<<compenv1_.CDuuid_
             <<"\",\"Cbuuid\":\""<<compenv1_.Cbuuid_
             <<"\",\"CRuuid\":\""<<compenv1_.CRuuid_
             <<"\",\"NTuuid\":\""<<compenv1_.NTuuid_
             <<"\",\"CSuuid\":\""<<CSuuid_
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

  #if defined(__linux__) || defined(__APPLE__)
  // convert encoding of any descriptions, channel-names, units etc.
  class iconverter
  {
    std::string in_enc_, out_enc_;
    iconv_t cd_;
    size_t out_buffer_size_;

    public:

      iconverter(std::string in_enc, std::string out_enc, size_t out_buffer_size = 1024) :
        in_enc_(in_enc), out_enc_(out_enc), out_buffer_size_(out_buffer_size)
      {
        // allocate descriptor for character set conversion
        // (https://man7.org/linux/man-pages/man3/iconv_open.3.html)
        cd_ = iconv_open(out_enc.c_str(), in_enc.c_str());

        if ( (iconv_t)-1 == cd_ )
        {
          if ( errno == EINVAL )
          {
            std::string errmsg = std::string("The encoding conversion from ") + in_enc
              + std::string(" to ") + out_enc + std::string(" is not supported by the implementation.");
            throw std::runtime_error(errmsg);
          }
        }
      }

      void convert(std::string &astring)
      {
        if ( astring.empty() ) return;

        std::vector<char> in_buffer(astring.begin(),astring.end());
        char *inbuf = &in_buffer[0];
        size_t inbytes = in_buffer.size();

        std::vector<char> out_buffer(out_buffer_size_);
        char *outbuf = &out_buffer[0];
        size_t outbytes = out_buffer.size();

        // perform character set conversion
        // ( - https://man7.org/linux/man-pages/man3/iconv.3.html
        //   - https://www.ibm.com/docs/en/zos/2.2.0?topic=functions-iconv-code-conversion )
        while ( inbytes > 0 )
        {
          size_t res = iconv(cd_,&inbuf,&inbytes,&outbuf,&outbytes);

          if ( (size_t)-1 == res )
          {
            std::string errmsg;
            if ( errno == EILSEQ )
            {
              errmsg = std::string("An invalid multibyte sequence is encountered in the input.");
              throw std::runtime_error(errmsg);
            }
            else if ( errno == EINVAL )
            {
              errmsg = std::string("An incomplete multibyte sequence is encountered in the input")
                     + std::string(" and the input byte sequence terminates after it.");
            }
            else if ( errno == E2BIG )
            {
              errmsg = std::string("The output buffer has no more room for the next converted character.");
            }
            throw std::runtime_error(errmsg);
          }
        }

        std::string outstring(out_buffer.begin(),out_buffer.end()-outbytes);
        astring = outstring;
      }
  };
  #elif defined(__WIN32__) || defined(_WIN32)
  class iconverter
  {
    public:
      iconverter(std::string in_enc, std::string out_enc, size_t out_buffer_size = 1024) {}
      void convert(std::string &astring) {}
  };
  #endif

  struct component_group
  {
    imc::component CC_;
    imc::packaging CP_;
    imc::abscissa CD_;
    imc::buffer Cb_;
    imc::range CR_;
    imc::channelobj CN_;
    imc::triggertime NT_;

    component_env compenv_;

    // Constructor to parse the associated blocks
    component_group(component_env &compenv, std::map<std::string, imc::block>* blocks, std::vector<unsigned char>* buffer)
        : compenv_(compenv)
    {
        if (blocks->count(compenv.CCuuid_) == 1)
        {
            CC_.parse(buffer, blocks->at(compenv.CCuuid_).get_parameters());
        }
        if (blocks->count(compenv.CPuuid_) == 1)
        {
            CP_.parse(buffer, blocks->at(compenv.CPuuid_).get_parameters());
        }
        if (blocks->count(compenv.CDuuid_) == 1)
        {
            CD_.parse(buffer, blocks->at(compenv.CDuuid_).get_parameters());
        }
        if (blocks->count(compenv.Cbuuid_) == 1)
        {
            Cb_.parse(buffer, blocks->at(compenv.Cbuuid_).get_parameters());
        }
        if (blocks->count(compenv.CRuuid_) == 1)
        {
            CR_.parse(buffer, blocks->at(compenv.CRuuid_).get_parameters());
        }
        if (blocks->count(compenv.NTuuid_) == 1)
        {
            NT_.parse(buffer, blocks->at(compenv.NTuuid_).get_parameters());
        }
    }
  };


  // channel
  struct channel
  {
    // associated environment of blocks and map of blocks
    channel_env chnenv_;
    std::map<std::string,imc::block>* blocks_;
    std::vector<unsigned char>* buffer_;

    imc::origin_data NO_;

    imc::text CT_;
    imc::groupobj CB_;
    imc::datafield CG_;

    // collect meta-data of channels according to env,
    // just everything valueable in here
    // TODO: is this necessary?
    std::string uuid_;
    std::string name_, comment_;
    std::string origin_, origin_comment_, text_;
    std::chrono::system_clock::time_point trigger_time_, absolute_trigger_time_;
    double trigger_time_frac_secs_;
    std::string language_code_, codepage_;
    std::string yname_, yunit_;
    std::string xname_, xunit_;
    double xstepwidth_, xstart_;
    int xprec_;
    int dimension_;

    // buffer and data
    int xsignbits_, xnum_bytes_;
    int ysignbits_, ynum_bytes_;
    // unsigned long int byte_offset_;
    unsigned long int xbuffer_offset_, ybuffer_offset_;
    unsigned long int xbuffer_size_, ybuffer_size_;
    long int addtime_;
    imc::numtype xdatatp_, ydatatp_;
    std::vector<imc::datatype> xdata_, ydata_;

    // range, factor and offset
    double xfactor_, yfactor_;
    double xoffset_, yoffset_;

    // group reference the channel belongs to
    int group_index_;
    std::string group_uuid_, group_name_, group_comment_;

    // constructor takes channel's block environment
    channel(channel_env &chnenv, std::map<std::string,imc::block>* blocks,
                                 std::vector<unsigned char>* buffer):
      chnenv_(chnenv), blocks_(blocks), buffer_(buffer),
      xfactor_(1.), yfactor_(1.), xoffset_(0.), yoffset_(0.),
      group_index_(-1)
    {
      // use uuid from CN block
      uuid_ = chnenv_.CNuuid_;

      // extract associated CB data
      if ( blocks_->count(chnenv_.CBuuid_) == 1 )
      {
        CB_.parse(buffer_, blocks_->at(chnenv_.CBuuid_).get_parameters());
      }

      // extract associated CT data
      if ( blocks_->count(chnenv_.CTuuid_) == 1 )
      {
        CT_.parse(buffer_, blocks_->at(chnenv_.CTuuid_).get_parameters());
        text_ = CT_.name_ + std::string(" - ")
              + CT_.text_ + std::string(" - ")
              + CT_.comment_;
      }

      if ( !chnenv_.compenv1_.uuid_.empty() && chnenv_.compenv2_.uuid_.empty() )
      {
        // normal dataset (single component)
        // set common NT and CD keys if no others are specified
        if (chnenv_.compenv1_.NTuuid_.empty()) chnenv_.compenv1_.NTuuid_ = chnenv_.NTuuid_;
        if (chnenv_.compenv1_.CDuuid_.empty()) chnenv_.compenv1_.CDuuid_ = chnenv_.CDuuid_;

        // comp_group1 contains y-data, x-data is based on xstepwidth_, xstart_ and the length of y-data
        component_group comp_group1(chnenv_.compenv1_, blocks_, buffer_);
        dimension_ = 1;

        xstepwidth_ = comp_group1.CD_.dx_;
        xunit_ = comp_group1.CD_.unit_;
        ybuffer_offset_ = comp_group1.Cb_.offset_buffer_;
        ybuffer_size_ = comp_group1.Cb_.number_bytes_;
        xstart_ = comp_group1.Cb_.x0_;
        yfactor_ = comp_group1.CR_.factor_;
        yoffset_ = comp_group1.CR_.offset_;
        yunit_ = comp_group1.CR_.unit_;
        name_ = comp_group1.CN_.name_;
        yname_ = comp_group1.CN_.name_;
        comment_ = comp_group1.CN_.comment_;
        ynum_bytes_ = comp_group1.CP_.bytes_;
        ydatatp_ = comp_group1.CP_.numeric_type_;
        ysignbits_ = comp_group1.CP_.signbits_;
        // generate std::chrono::system_clock::time_point type
        std::time_t ts = timegm(&comp_group1.NT_.tms_); // std::mktime(&tms);
        trigger_time_ = std::chrono::system_clock::from_time_t(ts);
        trigger_time_frac_secs_ = comp_group1.NT_.trigger_time_frac_secs_;
        // calculate absolute trigger-time
        addtime_ = static_cast<long int>(comp_group1.Cb_.add_time_);
        absolute_trigger_time_ = trigger_time_ + std::chrono::seconds(addtime_);
        //                                       + std::chrono::nanoseconds((long int)(trigger_time_frac_secs_*1.e9));
      }
      else if ( !chnenv_.compenv1_.uuid_.empty() && !chnenv_.compenv2_.uuid_.empty() )
      {
        // XY dataset (two components)
        // set common NT and CD keys if no others are specified
        if (chnenv_.compenv1_.NTuuid_.empty()) chnenv_.compenv1_.NTuuid_ = chnenv_.NTuuid_;
        if (chnenv_.compenv1_.CDuuid_.empty()) chnenv_.compenv1_.CDuuid_ = chnenv_.CDuuid_;
        if (chnenv_.compenv2_.NTuuid_.empty()) chnenv_.compenv2_.NTuuid_ = chnenv_.NTuuid_;
        if (chnenv_.compenv2_.CDuuid_.empty()) chnenv_.compenv2_.CDuuid_ = chnenv_.CDuuid_;

        // comp_group1 contains x-data, comp_group2 contains y-data
        component_group comp_group1(chnenv_.compenv1_, blocks_, buffer_);
        component_group comp_group2(chnenv_.compenv2_, blocks_, buffer_);
        dimension_ = 2;

        xbuffer_offset_ = comp_group2.Cb_.offset_buffer_;
        xbuffer_size_ = comp_group2.Cb_.number_bytes_;
        ybuffer_offset_ = comp_group1.Cb_.offset_buffer_;
        ybuffer_size_ = comp_group1.Cb_.number_bytes_;
        xfactor_ = comp_group2.CR_.factor_;
        xoffset_ = comp_group2.CR_.offset_;
        yfactor_ = comp_group1.CR_.factor_;
        yoffset_ = comp_group1.CR_.offset_;
        xdatatp_ = comp_group2.CP_.numeric_type_;
        xsignbits_ = comp_group2.CP_.signbits_;
        ydatatp_ = comp_group1.CP_.numeric_type_;
        ysignbits_ = comp_group1.CP_.signbits_;
        // generate std::chrono::system_clock::time_point type
        std::time_t ts = timegm(&comp_group2.NT_.tms_); // std::mktime(&tms);
        trigger_time_ = std::chrono::system_clock::from_time_t(ts);
        trigger_time_frac_secs_ = comp_group2.NT_.trigger_time_frac_secs_;
        absolute_trigger_time_ = trigger_time_;
      }
      else
      {
        // no datafield
      }

      // start converting binary buffer to imc::datatype
      if ( !chnenv_.CSuuid_.empty() ) convert_buffer();

      // convert any non-UTF-8 codepage to UTF-8
      convert_encoding();
    }

    // convert buffer to actual datatype
    void convert_buffer()
    {
      std::vector<imc::parameter> prms = blocks_->at(chnenv_.CSuuid_).get_parameters();
      if ( prms.size() < 4)
      {
        throw std::runtime_error("CS block is invalid and features to few parameters");
      }

      // extract (channel dependent) part of buffer
      unsigned long int buffstrt = prms[3].begin();
      std::vector<unsigned char> yCSbuffer( buffer_->begin()+buffstrt+ybuffer_offset_+1,
                                           buffer_->begin()+buffstrt+ybuffer_offset_+ybuffer_size_+1 );

      // determine number of values in buffer
      unsigned long int ynum_values = (unsigned long int)(yCSbuffer.size()/(ysignbits_/8));
      if ( ynum_values*(ysignbits_/8) != yCSbuffer.size() )
      {
        throw std::runtime_error("CSbuffer and significant bits of y datatype don't match");
      }


      if (dimension_ ==  1)
      {
        // process y-data
        process_data(ydata_, ynum_values, ydatatp_, yCSbuffer);

        // find appropriate precision for "xdata_" by means of "xstepwidth_"
        xprec_ = (xstepwidth_ > 0 ) ? (int)ceil(fabs(log10(xstepwidth_))) : 10;

        // fill xdata_
        for ( unsigned long int i = 0; i < ynum_values; i++ )
        {
          xdata_.push_back(xstart_+(double)i*xstepwidth_);
        }
      }
      else if (dimension_ == 2)
      {
        // process x- and y-data
        std::vector<unsigned char> xCSbuffer( buffer_->begin()+buffstrt+xbuffer_offset_+1,
                                            buffer_->begin()+buffstrt+xbuffer_offset_+xbuffer_size_+1 );

        // determine number of values in buffer
        unsigned long int xnum_values = (unsigned long int)(xCSbuffer.size()/(xsignbits_/8));
        if ( xnum_values*(xsignbits_/8) != xCSbuffer.size() )
        {
          throw std::runtime_error("CSbuffer and significant bits of x datatype don't match");
        }
        if ( xnum_values != ynum_values )
        {
          throw std::runtime_error("x and y data have different number of values");
        }

        xprec_ = 9;

        process_data(xdata_, xnum_values, xdatatp_, xCSbuffer);
        process_data(ydata_, ynum_values, ydatatp_, yCSbuffer);
      }
      else
      {
        throw std::runtime_error("unsupported dimension");
      }

      transformData(xdata_, xfactor_, xoffset_);
      transformData(ydata_, yfactor_, yoffset_);
    }

    // handle data type conversion
    void process_data(std::vector<imc::datatype>& data_, size_t num_values, numtype datatp_, std::vector<unsigned char>& CSbuffer)
    {
      // adjust size of data
      data_.resize(num_values);

      // handle data type conversion
      switch (datatp_)
      {
          case numtype::unsigned_byte:
              imc::convert_data_to_type<imc_Ubyte>(CSbuffer, data_);
              break;
          case numtype::signed_byte:
              imc::convert_data_to_type<imc_Sbyte>(CSbuffer, data_);
              break;
          case numtype::unsigned_short:
              imc::convert_data_to_type<imc_Ushort>(CSbuffer, data_);
              break;
          case numtype::signed_short:
              imc::convert_data_to_type<imc_Sshort>(CSbuffer, data_);
              break;
          case numtype::unsigned_long:
              imc::convert_data_to_type<imc_Ulongint>(CSbuffer, data_);
              break;
          case numtype::signed_long:
              imc::convert_data_to_type<imc_Slongint>(CSbuffer, data_);
              break;
          case numtype::ffloat:
              imc::convert_data_to_type<imc_float>(CSbuffer, data_);
              break;
          case numtype::ddouble:
              imc::convert_data_to_type<imc_double>(CSbuffer, data_);
              break;
          case numtype::two_byte_word_digital:
              imc::convert_data_to_type<imc_digital>(CSbuffer, data_);
              break;
          case numtype::six_byte_unsigned_long:
              imc::convert_data_to_type<imc_sixbyte>(CSbuffer, data_);
              break;
          default:
              throw std::runtime_error(std::string("unsupported/unknown datatype ") + std::to_string(datatp_));
      }
    }

    void transformData(std::vector<imc::datatype>& data, double factor, double offset) {
        if (factor != 1.0 || offset != 0.0) {
            for (imc::datatype& el : data) {
                double fact = (factor == 0.0) ? 1.0 : factor;
                el = imc::datatype(el.as_double() * fact + offset);
            }
        }
    }

    // convert any description, units etc. to UTF-8 (by default)
    void convert_encoding()
    {
      if ( !codepage_.empty() )
      {
        // construct iconv-compatible name for respective codepage
        std::string cpn = std::string("CP") + codepage_;

        // set up converter
        std::string utf = std::string("UTF-8");
        iconverter conv(cpn,utf);

        conv.convert(name_);
        conv.convert(comment_);
        conv.convert(origin_);
        conv.convert(origin_comment_);
        conv.convert(text_);
        conv.convert(language_code_);
        conv.convert(yname_);
        conv.convert(yunit_);
        conv.convert(xname_);
        conv.convert(xunit_);
        conv.convert(group_name_);
        conv.convert(group_comment_);
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
        <<std::setw(width)<<std::left<<"origin-comment:"<<origin_comment_<<"\n"
        <<std::setw(width)<<std::left<<"description:"<<text_<<"\n"
        <<std::setw(width)<<std::left<<"trigger-time-nt:"<<std::put_time(std::gmtime(&tt),"%FT%T")<<"\n"
        <<std::setw(width)<<std::left<<"trigger-time:"<<std::put_time(std::gmtime(&att),"%FT%T")<<"\n"
        <<std::setw(width)<<std::left<<"language-code:"<<language_code_<<"\n"
        <<std::setw(width)<<std::left<<"codepage:"<<codepage_<<"\n"
        <<std::setw(width)<<std::left<<"yname:"<<yname_<<"\n"
        <<std::setw(width)<<std::left<<"yunit:"<<yunit_<<"\n"
        <<std::setw(width)<<std::left<<"datatype:"<<ydatatp_<<"\n"
        <<std::setw(width)<<std::left<<"significant bits:"<<ysignbits_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-offset:"<<ybuffer_offset_<<"\n"
        <<std::setw(width)<<std::left<<"buffer-size:"<<ybuffer_size_<<"\n"
        <<std::setw(width)<<std::left<<"xname:"<<xname_<<"\n"
        <<std::setw(width)<<std::left<<"xunit:"<<xunit_<<"\n"
        <<std::setw(width)<<std::left<<"xstepwidth:"<<xstepwidth_<<"\n"
        <<std::setw(width)<<std::left<<"xoffset:"<<xstart_<<"\n"
        <<std::setw(width)<<std::left<<"factor:"<<yfactor_<<"\n"
        <<std::setw(width)<<std::left<<"offset:"<<yoffset_<<"\n"
        <<std::setw(width)<<std::left<<"group:"<<"("<<group_index_<<","<<group_name_
                                                    <<","<<group_comment_<<")"<<"\n"
        <<std::setw(width)<<std::left<<"ydata:"<<imc::joinvec<imc::datatype>(ydata_,6,9,true)<<"\n"
        <<std::setw(width)<<std::left<<"xdata:"<<imc::joinvec<imc::datatype>(xdata_,6,xprec_,true)<<"\n";
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
             <<"\",\"origin-comment\":\""<<origin_comment_
             <<"\",\"description\":\""<<text_
             <<"\",\"trigger-time-nt\":\""<<std::put_time(std::gmtime(&tt),"%FT%T")
             <<"\",\"trigger-time\":\""<<std::put_time(std::gmtime(&att),"%FT%T")
             <<"\",\"language-code\":\""<<language_code_
             <<"\",\"codepage\":\""<<codepage_
             <<"\",\"yname\":\""<<prepjsonstr(yname_)
             <<"\",\"yunit\":\""<<prepjsonstr(yunit_)
             <<"\",\"significantbits\":\""<<ysignbits_
             <<"\",\"buffer-size\":\""<<ybuffer_size_
             <<"\",\"xname\":\""<<prepjsonstr(xname_)
             <<"\",\"xunit\":\""<<prepjsonstr(xunit_)
             <<"\",\"xstepwidth\":\""<<xstepwidth_
             <<"\",\"xoffset\":\""<<xstart_
             <<"\",\"group\":{"<<"\"index\":\""<<group_index_
                               <<"\",\"name\":\""<<group_name_
                               <<"\",\"comment\":\""<<group_comment_<<"\""<<"}";
      if ( include_data )
      {
        ss<<",\"ydata\":"<<imc::joinvec<imc::datatype>(ydata_,0,9,true)
          <<",\"xdata\":"<<imc::joinvec<imc::datatype>(xdata_,0,xprec_,true);
      }
      // ss<<"\",\"aff. blocks\":\""<<chnenv_.get_json()
      ss<<"}";

      return ss.str();
    }

    // prepare string value for usage in JSON dump
    std::string prepjsonstr(std::string value)
    {
      std::stringstream ss;
      ss<<quoted(value);
      return strip_quotes(ss.str());
    }

    // remove any leading or trailing double quotes
    std::string strip_quotes(std::string astring)
    {
      // head
      if ( astring.front() == '"' ) astring.erase(astring.begin()+0);
      // tail
      if ( astring.back() == '"' ) astring.erase(astring.end()-1);

      return astring;
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
