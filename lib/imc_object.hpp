//---------------------------------------------------------------------------//

#ifndef IMCOBJECT
#define IMCOBJECT

#include "imc_key.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  // format and processor (corresponds to key CF)
  struct format
  {
    int fileformat_;
    int processor_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    format(): fileformat_(-1), processor_(-1) {}

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"format:"<<fileformat_<<"\n"
        <<std::setw(width)<<std::left<<"processor:"<<processor_<<"\n";
      return ss.str();
    }
  };

  // start of group of keys (corresponds to key CK)
  struct keygroup
  {
    int version_;
    int length_;
    bool closed_;  // corresponds to true = 1 and false = 0 in file

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

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

  // group definition (corresponds to key CB)
  struct group
  {
    unsigned long int group_index_;
    std::string name_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"group-index:"<<group_index_<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n";
      return ss.str();
    }
  };

  // text definition (corresponds to key CT)
  struct text
  {
    unsigned long int group_index_; // corresponding to group-index in CB block
    std::string name_;
    std::string text_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"group-index:"<<group_index_<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"text:"<<text_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n";
      return ss.str();
    }
  };

  enum fieldtype {
    realnumber = 1,       // 1
    xmonotony,            // 2
    xy,                   // 3
    complexrealimag,      // 4
    complexabsphase,      // 5
    complexdbphase        // 6
  };

  // definition of data field (corresponds to key CG)
  struct datafield
  {
    unsigned long int number_components_;
    fieldtype fldtype_;
    int dimension_; // corresponding to fieldtype \in {1,}

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"#components:"<<number_components_<<"\n"
        <<std::setw(width)<<std::left<<"fieldtype:"<<fldtype_<<"\n"
        <<std::setw(width)<<std::left<<"dimension:"<<dimension_<<"\n";
      return ss.str();
    }
  };

  // definition of abscissa (corresponds to key CD1)
  struct abscissa
  {
    double dx_;
    bool calibration_;
    std::string unit_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"dx:"<<dx_<<"\n"
        <<std::setw(width)<<std::left<<"calibration_:"<<(calibration_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"unit:"<<unit_<<"\n";
      return ss.str();
    }
  };

  // start of component (corresponds to key CC)
  struct component
  {
    int component_index_;
    bool analog_digital_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"index:"<<component_index_<<"\n"
        <<std::setw(width)<<std::left<<"analog/digital_:"<<(analog_digital_?"digital":"analog")<<"\n";
      return ss.str();
    }
  };

  enum numtype {
    unsigned_byte = 1,
    signed_byte,
    unsigned_short,
    signed_short,
    unsigned_long,
    signed_long,
    ffloat,
    ddouble,
    imc_devices_transitional_recording,
    timestamp_ascii,
    two_byte_word_digital,
    six_byte_unsigned_long
  };

  // packaging information of component (corresponds to key CP)
  struct packaging
  {
    unsigned long int buffer_reference_;
    int bytes_;
    numtype numeric_type_;
    int signbits_;
    int mask_;
    unsigned long int offset_;
    unsigned long int number_subsequent_samples_;
    unsigned long int distance_bytes_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"buffer-reference:"<<buffer_reference_<<"\n"
        <<std::setw(width)<<std::left<<"datatype:"<<numeric_type_<<"\n"
        <<std::setw(width)<<std::left<<"significant bits:"<<signbits_<<"\n"
        <<std::setw(width)<<std::left<<"mask:"<<mask_<<"\n"
        <<std::setw(width)<<std::left<<"offset:"<<offset_<<"\n"
        <<std::setw(width)<<std::left<<"#subsequent-samples:"<<number_subsequent_samples_<<"\n"
        <<std::setw(width)<<std::left<<"distance in bytes:"<<distance_bytes_<<"\n";
      return ss.str();
    }
  };

  // buffer description (corresponds to key Cb)
  struct buffer
  {
    unsigned long int number_buffers_;
    unsigned long int bytes_userinfo_;
    // for every single buffer
    unsigned long int buffer_reference_; // corresponds to buffer_reference_ in key CP
    unsigned long int sample_index_;     // corresponds to index of CS key
    unsigned long int offset_buffer_;    // number of bytes from beginning of CS key
    unsigned long int number_bytes_;     // number of bytes in buffer
    unsigned long int offset_first_sample_;
    unsigned long int number_filled_bytes_;
    unsigned long int time_offset_;
    unsigned long int add_time_;         // start of trigger time = NT + add_time
    bool user_info_;
    bool new_event_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"#buffers:"<<number_buffers_<<"\n"
        <<std::setw(width)<<std::left<<"bytes user info:"<<bytes_userinfo_<<"\n"
        <<std::setw(width)<<std::left<<"buffer reference:"<<buffer_reference_<<"\n"
        <<std::setw(width)<<std::left<<"sample index:"<<sample_index_<<"\n"
        <<std::setw(width)<<std::left<<"offset buffer:"<<offset_buffer_<<"\n"
        <<std::setw(width)<<std::left<<"buffer size:"<<number_bytes_<<"\n"
        <<std::setw(width)<<std::left<<"offset sample:"<<offset_first_sample_<<"\n"
        <<std::setw(width)<<std::left<<"#filled bytes:"<<number_filled_bytes_<<"\n"
        <<std::setw(width)<<std::left<<"time offset:"<<time_offset_<<"\n"
        <<std::setw(width)<<std::left<<"add time:"<<add_time_<<"\n";
      return ss.str();
    }
  };

  // range of values of component (corresponds to key CR)
  struct range
  {
    bool transform_;         // 1 = true: yes, requires offset + factor, 0 = false: no
    double factor_, offset_; // value = raw value * factor + offset
    bool calibration_;       // 1 = true: calibration, 0 = false: no calibration
    std::string unit_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"transform:"<<(transform_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"factor:"<<factor_<<"\n"
        <<std::setw(width)<<std::left<<"offset:"<<offset_<<"\n"
        <<std::setw(width)<<std::left<<"calibration:"<<(calibration_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"unit:"<<unit_<<"\n";
      return ss.str();
    }
  };

  // channel (corresponds to key CN)
  struct channel
  {
    unsigned long int group_index_;  // corresponds to group-index in CB key
    bool index_bit_;                 // true =  1: digital, false = 0: analog
    std::string name_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"group-index:"<<group_index_<<"\n"
        <<std::setw(width)<<std::left<<"index-bit:"<<index_bit_<<"\n"
        <<std::setw(width)<<std::left<<"name:"<<name_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n";
      return ss.str();
    }
  };

  // rawdata (corresponds to key CS)
  struct data
  {
    unsigned long int index_;  // starting from 1 in first CS block in file
    // std::vector<unsigned char> rawdata_;
    unsigned long int begin_buffer_, end_buffer_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"index:"<<index_<<"\n"
        <<std::setw(width)<<std::left<<"(begin,end) buffer:"
                                     <<"("<<begin_buffer_<<","<<end_buffer_<<")"<<"\n";
      return ss.str();
    }
  };

  // origin of data (corresponds to key NO)
  struct origin_data
  {
    bool origin_;  // corresponds to true = 1 ("verrechnet") and false = 0 ("Original")
    std::string generator_;
    bool comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"origin:"<<(origin_?"verrechnet":"Original")<<"\n"
        <<std::setw(width)<<std::left<<"generator:"<<generator_<<"\n"
        <<std::setw(width)<<std::left<<"comment:"<<comment_<<"\n";
      return ss.str();
    }
  };

  // trigger timestamp (corresponds to key NT)
  struct triggertime
  {
    std::string timestamp_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter> parameters)
    {

    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"timestamp:"<<timestamp_<<"\n";
      return ss.str();
    }
  };

}


