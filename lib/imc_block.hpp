//---------------------------------------------------------------------------//

#ifndef IMCBLOCK
#define IMCBLOCK

#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <vector>

//---------------------------------------------------------------------------//

namespace imc
{
  // define "magic bytes" announcing start/end of blocks and separation within
  const unsigned char ch_bgn_ = 0x7c, ch_end_ = 0x3b, ch_sep_ = 0x2c;

  // single parameter (in a block) is determined by offset of its first/last byte
  class parameter
  {
    // offset of first/last byte of parameter
    unsigned long int begin_, end_;

  public:

    parameter(unsigned long int begin, unsigned long int end):
      begin_(begin), end_(end)
    {
      if ( end_ <= begin_ )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
    }

    // set members
    void begin(unsigned long int begin)
    {
      if ( end_ <= begin )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
      begin_ = begin;
    }
    void end(unsigned long int end)
    {
      if ( end <= begin_ )
      {
        throw std::logic_error("parameter: offset of first byte larger than last byte's offset");
      }
      end_ = end;
    }

    // access members
    unsigned long int& begin() { return begin_; }
    unsigned long int& end() { return end_; }

    // comparison operator
    bool operator==(const parameter& param)
    {
      return ( this->begin_ == param.begin_ && this->end_ == param.end_ );
    }

    // get info
    std::string get_info()
    {
      return ( std::string("[") + std::to_string(begin_) + std::string(",")
                                + std::to_string(end_) + std::string("]") );
    }
  };

  // define properties of a raw file block
  class block
  {
    // associated key
    key thekey_;

    // offset (in byte) of first (=ch_bgn_) and last byte (=ch_end_) of block
    // w.r.t. to entire raw file
    unsigned long int begin_, end_;

    // name and buffer of associated raw file
    std::string raw_file_;
    const std::vector<unsigned char>* buffer_;

    // offset of first/last byte of parameters in block (separated by ch_sep_)
    // w.r.t. to first byte of block (=0)
    std::vector<parameter> parameters_;

  public:

    // constructor
    block(key thekey, unsigned long int begin, unsigned long int end,
                      std::string raw_file, std::vector<unsigned char>* buffer):
      thekey_(thekey)
    {
      if ( keys.count(thekey.name_) != 1 ) throw std::logic_error("unknown key");
      begin_ = begin;
      end_ = end;
      if ( end_ <= begin_ )
      {
        throw std::logic_error("block: offset of first byte larger than last byte's offset");
      }
      raw_file_ = raw_file;
      buffer_ = buffer;

      parse_parameters();
    }

    // identify/parse parameters in block
    void parse_parameters()
    {
      // parse entire block and check for separator tokens
      for ( unsigned long int b = begin_; b < end_; b++ )
      {
        if ( buffer_->at(b) == imc::ch_sep_ )
        {
          // define range of parameter with first byte = ch_sep_
          parameters_.push_back(imc::parameter(b,b+1));
        }
      }

      // set offset of parameters's last byte
      for ( unsigned long int p = 0; p < parameters_.size()-1; p++ )
      {
        parameters_[p].end( parameters_[p+1].begin() - 1 );
      }
      // parameters_.back().end(this->end_-1);
    }

    // access members
    imc::key& get_key() { return thekey_; }
    unsigned long int& get_begin() { return begin_; }
    unsigned long int& get_end() { return end_; }

    // get parameter
    std::vector<unsigned char> extract_parameter(parameter& param)
    {
      // check parameter w.r.t. to block
      if ( param.begin() < begin_ || param.end() > end_ )
      {
        throw std::logic_error("inconsistent parameter offsets");
      }
      std::vector<unsigned char> parambuff(buffer_->begin()+begin_+param.begin(),
                                           buffer_->begin()+begin_+param.end());
      return parambuff;
    }

    // get info string
    std::string get_info(int width = 20)
    {
      // summarize parameters in single string
      std::string prsstr("{");
      for ( auto par: parameters_ ) prsstr += par.get_info() + std::string(",");
      prsstr.pop_back();
      prsstr += std::string("}");

      // construct block info string
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"block-key:"<<thekey_.name_<<"\n"
        <<std::setw(width)<<std::left<<"begin:"<<begin_<<"\n"
        <<std::setw(width)<<std::left<<"end:"<<end_<<"\n"
        <<std::setw(width)<<std::left<<"rawfile:"<<raw_file_<<"\n"
        <<std::setw(width)<<std::left<<"buffersize:"<<buffer_->size()<<"\n"
        <<std::setw(width)<<std::left<<"parameters:"<<prsstr<<"\n";
      return ss.str();
    }

  };

}

#endif

//---------------------------------------------------------------------------//