//---------------------------------------------------------------------------//

#ifndef IMCBLOCK
#define IMCBLOCK

#include <iomanip>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include "imc_parameter.hpp"
#include "imc_object.hpp"

//---------------------------------------------------------------------------//

namespace imc
{
  // define "magic bytes" announcing start/end of blocks and separation of parameters within
  const unsigned char ch_bgn_ = 0x7c, ch_end_ = 0x3b, ch_sep_ = 0x2c;

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
    std::vector<imc::parameter> parameters_;

    // particular imc object represented by this block
    imc::rawobject imc_object_;

  public:

    // constructor
    block(key thekey, unsigned long int begin, unsigned long int end,
                      std::string raw_file, const std::vector<unsigned char>* buffer):
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

      // make sure "end_" does not exceed buffer size due to invalid "length" parameter of block
      if ( end_ > buffer_->size() )
      {
        std::cout<<"WARNING: invalid length parameter in "<<thekey_.name_<<"-block "
                 <<"(block-end:"<<end_<<",buffer-size:"<<buffer_->size()<<")"
                 <<" => resetting block-end to buffer-size\n";
        end_ = buffer_->size();
      }

      try {
        parse_parameters();
        parse_object();
      } catch (const std::exception& e) {
        throw std::runtime_error(
          std::string("block: failed to parse parameters/objects: ") + e.what()
        );
      }
    }

  private:

    // identify/parse parameters in block
    void parse_parameters()
    {
      // parse entire block and check for separator tokens
      for ( unsigned long int b = begin_; b < end_; b++ )
      {
        if ( buffer_->at(b) == imc::ch_sep_ )
        {
          // define range of parameter with first byte = ch_sep_
          parameters_.push_back(imc::parameter(b,b));
        }
      }

      // set offset of parameters's last byte
      for ( unsigned long int p = 0; p < parameters_.size()-1; p++ )
      {
        parameters_[p].end( parameters_[p+1].begin() - 1 );
      }
      parameters_.back().end( this->end_ - 1 );
    }

    // pass buffer and parameters associated to block to generate corres. object
    void parse_object()
    {
      try {
        imc_object_.parse(thekey_,buffer_,parameters_);
      } catch (const std::exception& e) {
        throw std::runtime_error(
          std::string("failed to parse imc::object for key ")
          + thekey_.name_ + std::string(": ") + e.what()
        );
      }
    }

  public:

    // access members
    imc::key get_key() { return thekey_; }
    unsigned long int get_begin() { return begin_; }
    unsigned long int get_end() { return end_; }

    // get list of parameters
    std::vector<parameter>& get_parameters()
    {
      return parameters_;
    }

    // get data of single parameter
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
    std::string get_info(bool include_object = true, int width = 20)
    {
      // summarize parameters in single string
      std::string prsstr("{");
      for ( auto par: parameters_ ) prsstr += par.get_info() + std::string(",");
      if ( prsstr.size() > 1 ) prsstr.pop_back();
      prsstr += std::string("}");

      // construct block info string
      std::stringstream ss;
      ss<<std::setw(width)<<std::left<<"block:"<<thekey_.name_
                                     <<" version "<<thekey_.version_
                                     <<" ("<<thekey_.description_<<")"<<"\n"
        <<std::setw(width)<<std::left<<"begin:"<<begin_<<"\n"
        <<std::setw(width)<<std::left<<"end:"<<end_<<"\n"
        <<std::setw(width)<<std::left<<"rawfile:"<<raw_file_<<"\n"
        <<std::setw(width)<<std::left<<"buffersize:"<<buffer_->size()<<"\n"
        <<std::setw(width)<<std::left<<"parameters:"<<prsstr<<"\n";

      // include meta data of specific object
      if ( include_object )
      {
        ss<<std::setfill('-')<<std::left<<std::setw(60)<<""<<std::setfill(' ')<<"\n";
        // ss<<thekey_.description_<<"\n";
        ss<<imc_object_.get_info()<<"\n";
      }

      return ss.str();
    }

  };

}

#endif

//---------------------------------------------------------------------------//