namespace imc {

  // create wrapper for imc_object types
  // (not particularly memory-efficient but it simplifies the remaining stuff
  // considerably and the structs are pretty small anyway!)
  class rawobject
  {
    format fmt_;      // 0
    keygroup kyg_;    // 1
    group grp_;       // 2
    text txt_;        // 3
    datafield dtf_;   // 4
    abscissa abs_;    // 5
    component cmt_;   // 6
    packaging pkg_;   // 7
    buffer bfr_;      // 8
    range rng_;       // 9
    channel chn_;     // 10
    data dat_;        // 11
    origin_data org_; // 12
    triggertime trt_; // 13
    int objidx_;

  public:

    rawobject(): objidx_(-1) { }

    void parse(imc::key key, const std::vector<unsigned char>* buffer,
                             const std::vector<parameter>& parameters)
    {
      if ( key.name_ == std::string("CF") )
      {
        fmt_.parse(buffer,parameters);
        objidx_ = 0;
      }
      else if ( key.name_ == std::string("CK") )
      {
        kyg_.parse(buffer,parameters);
        objidx_ = 1;
      }
      else if ( key.name_ == std::string("CB") )
      {
        grp_.parse(buffer,parameters);
        objidx_ = 2;
      }
      else if ( key.name_ == std::string("CT") )
      {
        txt_.parse(buffer,parameters);
        objidx_ = 3;
      }
      else if ( key.name_ == std::string("CG") )
      {
        dtf_.parse(buffer,parameters);
        objidx_ = 4;
      }
      else if ( key.name_ == std::string("CD") && key.version_ == 1 )
      {
        abs_.parse(buffer,parameters);
        objidx_ = 5;
      }
      else if ( key.name_ == std::string("CC") )
      {
        cmt_.parse(buffer,parameters);
        objidx_ = 6;
      }
      else if ( key.name_ == std::string("CP") )
      {
        cmt_.parse(buffer,parameters);
        objidx_ = 7;
      }
      else if ( key.name_ == std::string("Cb") )
      {
        bfr_.parse(buffer,parameters);
        objidx_ = 8;
      }
      else if ( key.name_ == std::string("CR") )
      {
        rng_.parse(buffer,parameters);
        objidx_ = 9;
      }
      else if ( key.name_ == std::string("CN") )
      {
        chn_.parse(buffer,parameters);
        objidx_ = 10;
      }
      else if ( key.name_ == std::string("CS") )
      {
        dat_.parse(buffer,parameters);
        objidx_ = 11;
      }
      else if ( key.name_ == std::string("NO") )
      {
        org_.parse(buffer,parameters);
        objidx_ = 12;
      }
      else if ( key.name_ == std::string("NT") )
      {
        trt_.parse(buffer,parameters);
        objidx_ = 13;
      }
      else
      {
        throw std::logic_error(
          std::string("unsupported block associated to key ") + key.name_
        );
      }
    }

    // provide info string
    std::string get_info(int width = 20)
    {
      switch (objidx_) {
        case 0:
          return fmt_.get_info();
        case 1:
          return kyg_.get_info();
        case 2:
          return grp_.get_info();
        case 3:
          return txt_.get_info();
        case 4:
          return dtf_.get_info();
        case 5:
          return abs_.get_info();
        case 6:
          return cmt_.get_info();
        case 7:
          return pkg_.get_info();
        case 8:
          return bfr_.get_info();
        case 9:
          return rng_.get_info();
        case 10:
          return chn_.get_info();
        case 11:
          return dat_.get_info();
        case 12:
          return org_.get_info();
        case 13:
          return trt_.get_info();
        default:
          return std::string("");
      }
    }

  };

}

#endif

//---------------------------------------------------------------------------//
