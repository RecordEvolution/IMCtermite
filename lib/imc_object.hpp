//---------------------------------------------------------------------------//

#ifndef IMCOBJECT
#define IMCOBJECT

#include <time.h>
#include "imc_key.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  // obtain specific parameters as string
  std::string get_parameter(const std::vector<unsigned char>* buffer, const imc::parameter* param)
  {
    std::string prm("");
    for ( unsigned long int i = param->begin()+1; i <= param->end(); i++ )
    {
      prm.push_back((char)(*buffer)[i]);
    }
    return prm;
  }

  // format and processor (corresponds to key CF)
  struct format
  {
    int fileformat_;
    int processor_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 3 ) throw std::runtime_error("invalid number of parameters in CF");
      fileformat_ = std::stoi(get_parameter(buffer,&parameters[0]));
      processor_ = std::stoi(get_parameter(buffer,&parameters[2]));
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 2 ) throw std::runtime_error("invalid number of parameters in CK");
      version_ = std::stoi(get_parameter(buffer,&parameters[0]));
      length_ = std::stoi(get_parameter(buffer,&parameters[1]));
      closed_ = ( get_parameter(buffer,&parameters[3])==std::string("1") );
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
  struct groupobj
  {
    unsigned long int group_index_;
    std::string name_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 7 ) throw std::runtime_error("invalid number of parameters in CB");
      group_index_ = std::stoul(get_parameter(buffer,&parameters[2]));
      name_ = get_parameter(buffer,&parameters[4]);
      comment_ = get_parameter(buffer,&parameters[6]);
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 9 ) throw std::runtime_error("invalid number of parameters in CT");
      group_index_ = std::stoul(get_parameter(buffer,&parameters[2]));
      name_ = get_parameter(buffer,&parameters[4]);
      text_ = get_parameter(buffer,&parameters[6]);
      comment_ = get_parameter(buffer,&parameters[8]);
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 5 ) throw std::runtime_error("invalid number of parameters in CG");
      number_components_ = std::stoul(get_parameter(buffer,&parameters[2]));
      fldtype_ = (fieldtype)std::stoi(get_parameter(buffer,&parameters[3]));
      dimension_ = std::stoi(get_parameter(buffer,&parameters[4]));
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 6 ) throw std::runtime_error("invalid number of parameters in CD1");
      dx_ = std::stod(get_parameter(buffer,&parameters[2]));
      calibration_ = ( get_parameter(buffer,&parameters[3]) == std::string("1") );
      unit_ = get_parameter(buffer,&parameters[5]);
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"dx:"<<dx_<<"\n"
        <<std::setw(width)<<std::left<<"calibration:"<<(calibration_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"unit:"<<unit_<<"\n";
      return ss.str();
    }
  };

  // definition of abscissa (corresponds to key CD2)
  struct abscissa2
  {
    double dx_;
    bool calibration_;
    std::string unit_;
    bool reduction_;
    bool ismultievent_;
    bool sortbuffer_;
    double x0_;
    int pretriggerapp_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 11 ) throw std::runtime_error("invalid number of parameters in CD2");
      dx_ = std::stod(get_parameter(buffer,&parameters[2]));
      calibration_ = ( get_parameter(buffer,&parameters[3]) == std::string("1") );
      unit_ = get_parameter(buffer,&parameters[5]);
      reduction_ = ( get_parameter(buffer,&parameters[6]) == std::string("1") );
      ismultievent_ = ( get_parameter(buffer,&parameters[7]) == std::string("1") );
      sortbuffer_ = ( get_parameter(buffer,&parameters[8]) == std::string("1") );
      x0_ = std::stod(get_parameter(buffer,&parameters[9]));
      pretriggerapp_ = std::stoi( get_parameter(buffer,&parameters[10]) );
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"dx:"<<dx_<<"\n"
        <<std::setw(width)<<std::left<<"calibration:"<<(calibration_?"yes":"no")<<"\n"
        <<std::setw(width)<<std::left<<"unit:"<<unit_<<"\n"
        <<std::setw(width)<<std::left<<"reduction:"<<reduction_<<"\n"
        <<std::setw(width)<<std::left<<"ismultievent:"<<ismultievent_<<"\n"
        <<std::setw(width)<<std::left<<"sortbuffer:"<<sortbuffer_<<"\n"
        <<std::setw(width)<<std::left<<"x0:"<<x0_<<"\n"
        <<std::setw(width)<<std::left<<"pretriggerapp:"<<pretriggerapp_<<"\n";
      return ss.str();
    }
  };

  // start of component (corresponds to key CC)
  struct component
  {
    int component_index_;
    bool analog_digital_; // 1 => false (analog), 2 => true (digital)

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 4 ) throw std::runtime_error("invalid number of parameters in CD2");
      component_index_ = std::stoi(get_parameter(buffer,&parameters[2]));
      analog_digital_ = ( std::stoi(get_parameter(buffer,&parameters[3])) == 2 );
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"index:"<<component_index_<<"\n"
        <<std::setw(width)<<std::left<<"analog/digital:"<<(analog_digital_?"digital":"analog")<<"\n";
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 10 ) throw std::runtime_error("invalid number of parameters in CP");
      buffer_reference_ = std::stoi(get_parameter(buffer,&parameters[2]));
      numeric_type_ = (numtype)std::stoi(get_parameter(buffer,&parameters[3]));
      bytes_ = std::stoi(get_parameter(buffer,&parameters[4]));
      signbits_ = std::stoi(get_parameter(buffer,&parameters[5]));
      mask_ = std::stoi(get_parameter(buffer,&parameters[6]));
      offset_ = std::stoul(get_parameter(buffer,&parameters[7]));
      number_subsequent_samples_ = std::stoul(get_parameter(buffer,&parameters[8]));
      distance_bytes_ = std::stoul(get_parameter(buffer,&parameters[9]));
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
        <<std::setw(width)<<std::left<<"#subseq.-samples:"<<number_subsequent_samples_<<"\n"
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
    double x0_;
    double add_time_;                    // start of trigger time = NT + add_time
    // bool user_info_;
    // bool new_event_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 13 ) throw std::runtime_error("invalid number of parameters in CD2");
      number_buffers_ = std::stoul(get_parameter(buffer,&parameters[2]));
      bytes_userinfo_ = std::stoul(get_parameter(buffer,&parameters[3]));
      buffer_reference_ = std::stoul(get_parameter(buffer,&parameters[4]));
      sample_index_ = std::stoul(get_parameter(buffer,&parameters[5]));
      offset_buffer_ = std::stoul(get_parameter(buffer,&parameters[6]));
      number_bytes_ = std::stoul(get_parameter(buffer,&parameters[7]));
      offset_first_sample_ = std::stoul(get_parameter(buffer,&parameters[8]));
      number_filled_bytes_ = std::stoul(get_parameter(buffer,&parameters[9]));
      x0_ = std::stod(get_parameter(buffer,&parameters[11]));
      add_time_ = std::stod(get_parameter(buffer,&parameters[12]));
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
        <<std::setw(width)<<std::left<<"time offset:"<<x0_<<"\n"
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
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 8 ) throw std::runtime_error("invalid number of parameters in CD2");
      transform_ = (get_parameter(buffer,&parameters[2]) == std::string("1"));
      factor_ = std::stod(get_parameter(buffer,&parameters[3]));
      offset_ = std::stod(get_parameter(buffer,&parameters[4]));
      calibration_ = (get_parameter(buffer,&parameters[5]) == std::string("1"));
      unit_ = get_parameter(buffer,&parameters[7]);
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
  struct channelobj
  {
    unsigned long int group_index_;  // corresponds to group-index in CB key
    bool index_bit_;                 // true =  1: digital, false = 0: analog
    std::string name_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 9 ) throw std::runtime_error("invalid number of parameters in CD2");
      group_index_ = std::stoul(get_parameter(buffer,&parameters[2]));
      index_bit_ = (get_parameter(buffer,&parameters[4]) == std::string("1"));
      name_ = get_parameter(buffer,&parameters[6]);
      comment_ = get_parameter(buffer,&parameters[8]);
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
    // unsigned long int begin_buffer_, end_buffer_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 4 ) throw std::runtime_error("invalid number of parameters in CD2");
      index_ = std::stoul(get_parameter(buffer,&parameters[2]));
    }

    // get info string
    std::string get_info(int width = 20)
    {
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"index:"<<index_<<"\n";
        // <<std::setw(width)<<std::left<<"(begin,end) buffer:"
        //                              <<"("<<begin_buffer_<<","<<end_buffer_<<")"<<"\n";
      return ss.str();
    }
  };

  // origin of data (corresponds to key NO)
  struct origin_data
  {
    bool origin_;  // corresponds to true = 1 ("verrechnet") and false = 0 ("Original")
    std::string generator_;
    std::string comment_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 7 ) throw std::runtime_error("invalid number of parameters in CD2");
      origin_ = ( get_parameter(buffer,&parameters[2]) == std::string("1") );
      generator_ = get_parameter(buffer,&parameters[4]);
      comment_ = get_parameter(buffer,&parameters[6]);
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

  // trigger timestamp (corresponds to key NT1)
  struct triggertime
  {
    int day_, month_, year_;
    int hour_, minute_;
    double second_;
    std::string timestamp_;

    // construct members by parsing particular parameters from buffer
    void parse(const std::vector<unsigned char>* buffer, const std::vector<parameter>& parameters)
    {
      if ( parameters.size() < 8 ) throw std::runtime_error("invalid number of parameters in CD2");
      day_ = std::stoi( get_parameter(buffer,&parameters[2]) );
      month_ = std::stoi( get_parameter(buffer,&parameters[3]) );
      year_ = std::stoi( get_parameter(buffer,&parameters[4]) );
      hour_ = std::stoi( get_parameter(buffer,&parameters[5]) );
      minute_ = std::stoi( get_parameter(buffer,&parameters[6]) );
      second_ = std::stod( get_parameter(buffer,&parameters[7]) );

      //time_t rawtime;
      //struct tm ts;
      //time(&rawtime);
      //localtime_r(&rawtime,&ts);
      //ts.tm_mday = day_;
      //ts.tm_mon = month_-1;
      //ts.tm_year = year_-1900;
      //ts.tm_hour = hour_;
      //ts.tm_min = minute_;
      //ts.tm_sec = (int)second_;
      //asctime_r(&ts,&timestamp_[0]);
      timestamp_ = std::to_string(year_) + std::string("-") + std::to_string(month_)
                                         + std::string("-") + std::to_string(day_)
                                         + std::string("T") + std::to_string(hour_)
                                         + std::string(":") + std::to_string(minute_)
                                         + std::string(":") + std::to_string(second_);
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
    groupobj grp_;       // 2
    text txt_;        // 3
    datafield dtf_;   // 4
    abscissa abs_;    // 5
    component cmt_;   // 6
    packaging pkg_;   // 7
    buffer bfr_;      // 8
    range rng_;       // 9
    channelobj chn_;     // 10
    data dat_;        // 11
    origin_data org_; // 12
    triggertime trt_; // 13
    abscissa2 abs2_;  // 14
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
        pkg_.parse(buffer,parameters);
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
      else if ( key.name_ == std::string("NT") && key.version_ == 1 )
      {
        trt_.parse(buffer,parameters);
        objidx_ = 13;
      }
      else if ( key.name_ == std::string("CD") && key.version_ == 2 )
      {
        abs2_.parse(buffer,parameters);
        objidx_ = 14;
      }
      else
      {
        if ( key.name_.at(0) == 'C' )
        {
          throw std::logic_error(
            std::string("unsupported block associated to critical key ")
            + key.name_ + std::to_string(key.version_)
          );
        }
        else
        {
          std::cout<<"WARNING: unsupported block associated to noncritical key "
                   <<key.name_<<key.version_<<"\n";
        }
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
        case 14:
          return abs2_.get_info();
        default:
          return std::string("");
      }
    }

  };

}

#endif

//---------------------------------------------------------------------------//
